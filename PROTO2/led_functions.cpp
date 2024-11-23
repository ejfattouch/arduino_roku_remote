#include "led_functions.h"

void flashAllLEDs(uint8_t n, long interval) {
  for (int i = 0; i < n; i++) {
    // Turn off all LEDs
    for (int j = 0; j < sizeof(led_pins) / sizeof(led_pins[0]); j++) {
      digitalWrite(led_pins[j], LOW);
    }
    delay(interval);  // Wait for 'interval' time

    // Turn on all LEDs
    for (int j = 0; j < sizeof(led_pins) / sizeof(led_pins[0]); j++) {
      digitalWrite(led_pins[j], HIGH);
    }
    delay(interval);  // Wait for 'interval' time
  }
}

void flashLED(uint8_t led_pin, uint8_t n, long interval) {
  for (int i = 0; i < n; i++) {
    digitalWrite(led_pin, LOW);
    delay(interval);  // Wait for 'interval' time
    digitalWrite(led_pin, HIGH);
    if (i != n - 1) {
      delay(interval);  // Wait for 'interval' time
    }
  }
}

void turnOnLeds(int mode) {
  if (mode >= 0 && mode <= 3) {
    for (int i = 0; i < 3; i++) {
      if (i != mode) {
        digitalWrite(led_pins[i], LOW);
        continue;
      }
      digitalWrite(led_pins[i], HIGH);
    }
  } else if (mode == 4) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(led_pins[i], HIGH);
    }
  }
}