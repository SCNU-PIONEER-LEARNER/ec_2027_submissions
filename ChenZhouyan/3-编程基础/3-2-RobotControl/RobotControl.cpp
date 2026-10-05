#include "RobotControl.hpp"

using namespace ROBOT;
using namespace std;

void Robot::decide() {
  if (canForward) {
    action = Action::Forward;
  } else {
    if (left > right)
      action = Action::Left;
    else if (right > left)
      action = Action::Right;
    else
      action = Action::Backward;
  }
  if (isNarrow) {
    action = Action::Backward;
  }
}

void Robot::update() {
  switch (action) {
  case Action::Forward:
    forward();
    break;
  case Action::Left:
    turnleft();
    break;
  case Action::Right:
    turnright();
    break;
  case Action::Backward:
    backward();
    break;
  }
}

int main() {
  Robot robot(10, 10, 10); // 实例化机器人，传入初始距离

  while (true) {
    robot.getData();
    robot.update();
  }
  return 0;
}