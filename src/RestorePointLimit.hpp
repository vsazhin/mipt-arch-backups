#pragma once

#include <vector>
#include <memory>
#include <deque>
#include <string>

#include "Common.hpp"
#include "Time.hpp"
#include "RestorePoint.hpp"


namespace backups {

struct IRestorePointLimit {
    virtual ~IRestorePointLimit() = default;

    virtual std::string getDescription() const = 0;

    virtual std::size_t badPrefixSize(const std::deque<RestorePoint>& restore_points) = 0;
};

struct RPLBySize: public IRestorePointLimit {
  std::size_t size = std::numeric_limits<std::size_t>::max();

  std::string getDescription() const override;

  std::size_t badPrefixSize(const std::deque<RestorePoint>& restore_points) override;
};

struct RPLByNumber: public IRestorePointLimit {
  std::size_t count = std::numeric_limits<std::size_t>::max();

  std::string getDescription() const override;

  std::size_t badPrefixSize(const std::deque<RestorePoint>& restore_points) override;
};

struct RPLByTime: public IRestorePointLimit {
  time::Duration period;

  std::string getDescription() const override;

  std::size_t badPrefixSize(const std::deque<RestorePoint>& restore_points) override;
};

enum class CombinationRule {
  Any,
  All
};

struct RPLHybrid: public IRestorePointLimit {
  CombinationRule delete_rule;
  std::vector<std::unique_ptr<IRestorePointLimit>> limits;

  std::string getDescription() const override;

  std::size_t badPrefixSize(const std::deque<RestorePoint>& restore_points) override;
};

}
