cmake_minimum_required(VERSION 3.18)
project(my)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_LIBRARIES -lbdd)

add_executable(my main.cpp)