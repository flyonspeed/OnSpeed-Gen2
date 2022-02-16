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
  GaugeWidgets.cpp, the Engine Gauge Widget Library
    by V.R. ("Voltar") Little
    Version 2.0, April 2, 2020

  This library uses the M5Stack hardware, but is easily ported to other ESP32 based devices that use TFT displays.
  It provides vertical,  horizontal, or arc (circular) widgets.

  The vertical and horizontal widgets use integer math for high speed drawing, but input pointer values may need
  to be scaled up to prevent integer rounding errors. Typically, multiplying single-digit pointer values by 
  10, 100 or 1000 will minize display errors.

  The arc widgets use mostly floating point math and will run slower.

   Widget Functions:
    <vBarGraph> defines the position and size of a vertical gauge.
    <hBarGraph> defines the position and size of a horizontal gauge.
    <arcGraph> defines the position, size and arc angles
      (both clockwise and counterclockwise)for circular or arc gauges.

   General Functions:
    <setPointer> defines the attributes of the discrete pointers and the pointer tags allowed per gauge.
    <clearPointers> clears all previously defined pointers variables.
    <setRange> defines the colored range bars for each gauge.
    <clearRanges> clears all previously defined ranges.
    <printNum> allows the printing of scalable, rotatable numbers and common arithmetical symbols.

   Library Variables:
    See GaugeWidgets.h file for a complete listing of library variables.
*/

#include <M5Stack.h>
#include <Free_Fonts.h>
#include "GaugeWidgets.h"
#define GFXFF 1
#define ARCSTEP 0.01745329252f   // Smaller steps for more arc accuracy, larger steps for faster execution for arc gauges.

#define TEXTSIZE FSSB12  // There are practical limits to font styles and sizes.  12 and 18 point sizes have been tested.
#define SCALEUP 4096     // upscaling integer math routines prevents significant rounding errors.

extern TFT_eSprite gdraw;

Gauges::Gauges() {}

/*
   Vertical bar graph gauge
*/

