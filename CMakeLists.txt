cmake_minimum_required(VERSION 3.23)
project(DPI_client)

set(CMAKE_CXX_STANDARD 11)

#include_directories(~/OpenSSL/include)

add_executable(main main.cpp utils.cpp)

#target_link_libraries(main /home/qwertyu/OpenSSL/lib/libssl.a  /home/qwertyu/OpenSSL/lib/libcrypto.a )