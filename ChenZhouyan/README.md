# 电控组千行计划 · C++ 模块提交

本分支仅包含 **编程基础（C++）模块** 的提交内容，对应任务 Task 1 ~ Task 7。

## 目录结构

```
1.my_cpp_task/3-1task/        # Task 1 ~ Task 6 源码
    task1/  Task1 质数计数（含 prime_utils 库）
    task2/  Task2 学生结构体（数组/指针）
    task3/  Task3 归一化函数 rangemap
    task4/  Task4 坐标旋转
    task5/  Task5 二进制位操作
    task6/  Task6 类与对象（几何体抽象基类）
3-编程基础/3-2-RobotControl/   # Task 7 阅读 RobotControl 代码 + 思维导图
    RobotControl.hpp / RobotControl.cpp   # 被阅读的代码
    RobotControl思维导图.svg               # 按 README 要求绘制的状态机思维导图
```

## 构建与运行

环境：GCC（C:/mingw64）+ CMake + Ninja，VSCode 打开 `3-1task` 文件夹即可用 CMake Tools 一键构建调试（仓库自带 `mingw-toolchain.cmake` 强制使用 g++）。

命令行方式（在 `3-1task` 目录下执行）：

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/task1/task1.exe   # task2 ~ task6 同理，在 build/taskX/ 下
```

> Task 7 为代码阅读任务，交付物是思维导图 `RobotControl思维导图.svg`，无需编译运行。

## 说明

- 其余模块（初识 RM、嵌入式舵机、硬件 PCB、拓展）的资料不在本分支提交范围内。
- 提交分支：`ChenZhouyan`。
