cmake_minimum_required(VERSION 3.23)
project(DPI_server)

set(CMAKE_CXX_STANDARD 11)

add_executable(main main.cpp memory_utilization.cpp memory_utilization.h cup_utilization.cpp cup_utilization.h utils.cpp utils.h)

add_executable(challenge_test challenge_test.cpp)