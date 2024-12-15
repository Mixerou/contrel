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

struct BackendRequest {
  int worker_request_id;
  Layer layer;
};

enum class ResponseStatus {
  kInProcess,
  kCompleted,
  kError,
};

typedef std::string ping_response_t;

BackendRequest Ping();

// true, if the response is ready
template <typename T>
ResponseStatus GetResponse(BackendRequest &request, T &response_reference) {
  switch (request.layer) {
    case Layer::kApi: {
      auto response = app::api_worker.GetResponse(request.worker_request_id);

      if (!response.has_value()) return ResponseStatus::kInProcess;

      auto unwrapped_response = response->get();

      if (unwrapped_response->errorCode != ix::HttpErrorCode::Ok)
        return ResponseStatus::kError;

      auto body = unwrapped_response->body;
      auto object = msgpack::unpack(body.c_str(), body.size()).get();

      // TODO: try ... catch
      object.convert(response_reference);

      return ResponseStatus::kCompleted;
    }
    default:
      // theoretically unreachable code
      return ResponseStatus::kError;
  }
}
}  // namespace backend

#endif  // DESKTOP_CLIENT_BACKEND_H
