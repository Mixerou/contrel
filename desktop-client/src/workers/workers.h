//
// Created by Mixerou on 15.12.2024.
//

#ifndef DESKTOP_CLIENT_WORKERS_H
#define DESKTOP_CLIENT_WORKERS_H

#include <string>
#include <thread>
#include <unordered_map>

#include <ixwebsocket/IXHttpClient.h>
#include <ixwebsocket/IXWebSocket.h>

namespace workers {
struct ApiRequest {
  int id;
  std::string path;
  std::string method;
  std::string body;
};

// This worker only works with the Contrel Backend API
class ApiWorker {
 private:
  std::thread worker_;
  std::queue<ApiRequest> requests_;
  std::unordered_map<int, std::shared_ptr<ix::HttpResponse>> responses_;
  std::mutex queue_mutex_;
  std::mutex response_mutex_;
  std::condition_variable request_condition_;
  std::atomic<bool> need_stop_;
  std::atomic<int> next_request_id_;

  void Loop();

 public:
  ApiWorker();

  ~ApiWorker();

  int Enqueue(std::string path, std::string method = ix::HttpClient::kGet,
              std::string body = "");

  std::optional<std::shared_ptr<ix::HttpResponse>> GetResponse(int request_id);
};

class WebSocketWorker {
 private:
  std::thread worker_;

  ix::WebSocket* web_socket_;
  std::mutex web_socket_mutex_;

  std::queue<std::string> requests_;
  std::mutex requests_mutex_;
  std::condition_variable request_condition_;

  std::unordered_map<int64_t, std::string> responses_;
  std::mutex response_mutex_;

  bool is_ready_;
  std::mutex is_ready_mutex_;

  bool need_stop_;
  std::mutex need_stop_mutex_;

  int next_message_id_;

  void Loop();

  void ReallyStop();

 public:
  WebSocketWorker();

  ~WebSocketWorker();

  void Start();

  void Stop();

  bool IsStarted();

  void Enqueue(const std::string& message);

  std::optional<std::string> GetResponse(int message_id);
};
}  // namespace workers

#endif  // DESKTOP_CLIENT_WORKERS_H
