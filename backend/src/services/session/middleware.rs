use std::future::{ready, Ready};
use std::rc::Rc;

use actix_web::dev::{forward_ready, Service, ServiceRequest, ServiceResponse, Transform};
use actix_web::http::header;
use actix_web::http::header::{HeaderName, HeaderValue};
use actix_web::{Error, HttpMessage};
use futures_util::future::LocalBoxFuture;

use crate::constants::HTTP_CODE_NOT_FOUND;
use crate::error::BackendError;
use crate::services::session::Session;

pub struct CheckSession;

impl<S, B> Transform<S, ServiceRequest> for CheckSession
where
    S: Service<ServiceRequest, Response = ServiceResponse<B>, Error = Error> + 'static,
    S::Future: 'static,
    B: 'static,
{
    type Response = ServiceResponse<B>;
    type Error = Error;
    type Transform = CheckSessionMiddleware<S>;
    type InitError = ();
    type Future = Ready<Result<Self::Transform, Self::InitError>>;

    fn new_transform(&self, service: S) -> Self::Future {
        ready(Ok(CheckSessionMiddleware {
            service: Rc::new(service),
        }))
    }
}

pub struct CheckSessionMiddleware<S> {
    service: Rc<S>,
}

impl<S, B> Service<ServiceRequest> for CheckSessionMiddleware<S>
where
    S: Service<ServiceRequest, Response = ServiceResponse<B>, Error = Error> + 'static,
    S::Future: 'static,
    B: 'static,
{
    type Response = ServiceResponse<B>;
    type Error = Error;
    type Future = LocalBoxFuture<'static, Result<Self::Response, Self::Error>>;

    forward_ready!(service);

    fn call(&self, request: ServiceRequest) -> Self::Future {
        let mut is_session_created = false;
        let service = self.service.clone();

        let token = match request
            .headers()
            .get(header::AUTHORIZATION)
            .unwrap_or(&HeaderValue::from_static(""))
            .to_str()
        {
            Ok(token) => token.to_string(),
            Err(error) => return Box::pin(async { Err(Error::from(BackendError::from(error))) }),
        };

        Box::pin(async move {
            let session = match Session::find_by_token(&token).await {
                Ok(session) => session,
                Err(error) if error.http_code == HTTP_CODE_NOT_FOUND => {
                    match Session::create().await {
                        Ok(session) => {
                            is_session_created = true;
                            session
                        }
                        Err(error) => return Err(Error::from(error)),
                    }
                }
                Err(error) => return Err(Error::from(error)),
            };

            request.extensions_mut().insert(session.clone());

            let response = service.call(request);

            let mut response = response.await?;

            if is_session_created {
                let value = match HeaderValue::from_str(&session.token) {
                    Ok(value) => value,
                    Err(error) => return Err(Error::from(error)),
                };

                response
                    .headers_mut()
                    .insert(HeaderName::from_static("x-set-session-token"), value);
            }

            Ok(response)
        })
    }
}
