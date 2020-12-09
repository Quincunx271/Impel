cmake_minimum_required(VERSION 3.4.0)
project(test_package CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_set_find_paths(TARGETS)

find_package(Impel REQUIRED CONFIG)

add_executable(test_package test.cpp)
target_link_libraries(test_package Impel::Impel)
