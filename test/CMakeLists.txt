cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall")

enable_testing()
add_executable(Test test.cc ../json.cc)
target_link_libraries(Test /usr/lib/libgtest.so /usr/lib/libgtest_main.so -pthread)
add_test(NAME Test COMMAND Test)

add_library(json ../json.cc)
add_executable(jsonchecker jsonchecker.cc)
target_link_libraries(jsonchecker json)
