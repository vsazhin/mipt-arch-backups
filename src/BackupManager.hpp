#pragma once

#include <vector>
#include <string>

#include "Common.hpp"
#include "Filesystem.hpp"
#include "Backup.hpp"
#include "BackupAlgorithm.hpp"


namespace backups {

class BackupManager {
public:
  BackupManager() = default;

  void loadBackupData();
  void saveBackupData();

  std::vector<std::string> printableList() const;

  Backup& createBackup(
    std::span<fs::Path> files,
    const fs::Path& location,
    std::unique_ptr<IBackupAlgorithm> algorithm
  );
  const std::vector<Backup>& getBackups();
  Backup& getBackup(Id id);
  bool removeBackup(Id id);

  BackupManager(const BackupManager&) = delete;
  BackupManager& operator=(const BackupManager&) = delete;

  BackupManager(BackupManager&&) = delete;
  BackupManager& operator=(BackupManager&&) = delete;

private:
  Id free_backup_id{0};
  std::vector<Backup> backups;
};

}
