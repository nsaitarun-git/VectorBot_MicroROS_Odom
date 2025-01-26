#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <oled_display.h>
#include <motor_control.h>
#include <ros_communication.h>
#include <drive_timer_fcn.h>

// OLED display settings
#define i2c_Address 0x3c

// Motor speed
const int mtr_power = 100;

// Control motor direction
int direction = 0;

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
