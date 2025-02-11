#include <Arduino.h>
#include <elapsedMillis.h>
#include "DebugUtils.h"
#include <WiFi.h>

elapsedMillis debugTimer;
WiFiClient telnetClient;

const char *ssid = "Antoine_AP";
const char *password = "papepipopu";
String programName = "shortLoop.nc";
IPAddress FluidNCIP(192, 168, 2, 126);

void initWiFi();
void connectTelnet();
void jog(int x, int y, int feedrate = 100);
void zero();
void releaseAlarms();
void restartController();
void playFile(bool restart = true); 
void stopPlayback();
