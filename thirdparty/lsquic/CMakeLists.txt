set(LSQUIC_EXTERNAL lsquic-external)
set(LSQUIC_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/${LSQUIC_EXTERNAL}-prefix/src)
set(LSQUIC_INCLUDE_DIR ${LSQUIC_PREFIX_DIR}/${LSQUIC_EXTERNAL}/include)
set(LSQUIC_LIB_DIR ${LSQUIC_PREFIX_DIR}/${LSQUIC_EXTERNAL}-build/src/liblsquic)

ExternalProject_Add(
  ${LSQUIC_EXTERNAL}
  GIT_REPOSITORY https://github.com/litespeedtech/lsquic/
  GIT_TAG v3.2.0
  CMAKE_ARGS -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
             -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DBORINGSSL_INCLUDE=${BORINGSSL_INCLUDE_DIR}
             -DBORINGSSL_LIB_ssl=boringssl-ssl
             -DBORINGSSL_LIB_crypto=boringssl-crypto
             -DLSQUIC_TESTS=0
             -DLSQUIC_BIN=0
  BUILD_BYPRODUCTS
    ${LSQUIC_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}lsquic${CMAKE_STATIC_LIBRARY_SUFFIX}
  UPDATE_COMMAND ""
  INSTALL_COMMAND "" # Do not build install target
  TEST_COMMAND "" # Do not build test target
  DEPENDS boringssl-ssl boringssl-crypto)

file(MAKE_DIRECTORY ${LSQUIC_INCLUDE_DIR})

add_library(lsquic STATIC IMPORTED GLOBAL)
set_target_properties(
  lsquic
  PROPERTIES
    IMPORTED_LOCATION
    ${LSQUIC_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}lsquic${CMAKE_STATIC_LIBRARY_SUFFIX}
    INTERFACE_INCLUDE_DIRECTORIES ${LSQUIC_INCLUDE_DIR})
find_package(ZLIB REQUIRED) # TODO: Get zlib build in source for cross compiling
target_link_libraries(lsquic INTERFACE boringssl-ssl boringssl-crypto
                                       ZLIB::ZLIB)
add_dependencies(lsquic ${LSQUIC_EXTERNAL})
