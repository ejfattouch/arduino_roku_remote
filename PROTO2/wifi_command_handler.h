#ifndef WIFI_COMMAND_HANDLER_H
#define WIFI_COMMAND_HANDLER_H

#include <Arduino.h>
#include <HTTPClient.h>

class RokuCommandHandler {
private:
  const String roku_host;  // IP address of the Roku device
  static const unsigned int port = 8060; // Roku default port

  // Function to send a command via HTTP
  void sendCommand(const String& command);

public:
  RokuCommandHandler(const String* ip) : roku_host(*ip) {}

  // Mode 0 directional
  void sendUpCommand() { sendCommand("/keypress/up"); }
  void sendDownCommand() { sendCommand("/keypress/down"); }
  void sendLeftCommand() { sendCommand("/keypress/left"); }
  void sendRightCommand() { sendCommand("/keypress/right"); }
  // Mode 2 ok_back
  void sendOkCommand() { sendCommand("/keypress/select"); }
  void sendBackCommand() { sendCommand("/keypress/back"); }
  void sendHomeCommand() { sendCommand("/keypress/home"); }
  // Mode 3 volume
  void sendVolumeUp() { sendCommand("/keypress/volumeup"); }
  void sendVolumeDown() { sendCommand("/keypress/volumedown"); }
  // Secret mode, Netflix + YT
  void sendNetflixCommand() { sendCommand("/launch/12"); }
  void sendYouTubeCommand() { sendCommand("/launch/837"); }
};

#endif