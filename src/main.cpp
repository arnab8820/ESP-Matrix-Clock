#include <Arduino.h>
#include <MD_Parola.h>
#include "otaHandler/OtaHandler.h"
#include "displayHandler/displayHandler.h"
#include "timeHandler/timeHandler.h"
#include "utils/buttonHandler.h"
#include <ESPWifiAssist.h>

#define WIFICONTIMEOUT 90

ESPWifiAssist wifi("Matrix Clock", "12345678");

void setup()
{
  Serial.begin(115200);
  initDisplay();
  setDisplaySpacing(2);
  flashText("WIFI");
  wifi.beginWifi();
  int retryCount = 0;
  while (!wifi.isConnected() && retryCount <= WIFICONTIMEOUT)
  {
    retryCount++;
    delay(500);
    Serial.print(".");
  }
  delay(500);
  flashText("OTA");
  initOta();
  delay(500);
  flashText("Time");
  initTime(wifi);
  delay(500);
  flashText("NTP");
  getNtpTime();
  delay(500);
  initButtons();
  displayClean();
}

void loop()
{
  handleOta();
  displayTick();
  btnTick();
  wifi.monitorWifiConnection();
}