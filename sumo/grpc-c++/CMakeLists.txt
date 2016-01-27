cmake_minimum_required(VERSION 3.3)
project(grpc_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(grpc_test ${SOURCE_FILES})