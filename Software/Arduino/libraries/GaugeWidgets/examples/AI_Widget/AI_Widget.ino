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
  Attitude Indicator composite gauge example using the GaugeWidgets Library.
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
void AiGraph  (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t pitch, int16_t roll, int16_t yaw);

void pitchGraph(int16_t pitch, int16_t roll, int16_t px0, int16_t py0, uint8_t scale);               
       
uint16_t displayBrightness = 100;       
/*
  Example data values. Usually these are derived from a data input function.
*/  
int16_t px0 = 159;
int16_t py0 = 119;
int16_t pitch = 0;
int16_t roll = 20;
int16_t yaw = 0;
int16_t arcSize = 115;
int16_t arcWidth = 15;
int16_t maxDisplay = 360;
int16_t minDisplay = 0;
int16_t startAngle = -roll;
int16_t arcAngle = 360;
int16_t clockWise = true;
uint8_t gradMarks = 0;

uint16_t pitchdirection = 4;
uint16_t rolldirection = 2;

void setup() {

  M5.begin();
  M5.Lcd.fillScreen(TFT_WHITE);
  delay (100);

  gdraw.setColorDepth(8);
  gdraw.createSprite(SCREEN_X, SCREEN_Y);
  gdraw.setFreeFont(FSSB9);
  gdraw.setTextWrap(false); // Very important for scaling
}

void loop() {

  gdraw.fillSprite (TFT_CYAN);
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
  AiGraph (px0, py0, arcSize, arcWidth, maxDisplay, minDisplay, startAngle, arcAngle, clockWise,
           gradMarks, pitch, roll, yaw);

  gdraw.pushSprite (0, 0);
  
  pitch += pitchdirection;
  roll += rolldirection;

  if (pitch >= 90) pitchdirection = -2;
  if (roll >= 180) rolldirection = -2;
  if (pitch <= -90) pitchdirection = +2;
  if (roll <= -180) rolldirection = +2;
  
}

