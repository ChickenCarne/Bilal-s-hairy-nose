#include "main.h"
#include "pros/misc.h"
#include "subsystems.hpp"
#include "autons.hpp"
//#include "pros/controller.h" 


/////
// For installation, upgrading, documentations, and tutorials, check out our website!
// https://ez-robotics.github.io/EZ-Template/
/////

// Chassis constructor
ez::Drive chassis(
    //20 -17 -3 left 
    //18   -1  -2 right
    // These are your drive motors, the first motor is used for sensing!
    {-20, 17, -3},     // Left Chassis Ports (negative port will reverse it!)
    {18, -4, 5},  // Right Chassis Ports (negative port will reverse it!)
    6,      // IMU Port
    3.25,  // Wheel Diameter (Remember, 4" wheels without screw holes are actually 4.125!)
    360);   // Wheel RPM = cartridge * (motor gear / wheel gear)

// Uncomment the trackers you're using here!
// - `8` and `9` are smart ports (making these negative will reverse the sensor)
//  - you should get positive values on the encoders going FORWARD and RIGHT
// - `2.75` is the wheel diameter
// - `4.0` is the distance from the center of the wheel to the center of the robot
// ez::tracking_wheel horiz_tracker(8, 2.75, 4.0);  // This tracking wheel is perpendicular to the drive wheels
// ez::tracking_wheel vert_tracker(9, 2.75, 4.0);   // This tracking wheel is parallel to the drive wheels

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

//ez::PID liftPID(.5,.3,0,"functionality")

#include <iostream>



void intakeDriver(){
  int color = 0;
// COLORSORT
    // RED
    // Senses for blue rings to eject
  if (master.get_digital_new_press(DIGITAL_A)) {
    if (color == 0 && (vision.get_hue() <= 230 && vision.get_hue() >= 200) &&
        vision.get_proximity() <= 100) {
      // for proximity => need to tune cuz idk what unit it returns
      // if optical sensor is in a weird spot u might need a delay here
      intakeHook.move_voltage(0); // stops intake
      pros::delay(100);           // ensures the ring has time to fly
    }
    //  BLUE
    // Senses for red rings to eject
    else if (color == 1 && (vision.get_hue() <= 50 && vision.get_hue() >= 20) &&
             vision.get_proximity() <= 100) {
      // for proximity => need to tune cuz idk what unit it returns
      // if optical sensor is in a weird spot u might need a delay here

      intakeHook.move_voltage(0); // stops intake
      pros::delay(100);           // ensures the ring has time to fly
    } else {
      intake11W.move((master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)-master.get_digital(pros::E_CONTROLLER_DIGITAL_R2))*127); // Normal intake control
    }
  } else {
    intake11W.move((master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)-master.get_digital(pros::E_CONTROLLER_DIGITAL_R2))*127);
  }
}
const int numStates = 3;
int states[numStates] = {0, -17200, -68250};
int currState = 0;
int target = 0;

void load() {
  currState +=1;
  if (currState != 1) {
    currState = 1;
  }
  target = states[currState];
}
void nextState(){
  currState +=1;
  if (currState == 3) {
    currState = 0;
  }
  target = states[currState];
}

void next2State(){
  currState +=2;
  if (currState >= 3) {
    currState = 0;
  }
  target = states[currState];
}
void liftControl(){
  double kp = .005;
  double error = target - lbRot.get_position();
  double velocity = kp * error;
  lbArm.move(velocity);
}

void driverCon(){
  if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
    nextState();
  }
}

