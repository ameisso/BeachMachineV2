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
    // while (client->available() == 0)
    // {
    //     if (millis() - timeout > 5000)
    //     {
    //         //  DEBUG_PRINTLN(">>> Client Timeout !");
    //         // client->stop();
    //         //  connectTelnet();
    //         return;
    //     }
    // }
    while (client->available())
    {
        String line = client->readStringUntil('\r');
        if (line.startsWith("ok"))
        {
            //   DEBUG_PRINTLN(line);
        }
        else if (line.startsWith("[MSG:INFO: MSG,PROGRAM DONE]"))
        {
            DEBUG_PRINTLN("PROGRAM LOOP");
            delay(1000);
            playFile(false);
        }
        else
        {
            DEBUG_PRINT(line);
        }
    }
}

void jog(int x, int y, int feedrate)
{
    releaseAlarms();
    DEBUG_PRINT("jog ");
    DEBUG_PRINT(x);
    DEBUG_PRINT(" ");
    DEBUG_PRINT(y);

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
    DEBUG_PRINTLN("\n____");
    DEBUG_PRINTLN_PARAM("RAW distance", analogRead(POT_2_PIN));
    DEBUG_PRINTLN_PARAM("distances", distance);

    DEBUG_PRINT(b.getPin());
    DEBUG_PRINTLN(" pressed");
    switch (b.getPin())
    {
    case UP_PIN:
        jog(0, distance, JOG_FEEDRATE);
        DEBUG_PRINTLN("jog up");
        break;
    case DOWN_PIN:
        jog(0, -distance, JOG_FEEDRATE);
        DEBUG_PRINTLN("jog down");
        break;
    case LEFT_PIN:
        jog(-distance, 0, JOG_FEEDRATE);
        DEBUG_PRINTLN("jog left");
        break;
    case RIGHT_PIN:
        jog(distance, 0, JOG_FEEDRATE);
        DEBUG_PRINTLN("jog right");
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
