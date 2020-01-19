cmake_minimum_required(VERSION 3.15)
project(Blatt_06)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(helper)
include_directories(helper/wulkanat)

add_executable(Blatt_06
    helper/wulkanat/commander.h
    helper/wulkanat/debug.h
    helper/wulkanat/descriptive_types.h
    helper/wulkanat/friendly_heap.h
    client.c
    client.h
    CMakeLists.txt)
