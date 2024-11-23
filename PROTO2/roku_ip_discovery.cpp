#include "roku_ip_discovery.h"

void RokuIpDiscoverer::sendSSDPRequest() {
  String request =
    "M-SEARCH * HTTP/1.1\r\n"
    "Host: 239.255.255.250:1900\r\n"
    "Man: \"ssdp:discover\"\r\n"
    "ST: roku:ecp\r\n"
    "MX: 3\r\n\r\n";

  udp.beginPacket(MULTICAST_ADDRESS, MULTICAST_PORT);
  udp.print(request);
  udp.endPacket();

  Serial.println("SSDP request sent.");
}

bool RokuIpDiscoverer::listenForResponse(unsigned long timeout) {
  char response[1024];
  unsigned long startTime = millis();

  while (millis() - startTime < timeout) {
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
      int len = udp.read(response, sizeof(response) - 1);
      if (len > 0) {
        response[len] = '\0';  // Null-terminate the response
        String responseStr = String(response);

        int locationPos = responseStr.indexOf("LOCATION: ");
        if (locationPos != -1) {
          int httpPos = responseStr.indexOf("http://", locationPos);
          int endPos = responseStr.indexOf(":", httpPos + 7);
          rokuIP = responseStr.substring(httpPos + 7, endPos);
          return true;
        }
      }
    }

    delay(100);  // Avoid busy-waiting
  }

  return false;
}


String RokuIpDiscoverer::findRokuIP() {
  IPAddress multicastAddress;

  if (!WiFi.hostByName(MULTICAST_ADDRESS, multicastAddress)) {
    Serial.println("Failed to resolve multicast address.");
    return "";
  }

  if (!udp.beginMulticast(multicastAddress, MULTICAST_PORT)) {
    Serial.println("Failed to join multicast group.");
    return "";
  }

  sendSSDPRequest();

  rokuIP = "";

  int retries = 10;
  while (retries-- > 0) {
    if (listenForResponse(5000)) {
      break;
    }
    Serial.println("No Roku found, trying again...");
    sendSSDPRequest();
  }

  udp.stop();
  return rokuIP;
}
