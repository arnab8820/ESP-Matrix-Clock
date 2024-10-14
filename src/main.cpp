#include <Arduino.h>
#include <MD_Parola.h>
#include "wifiHandler/wifiHandler.h"
#include "otaHandler/OtaHandler.h"
#include "displayHandler/displayHandler.h"
#include "timeHandler/timeHandler.h"
#include "utils/buttonHandler.h"

void setup()
{
  Serial.begin(115200);
  initDisplay();
  setDisplaySpacing(2);
  flashText("WIFI");
  initWifi();
  delay(500);
  flashText("OTA");
  initOta();
  delay(500);
  flashText("Time");
  initTime();
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
}