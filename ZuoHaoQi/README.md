# 千行任务 · 左浩淇

2027 赛季 PIONEER 电控组千行任务提交。

## 目录结构

```
ZuoHaoQi/
├── README.md
└── 1.my_cpp_task/
    ├── task1.cpp    # 素数筛选
    ├── task2.cpp    # 学生成绩统计
    ├── task3.cpp    # 归一化
    ├── task4.cpp    # 坐标系旋转
    ├── task5.cpp    # 位域解析
    ├── task6.cpp    # 几何体多态
    └── task7.png    # RobotControl 代码思维导图
```

## 编译运行

每个 task 都自带 `main`，可单独编译：

```bash
g++ -std=c++20 -Wall -Wextra task1.cpp -o task1
./task1
```

## 说明

- 代码为新手版（simple 版）实现，无额外依赖。
- `task6` 需要两次输入：先输入立方体边长，再输入球半径。
- `task7.png` 为依据 RobotControl 源码的结构与逻辑绘制的思维导图。
