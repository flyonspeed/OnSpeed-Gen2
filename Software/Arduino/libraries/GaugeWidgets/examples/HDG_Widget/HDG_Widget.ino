/*
  Copyright 2020 V.R. Little

  Permission is hereby granted, free of charge, to any person provided a copy of this software and associated documentation files
  (the "Software") to use, copy, modify, or merge copies of the Software for non-commercial purposes, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
  OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
  Heading Indicator composite gauge example using the GaugeWidgets Library.
    by V.R. ("Voltar") Little
    Version 2.0, April 2, 2020
*/

#include <GaugeWidgets.h>
#include <M5Stack.h>
#include <Free_Fonts.h>
#define SCREEN_Y  240   // Screen sizes are set here.
#define SCREEN_X  320

TFT_eSprite gdraw = TFT_eSprite(&M5.Lcd);

Gauges myGauges;
/*
   Function prototype
*/
void drawHDG (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t heading, int16_t navBug, int16_t courseBug);

uint16_t displayBrightness = 100;
              
/*
  Example data values. Usually these are derived from a data input function.
*/  
int16_t px0 = 159;
int16_t py0 = 119;
int16_t arcSize = 115;
int16_t arcWidth = 20;
int16_t maxDisplay = 360;
int16_t minDisplay = 0;
int16_t startAngle = 270;
int16_t arcAngle = 360;
int16_t clockWise = true;
uint8_t gradMarks = 36;
int16_t heading = 210;
int16_t navBug = 110;
int16_t headingBug = 120;

void setup() {

  M5.begin();
  M5.Lcd.fillScreen(TFT_WHITE);
  delay (100);

  gdraw.setColorDepth(8);
  gdraw.createSprite(SCREEN_X, SCREEN_Y);
  gdraw.setTextWrap(false); // Very important for scaling
}

void loop() {
  gdraw.fillSprite (TFT_BLACK);
  M5.update();
  
  /*
   Change display brightness using panel buttons.
  */
  if (M5.BtnC.wasPressed() && !M5.BtnA.wasPressed() && (displayBrightness > 0)) {
    displayBrightness *= 2; // brightness up
  }

  if (M5.BtnA.wasPressed() && !M5.BtnC.wasPressed() && (displayBrightness <= 255)) {
    displayBrightness /= 2; // brightness down
  }

  if (displayBrightness < 1) displayBrightness = 1;
  if (displayBrightness > 255) displayBrightness = 255;
  
  M5.Lcd.setBrightness (displayBrightness);
  
  /*
  */
  drawHDG (px0, py0, arcSize, arcWidth, maxDisplay, minDisplay, 270 - heading, arcAngle, clockWise,
           gradMarks, heading, navBug, headingBug);

  gdraw.pushSprite (0, 0);

  heading +=2; if (heading>359) heading = 0; 
}

