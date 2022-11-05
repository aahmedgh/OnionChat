set(BORINGSSL_EXTERNAL boringssl-external)
set(BORINGSSL_PREFIX_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/${BORINGSSL_EXTERNAL}-prefix/src)
set(BORINGSSL_INCLUDE_DIR ${BORINGSSL_PREFIX_DIR}/${BORINGSSL_EXTERNAL}/include)
set(BORINGSSL_SSL_DIR ${BORINGSSL_PREFIX_DIR}/${BORINGSSL_EXTERNAL}-build/ssl)
set(BORINGSSL_CRYPTO_DIR
    ${BORINGSSL_PREFIX_DIR}/${BORINGSSL_EXTERNAL}-build/crypto)

ExternalProject_Add(
  ${BORINGSSL_EXTERNAL}
  GIT_REPOSITORY https://github.com/google/boringssl
  GIT_TAG a9670a8b476470e6f874fef3554e8059683e1413
  CMAKE_ARGS -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
             -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  INSTALL_COMMAND "" # Do not build install target (there is none for BoringSSL)
  TEST_COMMAND "" # Do not build test target
)

add_library(boringssl-ssl STATIC IMPORTED GLOBAL)
set_target_properties(
  boringssl-ssl
  PROPERTIES IMPORTED_LOCATION
             ${BORINGSSL_SSL_DIR}/libssl${CMAKE_STATIC_LIBRARY_SUFFIX}
             INTERFACE_INCLUDE_DIRECTORIES ${BORINGSSL_INCLUDE_DIR})
add_dependencies(boringssl-ssl ${BORINGSSL_EXTERNAL})

add_library(boringssl-crypto STATIC IMPORTED GLOBAL)
set_target_properties(
  boringssl-crypto
  PROPERTIES IMPORTED_LOCATION
             ${BORINGSSL_CRYPTO_DIR}/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}
             INTERFACE_INCLUDE_DIRECTORIES ${BORINGSSL_INCLUDE_DIR})
add_dependencies(boringssl-crypto ${BORINGSSL_EXTERNAL})
