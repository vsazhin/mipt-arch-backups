#include <iostream>
#include <string>
#include <vector>
#include <span>
#include <sstream>

#include "BackupManager.hpp"


static backups::BackupManager backup_manager;

bool parseCommand(std::span<std::string> arguments) {
  std::string command = arguments[0];
  if (command == "list") {
    for (const auto& line: backup_manager.printableList()) {
      std::cout << line << std::endl;
    }
  } else if (command == "new") {
    std::unique_ptr<backups::IBackupAlgorithm> algorithm;
    if (arguments[1] == "ss") {
      algorithm = std::make_unique<backups::BASeparateStorage>();
    } else if (arguments[1] == "cs") {
      algorithm = std::make_unique<backups::BACombinedStorage>();
    }
    std::vector<backups::fs::Path> files{arguments.begin() + 3, arguments.end()};
    backup_manager.createBackup(files, arguments[2], std::move(algorithm));
  } else if (command == "rp") {
    backups::Id id = std::stoi(arguments[1]);
    bool incremental = arguments[2] == "inc";
    backup_manager.getBackup(id).createRestorePoint(incremental);
  } else if (command == "af") {
    backups::Id id = std::stoi(arguments[1]);
    bool incremental = arguments[2] == "inc";
    std::vector<backups::fs::Path> files{arguments.begin() + 3, arguments.end()};
    backup_manager.getBackup(id).addFiles(files, incremental);
  } else if (command == "rf") {
    backups::Id id = std::stoi(arguments[1]);
    bool incremental = arguments[2] == "inc";
    std::vector<backups::fs::Path> files{arguments.begin() + 3, arguments.end()};
    backup_manager.getBackup(id).removeFiles(files, incremental);
  } else if (command == "rrp") {
    backups::Id id = std::stoi(arguments[1]);
    backups::Id rp_id = std::stoi(arguments[2]);
    backup_manager.getBackup(id).removeRestorePoint(rp_id);
  } else if (command == "remove") {
    backups::Id id = std::stoi(arguments[1]);
    backup_manager.removeBackup(id);
  } else if (command == "cleanup") {
    backups::Id id = std::stoi(arguments[1]);
    backup_manager.getBackup(id).cleanup();
  } else if (command == "limit") {
    backups::Id id = std::stoi(arguments[1]);
    std::unique_ptr<backups::IRestorePointLimit> limit;
    if (arguments[2] == "size") {
      std::size_t size = std::stoi(arguments[3]);
      auto limit = std::make_unique<backups::RPLBySize>();
      limit->size = size;
      backup_manager.getBackup(id).setLimit(std::move(limit));
    } else if (arguments[2] == "number") {
      std::size_t count = std::stoi(arguments[3]);
      auto limit = std::make_unique<backups::RPLByNumber>();
      limit->count = count;
      backup_manager.getBackup(id).setLimit(std::move(limit));
    } else if (arguments[2] == "hybrid") {
      backups::CombinationRule delete_rule = arguments[3] == "all"
        ? backups::CombinationRule::All
        : backups::CombinationRule::Any;
      auto size_limit = std::make_unique<backups::RPLBySize>();
      size_limit->size = std::stoi(arguments[4]);
      auto number_limit = std::make_unique<backups::RPLByNumber>();
      number_limit->count = std::stoi(arguments[5]);
      auto limit = std::make_unique<backups::RPLHybrid>();
      limit->delete_rule = delete_rule;
      limit->limits.resize(2);
      limit->limits[0] = std::move(size_limit);
      limit->limits[1] = std::move(number_limit);
      backup_manager.getBackup(id).setLimit(std::move(limit));
    }
  } else if (command == "restore") {
    backups::Id id = std::stoi(arguments[1]);
    backups::Id rp_id = std::stoi(arguments[2]);
    backups::fs::Path location = arguments[3];
    backup_manager.getBackup(id).restoreFiles(rp_id, location);
  } else if (command == "exit") {
    return false;
  }
  return true;
}

void run() {
  std::string command;
  std::string argument;
  while (true) {
    std::cout << "> ";
    std::getline(std::cin, command);
    std::stringstream stream(command);
    std::vector<std::string> arguments;
    while (std::getline(stream, argument, ' ')) {
      arguments.push_back(argument);
    }
    if (!parseCommand(arguments)) {
      break;
    }
  }
}

int main() {
  run();
  return 0;
}