void Gauges::vBarGraph (int16_t x0, int16_t y0, int16_t barSize, int16_t barWidth, int16_t maxDisplay, int16_t minDisplay, uint8_t gradMarks) {

  int32_t _Width = barWidth;
  int32_t _pointerAdj[NUM_POINTERS + 1];
  int32_t _normAxis; 

  if ((maxDisplay - minDisplay) != 0) _normAxis  =  SCALEUP * barSize  / (maxDisplay - minDisplay) - 1;
  else _normAxis = 1; //don't ever want to divide by 0!
   
  for (int i = 1; i <= NUM_POINTERS; i++){
    _pointerAdj[i] = _normAxis * pointerValue[i] / SCALEUP;
  }
  
  int32_t _MaxDisplay = _normAxis * maxDisplay / SCALEUP;
  int32_t _MinDisplay = _normAxis * minDisplay / SCALEUP;
  int32_t _X0 = x0;
  int32_t _Y0 = y0;

  /*
     Normalize all of the gauge ranges.
   */
   
  _Y0 = _Y0 + _MinDisplay; // Offset widget to eliminate black space at bottom

  bool rangeisValid = false;
  
  /*
     Draw all of the enabled display bars.  Setting non-overlaping ranges allows for black bars between ranges.
   */  
  for (int16_t i = 1; i <= NUM_RANGES; i++){
    rangeTop[i] = rangeTop[i] * _normAxis / SCALEUP;
    rangeBot[i] = rangeBot[i] * _normAxis / SCALEUP;
    
    if (rangeTop[i] > _MaxDisplay) rangeTop[i] = _MaxDisplay;
    if (rangeBot[i] < _MinDisplay) rangeBot[i] = _MinDisplay;
    
    if (rangeValid[i]) {
	  gdraw.fillRect (_X0, _Y0 - rangeTop[i], _Width, rangeTop[i] - rangeBot[i], rangeColor[i]);
	  gdraw.drawRect (_X0, _Y0 - rangeTop[i], _Width, rangeTop[i] - rangeBot[i], TFT_BLACK);
      rangeisValid = true;
    }
  }
	
 /*
     Draw a box around the gauge if any ranges enabled
  */
  if (rangeisValid) gdraw.drawRect(_X0, _Y0 - _MaxDisplay, _Width,  _MaxDisplay - _MinDisplay, TFT_DARKGREY);

  /*
     Draw gauge graduations
  */
  if (gradMarks > 1) {	
  
    int16_t delta = (_MaxDisplay - _MinDisplay) / gradMarks; 
    for (int16_t i = 0; i < _MaxDisplay; i += delta ) { // major marks
      int16_t _X1 = _X0;
      int16_t _X2 = _X0  + _Width  - 1 ;
      int16_t _Y1 = _Y0  - _MinDisplay - i;
      
      gdraw.drawLine (_X1, _Y1, _X2, _Y1, TFT_WHITE);
	  gdraw.drawLine (_X1, _Y1-1, _X2, _Y1-1, TFT_WHITE);
      gdraw.drawLine (_X1, _Y1+1, _X2, _Y1+1, TFT_WHITE);
	  gdraw.drawLine (_X1, _Y1-2, _X2, _Y1-2, TFT_BLACK);
      gdraw.drawLine (_X1, _Y1+2, _X2, _Y1+2, TFT_BLACK);
      
      gdraw.drawLine (_X1, _Y1-2, _X1, _Y1+2 , TFT_BLACK);
      gdraw.drawLine (_X2, _Y1-2, _X2, _Y1+2, TFT_BLACK);
    }

    for (int16_t i = delta / 2; i < _MaxDisplay; i += delta ) { // minor marks
      int16_t _X1 = _X0  + _Width / 4;
      int16_t _X2 = _X0  + 3 * _Width / 4 - 1;
      int16_t _Y1 = _Y0 - _MinDisplay - i;
      
      gdraw.drawLine (_X1, _Y1, _X2, _Y1, TFT_WHITE);
	  gdraw.drawLine (_X1, _Y1-1, _X2, _Y1-1, TFT_WHITE);
      gdraw.drawLine (_X1, _Y1+1, _X2, _Y1+1, TFT_WHITE);
	  gdraw.drawLine (_X1, _Y1-2, _X2, _Y1-2, TFT_BLACK);
      gdraw.drawLine (_X1, _Y1+2, _X2, _Y1+2, TFT_BLACK);
      
      gdraw.drawLine (_X1, _Y1-2, _X1, _Y1+2 , TFT_BLACK);
      gdraw.drawLine (_X2, _Y1-2, _X2, _Y1+2, TFT_BLACK);
    }
  }

  /*
     Pointer limits.
  */
  for (int16_t i = 1; i <= NUM_POINTERS; i++) {
  
    if (_pointerAdj[i] < _MinDisplay) _pointerAdj[i] = _MinDisplay;
    if (_pointerAdj[i] > _MaxDisplay) _pointerAdj[i] = _MaxDisplay;

    switch (pointerType[i]){
      case ARROW_LEFT: MarkArrowLeft(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;// arrow on left
      case ARROW_RIGHT: MarkArrowRight(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break; // arrow on right
      case BAR_LONG: MarkHbar(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;      // long horizontal bar
      case BAR_SHORT: MarkHbarShort(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;     // short horizontal bar
      case BUG_LEFT: MarkBugLeft(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;    // bug on left  
      case BUG_RIGHT: MarkBugRight(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;   // bug on right 
      case ROUND_DOT: MarkHdot(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;   // round dot
      case 8: break; 
      default: break;
    } 
  }
 
  /*
     Text marker Top
   */   
  topDatumX = _X0 +  _Width/2; 
  topDatumY = _Y0 -  _MinDisplay - barSize;

  /*
     Text marker Bottom
   */  
  btmDatumX = topDatumX;
  btmDatumY = _Y0 - _MinDisplay; 
}

/*
   Horizontal bar graph gauge
 */
void Gauges::hBarGraph (int16_t x0, int16_t y0, int16_t barSize, int16_t barWidth, int16_t maxDisplay, int16_t minDisplay, uint8_t gradMarks) {
 
  int32_t _Width = barWidth;
  int32_t _pointerAdj[NUM_POINTERS + 1];
  int32_t _normAxis; 

  if ((maxDisplay - minDisplay) != 0) _normAxis  =  SCALEUP * barSize  / (maxDisplay - minDisplay) - 1;
  else _normAxis = 1; //don't ever want to divide by 0!
   
  for (int i = 1; i <= NUM_POINTERS; i++){
    _pointerAdj[i] = _normAxis * pointerValue[i] / SCALEUP;
  }
  
  int32_t _MaxDisplay = _normAxis * maxDisplay / SCALEUP;
  int32_t _MinDisplay = _normAxis * minDisplay / SCALEUP;
  int32_t _X0 = x0;
  int32_t _Y0 = y0;

  /*
     Normalize all of the gauge ranges.
   */
   
  _X0 = _X0 - _MinDisplay; // Offset widget to eliminate black space at the left.

  bool rangeisValid = false;
  
  /*
     Draw all of the enabled display bars.  Setting non-overlaping ranges allows for black bars between ranges.
   */  
  for (int16_t i = 1; i <= NUM_RANGES; i++){
    rangeTop[i] = rangeTop[i] * _normAxis / SCALEUP;
    rangeBot[i] = rangeBot[i] * _normAxis / SCALEUP;
    
    if (rangeTop[i] > _MaxDisplay) rangeTop[i] = _MaxDisplay;
    if (rangeBot[i] < _MinDisplay) rangeBot[i] = _MinDisplay;
    
    if (rangeValid[i]) {
      gdraw.fillRect (_X0 + rangeBot[i], _Y0, rangeTop[i] - rangeBot[i], _Width, rangeColor[i]);
      gdraw.drawRect (_X0 + rangeBot[i], _Y0, rangeTop[i] - rangeBot[i], _Width, TFT_BLACK);
      rangeisValid = true;
    }
  }

  if (rangeisValid) gdraw.drawRect (_X0 + _MinDisplay, _Y0, _MaxDisplay - _MinDisplay, _Width, TFT_DARKGREY);

  /*
     Draw gauge graduations
   */
  if (gradMarks > 1) {

    int16_t delta = (_MaxDisplay - _MinDisplay) / gradMarks;
    for (int16_t i = 0; i < _MaxDisplay; i += delta ) { // major marks
      int16_t _Y1 = _Y0;
      int16_t _Y2 = _Y0  + _Width - 1;
      int16_t _X1 = _X0  + _MinDisplay + i;
      gdraw.drawLine (_X1, _Y1, _X1, _Y2, TFT_WHITE);
	  gdraw.drawLine (_X1+1, _Y1, _X1+1, _Y2, TFT_WHITE);
	  gdraw.drawLine (_X1-1, _Y1, _X1-1, _Y2, TFT_WHITE);
 	  gdraw.drawLine (_X1+2, _Y1, _X1+2, _Y2, TFT_BLACK);
	  gdraw.drawLine (_X1-2, _Y1, _X1-2, _Y2, TFT_BLACK);
	  gdraw.drawLine (_X1-2, _Y1, _X1+2, _Y1, TFT_BLACK);
	  gdraw.drawLine (_X1-2, _Y2, _X1+2, _Y2, TFT_BLACK);
     
    }

    for (int16_t i = delta / 2; i < _MaxDisplay; i += delta ) { // minor marks
      int16_t _Y1 = _Y0  + _Width / 4;
      int16_t _Y2 = _Y0  + 3 * _Width / 4 - 1;
      int16_t _X1 = _X0 + _MinDisplay + i;
      gdraw.drawLine (_X1, _Y1, _X1, _Y2, TFT_WHITE);
	  gdraw.drawLine (_X1+1, _Y1, _X1+1, _Y2, TFT_WHITE);
	  gdraw.drawLine (_X1-1, _Y1, _X1-1, _Y2, TFT_WHITE);
 	  gdraw.drawLine (_X1+2, _Y1, _X1+2, _Y2, TFT_BLACK);
	  gdraw.drawLine (_X1-2, _Y1, _X1-2, _Y2, TFT_BLACK);
      gdraw.drawLine (_X1-2, _Y1, _X1+2, _Y1, TFT_BLACK);
	  gdraw.drawLine (_X1-2, _Y2, _X1+2, _Y2, TFT_BLACK);
    }
  }

   /*
     Pointer limits.
  */
  for (int16_t i = 1; i <= NUM_POINTERS; i++) {
  
    if (_pointerAdj[i] < _MinDisplay) _pointerAdj[i] = _MinDisplay;
    if (_pointerAdj[i] > _MaxDisplay) _pointerAdj[i] = _MaxDisplay;

    switch (pointerType[i]){
      case ARROW_TOP: MarkArrowTop(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;// arrow on top
      case ARROW_BOTTOM: MarkArrowBottom(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break; // arrow on bottom
      case BAR_LONG: MarkVbar(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;     // long horizontal bar
      case BAR_SHORT: MarkVbarShort(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;     // short horizontal bar
      case BUG_TOP: MarkBugTop(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;    // bug on top  
      case BUG_BOTTOM: MarkBugBot(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;   // bug on bottom 
      case ROUND_DOT: MarkVdot(_X0, _Y0, _Width, _pointerAdj[i], pointerTag[i], pointerColor[i]); break;   // round dot 
      case 8: break; 
      default: break;
    } 
  }

  /*
     Text marker Right
   */
  topDatumX = _X0 -  _MinDisplay + barSize;
  topDatumY = _Y0 + _Width/2; 
  
  /*
     Text marker Left
   */  
  btmDatumX = _X0 + _MinDisplay; 
  btmDatumY = topDatumY; ;
}

/*
   Arc bar graph gauge, both clockwise and counterclockwise.
*/
void Gauges::arcGraph (int16_t x0, int16_t y0, int16_t barSize, int16_t barWidth, int16_t maxDisplay, int16_t minDisplay,
                       int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks) {

  /*
    Define floating point variables derived from integer parameters
  */                      
  float _X0 = x0;
  float _Y0 = y0;
  float _Radius = barSize;
  float _Width = barWidth; 
  float _StartAngle = DEG_TO_RAD * startAngle;
  float _ArcAngle = abs(DEG_TO_RAD * arcAngle);
  float _normAxis;
  float _pointerAdj[NUM_POINTERS + 1];
  float _rangeTopAdj[NUM_RANGES + 1];
  float _rangeBotAdj[NUM_RANGES + 1];
  
  if ((maxDisplay - minDisplay) != 0) _normAxis = _ArcAngle / (maxDisplay - minDisplay);
  else _normAxis = 1.0f;

  float _MaxDisplay = _normAxis * maxDisplay;
  float _MinDisplay = _normAxis * minDisplay;
  
  float _theta = _StartAngle - _MinDisplay; // for widget rotation
  float _X1, _X2, _X3, _X4, _Y1, _Y2, _Y3, _Y4;
  float _cosA, _cosB, _sinA, _sinB;

  
  /*
     Scale all the pointers.
  */  
  for (int i = 1; i <= NUM_POINTERS; i++){
    _pointerAdj[i] = _normAxis * pointerValue[i];
  }

  /*
     Normalize all of the gauge ranges.
  */
  for (int16_t i = 1; i <= NUM_RANGES; i++){
    _rangeTopAdj[i] = rangeTop[i] * _normAxis; // SCALEUP;
    _rangeBotAdj[i] = rangeBot[i] * _normAxis; // SCALEUP;
    
    if (_rangeTopAdj[i] > _MaxDisplay) _rangeTopAdj[i] = _MaxDisplay;
    if (_rangeBotAdj[i] < _MinDisplay) _rangeBotAdj[i] = _MinDisplay;
 

	/*
	   Draw all of the enabled display sectors.  Setting non-overlaping ranges allows for blank bars between ranges.
	*/

	if (rangeValid[i]) {
	
	  for (float j = _rangeBotAdj[i]; j < (_rangeTopAdj[i]); j += ARCSTEP) {

		if (clockWise) {
		  _cosA = cos(j + _theta);
		  _sinA = sin(j + _theta);
		  _cosB = cos(j + _theta + ARCSTEP);
		  _sinB = sin(j + _theta + ARCSTEP);
		}
		else { // counterClockwise
		  _cosA = -cos(j + _theta);
		  _sinA =  sin(j + _theta);
		  _cosB = -cos(j + _theta - ARCSTEP);
		  _sinB =  sin(j + _theta - ARCSTEP);
		}
   
		_X1 = _X0 + _Radius * _cosA;
		_Y1 = _Y0 + _Radius * _sinA;
		_X2 = _X0 + (_Radius - _Width) * _cosA;
		_Y2 = _Y0 + (_Radius - _Width) * _sinA;
		_X3 = _X0 + _Radius * _cosB;
		_Y3 = _Y0 + _Radius * _sinB;
		_X4 = _X0 + (_Radius - _Width) * _cosB;
		_Y4 = _Y0 + (_Radius - _Width) * _sinB;

		gdraw.fillTriangle(_X1, _Y1, _X2, _Y2, _X3, _Y3, rangeColor[i]);
		gdraw.fillTriangle(_X3, _Y3, _X2, _Y2, _X4, _Y4, rangeColor[i]);
	  }

	  if (clockWise) gdraw.drawLine (_X3, _Y3, _X4, _Y4, TFT_BLACK);
	  else gdraw.drawLine (_X1, _Y1, _X2, _Y2, TFT_BLACK);
	}
  }

  /*
     Draw dial graduations
  */

  if (gradMarks > 1) {
	for (float i = 0; i <= _ArcAngle; i += _ArcAngle / gradMarks) { // major marks
      float _angle = i+_StartAngle;
	  float _cosA = cos (_angle);
	  float _sinA = sin (_angle);
	  float _cosB = -sin (_angle);
	  float _sinB = cos (_angle);
	  
	  if (!clockWise) {
		_cosA = -_cosA;
		_cosB = -_cosB;
	  }
	  
	  float _X1 = _X0 + (_Radius - 1.25f * _Width) * _cosA;
	  float _Y1 = _Y0 + (_Radius - 1.25f * _Width) * _sinA;
	  float _X2 = _X0 + _Radius * _cosA;
	  float _Y2 = _Y0 + _Radius * _sinA;
	  gdraw.drawLine (_X1, _Y1, _X2, _Y2, TFT_WHITE);
	  
	  float _X1X = _X1 + _cosB;
	  float _Y1X = _Y1 + _sinB;      	 
	  float _X2X = _X2 + _cosB;
	  float _Y2X = _Y2 + _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_WHITE);	
	  
	  _X1X = _X1 - _cosB;
	  _Y1X = _Y1 - _sinB;      	 
	  _X2X = _X2 - _cosB;
	  _Y2X = _Y2 - _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_WHITE);				
  
	  _X1X = _X1 + 2.0f * _cosB;
	  _Y1X = _Y1 + 2.0f * _sinB;      	 
	  _X2X = _X2 + 2.0f * _cosB;
	  _Y2X = _Y2 + 2.0f * _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_BLACK);	
	  
	  _X1X = _X1 - 2.0f * _cosB;
	  _Y1X = _Y1 - 2.0f * _sinB;      	 
	  _X2X = _X2 - 2.0f * _cosB;
	  _Y2X = _Y2 - 2.0f * _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_BLACK);		
	}

	for (float i = _ArcAngle / (gradMarks * 2); i < _ArcAngle; i += _ArcAngle / gradMarks) { // minor marks
      float _angle = i+_StartAngle;
	  float _cosA = cos (_angle);
	  float _sinA = sin (_angle);
	  float _cosB = -sin (_angle);
	  float _sinB = cos (_angle);
	  
	  if (!clockWise) {
		_cosA = -_cosA;
		_cosB = -_cosB;
	  }

	  float _X1 = _X0 + (_Radius - 0.75f * _Width) * _cosA;
	  float _Y1 = _Y0 + (_Radius - 0.75f * _Width) * _sinA;
	  float _X2 = _X0 + _Radius * _cosA;
	  float _Y2 = _Y0 + _Radius * _sinA;
	  gdraw.drawLine (_X1, _Y1, _X2, _Y2, TFT_WHITE);
	  
	  float _X1X = _X1 + _cosB;
	  float _Y1X = _Y1 + _sinB;      	 
	  float _X2X = _X2 + _cosB;
	  float _Y2X = _Y2 + _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_WHITE);				
		  
	  _X1X = _X1 - _cosB;
	  _Y1X = _Y1 - _sinB;      	 
	  _X2X = _X2 - _cosB;
	  _Y2X = _Y2 - _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_WHITE);				
  
	  float _X1Y = _X1 + 2.0f * _cosB;
	  float _Y1Y = _Y1 + 2.0f * _sinB;      	 
	  float _X2Y = _X2 + 2.0f * _cosB;
	  float _Y2Y = _Y2 + 2.0f * _sinB;	
	  gdraw.drawLine (_X1Y, _Y1Y, _X2Y, _Y2Y, TFT_BLACK);	
	  
	  _X1X = _X1 - 2.0f * _cosB;
	  _Y1X = _Y1 - 2.0f * _sinB;      	 
	  _X2X = _X2 - 2.0f * _cosB;
	  _Y2X = _Y2 - 2.0f * _sinB;	
	  gdraw.drawLine (_X1X, _Y1X, _X2X, _Y2X, TFT_BLACK);	        
	  gdraw.drawLine (_X1X, _Y1X, _X1Y, _Y1Y, TFT_BLACK);
	  gdraw.drawLine (_X2X, _Y2X, _X2Y, _Y2Y, TFT_BLACK);
	}  
  }

  /*
   Text markers
  */ 
  float midRadius = _Radius-_Width/2;
  
  if (clockWise){
	  topDatumX = _X0 +  midRadius * cos (_StartAngle + _ArcAngle);  
	  topDatumY = _Y0 +  midRadius * sin (_StartAngle + _ArcAngle);  

	  btmDatumX = _X0 +  midRadius * cos (_StartAngle);    
	  btmDatumY = _Y0 +  midRadius * sin (_StartAngle);   	  
  }
  
  else {
  	  btmDatumX = _X0 +  midRadius * cos (_StartAngle + _ArcAngle);  
	  btmDatumY = _Y0 +  midRadius * sin (_StartAngle + _ArcAngle);  

	  topDatumX = _X0 +  midRadius * cos (_StartAngle);    
	  topDatumY = _Y0 +  midRadius * sin (_StartAngle);  
  } 
  
  /*
     Pointer adjustments
  */
  float _Angle = _theta;
          
  for (int16_t i = 1; i <= NUM_POINTERS; i++){
  
      if (_pointerAdj[i] < _MinDisplay) _pointerAdj[i]  = _MinDisplay;
      if (_pointerAdj[i] > _MaxDisplay) _pointerAdj[i] = _MaxDisplay;

      if (!clockWise) {
        _pointerAdj[i]  = PI - _pointerAdj[i];
        _Angle = -_theta;  
      }

    switch (pointerType[i]){
      case ARROW_OUT: MarkArrowOut(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case ARROW_IN: MarkArrowIn(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case BAR_LONG: MarkRbar(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case BAR_SHORT: MarkRbarShort(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case BUG_OUT: MarkBugOut(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case BUG_IN: MarkBugIn(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case NEEDLE: MarkNeedle(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      case ROUND_DOT: MarkRdot(_X0, _Y0, _Radius, _Width, _pointerAdj[i], pointerTag[i], _Angle, pointerColor[i]); break;
      default: break;
    }
   } 
 }

void Gauges::printNum (String value, int16_t x0, int16_t y0, int16_t width, int16_t height, int16_t roll, uint16_t color, uint8_t datum){
/*
  Draw a fully scalable and rotable numeric string using segmented characters.
  This function only supports numeric digits, plus common mathematical symbols.
  Extensions for full alphanumerics is possible by adding segments, but execution will be slower.
*/

/*
    p3----p4 
    |     |
    |     |
    |     |
    p1-p7-p2
    |     |
    |     |
    |     |
    p5----p6

 /*
  Establish a midbaseline segment
  */ 
    float halfWidth = (float)width * 0.5f;
    float halfHeight = (float)height * 0.5f;
    float xRotate = (halfWidth) * cos (roll * DEG_TO_RAD); 
    float yRotate = (halfWidth) * sin (roll * DEG_TO_RAD); 
    float px0;
    float py0;
    float px7 = px0;
    float py7 = py0;
    
    switch (datum){

      case TL_DATUM:
        px0 = (float)x0 + halfWidth; 
        py0 = (float)y0 + halfHeight;
        break;
      
      case ML_DATUM: 
        px0 = (float)x0 + halfWidth; 
        py0 = (float)y0;
        break;
      
      case BL_DATUM: 
        px0 = (float)x0 + halfWidth; 
        py0 = (float)y0 - halfHeight;     
      break;
      
      case TC_DATUM: 
        px0 = (float)x0 - (((value.length()) * 3.0f * halfWidth))/2 + 1.5f * halfWidth;
        py0 = (float)y0 + halfHeight;
      break; 

      case MC_DATUM: 
        px0 = (float)x0 - (((value.length()) * 3.0f * halfWidth))/2 + 1.5f * halfWidth;
        py0 = (float)y0;
      break;  
      
      case BC_DATUM: 
        px0 = (float)x0 - (((value.length()) * 3.0f * halfWidth))/2 + 1.5f * halfWidth;
        py0 = (float)y0 - halfHeight;
      break;  

      case TR_DATUM: 
        px0 = (float)x0 - (((value.length()) * 3.0f * halfWidth)) + width; 
        py0 = (float)y0 + halfHeight;
      break;
           
      case MR_DATUM: 
        px0 = (float)x0 - (((value.length()) * 3.0f * halfWidth)) + width; 
        py0 = (float)y0;
      break;
      
      case BR_DATUM: 
        px0 = (float)x0 - (((value.length()) * 3.0f * halfWidth)) + width; 
        py0 = (float)y0 - halfHeight;
      break;
      
      default: 
        px0 = (float)x0; // datum at center of first character
        py0 = (float)y0;
        break;
    }  
    
  for (uint16_t i = 0; i< value.length(); i++){
  
    char numChar = value.charAt(i);
  
    /*
    Adjust it for roll and pitch
    */
      float px1 = px0 - xRotate;
      float py1 = py0 + yRotate;
      
      float px2 = px0 + xRotate;
      float py2 = py0 - yRotate;

      float px7 = px0;
      float py7 = py0;

    /*  
     Compute offset parallel line segments
    */
    
      float rollRad = roll * DEG_TO_RAD;
      float sinA = sin(rollRad) * halfHeight;
      float cosA = cos(rollRad) * halfHeight;
    
      float px3 = px1 - sinA;    //cos(roll * DEG_TO_RAD - HALF_PI);
      float py3 = py1 - cosA;    //sin(-roll * DEG_TO_RAD - HALF_PI);
                        
      float px4 = px2 - sinA;    //cos(rollRad + HALF_PI);
      float py4 = py2 - cosA;    //sin(roll * DEG_TO_RAD + HALF_PI);
                       
      float px5 = px1 + sinA;    //cos(roll * DEG_TO_RAD - HALF_PI);
      float py5 = py1 + cosA;    //sin(-roll * DEG_TO_RAD - HALF_PI);
                       
      float px6 = px2 + sinA;    //cos(rollRad + HALF_PI);
      float py6 = py2 + cosA;    //sin(roll * DEG_TO_RAD + HALF_PI);
    
  switch (numChar){

    case '0':
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   
      gdraw.drawLine (px3, py3, px5, py5, color);
      gdraw.drawLine (px4, py4, px6, py6, color);
      gdraw.drawLine (px4, py4, px5, py5, color);
    break;
    
    case '1':
      gdraw.drawLine (px2, py2, px4, py4, color);
      gdraw.drawLine (px2, py2, px6, py6, color);
    break;
     
    case '2':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   

      gdraw.drawLine (px1, py1, px5, py5, color);
      gdraw.drawLine (px2, py2, px4, py4, color);

    break;
     
    case '3':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   
      gdraw.drawLine (px2, py2, px4, py4, color);
      gdraw.drawLine (px2, py2, px6, py6, color);
    break;
     
    case '4':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px1, py1, px4, py4, color);
      gdraw.drawLine (px2, py2, px4, py4, color);
      gdraw.drawLine (px2, py2, px6, py6, color);
    break;
    
    case '5':
      gdraw.drawLine (px4, py4, px3, py3, color);
      gdraw.drawLine (px3, py3, px1, py1, color);
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px2, py2, px6, py6, color);         
      gdraw.drawLine (px6, py6, px5, py5, color);
                    
    break; 
    
    case '6':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   
      gdraw.drawLine (px1, py1, px5, py5, color);
      gdraw.drawLine (px2, py2, px6, py6, color);
      gdraw.drawLine (px1, py1, px4, py4, color);    
    break;
    
    case '7':
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px4, py4, px5, py5, color);
    break;
    
    case '8':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   
      gdraw.drawLine (px3, py3, px5, py5, color);
      gdraw.drawLine (px4, py4, px6, py6, color);
    break;
      
    case '9':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px3, py3, px4, py4, color); 
      gdraw.drawLine (px2, py2, px4, py4, color);          
      gdraw.drawLine (px1, py1, px3, py3, color);
      gdraw.drawLine (px2, py2, px5, py5, color);
    break;
      
    case '-':
      gdraw.drawLine (px1, py1, px2, py2, color);
    break;  
    
    case '.':
      gdraw.drawCircle(px6, py6, 0.50f * halfWidth, color);
    break;
    
    case 'o':
      gdraw.drawCircle (px3, py3, 0.50f * halfWidth, color);
    break; 
    
    case '%':
      gdraw.drawLine (px4, py4, px5, py5, color);
      gdraw.drawCircle (px3, py3, 0.50f * halfWidth, color);
      gdraw.drawCircle (px6, py6, 0.50f * halfWidth, color);
    break;
    
    case ':':
      gdraw.drawCircle (px3, py3, 0.50f * halfWidth, color);
      gdraw.drawCircle (px5, py5, 0.50f * halfWidth, color);
    break;
    
    case '[':
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   
      gdraw.drawLine (px1, py1, px5, py5, color);
    break;
      
    case ']':
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px4, py4, px6, py6, color);                                   
      gdraw.drawLine (px5, py5, px6, py6, color);
    break;
    
    case '\\':
      gdraw.drawLine (px3, py3, px6, py6, color);
    break;

    case '/':
      gdraw.drawLine (px4, py4, px5, py5, color);
    break;
    
    case '*':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px3, py3, px6, py6, color);
      gdraw.drawLine (px4, py4, px5, py5, color);
    break;
      
    case '=':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px5, py5, px6, py6, color);
    break;
    
    case '>':
      gdraw.drawLine (px3, py3, px2, py2, color);
      gdraw.drawLine (px2, py2, px5, py5, color);
    break;
      
    case '<':
      gdraw.drawLine (px1, py1, px4, py4, color);
      gdraw.drawLine (px1, py1, px6, py6, color);
    break;
  
    case '$':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px3, py3, px4, py4, color);
      gdraw.drawLine (px5, py5, px6, py6, color);                                   
      gdraw.drawLine (px1, py1, px3, py3, color);
      gdraw.drawLine (px2, py2, px6, py6, color);
      gdraw.drawLine (px4, py4, px5, py5, color);      
    break;
    
    case '+':
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px5, py5, px4, py4, color);   
    break;
    
    case '^':
      gdraw.drawLine (px1, py1, px4, py4, color);
      gdraw.drawLine (px2, py2, px4, py4, color);   
    break;
    
    case '(':
      gdraw.drawLine (px4, py4, px7, py7, color);
      gdraw.drawLine (px7, py7, px6, py6, color);   
    break;
    
    case ')':
      gdraw.drawLine (px3, py3, px7, py7, color);
      gdraw.drawLine (px7, py7, px5, py5, color);   
    break;
         
    case '&':
      gdraw.drawLine (px4, py4, px1, py1, color);
      gdraw.drawLine (px1, py1, px2, py2, color);
      gdraw.drawLine (px1, py1, px6, py6, color);                                   
    break;
    
    case '\n': 
      px0 -= 3.0f * xRotate * (i + 1);
      py0 += 1.5f * height;
    break;

    default: 
      break;
    }

   px0 += 3.0f * xRotate;  //step to next character position
   py0 -= 3.0f * yRotate;  //with a half space between (3 = 3/2)
  }
}
/*
   *******Private Functions******
   Arrows, bars, bugs and needle widgets
*/

void Gauges::MarkArrowLeft(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  gdraw.fillTriangle (x0 + barWidth / 2, y0 - pointer, x0 - barWidth / 2, y0 - pointer - barWidth / 3,
                      x0 - barWidth / 2, y0 - pointer + barWidth / 3, color);
  gdraw.drawTriangle (x0 + barWidth / 2, y0 - pointer, x0 - barWidth / 2, y0 - pointer - barWidth / 3,
                      x0 - barWidth / 2, y0 - pointer + barWidth / 3, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 - barWidth - 1, y0 - pointer - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 - barWidth + 1, y0 - pointer + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 - barWidth, y0 - pointer, GFXFF);
}

void Gauges::MarkArrowRight(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  gdraw.fillTriangle (x0 + barWidth / 2, y0 - pointer, x0 + 3 * barWidth / 2, y0 - pointer - barWidth / 3,
                      x0 + 3 * barWidth / 2, y0 - pointer + barWidth / 3, color);
  gdraw.drawTriangle (x0 + barWidth / 2, y0 - pointer, x0 + 3 * barWidth / 2, y0 - pointer - barWidth / 3,
                      x0 + 3 * barWidth / 2, y0 - pointer + barWidth / 3, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + 2 * barWidth - 1, y0 - pointer - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + 2 * barWidth + 1, y0 - pointer + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 + 2 * barWidth, y0 - pointer, GFXFF);
}

void Gauges::MarkBugLeft(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

 /*
   Left bug
      
  p3------p1      `       
   |      | 
  p8\     |
     \    |        
  p6 p7  p5 
     /    |  
  p9/     |
   |      |
  p4------p2             
    
  */
  
  int16_t x5 = x0;
  int16_t y5 = y0 - pointer;
  
  int16_t x6 = x5 - barWidth / 2;
  int16_t y6 = y5;
  int16_t x1 = x5;
  int16_t y1 = y5 - barWidth / 2;
  int16_t x2 = x5;
  int16_t y2 = y5 + barWidth / 2;
  int16_t x3 = x6;
  int16_t y3 = y1;
  int16_t x4 = x6;
  int16_t y4 = y2;
  int16_t x7 = x5 - barWidth / 4 ;
  int16_t y7 = y5;
  int16_t x8 = x6;
  int16_t y8 = y6 - barWidth / 4;
  int16_t x9 = x6;
  int16_t y9 = y6 + barWidth / 4;
  
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x2, y2, x4, y4, x1, y1,  color);
  gdraw.fillTriangle (x8, y8, x9, y9, x7, y7, TFT_BLACK);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);
  
  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 - barWidth - 1, y0 - pointer - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 - barWidth + 1, y0 - pointer + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 - barWidth, y0 - pointer, GFXFF);
}

void Gauges::MarkBugRight(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

 /*
   Right bug
      
  p1------p3
   |      |
   |     /p8
   |    /   
  p5   p7 p6
   |    \
   |     \p9  
   |      |
  p2------p4

  */
  
  int16_t x5 = x0 + barWidth;
  int16_t y5 = y0 - pointer;
  int16_t x6 = x5 + barWidth / 2;
  int16_t y6 = y5;
  int16_t x1 = x5;
  int16_t y1 = y5 - barWidth / 2;
  int16_t x2 = x5;
  int16_t y2 = y5 + barWidth / 2;
  int16_t x3 = x6;
  int16_t y3 = y1;
  int16_t x4 = x6;
  int16_t y4 = y2;
  int16_t x7 = x5 + barWidth / 4;
  int16_t y7 = y5;
  int16_t x8 = x6;
  int16_t y8 = y6 - barWidth / 4;
  int16_t x9 = x6;
  int16_t y9 = y6 + barWidth / 4;
  
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x2, y2, x4, y4, x1, y1,  color);
  gdraw.fillTriangle (x8, y8, x9, y9, x7, y7, TFT_BLACK);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);
	
  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + 2 * barWidth - 1, y0 - pointer - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + 2 * barWidth + 1, y0 - pointer + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 + 2 * barWidth, y0 - pointer, GFXFF);
}

