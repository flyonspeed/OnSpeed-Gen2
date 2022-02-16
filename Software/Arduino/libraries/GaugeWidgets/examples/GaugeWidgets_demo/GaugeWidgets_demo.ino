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
   This is the master example sketch for 'GaugeWidgets'. Cycle between examples by pressing the 'B' (center) button.
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

uint8_t displayMode;
uint16_t displayBrightness = 100;

/*
  Local function prototypes
*/
void drawPower (int16_t rpmValue, int16_t mapValue, int16_t fuelFlowValue, int16_t powerPercentValue); // Engine power display
void drawEGTCHT (int16_t egtValue[], int16_t chtValue[] );
void drawFuel (int16_t fuelLeftValue, int16_t fuelRightValue, int16_t fuelFlowValue,
               int16_t fuelPressValue, int16_t powerPercentValue, char leaningState);
void drawOilElectric(int16_t oilPressValue, int16_t oilTempValue,
               int16_t volt1Value, int16_t volt2Value, int16_t ampValue);
void drawVolts (int16_t volt1Value, int16_t volt2Value);
void drawAmps (int16_t ampValue);

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(TFT_WHITE);
  delay (100);

  gdraw.setColorDepth(8);
  gdraw.createSprite(SCREEN_X, SCREEN_Y);
  gdraw.setTextWrap(false); // Very important for scaling

  displayMode = 1;
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

  if (M5.BtnB.wasPressed()) displayMode++;
  if (displayMode > 6) displayMode = 1;

  /*
     example data values. Usually these are derived from a data input function.
  */
  int16_t rpmValue = 2150;
  int16_t fuelFlowValue = 120; // scaled up by 10x
  int16_t mapValue = 2992;
  int16_t powerPercentValue = 75;

  int16_t egtValue[4] = {1400, 1350, 1450, 1500};
  int16_t chtValue[4] = {290, 320, 350, 330};

  int16_t fuelLeftValue = 25;
  int16_t fuelRightValue = 15;
  int16_t fuelPressValue = 70;
  int16_t leaningState = 'P';

  int16_t oilPressValue = 75;
  int16_t oilTempValue = 220;
  int16_t volt1Value = 145; // scaled up by 10x
  int16_t volt2Value = 120; // scaled up by 10x
  int16_t ampValue = 340;

  switch (displayMode) {
    case 1: // tach, manifold pressure, fuel flow, power percent
      drawPower(rpmValue, mapValue, fuelFlowValue, powerPercentValue);
      break;
    case 2: // exhaust gas and cylinder head temperatures
      drawEGTCHT(egtValue, chtValue);
      break;
    case 3: // fuel, power percent, leaning state
      drawFuel(fuelLeftValue, fuelRightValue, fuelFlowValue, fuelPressValue,
               powerPercentValue, leaningState);
      break;
    case 4: // oil, volts, amps
      drawOilElectric(oilPressValue, oilTempValue, volt1Value, volt2Value, ampValue);
      break;
    case 5: // stand-alone dual voltmeter
      drawVolts(volt1Value, volt2Value);
      break;
    case 6: // stand-alone ammeter
      drawAmps(ampValue);
      break;
    default:
      break;
  }
  gdraw.pushSprite (0, 0);
}

