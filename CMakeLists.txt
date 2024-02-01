cmake_minimum_required(VERSION 3.15)
project(webrtc-vad)

include(CTest)

# add search path for find_package
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(example)
