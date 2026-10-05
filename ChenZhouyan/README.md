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

环境：GCC（WinLibs UCRT）+ CMake 4.4.x + Ninja，标准 C++20。

每个 task 独立构建（以 task1 为例）：

```bash
cmake -S 1.my_cpp_task/3-1task/task1 -B 1.my_cpp_task/3-1task/task1/build -G Ninja \
      -DCMAKE_C_COMPILER=C:/mingw64/bin/gcc.exe \
      -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
ninja -C 1.my_cpp_task/3-1task/task1/build
./1.my_cpp_task/3-1task/task1/build/task1.exe
```

> 其他 task 把路径里的 `task1` 换成 `task2` ~ `task6` 即可。
> Task 7 为代码阅读任务，交付物是思维导图 `RobotControl思维导图.svg`，无需编译运行。

## 说明

- 其余模块（初识 RM、嵌入式舵机、硬件 PCB、拓展）的资料不在本分支提交范围内。
- 提交分支：`cpp-task`（不合并至 main）。
