cmake_minimum_required(VERSION 3.17)
project(uv_echo)

include_directories(${CMAKE_SOURCE_DIR}/libuv_cb/include)

set(CMAKE_CXX_STANDARD 14)

add_executable(uv_echo main.cpp User.cpp User.h Channel.cpp Channel.h)

target_link_libraries(uv_echo ${CMAKE_SOURCE_DIR}/libuv_cb/lib/libuv.a ws2_32 psapi iphlpapi userenv)