void AiGraph (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t pitch, int16_t roll, int16_t yaw) {        
  
/*
    Draw Horizon first

<----------------------p0----------------------> about +/- 2x screen width
       |
       |
       | Pitch
      \ /
      p1---------------pc---------------p2   
      |                                 |
      |                                 |
      |                                 |
      p3--------------------------------p4 
      
  */
 {
    arcSize = 160;
   
  /*
  Establish a wide horizontal baseline segment
  */ 
    float xRotate = (2.0f * (float)SCREEN_X) * cos (roll * DEG_TO_RAD); 
    float yRotate = (2.0f * (float)SCREEN_X) * sin (roll * DEG_TO_RAD); 

  /*
  Adjust it for roll and pitch
  */
    float pxc = px0 + pitch * SCREEN_Y/80 * sin (roll * DEG_TO_RAD ); 
    float pyc = py0 + pitch * SCREEN_Y/80 * cos (roll * DEG_TO_RAD ); 

    float px1 = pxc - xRotate;
    float py1 = pyc + yRotate;;;
    
    float px2 = pxc + xRotate;
    float py2 = pyc - yRotate;

  /*  
   Compute offset parallel line segment to establish a wide bar
  */
    float px3 = px1 + 3 * SCREEN_Y * cos(roll * DEG_TO_RAD - HALF_PI);
    float py3 = py1 - 3 * SCREEN_Y * sin(-roll * DEG_TO_RAD - HALF_PI);
                      
    float px4 = px2 - 3 * SCREEN_Y * cos(roll * DEG_TO_RAD + HALF_PI);
    float py4 = py2 + 3 * SCREEN_Y * sin(roll * DEG_TO_RAD + HALF_PI);
 
/*
 Fill the bar.  Will be automatically clipped outside of screen bounds
*/ 
    
    gdraw.fillTriangle (px1, py1, px2, py2, px3, py3, TFT_OLIVE);
    gdraw.fillTriangle (px3, py3, px4, py4, px2, py2, TFT_OLIVE);
    gdraw.drawLine (px1, py1, px2, py2, TFT_BLACK);
    gdraw.drawLine (px3, py3, px4, py4, TFT_BLUE);
    gdraw.fillCircle (pxc, pyc, 3, TFT_BLACK);
       
//  gdraw.drawString("1", px1, py1);
//  gdraw.drawString("2", px2, py2);
//  gdraw.drawString("3", px3, py3);
//  gdraw.drawString("4", px4, py4);

}
  /*
    Draw pitch graphic in background
  */
  pitchGraph(pitch, roll, px0, py0, 10);

  /*
    Draw Horizon Indicator
  */  
  arcSize = 115;
  arcWidth = 15;
  
  myGauges.clearRanges();

  myGauges.clearPointers();
  myGauges.setPointer (1, 0,   BAR_LONG, TFT_WHITE, '\0');
  myGauges.setPointer (2, 180, BAR_LONG, TFT_WHITE, '\0'); 
  myGauges.setPointer (3, 210, BAR_LONG, TFT_WHITE, '\0'); 
  myGauges.setPointer (4, 240, BAR_LONG, TFT_WHITE, '\0'); 
  myGauges.setPointer (5, 270, ARROW_OUT, TFT_YELLOW, '\0'); 
  myGauges.setPointer (6, 300, BAR_LONG, TFT_WHITE, '\0'); 
  myGauges.setPointer (7, 330, BAR_LONG, TFT_WHITE, '\0'); 
  myGauges.setPointer (8, 0, 0,  0, '\0');

  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     -roll, arcAngle, clockWise, gradMarks);
				 
  /*
    Draw additional small markers
  */	
  arcSize = 115;
  arcWidth = 15;
  
  myGauges.clearRanges();
 
  myGauges.clearPointers();
  myGauges.setPointer (1, 250, BAR_SHORT, TFT_WHITE, '\0');
  myGauges.setPointer (2, 260, BAR_SHORT, TFT_WHITE, '\0'); 
  myGauges.setPointer (3, 280, BAR_SHORT, TFT_WHITE, '\0');
  myGauges.setPointer (4, 290, BAR_SHORT, TFT_WHITE, '\0');
  myGauges.setPointer (5, 225, ROUND_DOT, TFT_WHITE, '\0');
  myGauges.setPointer (6, 315, ROUND_DOT, TFT_WHITE, '\0');
  myGauges.setPointer (7, 0, 0,  0, '\0');
  myGauges.setPointer (8, 0, 0,  0, '\0');
 
  
  myGauges.arcGraph (px0, py0, arcSize, arcWidth , maxDisplay, minDisplay,
                     -roll, arcAngle, clockWise, gradMarks);					 
  

  /*
    Draw Airplane
	
                   p0
	p1----------p2 o  p3---------p4
			     \   /
			      \ /
				   p5
  */
 
  arcSize = 100;
  arcWidth = 15;
  
  uint16_t px1 = px0 - arcSize; 
  uint16_t py1 = py0;
  uint16_t px2 = px0 - arcSize / 4 ; 
  uint16_t py2 = py0;
  uint16_t px3 = px0 + arcSize / 4 ; 
  uint16_t py3 = py0;
  uint16_t px4 = px0 + arcSize; 
  uint16_t py4 = py0;
  uint16_t px5 = px0; 
  uint16_t py5 = py0 + arcSize / 4;
  
  gdraw.fillCircle (px0, py0, 2 * SCREEN_Y/80, TFT_YELLOW);   // 2 degree radius circle
  gdraw.drawCircle (px0, py0, 2 * SCREEN_Y/80, TFT_BLACK);  
  
  gdraw.drawFastHLine (px1, py1, 3*arcSize/4, TFT_YELLOW);
  gdraw.drawLine (px2, py2, px5, py5, TFT_YELLOW);
  gdraw.drawLine (px5, py5, px3, py3, TFT_YELLOW);
  gdraw.drawFastHLine (px3, py3, 3*arcSize/4, TFT_YELLOW);

  gdraw.drawFastHLine (px1, py1-1, 3*arcSize/4, TFT_YELLOW);
  gdraw.drawLine (px2, py2-1, px5, py5-1, TFT_YELLOW);
  gdraw.drawLine (px5, py5-1, px3, py3-1, TFT_YELLOW);
  gdraw.drawFastHLine (px3, py3-1, 3*arcSize/4, TFT_YELLOW);
  
  gdraw.drawFastHLine (px1, py1-2, 3*arcSize/4, TFT_YELLOW);
  gdraw.drawLine (px2, py2-2, px5, py5-2, TFT_YELLOW);
  gdraw.drawLine (px5, py5-2, px3, py3-2, TFT_YELLOW);
  gdraw.drawFastHLine (px3, py3-2, 3*arcSize/4, TFT_YELLOW);
 
  gdraw.drawFastHLine (px1, py1-3, 3*arcSize/4, TFT_BLACK);
  gdraw.drawLine (px2, py2-3, px5, py5-3, TFT_BLACK);
  gdraw.drawLine (px5, py5-3, px3, py3-3, TFT_BLACK);
  gdraw.drawFastHLine (px3, py3-3, 3*arcSize/4, TFT_BLACK);
  
  gdraw.drawFastHLine (px1, py1+1, 3*arcSize/4, TFT_YELLOW);
  gdraw.drawLine (px2, py2+1, px5, py5+1, TFT_YELLOW);
  gdraw.drawLine (px5, py5+1, px3, py3+1, TFT_YELLOW);
  gdraw.drawFastHLine (px3, py3+1, 3*arcSize/4, TFT_YELLOW);
									   
  gdraw.drawFastHLine (px1, py1+2, 3*arcSize/4, TFT_YELLOW);
  gdraw.drawLine (px2, py2+2, px5, py5+2, TFT_YELLOW);
  gdraw.drawLine (px5, py5+2, px3, py3+2, TFT_YELLOW);
  gdraw.drawFastHLine (px3, py3+2, 3*arcSize/4, TFT_YELLOW);
									   
  gdraw.drawFastHLine (px1, py1+3, 3*arcSize/4, TFT_BLACK);
  gdraw.drawLine (px2, py2+3, px5, py5+3, TFT_BLACK);
  gdraw.drawLine (px5, py5+3, px3, py3+3, TFT_BLACK);
  gdraw.drawFastHLine (px3, py3+3, 3*arcSize/4, TFT_BLACK);

  gdraw.drawFastVLine (px1, py1-3, 6, TFT_BLACK);
  gdraw.drawFastVLine (px4, py4-3, 6, TFT_BLACK);

  /*
	Draw top pointer
  */
  px1 = px0;
  py1 = py0 - arcSize + arcWidth / 2;
  px2 = px0 - arcWidth / 2;
  py2 = py0 - arcSize + 2 * arcWidth;
  px3 = px0 + arcWidth / 2;
  py3 = py0 - arcSize + 2 * arcWidth;
  
  gdraw.fillTriangle (px1, py1, px2, py2, px3, py3, TFT_YELLOW);
					 
  gdraw.drawLine (px1, py1, px2, py2, TFT_BLACK);
  gdraw.drawLine (px2, py2, px3, py3, TFT_BLACK);
  gdraw.drawLine (px3, py3, px1, py1, TFT_BLACK);
}

