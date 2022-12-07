#pragma once

#include <chrono>
#include <string>


namespace backups::time {

using DateTime = std::chrono::time_point<std::chrono::system_clock>;
using Duration = std::chrono::system_clock::duration;

DateTime now();

std::string toString(DateTime timestamp);

}
