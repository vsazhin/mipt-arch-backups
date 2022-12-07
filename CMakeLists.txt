cmake_minimum_required(VERSION 3.10)
project(backups)

set(CMAKE_CXX_STANDARD 20)

set(
    SOURCE_FILES
    src/BasicConsoleApp.cpp
    src/Backup.cpp
    src/Time.cpp
    src/BackupManager.cpp
    src/RestorePointLimit.cpp
)

add_executable(backups ${SOURCE_FILES})
