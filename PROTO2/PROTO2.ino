#define CAPACITIVE_PIN_FORWARD 3
#define CAPACITIVE_PIN_BACKWARDS 4

#define MODE_SWITCH_DELAY 1000
#define DOUBLE_PRESS_MILLISECOND_DELAY 1000

#include "led_functions.h"
#include "roku_ip_discovery.h"
#include "wifi_command_handler.h"

const char* ssid = "SSID_NAME";
const char* password = "SSID_PASSWORD";
String rokuHost = "";

// Previous button states
bool lastStateForward = LOW;
bool lastStateBackward = LOW;
bool lastStateToggle = LOW;

RokuCommandHandler* handler;

// Variables related to main loop
enum Mode {
  DIRECTIONAL,     // 0
  OK_BACK,         // 1
  VOL,             // 2
  NETFLIX_YOUTUBE  // 3 (Secret menu)
};
Mode currMode;
bool isPressed = false;  // This checks if any key is pressed
bool isFrontPressed = false;
bool isBackPressed = false;
bool hold = false;
bool lockSensors = false;  // This locks sensors until both are released
bool frontLocked = false;
bool backLocked = false;
unsigned long start, end;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Set pin modes
  for (int i = 0; i < sizeof(led_pins) / sizeof(led_pins[0]); i++) {
    pinMode(led_pins[i], OUTPUT);
  }
  pinMode(CAPACITIVE_PIN_FORWARD, INPUT);
  pinMode(CAPACITIVE_PIN_BACKWARDS, INPUT);

  // Connect ESP to WIFI
  connectToWiFi();

  RokuIpDiscoverer ip_disc;

  while (rokuHost == "") {
    rokuHost = ip_disc.findRokuIP();
  }

  Serial.println("Discovered Roku IP: " + rokuHost);

  handler = new RokuCommandHandler(&rokuHost);
  currMode = Mode::DIRECTIONAL;  // Set directional as the default mode

  flashAllLEDs(3);
  turnOnLeds(static_cast<int>(currMode));
}

void loop() {
  // Check which sensors are currently pressed
  bool frontPressed = checkCapacitiveTouch(CAPACITIVE_PIN_FORWARD);
  bool backPressed = checkCapacitiveTouch(CAPACITIVE_PIN_BACKWARDS);

  if (lockSensors) {
    if (!frontPressed && !backPressed) {
      // Unlock if both buttons are released
      lockSensors = false;
    }
    return;
  }

  // front will take precedence if both are pressed
  if (frontPressed && !backLocked) {
    frontLocked = true;
    if (!isPressed && !hold) {
      start = millis();
      isPressed = true;
    } else if (isPressed) {
      end = millis();

      if (end - start >= MODE_SWITCH_DELAY) {
        switchModes();
        isPressed = false;
        hold = true;
      }
    }
  } else if (backPressed && !frontLocked) {
    backLocked = true;
    if (!isPressed && !hold) {
      start = millis();
      isPressed = true;
    } else if (isPressed) {
      end = millis();

      if (end - start >= MODE_SWITCH_DELAY) {
        // Go to secret menu
        currMode = Mode::NETFLIX_YOUTUBE;
        turnOnLeds(4);
        Serial.println("Switched to secret Menu");
        isPressed = false;
        hold = true;
      }
    }
  } else {
    if (isPressed) {  // This will send the command once the button is released
      if (frontLocked) {
        handleKeyUp(handler);
      } else if (backLocked) {
        handleKeyDown(handler);
      }
    }

    // Release both buttons
    frontLocked = false;
    backLocked = false;
    isPressed = false;
    hold = false;
  }
  delay(50);
}

bool checkCapacitiveTouch(uint8_t capacitive_pin) {
  return digitalRead(capacitive_pin) == HIGH;
}

void connectToWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Flash leds on startup
  flashAllLEDs(2, 400);

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void switchModes() {
  // This will cycle through modes [0..2]
  // Secret mode 3 will not be reached through this
  if (currMode == Mode::NETFLIX_YOUTUBE){
    currMode = Mode::DIRECTIONAL;
  }
  else {
    currMode = static_cast<Mode>((currMode + 1) % 3);
  }
  Serial.println("Switched modes: " + String(currMode));
  lockSensors = true;
  turnOnLeds(static_cast<int>(currMode));
  delay(200);
}

void handleKeyUp(RokuCommandHandler* commandHandler) {
  if (currMode == Mode::DIRECTIONAL) {
    unsigned long start, end;
    start = millis();
    end = millis();

    int c = 1;  // Number of times up has been pressed;
    while (end - start <= DOUBLE_PRESS_MILLISECOND_DELAY) {
      end = millis();
      if (checkCapacitiveTouch(CAPACITIVE_PIN_FORWARD)) {
        c = 2;
        break;
      }
      delay(100);
    }

    if (c == 1) {
      commandHandler->sendRightCommand();
    } else {
      commandHandler->sendUpCommand();
    }
  } else if (currMode == Mode::OK_BACK) {
    commandHandler->sendOkCommand();
  } else if (currMode == Mode::VOL) {
    commandHandler->sendVolumeUp();
  } else if (currMode == Mode::NETFLIX_YOUTUBE) {
    commandHandler->sendNetflixCommand();
    switchModes();
  }

  lockSensors = true;
  frontLocked = false;
  delay(200);
}

void handleKeyDown(RokuCommandHandler* commandHandler) {
  if (currMode == Mode::DIRECTIONAL) {
    unsigned long start, end;
    start = millis();
    end = millis();

    int c = 1;  // Number of times up has been pressed;
    while (end - start < DOUBLE_PRESS_MILLISECOND_DELAY) {
      end = millis();
      if (checkCapacitiveTouch(CAPACITIVE_PIN_BACKWARDS)) {
        c = 2;
        break;
      }
      delay(100);
    }

    if (c == 1) {
      commandHandler->sendLeftCommand();
    } else {
      commandHandler->sendDownCommand();
    }
  } else if (currMode == Mode::OK_BACK) {
    unsigned long start, end;
    start = millis();
    end = millis();

    int c = 1;  // Number of times up has been pressed;
    while (end - start < DOUBLE_PRESS_MILLISECOND_DELAY) {
      end = millis();
      if (checkCapacitiveTouch(CAPACITIVE_PIN_BACKWARDS)) {
        c = 2;
        break;
      }
      delay(100);
    }

    if (c == 1) {
      commandHandler->sendBackCommand();
    } else {
      commandHandler->sendHomeCommand();
    }
  } else if (currMode == Mode::VOL) {
    commandHandler->sendVolumeDown();
  } else if (currMode == Mode::NETFLIX_YOUTUBE) {
    commandHandler->sendYouTubeCommand();
    switchModes();
  }

  lockSensors = true;
  backLocked = false;
  delay(200);
}
