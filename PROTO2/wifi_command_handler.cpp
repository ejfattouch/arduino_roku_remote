#include "wifi_command_handler.h"

void RokuCommandHandler::sendCommand(const String& command){
  HTTPClient http;
  String url = "http://" + roku_host + ":" + String(port) + command;

  // Initiate the HTTP POST request
  http.begin(url);
  http.POST("");
  http.end();

  Serial.println("HTTP Post Sent: " + url);
}