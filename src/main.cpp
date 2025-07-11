#include "main.h"
#include <numeric> //std::accumulate

pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::MotorGroup leftDrive({12, 3});
pros::MotorGroup rightDrive({-20, -2});

pros::MotorGroup claw({15}); // opens when fwd
pros::MotorGroup arm({11});

pros::MotorGroup bucket({13, -18});

void initialize() {}
void disabled() {}
void autonomous() {}

double vector_average(const std::vector<double>& v) {
	return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

void arcadeDrive(double turnScale) {
    double power = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    double rawTurn = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

    // move motors based on direction (eg move left more when turn is positive)
    leftDrive.move_voltage((power + rawTurn * turnScale) * (12000.0 / 127));
    rightDrive.move_voltage((power - rawTurn * turnScale) * (12000.0 / 127));
}

void PIDMove(pros::MotorGroup& mtr, int target, double velocity, int timeout) {
	mtr.move_absolute(target, velocity);
	int timer = 0;

	while (abs((int)mtr.get_position() - target) > 80 || timer < timeout) {
		timer += 10;
		pros::delay(10);
	}
	
	mtr.brake();
	mtr.move_voltage(0);
}

/* Macros */
bool bucketLock = false;
void liftBucket() {
	bucketLock = true;
	bucket.move_absolute(800, 90);
	pros::delay(1000); // wait for bucket to move
	bucket.move_absolute(-1200, 50);
	pros::delay(2000);
	bucket.move(0);
	bucketLock = false;
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	claw.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	bucket.set_brake_mode_all(pros::E_MOTOR_BRAKE_COAST);

	while (true) {
		// Arcade control scheme
		arcadeDrive(1);

		// claw
		if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) { // open claw
			claw.move_voltage(6000);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) { // close
			claw.move_voltage(-6000);
		} else {
			claw.brake();
		}

		// claw arm
		if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
			arm.move_voltage(4000);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
			arm.move_voltage(-4000);
		} else {
			arm.brake();
		}

		// bucket
		if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN) 
			&& controller.get_digital(pros::E_CONTROLLER_DIGITAL_B)
			&& !bucketLock) {
				pros::Task([] {
					liftBucket();
    			});
		}

		double drivetrainTemps = vector_average(leftDrive.get_temperature_all());
		controller.print(0, 0, "DT%.0lf | L%.0lf | R%.0lf  ", drivetrainTemps, bucket.get_temperature(0), bucket.get_temperature(1));
		pros::delay(20);
	}
}