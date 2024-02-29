
include(CTest)

add_executable(test_file1 test_file1.cpp)

target_link_libraries(test_file1 PRIVATE calculator)
target_compile_definitions(test_file1 PRIVATE CATCH_CONFIG_MAIN)

add_test(NAME test_file1 COMMAND test_file1)