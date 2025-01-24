#include "main.h"
#include "pros/abstract_motor.hpp"

//motors 
pros::Motor intake5W(20, pros::v5::MotorGears::blue);
pros::MotorGroup intake11W({8,14},pros::v5::MotorGears::blue);
pros::Motor lbArm(-19, pros::v5::MotorGears::blue);
pros::Rotation lbRot(2);
pros::Controller master(pros::E_CONTROLLER_MASTER);

