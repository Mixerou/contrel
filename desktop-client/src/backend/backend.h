//
// Created by Mixerou on 15.12.2024.
// This is the backend transport layer,
// not the backend itself.
//

#ifndef DESKTOP_CLIENT_BACKEND_H
#define DESKTOP_CLIENT_BACKEND_H

#include <iostream>

#include <msgpack.hpp>

#include "app.h"

namespace backend {
enum class Layer {
  kApi,
};

struct ErrorResponse {
  int code;
  std::string message;

  ErrorResponse() {}
  MSGPACK_DEFINE(code, message);
};

struct BackendRequest {
  int worker_request_id;
  Layer layer;
  ErrorResponse error_response;

  BackendRequest() {}

  BackendRequest(int worker_request_id, Layer layer)
      : worker_request_id(worker_request_id), layer(layer) {}
};

enum class ResponseStatus {
  kInProcess,
  kCompleted,
  kError,
  kCompetedWithError,
};

// In MessagePack this is a `90`
struct EmptyResponse {
  EmptyResponse() {}
  MSGPACK_DEFINE();
};

enum class WebSocketOpcode : uint8_t {
  kHeartBeat = 0,
  kRequest = 1,
  kResponse = 2,
  kError = 3,
  kAuthorize = 4,
};

template <typename T>
struct WebSocketMessage {
  int id;
  int opcode;
  T payload;

  WebSocketMessage(int id, WebSocketOpcode opcode, T payload)
      : id(id), opcode(static_cast<int>(opcode)), payload(payload) {}
  WebSocketMessage(int id, T payload)
      : id(id),
        opcode(static_cast<int>(WebSocketOpcode::kRequest)),
        payload(payload) {}

  MSGPACK_DEFINE(id, opcode, payload);
};

// Ping
typedef std::string ping_response_t;

BackendRequest Ping();

// Login
struct LoginRequestPayload {
  std::string email;
  std::string password;

  MSGPACK_DEFINE(email, password);
};

BackendRequest Login(LoginRequestPayload payload);

// Register
struct RegisterRequestPayload {
  std::string email;
  std::string password;
  std::string first_name;
  std::string last_name;

  MSGPACK_DEFINE(email, password, first_name, last_name);
};

BackendRequest Register(RegisterRequestPayload payload);

// Logout
BackendRequest Logout();

// WS Authorise
struct WebSocketAuthorizationPayload {
  int payload_type;
  std::string token;

  WebSocketAuthorizationPayload(std::string token)
      : payload_type(300), token(token) {}

  MSGPACK_DEFINE(payload_type, token);
};

template <typename T>
ResponseStatus GetResponse(BackendRequest &request, T &response_reference) {
  switch (request.layer) {
    case Layer::kApi: {
      auto response = app::api_worker.GetResponse(request.worker_request_id);

      if (!response.has_value()) return ResponseStatus::kInProcess;

      auto unwrapped_response = response->get();
      auto body = unwrapped_response->body;

      if (unwrapped_response->errorCode != ix::HttpErrorCode::Ok)
        return ResponseStatus::kError;

      if (body.empty()) return ResponseStatus::kCompleted;

      auto object = msgpack::unpack(body.c_str(), body.size()).get();

      // TODO: catch msgpack::type_error
      try {
        object.convert(request.error_response);
        return ResponseStatus::kCompetedWithError;
      } catch (...) {
        object.convert(response_reference);
        return ResponseStatus::kCompleted;
      }

      //      catch (...) {
      //        return ResponseStatus::kError;
      //      }
    }
    default:
      // theoretically unreachable code
      return ResponseStatus::kError;
  }
}
}  // namespace backend

#endif  // DESKTOP_CLIENT_BACKEND_H
