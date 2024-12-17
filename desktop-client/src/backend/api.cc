//
// Created by Mixerou on 15.12.2024.
//

#include <ixwebsocket/IXHttpClient.h>

#include "app.h"
#include "backend.h"

namespace backend {
BackendRequest Ping() {
  BackendRequest request(app::api_worker.Enqueue("/ping"), Layer::kApi);
  return request;
}

BackendRequest Login(LoginRequestPayload payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/auth/login", ix::HttpClient::kPost,
                              buffer.data()),
      Layer::kApi);

  return request;
}

BackendRequest Register(RegisterRequestPayload payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/auth/register", ix::HttpClient::kPost,
                              buffer.data()),
      Layer::kApi);

  return request;
}
}  // namespace backend
