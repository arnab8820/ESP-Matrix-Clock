#include "setup/setup.h"
#include <ESPWifiAssist.h>
#include "RTClib.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <MD_Parola.h>
#include "displayHandler/displayHandler.h"

void initTime(ESPWifiAssist &_wifi);
void setTime();
bool getNtpTime();
void getRtcTime();
void showTemp();