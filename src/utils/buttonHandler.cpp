#include <Arduino.h>
#include "setup/setup.h"

// interrupt pin config
const byte btn1IntPin = 0;
const byte btn2IntPin = 2;

bool btnEnabled = true;
long long btnTimer = 0;
long debounceTime = 200;

ICACHE_RAM_ATTR void handleBtn1(){
    if(!btnEnabled) return;
    btnEnabled = false;
    btnTimer = millis();
    incrementSetup();
}

ICACHE_RAM_ATTR void handleBtn2(){
    if(!btnEnabled) return;
    btnEnabled = false;
    btnTimer = millis();
    incrementSetupVal();
}

void initButtons(){
    pinMode(btn1IntPin, INPUT_PULLUP);
    digitalWrite(btn1IntPin, HIGH);
    pinMode(btn2IntPin, INPUT_PULLUP);
    digitalWrite(btn2IntPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(btn1IntPin), handleBtn1, FALLING);
    attachInterrupt(digitalPinToInterrupt(btn2IntPin), handleBtn2, FALLING);
}

void btnTick(){
    if((millis() - btnTimer >= debounceTime)&&!btnEnabled){
        btnEnabled = true;
    }
}