void drawHDG (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t heading, int16_t navBug, int16_t headingBug) {

  /*
    Heading Indicator
  */
  myGauges.setRange (0, 0, 0, 0, 0); //disables all previously enabled ranges.
  myGauges.setRange (1, true, maxDisplay, minDisplay, TFT_BLACK);

  uint16_t pointerColor1 = TFT_WHITE;

  myGauges.clearPointers();
  
  if (headingBug >= 0 && headingBug <= 360) myGauges.setPointer (1, headingBug, BUG_OUT , TFT_CYAN, '\0');
  else myGauges.setPointer (1, 0, 0 , 0, '\0');
  
  if (navBug >= 0 && navBug <= 360) myGauges.setPointer (2, navBug, BUG_OUT, TFT_MAGENTA, '\0');
  else myGauges.setPointer (2, 0, 0 , 0, '\0');

  /*
    Print scale values
  */
  gdraw.setFreeFont(FSSB9);
  if (arcSize >= 135) gdraw.setFreeFont(FSSB12);
  gdraw.setTextColor (TFT_LIGHTGREY);
  gdraw.setTextDatum (MC_DATUM);


  for (uint16_t i = 3; i <= gradMarks; i += 3) {
    
    int16_t headingDraw = minDisplay + i * (maxDisplay - minDisplay) / gradMarks;
    int16_t _X0;
    if (clockWise) _X0 = px0 + (arcSize - 46) * cos(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle));
    else _X0 = px0 + (arcSize - 46) * -cos(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle));
    int16_t _Y0 = py0 + (arcSize - 46) * sin(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle));
  
    gdraw.setFreeFont(FSSB12);
     
  switch (headingDraw){
    
    case 90:
      gdraw.setFreeFont(FSSB18);
      gdraw.drawString("E", _X0, _Y0, GFXFF);
    break;
      
    case 180:
      gdraw.setFreeFont(FSSB18);
      gdraw.drawString("S", _X0, _Y0, GFXFF);
    break;
      
    case 270:
      gdraw.setFreeFont(FSSB18);
      gdraw.drawString("W", _X0, _Y0, GFXFF);
    break;
      
    case 360:
      gdraw.setFreeFont(FSSB18);
      gdraw.drawString("N", _X0, _Y0, GFXFF);
    break; 
  
    default:
      gdraw.drawString(String(headingDraw/10), _X0, _Y0, GFXFF);
    break;
  }
}

  /*
     Draw main gauge
  */
  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);
                     
  /*
    Draw exterior decorative ring
  */
  arcSize = 130;
  arcWidth = 5;
  gradMarks = 0;
  
  myGauges.setRange (1, true, maxDisplay, minDisplay, TFT_DARKGREY);
  myGauges.clearPointers();

  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Draw marker rings
  */
  arcSize = 115;
  arcWidth = 30;
  maxDisplay = 360;
  minDisplay = 0;
  startAngle = 270;
  arcAngle = 360;
  
  myGauges.clearRanges();

  myGauges.clearPointers();
  myGauges.setPointer (1, 90, ARROW_TOP, TFT_ORANGE, '\0');
  myGauges.setPointer (2, 180, ARROW_TOP, TFT_ORANGE, '\0');
  myGauges.setPointer (3, 270, ARROW_TOP, TFT_ORANGE, '\0');
  myGauges.setPointer (4, 360, ARROW_TOP, TFT_ORANGE, '\0');

  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks); 

  arcWidth = 20;  
  
  myGauges.clearPointers();
  myGauges.setPointer (1, 45, ARROW_TOP,  TFT_LIGHTGREY, '\0');
  myGauges.setPointer (2, 135, ARROW_TOP, TFT_LIGHTGREY, '\0');
  myGauges.setPointer (3, 225, ARROW_TOP, TFT_LIGHTGREY, '\0');
  myGauges.setPointer (4, 315, ARROW_TOP, TFT_LIGHTGREY, '\0');

  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);  
  /*
     Print values
  */
  gdraw.setTextDatum (MC_DATUM);
  gdraw.setFreeFont(FSSB18);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.drawString(String(heading), 160, 90);

  if (headingBug >= 0 && headingBug <= 360){
    gdraw.setFreeFont(FSSB12);
    gdraw.setTextColor (TFT_CYAN);
    gdraw.setTextDatum (BL_DATUM);
    gdraw.drawString("HDG", 0, 215);
    gdraw.setTextColor (TFT_WHITE);
    gdraw.drawString (String(headingBug), 5, 240);
  }

  if (navBug >= 0 && navBug <= 360) {
    gdraw.setFreeFont(FSSB12);
    gdraw.setTextColor (TFT_MAGENTA);
    gdraw.setTextDatum (BL_DATUM);
    gdraw.drawString("NAV", 300, 215);
    gdraw.setTextColor (TFT_WHITE);
    gdraw.drawString (String(navBug), 300, 240);
  }
  
  /*
    Draw Airplane
    
            p1
           / \
          /   \
        p2     p3
        /|     |\
      /  |     |  \
    /    |     |    \
  /      |  p0 |      \
p4 -_____p5    p6____- p7
         \     /
         /\   /\
       /   \ /   \
     /   /  p8 \   \
   / /             \ \
   p9              p10
            
  */
  
  uint16_t x1 = px0; 
  uint16_t y1 = py0 - arcSize / 2;
  uint16_t x2 = px0 - arcSize / 16; 
  uint16_t y2 = py0 - 3 * arcSize / 16;
  uint16_t x3 = px0 + arcSize / 16; 
  uint16_t y3 = y2;
  uint16_t x4 = px0 - 5 * arcSize / 16; 
  uint16_t y4 = py0 + arcSize / 4;
  uint16_t x5 = x2; 
  uint16_t y5 = py0 + arcSize / 8;
  uint16_t x6 = x3; 
  uint16_t y6 = y5;
  uint16_t x7 = px0 + 5 * arcSize / 16 ; 
  uint16_t y7 = y4;
  uint16_t x8 = px0; 
  uint16_t y8 = py0 + 5 * arcSize / 16;
  uint16_t x9 = px0 - 3 * arcSize / 16;
  uint16_t y9 = py0 + 3 * arcSize / 8;
  uint16_t x10 = px0 + 3 * arcSize / 16;
  uint16_t y10 = y9;
  
  gdraw.drawLine (x1, y1, x2, y2, TFT_ORANGE);
  gdraw.drawLine (x2, y2, x5, y5, TFT_ORANGE);
  gdraw.drawLine (x5, y5, x8, y8, TFT_ORANGE);
  gdraw.drawLine (x1, y1, x3, y3, TFT_ORANGE);
  gdraw.drawLine (x3, y3, x6, y6, TFT_ORANGE);
  gdraw.drawLine (x6, y6, x8, y8, TFT_ORANGE);
  gdraw.drawTriangle (x2, y2, x5, y5, x4, y4, TFT_ORANGE);
  gdraw.drawTriangle (x3, y3, x6, y6, x7, y7, TFT_ORANGE);
  gdraw.drawTriangle (x5, y5, x9, y9, x8, y8, TFT_ORANGE);
  gdraw.drawTriangle (x6, y6, x10, y10, x8, y8, TFT_ORANGE);
}