void Gauges::MarkHbar(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  /* 
    Long Horizontal Bar

    ---------
   |         |
    ---------

  */

  gdraw.fillRect(x0 - barWidth / 4, y0 - pointer - barWidth / 6, 3 * barWidth / 2, barWidth / 3, color);
  gdraw.drawRect (x0 - barWidth / 4, y0 - pointer - barWidth / 6, 3 * barWidth / 2, barWidth / 3, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + barWidth / 2 - 1, y0 - pointer - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + barWidth / 2 + 1, y0 - pointer + 1, GFXFF);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.drawString ((String)tag, x0 + barWidth / 2 , y0 - pointer, GFXFF);
}

void Gauges::MarkHbarShort(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  /* 
    Short Horizontal Bar

    ---------
   |         |
    ---------

  */

  gdraw.fillRect(x0 + barWidth / 8, y0 - pointer - barWidth / 8, 3 * barWidth / 4, barWidth / 4, color);
  gdraw.drawRect (x0 + barWidth / 8, y0 - pointer - barWidth / 8, 3 * barWidth / 4, barWidth / 4, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + barWidth / 2 - 1, y0 - pointer - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + barWidth / 2 + 1, y0 - pointer + 1, GFXFF);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.drawString ((String)tag, x0 + barWidth / 2 , y0 - pointer, GFXFF);
}

