project(libvad)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE srcs *)
file(GLOB_RECURSE unittest_srcs **/*_unittest.cc)

# exclude unittests from srcs
list(REMOVE_ITEM srcs ${unittest_srcs})

add_library(libvad ${srcs})
target_include_directories(libvad PRIVATE .)
target_link_libraries(libvad PRIVATE absl::absl)

if (WIN32)
    target_compile_definitions(libvad PRIVATE WEBRTC_WIN=1)
elseif(ANDROID)
    target_compile_definitions(libvad PRIVATE WEBRTC_ANDROID=1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(libvad PRIVATE WEBRTC_POSIX=1)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()