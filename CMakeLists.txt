cmake_minimum_required(VERSION 3.2)

project(BTree)

add_executable(tree_test tree_test.cpp)
add_executable(speed_test speed_test.cpp)

set_target_properties(tree_test PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)