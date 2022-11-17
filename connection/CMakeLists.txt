if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
  set(CONNECTION_SOURCES src/unix/socket.cpp)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  set(CONNECTION_SOURCES src/windows/socket.cpp)
  set(PLATFORM_LIBS ws2_32)
else()
  message(FATAL_ERROR "This toolchain is not supported")
endif()
add_library(connection-lib STATIC ${CONNECTION_SOURCES})
target_include_directories(connection-lib PUBLIC include)
target_link_libraries(connection-lib PRIVATE common-lib logging-lib
                                             ${PLATFORM_LIBS})

add_library(udp-lib STATIC udp/src/udp.cpp)
target_include_directories(udp-lib PUBLIC udp/include)
target_link_libraries(udp-lib PRIVATE common-lib logging-lib connection-lib)
set_target_properties(udp-lib PROPERTIES PREFIX "")

add_executable(transport-bin transport.cpp)
target_link_libraries(transport-bin PRIVATE common-lib connection-lib udp-lib
                                            logging-lib)
