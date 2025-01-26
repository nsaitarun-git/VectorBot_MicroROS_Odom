#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <oled_display.h>
#include <motor_control.h>
#include <ros_communication.h>
#include <drive_timer_fcn.h>

// OLED display address
#define i2c_Address 0x3c

// Direction flags
const int FORWARD_DIR = 1;
const int BACKWARD_DIR = 2;
const int LEFT_DIR = 3;
const int RIGHT_DIR = 4;

// Globals
const int mtr_power = 110; // Motor speed (0-255)
int direction = 0;         // Control motor direction

// Auto-relod software timer
static TimerHandle_t drive_timer = NULL;

// ROS communication object
RosCommunication ros_com;

void setup()
{
  Serial.begin(115200);

  // Configure motor pins
  configMotorPins();

  // Initialise ROS subscriber node
  ros_com.initialize();
  ros_com.pub_sub_define();
  ros_com.executors_start();

  // Configure OLED display
  display.begin(i2c_Address, true);

  // Initialise encoders
  encoder_init();

  // Create and start timer
  create_timer();
}

// Main loop
void loop()
{
  // Start publishing and subscribing to topics
  ros_com.start_msgs();
}
