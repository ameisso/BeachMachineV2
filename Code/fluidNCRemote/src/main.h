#include <Arduino.h>
#include <elapsedMillis.h>
#include "DebugUtils.h"
#include <WiFi.h>

elapsedMillis debugTimer;
WiFiClient telnetClient;

const char *ssid = "Antoine_AP";
const char *password = "papepipopu";

IPAddress FluidNCIP(192, 168, 2, 126);

void initWiFi();
void connectTelnet();
void jog(int x, int y, int feedrate = 100);
void zero();
void playFile(); 