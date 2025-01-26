#include <ros_communication.h>
#include <motor_control.h>
#include <oled_display.h>

// Globals
unsigned long time_offset = 0;
unsigned long prev_odom_update = 0;
unsigned long current_ticks_left = 0;
unsigned long current_ticks_right = 0;
unsigned long diff_ticks_left = 0;
unsigned long diff_ticks_right = 0;
unsigned long prev_ticks_left = 0;
unsigned long prev_ticks_right = 0;

// Message types
geometry_msgs__msg__Twist cmd_vel_msg;
std_msgs__msg__Int16 enc_ticks_msg;
nav_msgs__msg__Odometry odom_msg;

// Come variables
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

// Subscriber
rcl_subscription_t cmd_vel_sub;
rclc_executor_t executor_sub;

// Publisher
rcl_publisher_t lweel_pub;
rcl_publisher_t rweel_pub;
rclc_executor_t executor_pub;

// Odom publisher
rcl_publisher_t odom_publisher;

RosCommunication::RosCommunication()
{
}

// Initialisation (Wifi and node)
void RosCommunication::initialize()
{
    // Add Wifi (PC IP Address)
    IPAddress agent_ip(192, 168, 1, 184);
    size_t agent_port = 8888;

    // Wifi credentials
    char ssid[] = "CommunityFibre10Gb_57E28";
    char psk[] = "srinivas";

    // Set transport
    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
    delay(2000);

    allocator = rcl_get_default_allocator();

    // create init_options
    rclc_support_init(&support, 0, NULL, &allocator);

    // create node
    rclc_node_init_default(&node, "micro_ros_platformio_node", "", &support);
}

// Start executors for publishers and subscriber
void RosCommunication::executors_start()
{
    // create executor subscriber
    rclc_executor_init(&executor_sub, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor_sub, &cmd_vel_sub, &cmd_vel_msg, &RosCommunication::cmd_vel_callback, ON_NEW_DATA);

    // create executor lweel_pub
    rclc_executor_init(&executor_pub, &support.context, 1, &allocator);
    rclc_executor_add_timer(&executor_pub, &timer);
}

struct timespec get_time()
{
    struct timespec tp = {0};
    unsigned long current_time = millis() + time_offset;
    tp.tv_sec = current_time / 1000;
    tp.tv_nsec = (current_time % 1000) * 1000000;

    return tp;
}

// Publisher timer callback
void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    // Publish encoder ticks
    enc_ticks_msg.data = enc_left;
    rcl_publish(&lweel_pub, &enc_ticks_msg, NULL);

    enc_ticks_msg.data = enc_right;
    rcl_publish(&rweel_pub, &enc_ticks_msg, NULL);

    // Calculate new encoder ticks from last time
    current_ticks_left = enc_left;
    current_ticks_right = enc_right;

    diff_ticks_left = current_ticks_left - prev_ticks_left;
    diff_ticks_right = current_ticks_right - prev_ticks_right;

    prev_ticks_left = current_ticks_left;
    prev_ticks_right = current_ticks_right;

    // Calculate odometry with RPM
    float counts_per_rev = 40.0;
    float wheel_diameter = 65.0;  // in millimeters
    float wheel_seperation = 115; // in millimeters
    float wheel_circumference = PI * wheel_diameter;

    //  Calculate RPM
    float rpm_left = (diff_ticks_left / counts_per_rev) * 120.0;
    float rpm_right = (diff_ticks_right / counts_per_rev) * 120.0;

    float average_rps_x = ((float)(rpm_left + rpm_right));                              // Average RPM
    float linear_x = average_rps_x * wheel_circumference;                               // m/s
    float average_rps_a = ((float)(-rpm_left + rpm_right));                             // Average RPM// Average RPM
    float angular_z = (average_rps_a * wheel_circumference) / (wheel_seperation / 2.0); //  rad/s
    float linear_y = 0;

    unsigned long now = millis();
    float vel_dt = (now - prev_odom_update) / 1000.0;
    prev_odom_update = now;

    // Publish odom data
    odom_msg.pose = {linear_x, 0, 0};
    odom_msg.twist = {0, 0, angular_z};

    struct timespec time_stamp = get_time();
    odom_msg.header.stamp.sec = time_stamp.tv_sec;
    odom_msg.header.stamp.nanosec = time_stamp.tv_nsec;
    rcl_publish(&odom_publisher, &odom_msg, NULL);
}

// Define lweel_pub and subscriber
void RosCommunication::pub_sub_define()
{
    // create subscriber
    // subscribe to 'cmd_vel' topic
    rclc_subscription_init_default(
        &cmd_vel_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel");

    // create left wheel publisher
    rclc_publisher_init_default(
        &lweel_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int16),
        "lwheel");

    // create right wheel publisher
    rclc_publisher_init_default(
        &rweel_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int16),
        "rwheel");

    // create Odom publisher
    rclc_publisher_init_default(
        &odom_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        "odom/unfiltered");

    // create timer (publish period milli seconds)
    const unsigned int timer_timeout = 500;
    rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(timer_timeout),
        timer_callback);
}

// Subscriber callback
void RosCommunication::cmd_vel_callback(const void *msg_recv)
{
    const geometry_msgs__msg__Twist *recieved_data = (const geometry_msgs__msg__Twist *)msg_recv;

    // Save recieved data into fields
    float linear_vel = recieved_data->linear.x;
    float angular_vel = recieved_data->angular.z;

    // Debug
    Serial.print(linear_vel);
    Serial.print(" , ");
    Serial.println(angular_vel);

    // Move robot according to commands
    // Display direction arrows
    if (linear_vel > 0)
    {
        // Serial.println("forward");J
        direction = 1;
        displayArrow("forward");
    }
    else if (linear_vel < 0)
    {
        // Serial.println("backward");
        direction = 2;
        displayArrow("backward");
    }
    else if (angular_vel > 0)
    {
        // Serial.println("left");
        direction = 3;
        displayArrow("left");
    }
    else if (angular_vel < 0)
    {
        // Serial.println("right");
        direction = 4;
        displayArrow("right");
    }
    else
    {
        // Serial.println("stop");
        direction = 0;
        displayArrow("stop");
    }
}

// Start publishing and subscribing to topics
void RosCommunication::start_msgs()
{
    rclc_executor_spin_some(&executor_pub, RCL_MS_TO_NS(100));
    rclc_executor_spin_some(&executor_sub, RCL_MS_TO_NS(100));
}
