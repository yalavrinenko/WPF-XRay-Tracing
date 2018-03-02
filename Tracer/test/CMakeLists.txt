cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

set (TEST_SRC main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_executable(rt_test ${TEST_SRC})

target_link_libraries(rt_test xray-tracing stdc++fs)