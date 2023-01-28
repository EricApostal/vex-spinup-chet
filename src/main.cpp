#include "main.h"
#include <string> 
#include <sstream>

using namespace pros;
// using namespace pros::c;

Motor front_left_motor(20, E_MOTOR_GEARSET_06, 0, E_MOTOR_ENCODER_DEGREES);
Motor back_left_motor(19, E_MOTOR_GEARSET_06, 0, E_MOTOR_ENCODER_DEGREES);
Motor front_right_motor(2, E_MOTOR_GEARSET_06, 1, E_MOTOR_ENCODER_DEGREES);
Motor back_right_motor(6, E_MOTOR_GEARSET_06, 1, E_MOTOR_ENCODER_DEGREES);

Motor conveyer_motor(9, E_MOTOR_GEARSET_18, 1, E_MOTOR_ENCODER_DEGREES);

Motor flywheel_motor(5, E_MOTOR_GEARSET_06, 0, E_MOTOR_ENCODER_DEGREES);
Motor flywheel_motor_2(3, E_MOTOR_GEARSET_06, 1, E_MOTOR_ENCODER_DEGREES);

Motor pusher_motor(7, E_MOTOR_GEARSET_18, 0, E_MOTOR_ENCODER_DEGREES);

ADIAnalogIn inertial_sensor(2);
ADIAnalogIn roller_sensor(3);

Controller controller (E_CONTROLLER_MASTER);

bool intake_running = false;
bool flywheel_running = false;

bool pusher_moving_right = false;
bool pusher_moving_left = false;

bool is_pushing = false;
bool is_unwinding = false;

bool is_rotating_roller = false;

int flywheel_motor_speed = 110; // volt

bool is_auto = false;

void move(Controller& controller) {
    int left_y = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
    int left_x = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
    int right_x = controller.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);

    front_left_motor.move((left_y + left_x + right_x) / 1);
    back_left_motor.move((left_y - left_x + right_x) / 1);
    front_right_motor.move((left_y - left_x - right_x) / 1);
    back_right_motor.move((left_y + left_x - right_x) / 1);

	// screen::print(E_TEXT_LARGE_CENTER, 3, std::to_string(1).c_str() );

	if ( (left_y == 0) and (left_x == 0) and (right_x == 0) ) {
		front_left_motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
		back_left_motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
		front_right_motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
		back_right_motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);

		front_left_motor.brake();
		back_left_motor.brake();
		front_right_motor.brake();
		back_right_motor.brake();
	}
}

void fire_disk() {
    is_pushing = true;

	pusher_motor.move_absolute(75, 200); // Moves 100 units forward
	while (!(pusher_motor.get_position() > 70)) {
		// Continue running this loop as long as the motor is not within +-5 units of its goal
		pros::delay(2);
	}
	// delay(2);
	pusher_motor.move_absolute(0, 200);
	while ( !(pusher_motor.get_position() < 5) ) {
		// Continue running this loop as long as the motor is not within +-5 units of its goal
		pros::delay(2);
	}
	// delay(10);
    is_pushing = false;
}

void tick() {
    if (!is_auto) {
        move(controller);
    }

    if (intake_running || is_rotating_roller) {
        conveyer_motor.move(200);
    } else if (is_unwinding) {
        conveyer_motor.move(-200);
    } else {
        conveyer_motor.move(0);
    }


    if (controller.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)) {
        flywheel_running = !flywheel_running;
    }

    if (flywheel_running) {
        flywheel_motor.move(flywheel_motor_speed);
        flywheel_motor_2.move(flywheel_motor_speed);
    } else {
        flywheel_motor.move(0);
        flywheel_motor_2.move(0);
    }
        

    if (controller.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) {
        intake_running = !intake_running;
    }

    if (controller.get_digital(E_CONTROLLER_DIGITAL_R2)) {
        pros::delay(20);
		if (!is_pushing) {
			Task push_thread(fire_disk);
		}
    }

	pros::delay(20);
}

int main() {

    while(true) {
        tick();
    }
    return 0;
}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

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
	pros::delay(20);
	std::cout << "running op control";
	main();
}