void initialize() {
  // Print our branding over your terminal :D
     pros::Task liftControlTask([]{
       lbRot.set_position(0);
         while (true) {
             liftControl();
             pros::delay(10);
         }
     });
  ez::ez_template_print();

  pros::delay(500);  // Stop the user from doing anything while legacy ports configure

  //fad


  // Look at your horizontal tracking wheel and decide if it's in front of the midline of your robot or behind it
  //  - change `back` to `front` if the tracking wheel is in front of the midline
  //  - ignore this if you aren't using a horizontal tracker
  // chassis.odom_tracker_back_set(&horiz_tracker);
  // Look at your vertical tracking wheel and decide if it's to the left or right of the center of the robot
  //  - change `left` to `right` if the tracking wheel is to the right of the centerline
  //  - ignore this if you aren't using a vertical tracker
  // chassis.odom_tracker_left_set(&vert_tracker);

  // Configure your chassis controls
  chassis.opcontrol_curve_buttons_toggle(true);   // Enables modifying the controller curve with buttons on the joysticks
  chassis.opcontrol_drive_activebrake_set(0.0);   // Sets the active brake kP. We recommend ~2.  0 will disable.
  chassis.opcontrol_curve_default_set(0.0, 0.0);  // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)

  // Set the drive to your own constants from autons.cpp!
  default_constants();

  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.opcontrol_curve_buttons_left_set(pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT);  // If using tank, only the left side is used.
  // chassis.opcontrol_curve_buttons_right_set(pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
    ez::as::auton_selector.autons_add({
    Auton("left two rings", left2),
    //Auton("right two rings", right2),
    //Auton("right red rush", rightRedRush),
    //Auton("left blue", leftBlue),
    //Auton("left blue", leftBlue),
    //Auton("left blue rush", leftBlueRush),
    //Auton("right blue", rightBlue)
    //Auton("skills", skills),

  });


  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();
  master.rumble(chassis.drive_imu_calibrated() ? "." : "---");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}

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
void autonomous() {
  chassis.pid_targets_reset();                // Resets PID targets to 0
  chassis.drive_imu_reset();                  // Reset gyro position to 0
  chassis.drive_sensor_reset();               // Reset drive sensors to 0
  chassis.odom_xyt_set(0_in, 0_in, 0_deg);    // Set the current position, you can start at a specific position with this
  chassis.drive_brake_set(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency

  /*
  Odometry and Pure Pursuit are not magic

  It is possible to get perfectly consistent results without tracking wheels,
  but it is also possible to have extremely inconsistent results without tracking wheels.
  When you don't use tracking wheels, you need to:
   - avoid wheel slip
   - avoid wheelies
   - avoid throwing momentum around (super harsh turns, like in the example below)
  You can do cool curved motions, but you have to give your robot the best chance
  to be consistent
  */

  ez::as::auton_selector.selected_auton_call();  // Calls selected auton from autonomous selector
}

/**
 * Simplifies printing tracker values to the brain screen
 */
void screen_print_tracker(ez::tracking_wheel *tracker, std::string name, int line) {
  std::string tracker_value = "", tracker_width = "";
  // Check if the tracker exists
  if (tracker != nullptr) {
    tracker_value = name + " tracker: " + util::to_string_with_precision(tracker->get());             // Make text for the tracker value
    tracker_width = "  width: " + util::to_string_with_precision(tracker->distance_to_center_get());  // Make text for the distance to center
  }
  ez::screen_print(tracker_value + tracker_width, line);  // Print final tracker text
}

/**
 * Ez screen task
 * Adding new pages here will let you view them during user control or autonomous
 * and will help you debug problems you're having
 */
void ez_screen_task() {
  while (true) {
    // Only run this when not connected to a competition switch
    if (!pros::competition::is_connected()) {
      // Blank page for odom debugging
      if (chassis.odom_enabled() && !chassis.pid_tuner_enabled()) {
        // If we're on the first blank page...
        if (ez::as::page_blank_is_on(0)) {
          // Display X, Y, and Theta
          ez::screen_print("x: " + util::to_string_with_precision(chassis.odom_x_get()) +
                               "\ny: " + util::to_string_with_precision(chassis.odom_y_get()) +
                               "\na: " + util::to_string_with_precision(chassis.odom_theta_get()),
                           1);  // Don't override the top Page line

          // Display all trackers that are being used
          screen_print_tracker(chassis.odom_tracker_left, "l", 4);
          screen_print_tracker(chassis.odom_tracker_right, "r", 5);
          screen_print_tracker(chassis.odom_tracker_back, "b", 6);
          screen_print_tracker(chassis.odom_tracker_front, "f", 7);
        }
      }
    }

    // Remove all blank pages when connected to a comp switch
    else {
      if (ez::as::page_blank_amount() > 0)
        ez::as::page_blank_remove_all();
    }

    pros::delay(ez::util::DELAY_TIME);
  }
}
pros::Task ezScreenTask(ez_screen_task);

/**
 * Gives you some extras to run in your opcontrol:
 * - run your autonomous routine in opcontrol by pressing DOWN and B
 *   - to prevent this from accidentally happening at a competition, this
 *     is only enabled when you're not connected to competition control.
 * - gives you a GUI to change your PID values live by pressing X
 */
void ez_template_extras() {
  // Only run this when not connected to a competition switch
  if (!pros::competition::is_connected()) {
    // PID Tuner
    // - after you find values that you're happy with, you'll have to set them in auton.cpp

    // Enable / Disable PID Tuner
    //  When enabled:
    //  * use A and Y to increment / decrement the constants
    //  * use the arrow keys to navigate the constants
    if (master.get_digital_new_press(DIGITAL_X))
      chassis.pid_tuner_toggle();

    // Trigger the selected autonomous routine
    if (master.get_digital(DIGITAL_B) && master.get_digital(DIGITAL_DOWN)) {
      pros::motor_brake_mode_e_t preference = chassis.drive_brake_get();
      autonomous();
      chassis.drive_brake_set(preference);
    }

    // Allow PID Tuner to iterate
    chassis.pid_tuner_iterate();
  }



  // Disable PID Tuner when connected to a comp switch
  else {
    if (chassis.pid_tuner_enabled())
      chassis.pid_tuner_disable();
  }
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
  // This is preference to what you like to drive on
  chassis.drive_brake_set(MOTOR_BRAKE_COAST);
  while(true) {
    // Gives you some extras to make EZ-Template ezier
    ez_template_extras();

    chassis.opcontrol_tank();  // Tank control
    //chassis.opcontrol_arcade_standard(ez::SPLIT);   // Standard split arcade
    // chassis.opcontrol_arcade_standard(ez::SINGLE);  // Standard single arcade
    // chassis.opcontrol_arcade_flipped(ez::SPLIT);    // Flipped split arcade
    // chassis.opcontrol_arcade_flipped(ez::SINGLE);   // Flipped single arcade

    // . . .
    // Put more user control code here!
    // . . .

    // if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)){
    //   setIntake(127);
    // }else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)){
    //   setIntake(-127);
    // }else{
    //   setIntake(0);
    // }

    
    //LB driver control
    // if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
    //    nextState(); 
    //  }
    intakeDriver();
    driverCon();
    
    clampbru.button_toggle(master.get_digital(DIGITAL_L2));
    //intake11W.move((master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)-master.get_digital(pros::E_CONTROLLER_DIGITAL_R2))*127);
    //secRing.button_toggle(master.get_digital(DIGITAL_L2));
    pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
