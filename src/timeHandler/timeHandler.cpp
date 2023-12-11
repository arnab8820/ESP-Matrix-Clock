#include "RTClib.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <MD_Parola.h>
#include "displayHandler/displayHandler.h"
#include "wifiHandler/wifiHandler.h"
#include "setup/setup.h"

extern bool setupMode;

// interrupt pin config
const byte rtcTimerIntPin = 12;

const long utcOffsetInSeconds = 19800;

RTC_DS3231 rtc;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

int hr = 0;
int mint = 0;
int sec = 0;
int dt = 1;
int mnth = 1;
int year = 2023;

String months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

bool dotState = true;
bool tempMode = false;
long long tempTimer = 0;

ICACHE_RAM_ATTR void keepTime(){
    if(tempMode && (millis() - tempTimer > 2000)){
        tempMode = false;
    }
    if(setupMode||tempMode) return;
    sec+=1;
    if(sec>59){
        sec = 0;
        mint++;
        setDisplayAnimation(PA_SCROLL_UP, PA_NO_EFFECT);
        int printHr = hr%12;
        if(printHr==0){
            printHr = 12;
        }
        String timeText = String(printHr, DEC) + (dotState?":":" ") + (mint<10?("0"+String(mint, DEC)):String(mint, DEC));
        setDisplayText(timeText);
    }
    if(mint>59){
        hr++;
        mint = 0;
    }
    if(hr>23){
        hr = 0;
    }
    if(sec!=0){
        setDisplayAnimation(PA_NO_EFFECT, PA_NO_EFFECT);
        int printHr = hr%12;
        if(printHr==0){
            printHr = 1;
        }
        String timeText = String(printHr, DEC) + (dotState?":":" ") + (mint<10?("0"+String(mint, DEC)):String(mint, DEC));
        setDisplayText(timeText);
        dotState = !dotState;
    }
    
}

bool getNtpTime();
void getRtcTime();

void initTime(){
    pinMode(rtcTimerIntPin, INPUT_PULLUP);
    // digitalWrite(rtcTimerIntPin, HIGH);
    if (! rtc.begin()) {
        flashText("RTC Err");
    } else {
        rtc.writeSqwPinMode (DS3231_SquareWave1Hz);
        // if(!getNtpTime()){
            getRtcTime();
        // }
        attachInterrupt(digitalPinToInterrupt(rtcTimerIntPin), keepTime, RISING);
    }
}

void setTime(){
    rtc.adjust(DateTime(year, mnth, dt, hr, mint, sec));
}

bool getNtpTime(){
    if(!isWifiConnected()) return false;
    int retryCount = 0;
    while(!timeClient.update() && retryCount <= 60){
        retryCount++;
        delay(1000);
    }
    if(retryCount<=60){
        time_t epochTime = timeClient.getEpochTime();
        struct tm *ptm = gmtime ((time_t *)&epochTime);

        hr = timeClient.getHours();
        mint = timeClient.getMinutes();
        sec = timeClient.getSeconds();
        dt = ptm->tm_mday;
        mnth = ptm->tm_mon+1;
        year = ptm->tm_year+1900;
        Serial.printf("%d-%d-%d %d:%d:%d", dt, mnth, year, hr, mint, sec);
        setTime();
        return true;
    }
    return false;
}

void getRtcTime(){
    DateTime now = rtc.now();
    year = now.year();
    mnth = now.month();
    dt = now.day();
    hr = now.hour();
    mint = now.minute();
    sec = now.second();
    Serial.println("Got RTC time");
}

void showTemp(){
    tempMode = true;
    tempTimer = millis();
    int temp = rtc.getTemperature();
    setDisplayText(String(temp, DEC)+" 'C");
}