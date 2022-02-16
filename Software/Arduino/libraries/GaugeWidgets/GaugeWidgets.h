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
   GaugeWidgets.h - Library for drawing engine gauges.
    by V.R. ("Voltar") Little
    Version 2.0, April 2, 2020
*/

#ifndef GaugeWidgets_h
#define GaugeWidgets_h
 
/*
 Helpful definitions for various gauge markers
 */
#define ARROW_LEFT 1
#define ARROW_RIGHT 2

#define ARROW_TOP 1
#define ARROW_BOTTOM 2

#define ARROW_OUT 1
#define ARROW_IN 2

#define BAR_LONG 3
#define BAR_SHORT 4

#define BUG_LEFT 5
#define BUG_RIGHT 6

#define BUG_TOP 5
#define BUG_BOTTOM 6

#define BUG_OUT 5
#define BUG_IN 6

#define ROUND_DOT 7

#define NEEDLE 8

/*
 Define the number of allowed pointers & colored range bars.
 Beware of the memory requirements when upscaling these values!
 */
#define NUM_POINTERS 8
#define NUM_RANGES 5

class Gauges {

  public:
    Gauges();
	void drawLine (int16_t px1, int16_t py1, int16_t px2, int16_t py2, uint16_t color, uint8_t lineWidth = 0); // draw a line of width lineWidth
    void setPointer (uint8_t Num, int16_t Value, uint8_t Type, uint16_t color, char Tag);   // set a pointer
    void clearPointers ();                                                                  // clear all pointers
    void setRange (uint8_t Num, bool Valid, int16_t Top, int16_t Bot, uint16_t color);      // set color ranges
    void clearRanges ();                                                                    // clear all ranges
    void vBarGraph (int16_t x0, int16_t y0, int16_t BarSize, int16_t barWidth, 
                    int16_t maxDisplay, int16_t minDisplay, uint8_t gradMarks = 0);         // draw a vertical bar graph
    void hBarGraph (int16_t x0, int16_t y0, int16_t barSize, int16_t barWidth, 
                    int16_t maxDisplay, int16_t minDisplay, uint8_t gradMarks = 0);         // draw a horizontal bar graph
    void arcGraph (int16_t x0, int16_t y0, int16_t barSize, int16_t barWidth, 
                   int16_t maxDisplay, int16_t minDisplay, int16_t startAngle, 
                   int16_t arcAngle, bool clockWise, uint8_t gradMarks = 0);         // draw an arc graph (cw or ccw)
    void printNum (String value, int16_t x0, int16_t y0, int16_t width,                     // scalable, rotable vector font
                   int16_t height, int16_t roll, uint16_t color, uint8_t datum); // for numbers and symbols only.
    
    /*
     Public variables.  These are accessible but not normally used. 
     Instead, for program readability, use the public function calls to configure the variables.
     */
    bool rangeValid[NUM_RANGES + 1];
    int32_t rangeTop [NUM_RANGES + 1];
    int32_t rangeBot [NUM_RANGES + 1];
    int32_t rangeColor [NUM_RANGES + 1];
    int32_t pointerValue [NUM_POINTERS + 1];
    int32_t pointerType [NUM_POINTERS + 1];
    int32_t pointerColor [NUM_POINTERS + 1];
    char pointerTag [NUM_POINTERS + 1];
    
    int16_t maxDisplay; int16_t minDisplay; int16_t x0; int16_t y0;
    int16_t barWidth; int16_t barSize;

    int16_t startAngle; int16_t arcAngle; 
	bool clockWise; bool gradMarks;
    
    /*
     The following variables are accessible after drawing a gauge, and are used to help position additional text.
     */	
	int16_t topDatumX, topDatumY, btmDatumX, btmDatumY; // endpoint text datum helpers

  private:

    /*
     Pointer types for vertical or horizontal bar graph
     */
    void MarkArrowLeft (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);  // PointerType 1 for Vertical graphs
    void MarkArrowRight (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color); // PointerType 2 for Vertical graphs
    void MarkHbar (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);       // PointerType 3 for Vertical graphs
    void MarkHbarShort (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);  // PointerType 4 for Vertical graphs
    void MarkBugLeft (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);    // PointerType 5 for Vertical graphs
    void MarkBugRight (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);   // PointerType 6 for Vertical graphs
    void MarkHdot (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);       // PointerType 7 for Vertical graphs

    void MarkArrowTop (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);   // PointerType 1 for Horizontal graphs
    void MarkArrowBottom (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);// PointerType 2 for Horizontal graphs
    void MarkVbar (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);       // PointerType 3 for Horizontal graphs
    void MarkVbarShort (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);  // PointerType 4 for Horizontal graphs
    void MarkBugTop (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);     // PointerType 5 for Horizontal graphs
    void MarkBugBot (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);     // PointerType 6 for Horizontal graphs
    void MarkVdot (int16_t x0, int16_t y0, int16_t barWidth, int16_t pointer, char tag, uint16_t color);       // PointerType 7 for Horizontal graphs
   
    /*
      Pointer types for radial arc or circle graphs
     */   
    void MarkArrowOut (float x0, float y0, float barSize, float barWidth, float pointer, 
                       char tag, float theta, uint16_t color);                                  // PointerType 1 for arc graphs
    void MarkArrowIn (float x0, float y0, float barSize, float barWidth, float pointer, 
                      char tag, float theta, uint16_t color);                                   // PointerType 2 for arc graphs
    void MarkRbar (float x0, float y0, float barSize, float barWidth, float pointer, 
                   char tag, float theta, uint16_t color);                                      // PointerType 3 for arc graphs
    void MarkRbarShort (float x0, float y0, float barSize, float barWidth, float pointer, 
                        char tag, float theta, uint16_t color);                                 // PointerType 4 for arc graphs
    void MarkBugOut (float x0, float y0, float barSize, float barWidth, float pointer, 
                       char tag, float theta, uint16_t color);                                  // PointerType 5 for arc graphs        
    void MarkBugIn (float x0, float y0, float barSize, float barWidth, float pointer, 
                       char tag, float theta, uint16_t color);                                  // PointerType 6 for arc graphs     
    void MarkRdot (float x0, float y0, float barSize, float barWidth, float pointer, 
                        char tag, float theta, uint16_t color);                                 // PointerType 7 for arc graphs                              
    void MarkNeedle (float x0, float y0, float barSize, float barWidth, float pointer, 
                     char tag, float theta, uint16_t color);                                    // PointerType 8 for arc graphs      
              
};
#endif