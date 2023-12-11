#include <ESP8266WiFi.h>

#define WIFICONTIMEOUT 90

const char *ssid = "Arnab-Home";
const char *password = "arnabhome";

const char *apSsid = "Matrix Clock";
const char *apPasswd = "";

void startAp();

void initWifi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("Dot Matrix Clock");
    WiFi.begin(ssid, password);
    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount <= WIFICONTIMEOUT)
    {
        retryCount++;
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Failed to connect WiFi");
        startAp();
    }
}

void startAp()
{
    Serial.println();
    Serial.print("Configuring access point...");
    WiFi.softAP(apSsid, apPasswd);
    delay(1000);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

bool isWifiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}
