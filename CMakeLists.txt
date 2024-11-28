cmake_minimum_required(VERSION 3.10)
project(myprog)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -g")
add_executable(myprog ./test.cpp ./Storage/Storage.cpp ./Terminal/Terminal.cpp)
target_link_libraries(myprog gtest pthread)