void drawPower(int16_t rpmValue, int16_t mapValue, int16_t fuelFlowValue, int16_t powerPercentValue) { // Engine power display

  /*
    x0 and y0 are the X and Y positions of the gauge datum, assuming
    a 320 x 240 pixel screen for the M5 physical display.

    Set barSize and barWidth in pixels to
    establish the gauge size.

    For arcGraph gauges, angles (in degrees) increase clockwise when the
    clockWise parameter is set true, and counterclockwise when set false.

    maxDisplay and minDisplay are in the native units of the gauge.
    Because of the integer math used in the library for the
    vertical or horizontal gauges, you should scale these numbers
    to display fractions(for example, 16.5 volts can be set to 165.

    You can have several pointers displayed in each gauge widget,
    with a defined color and an optional single character tag for each.

    When you set a pointerType to zero, it disables the pointer
    and the pointerTag display. You can independently disable
    the pointerTag by setting it to '\0'.

    Finally, you can add graduations to any display by setting the gradMarks
    variable.  This defines the number of major graduations per gauge. Smaller
    graduations are drawn in between the major ones.
  */

  /*
    Tachometer gauge
  */
  int16_t x0 = 190;
  int16_t y0 = 190;
  int16_t barSize = 190;
  int16_t barWidth = 25;
  int16_t maxDisplay = 2800;
  int16_t minDisplay = 0;
  int16_t startAngle = 180;
  int16_t arcAngle = 90;
  bool clockWise = true;
  uint8_t gradMarks = 7; // 2800/7 provides 400 RPM per major graduation.

  /*
      Here's where you make the color bars and define the ranges for
      the gauges. Up to NUM_RANGES (nominally 5) ranges are supported (see GaugeWidgets.h).

      To enable a range, you set rangeValid true.  To disable a range,
      set it to false.  To disable all ranges use clearRanges().
      If you set rangeValid false, the additional
      data for that range is ignored.
  */
  myGauges.clearRanges();
  myGauges.setRange(1, true, 2800, 2700, TFT_RED);
  myGauges.setRange(2, true, 2700, 2100, TFT_GREEN);
  myGauges.setRange(3, true, 2100, 1100, TFT_DARKGREY);
  myGauges.setRange(4, true, 1100, 0, TFT_BLUE);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  /*
    Up to NUM_POINTERS (nominally 8) pointers of several Types.  See GaugeWidgets.h for constants.
    Types include various arrows, long and short bars, bugs, dots and needles.
  */
  myGauges.clearPointers();
  myGauges.setPointer (1, rpmValue, ARROW_BOTTOM, TFT_WHITE, '\0');

  /*
     Clockwise arc (clockWise is true and gradMarks is between 2 and 255. 0 to disable).
  */
  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Print values.
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setTextColor (TFT_WHITE);

  /*
    text datum locations are addressed directly through public variables
  */
  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 13);
  gdraw.printf("%d\n", rpmValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 35);
  gdraw.printf("RPM");

  /*
    Manifold pressure gauge
  */
  x0 = 190;
  y0 = 190;
  barSize = 125;
  barWidth = 25;
  maxDisplay = 3600;
  minDisplay = 0;
  startAngle = 180;
  arcAngle = 90;
  clockWise = true;
  gradMarks = 0;

  myGauges.clearRanges();
  myGauges.setRange(1, true, 3600, 3200, TFT_RED);
  myGauges.setRange(2, true, 3200, 0, TFT_GREEN);

  myGauges.clearPointers();
  myGauges.setPointer (1, mapValue, ARROW_IN, TFT_WHITE, '\0');
  myGauges.setPointer (2, 0, 0, TFT_WHITE, '\0');
  myGauges.setPointer (3, 0, 0, TFT_WHITE, '\0');
  myGauges.setPointer (4, 0, 0, TFT_WHITE, '\0');
  
  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
      Print values
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setTextColor (TFT_WHITE);

  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 13);
  gdraw.printf("%.1f", (float)mapValue / 100);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 35);
  gdraw.printf("MP inHg");

  /*
    Fuel flow gauge
  */
  x0 = 190;
  y0 = 190;
  barSize = 65;
  barWidth = 25;
  maxDisplay = 250;
  minDisplay = 0;
  startAngle = 180;
  arcAngle = 90;
  clockWise = true;
  gradMarks = 0;

  myGauges.clearRanges();
  myGauges.setRange(1, true, 250, 150, TFT_ORANGE);
  myGauges.setRange(2, true, 150, 50, TFT_GREEN);
  myGauges.setRange(3, true, 50, 0, TFT_DARKGREY);

  
  myGauges.clearPointers();
  myGauges.setPointer (1, fuelFlowValue, ARROW_IN, TFT_WHITE, '\0');
  myGauges.setPointer (2, 0, 0, TFT_WHITE, '\0');
  myGauges.setPointer (3, 0, 0, TFT_WHITE, '\0');
  myGauges.setPointer (4, 0, 0, TFT_WHITE, '\0');
  
  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
       Print values
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setTextColor (TFT_WHITE);

  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 13);
  gdraw.printf("%.1f", (float)fuelFlowValue / 10);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 35);
  gdraw.printf("FF gph");

  /*
     Percent power gauge
  */
  x0 = 0;
  y0 = 200;
  barSize = 190;
  barWidth = 25;
  maxDisplay = 100;
  minDisplay = 0;
  gradMarks = 4;

  myGauges.clearPointers ();
  myGauges.setPointer (1, powerPercentValue, BAR_LONG, TFT_WHITE, '\0');
  myGauges.setPointer (2, 50, BUG_BOTTOM, TFT_MAGENTA, '\0');
 
  myGauges.clearRanges(); 
  myGauges.setRange(1, true, 110, 100, TFT_RED);
  myGauges.setRange(2, true, 100, 75, TFT_ORANGE);
  myGauges.setRange(3, true, 75, 50, TFT_GREEN);
  myGauges.setRange(4, true, 50, 0, TFT_BLUE);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  myGauges.hBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);

  /*
       Print values
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setTextColor (TFT_WHITE);

  gdraw.setCursor (myGauges.topDatumX + 10, myGauges.topDatumY + 13);
  gdraw.printf("%d" , powerPercentValue);

  gdraw.setFreeFont(FSSB12);
  gdraw.setTextColor (TFT_CYAN);
  gdraw.printf(" %%hp");
}

void drawEGTCHT(int16_t egtValue[], int16_t chtValue[] ) {

  /*
    EGT/CHT gauge type of display.
    This example is a complex overlay of gridlines, text, and progressive gauge bars showing two
    independent (CHT and EGT) values. It is about the limit of a legible 320x240 display, displaying
    eight independent values and associated scales.
  */
  int16_t x0;
  int16_t y0 = 200;
  int16_t barSize = 140;
  int16_t barWidth = 35;
  int16_t maxDisplay = 1500;  // in this example degrees Fahrenheit
  int16_t minDisplay = 600;
  int8_t  gradMarks = 0;      // no graduation marks

  int16_t egtVal;
  int16_t CHTValue;
  int16_t pointerType;
  int16_t pointerTag;
  uint16_t pointerColor;

  /*
     Draw some horizontal lines on the screen to help define the display.
  */
  gdraw.setTextColor (TFT_DARKGREY);
  gdraw.setFreeFont(FSSB9);

  int16_t j = 0;

  for (int16_t i = (y0 - 140) ; i <= (y0 + 1); i += 47) {

    gdraw.drawLine (31, i, 266, i, TFT_DARKGREY);
    gdraw.setCursor (0, i + 5);
    gdraw.setTextColor (TFT_CYAN);
    gdraw.print(500 - 100 * j);
    gdraw.setTextColor (TFT_MAGENTA);
    gdraw.setCursor (270, i + 5);
    gdraw.print (1500 - 300 * j);
    j++;
  }

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setCursor (0, 227);
  gdraw.print("CHT");
  gdraw.setTextColor (TFT_MAGENTA);
  gdraw.setCursor (270, 42);
  gdraw.print("EGT");

  /*
    In this example for EGT gauges, you want to set the
    pointerTypes to 0 in order to generate a progressive bar display.

    To display CHT temperatures overlaid on the EGT display, you
    need to normalize the CHT temperature range to match the EGT
    temperature range.

    Use one of the pointers assigned to the EGT gauge to
    overlay the CHT pointer.  This is a bit of a trick to reuse an
    existing pointer without defining a whole new gauge.  Adding a tag
    helps to define the channel number.
  */
  for (int i = 0; i < 4; i++) {
    x0 = 39 + 63 * i;
    
    myGauges.clearPointers();
    pointerType = 0;    // no pointer
    pointerTag = '\0';
    pointerColor = TFT_WHITE;
    
    myGauges.setPointer (1, egtValue[i], pointerType, pointerColor, pointerTag);

    pointerType = 3;  // pointer Type 3 is a horizontal bar
    pointerTag = i + '1';
    pointerColor = TFT_WHITE;
    if (chtValue[i] >= 400) pointerColor = TFT_RED;
    

    myGauges.setPointer (2, chtValue[i] * 1500 / 500, pointerType, pointerColor, pointerTag);
    myGauges.setPointer (3, 0, 0, TFT_WHITE, '\0');
    myGauges.setPointer (4, 0, 0, TFT_WHITE, '\0');

    /*
      To make a progressive bar display, modify the
      rangeTop and rangeBot information as shown.
    */
    myGauges.clearRanges();
    myGauges.setRange(1, true, 1500, egtValue[i], TFT_BLACK);
    myGauges.setRange(2, true, min(1500, egtValue[i]), 600, TFT_GREEN);


    myGauges.vBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);  //draw the gauge

    /*
       Add the labels and values displays.
       You may need to modify the font size for visibility.
    */
    gdraw.setTextColor (TFT_CYAN);
    gdraw.setFreeFont(FSSB12);

    gdraw.setCursor (0, 19);
    gdraw.print("Cylinder Temperatures (*F)");

    gdraw.setTextColor (TFT_WHITE);
    gdraw.setFreeFont(FSSB12);

    gdraw.setCursor (24 + 63 * i, 44);
    gdraw.print(egtValue[i]);

    gdraw.setTextColor (pointerColor);
    gdraw.setFreeFont(FSSB12);

    gdraw.setCursor (40 + 63 * i , 228);
    gdraw.print(chtValue[i]);

    /*
       Display the leaning state. Extend black shadows around text.
    */

    char LOPState = 'R';
    gdraw.setFreeFont(FSSB18);
    gdraw.setCursor (115, 190);
    gdraw.setTextColor (TFT_BLACK);
    if (LOPState == 'R') gdraw.print("ROP");
    else if (LOPState == 'P') gdraw.print("PEAK");
    else if (LOPState == 'L') gdraw.print("LOP");

    gdraw.setFreeFont(FSSB18);
    gdraw.setCursor (113, 192);
    gdraw.setTextColor (TFT_WHITE);
    if (LOPState == 'R') gdraw.print("ROP");
    else if (LOPState == 'P') gdraw.print("PEAK");
    else if (LOPState == 'L') gdraw.print("LOP");
  }
}
void drawFuel(int16_t fuelLeftValue, int16_t fuelRightValue, int16_t fuelFlowValue,
              int16_t fuelPressValue, int16_t powerPercentValue, char leaningState) {
  /*
    Left fuel gauge
  */

  int16_t x0 = 20;
  int16_t y0 = 200;
  int16_t barSize = 140;
  int16_t barWidth = 30;
  int16_t maxDisplay = 42;
  int16_t minDisplay = 0;
  int8_t gradMarks = 2;
  uint16_t pointerColor = TFT_WHITE;
  if (fuelLeftValue <= 5) pointerColor = TFT_RED;

  myGauges.clearRanges();
  myGauges.setRange(1, true, fuelLeftValue, 5, TFT_GREEN);
  myGauges.setRange(2, true, 5, 0, TFT_RED);

  myGauges.clearPointers();
  myGauges.setPointer (1, fuelLeftValue, BAR_LONG, pointerColor, 'L');

  myGauges.vBarGraph (x0, y0, barSize, barWidth, maxDisplay, minDisplay, gradMarks);

  /*
    Print values
  */
  gdraw.setTextColor (TFT_WHITE);
  if (fuelLeftValue <= 5) gdraw.setTextColor(TFT_RED);

  /*
    Use the top and bottom text datum positions to help place text labels.
    These are special library variables that are effective after a call to a
    Graph function.  Note that the datum offsets are in pixels and are releated
    to the font size specified.
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor(myGauges.topDatumX - gdraw.textWidth(String(fuelLeftValue)) / 2, myGauges.topDatumY - 8);
  gdraw.printf("%d", fuelLeftValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor(myGauges.btmDatumX - gdraw.textWidth("gal") / 2, myGauges.btmDatumY + 24);
  gdraw.printf("gal");

  /*
    Right fuel gauge
  */
  x0 = 95;
  y0 = 200;
  barSize = 140;
  barWidth = 30;
  maxDisplay = 42;
  minDisplay = 0;
  gradMarks = 0;

  myGauges.clearRanges();
  myGauges.setRange(1, true, fuelRightValue, 5, TFT_GREEN);
  myGauges.setRange(2, true, 5, 0, TFT_RED);

  pointerColor = TFT_WHITE;

  myGauges.clearPointers(); //disables all previously set pointers and clears values
  myGauges.setPointer (1, fuelRightValue, BAR_LONG, pointerColor, 'R');
  myGauges.setPointer (2, 10, BUG_RIGHT, TFT_YELLOW, '\0');

  myGauges.vBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);

  /*
    Print values
  */
  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);

  gdraw.setCursor (0, 20);
  gdraw.printf("Fuel System");

  gdraw.setTextColor (TFT_WHITE);
  if (fuelRightValue <= 5) gdraw.setTextColor(TFT_RED);
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor(myGauges.topDatumX - gdraw.textWidth(String(fuelRightValue)) / 2, myGauges.topDatumY - 8);
  gdraw.printf("%d", fuelRightValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor(myGauges.btmDatumX - gdraw.textWidth("gal") / 2, myGauges.btmDatumY + 24);
  gdraw.printf("gal");


  /*
    Fuel flow gauge
  */
  x0 = 172;
  y0 = 200;
  barSize = 94;
  barWidth = 31;
  maxDisplay = 250;
  minDisplay = 0;
  gradMarks = 0;

  myGauges.clearRanges();
  myGauges.setRange(1, true, 250, 150, TFT_ORANGE);
  myGauges.setRange(2, true, 150, 50, TFT_GREEN);
  myGauges.setRange(3, true, 50, 0, TFT_DARKGREY);
  myGauges.setRange(4, false, 0, 0, TFT_BLACK);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  pointerColor = TFT_WHITE;

  myGauges.clearPointers();
  myGauges.setPointer (1, fuelFlowValue, ARROW_RIGHT, pointerColor, '\0');

  myGauges.vBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);

  /*
    Print values
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor(myGauges.topDatumX - gdraw.textWidth(String(fuelFlowValue) + ".") / 2, myGauges.topDatumY - 8);
  gdraw.printf("%.1f", (float)fuelFlowValue / 10);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor(myGauges.btmDatumX - gdraw.textWidth("gph") / 2, myGauges.btmDatumY + 24);
  gdraw.printf("gph");

  /*
    Fuel pressure gauge
  */
  x0 = 250;
  y0 = 200;
  barSize = 94;
  barWidth = 31;
  maxDisplay = 100;
  minDisplay = 0;
  gradMarks = 0;

  myGauges.clearRanges();
  myGauges.setRange(1, true, 100, 75, TFT_RED);
  myGauges.setRange(2, true, 75, 20, TFT_GREEN);
  myGauges.setRange(3, true, 20, 0, TFT_RED);
  myGauges.setRange(4, false, 0, 0, TFT_BLACK);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  pointerColor = TFT_WHITE;

  myGauges.clearPointers();
  myGauges.setPointer (1, fuelPressValue, ARROW_RIGHT, pointerColor, '\0');

  myGauges.vBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);

  /*
    Print values
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor(myGauges.topDatumX - gdraw.textWidth(String(fuelPressValue)) / 2, myGauges.topDatumY - 8);
  gdraw.printf("%d", fuelPressValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor(myGauges.btmDatumX - gdraw.textWidth("psi") / 2, myGauges.btmDatumY + 24);
  gdraw.printf("psi");

  /*
    % power
  */
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor (156, 52);
  gdraw.printf("%d" , powerPercentValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.printf(" %%");

  /*
    LOP state
  */
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor (234, 52);
  gdraw.setTextColor (TFT_CYAN);
  if (leaningState == 'R') gdraw.print("ROP");
  else if (leaningState == 'P') gdraw.print("PK");
  else if (leaningState == 'L') gdraw.print("LOP");
}

