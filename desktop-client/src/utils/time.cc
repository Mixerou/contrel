//
// Created by Mixerou on 18.12.2024.
//

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "utils.h"

namespace utils {
std::string FormatUnixTimestampToHumanReadable(int64_t timestamp) {
  std::time_t time = static_cast<std::time_t>(timestamp);
  std::tm *gmt_time = std::gmtime(&time);
  std::ostringstream result;

  result << std::put_time(gmt_time, "%d/%m/%Y");

  return result.str();
}
}  // namespace utils
