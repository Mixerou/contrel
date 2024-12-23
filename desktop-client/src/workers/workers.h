//
// Created by Mixerou on 15.12.2024.
//

#ifndef DESKTOP_CLIENT_WORKERS_H
#define DESKTOP_CLIENT_WORKERS_H

#include <string>
#include <thread>
#include <unordered_map>

#include <ixwebsocket/IXHttpClient.h>

namespace workers {
struct ApiRequest {
  int id;
  std::string path;
  std::string method;
  std::string body;
};

// This worker only works with the Contrel Backend API
class ApiWorker {
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

  int Enqueue(const std::string &path,
              const std::string &method = ix::HttpClient::kGet,
              const std::string &body = "");

  std::optional<std::shared_ptr<ix::HttpResponse>> GetResponse(int request_id);
};
}  // namespace workers

#endif  // DESKTOP_CLIENT_WORKERS_H
