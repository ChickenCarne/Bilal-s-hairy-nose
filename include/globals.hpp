#include "main.h"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"

//motors --> these exists
extern pros::Motor intake5W;
extern pros::MotorGroup intake11W;
extern pros::Motor preRoller;
extern pros::Motor intakeHook;
extern pros::Motor lbArm;
extern pros::Controller controller; //or pros::Controller master; or similar
extern pros::Rotation lbRot;
extern pros::Optical vision;
inline ez::Piston secRing('B', false);
inline ez::Piston clampbru('A', false);