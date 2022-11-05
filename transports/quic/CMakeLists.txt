add_executable(quic_hello_world src/hello_world.cpp)
target_link_libraries(quic_hello_world lsquic boringssl-crypto)
