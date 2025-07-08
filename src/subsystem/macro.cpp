#include "main.h"

bool intakeLock = false;
bool colourSortToggle = true;
bool antiJamToggle = true;

ks::PIDConstants PIDconst(1.7, 0, 2);
ks::PID armPID(&PIDconst);
void liftControl(double target, int minSpeed) {
	armPID.reset();
	double error = target - arm.get_position() / 100.0;

	double timer = 0;
	arm.move(20);
	while (timer < 450) { 
    	error = target - arm.get_position() / 100.0;

		if (minSpeed != 0) { // force speed
			arm.move(minSpeed);
		} else { // use normal pid
    		arm.move(armPID.output(error));
		}

		if (fabs(error) < 3) {
			break;
		}

		timer += 10;
		pros::delay(10);
	}
	arm.brake();
}