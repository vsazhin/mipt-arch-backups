#pragma once

#include <memory>
#include <vector>

#include "Common.hpp"
#include "Filesystem.hpp"
#include "Time.hpp"


namespace backups {

struct RestorePoint {
  Id id;
  time::DateTime creation_time;
  fs::Path location;
  bool is_incremental;
  std::vector<fs::Path> files;
  std::size_t size;
};

}
