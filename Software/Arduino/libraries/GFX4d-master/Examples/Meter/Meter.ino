//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//
/**************************************************************
* Meter                                                       *
* A Demo to show the use of gfx.UserImagesDR to speed up      *
* UserImages drawing by only drawing the part of the images   *
* that will have changed. The code is larger but the increase *
* in speed is up to 200%                                      *
* Files in the SDCardFiles folder need to be copied on to the *
* IoD SD card.                                                *
* This project can also be opened in Workshop 4.              *
**************************************************************/

#include "ESP8266WiFi.h"
#include "GFX4d.h"

GFX4d gfx = GFX4d();

#include "DRtest2Const.h"    // Note. This file will not be created if there are no generated graphics

int value = 0;

int vdir = 0;
int buttw;
int but;
int lastbut;

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.Open4dGFX("DRTEST~1"); // Opens DAT and GCI files for read using filename without extension. Note! Workshop generates files with Short filenames
  gfx.UserImages(iAngularmeter1,0) ;                         // Angularmeter1 show initialy, if required
  gfx.UserImages(iGauge1,0) ;  // Gauge1 show initialy, if required
  gfx.UserImage(iLeddigits1);  // Leddigits1 show all digits at 0, only do this once
  gfx.Buttonx(33, 14, 282, 64, 22, SILVER, "START", 1, BLACK);
  gfx.Buttonx(34, 86, 282, 64, 22, SILVER, "STOP", 1, BLACK);
  gfx.Buttonx(35, 158, 282, 64, 22, SILVER, "REVERSE", 1, BLACK);
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  if(value > -1 && value < 19){
  gfx.UserImagesDR(iAngularmeter1, value, 50, 98, 76, 60) ; // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if(value > 18 && value < 51){
  gfx.UserImagesDR(iAngularmeter1, value, 50, 32, 76, 78) ; // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if(value > 50 && value < 82){
  gfx.UserImagesDR(iAngularmeter1, value, 114, 32, 76, 78) ; // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if(value > 81 && value < 101){
  gfx.UserImagesDR(iAngularmeter1, value, 114, 98, 76, 60) ; // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  float intv = (213/100) * value;
  if(vdir == 1){
  gfx.UserImagesDR(iGauge1, value, intv, 3, 6, 18) ; // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if(vdir == -1){
  gfx.UserImagesDR(iGauge1, value, intv + 2, 3, 8, 18) ; // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  gfx.LedDigitsDisplay(value, iiLeddigits1, 3, 1, 28, 0) ;  // Leddigits1
  value = value + vdir;
  if(value > 100){
  gfx.UserImages(iAngularmeter1,0) ;                         // Angularmeter1 show initialy, if required
  gfx.UserImages(iGauge1,0) ;  // Gauge1 show initialy, if required
  gfx.UserImage(iLeddigits1);  // Leddigits1 show all digits at 0, only do this once
  value = 0;
  }
  if(value < 0){
  gfx.UserImages(iAngularmeter1,100) ;                         // Angularmeter1 show initialy, if required
  gfx.UserImages(iGauge1,100) ;  // Gauge1 show initialy, if required
  gfx.UserImage(iLeddigits1);  // Leddigits1 show all digits at 0, only do this once
  value = 100;
  }
  buttw = gfx.CheckButtons();
  if(buttw != lastbut && buttw > 0){
  but = buttw;
  if(but == 33){
  vdir = 1;
  }
  if(but == 34){
  vdir = 0;
  }
  if(but == 35){
  vdir = -1;
  }

  } else {
  but = 0;
  }
  lastbut = buttw;
  // put your main code here, to run repeatedly:
  yield(); // Required for ESP
}

