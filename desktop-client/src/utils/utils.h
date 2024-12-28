//
// Created by Mixerou on 17.12.2024.
//

#ifndef DESKTOP_CLIENT_UTILS_H
#define DESKTOP_CLIENT_UTILS_H

#include <string>

#include <glad/gl.h>
#include <imgui.h>

namespace utils {
class Texture {
  ImVec2 size_;
  ImVec2 original_size_;
  GLuint texture_;
  const char *file_name_;
  bool is_loaded_ = false;
  bool is_vector_ = false;

  void Load(const unsigned char *data, const int &width, const int &height);

  void LoadVectorFromFile(const char *file_name, const ImVec2 &size);

  void LoadRasterFromFile(const char *file_name);

 public:
  explicit Texture(const char *file_name, const ImVec2 &size = ImVec2());

  ~Texture();

  void LoadAnother(const char *file_name, const ImVec2 &size = ImVec2());

  void Unload();

  void CheckSize(const ImVec2 &expected_size);

  ImVec2 GetSize() const;

  GLuint GetTexture() const;

  const char *GetFileName() const;

  bool IsLoaded() const;

  void SetSize(const ImVec2 &new_size, const bool &reload_if_needed = true);
};

bool StringEndsWith(std::string const &str, std::string const &ending);

std::string FormatUnixTimestampToHumanReadable(int64_t timestamp);

int64_t ConvertHumanReadableTimestampToUnix(const std::string &date);

int64_t GetCurrentUnixTimestamp();
}  // namespace utils

#endif
