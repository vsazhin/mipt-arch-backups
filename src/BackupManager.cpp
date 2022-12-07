#include "BackupManager.hpp"


namespace backups {

void BackupManager::loadBackupData() {
  //! placeholder
}

void BackupManager::saveBackupData() {
  //! placeholder
}

std::vector<std::string> BackupManager::printableList() const {
  std::vector<std::string> list;
  for (const auto& backup: backups) {
    auto backup_info = backup.printableList();
    list.insert(list.end(), backup_info.begin(), backup_info.end());
  }
  return list;
}

Backup& BackupManager::createBackup(
  std::span<fs::Path> files,
  const fs::Path& location,
  std::unique_ptr<IBackupAlgorithm> algorithm
) {
  backups.emplace_back(std::move(Backup::create(
    free_backup_id++,
    files,
    location,
    std::move(algorithm)
  )));
  return backups.back();
}

const std::vector<Backup>& BackupManager::getBackups() {
  return backups;
}

Backup& BackupManager::getBackup(Id id) {
  for (auto& backup: backups) {
    if (backup.getId() == id) return backup;
  }
  throw std::runtime_error("Could not find backup");
}

bool BackupManager::removeBackup(std::uint64_t id) {
  for (auto it = backups.begin(); it != backups.end(); ++it) {
    if (it->getId() != id) continue;
    it->remove();
    backups.erase(it);
    return true;
  }
  return false;
}

}
