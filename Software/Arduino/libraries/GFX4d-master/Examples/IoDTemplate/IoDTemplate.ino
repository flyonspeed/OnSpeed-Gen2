/******************************************
* IoD Template                            *
* A blank template for any IoD sketch     *
* with common commands to quickly start   *
* a sketch that initializes the screen    *
*                                         *
******************************************/


#include "ESP8266WiFi.h"
#include "GFX4d.h"

GFX4d gfx = GFX4d();

void setup() {

  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(true);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE); gfx.Font(2);  gfx.TextSize(1);

}

void loop() {
 
 yield();

}