void Gauges::MarkHdot(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {
 
/*
  This is for Vertical bar gauges, despite the name.
*/
  gdraw.fillCircle (x0 + barWidth / 2, y0 - pointer, barWidth / 5, color);
  gdraw.drawCircle (x0 + barWidth / 2, y0 - pointer, barWidth / 5, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + barWidth / 2, y0 - pointer, GFXFF);
}

void Gauges::MarkArrowTop(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  /* 
    Top arrow

    .------.
     \    /
      \  /
       \/        
                 
  */

  gdraw.fillTriangle (x0 + pointer, y0 + barWidth / 2, x0 + pointer - barWidth / 3, y0 - barWidth / 2,
                      x0 + pointer + barWidth / 3, y0 - barWidth / 2, color);
  gdraw.drawTriangle (x0 + pointer, y0 + barWidth / 2, x0 + pointer - barWidth / 3, y0 - barWidth / 2,
                      x0 + pointer + barWidth / 3, y0 - barWidth / 2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer - 1, y0 - barWidth - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + pointer + 1 , y0 - barWidth + 1, GFXFF);
  gdraw.setTextColor(color);
  gdraw.drawString ((String)tag, x0 + pointer, y0 - barWidth, GFXFF);
}

void Gauges::MarkArrowBottom(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {


  /* 
    Bottom Arrow

       /\
      /  \
     /    \
    '------'
        
  */

  gdraw.fillTriangle (x0 + pointer, y0 + barWidth / 2, x0 + pointer - barWidth / 3, y0 + 3 * barWidth / 2,
                      x0 + pointer + barWidth / 3, y0 + 3 * barWidth / 2, color);
  gdraw.drawTriangle (x0 + pointer, y0 + barWidth / 2, x0 + pointer - barWidth / 3, y0 + 3 * barWidth / 2,
                      x0 + pointer + barWidth / 3, y0 + 3 * barWidth / 2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer - 1, y0 + 2 * barWidth - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + pointer + 1 , y0 + 2 * barWidth + 1, GFXFF);
  gdraw.setTextColor(color);
  gdraw.drawString ((String)tag, x0 + pointer, y0 + 2 * barWidth, GFXFF);
}

void Gauges::MarkBugTop(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  /*
  
	Top bug
	  
    p3_p8  p6  p9_p4
    |   \     /   |
    |    \   /    | 
    |     \ /     |
    |      p7     |
    |             |
    p1_____p5_____p2   

  */
  
  int16_t x5 = x0 + pointer;
  int16_t y5 = y0;  
  int16_t x6 = x5;
  int16_t y6 = y5 - barWidth / 2;    
  int16_t x1 = x5 - barWidth / 2;
  int16_t y1 = y5;
  int16_t x2 = x5 + barWidth / 2;
  int16_t y2 = y5;
  int16_t x3 = x6 - barWidth / 2;
  int16_t y3 = y6;  
  int16_t x4 = x6 + barWidth / 2;
  int16_t y4 = y6;  
  int16_t x7 = x5;
  int16_t y7 = y5 - barWidth / 4;
  int16_t x8 = x6 - barWidth / 4;
  int16_t y8 = y6;
  int16_t x9 = x6 + barWidth / 4;
  int16_t y9 = y6;
  
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x2, y2, x4, y4, x1, y1,  color);
  gdraw.fillTriangle (x8, y8, x9, y9, x7, y7, TFT_BLACK);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer - 1, y0 - barWidth - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + pointer + 1 , y0 - barWidth + 1, GFXFF);
  gdraw.setTextColor(color);
  gdraw.drawString ((String)tag, x0 + pointer, y0 - barWidth, GFXFF);
}

