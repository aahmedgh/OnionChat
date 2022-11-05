include(ExternalProject)

# Put these variables out here so lsquic can consume these
set(BORINGSSL_EXTERNAL boringssl-external)
set(BORINGSSL_PREFIX_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/boringssl/${BORINGSSL_EXTERNAL}-prefix/src)
set(BORINGSSL_INCLUDE_DIR ${BORINGSSL_PREFIX_DIR}/${BORINGSSL_EXTERNAL}/include)

add_subdirectory(boringssl)
add_subdirectory(lsquic)
