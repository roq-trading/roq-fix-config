cmake_minimum_required(VERSION 3.12)

# cmake

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake
                      ${CMAKE_MODULE_PATH})

# config

include(RoqConfig)

# project

project(roq-fix-config)

# filesystem

include(GNUInstallDirs)

# dependencies

find_package(absl REQUIRED)
find_package(fmt REQUIRED)
find_package(roq-api REQUIRED)
find_package(roq-flags REQUIRED)
find_package(roq-io REQUIRED)
find_package(roq-logging REQUIRED)
find_package(roq-web REQUIRED)
find_package(unordered_dense REQUIRED)

if(UNIX AND NOT APPLE)
  set(RT_LIBRARIES rt)
else()
  set(RT_LIBRARIES)
endif()

find_package(PkgConfig REQUIRED)
find_package(Catch2 REQUIRED)

find_program(CLANG_FORMAT clang-format REQUIRED)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

if(BUILD_TESTING)
  find_package(Catch2 REQUIRED)
endif()

option(BUILD_BENCHMARK "Enable benchmark" ON)

if(BUILD_BENCHMARK)
  find_package(benchmark REQUIRED)
endif()

# include

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  # >>> autogen >>>
  ${CMAKE_BINARY_DIR}/src)

# sub-projects

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/roq/config/fix)

# config

install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

install(FILES ${CMAKE_SOURCE_DIR}/CHANGELOG.md
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME})

# docs

option(BUILD_DOCS "Enable documentation" OFF)

if(BUILD_DOCS)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/
          DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME})
endif()
