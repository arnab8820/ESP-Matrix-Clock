#include <MD_Parola.h>
#include "displayHandler/displayHandler.h"
#include "timeHandler/timeHandler.h"

extern int hr, mint, dt, mnth, year;
String monthss[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

bool setupMode = false;
int setupState = 0;

void displaySetupState();

void incrementSetup(){
    setDisplayAnimation(PA_SCROLL_UP, PA_NO_EFFECT);
    if(!setupMode){
        setupMode = true;
        displaySetupState();
        return;
    }
    setupState++;
    displaySetupState();
    if(setupState>4){
        setDisplayText("Saved");
        setTime();
        setupMode = false;
        setupState = 0;
    }
}

void displaySetupState(){
    
    switch (setupState)
    {
        case 0:{
            int printHr = hr%12;
            if(printHr==0){
                printHr = 12;
            }
            setDisplayText(String(printHr, DEC)+": "+(hr>=12?"P":"A"));
            break;
        }
        case 1:{
            setDisplayText("M :"+(mint<10?("0"+String(mint, DEC)):String(mint, DEC)));
            break;
        }
        case 2: {
            setDisplayText(String(year, DEC));
            break;
        }
        
        case 3: {
            String month = monthss[mnth-1] + "";
            setDisplayText(month);
            break;
        }
        case 4: {
            setDisplayText(String(dt, DEC));
            break;
        }
        default:
            break;
    }
}

void incrementSetupVal(){
    if(!setupMode){
        setDisplayAnimation(PA_SCROLL_UP, PA_NO_EFFECT);
        displayClean();
        showTemp();
        return;
    }
    setDisplayAnimation(PA_NO_EFFECT, PA_NO_EFFECT);
    switch (setupState)
    {
        case 0:
            hr++;
            if(hr>23){
                hr=0;
            }
            break;
        case 1:
            mint++;
            if(mint>59){
                mint = 0;
            }
            break;
        case 2: 
            year++;
            if(year>2030){
                year = 2023;
            }
            break;
        case 3: 
            mnth++;
            if(mnth>12){
                mnth = 1;
            }
            break;
        case 4:
            dt++;
            if(mnth==1 || mnth==3 || mnth==5 || mnth==7 || mnth==8 || mnth == 10 || mnth==12){
                if(dt > 31){
                    dt = 1;
                }
            } else if(mnth==2){
                if(year%4==0){
                    if(dt>29){
                        dt = 1;
                    }
                } else {
                    if(dt>28){
                        dt = 1;
                    }
                }
            } else {
                if(dt>30){
                    dt = 1;
                }
            }
            break;
        default:
            break;
    }
    displaySetupState();
}