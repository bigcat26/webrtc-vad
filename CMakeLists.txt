cmake_minimum_required(VERSION 3.15)

include(CTest)

# add search path for find_package
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

find_package(absl REQUIRED)

add_subdirectory(src)
