#pragma once

#include <optional>
#include <span>
#include <string>

#include "RestorePoint.hpp"
#include "Filesystem.hpp"


namespace backups {

class IBackupAlgorithm {
public:
  virtual ~IBackupAlgorithm() = default;

  virtual std::string getName() const = 0;

  virtual std::size_t backupFiles(
    std::span<fs::Path> files,
    const fs::Path& location,
    bool incremental,
    std::optional<fs::Path> parent_rp = std::nullopt
  ) = 0;
  virtual void restoreFiles(const fs::Path& backup_location, const fs::Path& destination) = 0;
  virtual std::size_t mergeRestorePoints(const fs::Path& source, const fs::Path& destination) = 0;
};

class BASeparateStorage: public IBackupAlgorithm {
public:
  std::string getName() const override { return "Separate Storage"; };

  std::size_t backupFiles(
    std::span<fs::Path> files,
    const fs::Path& location,
    bool incremental,
    std::optional<fs::Path> parent_rp = std::nullopt
  ) override { return rand() % 1000; }
  void restoreFiles(const fs::Path& backup_location, const fs::Path& destination) override {}
  std::size_t mergeRestorePoints(const fs::Path& source, const fs::Path& destination) override { return rand() % 1000; }
};

class BACombinedStorage: public IBackupAlgorithm {
public:
  std::string getName() const override { return "Combined Storage"; };

  std::size_t backupFiles(
    std::span<fs::Path> files,
    const fs::Path& location,
    bool incremental,
    std::optional<fs::Path> parent_rp = std::nullopt
  ) override { return rand() % 1000; }
  void restoreFiles(const fs::Path& backup_location, const fs::Path& destination) override {}
  std::size_t mergeRestorePoints(const fs::Path& source, const fs::Path& destination) override { return rand() % 1000; }
};

}
