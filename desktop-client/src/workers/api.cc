//
// Created by Mixerou on 15.12.2024.
//

#include <format>
#include <mutex>
#include <thread>

#include <ixwebsocket/IXHttpClient.h>

#include "app.h"
#include "constants.h"
#include "workers.h"

using namespace constants;

namespace workers {
void ApiWorker::Loop() {
  while (true) {
    ApiRequest request;
    ix::HttpClient http_client;
    ix::WebSocketHttpHeaders headers;
    const auto args = http_client.createRequest();

    headers.emplace("authorization", app::states::system.GetSessionToken());

    args->extraHeaders = headers;
    args->compress = false;
    args->followRedirects = false;

    {
      std::unique_lock lock(queue_mutex_);
      request_condition_.wait(
          lock, [this] { return need_stop_ || !requests_.empty(); });

      if (need_stop_) return;

      request = requests_.front();
      requests_.pop();
    }

    {
      auto response =
          http_client.request(format("{}{}", kBackendApiUrl, request.path),
                              request.method, request.body, args);
      std::lock_guard lock(response_mutex_);
      responses_.emplace(request.id, response);
    }
  }
}

ApiWorker::ApiWorker() : need_stop_(false), next_request_id_(0) {
  worker_ = std::thread(&ApiWorker::Loop, this);
}

ApiWorker::~ApiWorker() {
  {
    std::lock_guard lock(queue_mutex_);
    need_stop_ = true;
  }

  request_condition_.notify_one();
  worker_.join();
}

auto ApiWorker::Enqueue(const std::string &path, const std::string &method,
                        const std::string &body) -> int {
  const int request_id = next_request_id_++;

  {
    std::lock_guard lock(queue_mutex_);
    requests_.push({request_id, path, method, body});
  }

  request_condition_.notify_one();

  return request_id;
}

std::optional<std::shared_ptr<ix::HttpResponse>> ApiWorker::GetResponse(
    const int request_id) {
  std::shared_ptr<ix::HttpResponse> response;
  {
    std::lock_guard lock(response_mutex_);
    const auto found = responses_.find(request_id);

    if (found == responses_.end()) return std::nullopt;

    response = found->second;
    responses_.erase(found);
  }

  // Possible delay between setting a new token in the system state
  // and sending a new request with this new token

  if (const auto response_session_token =
          response->headers["x-set-session-token"];
      response_session_token != "")
    app::states::system.SetSessionToken(response_session_token);

  return response;
}
}  // namespace workers
