project(mINI)

add_library(mini INTERFACE ini.hh)
target_include_directories(mini INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
