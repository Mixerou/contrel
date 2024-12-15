//
// Created by Mixerou on 15.12.2024.
//

#include "app.h"
#include "backend.h"

namespace backend {
BackendRequest Ping() {
  BackendRequest request(app::api_worker.Enqueue("/ping"), Layer::kApi);
  return request;
}
}  // namespace backend
