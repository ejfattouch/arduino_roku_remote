#ifndef LED_MANAGER_H
#define LED_MANAGER_H
#include <Arduino.h>

const int led_pins[] = { 9, 10, 11 };
// Flashes all leds an n number of times
void flashAllLEDs(uint8_t n, long interval = 200);
// Flashes specified led an n number of times
void flashLED(uint8_t led_pin, uint8_t n, long interval = 200);
// This turns on the led specified by the mode and turns off the others
// Mode: 0 is led0, 1 is led1, 2 is led2, 3 is all leds
void turnOnLeds(int mode);
void cycleAnimation(); // Status is element of [0..2]

#endif