cmake_minimum_required(VERSION 2.8.12)
project(jakmt)
add_executable(jakmt src/jakmt/main.cpp)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -Wpedantic -O3 -march=native -funroll-loops")
# set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -Wpedantic -g -O0 -march=native")

include_directories(src/include)
