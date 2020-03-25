//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//
/***************************************************
* Very simple Serial Terminal using 4D SD Keyboard * 
* Widget and automatic keyboard handling           *
***************************************************/
#include "ESP8266WiFi.h"
#include "GFX4d.h"

GFX4d gfx = GFX4d();

//bool shift, oldshift;

#include "IoDkBTestAutoConst.h"    // Note. This file will not be created if there are no generated graphics

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(LANDSCAPE);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.Open4dGFX("IODKBT~1"); // Opens DAT and GCI files for read using filename without extension. Note! Workshop generates files with Short filenames
  gfx.imageTouchEnable(-1, true) ;   // Keyboard1 show initialy, if required
  gfx.UserImages(iKeyboard1,0) ;             // Keyboard1 show initialy, if required
  gfx.imageTouchEnable(iKeyboard1, false) ;
  gfx.TextWindow(0, 0, 320, 75, LIME, BLACK, LIGHTGREY);
  Serial.begin(115200);
}
// end Setup **do not alter, remove or duplicate this line**

void loop()
{
  int butt = gfx.ImageTouchedAuto(KEYPAD);
  int kd = gfx.DecodeKeypad(iKeyboard1, butt, iKeyboard1keystrokes, oKeyboard1);
  if(kd > -1)gfx.TWwrite(kd);
  String sercom = gfx.GetCommand();
  if(sercom != "")Serial.println(sercom); 
  while(Serial.available() > 0)gfx.TWwrite(Serial.read());  
  yield(); // Required for ESP
}
