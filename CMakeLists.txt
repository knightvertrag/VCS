cmake_minimum_required(VERSION 3.10)

project(Imperium)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(imperium src/main.cpp)
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  # Add the include directories for compiling
  include_directories(${OPENSSL_INCLUDE_DIR})

  # Add the static lib for linking
  target_link_libraries(imperium OpenSSL::SSL OpenSSL::Crypto)

  message(STATUS "Found OpenSSL ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR}")

else()

  message(STATUS "OpenSSL Not Found")

endif()
include_directories(include src)

add_library(repo src/repository.cpp)
add_library(imperium_object src/imperium_object.cpp)
add_library(blob src/blob_object.cpp)
add_library(tree src/tree_object.cpp)
add_library(commit src/commit_object.cpp)
add_library(lock src/lockfile.cpp)
add_library(refs src/refs.cpp)
add_library(index src/index.cpp)


target_link_libraries(imperium repo imperium_object blob tree commit refs lock index z)