void Gauges::MarkBugBot(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

 /*
   Bottom bug

  p1_____p5_____p2
  |             |
  |      p7     |
  |      /\     |
  |     /  \    |
  |    /    \   |
  p3_p8  p6  p9_p4
    
  */

  int16_t x5 = x0 + pointer;
  int16_t y5 = y0 + barWidth;  
  int16_t x6 = x5;
  int16_t y6 = y5 + barWidth / 2;
  int16_t x1 = x5 - barWidth / 2;
  int16_t y1 = y5;  
  int16_t x2 = x5 + barWidth / 2;
  int16_t y2 = y5;  
  int16_t x3 = x6 - barWidth / 2;
  int16_t y3 = y6;  
  int16_t x4 = x6 + barWidth / 2;
  int16_t y4 = y6;  
  int16_t x7 = x5;
  int16_t y7 = y5 + barWidth / 4;
  int16_t x8 = x6 - barWidth / 4;
  int16_t y8 = y6;
  int16_t x9 = x6 + barWidth / 4;
  int16_t y9 = y6;
  
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x2, y2, x4, y4, x1, y1,  color);
  gdraw.fillTriangle (x8, y8, x9, y9, x7, y7, TFT_BLACK);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer - 1, y0 + 2 * barWidth - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + pointer + 1 , y0 + 2 * barWidth + 1, GFXFF);
  gdraw.setTextColor(color);
  gdraw.drawString ((String)tag, x0 + pointer, y0 + 2 * barWidth, GFXFF);

}

