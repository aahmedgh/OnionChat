if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
  set(CONNECTION_SOURCES unix/src/socket.cpp)
else()
  message(FATAL_ERROR "This toolchain is not supported")
endif()
add_library(connection-lib STATIC ${CONNECTION_SOURCES})
target_include_directories(connection-lib PUBLIC include)
target_link_libraries(connection-lib PRIVATE common-lib logging-lib)

set(QUIC_SOURCES quic/src/quic.cpp quic/src/engine.cpp)
find_library(libevent REQUIRED) # TODO: Build this in source for cross compiling
add_library(quic-lib STATIC ${QUIC_SOURCES})
target_include_directories(quic-lib PUBLIC quic/include)
target_link_libraries(
  quic-lib
  PUBLIC lsquic
  PRIVATE common-lib logging-lib connection-lib event)
set_target_properties(quic-lib PROPERTIES PREFIX "")

add_executable(transport-bin transport.cpp)
target_link_libraries(transport-bin PRIVATE common-lib connection-lib quic-lib)
