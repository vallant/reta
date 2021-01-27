cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
project(reta VERSION 0.0.1)

set(USE_SOURCE_GROUPS
    ON
    CACHE BOOL "Specify if source groups should be generated")

add_subdirectory(cmake)
add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(resources)
reta_add_helper_targets()
