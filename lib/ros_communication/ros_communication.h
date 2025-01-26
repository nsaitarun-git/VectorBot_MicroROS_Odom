#ifndef ROS_PUB_SUB_H
#define ROS_PUB_SUB_H

#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <motor_control.h>

#include <geometry_msgs/msg/twist.h>
#include <std_msgs/msg/int16.h>
#include <nav_msgs/msg/odometry.h>

#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_WIFI)
#endif

extern const int FORWARD_DIR;
extern const int BACKWARD_DIR;
extern const int LEFT_DIR;
extern const int RIGHT_DIR;

extern volatile unsigned long enc_left;
extern volatile unsigned long enc_right;
extern int direction;

class RosCommunication
{
public:
    RosCommunication();

    void initialize();
    void pub_sub_define();
    static void cmd_vel_callback(const void *msg_recv);
    void start_msgs();
    void executors_start();

private:
    static float linear_vel;
    static float angular_vel;
};

#endif