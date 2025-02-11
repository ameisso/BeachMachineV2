#include <Arduino.h>
#include <elapsedMillis.h>
#include "DebugUtils.h"
#include <WiFi.h>
#include <Button2.h>
#include "pinUtils.h"

#define JOG_FEEDRATE 100

elapsedMillis debugTimer;
WiFiClient telnetClient;

const char *ssid = "Antoine_AP";
const char *password = "papepipopu";
String programName = "shortLoop.nc";
IPAddress FluidNCIP(192, 168, 2, 126);


Button2 upButton(UP_PIN, INPUT_PULLUP,true);
Button2 downButton(DOWN_PIN, INPUT_PULLUP,true);
Button2 leftButton(LEFT_PIN, INPUT_PULLUP,true);
Button2 rightButton(RIGHT_PIN, INPUT_PULLUP,true);

Button2 zeroButton(ZERO_PIN, INPUT_PULLUP,true);
Button2 playButton(PLAY_PIN, INPUT_PULLUP,true);


void initWiFi();
void initHardware();
void connectTelnet();
void jog(int x, int y, int feedrate = 100);
void zero();
void releaseAlarms();
void restartController();
void playFile(bool restart = true); 
void stopPlayback();

void pressedHandler(Button2 &b);
