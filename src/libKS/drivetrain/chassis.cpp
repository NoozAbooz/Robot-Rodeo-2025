#include "main.h"

void ks::moveRaw(int voltage, int time) {
	leftDrive.move_voltage(voltage);
	rightDrive.move_voltage(voltage);

	pros::delay(time);

	leftDrive.move_voltage(0);
	rightDrive.move_voltage(0);
}

// opcontrol
double ks::driveCurve(double input, double curve) {
    return (std::pow(2.718, -(curve / 10)) + std::pow(2.718, (std::fabs(input) - 127) / 10) * (1 - std::pow(2.718, -(curve / 10)))) * input;
}

void ks::arcadeDrive(int linCurve, int rotCurve, double turnScale) {
    double power = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    double rawTurn = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

    // move motors based on direction (eg move left more when turn is positive)
    if (!bucketLock) {
        leftDrive.move_voltage((power + rawTurn * turnScale) * (12000.0 / 127));
        rightDrive.move_voltage((power - rawTurn * turnScale) * (12000.0 / 127));
    }
}