void pitchGraph(int16_t pitch, int16_t roll, int16_t px0, int16_t py0, uint8_t scale) {  

  /* Draw Pitch scale
    
    --------- 40
      -----
    --------- 30
      -----
    --------- 20
      -----
  p3---------p4 10
    p1-----p2 
      p0+
      
  */

   
 /*
  Establish a horizontal baseline segment
  */ 

  float px1, px2, px3, px4, px5;
  float py1, py2, py3, py4, py5;
  float xRotate;
  float yRotate;   
  
  float pxc = px0 + pitch * SCREEN_Y/80  * sin (roll * DEG_TO_RAD ); 
  float pyc = py0 + pitch * SCREEN_Y/80 * cos (roll * DEG_TO_RAD ); 

  /*
    Compute pitch scale
  */
    
    gdraw.setTextDatum(MC_DATUM);
    
    xRotate = (0.10f * arcSize) * cos (roll * DEG_TO_RAD); // establish the width.
    yRotate = (0.10f * arcSize) * sin (roll * DEG_TO_RAD); 
    
    px1 = pxc - xRotate*1.0f; 
    py1 = pyc + yRotate*1.0f;  
                           
    px2 = pxc + xRotate*1.0f;
    py2 = pyc - yRotate*1.0f;
    
  for (int16_t i = -85; i <= 85; i+= scale){

// Marks every 5 degrees
     
    px3 = px1 - (i * SCREEN_Y/80) * cos(roll * DEG_TO_RAD - HALF_PI);
    py3 = py1 + (i * SCREEN_Y/80) * sin(-roll * DEG_TO_RAD - HALF_PI);
                         
    px4 = px2 + (i * SCREEN_Y/80) * cos(roll * DEG_TO_RAD + HALF_PI);
    py4 = py2 - (i * SCREEN_Y/80) * sin(roll * DEG_TO_RAD + HALF_PI);

    gdraw.drawLine (px3, py3, px4, py4, TFT_BLACK); 
}

    px1 = pxc - xRotate*2.0f; 
    py1 = pyc + yRotate*2.0f;  
                         
    px2 = pxc + xRotate*2.0f;
    py2 = pyc - yRotate*2.0f; 

  for (int16_t i = -90; i <= 90; i+= scale){

// Marks every 5 degrees
     
    px3 = px1 - (i * SCREEN_Y/80) * cos(roll * DEG_TO_RAD - HALF_PI);
    py3 = py1 + (i * SCREEN_Y/80) * sin(-roll * DEG_TO_RAD - HALF_PI);
                         
    px4 = px2 + (i * SCREEN_Y/80) * cos(roll * DEG_TO_RAD + HALF_PI);
    py4 = py2 - (i * SCREEN_Y/80) * sin(roll * DEG_TO_RAD + HALF_PI);

    gdraw.setCursor(px4, py4);
    gdraw.drawLine (px3, py3, px4, py4, TFT_BLACK); 
    
    px4 += xRotate*0.75f; 
    py4 -= yRotate*0.75f; 
    //myGauges.printNum ("123456789", 160, 120, 8, 12, roll, TFT_BLACK, MR_DATUM);
    myGauges.printNum (String (i)+"o", px4, py4, 8, 12, roll, TFT_BLACK, ML_DATUM);
  }    
} // end pitchGraph

