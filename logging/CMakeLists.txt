add_library(logging-lib STATIC src/logger.cpp)
target_include_directories(logging-lib PUBLIC include)
target_link_libraries(logging-lib PRIVATE common-lib)
set_target_properties(logging-lib PROPERTIES PREFIX "")