void drawOilElectric (int16_t oilPressValue, int16_t oilTempValue,
               int16_t volt1Value, int16_t volt2Value, int16_t ampValue) {
  /*
    Oil pressure
  */
  int16_t x0 = 20;
  int16_t y0 = 200;
  int16_t barSize = 140;
  int16_t barWidth = 30;
  int16_t maxDisplay = 100;
  int16_t minDisplay = 0;
  int8_t gradMarks = 0;
  
  myGauges.clearRanges();
  myGauges.setRange(1, true, 100, 75, TFT_RED);
  myGauges.setRange(2, true, 75, 50, TFT_GREEN);
  myGauges.setRange(3, true, 50, 0, TFT_RED);
  myGauges.setRange(4, false, 0, 0, TFT_BLACK);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  uint16_t pointerColor = TFT_WHITE;
  
  myGauges.clearPointers();
  myGauges.setPointer (1, oilPressValue, BAR_LONG, pointerColor, 'P');

  myGauges.vBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);

  /*
    Print values
  */
  gdraw.setTextColor (TFT_WHITE);
  if (oilPressValue <= 50) gdraw.setTextColor(TFT_RED);

  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor(myGauges.topDatumX - gdraw.textWidth(String(oilPressValue)) / 2, myGauges.topDatumY - 8);
  gdraw.printf("%d", oilPressValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor(myGauges.btmDatumX - gdraw.textWidth("psi") / 2, myGauges.btmDatumY + 24);
  gdraw.printf("psi");

  /*
    Oil Temperature
  */
  x0 = 85;
  y0 = 200;
  barSize = 140;
  barWidth = 30;
  maxDisplay = 250;
  minDisplay = 0;
  gradMarks = 0;

  myGauges.clearRanges();
  myGauges.setRange(1, true, 250, 210, TFT_RED);
  myGauges.setRange(2, true, 210, 160, TFT_GREEN);
  myGauges.setRange(3, true, 160, 100, TFT_ORANGE);
  myGauges.setRange(4, false, 0, 0, TFT_BLACK);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  pointerColor = TFT_WHITE;
 
  myGauges.clearPointers(); 
  myGauges.setPointer (1, oilTempValue, BAR_LONG, pointerColor, 'T');

  myGauges.vBarGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay, gradMarks);

  /*
    Print values
  */
  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);

  gdraw.setCursor (15, 20);
  gdraw.printf("     Oil        Volts & Amps");

  gdraw.setTextColor (TFT_WHITE);
  if (oilTempValue <= 50) gdraw.setTextColor(TFT_RED);

  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor(myGauges.topDatumX - gdraw.textWidth(String(oilTempValue)) / 2, myGauges.topDatumY - 8);
  gdraw.printf("%d", oilTempValue);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor(myGauges.btmDatumX - gdraw.textWidth("degF") / 2, myGauges.btmDatumY + 24);
  gdraw.printf("degF");

  /*
    Voltmeter
  */
  x0 = 219;
  y0 = 188;
  barSize = 125;
  barWidth = 19;
  maxDisplay = 160;
  minDisplay = 100;
  uint16_t  startAngle = 240;
  uint16_t  arcAngle = 60;
  bool clockWise = true;
  gradMarks = 5;

  myGauges.clearRanges();
  myGauges.setRange(1, true, 160, 146, TFT_RED);
  myGauges.setRange(2, true, 146, 132, TFT_GREEN);
  myGauges.setRange(3, true, 132, 0, TFT_RED);
  myGauges.setRange(4, false, 0, 0, TFT_BLACK);
  myGauges.setRange(5, false, 0, 0, TFT_BLACK);

  uint16_t pointerColor1 = TFT_WHITE;
  uint16_t pointerColor2 = TFT_WHITE;

  myGauges.clearPointers();
  myGauges.setPointer (1, volt1Value, ARROW_TOP, pointerColor1, '\0');
  myGauges.setPointer (2, volt2Value, ARROW_BOTTOM, pointerColor2, '\0');

  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Print values
  */
  gdraw.setTextColor (TFT_WHITE);
  if (volt1Value > 145) gdraw.setTextColor(TFT_RED);
  gdraw.setFreeFont(FSSB18);

  gdraw.setCursor (155, 52);
  gdraw.printf("%.1f", (float)volt1Value / 10);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor (250, 52);
  gdraw.printf("V1");

  gdraw.setTextColor (TFT_WHITE);
  if (volt2Value > 145) gdraw.setTextColor(TFT_RED);
  gdraw.setFreeFont(FSSB18);
  gdraw.setCursor (155, 128);
  gdraw.printf("%.1f", (float)volt2Value / 10);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor (250, 128);
  gdraw.printf("V2");

  /*
    Ammeter
  */
  x0 = 219;
  y0 = 281;
  barSize = 125;
  barWidth = 20;
  maxDisplay = 600;
  minDisplay = 0;
  startAngle = 240;
  arcAngle = 60;
  clockWise = true;
  gradMarks = 5;

  myGauges.clearRanges();
  myGauges.setRange (1, true, 600, 500, TFT_ORANGE);
  myGauges.setRange (2, true, 500, 20, TFT_GREEN);
  myGauges.setRange (3, true, 20, 0, TFT_RED);
  myGauges.setRange (4, false, 0, 0, TFT_BLACK);
  myGauges.setRange (5, false, 0, 0, TFT_BLACK);

  myGauges.clearPointers();
  myGauges.setPointer (1, ampValue, ARROW_BOTTOM, TFT_WHITE, '\0');
  myGauges.setPointer (2, 0, 0, TFT_WHITE, '\0');
  myGauges.setPointer (3, 0, 0, TFT_WHITE, '\0');
  myGauges.setPointer (4, 0, 0, TFT_WHITE, '\0');

  pointerColor = TFT_WHITE;

  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Print values
  */
  gdraw.setTextColor (TFT_WHITE);
  if (volt1Value <= 20) gdraw.setTextColor(TFT_RED);
  gdraw.setFreeFont(FSSB18);

  gdraw.setCursor (155, 225);
  gdraw.printf("%.1f", (float)ampValue / 10);

  gdraw.setTextColor (TFT_CYAN);
  gdraw.setFreeFont(FSSB12);
  gdraw.setCursor (250, 225);
  gdraw.printf("A");
}

