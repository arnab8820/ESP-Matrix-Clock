#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "displayHandler.h"

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN 14  // or SCK
#define DATA_PIN 13 // or MOSI
#define CS_PIN 15   // or SS

MD_Parola displayPanel = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

int intensity = 0;
char text[100] = {""};
uint8_t effectSpeed = 50;
uint16_t effectPause = 0;
textPosition_t textAlign = PA_CENTER;
textEffect_t textEntryEffect = PA_SCROLL_UP;
textEffect_t textExitEffect = PA_SCROLL_UP;

void initDisplay()
{
    displayPanel.begin();
    displayPanel.setIntensity(intensity);
}

void displayTick()
{
    displayPanel.displayAnimate();
}

int getLight()
{
    return intensity;
}

void lightInc()
{
    intensity++;
    if (intensity > 16)
    {
        intensity = 0;
    }
    displayPanel.setIntensity(intensity);
}

void displayClean()
{
    displayPanel.displayClear();
}

void setDisplaySpacing(int space)
{
    displayPanel.setCharSpacing(space);
}

void flashText(String txt)
{
    displayPanel.print(txt.c_str());
    displayPanel.displayReset();
}

void setDisplayText(String disText){
    strcpy(text, disText.c_str());
    displayPanel.displayText(text, textAlign, effectSpeed, effectPause, textEntryEffect, textExitEffect);
    displayPanel.displayReset();
}

void setDisplayAnimation(textEffect_t entryEffect, textEffect_t exitEffect){
    textEntryEffect = entryEffect;
    textExitEffect = exitEffect;
}