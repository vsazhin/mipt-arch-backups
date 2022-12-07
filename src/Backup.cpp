#include "Backup.hpp"

#include <sstream>
#include <iostream>


namespace backups {

Backup Backup::create(
  Id id,
  std::span<fs::Path> files,
  const fs::Path& location,
  std::unique_ptr<IBackupAlgorithm> algorithm
) {
  Backup backup;
  backup.id = id;
  backup.location = location;
  backup.algorithm = std::move(algorithm);
  backup.createRestorePoint(files, false);
  return backup;
}

Id Backup::getId() const {
  return id;
}

const std::vector<fs::Path>& Backup::getFiles() const {
  return restore_points.back().files;
}

time::DateTime Backup::getCreationTime() const {
  return creation_time;
}

std::size_t Backup::getSize() const {
  return size;
}

std::vector<std::string> Backup::printableList() const {
  using SS = std::stringstream;
  std::vector<std::string> list;
  list.push_back((SS{} << "id: " << id).str());
  list.push_back((SS{} << "creation time: " << time::toString(creation_time)).str());
  list.push_back((SS{} << "location: " << location).str());
  list.push_back((SS{} << "size: " << size).str());
  list.push_back((SS{} << "algorithm: " << algorithm->getName()).str());
  list.push_back((SS{} << "limit: " << rp_limit->getDescription()).str());
  list.push_back("restore points:");
  for (const auto& rp: restore_points) {
    list.push_back((SS{} << "  id: " << rp.id).str());
    list.push_back((SS{} << "  creation time: " << time::toString(rp.creation_time)).str());
    list.push_back((SS{} << "  location: " << rp.location).str());
    list.push_back((SS{} << "  incremental: " << (rp.is_incremental ? "true" : "false")).str());
    list.push_back((SS{} << "  size: " << rp.size).str());
    list.push_back("  files:  ");
    for (const auto& file: rp.files) {
      list.push_back((SS{} << "    " << file).str());
    }
  }
  return list;
}

void Backup::setLimit(std::unique_ptr<IRestorePointLimit> limit) {
  rp_limit = std::move(limit);
}

void Backup::restoreFiles(Id restore_point_id, fs::Path location) const {
  auto& restore_point = getRestorePoint(restore_point_id);
  algorithm->restoreFiles(restore_point.location, location);
}

std::vector<Id> Backup::getRestorePoints() const {
  std::vector<Id> rp_ids(restore_points.size());
  for (std::size_t i = 0; i < restore_points.size(); ++i) {
    rp_ids[i] = restore_points[i].id;
  }
  return rp_ids;
}

Id Backup::createRestorePoint(bool incremental) {
  return createRestorePoint(restore_points.back().files, incremental);
}

Id Backup::addFiles(std::span<fs::Path> files, bool incremental) {
  std::vector<fs::Path> backup_files = restore_points.back().files;
  backup_files.insert(backup_files.end(), files.begin(), files.end());
  return createRestorePoint(backup_files, incremental);
}

Id Backup::removeFiles(std::span<fs::Path> files, bool incremental) {
  std::vector<fs::Path> backup_files = restore_points.back().files;
  backup_files.erase(
    std::remove_if(backup_files.begin(), backup_files.end(), [&](const auto& file) {
      return std::find(files.begin(), files.end(), file) != files.end();
    }),
    backup_files.end()
  );
  return createRestorePoint(backup_files, incremental);
}

void Backup::removeRestorePoint(Id id) {
  std::size_t index = 0;
  for (std::size_t i = 0; i < restore_points.size(); ++i) {
    if (restore_points[i].id != id) continue;
    index = i;
    break;
  }
  removeRestorePointAt(index);
}

void Backup::remove() {
  fs::remove(location);
  restore_points.clear();
}

void Backup::cleanup() {
  std::size_t bad_prefix_size = std::min(
    rp_limit->badPrefixSize(restore_points),
    restore_points.size() - 1
  );
  while (bad_prefix_size > 0) {
    for (std::size_t i = 0; i < bad_prefix_size; ++i) {
      removeRestorePointAt(0);
    }
    bad_prefix_size = std::min(rp_limit->badPrefixSize(restore_points), restore_points.size() - 1);
  }
}

Id Backup::createRestorePoint(std::span<fs::Path> files, bool incremental) {
  Id rp_id = free_rp_id++;
  fs::Path rp_location = fs::Path{location}.append(std::to_string(rp_id));
  std::size_t rp_size = 0;
  if (restore_points.empty()) {
    rp_size = algorithm->backupFiles(files, rp_location, incremental);
  } else {
    rp_size = algorithm->backupFiles(files, rp_location, incremental, restore_points.back().location);
  }
  size += rp_size;
  restore_points.push_back(std::move(RestorePoint{
    rp_id,
    time::now(),
    std::move(rp_location),
    incremental,
    std::vector<fs::Path>(files.begin(), files.end()),
    rp_size
  }));
  return rp_id;
}

void Backup::removeRestorePointAt(std::size_t index) {
  auto& restore_point = restore_points[index];
  size -= restore_point.size;
  if (index + 1 < restore_points.size()) {
    auto& child = restore_points[index + 1];
    if (child.is_incremental) {
      size -= child.size;
      child.size = algorithm->mergeRestorePoints(restore_point.location, child.location);
      size += child.size;
      child.is_incremental = restore_point.is_incremental;
    }
  }
  fs::remove(restore_point.location);
  restore_points.erase(restore_points.begin() + index);
}

RestorePoint& Backup::getRestorePoint(Id id) {
  for (auto it = restore_points.begin(); it != restore_points.end(); ++it) {
    if (it->id == id) return *it;
  }
  throw std::runtime_error("Could not find restore point");
}

const RestorePoint& Backup::getRestorePoint(Id id) const {
  for (auto it = restore_points.begin(); it != restore_points.end(); ++it) {
    if (it->id == id) return *it;
  }
  throw std::runtime_error("Could not find restore point");
}

}
