/******************************************
* Font One Styles                         *
* Shows the 4 different styles that can   *
* be used with Font 1 only. Can be used   *
* as an alternative to LEDdigits          *
*                                         *
******************************************/


#include "ESP8266WiFi.h"
#include "GFX4d.h"

GFX4d gfx = GFX4d();

int count;

void setup() {

  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(true);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE); gfx.Font(1);  gfx.TextSize(5);
  gfx.MoveTo(0,0);
  gfx.TextColor(WHITE); gfx.Font(2);  gfx.TextSize(1);
  gfx.print("Style DOT MATRIX");
  gfx.MoveTo(0,70);
  gfx.print("Style DOT MATRIX SQUARE");
  gfx.MoveTo(0,140);
  gfx.print("Style DOT MATRIX LED");
  gfx.MoveTo(0,222);
  gfx.print("Style DOT MATRIX FADE");
  gfx.TextColor(WHITE); gfx.Font(1);  gfx.TextSize(6);
}

void loop() {
  gfx.TextSize(6);
  gfx.FontStyle(DOTMATRIXROUND);
  gfx.TextColor(ORANGE, BLACK);
  gfx.MoveTo(0, 20);
  gfx.print(count);
  gfx.print("   ");
  gfx.FontStyle(DOTMATRIXSQUARE);
  gfx.TextColor(LIME, BLACK);
  gfx.MoveTo(0, 90);
  gfx.print(count);
  gfx.print("   ");
  gfx.TextSize(8);
  gfx.FontStyle(DOTMATRIXLED);
  gfx.TextColor(RED, BLACK);
  gfx.MoveTo(0, 160);
  gfx.print(count);
  gfx.print("  ");
  gfx.FontStyle(DOTMATRIXFADE);
  gfx.TextColor(SKYBLUE, BLACK);
  gfx.MoveTo(0, 244);
  gfx.print(count);
  gfx.print("  ");
  count ++;
  if(count > 999) count = 0;
 yield();

}