void drawVolts(int16_t volt1Value, int16_t volt2Value) {

  /*
      Large Voltmeter
  */
  uint16_t x0 = 159;
  uint16_t y0 = 375;
  uint16_t barSize = 280;
  uint16_t barWidth = 45;
  uint16_t maxDisplay = 160;
  uint16_t minDisplay = 100;
  uint16_t startAngle = 240;
  uint16_t arcAngle = 60;
  bool clockWise = true;
  uint16_t gradMarks = 6;

  myGauges.clearRanges();
  myGauges.setRange (1, true, 159, 146, TFT_RED);
  myGauges.setRange (2, true, 146, 132, TFT_GREEN);
  myGauges.setRange (3, true, 132, 0, TFT_WHITE);
  myGauges.setRange (4, false, 0, 0, TFT_BLACK);
  myGauges.setRange (5, false, 0, 0, TFT_BLACK);

  uint16_t pointerColor1 = TFT_WHITE;
  uint16_t pointerColor2 = TFT_WHITE;
  
  myGauges.clearPointers();
  myGauges.setPointer (1, 130, BUG_TOP, TFT_CYAN, '\0');
  myGauges.setPointer (2, 120, BUG_BOTTOM, TFT_MAGENTA, '\0');
  myGauges.setPointer (3, volt1Value, ARROW_TOP, pointerColor1, '1');
  myGauges.setPointer (4, volt2Value, ARROW_BOTTOM, pointerColor2, '2');


  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Print values
  */
  gdraw.setFreeFont(FSSB24);
  gdraw.setTextColor (TFT_CYAN);
  gdraw.setTextDatum (TC_DATUM);

  gdraw.drawString("VOLTS", 159, 0, GFXFF);

  gdraw.setTextColor (TFT_WHITE);
  if (volt1Value > 145) gdraw.setTextColor(TFT_RED);

  gdraw.setCursor (0, 95);
  gdraw.printf("%.1f", (float)volt1Value / 10);

  gdraw.setTextColor (TFT_WHITE);
  if (volt2Value > 145) gdraw.setTextColor(TFT_RED);
  gdraw.setFreeFont(FSSB24);
  gdraw.setCursor (0, 220);
  gdraw.printf("%.1f", (float)volt2Value / 10);
}

