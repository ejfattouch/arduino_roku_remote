#ifndef ROKU_IP_DISCOVERY_H
#define ROKU_IP_DISCOVERY_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class RokuIpDiscoverer {
private:
  // SSDP multicast address and port
  const char* MULTICAST_ADDRESS = "239.255.255.250";
  const unsigned int MULTICAST_PORT = 1900;
  WiFiUDP udp;
  String rokuIP;

  void sendSSDPRequest();
  bool listenForResponse(unsigned long timeout);

public:
  RokuIpDiscoverer(){}
  String findRokuIP();
};

#endif