//
// Created by Mixerou on 18.12.2024.
//

#include <format>
#include <mutex>
#include <optional>
#include <thread>

#include <ixwebsocket/IXWebSocket.h>

#include "backend.h"
#include "constants.h"
#include "workers.h"

using namespace constants;

namespace workers {
void WebSocketWorker::Loop() {
  while (true) {
    std::string message;
    {
      std::unique_lock<std::mutex> requests_lock(requests_mutex_);
      request_condition_.wait(
          requests_lock, [this] { return need_stop_ || !requests_.empty(); });

      if (need_stop_) break;

      message = requests_.front();
      requests_.pop();
    }

    web_socket_->sendBinary(message);
  }

  ReallyStop();
}

WebSocketWorker::WebSocketWorker() : is_ready_(false), need_stop_(true) {}

WebSocketWorker::~WebSocketWorker() { Stop(); }

void WebSocketWorker::Start() {
  std::lock_guard<std::mutex> requests_lock(requests_mutex_);

  if (web_socket_ == nullptr) web_socket_ = new ix::WebSocket;
  web_socket_->setUrl(std::string(kBackendWebSocketUrl));
  web_socket_->disablePerMessageDeflate();
  web_socket_->disableAutomaticReconnection();
  web_socket_->disablePong();
  web_socket_->disablePong();

  web_socket_->setOnMessageCallback(
      [this](const ix::WebSocketMessagePtr& message) {
        printf("MESSAGE: %d\n", message->type);
        if (message->type == ix::WebSocketMessageType::Open) {
          std::lock_guard<std::mutex> lock(web_socket_mutex_);

          backend::WebSocketAuthorizationPayload payload(
              app::states::system.GetSessionToken());
          backend::WebSocketMessage message(
              next_message_id_++, backend::WebSocketOpcode::kAuthorize,
              payload);

          msgpack::sbuffer buffer;
          msgpack::pack(buffer, message);

          web_socket_->sendBinary(std::string(buffer.data(), buffer.size()));
        } else if (message->type == ix::WebSocketMessageType::Message) {
          std::lock_guard<std::mutex> is_ready_lock(is_ready_mutex_);
          std::lock_guard<std::mutex> response_lock(response_mutex_);

          if (message->str.size() != 0) {
            msgpack::object_handle object_handle;
            msgpack::unpack(object_handle, message->str.c_str(),
                            message->str.size());
            msgpack::object object(object_handle.get());

            if (!is_ready_) {
              is_ready_ = true;
              app::states::system.SetWebSocketWorkerReadyState(is_ready_);
            }

            if (object.type == msgpack::type::ARRAY) {
              auto array = object.via.array.ptr;

              if (array->type == msgpack::type::POSITIVE_INTEGER ||
                  array->type == msgpack::type::NEGATIVE_INTEGER) {
                responses_.emplace(array->via.i64, message->str);
                return;
              }
            }
          }
        } else if (message->type == ix::WebSocketMessageType::Close ||
                   message->type == ix::WebSocketMessageType::Error) {
          std::lock_guard<std::mutex> is_ready_lock(is_ready_mutex_);

          if (!is_ready_) Stop();
        }
      });

  responses_.clear();
  need_stop_ = false;
  next_message_id_ = 0;

  web_socket_->start();
  worker_ = std::thread(&WebSocketWorker::Loop, this);
}

void WebSocketWorker::ReallyStop() {
  std::lock_guard<std::mutex> is_ready_lock(is_ready_mutex_);

  web_socket_->stop();

  is_ready_ = false;

  app::states::system.SetWebSocketWorkerReadyState(is_ready_);
}

void WebSocketWorker::Stop() {
  {
    std::lock_guard<std::mutex> requests_lock(requests_mutex_);
    need_stop_ = true;
  }

  request_condition_.notify_one();
}

bool WebSocketWorker::IsStarted() { return !need_stop_; }

void WebSocketWorker::Enqueue(const std::string& message) {
  //  std::lock_guard<std::mutex> lock(requests_mutex_);
  //  requests_.push(message);
  //  request_condition_.notify_one();
}

std::optional<std::string> WebSocketWorker::GetResponse(int message_id) {
  //  std::lock_guard<std::mutex> lock(response_mutex_);
  //  auto found = responses_.find(message_id);
  //
  //  if (found != responses_.end()) {
  //    std::string response = found->second;
  //    responses_.erase(found);
  //
  //    return response;
  //  }

  return std::nullopt;
}
}  // namespace workers
