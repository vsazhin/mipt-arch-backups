#pragma once

#include <memory>
#include <vector>
#include <span>
#include <deque>
#include <string>

#include "Common.hpp"
#include "Filesystem.hpp"
#include "RestorePoint.hpp"
#include "RestorePointLimit.hpp"
#include "Time.hpp"
#include "BackupAlgorithm.hpp"


namespace backups {

class Backup {
public:
  static Backup create(
    Id id,
    std::span<fs::Path> files,
    const fs::Path& location,
    std::unique_ptr<IBackupAlgorithm> algorithm
  );

  Id getId() const;
  const std::vector<fs::Path>& getFiles() const;
  time::DateTime getCreationTime() const;
  std::size_t getSize() const;

  std::vector<std::string> printableList() const;

  void setLimit(std::unique_ptr<IRestorePointLimit> limit);

  void restoreFiles(Id restore_point_id, fs::Path location) const;

  std::vector<Id> getRestorePoints() const;
  const RestorePoint& getRestorePoint(Id id) const;
  Id createRestorePoint(bool incremental = true);
  Id addFiles(std::span<fs::Path> files, bool incremental = true);
  Id removeFiles(std::span<fs::Path> files, bool incremental = true);
  void removeRestorePoint(Id id);

  void remove();

  void cleanup();

  Backup(const Backup&) = delete;
  Backup& operator=(const Backup&) = delete;

  Backup(Backup&&) = default;
  Backup& operator=(Backup&&) = default;

private:
  Backup() = default;

  Id createRestorePoint(std::span<fs::Path> files, bool incremental);
  void removeRestorePointAt(std::size_t index);
  RestorePoint& getRestorePoint(Id id);

private:
  Id id;
  time::DateTime creation_time{time::now()};
  fs::Path location;
  std::size_t size{0};
  std::unique_ptr<IBackupAlgorithm> algorithm;
  std::unique_ptr<IRestorePointLimit> rp_limit{std::make_unique<RPLBySize>()};
  std::deque<RestorePoint> restore_points;
  Id free_rp_id{0};
};

}
