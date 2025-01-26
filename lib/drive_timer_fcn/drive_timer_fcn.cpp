#include <drive_timer_fcn.h>

// Encoder pins
const int enc_left_pin = 13;  // Left Motor
const int enc_right_pin = 15; // Right Motor

// Encoder ticks ISR
volatile unsigned long enc_left = 0;
volatile unsigned long enc_right = 0;

// Encoder variables
unsigned long num_ticks_left = 0;
unsigned long num_ticks_right = 0;
int power_left = 0;
int power_right = 0;

// Used to determine which way to turn to adjust
unsigned long diff_left;
unsigned long diff_right;

// Previous encoder values
unsigned long enc_left_prev = 0;
unsigned long enc_right_prev = 0;

// Adjust motor power
const int mtr_offset = 5;
const int left_mtr_offset = -12;

// Auto-relod software timer
static TimerHandle_t drive_timer = NULL;

// ISR for left motor
void count_left()
{
    enc_left++;
}

// ISR for right motor
void count_right()
{
    enc_right++;
}

// Initialise encoders
void encoder_init(void)
{
    // Configure encoder pins
    pinMode(enc_left_pin, INPUT_PULLUP);
    pinMode(enc_right_pin, INPUT_PULLUP);

    // Set up interrupts
    attachInterrupt(digitalPinToInterrupt(enc_left_pin), count_left, CHANGE);
    attachInterrupt(digitalPinToInterrupt(enc_right_pin), count_right, CHANGE);

    // Set initial motor power
    power_left = mtr_power;
    power_right = mtr_power;

    // Reset encoder counts
    enc_left = 0;
    enc_right = 0;

    // Remember previous encoder counts
    enc_left_prev = enc_left;
    enc_right_prev = enc_right;
}

// Drive robot straight with motor feedback
void drive(int dir)
{
    // Sample number of encoder ticks
    num_ticks_left = enc_left;
    num_ticks_right = enc_right;

    power_left -= left_mtr_offset; // Reduce left motor power

    // Set direction (reduce left wheel power)
    if (dir == FORWARD_DIR)
    {
        forward(power_left, power_right);
    }
    else if (dir == BACKWARD_DIR)
    {
        back(power_left, power_right);
    }
    else if (dir == LEFT_DIR)
    {
        left(power_left, power_right);
    }
    else if (dir == RIGHT_DIR)
    {
        right(power_left, power_right);
    }

    // Number of ticks counted since last time
    diff_left = num_ticks_left - enc_left_prev;
    diff_right = num_ticks_right - enc_right_prev;

    // Store current tick counter for next time
    enc_left_prev = num_ticks_left;
    enc_right_prev = num_ticks_right;

    // If left is faster, slow it down and speed up right
    if (diff_left > diff_right)
    {
        power_left -= mtr_offset;
        power_right += mtr_offset;
    }

    // If right is faster, slow it down and speed up left
    if (diff_left < diff_right)
    {
        power_left += mtr_offset;
        power_right -= mtr_offset;
    }
}

// Software timer callback
void drive_timer_callback(TimerHandle_t xTimer)
{

    // Use flag to control motor direction
    if (direction == FORWARD_DIR)
    {
        drive(FORWARD_DIR);
    }
    else if (direction == BACKWARD_DIR)
    {
        drive(BACKWARD_DIR);
    }
    else if (direction == LEFT_DIR)
    {
        drive(LEFT_DIR);
    }
    else if (direction == RIGHT_DIR)
    {
        drive(RIGHT_DIR);
    }
    else
    {
        stop();
    }
}

// Create an auto-reload software timer to update motor speed
void create_timer(void)
{

    drive_timer = xTimerCreate(
        "Drive Straight Timer",  // Name of timer
        10 / portTICK_PERIOD_MS, // Period of timer (in ticks)
        pdTRUE,                  // Auto-reload
        (void *)1,               // Timer ID
        drive_timer_callback);   // Callback function

    // Check if timer was created
    if (drive_timer == NULL)
    {
        Serial.println("Could not create timer!");
    }

    xTimerStart(drive_timer, portMAX_DELAY); // Start timer
}