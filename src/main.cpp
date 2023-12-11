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
  flashText("OTA");
  initOta();
  flashText("Time");
  initTime();
  initButtons();
  displayClean();
}

void loop()
{
  handleOta();
  displayTick();
  btnTick();
}