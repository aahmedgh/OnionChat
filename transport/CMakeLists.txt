add_library(quic-lib STATIC quic/src/quic.cpp)
target_include_directories(quic-lib PUBLIC include quic/include)
target_link_libraries(
  quic-lib
  PUBLIC lsquic
  PRIVATE common-lib logging-lib)
set_target_properties(quic-lib PROPERTIES PREFIX "")

add_executable(transport-bin transport.cpp)
target_link_libraries(transport-bin PRIVATE common-lib quic-lib)
