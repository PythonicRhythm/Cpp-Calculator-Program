cmake_minimum_required(VERSION 3.19)
project(spring2024-csc3380-lecture7)

set(CMAKE_CXX_STANDARD 20)

add_executable(testing_calculator code/calculator.cpp)

enable_testing()
include(CTest)

target_compile_definitions(testing_calculator PRIVATE CATCH_CONFIG_MAIN)
add_test(NAME testing_calculator COMMAND testing_calculator)