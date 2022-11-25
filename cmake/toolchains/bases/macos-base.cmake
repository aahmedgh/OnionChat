if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
  message(FATAL_ERROR "Cannot build on a non MacOS system")
endif()

set(CMAKE_SYSTEM_NAME Darwin)
