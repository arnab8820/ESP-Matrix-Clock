#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include "file-manager.h"

class ESPWifiAssist
{
private:
    char ssid[64] = "";
    char password[64] = "";

    String apSsid = "ESP-wifi-assist";
    String apPassword = "";

    bool wifiConnecting = false;
    long connectionStartTime = 0;

    const byte DNS_PORT = 53; // DNS port number
    DNSServer dnsServer;

    ESP8266WebServer* webServer;
    
    void connectToWiFi();
    void startAp();
    void initWebServer();
    void setWifiCredentials(const char *inputSsid, const char *inputPassword);

public:
    ESPWifiAssist(String _apSsid, String _apPassword);
    ESPWifiAssist(String _apSsid, String _apPassword, ESP8266WebServer& _server);
    void beginWifi();
    void monitorWifiConnection();
    bool isConnected();
};
