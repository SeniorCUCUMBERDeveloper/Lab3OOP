cmake_minimum_required(VERSION 3.10)
project(myprog)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fsanitize=thread -fPIE -pie")
add_executable(myprog ./test.cpp ./Storage/Storage.cpp ./Terminal/Terminal.cpp)
target_link_libraries(myprog gtest gtest_main pthread)


