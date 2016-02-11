cmake_minimum_required(VERSION 3.3.2)
project(grpc_test)

#version number
set (grpc_test_VERSION_MAJOR 0)
set (grpc_test_VERSION_MINOR 1)

#header file for CMake settings
configure_file(
        "${PROJECT_SOURCE_DIR}/GRPCTestConfig.h.in"
        "${PROJECT_BINARY_DIR}/GRPCTestConfig.h"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include_directories("${PROJECT_BINARY_DIR}")




find_package(Protobuf REQUIRED)
if (PROTOBUF_FOUND)
	#// message(STATUS "PROTOBUF VERSION: " $$)
	message(STATUS "Protobuf incl"  ${PROTOBUF_INCLUDE_DIRS})
	message(STATUS "Protobuf libg"  ${PROTOBUF_LINK_DIRS})

    include_directories("${PROTOBUF_INCLUDE_DIRS}")
    link_directories("/usr/local/lib/")

#    target_link_libraries(main grpc++)
#link_libraries(protobuf)
#    else()

endif(PROTOBUF_FOUND)

find_package(GRPC REQUIRED)
link_libraries(grpc++ grpc gpr protobuf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/hybridsim.grpc.pb.cc src/hybridsim.pb.cc src/DummyJPS.cpp src/DummyJPS.h)
add_executable(grpc_test ${SOURCE_FILES})