void drawAmps (int16_t ampValue) {

  /*
    Large Ammeter
  */
  uint16_t x0 = 159;
  uint16_t y0 = 375;
  uint16_t barSize = 280;
  uint16_t barWidth = 45;
  uint16_t maxDisplay = 600;
  uint16_t minDisplay = 0;
  uint16_t startAngle = 240;
  uint16_t arcAngle = 60;
  bool clockWise = true;
  uint16_t gradMarks = 6;

  myGauges.clearRanges();
  myGauges.setRange (1, true, 600, 50, TFT_GREEN);
  myGauges.setRange (2, true, 50, 0, TFT_RED);

  uint16_t pointerColor1 = TFT_WHITE;

  myGauges.clearPointers();
  myGauges.setPointer (1, ampValue, NEEDLE, pointerColor1, '\0');

  myGauges.arcGraph (x0, y0, barSize, barWidth , maxDisplay, minDisplay,
                     startAngle, arcAngle, clockWise, gradMarks);

  /*
    Print values
  */
  gdraw.setFreeFont(FSSB24);
  gdraw.setTextColor (TFT_CYAN);
  gdraw.setTextDatum (TC_DATUM);

  gdraw.drawString("AMPS", 159, 0, GFXFF);

  gdraw.setTextColor (TFT_WHITE);
  if (ampValue < 50) gdraw.setTextColor(TFT_RED);

  gdraw.setCursor (110, 80);
  gdraw.printf("%.1f", (float)ampValue / 10);
}
