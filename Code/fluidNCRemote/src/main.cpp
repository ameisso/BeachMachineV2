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

void connectTelnet()
{
    DEBUG_PRINT("Connecting telnet...");
    if (!telnetClient.connect(FluidNCIP, 23))
    {
        DEBUG_PRINTLN(" failed");
        delay(2000);
        return;
    }
    DEBUG_PRINTLN(" ok");
}

void readResponse(WiFiClient *client)
{
    unsigned long timeout = millis();
    while (client->available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            //  DEBUG_PRINTLN(">>> Client Timeout !");
            // client->stop();
            //  connectTelnet();
            return;
        }
    }
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

void zero()
{
    releaseAlarms();
    DEBUG_PRINTLN("zero ");
    telnetClient.print("G91\n");
    telnetClient.print("G92 X0 Y0 Z0\n");
}

void restartController()
{
    DEBUG_PRINTLN("reset ");
    telnetClient.print("$Bye\n");
    delay(7000);
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
        DEBUG_PRINTLN("play ");
    }

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
    Serial.begin(115200);
    delay(1000);
    initWiFi();
    connectTelnet();
    delay(1000);
    playFile(true);
}

void loop()
{
    readResponse(&telnetClient);
    if (debugTimer > 10000)
    {
        debugTimer = 0;
        // stopPlayback();
    }
}
