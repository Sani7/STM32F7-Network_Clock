cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
  
project(Network_Clock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

# defines targets and sources
# add_subdirectory(src)

# contains an "external" library we will link to
# add_subdirectory(external)

# defines the executable
# add_subdirectory(tools)

# enable testing and define tests
enable_testing()
add_subdirectory(tests)
