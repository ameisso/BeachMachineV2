#include "main.h"

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void initHardware()
{
    DEBUG_PRINT("init hardware ...");

    pinMode(POT_2_PIN, INPUT);
    upButton.begin(UP_PIN, INPUT_PULLUP, true);
    downButton.begin(DOWN_PIN, INPUT_PULLUP, true);
    leftButton.begin(LEFT_PIN, INPUT_PULLUP, true);
    rightButton.begin(RIGHT_PIN, INPUT_PULLUP, true);
    zeroButton.begin(ZERO_PIN, INPUT_PULLUP, true);
    playButton.begin(PLAY_PIN, INPUT_PULLUP, true);

    upButton.setPressedHandler(pressedHandler);
    downButton.setPressedHandler(pressedHandler);
    leftButton.setPressedHandler(pressedHandler);
    rightButton.setPressedHandler(pressedHandler);
    zeroButton.setPressedHandler(pressedHandler);
    playButton.setPressedHandler(pressedHandler);
    DEBUG_PRINTLN("DONE");
}

void connectTelnet()
{
    DEBUG_PRINT("Connecting telnet...");
    if (!telnetClient.connect(FluidNCIP, 23))
    {
        DEBUG_PRINTLN(" failed");
        delay(2000);
        return;
    }
    DEBUG_PRINTLN("ok");
}

void readResponse(WiFiClient *client)
{
    unsigned long timeout = millis();
    while (client->available())
    {
        String line = client->readStringUntil('\r');
        if (line.indexOf("ok") >= 0)
        {
            // DEBUG_PRINTLN_PARAM("OK LINE ", line);
        }
        else if (line.length() <= 1)
        {
            // DEBUG_PRINTLN_PARAM("SHORT LINE ", line);
        }
        else if (line.startsWith("[MSG:INFO: MSG,PROGRAM DONE]"))
        {
            DEBUG_PRINTLN("PROGRAM LOOP");
            delay(1000);
            playFile(false);
        }
        else
        {
           // DEBUG_PRINT(">>> ");
            DEBUG_PRINTLN(line);
        }
    }
}

void jog(int x, int y, int feedrate)
{
    if ((WiFi.status() != WL_CONNECTED))
    {
        initWiFi();
    }
    releaseAlarms();
   // DEBUG_PRINT("jog ");
    DEBUG_PRINT(x);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN(y);

    telnetClient.print("$J=G91 G21");
    if (x != 0)
    {
        telnetClient.print(" X");
        telnetClient.print(x);
    }
    else if (y != 0)
    {
        telnetClient.print(" Y");
        telnetClient.print(y);
    }
    telnetClient.print(" F");
    telnetClient.print(feedrate);
    telnetClient.print("\n");
}

void pressedHandler(Button2 &b)
{
    int distance = map(analogRead(POT_2_PIN), 4095, 0, 0, DISTANCE_RANGE);
    // DEBUG_PRINTLN("\n____");
    //  DEBUG_PRINTLN_PARAM("RAW distance", analogRead(POT_2_PIN));
    // DEBUG_PRINTLN_PARAM("distances", distance);

    // DEBUG_PRINT(b.getPin());
    // DEBUG_PRINTLN(" pressed");
    switch (b.getPin())
    {
    case UP_PIN:
        DEBUG_PRINTLN("jog up");
        jog(0, distance, JOG_FEEDRATE);
        break;
    case DOWN_PIN:
        DEBUG_PRINTLN("jog down");
        jog(0, -distance, JOG_FEEDRATE);
        break;
    case LEFT_PIN:
        DEBUG_PRINTLN("jog left");
        jog(-distance, 0, JOG_FEEDRATE);
        break;
    case RIGHT_PIN:
        DEBUG_PRINTLN("jog right");
        jog(distance, 0, JOG_FEEDRATE);
        break;
    case ZERO_PIN:
        zero();
        break;
    case PLAY_PIN:
        playFile(true);
        break;
    }
}

void zero()
{
    releaseAlarms();
    DEBUG_PRINTLN("zero ");
    telnetClient.print("G91\n");
    telnetClient.print("G92 X0 Y0 Z0\n");
}

void restartController()
{
    DEBUG_PRINTLN("restart FluidNC...");
    telnetClient.print("$Bye\n");
    delay(7000);
    initWiFi();
    connectTelnet();
}

void releaseAlarms()
{
    telnetClient.print("~\n");
    telnetClient.print("$X\n");
}

void playFile(bool restart)
{
    releaseAlarms();
    if (restart)
    {
        restartController();
    }
    DEBUG_PRINT("play ");
    DEBUG_PRINTLN(programName);
    telnetClient.print("$LocalFS/Run=");
    telnetClient.print(programName);
    telnetClient.print("\n");
}

void stopPlayback()
{
    DEBUG_PRINTLN("stop playback");
    telnetClient.print("!\n");
}

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif
    delay(1000);
    initWiFi();
    connectTelnet();
    initHardware();
    delay(1000);
    //  playFile(true);
}

void loop()
{
    upButton.loop();
    downButton.loop();
    leftButton.loop();
    rightButton.loop();
    zeroButton.loop();
    playButton.loop();

    if (debugTimer > 1000)
    {
        debugTimer = 0;
        // stopPlayback();
    }
    readResponse(&telnetClient);
}
