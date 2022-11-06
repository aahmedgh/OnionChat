set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

add_compile_options(-Wall -fsanitize=address)
add_link_options(-fsanitize=address)
