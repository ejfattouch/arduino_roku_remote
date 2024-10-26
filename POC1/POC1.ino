#define BUTTON_PIN_FORWARD 2
#define BUTTON_PIN_TOGGLE 3
#define BUTTON_PIN_BACKWARD 4

const int led_pins[] = { 8, 9, 11 };

int mode;  // 0 is green, 1 is white, 2 is red

const long interval = 200;  // interval at which to blink LED

struct BlinkState {
  uint8_t ledPin;
  uint8_t blinkCount;
  uint8_t currentBlink;
  unsigned long previousMillis;
  bool isOn;
  bool active;  // Indicates if flashing is currently active
};

BlinkState blinkState = { 0, 0, 0, 0, false, false };

// Previous button states
bool lastStateForward = LOW;
bool lastStateBackward = LOW;
bool lastStateToggle = LOW;

void setup() {
  // Set pin modes
  for (int i = 0; i < sizeof(led_pins) / sizeof(led_pins[0]); i++) {
    pinMode(led_pins[i], OUTPUT);
  }
  pinMode(BUTTON_PIN_FORWARD, INPUT);
  pinMode(BUTTON_PIN_BACKWARD, INPUT);
  pinMode(BUTTON_PIN_TOGGLE, INPUT);

  mode = 0;

  // Flash leds on startup
  flashAllLEDs(2, interval * 2);
}

void loop() {
  toggleModes();

  directionalBtnPress(BUTTON_PIN_FORWARD, lastStateForward);
  directionalBtnPress(BUTTON_PIN_BACKWARD, lastStateBackward);

  // Turn off all LEDs except the current one
  for (int i = 0; i < sizeof(led_pins) / sizeof(led_pins[0]); i++) {
    if (i != mode) {
      digitalWrite(led_pins[i], LOW);
    }
  }

  updateLED();
}

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

void startFlashing(uint8_t led_pin, uint8_t n) {
  blinkState.ledPin = led_pin;
  blinkState.blinkCount = n;
  blinkState.currentBlink = 0;
  blinkState.previousMillis = millis();
  blinkState.isOn = false;
  blinkState.active = true;  // Set active to true to start flashing
}

// this function will be called every loop() itiration to verify the status of the LED and check if it needs to be updated or not.
void updateLED() {
  // Handle LED flashing
  if (blinkState.active) {
    unsigned long currentMillis = millis();
    if (currentMillis - blinkState.previousMillis >= interval) {
      // Save the last time LED was updated
      blinkState.previousMillis = currentMillis;

      // Toggle LED state
      blinkState.isOn = !blinkState.isOn;
      digitalWrite(blinkState.ledPin, blinkState.isOn ? HIGH : LOW);

      // Check if we need to stop flashing
      if (blinkState.isOn && blinkState.currentBlink <= blinkState.blinkCount) {
        blinkState.currentBlink++;
      } else if (!blinkState.isOn && blinkState.currentBlink > blinkState.blinkCount) {
        // Reset after finishing the flash
        blinkState.active = false;
        digitalWrite(blinkState.ledPin, HIGH);  // Ensure LED is turned on at the end
      }
    }
  }
}

void toggleModes() {
  if (checkBtn(BUTTON_PIN_TOGGLE) && lastStateToggle == LOW) {
    mode = ++mode % 3;  // The magic number 3 is the number of modes
    delay(200);
    startFlashing(led_pins[mode], 2); // Setting to 2 will flash LED 3 times because the state of the next LED will be off by default.
  }
  // Update last state
  lastStateToggle = checkBtn(BUTTON_PIN_TOGGLE);
}

bool checkBtn(uint8_t btn_pin) {
  return digitalRead(btn_pin) == HIGH;
}

void directionalBtnPress(uint8_t pin, bool &lastState) {
  bool currentState = checkBtn(pin);

  if (currentState && lastState == LOW) {             // Check for rising edge
    uint8_t n = (pin == BUTTON_PIN_FORWARD) ? 1 : 2;  // If forward is pressed flash LED once, if backward flash twice
    startFlashing(led_pins[mode], n);
  }

  // Update last state
  lastState = currentState;
}