void Gauges::MarkVbar(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {

  /* 
    Long Vertical Bar
    
    --
   |  | 
   |  | 
   |  | 
   |  |  
    --
    
    */

  gdraw.fillRect(x0 + pointer - barWidth / 6, y0 - barWidth / 3, barWidth / 3, 3 * barWidth / 2, color);
  gdraw.drawRect (x0 + pointer - barWidth / 6, y0 - barWidth / 3, barWidth / 3, 3 * barWidth / 2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer - 1, y0 + barWidth / 2 - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + pointer + 1, y0 + barWidth / 2 + 1, GFXFF);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.drawString ((String)tag, x0 + pointer, y0 + barWidth / 2, GFXFF);
}

void Gauges::MarkVbarShort(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {


  /* 
    Short Vertical Bar
    
    --
   |  | 
   |  | 
   |  | 
   |  |  
    --
    
    */

  gdraw.fillRect(x0 + pointer - barWidth / 8, y0 + barWidth / 8, barWidth / 4, 3 * barWidth / 4, color);
  gdraw.drawRect (x0 + pointer - barWidth / 8, y0 + barWidth / 8, barWidth / 4, 3 * barWidth / 4, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer - 1, y0 + barWidth / 2 - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + pointer + 1, y0 + barWidth / 2 + 1, GFXFF);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.drawString ((String)tag, x0 + pointer, y0 + barWidth / 2, GFXFF);
}

void Gauges::MarkVdot(int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color) {
 
 /*
  This is for horizontal bar gauges, despite the name.
 */
  gdraw.fillCircle (x0 + pointer, y0 + barWidth / 2 , barWidth / 5, color);
  gdraw.drawCircle (x0 + pointer, y0 + barWidth / 2 , barWidth / 5, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + pointer, y0 + barWidth / 2, GFXFF);
}

void Gauges::MarkArrowOut(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  /* 
    Outside arrow (pointing in)
  
    p2-p1-p3
     \    /
      \  /       
       \/        
       p4        

  */
        
  float _Pointer = (pointer + theta);
  float _Factor = barSize / (barSize - 1.5f * barWidth);
  float cosA = cos (_Pointer);
  float sinA = sin (_Pointer);
  float bw1 = barSize + 0.50f * barWidth;
  float bw2 = barSize - 0.50f * barWidth;
  float bw3 = 0.33f * barWidth;
  
  float x1 = x0 + bw1 * cosA;
  float y1 = y0 + bw1 * sinA;
  float x4 = x0 + bw2 * cosA;
  float y4 = y0 + bw2 * sinA;  
  float x2 = x1 + bw3 * sinA;  
  float y2 = y1 + bw3 * -cosA;
  float x3 = x1 + bw3 * -sinA;
  float y3 = y1 + bw3 * cosA;
  
  gdraw.fillTriangle (x4, y4, x3, y3, x2, y2, color);
  gdraw.drawTriangle (x4, y4, x3, y3, x2, y2, TFT_BLACK);
  
  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize + barWidth) * cos(_Pointer)) - 1,
                    y0 + (int16_t)((barSize + barWidth)*sin(_Pointer)) - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize + barWidth) * cos(_Pointer)) + 1,
                    y0 + (int16_t)((barSize + barWidth)*sin(_Pointer)) + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize + barWidth) * cos(_Pointer)),
                    y0 + (int16_t)((barSize + barWidth)*sin(_Pointer)), GFXFF);
 
}

