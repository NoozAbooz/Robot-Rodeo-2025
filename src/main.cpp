#include "main.h"

bool bucketLock = false;
bool armLock = false;

void liftBucket() {
	bucketLock = true;
	armLock = true;

	// drive closer into hopper
	claw.move_voltage(-3000);
	liftControl(20);
	pros::delay(200);

	leftDrive.move_voltage(2000);
	rightDrive.move_voltage(2000);

	// flip (nb)
	bucket.move_absolute(800, 90);
	pros::delay(1000); // wait for bucket to move

	// drive away
	leftDrive.move_voltage(-2000);
	rightDrive.move_voltage(-2000);
	pros::delay(100);

	//bucket down (b)
	pros::Task([] {
		bucket.move_absolute(-1200, 30);
		pros::delay(2000);
		bucket.move(0);
		bucket.brake();
		bucket.set_brake_mode_all(pros::E_MOTOR_BRAKE_COAST);
    });

	arm.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	arm.move_voltage(-8000);
	pros::delay(200);

	// open claw
	claw.move_voltage(12000);
	pros::delay(200);
	claw.brake();
	arm.move_voltage(0);

	arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	bucketLock = false;
	armLock = false;
}

void liftArm() {
	armLock = true;

	// close claw and move arm up
	claw.move_voltage(-3000);
	liftControl(50);
	pros::delay(150);

	// open claw
	claw.move_voltage(6000);
	pros::delay(400);

	// close claw
	claw.move_voltage(-8000);
	pros::delay(50);

	// lift down
	arm.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	arm.move_voltage(-8000);
	pros::delay(200);

	// open claw
	claw.move_voltage(12000);
	pros::delay(200);
	claw.brake();
	arm.move_voltage(0);

	arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	armLock = false;
}

void opcontrol() {
	claw.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	bucket.set_brake_mode_all(pros::E_MOTOR_BRAKE_COAST);

	arm.tare_position();
	bucket.tare_position();

	//bucket.set_encoder_units_all(pros::E_MOTOR_ENCODER_DEGREES);
	arm.set_encoder_units_all(pros::E_MOTOR_ENCODER_DEGREES);

	while (true) { // Main continuous loop
		/* Drive */
		ks::arcadeDrive(0, 0, 1);

		/* Subsystem Listeners */
		// claw
		if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) { // open claw
			claw.move_voltage(8000);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) { // close
			claw.move_voltage(-4000);
		} else if (armLock == false) {
			claw.brake();
		}

		// claw arm
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && armLock == false) {
			pros::Task([] {
				liftArm();
    		});
		} else if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
			arm.move_voltage(5000);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
			arm.move_voltage(-5000);
		} else if (armLock == false) {
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

		double drivetrainTemps = ks::vector_average(leftDrive.get_temperature_all());
		controller.print(0, 0, "DT%.0lf|L%.0lf|R%.0lf     ", drivetrainTemps, bucket.get_temperature(0), bucket.get_temperature(1));
		pros::delay(20);
	}
}