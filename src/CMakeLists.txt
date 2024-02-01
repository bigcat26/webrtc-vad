project(vad)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE srcs *)
file(GLOB_RECURSE unittest_srcs **/*_unittest.cc)

# exclude unittests from srcs
list(REMOVE_ITEM srcs ${unittest_srcs})

find_package(absl REQUIRED)

add_library(vad ${srcs})
target_include_directories(vad PUBLIC .)
target_link_libraries(vad PUBLIC absl::absl)

if (WIN32)
    target_compile_definitions(vad PUBLIC WEBRTC_WIN=1)
elseif(ANDROID)
    target_compile_definitions(vad PUBLIC WEBRTC_ANDROID=1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(vad PUBLIC WEBRTC_POSIX=1)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()