void Gauges::MarkArrowIn(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  /* 
    Inside arrow (pointing out)
                 
       p4        
       /\     
      /  \       
     /    \
    p2-p1-p3 
        
  */
        
  float _Pointer = (pointer + theta);
  float _Factor = barSize / (barSize - 1.5f * barWidth);
  float cosA = cos (_Pointer);
  float sinA = sin (_Pointer);
  float bw1 = barSize - 1.50f * barWidth;
  float bw2 = barSize - 0.50f * barWidth;
  float bw3 = 0.33f * barWidth;
  
  float x1 = x0 + bw1 * cosA;
  float y1 = y0 + bw1 * sinA;
  float x4 = x0 + bw2 * cosA;
  float y4 = y0 + bw2 * sinA;  
  float x2 = x1 + bw3 * sinA;  
  float y2 = y1 + bw3 * -cosA;
  float x3 = x1 + bw3 * -sinA;
  float y3 = y1 + bw3 * cosA;
  
  gdraw.fillTriangle (x4, y4, x3, y3, x2, y2, color);
  gdraw.drawTriangle (x4, y4, x3, y3, x2, y2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 2.0f * barWidth) * cos (_Pointer)) - 1,
                    y0 + (int16_t)((barSize - 2.0f * barWidth)*sin(_Pointer)) - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 2.0f * barWidth) * cos (_Pointer)) + 1,
                    y0 + (int16_t)((barSize - 2.0f * barWidth)*sin(_Pointer)) + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 2.0f * barWidth) * cos (_Pointer)),
                    y0 + (int16_t)((barSize - 2.0f * barWidth)*sin(_Pointer)), GFXFF);
}

void Gauges::MarkRbar(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  /* 
    Radial Bar

   p4-p5-p6
   |     |    
   |     |       
   |     |        
   |     | 
   p1-p2-p3

  */
  float _Pointer = (pointer + theta);
  
  float cosA = cos(_Pointer);
  float sinA = sin(_Pointer);
  
  float bw1 = barSize - 1.25f * barWidth;
  float bw2 = barSize + 0.25f * barWidth;
  float bw3 = 0.165f * barWidth;
  
  float x2 = x0 + bw1 * cosA;
  float y2 = y0 + bw1 * sinA;
  float x5 = x0 + bw2 * cosA;
  float y5 = y0 + bw2 * sinA;
  
  sinA *= bw3;
  cosA *= bw3;
  
  float x1 = x2 + sinA;
  float y1 = y2 + -cosA;
  float x3 = x2 + -sinA;
  float y3 = y2 + cosA;
  float x4 = x5 + sinA;
  float y4 = y5 + -cosA;
  float x6 = x5 + -sinA;
  float y6 = y5 + cosA;
  						 
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x3, y3, x6, y6, x4, y4, color);

  gdraw.drawLine (x1, y1, x3, y3, TFT_BLACK);
  gdraw.drawLine (x3, y3, x6, y6, TFT_BLACK);
  gdraw.drawLine (x4, y4, x6, y6, TFT_BLACK);
  gdraw.drawLine (x1, y1, x4, y4, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 0.50f * barWidth) * cos(_Pointer)) - 1,
                    y0 + (int16_t)((barSize - 0.50f * barWidth) * sin(_Pointer)) - 1 , GFXFF);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 0.50f * barWidth) * cos(_Pointer)) + 1,
                    y0 + (int16_t)((barSize - 0.50f * barWidth)*sin(_Pointer)) + 1, GFXFF);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 0.50f * barWidth)*cos(_Pointer)),
                    y0 + (int16_t)((barSize - 0.50f * barWidth)*sin(_Pointer)), GFXFF);
}

void Gauges::MarkRbarShort(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  /* 
    Short Radial Bar

   p4-p5-p6
   |     |    
   |     |       
   |     |        
   |     | 
   p1-p2-p3

  */
  float _Pointer = (pointer + theta);
  
  float cosA = cos(_Pointer);
  float sinA = sin(_Pointer);
    
  float bw1 = barSize - 0.125f * barWidth;
  float bw2 = barSize - 0.875f * barWidth;
  float bw3 = 0.120f * barWidth;
  
  float x2 = x0 + bw1 * cosA;
  float y2 = y0 + bw1 * sinA;
  float x5 = x0 + bw2 * cosA;
  float y5 = y0 + bw2 * sinA;
  
  sinA *= bw3;
  cosA *= bw3;  
  
  float x1 = x2 + sinA;
  float y1 = y2 + -cosA;
  float x3 = x2 + -sinA;
  float y3 = y2 + cosA;
  float x4 = x5 + sinA;
  float y4 = y5 + -cosA;
  float x6 = x5 + -sinA;
  float y6 = y5 + cosA;
  						 
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x3, y3, x6, y6, x4, y4, color);

  gdraw.drawLine (x1, y1, x3, y3, TFT_BLACK);
  gdraw.drawLine (x3, y3, x6, y6, TFT_BLACK);
  gdraw.drawLine (x4, y4, x6, y6, TFT_BLACK);
  gdraw.drawLine (x1, y1, x4, y4, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 0.50f * barWidth) * cos(_Pointer)) - 1,
                    y0 + (int16_t)((barSize - 0.50f * barWidth) * sin(_Pointer)) - 1 , GFXFF);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 0.50f * barWidth) * cos(_Pointer)) + 1,
                    y0 + (int16_t)((barSize - 0.50f * barWidth)*sin(_Pointer)) + 1, GFXFF);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 0.50f * barWidth)*cos(_Pointer)),
                    y0 + (int16_t)((barSize - 0.50f * barWidth)*sin(_Pointer)), GFXFF);
}

void Gauges::MarkRdot(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  float _Pointer = (pointer + theta);
  
  float x1 = x0 + (barSize - 0.50f * barWidth)* cos (_Pointer);
  float y1 = y0 + (barSize - 0.50f * barWidth )* sin (_Pointer);
  
  gdraw.fillCircle (x1, y1, 0.25f * barWidth, color);
  gdraw.drawCircle (x1, y1, 0.25f * barWidth, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x1, y1, GFXFF);
}

void Gauges::MarkNeedle (float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

/*
  Needle (triangular)
  
  p1_____________________________
   |                             \_______________________
   |                                                     \_______________
   |                                                                     \p3
   |                                                      _______________/              
   |                               ______________________/                
  p2_____________________________/
  
  */
  
  float _Pointer = (pointer + theta);
  
  float bw1 = barSize - 0.50f * barWidth;  
  float bw2 = 0.40f * barWidth;  
  
  float _angleA = _Pointer - HALF_PI;
  float _angleB = _Pointer + HALF_PI;
  
  float x1 = x0 + bw2 * cos(_angleA);
  float y1 = y0 + bw2 * sin(_angleA);
  float x2 = x0 + bw2 * cos(_angleB);
  float y2 = y0 + bw2 * sin(_angleB);
  float x3 = x0 + bw1 * cos(_Pointer);
  float y3 = y0 + bw1 * sin(_Pointer);

  gdraw.fillTriangle (x1, y1, x2, y2, x3, y3, color);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);
  gdraw.drawLine (x2, y2, x3, y3, TFT_BLACK);
  gdraw.drawLine (x3, y3, x1, y1, TFT_BLACK);
  
  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 - 1, y0 - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + 1, y0 - 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0, y0, GFXFF);
}

