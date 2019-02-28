cmake_minimum_required(VERSION 3.12.3)
project(jakmt)
add_executable(jakmt src/jakmt/main.cpp)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -Wpedantic -O2")