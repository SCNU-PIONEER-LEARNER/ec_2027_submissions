cmake_minimum_required(VERSION 3.25)
project(demo CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(prime_utils prime_utils.cpp)     # 把质数逻辑编成静态库
add_executable(task1 3-1-task1.cpp)         # 可执行程序
target_link_libraries(task1 PRIVATE prime_utils)  # 程序链接 prime_utils 库
