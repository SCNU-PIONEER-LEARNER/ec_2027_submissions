#pragma once

namespace ROBOT {

enum class Action { Forward, Left, Right, Backward }; // 机器人运动方向枚举

class Robot {
public:
  Robot(int l, int f, int r) : left(l), front(f), right(r) {}

  Action action;
  void forward();
  void turnleft();
  void turnright();
  void backward(); // 控制机器人行动的函数

  void decide(); 
  void getData(); // 获取当前距离数据
  void update(); 

private:
  int left, front, right; // 左、前、右距离

  bool canForward = (front > 30);              
  bool isNarrow = (left < 15 || right < 15); 
};

} // namespace ROBOT