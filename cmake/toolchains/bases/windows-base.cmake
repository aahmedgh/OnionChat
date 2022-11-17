if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
  message(FATAL_ERROR "Cannot build on a non Windows system")
endif()

set(CMAKE_SYSTEM_NAME Windows)

add_compile_definitions(WIN32_LEAN_AND_MEAN)
