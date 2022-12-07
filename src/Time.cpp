#include "Time.hpp"

#include <sstream>
#include <iomanip>


namespace backups::time {

DateTime now() {
  return std::chrono::system_clock::now();
}

std::string toString(DateTime timestamp) {
  std::time_t tt = std::chrono::system_clock::to_time_t(timestamp);
  std::tm tm = *std::gmtime(&tt);
  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

}
