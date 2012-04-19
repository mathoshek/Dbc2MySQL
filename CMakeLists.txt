cmake_minimum_required(VERSION 2.6)

project(Dbc2MySQL)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/bin")

find_package(MySQL REQUIRED)

add_subdirectory(dep/tinyxml)
add_subdirectory(src)