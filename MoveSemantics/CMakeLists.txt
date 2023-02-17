cmake_minimum_required(VERSION 3.24)
project(MoveSemantics)

set(CMAKE_CXX_STANDARD 17)

add_executable(basic main.cpp)
add_executable(forwarding forwarding.cpp)
