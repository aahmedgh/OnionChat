cmake_minimum_required(VERSION 3.22.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(onion-chat VERSION 0.1.0)

# Install gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1)
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)
enable_testing()
