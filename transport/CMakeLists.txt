set(QUIC_SOURCES quic/src/quic.cpp quic/src/engine.cpp)

find_library(libevent REQUIRED) # TODO: Build this in source for cross compiling
add_library(quic-lib STATIC ${QUIC_SOURCES})
target_include_directories(quic-lib PUBLIC include quic/include)
target_link_libraries(
  quic-lib
  PUBLIC lsquic
  PRIVATE common-lib logging-lib event)
set_target_properties(quic-lib PROPERTIES PREFIX "")

add_executable(transport-bin transport.cpp)
target_link_libraries(transport-bin PRIVATE common-lib quic-lib)
