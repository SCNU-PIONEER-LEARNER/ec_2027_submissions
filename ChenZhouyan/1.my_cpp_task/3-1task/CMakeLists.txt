cmake_minimum_required(VERSION 3.25)
project(my_cpp_task CXX)

# 统一用 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 生成 clangd 需要的编译数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 6 个小任务，每个一个子目录
add_subdirectory(task1)
add_subdirectory(task2)
add_subdirectory(task3)
add_subdirectory(task4)
add_subdirectory(task5)
add_subdirectory(task6)
