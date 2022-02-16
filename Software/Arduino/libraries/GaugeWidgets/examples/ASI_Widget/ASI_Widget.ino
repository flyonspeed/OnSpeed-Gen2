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
  Airspeed Indicator composite gauge example using the GaugeWidgets Library.
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
void drawASI (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t VS0, int16_t VS1, int16_t VFE, int16_t VA, int16_t VNO, int16_t VNE,
              int16_t airSpeed, int16_t trueAirspeed, int16_t speedBug);              

  uint16_t displayBrightness = 100;
  
  /*
     Example data values. Usually these are derived from a data input function.
  */
  int16_t px0 = 159;
  int16_t py0 = 92;
  int16_t arcSize = 145;
  int16_t arcWidth = 22;
  int16_t maxDisplay = 260;
  int16_t minDisplay = 20;
  int16_t startAngle = 315;
  int16_t arcAngle = 270;
  int16_t clockWise = true;
  uint8_t gradMarks = 24;
  int16_t VS0 = 53;
  int16_t VS1 = 60;
  int16_t VFE = 95;
  int16_t VX= 74;
  int16_t VY= 105;  
  int16_t VA = 125;
  int16_t VNO = 160;
  int16_t VNE = 240;
  int16_t airSpeed = 185;
  int16_t trueAirspeed = 208;
  int16_t speedBug = -110;
  
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
  drawASI (px0, py0, arcSize, arcWidth, maxDisplay, minDisplay, startAngle, arcAngle, clockWise,
           gradMarks, VS0, VS1, VFE, VA, VNO, VNE, airSpeed, trueAirspeed, speedBug);

  gdraw.pushSprite (0, 0);
}
void drawASI (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t VS0, int16_t VS1, int16_t VFE, int16_t VA, int16_t VNO, int16_t VNE,
              int16_t airSpeed, int16_t trueAirspeed, int16_t speedBug) {

  /*
    Airspeed Indicator
  */
  myGauges.clearRanges(); //disables all previously enabled ranges
  myGauges.setRange (1, true, maxDisplay, VNE, TFT_RED);
  myGauges.setRange (2, true, VNE, VNO, TFT_ORANGE);
  myGauges.setRange (3, true, VNO, VS1, TFT_GREEN);
  myGauges.setRange (4, true, VS1, 0, DARKGREY);
  myGauges.setRange (5, false, 0, 0, TFT_RED);

  uint16_t pointerColor1 = TFT_WHITE;
  
  myGauges.clearPointers(); //disables all previously set pointers
  myGauges.setPointer (1, airSpeed, NEEDLE, pointerColor1, '\0');
  myGauges.setPointer (2, VNE, BAR_LONG, TFT_RED, '\0');
  myGauges.setPointer (3, VX, BAR_LONG, TFT_LIGHTGREY, 'x');
  myGauges.setPointer (4, VY, BAR_LONG, TFT_LIGHTGREY, 'y');
  myGauges.setPointer (5, VA, BAR_LONG, TFT_BLUE, 'a');
  myGauges.setPointer (6, VNO, BAR_LONG, TFT_ORANGE, '\0');

  if (speedBug >= 0) myGauges.setPointer (7, speedBug, BUG_OUT, TFT_MAGENTA, '\0');
  else myGauges.setPointer (7, 0, 0, 0, '\0');
  
  myGauges.setPointer (8, 0, 0, 0, '\0');
  
  /*
    Print scale values
  */
  gdraw.setFreeFont(FSSB9);
  if (arcSize >= 135) gdraw.setFreeFont(FSSB12);
  gdraw.setTextColor (TFT_LIGHTGREY);
  gdraw.setTextDatum (MC_DATUM);


  for (uint16_t i = 0; i <= gradMarks; i += 2) {
    
    if (clockWise){
      gdraw.drawString(String (minDisplay + i * (maxDisplay - minDisplay) / gradMarks),
                       px0 + (arcSize - 48) * cos(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle)),                 
                       py0 + (arcSize - 48) * sin(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle)), GFXFF);
    }
    else {
        gdraw.drawString(String (minDisplay + i * (maxDisplay - minDisplay) / gradMarks),
                         px0 + (arcSize - 48) * -cos(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle)),
                         py0 + (arcSize - 48) * sin(DEG_TO_RAD * (i * arcAngle / gradMarks + startAngle)), GFXFF);   
    }
  }
  /*
    Draw main gauge
  */
  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Draw flaps range marker
  */
  arcSize += arcWidth / 2;  // offset
  arcWidth  = 2 * arcWidth / 3;
  maxDisplay = 260;
  minDisplay = 20;
  startAngle = 315;
  arcAngle = 270;
  gradMarks = 0;

  myGauges.clearRanges(); //disables all previously enabled ranges
  myGauges.setRange (1, true, 95, 53, TFT_WHITE);

  pointerColor1 = TFT_WHITE;
  myGauges.clearPointers(); //disables all previously set pointers

  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Print values
  */
  gdraw.setFreeFont(FSSB9);
  gdraw.setTextColor (TFT_CYAN);
  gdraw.setTextDatum (TC_DATUM);
  gdraw.drawString("AIRSPEED", 159, 0, GFXFF);
  gdraw.drawString("KNOTS", 159, 69, GFXFF);

  gdraw.setFreeFont(FSSB24);
  gdraw.setTextColor (TFT_WHITE);
  if (airSpeed > 240) gdraw.setTextColor(TFT_RED, BLACK);
  gdraw.setCursor (120, 56);
  gdraw.print(airSpeed);


  gdraw.setFreeFont(FSSB12);
  gdraw.setTextColor (TFT_CYAN);
  gdraw.setTextDatum (BL_DATUM);
  gdraw.drawString("TAS", 0, 215);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.drawString (String(trueAirspeed), 5, 240);

if (speedBug >= 0) {
  gdraw.setFreeFont(FSSB12);
  gdraw.setTextColor (TFT_MAGENTA);
  gdraw.setTextDatum (BL_DATUM);
  gdraw.drawString("Vy", 300, 215);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.drawString (String(speedBug), 300, 240);
  }
}
