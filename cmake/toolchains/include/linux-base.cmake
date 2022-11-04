if(NOT ${CMAKE_HOST_SYSTEM} STREQUAL "Linux")
  message(FATAL_ERROR "Cannot build on a non Linux system")
endif()

set(CMAKE_SYSTEM_NAME Linux)
