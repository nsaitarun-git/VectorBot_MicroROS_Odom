#ifndef OLED_ARROWS_H
#define OLED_ARROWS_H
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Direction flags
extern const int FORWARD_DIR;
extern const int BACKWARD_DIR;
extern const int LEFT_DIR;
extern const int RIGHT_DIR;

// Display object
extern Adafruit_SH1106G display;

void displayArrow(const int request);
#endif