#pragma once
#include "main.h"

inline pros::Controller controller(pros::E_CONTROLLER_MASTER);
inline pros::MotorGroup leftDrive({12, 3});
inline pros::MotorGroup rightDrive({-20, -2});

inline pros::MotorGroup claw({15}); // opens when fwd
inline pros::MotorGroup arm({11});

inline pros::MotorGroup bucket({13, -18});