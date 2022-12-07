#include "RestorePointLimit.hpp"

#include <sstream>


namespace backups {

std::string RPLBySize::getDescription() const {
  return (std::stringstream{} << "[by size: " << size << "]").str();
}

std::size_t RPLBySize::badPrefixSize(const std::deque<RestorePoint>& restore_points) {
  std::size_t suffix_size = 0;
  for (auto it = restore_points.rbegin(); it != restore_points.rend(); ++it) {
    suffix_size += it->size;
    if (suffix_size <= size) continue;
    return restore_points.rend() - it;
  }
  return 0;
}

std::string RPLByNumber::getDescription() const {
  return (std::stringstream{} << "[by number: " << count << "]").str();
}

std::size_t RPLByNumber::badPrefixSize(const std::deque<RestorePoint>& restore_points) {
  return restore_points.size() <= count ? 0 : restore_points.size() - count;
}

std::string RPLByTime::getDescription() const {
  return (std::stringstream{} << "[by time: " << period.count() << "]").str();
}

std::size_t RPLByTime::badPrefixSize(const std::deque<RestorePoint>& restore_points) {
  auto oldest_time = restore_points.back().creation_time - period;
  for (auto it = restore_points.rbegin(); it != restore_points.rend(); ++it) {
    if (it->creation_time >= oldest_time) continue;
    return restore_points.rend() - it;
  }
  return 0;
}

std::string RPLHybrid::getDescription() const {
  std::stringstream ss;
  ss << "[";
  switch (delete_rule) {
  case CombinationRule::Any: {ss << "any of: "; break;}
  case CombinationRule::All: {ss << "all of: "; break;}
  default: throw std::runtime_error("Unsupported combination rule");
  }
  for (std::size_t i = 0; i + 1 < limits.size(); ++i) {
    ss << limits[i]->getDescription() << ", ";
  }
  ss << limits.back()->getDescription() << "]";
  return ss.str();
}

std::size_t RPLHybrid::badPrefixSize(const std::deque<RestorePoint>& restore_points) {
  std::vector<std::size_t> prefix_sizes;
  for (const auto& limit: limits) {
    prefix_sizes.push_back(limit->badPrefixSize(restore_points));
  }
  switch (delete_rule) {
  case CombinationRule::Any: return *std::max_element(prefix_sizes.begin(), prefix_sizes.end());
  case CombinationRule::All: return *std::min_element(prefix_sizes.begin(), prefix_sizes.end());
  default: throw std::runtime_error("Unsupported combination rule");
  }
}

}
