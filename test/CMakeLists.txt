cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall")

add_library(json ../src/json.cc)
add_library(parse ../src/parse.cc)

enable_testing()
add_executable(Test test.cc)
target_link_libraries(Test json parse /usr/lib/libgtest.so /usr/lib/libgtest_main.so -pthread)
add_test(NAME Test COMMAND Test)

add_executable(jsonchecker jsonchecker.cc)
target_link_libraries(jsonchecker json parse)
