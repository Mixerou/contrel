//
// Created by Mixerou on 17.12.2024.
//

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include <string>

#include <glad/gl.h>
#include <imgui.h>
#include <lunasvg.h>
#include <stb_image.h>

#include "constants.h"
#include "utils.h"

using namespace constants;

namespace utils {
void Texture::Load(const unsigned char *data, int &width, int &height) {
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  original_size_ =
      ImVec2(static_cast<float>(width), static_cast<float>(height));
  is_loaded_ = true;
}

void Texture::LoadVectorFromFile(const char *file_name, const ImVec2 &size) {
  is_vector_ = true;

  auto document = lunasvg::Document::loadFromFile(file_name);
  if (!document) return;

  auto bitmap = document->renderToBitmap(
      size.x == 0.0 ? -1 : size.x * kVectorTextureScaleFactor,
      size.y == 0 ? -1 : size.y * kVectorTextureScaleFactor);
  if (bitmap.isNull()) return;

  int width = bitmap.width();
  int height = bitmap.height();
  bitmap.convertToRGBA();

  Load(bitmap.data(), width, height);
}

void Texture::LoadRasterFromFile(const char *file_name) {
  auto *file = fopen(file_name, "rb");
  if (!file) return;

  fseek(file, 0, SEEK_END);

  size_t file_size = (size_t)ftell(file);
  if (file_size == -1) return;

  fseek(file, 0, SEEK_SET);

  void *file_data = IM_ALLOC(file_size);
  fread(file_data, 1, file_size, file);

  int width = 0;
  int height = 0;
  unsigned char *image_data =
      stbi_load_from_memory((const unsigned char *)file_data, (int)file_size,
                            &width, &height, NULL, 4);
  if (!image_data) return;

  Load(image_data, width, height);
  stbi_image_free(image_data);
  IM_FREE(file_data);
}

Texture::Texture(const char *file_name, const ImVec2 &size) {
  LoadAnother(file_name, size);
}

Texture::~Texture() { Unload(); }

void Texture::LoadAnother(const char *file_name, const ImVec2 &size) {
  file_name_ = file_name;

  if (IsLoaded()) Unload();

  if (StringEndsWith(std::string(file_name), ".svg"))
    LoadVectorFromFile(file_name, size);
  else
    LoadRasterFromFile(file_name);

  SetSize(size, false);
}

void Texture::Unload() {
  original_size_ = ImVec2(0.0, 0.0);
  is_loaded_ = false;
  is_vector_ = false;

  glDeleteTextures(1, &texture_);
}

void Texture::CheckSize(const ImVec2 &expected_size) {
  if (expected_size.x == 0.0 && expected_size.y == 0.0 &&
      size_.x != original_size_.x && size_.y != original_size_.y)
    SetSize(ImVec2(0.0, 0.0));
  if ((expected_size.x != 0.0 && expected_size.x != size_.x) ||
      (expected_size.y != 0.0 && expected_size.y != size_.y))
    SetSize(expected_size);
}

ImVec2 Texture::GetSize() const { return size_; }

GLuint Texture::GetTexture() const { return texture_; }

const char *Texture::GetFileName() const { return file_name_; }

bool Texture::IsLoaded() const { return is_loaded_; }

void Texture::SetSize(const ImVec2 &new_size, const bool &reload_if_needed) {
  if (is_vector_ && reload_if_needed) return LoadAnother(file_name_, new_size);

  if (new_size.x == 0.0 && new_size.y == 0.0)
    size_ = original_size_;
  else if (new_size.x == 0.0 && new_size.y != 0.0)
    size_ =
        ImVec2(original_size_.x * (new_size.y / original_size_.y), new_size.y);
  else if (new_size.x != 0.0 && new_size.y == 0.0)
    size_ =
        ImVec2(new_size.x, original_size_.y * (new_size.x / original_size_.x));
  else
    size_ = new_size;
}
}  // namespace utils
