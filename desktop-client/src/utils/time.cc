//
// Created by Mixerou on 18.12.2024.
//

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "utils.h"

namespace utils {
std::string FormatUnixTimestampToHumanReadable(const int64_t timestamp) {
  const std::time_t time = timestamp;
  const std::tm* gmt_time = std::gmtime(&time);
  std::ostringstream result;

  result << std::put_time(gmt_time, "%d/%m/%Y");

  return result.str();
}

int64_t ConvertHumanReadableTimestampToUnix(const std::string& date) {
  int day, month, year;
  char delimiter;
  std::stringstream date_stream(date);
  std::tm time_struct{};

  date_stream >> day >> delimiter >> month >> delimiter >> year;

  if (day >= 1 && day <= 31) time_struct.tm_mday = day;
  if (month >= 1 && month <= 12) time_struct.tm_mon = month - 1;

  if (year >= 1970 && year <= 2100)
    time_struct.tm_year = year - 1900;
  else
    time_struct.tm_year = 70;

  if (time_struct.tm_mday == 0 && time_struct.tm_mon == 0 &&
      time_struct.tm_year == 70) {
    time_struct.tm_mday = 1;
  }

#if defined(_WIN32) || defined(_WIN64)
  const std::time_t time = _mkgmtime(&time_struct);
#else
  const std::time_t time = timegm(&time_struct);
#endif

  return time;
}
}  // namespace utils
