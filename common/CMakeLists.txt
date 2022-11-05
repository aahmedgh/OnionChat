add_library(common-lib INTERFACE)
target_include_directories(common-lib INTERFACE include)
set_target_properties(common-lib PROPERTIES PREFIX "")
