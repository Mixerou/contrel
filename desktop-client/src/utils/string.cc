//
// Created by Mixerou on 17.12.2024.
//

#include <string>

#include "utils.h"

namespace utils {
bool StringEndsWith(std::string const &str, std::string const &ending) {
  if (str.length() >= ending.length())
    return (0 == str.compare(str.length() - ending.length(), ending.length(),
                             ending));
  else
    return false;
}
}  // namespace utils
