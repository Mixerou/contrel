//
// Created by Mixerou on 15.12.2024.
//

#include <format>
#include <mutex>
#include <thread>

#include <ixwebsocket/IXHttpClient.h>

#include "constants.h"
#include "workers.h"

using namespace constants;

namespace workers {
void ApiWorker::Loop() {
  while (true) {
    ApiRequest request;

    ix::HttpClient http_client;
    auto args = http_client.createRequest();
    args->compress = false;

    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      request_condition_.wait(
          lock, [this] { return need_stop_ || !requests_.empty(); });

      if (need_stop_) return;

      request = requests_.front();
      requests_.pop();
    }

    auto response =
        http_client.request(format("{}{}", kBackendApiUrl, request.path),
                            request.method, request.body, args);

    {
      std::lock_guard<std::mutex> lock(response_mutex_);
      responses_.emplace(request.id, response);
    }
  }
}

ApiWorker::ApiWorker() : need_stop_(false), next_request_id_(1) {
  worker_ = std::thread(&ApiWorker::Loop, this);
}

ApiWorker::~ApiWorker() {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    need_stop_ = true;
  }

  request_condition_.notify_one();
  worker_.join();
}

int ApiWorker::Enqueue(std::string path, std::string method, std::string body) {
  int request_id = next_request_id_++;

  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    requests_.push({request_id, path, method, body});
  }

  request_condition_.notify_one();

  return request_id;
}

std::optional<std::shared_ptr<ix::HttpResponse>> ApiWorker::GetResponse(
    int request_id) {
  std::lock_guard<std::mutex> lock(response_mutex_);
  auto found = responses_.find(request_id);

  if (found == responses_.end()) return std::nullopt;

  auto response = found->second;
  responses_.erase(found);

  return response;
}
}  // namespace workers