void Gauges::MarkBugOut(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  float _Pointer = (pointer + theta);
  float _Factor = barSize / (barSize - 1.5f * barWidth);
  
  float _barScale = barWidth * 0.5f + barSize;  
  float _PointerX = _Pointer - 0.080f;  
  
  /*
  
    Outside radial bug
    
  	p3-p8 p6 p9-p4
	|   \    /   |
	|    \  /    | 
	|     \/     |
    |     p7     |
    |            |
    p1----p5----p2   
       
  */
  
  float cosA = cos(_Pointer);
  float sinA = sin(_Pointer);
  
  float bw1 = barSize + 0.5f  * barWidth;
  float bw2 = barSize + 0.125f * barWidth;
  float bw3 = 0.5f * barWidth;
  float bw4 = 0.25f * barWidth;
  
  float x6 = x0 + bw1 * cosA;
  float y6 = y0 + bw1 * sinA; 
  
  float x7 = x0 + bw2 * cosA;  
  float y7 = y0 + bw2 * sinA;  
  
  float x5 = x0 + barSize * cosA;
  float y5 = y0 + barSize * sinA;  
   
  float x1 = x5 + bw3 * sinA;
  float y1 = y5 + bw3 * -cosA;  
  float x2 = x5 + bw3 * -sinA;
  float y2 = y5 + bw3 * cosA; 
  float x3 = x6 + bw3 * sinA;
  float y3 = y6 + bw3 * -cosA;
  float x4 = x6 + bw3 * -sinA;
  float y4 = y6 + bw3 * cosA;  
  
  float x8 = x6 + bw4 * sinA;
  float y8 = y6 + bw4 * -cosA;
  float x9 = x6 + bw4 * -sinA;
  float y9 = y6 + bw4 * cosA;
  
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x2, y2, x4, y4, x1, y1,  color);
  gdraw.fillTriangle (x8, y8, x9, y9, x7, y7, TFT_BLACK);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize + barWidth) * cos(_Pointer)) - 1,
                    y0 + (int16_t)((barSize + barWidth)*sin(_Pointer)) - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize + barWidth) * cos(_Pointer)) + 1,
                    y0 + (int16_t)((barSize + barWidth)*sin(_Pointer)) + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize + barWidth) * cos(_Pointer)),
                    y0 + (int16_t)((barSize + barWidth)*sin(_Pointer)), GFXFF);
}

void Gauges::MarkBugIn(float x0, float y0, float barSize, float barWidth, float pointer, char tag, float theta, uint16_t color) {

  float _Pointer = (pointer + theta);
  float _Factor = barSize / (barSize - 1.5f * barWidth);
  
  float _barScale = barWidth * 0.5f + barSize;  
  float _PointerX = _Pointer - 0.080f;  
  
  /*
    Inside radial bug 
    
    p1----p5----p2
    |           |
    |     p7    |
    |     /\    |   
	|    /  \   | 
    |   /    \  |
    p3-p8 p6 p9-p4
    
  */
 
  float cosA = cos(_Pointer);
  float sinA = sin(_Pointer);
  
  float bw1 = barSize - 1.5f * barWidth;
  float bw2 = barSize - 1.125f * barWidth;
  float bw3 = 0.5f * barWidth;
  float bw4 = 0.25f * barWidth;
  
  float x6 = x0 + bw1 * cosA;
  float y6 = y0 + bw1 * sinA; 
  
  float x7 = x0 + bw2 * cosA;  
  float y7 = y0 + bw2 * sinA;  
  
  float x5 = x0 + (barSize-barWidth) * cosA;
  float y5 = y0 + (barSize-barWidth)  * sinA;  
   
  float x1 = x5 + bw3 * sinA;
  float y1 = y5 + bw3 * -cosA;  
  float x2 = x5 + bw3 * -sinA;
  float y2 = y5 + bw3 * cosA; 
  float x3 = x6 + bw3 * sinA;
  float y3 = y6 + bw3 * -cosA;
  float x4 = x6 + bw3 * -sinA;
  float y4 = y6 + bw3 * cosA;  
  
  float x8 = x6 + bw4 * sinA;
  float y8 = y6 + bw4 * -cosA;
  float x9 = x6 + bw4 * -sinA;
  float y9 = y6 + bw4 * cosA;
  
  gdraw.fillTriangle (x1, y1, x3, y3, x4, y4, color);
  gdraw.fillTriangle (x2, y2, x4, y4, x1, y1,  color);
  gdraw.fillTriangle (x8, y8, x9, y9, x7, y7, TFT_BLACK);
  gdraw.drawLine (x1, y1, x2, y2, TFT_BLACK);

  gdraw.setFreeFont (TEXTSIZE);
  gdraw.setTextColor (TFT_BLACK);
  gdraw.setTextDatum (MC_DATUM);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 2.0f * barWidth) * cos (_Pointer)) - 1,
                    y0 + (int16_t)((barSize - 2.0f * barWidth)*sin(_Pointer)) - 1, GFXFF);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 2.0f * barWidth) * cos (_Pointer)) + 1,
                    y0 + (int16_t)((barSize - 2.0f * barWidth)*sin(_Pointer)) + 1, GFXFF);
  gdraw.setTextColor (color);
  gdraw.drawString ((String)tag, x0 + (int16_t)((barSize - 2.0f * barWidth) * cos (_Pointer)),
                    y0 + (int16_t)((barSize - 2.0f * barWidth)*sin(_Pointer)), GFXFF);
}

void Gauges::setPointer (uint8_t Num, int16_t Value, uint8_t Type, uint16_t color, char Tag) {

  if (Num >= 1 && Num <= NUM_POINTERS) {
    pointerValue[Num] = Value;
    pointerType [Num] = Type;
    pointerColor [Num] = color;
    pointerTag [Num] = Tag;
  }
}

void Gauges::clearPointers(){
  for (int16_t i = 1; i <= NUM_POINTERS; i++) {
    pointerValue [i] = 0;
    pointerType [i] = 0;
    pointerColor [i] = 0;
    pointerTag [i] = '\0';
  }
}
  
void Gauges::setRange (uint8_t Num, bool Valid, int16_t Top, int16_t Bottom, uint16_t color) {

  if (Num >= 1 && Num <= NUM_RANGES) { 
    rangeValid[Num] = Valid;
    rangeTop[Num] = Top;
    rangeBot[Num] = Bottom;
    rangeColor[Num] = color;
  }
}

void Gauges::clearRanges(){
  for (int16_t i = 1; i <= NUM_RANGES; i++) {
    rangeValid[i] = false;
    rangeTop[i] = 0;
    rangeBot[i] = 0;
    rangeColor[i] = 0;
  }
}

void Gauges::drawLine (int16_t px1, int16_t py1, int16_t px2, int16_t py2, uint16_t color, uint8_t lineWidth){
  float deltaX;
  float deltaY;
  float angle;
  
  float sinA;
  float cosA;
  float px3;
  float py3;
  float px4;
  float py4;

  float px5;
  float py5;
  float px6;
  float py6;
   
  if (lineWidth == 0) gdraw.drawLine (px1, py1, px2, py2, color);     
  
  else {
    deltaX = px2 - px1;
    deltaY = py2 - py1;

    if (deltaX == 0) { 
      sinA = 0.5f * lineWidth;
      cosA = 0;
     } 
     
    else if (deltaY == 0) {
      sinA = 0; 
      cosA = 0.5f * lineWidth;           
    }

    else {     
      angle = atan (deltaY/deltaX);
      sinA = 0.5f * lineWidth * sin (angle);
      cosA = 0.5f * lineWidth * cos (angle);
    }
    
    px3 = px1 + sinA;
    py3 = py1 - cosA;
    px4 = px2 + sinA;
    py4 = py2 - cosA;

    px5 = px1 - sinA;
    py5 = py1 + cosA;
    px6 = px2 - sinA;
    py6 = py2 + cosA;

    gdraw.fillTriangle (px5, py5, px4, py4, px3, py3, color);         
    gdraw.fillTriangle (px6, py6, px4, py4, px5, py5, color);  
  }
}