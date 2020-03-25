/***************************************************************************
*                                                                          *
*  4D Systems GFX4d Library                                                *
*                                                                          *
*  Date:        11 July 2016                                               *
*                                                                          *
*  Description: Provides Graphics, Touch Control and SD Card functions     *
*               for 4D Systems Gen4 IoD range of intelligent displays.     *
*                                                                          *
*                   GNU LESSER GENERAL PUBLIC LICENSE                      *
*                       Version 3, 29 June 2007                            *
*                                                                          *
* Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>      *
* Everyone is permitted to copy and distribute verbatim copies             *
* of this license document, but changing it is not allowed.                *
*                                                                          *
*                                                                          *
* This version of the GNU Lesser General Public License incorporates       *
* the terms and conditions of version 3 of the GNU General Public          *
* License, supplemented by the additional permissions listed below.        *
*                                                                          *
*  0. Additional Definitions.                                              *
*                                                                          *
*   As used herein, "this License" refers to version 3 of the GNU Lesser   *
* General Public License, and the "GNU GPL" refers to version 3 of the GNU *
* General Public License.                                                  *
*                                                                          *
*   "The Library" refers to a covered work governed by this License,       *
* other than an Application or a Combined Work as defined below.           *
*                                                                          *
*   An "Application" is any work that makes use of an interface provided   *
* by the Library, but which is not otherwise based on the Library.         *
* Defining a subclass of a class defined by the Library is deemed a mode   *
* of using an interface provided by the Library.                           *
*                                                                          *
*   A "Combined Work" is a work produced by combining or linking an        *
* Application with the Library.  The particular version of the Library     *
* with which the Combined Work was made is also called the "Linked         *
* Version".                                                                *
*                                                                          *
*   The "Minimal Corresponding Source" for a Combined Work means the       *
* Corresponding Source for the Combined Work, excluding any source code    *
* for portions of the Combined Work that, considered in isolation, are     *
* based on the Application, and not on the Linked Version.                 *
*                                                                          *
*   The "Corresponding Application Code" for a Combined Work means the     *
* object code and/or source code for the Application, including any data   *
* and utility programs needed for reproducing the Combined Work from the   *
* Application, but excluding the System Libraries of the Combined Work.    *
*                                                                          *
*  1. Exception to Section 3 of the GNU GPL.                               *
*                                                                          *
*   You may convey a covered work under sections 3 and 4 of this License   *
* without being bound by section 3 of the GNU GPL.                         *
*                                                                          *
*  2. Conveying Modified Versions.                                         *
*                                                                          *
*   If you modify a copy of the Library, and, in your modifications, a     *
* facility refers to a function or data to be supplied by an Application   *
* that uses the facility (other than as an argument passed when the        *
* facility is invoked), then you may convey a copy of the modified         *
* version:                                                                 *
*                                                                          *
*   a) under this License, provided that you make a good faith effort to   *
*   ensure that, in the event an Application does not supply the           *
*   function or data, the facility still operates, and performs            *
*   whatever part of its purpose remains meaningful, or                    *
*                                                                          *
*   b) under the GNU GPL, with none of the additional permissions of       *
*   this License applicable to that copy.                                  *
*                                                                          *
*  3. Object Code Incorporating Material from Library Header Files.        *
*                                                                          *
*   The object code form of an Application may incorporate material from   *
* a header file that is part of the Library.  You may convey such object   *
* code under terms of your choice, provided that, if the incorporated      *
* material is not limited to numerical parameters, data structure          *
* layouts and accessors, or small macros, inline functions and templates   *
* (ten or fewer lines in length), you do both of the following:            *
*                                                                          *
*   a) Give prominent notice with each copy of the object code that the    *
*   Library is used in it and that the Library and its use are             *
*   covered by this License.                                               *
*                                                                          *
*   b) Accompany the object code with a copy of the GNU GPL and this       *
*   license document.                                                      *
*                                                                          *
*  4. Combined Works.                                                      *
*                                                                          *
*   You may convey a Combined Work under terms of your choice that,        *
* taken together, effectively do not restrict modification of the          *
* portions of the Library contained in the Combined Work and reverse       *
* engineering for debugging such modifications, if you also do each of     *
* the following:                                                           *
*                                                                          *
*   a) Give prominent notice with each copy of the Combined Work that      *
*   the Library is used in it and that the Library and its use are         *
*   covered by this License.                                               *
*                                                                          *
*   b) Accompany the Combined Work with a copy of the GNU GPL and this     * 
*   license document.                                                      *
*                                                                          *
*   c) For a Combined Work that displays copyright notices during          *
*   execution, include the copyright notice for the Library among          *
*   these notices, as well as a reference directing the user to the        *
*   copies of the GNU GPL and this license document.                       *
*                                                                          *
*   d) Do one of the following:                                            *
*                                                                          *
*       0) Convey the Minimal Corresponding Source under the terms of this *
*       License, and the Corresponding Application Code in a form          *
*       suitable for, and under terms that permit, the user to             *
*       recombine or relink the Application with a modified version of     *
*       the Linked Version to produce a modified Combined Work, in the     *
*       manner specified by section 6 of the GNU GPL for conveying         *
*       Corresponding Source.                                              *
*                                                                          *
*       1) Use a suitable shared library mechanism for linking with the    *
*       Library.  A suitable mechanism is one that (a) uses at run time    *
*       a copy of the Library already present on the user's computer       *
*       system, and (b) will operate properly with a modified version      *
*       of the Library that is interface-compatible with the Linked        *
*       Version.                                                           *
*                                                                          *
*   e) Provide Installation Information, but only if you would otherwise   *
*   be required to provide such information under section 6 of the         *
*   GNU GPL, and only to the extent that such information is               *
*   necessary to install and execute a modified version of the             *
*   Combined Work produced by recombining or relinking the                 *
*   Application with a modified version of the Linked Version. (If         *
*   you use option 4d0, the Installation Information must accompany        *
*   the Minimal Corresponding Source and Corresponding Application         *
*   Code. If you use option 4d1, you must provide the Installation         *
*   Information in the manner specified by section 6 of the GNU GPL        *
*   for conveying Corresponding Source.)                                   *
*                                                                          *
*  5. Combined Libraries.                                                  *
*                                                                          *
*   You may place library facilities that are a work based on the          *
* Library side by side in a single library together with other library     *
* facilities that are not Applications and are not covered by this         *
* License, and convey such a combined library under terms of your          *
* choice, if you do both of the following:                                 *
*                                                                          *
*   a) Accompany the combined library with a copy of the same work based   *
*   on the Library, uncombined with any other library facilities,          *
*   conveyed under the terms of this License.                              *
*                                                                          *
*   b) Give prominent notice with the combined library that part of it     *
*   is a work based on the Library, and explaining where to find the       *
*   accompanying uncombined form of the same work.                         *
*                                                                          *
*  6. Revised Versions of the GNU Lesser General Public License.           *
*                                                                          *
*   The Free Software Foundation may publish revised and/or new versions   *
* of the GNU Lesser General Public License from time to time. Such new     *
* versions will be similar in spirit to the present version, but may       *
* differ in detail to address new problems or concerns.                    *
*                                                                          *
*   Each version is given a distinguishing version number. If the          *
* Library as you received it specifies that a certain numbered version     *
* of the GNU Lesser General Public License "or any later version"          *
* applies to it, you have the option of following the terms and            *
* conditions either of that published version or of any later version      *
* published by the Free Software Foundation. If the Library as you         *
* received it does not specify a version number of the GNU Lesser          *
* General Public License, you may choose any version of the GNU Lesser     *
* General Public License ever published by the Free Software Foundation.   *
*                                                                          *
*   If the Library as you received it specifies that a proxy can decide    *
* whether future versions of the GNU Lesser General Public License shall   *
* apply, that proxy's public statement of acceptance of any version is     *
* permanent authorization for you to choose that version for the           *
* Library.                                                                 *
*                                                                          *
***************************************************************************/

#include "GFX4d.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>
#ifndef ESP32
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#endif

#define hwSPI true

#define swap(a, b) { int16_t tab = a; a = b; b = tab; }

#ifndef ESP8266
#define analogWrite(a, b) { ledcWrite(a + 1, b); }
#endif

#ifdef USE_FS
#include <FS.h>
#endif

GFX4d::GFX4d(){
    
    
    #ifndef ESP32
    _dc   = 4;
    _cs   = 15;
    _disp = 0;
    _tcs  = 2;
    _sd   = 5;
    #else
    _dc   = 33;
    _cs   = 27;
    _disp = 32;
    _tcs  = 9;
    _sd   = 10;
    #endif
    scrollOffset = 0;
    width    = 240;
    height   = 320;  
    rotation  = 0;
    cursor_y  = cursor_x    = 0;
    textsize  = 1;
    textcolor = 0xFFFF;
    textbgcolor = 0x0000;
    wrap      = true;
    fno       = 1;
    fsh       = 8;
    fsw       = 5;
    scrolled  = false;
    sEnable   = false;
    nl        = false;
    ssSpeed   = 5;
    twcolnum  = 13;
    tchen     = true;
    twcurson  = true;
    
}

SPISettings spiSettings = SPISettings(79000000, MSBFIRST, SPI_MODE0);
SPISettings spiSettingsD = SPISettings(79000000, MSBFIRST, SPI_MODE0);
SPISettings spiSettingsT = SPISettings(6900000, MSBFIRST, SPI_MODE0);
SPISettings spiSettingsT32 = SPISettings(4900000, MSBFIRST, SPI_MODE0);
 
void GFX4d::begin(void) {

  #ifdef ESP32
  delay(500);
  #endif
  pinMode(_sclk, OUTPUT);
  pinMode(_mosi, OUTPUT);
  pinMode(_miso, INPUT);
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT);
  pinMode(_disp, OUTPUT);
  pinMode(_tcs, OUTPUT);
  pinMode(_sd, OUTPUT);
  #ifndef ESP8266
  ledcSetup(1, 1000, 10);
  ledcAttachPin(_disp, 1);
  #endif
  digitalWrite(_sd, HIGH);
  digitalWrite(_tcs, HIGH);
  SPI.begin();
  SPI.beginTransaction(spiSettingsD);

  SetCommand(0xEF); SetData(0x03); SetData(0x80); SetData(0x02);
  SetCommand(0xCF); SetData(0x00); SetData(0XC1); SetData(0X30); 
  SetCommand(0xED); SetData(0x64); SetData(0x03); SetData(0X12); SetData(0X81); 
  SetCommand(0xE8); SetData(0x85); SetData(0x00); SetData(0x78); 
  SetCommand(0xCB); SetData(0x39); SetData(0x2C); SetData(0x00); SetData(0x34); SetData(0x02); 
  SetCommand(0xF7); SetData(0x20); 
  SetCommand(0xEA); SetData(0x00); SetData(0x00); 
  SetCommand(GFX4d_PWCTR1); SetData(0x23);
  SetCommand(GFX4d_PWCTR2); SetData(0x10);
  SetCommand(GFX4d_VMCTR1); SetData(0x3e); SetData(0x28); 
  SetCommand(GFX4d_VMCTR2); SetData(0x86);
  SetCommand(GFX4d_MADCTL); SetData(0x48);
  SetCommand(GFX4d_PIXFMT); SetData(0x55); 
  SetCommand(GFX4d_FRMCTR1); SetData(0x00); SetData(0x18); 
  SetCommand(GFX4d_DFUNCTR); SetData(0x08); SetData(0x82); SetData(0x27);  
  SetCommand(0xF2); SetData(0x00); 
  SetCommand(GFX4d_GAMMASET); SetData(0x01); 
  SetCommand(GFX4d_GMCTRP1); SetData(0x0F); SetData(0x31); SetData(0x2B); SetData(0x0C); 
                             SetData(0x0E); SetData(0x08); SetData(0x4E); SetData(0xF1); 
                             SetData(0x37); SetData(0x07); SetData(0x10); SetData(0x03); 
                             SetData(0x0E); SetData(0x09); SetData(0x00); 
  SetCommand(GFX4d_GMCTRN1); SetData(0x00); SetData(0x0E); SetData(0x14); SetData(0x03); 
                             SetData(0x11); SetData(0x07); SetData(0x31); SetData(0xC1); 
                             SetData(0x48); SetData(0x08); SetData(0x0F); SetData(0x0C); 
                             SetData(0x31); SetData(0x36); SetData(0x0F); 
  SetCommand(GFX4d_SLPOUT);
  SPI.endTransaction();  
  delay(120);     
  SPI.beginTransaction(spiSettingsD);
  SetCommand(GFX4d_DISPON);
  SPI.endTransaction();
  setScrollArea(0, 0);
  Cls(0);
  #ifndef USE_FS
  #ifndef ESP32
  #ifdef SDFS_H
  if(SD.begin(_sd, spiSettings)){
  #else
  if(SD.begin(_sd, 79000000)){
  #endif
  #else
  if(SD.begin(_sd, SPI, 79000000)){
  #endif
  #else
  if(SPIFFS.begin()){
  #endif
  sdok = true;
  } else {
  sdok = false;
  }
  BacklightOn(true);
  Orientation(0);
}

void GFX4d::Contrast(int ctrst){
  #ifndef ESP32
  if(ctrst > -1 && ctrst < 15) analogWrite(0, ctrst * 68);
  if(ctrst > 15) analogWrite(0, 1023);
  #else
  if(ctrst > -1 && ctrst < 15) ledcWrite(1, ctrst * 68);
  if(ctrst > 15) ledcWrite(1, 1023);
  #endif
}

void GFX4d::setScrollArea(uint16_t tfa, uint16_t bfa) {
  SetCommand(GFX4d_VSCRDEF); SetData(tfa >> 8); SetData(tfa);
                             SetData((320 - tfa - bfa) >> 8); SetData(320 - tfa - bfa);
                             SetData(tfa >> 8); SetData(bfa);
}

void GFX4d::Scroll(uint16_t vsp) {
  if(vsp > height - 1){
  vsp = vsp - height;
  }
  if(sEnable){
  SetCommand(GFX4d_VSCRSADD); SetData(vsp >> 8); SetData(vsp);
  scrollOffset = vsp;
  scrolled = true;
  }
}

void GFX4d::MoveTo(int16_t x, int16_t y) {
  cursor_x = x;
  if(scrolled){
  cursor_y = y + (height - scrollOffset);
  if(cursor_y > height -1){
  cursor_y = cursor_y - height;
  }
  } else {
  cursor_y = y;
  if(cursor_y > (height - 1)) cursor_y = (height - 1);
  }
  if(cursor_y < 0) cursor_y = 0;
  if(cursor_x < 0) cursor_x = 0;
    if(cursor_y < 0) cursor_y = 0;
  if(cursor_x > (width - 1)) cursor_x = width - 1;
  nl = false;
}

void GFX4d::TextSize(uint8_t s) {
  if(s > 0){  
  textsize = s;
  textsizeht = s;
  }
}

int8_t GFX4d::Font(void) {
  return fno;
}

void GFX4d::Font(uint8_t f) {
  if(f < 1 || f > 2){
  return;
  }
  fno = f;
  if(fno == 1){
  fsw = 5;
  fsh = 8;
  } 
  if(fno == 2){
  fsw = 8;
  fsh = 16;
  } 
}

void GFX4d::TextColor(uint16_t c) {  
  textcolor = c; 
  textbgcolor = c;
}

void GFX4d::TextColor(uint16_t c, uint16_t b) {
  textcolor   = c;
  textbgcolor = b; 
}

void GFX4d::TextWrap(boolean w) {
  wrap = w;
}

int8_t GFX4d::FontHeight(void) {
  return fsh;
}

void GFX4d::Rectangle(int16_t x, int16_t y, int16_t x1, int16_t y1, uint16_t color) {
  int w = x1 - x + 1;
  int h = y1 - y + 1;
  Hline(x, y, w, color);
  Hline(x, y + h - 1, w, color);
  Vline(x, y, h, color);
  Vline(x + w - 1, y, h, color);
}

void GFX4d::RoundRectFilled(int16_t x, int16_t y, int16_t x1, int16_t y1, int16_t r, uint16_t color) {
  if (x > x1) {
  swap(x, x1);
  }
  if (y > y1) {
  swap(y, y1);
  }
  int w = x1 - x + 1;
  int h = y1 - y + 1;
  int maxR;
  if (w > h) 
  maxR = (h-1)/2;
  else if (h > w)
  maxR = (w-1)/2;
  if (r > maxR) r = maxR;
  RectangleFilled(x + r, y, x + (w - r) - 1, y + h - 1, color);
  ArcFilled(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  ArcFilled(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void GFX4d::RoundRect(int16_t x, int16_t y, int16_t x1, int16_t y1, int16_t r, uint16_t color) {
  int w = x1 - x + 1;
  int h = y1 - y + 1;
  Hline(x + r, y, w - 2 * r, color);
  Hline(x + r, y + h - 1, w - 2 * r, color);
  Vline(x, y + r, h - 2  *r, color);
  Vline(x + w - 1, y + r, h - 2 * r, color);
  Arc(x + r, y + r, r, 1, color);
  Arc(x + w - r - 1, y + r, r, 2, color);
  Arc(x + w - r - 1, y + h - r - 1, r, 4, color);
  Arc(x + r, y + h - r - 1, r, 8, color);
}

void GFX4d::Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  Line(x0, y0, x1, y1, color);
  Line(x1, y1, x2, y2, color);
  Line(x2, y2, x0, y0, color);
}

void GFX4d::TriangleFilled ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  int16_t p0, p1, y, last;
  if (y0 > y1) {
  swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
  swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
  swap(y0, y1); swap(x0, x1);
  }
  if(y0 == y2) {
  p0 = p1 = x0;
  if(x1 < p0) p0 = x1;
  else if(x1 > p1) p1 = x1;
  if(x2 < p0) p0 = x2;
  else if(x2 > p1) p1 = x2;
  Hline(p0, y0, p1 - p0 + 1, color);
  return;
  }
  int16_t xx01 = x1 - x0, yy01 = y1 - y0, xx02 = x2 - x0, yy02 = y2 - y0;
  int16_t xx12 = x2 - x1, yy12 = y2 - y1;
  int32_t z1   = 0, z2   = 0;
  if(y1 == y2){
  last = y1;
  } else {
  last = y1 - 1;
  }
  for(y = y0; y <= last; y++) {
  p0 = x0 + z1 / yy01;
  p1 = x0 + z2 / yy02;
  z1 += xx01;
  z2 += xx02;
  if(p0 > p1){
  swap(p0, p1);
  }   
  Hline(p0, y, p1 - p0 + 1, color);
  }
  z1 = xx12 * (y - y1);
  z2 = xx02 * (y - y0);
  for(; y <= y2; y++) {
  p0 = x1 + z1 / yy12;
  p1 = x0 + z2 / yy02;
  z1 += xx12;
  z2 += xx02;
  if(p0 > p1){
  swap(p0, p1);
  }   
  Hline(p0, y, p1 - p0 + 1, color);
  }
}

void GFX4d::Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t angH = abs(y1 - y0) > abs(x1 - x0);
  if (angH) {
  swap(x0, y0);
  swap(x1, y1);
  }
  if (x0 > x1) {
  swap(x0, x1);
  swap(y0, y1);
  }
  int16_t xx;
  int16_t yy;
  xx = x1 - x0;
  yy = abs(y1 - y0);
  int16_t edx = xx / 2;
  int16_t incy;
  if (y0 < y1) {
  incy = 1;
  } else {
  incy = -1;
  }
  for (; x0 <= x1; x0++) {
  if (angH) {
  PutPixel(y0, x0, color);
  } else {
  PutPixel(x0, y0, color);
  }
  edx = edx - yy;
  if (edx < 0) {
  y0 = y0 + incy;
  edx = edx + xx;
  }
  }
}

void GFX4d::CircleFilled(int16_t xc, int16_t yc, int16_t r, uint16_t color) {
  Vline(xc, yc-r, 2 * r + 1, color);
  ArcFilled(xc, yc, r, 3, 0, color);
}

void GFX4d::Ellipse(int16_t xe, int16_t ye, int16_t radx, int16_t rady, uint16_t color){
  if(radx < 2){
  return;
  }
  if(rady < 2){
  return;
  }
  int16_t x, y;
  int32_t es;
  int32_t radxx = radx * radx;
  int32_t radyy = rady * rady;
  int32_t xr = 4 * radxx;
  int32_t yr = 4 * radyy;
  for(x = 0, y = rady, es = 2 * radyy + radxx * (1 - 2 * rady); radyy * x <= radxx * y; x++){
  PutPixel(xe + x, ye + y, color);
  PutPixel(xe - x, ye + y, color);
  PutPixel(xe - x, ye - y, color);
  PutPixel(xe + x, ye - y, color);
  if(es >= 0){
  es += xr * (1 - y);
  y--;
  }
  es += radyy * ((4 * x) + 6);
  }
  for(x = radx, y = 0, es = 2 * radxx + radyy * (1 - 2 * radx); radxx * y <= radyy * x; y++){
  PutPixel(xe + x, ye + y, color);
  PutPixel(xe - x, ye + y, color);
  PutPixel(xe - x, ye - y, color);
  PutPixel(xe + x, ye - y, color);
  if(es >= 0){
  es += yr * (1 - x);
  x --;
  }
  es += radxx * ((4 * y) + 6);
  }
}

void GFX4d::EllipseFilled(int16_t xe, int16_t ye, int16_t radx, int16_t rady, uint16_t color){
  if(radx < 2){
  return;
  }
  if(rady < 2){
  return;
  }
  int16_t x, y;
  int32_t es;
  int32_t radxx = radx * radx;
  int32_t radyy = rady * rady;
  int32_t xr = 4 * radxx;
  int32_t yr = 4 * radyy;
  for(x = 0, y = rady, es = 2 * radyy + radxx * (1 - 2 * rady); radyy * x <= radxx * y; x++){
  Hline(xe - x, ye - y, 1 + x + x, color);
  Hline(xe - x, ye + y, 1 + x + x, color);
  if(es >= 0){
  es += xr * (1 - y);
  y --;
  }
  es += radyy * ((4 * x) + 6);
  }
  for(x = radx, y = 0, es = 2 * radxx + radyy * (1 - 2 * radx); radxx * y <= radyy * x; y++){
  Hline(xe - x, ye - y, 1 + x + x, color);
  Hline(xe - x, ye + y, 1 + x + x, color);
  if(es >= 0){
  es += yr * (1 - x);
  x --;
  }
  es += radxx * ((4 * y) + 6);
  }
}

void GFX4d::ArcFilled(int16_t xa, int16_t ya, int16_t r, uint8_t sa, int16_t ea, uint16_t color) {
  int16_t c = 1 - r;
  int16_t x = 0;
  int16_t y = r;
  int16_t xx = 1;
  int16_t yy = -2 * r;
  while (x < y) {
  if (c >= 0) {
  y--;
  yy = yy + 2;
  c = c + yy;
  }
  x++ ;
  xx = xx + 2;
  c = c + xx;
  if (sa & 0x1) {
  Vline(xa + x, ya - y, 2 * y + 1 + ea, color);
  Vline(xa + y, ya - x, 2 * x + 1 + ea, color);
  }
  if (sa & 0x2) {
  Vline(xa - x, ya - y, 2 * y + 1 + ea, color);
  Vline(xa - y, ya - x, 2 * x + 1 + ea, color);
  }
  }
}

void GFX4d::Circle(int16_t xc, int16_t yc, int16_t r, uint16_t color) {
  int16_t c = 1 - r;
  int16_t xx = 1;
  int16_t yy = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  PutPixel(xc, yc + r, color);
  PutPixel(xc, yc - r, color);
  PutPixel(xc + r, yc, color);
  PutPixel(xc - r, yc, color);
  while (x < y) {
  if (c >= 0) {
  y-- ;
  yy = yy + 2;
  c = c + yy;
  }
  x++ ;
  xx = xx + 2;
  c = c + xx;
  PutPixel(xc + x, yc + y, color);
  PutPixel(xc - x, yc + y, color);
  PutPixel(xc + x, yc - y, color);
  PutPixel(xc - x, yc - y, color);
  PutPixel(xc + y, yc + x, color);
  PutPixel(xc - y, yc + x, color);
  PutPixel(xc + y, yc - x, color);
  PutPixel(xc - y, yc - x, color);
  }
}

void GFX4d::PrintImage(uint16_t ui){
  if(cursor_x > (width - 1)) return;
  boolean tempnl = false;
  if(nl){
  cursor_x = 0;
  tempnl = true;
  newLine(lastfsh, textsizeht, 0);
  }
  if(cursor_y > (height - 1) && (rotation == 2 || rotation == 3 || sEnable == false)) return;
  if(userImag){
  String resultFO = "Success. ";
  } else {
  return;
  }
  uint16_t iwidth = tuiw[ui];
  uint16_t iheight = tuih[ui];
  uint8_t mul = cdv[ui] / 8;
  uint8_t tempfsh = fsh;
  uint32_t pos = tuiIndex[ui] + 6;
  userImag.seek(pos);
  uint16_t isize = iwidth * iheight;
  uint16_t ichunk = iwidth << (mul - 1);
  uint8_t buf[width << (mul - 1)];
  if(rotation == 0 || rotation == 1 || sEnable == false){
  if(((cursor_y + iheight) - 1) > height - 1){
  iheight = iheight - ((cursor_y + iheight) - height);
  }
  }
  boolean off = false;
  int cuiw = iwidth; 
  if((cursor_x + iwidth - 1) >= width){
  cuiw = iwidth - ((cursor_x + iwidth - 1) - width) - 1;
  off = true;
  }
  for(int idraw = 0; idraw < iheight; idraw ++){
  nl = true;
  newLine(1, 1, cursor_x);
  if((cursor_y - 1) < 0){
  setGRAM(cursor_x, cursor_y + height - 1 , cursor_x + cuiw -1 , cursor_y + height - 1);
  } else {
  setGRAM(cursor_x, cursor_y - 1 , cursor_x + cuiw -1 , cursor_y - 1);
  }
  if(off){
  userImag.read(buf, cuiw << (mul - 1));
  WrGRAMs8(buf, cuiw << (mul - 1), mul);
  pos = pos + (iwidth << (mul - 1));
  userImag.seek(pos);
  } else {
  userImag.read(buf, ichunk);
  WrGRAMs8(buf, ichunk, mul);
  }
  }
  if(tempnl){
  nl = true;
  lastfsh = 1;
  }
}

void GFX4d::DrawWidget(uint32_t Index, int16_t uix, int16_t uiy, int16_t uiw, int16_t uih, uint16_t frame, int16_t bar, bool images, byte cdv){
  if(bar != 0) uix = uix + bar;
  if(uix >= width || uiy >= height) return;
  if((uix + uiw - 1) < 0 || (uiy + uih - 1) < 0) return;
  if(!userImag) return;
  boolean off = false;
  byte ofst = 6;
  if(images) ofst = 8;
  int left = 0;
  int top = 0;
  int right = 0;
  int bottom = 0;
  int cuix = uix;
  int cuiy = uiy;
  int cuiw = uiw;
  int cuih = uih;  
  if(uix < 0){
  off = true;
  left = 0 - uix;
  cuix = 0;
  cuiw = cuiw + uix;
  }
  if(uiy < 0){
  off = true;
  top = 0 - (uiy * uiw);
  cuiy = 0;
  cuih = cuih + uiy;
  }
  if((uix + uiw - 1) > width){
  off = true;
  right = uiw - width;
  cuiw = width - cuix;
  }
  if((uiy + uih - 1) > height){
  off = true;
  bottom = uih - height;
  cuih = height - cuiy;
  }
  int steps = 0;
  int rem;
  int mul = 2;
  uint32_t isize;
  uint32_t isize2;
  uint32_t pos;
  int bufsize = 2048;
  if(cdv == 8) mul = 1;
  isize = (uiw * uih) << (mul - 1);
  setGRAM(cuix, cuiy , cuix + cuiw - 1 , cuiy + cuih - 1);
  if(off == true){
  pos = ((isize * frame) + ((left + top) << (mul - 1)));
  userImag.seek(Index + ofst + pos);
  isize2 = (cuiw * cuih) << (mul - 1);
  bufsize = cuiw << (mul - 1);
  steps = isize2 / bufsize;
  rem = 0;
  } else {
  pos = (isize * frame);
  userImag.seek(Index + ofst + pos);
  if(isize > bufsize) steps = isize / bufsize;
  rem = isize - (steps * bufsize);
  }
  uint8_t buf[bufsize];
  #ifndef ESP8266
  uint16_t pc = 0;
  #endif
  if(steps > 0){
  for(int n = 0; n < steps; n++){
  #ifndef ESP32
  userImag.read(buf, bufsize);
  WrGRAMs8(buf, bufsize, mul);
  #else
  userImag.read(buf, bufsize);
  if(mul == 2){
  for(int b = 0; b < bufsize; b += 2){
  buf16[pc] = (buf[b] << 8) | buf[b + 1];
  pc ++;
  if(pc == 12000){
  WrGRAMs16(buf16, pc << 1);
  //WrGRAMs16232(buf16, pc);
  pc = 0;
  } 
  }
  } else {
  WrGRAMs8(buf, bufsize, mul);
  }
  #endif
  if(off == true){
  pos = pos + (uiw << (mul - 1));
  userImag.seek(Index + ofst + pos);
  }
  }
  }
  if(rem > 0){
  userImag.read(buf, rem);
  #ifndef ESP32
  WrGRAMs8(buf, rem, mul);
  #else
  if(mul == 2){
  for(int b = 0; b < rem; b += 2){
  buf16[pc] = (buf[b] << 8) | buf[b + 1];
  pc ++;
  if(pc == 12000){
  WrGRAMs16(buf16, pc << 1);
  //WrGRAMs16232(buf16, pc);
  pc = 0;
  } 
  }
  } else {
  WrGRAMs8(buf, rem, mul);
  }
  #endif
  }
  #ifndef ESP8266
  if(pc > 0 && mul == 2){
  WrGRAMs16(buf16, pc << 1);
  //WrGRAMs16232(buf16, pc);
  }
  #endif
}

void GFX4d::UserImage(uint16_t ui){
  UserImage(ui, 0x7fff, 0x7fff);
}

void GFX4d::UserImage(uint16_t ui, int altx, int alty){
  if(!userImag){
  return;
  }
  boolean setemp = sEnable;
  ScrollEnable(false);
  int gciapos = (ui) * 13;
  if(altx == 0x7fff && alty == 0x7fff){
  DrawWidget(tuiIndex[ui], tuix[ui], tuiy[ui], tuiw[ui], tuih[ui], 0, 0, false, cdv[ui]);
  } else {
  DrawWidget(tuiIndex[ui], altx, alty, tuiw[ui], tuih[ui], 0, 0, false, cdv[ui]);
  }
  ScrollEnable(setemp);
}

void GFX4d::LedDigitsDisplaySigned(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks){
  LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks, 0x7fff ,0x7fff);
}

void GFX4d::LedDigitsDisplaySigned(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks, int16_t altx, int16_t alty){
  int16_t i, m, lstb, nv, digita[7];
  int leftpos = 0;
  nv = newval ;
  lstb = 1 ;
  for (i = Digits; i > 0; i--){
  m = nv % 10 ;
  if ( LeadingBlanks && (i <= Digits - MinDigits) ){
  if (nv == 0){
  m = 10 ;
  if (lstb == 1) lstb = i ;
  }
  }
  digita[i] = abs(m) ;
  nv /= 10 ;
  }
  if (newval < 0){
  digita[lstb] = 11 ;
  }
  for (i = 1; i <= Digits; i++){
  if(altx == 0x7fff && alty == 0x7fff){
  UserImages(index, digita[i], leftpos);
  } else {
  UserImages(index, digita[i], leftpos, altx, alty);
  }
  leftpos += WidthDigit ;
  }
}

void GFX4d::LedDigitsDisplay(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks){
  LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks, 0x7fff ,0x7fff);
}

void GFX4d::LedDigitsDisplay(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks, int16_t altx, int16_t alty){
  int16_t i, k, lb;
  int32_t l;
  l = 1;
  for(i = 1; i < Digits; i++)
  l *= 10;
  lb = LeadingBlanks;
  for(i = 0; i < Digits; i++){
  k = newval / l;
  newval -= k * l;
  if(lb && (i < Digits - MinDigits)){ 
  if(k == 0)
  k = 10;
  else
  lb = 0;
  }
  l /= 10;
  if(altx == 0x7fff && alty == 0x7fff){
  UserImages(index, k , i*WidthDigit);
  } else {
  UserImages(index, k , i*WidthDigit, altx, alty);
  }
  }
}

void GFX4d::UserImages(uint16_t uisnb, int16_t framenb){  
  boolean setemp = sEnable;
  ScrollEnable(false);
  if(framenb > (gciobjframes[uisnb] -1) || framenb < 0){
  outofrange(tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb]);
  } else {
  DrawWidget(tuiIndex[uisnb], tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb], framenb, 0, true, cdv[uisnb]);
  }
  ScrollEnable(setemp);
}

void GFX4d::UserImages(uint16_t uis, int16_t frame, int offset, int16_t altx, int16_t alty){  
  boolean setemp = sEnable;
  ScrollEnable(false);
  if(frame > (gciobjframes[uis]- 1) || frame < 0){
  outofrange(altx, alty, tuiw[uis], tuih[uis]);
  } else {
  DrawWidget(tuiIndex[uis], altx, alty, tuiw[uis], tuih[uis], frame, offset, true, cdv[uis]);
  }
  ScrollEnable(setemp);
}

void GFX4d::outofrange(int16_t euix, int16_t euiy, int16_t euiw, int16_t euih){
  if(euix >= width || euiy >= height) return;
  if((euix + euiw - 1) < 0 || (euiy + euih - 1) < 0) return;
  int cuix = euix;
  int cuiy = euiy;
  int cuiw = euiw;
  int cuih = euih;
  if(euix < 0){
  cuix = 0;
  cuiw = euiw + euix;
  }
  if(euiy < 0){
  cuiy = 0;
  cuih = euih + euiy;
  }
  if((euix + euiw - 1) >= width) cuiw = euiw - ((euix + euiw - 1) - width) - 1;
  if((euiy + euih - 1) >= height) cuih = euih - ((euiy + euih - 1) - height) - 1; 
  RectangleFilled(cuix, cuiy, cuix + cuiw - 1, cuiy + cuih - 1, BLACK);
  Rectangle(cuix + 1, cuiy + 1, cuix + cuiw - 2, cuiy + cuih - 2, RED);
  Line(cuix + 1, cuiy + 1, cuix + cuiw - 2, cuiy + cuih - 2, RED); 
  Line(cuix + cuiw - 2, cuiy + 1, cuix + 1, cuiy + cuih - 2, RED); 
}

void GFX4d::UserImages(uint16_t uisnb, int16_t framenb, int16_t newx, int16_t newy){  
  uimage = false;  
  boolean setemp = sEnable;
  ScrollEnable(false);
  if(framenb > (gciobjframes[uisnb] -1) || framenb < 0){
  outofrange(tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb]);
  } else {
  DrawWidget(tuiIndex[uisnb], newx, newy, tuiw[uisnb], tuih[uisnb], framenb, 0, true, cdv[uisnb]);
  }
  ScrollEnable(setemp);
}

void GFX4d::UserImages(uint16_t uis, int16_t frame, int offset){   
  boolean setemp = sEnable;
  ScrollEnable(false);
  if(frame > (gciobjframes[uis]- 1) || frame < 0){
  outofrange(tuix[uis], tuiy[uis], tuiw[uis], tuih[uis]);
  } else {
  DrawWidget(tuiIndex[uis], tuix[uis], tuiy[uis], tuiw[uis], tuih[uis], frame, offset, true, cdv[uis]);
  }
  ScrollEnable(setemp);
}

void GFX4d::ScrollEnable(boolean se){
  if(se){
  sEnable = true;
  } else {
  sEnable = false;
  }
}

void GFX4d::PrintImageFile(String ifile){
  if(cursor_x >= (width - 1)) return;
  boolean tempnl = false;
  if(nl){
  tempnl = true;
  newLine(lastfsh, textsizeht, 0);
  }
  if(cursor_y > (height - 1) && (rotation == 2 || rotation == 3 || sEnable == false)) return;
  uint16_t iwidth;
  uint16_t iheight; 
  #ifndef USE_FS
  dataFile = SD.open(ifile);
  #else
  dataFile = SPIFFS.open((char*)ifile.c_str(), "r");
  #endif
  if(!dataFile){
  return;
  }
  iwidth = (dataFile.read() << 8); iwidth = iwidth + dataFile.read();
  iheight = (dataFile.read() << 8); iheight =  iheight + dataFile.read();
  uint8_t mul = dataFile.read() / 8;
  uint8_t dummy = dataFile.read();
  uint8_t tempfsh = fsh;
  uint32_t pos = 6;
  uint16_t isize = iwidth * iheight;
  uint16_t ichunk = iwidth << (mul - 1);
  uint8_t buf[width << (mul - 1)];
  if(rotation == 0 || rotation == 1 || sEnable == false){
  if(((cursor_y + iheight) - 1) > height - 1){
  iheight = iheight - ((cursor_y + iheight) - height);
  }
  }
  boolean off = false;
  int cuiw = iwidth; 
  if((cursor_x + iwidth - 1) >= width){
  cuiw = iwidth - ((cursor_x + iwidth - 1) - width) - 1;
  off = true;
  }
  for(int idraw = 0; idraw < iheight; idraw ++){
  nl = true;
  newLine(1, 1, cursor_x);
  if((cursor_y - 1) < 0){
  setGRAM(cursor_x, cursor_y + height - 1 , cursor_x + cuiw -1 , cursor_y + height - 1);
  } else {
  setGRAM(cursor_x, cursor_y - 1 , cursor_x + cuiw -1 , cursor_y - 1);
  }
  if(off){
  dataFile.read(buf, cuiw << (mul - 1));
  WrGRAMs8(buf, cuiw << (mul - 1), mul);
  pos = pos + (iwidth << (mul - 1));
  dataFile.seek(pos);
  } else {
  dataFile.read(buf, ichunk);
  WrGRAMs8(buf, ichunk, mul);
  }
  }
  if(tempnl){
  nl = true;
  lastfsh = 1;
  }
}

void GFX4d::PrintImageWifi(String Address, uint16_t port, String hfile){
  ImageWifi(true, Address, port, hfile, "");
}

void GFX4d::PrintImageWifi(String WebAddr){
  ImageWifi(false, WebAddr, 0, "", "");
}

void GFX4d::PrintImageWifi(String Address, uint16_t port, String hfile, String sha1){
  ImageWifi(true, Address, port, hfile, sha1);
}

void GFX4d::PrintImageWifi(String WebAddr, String sha1){
  ImageWifi(false, WebAddr, 0, "", sha1);
}

void GFX4d::ImageWifi(boolean local, String Address, uint16_t port, String hfile, String sha1){
  if(cursor_x >= (width - 1)) return;
  boolean tempnl = false;
  if(nl){
  tempnl = true;
  newLine(lastfsh, textsizeht, 0);
  }
  HTTPClient http;
  if(local){
  if(sha1 == ""){
  if(!http.begin(Address,port,hfile)) return;
  } else {
  #ifndef ESP32
  if(!http.begin(Address,port,hfile,sha1)) return;
  #endif
  }
  } else {
  if(sha1 == ""){
  if(!http.begin(Address)) return;
  } else {
  #ifndef ESP32
  if(!http.begin(Address,sha1)) return;
  #endif
  }
  }
  yield();
  int httpCode = http.GET();
  yield();
  if(httpCode == 404){
  println("File Error");
  return;
  }
  yield();
  uint16_t iwidth;
  uint16_t iheight;   
  uint16_t coldepth;
  int mul = 2; 
  uint32_t len = http.getSize();
  yield();
  uint8_t buff[6] = { 0 };
  WiFiClient * stream = http.getStreamPtr();
  size_t size = stream->available();
  if(size < 1) return;
  int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
  yield();
  size = size - c;
  if(size < 1) return;
  iwidth = (buff[0] << 8) + buff[1];
  iheight = (buff[2] << 8) + buff[3];
  mul = buff[4] / 8;
  uint8_t tempfsh = fsh;
  if(rotation == 0 || rotation == 1 || sEnable == false){
  if(((cursor_y + iheight) - 1) > height - 1){
  iheight = iheight - ((cursor_y + iheight) - height);
  }
  }
  boolean off = false;
  int cuiw = iwidth;
  int tbufsize = 0; 
  if((cursor_x + iwidth - 1) >= width){
  cuiw = iwidth - ((cursor_x + iwidth - 1) - width) - 1;
  off = true;
  tbufsize = (iwidth - cuiw) << (mul - 1);
  }
  uint8_t tbuf[tbufsize];
  int bufsize = cuiw << (mul - 1);
  uint8_t buf[bufsize];
  for(int idraw = 0; idraw < iheight; idraw ++){
  yield();
  nl = true;
  newLine(1, 1, cursor_x);
  if((cursor_y - 1) < 0){
  setGRAM(cursor_x, cursor_y + height - 1 , cursor_x + cuiw -1 , cursor_y + height - 1);
  } else {
  setGRAM(cursor_x, cursor_y - 1 , cursor_x + cuiw -1 , cursor_y - 1);
  }
  if(off){
  c = stream->readBytes(buf, bufsize);
  c = stream->readBytes(tbuf, tbufsize);
  WrGRAMs8(buf, bufsize, mul);
  } else {
  c = stream->readBytes(buf, bufsize);
  WrGRAMs8(buf, bufsize, mul);
  }
  }
  if(tempnl){
  nl = true;
  lastfsh = 1;
  }
}

void GFX4d::Open4dGFX(String file4d){
  if(userImag) Close4dGFX();
  uint8_t um;
  uint8_t strpos = 0;
  uint8_t gotchar = 0;
  uint8_t ofset = 0;
  gciobjnum = 0;
  imageTouchEnable(-1, false);
  String inputString;
  dat4d = file4d + ".dat";
  gci4d = file4d + ".gci";
  #ifndef USE_FS
  userDat = SD.open(dat4d);
  #else
  userDat = SPIFFS.open((char*)dat4d.c_str(), "r");
  #endif
  if(userDat){
  char c;
  while(userDat.available() >0) {
  c = userDat.read();
  strpos = strpos + 1;
  if(c != char(13) && c != char(10)){
  if(c == char(34)){
  gotchar = gotchar + 1;
  }
  if(gotchar == 2){
  ofset = strpos - 1;
  gotchar = 0;
  }
  inputString = inputString + char(c);
  }
  if(c == char(13)){ 
  strpos = 0;
  String tempis = inputString;
  uint32_t tuindex = getIndexfromString(tempis, (2 + ofset)); 
  tuiIndex[gciobjnum] = tuindex;
  uint32_t tuiPos = getCoordfromString(tempis, (12 + ofset));
  tuix[gciobjnum] = xic;
  tuiy[gciobjnum] = yic;
  inputString = "";
  strpos = 0;
  gciobjnum = gciobjnum + 1;
  }
  }  
  }
  userDat.close();
  #ifndef USE_FS
  userImag = SD.open(gci4d);
  #else
  userImag = SPIFFS.open((char*)gci4d.c_str(), "r");
  #endif
  uint32_t tIndex;
  int coldepth;
  for(int n = 0; n < gciobjnum; n ++){
  tIndex = tuiIndex[n];
  userImag.seek(tIndex);
  tuiw[n] = (userImag.read() << 8) + userImag.read();
  tuih[n] = (userImag.read() << 8) + userImag.read();
  cdv[n] = userImag.read();
  coldepth = userImag.read();
  gciobjframes[n] = (userImag.read() << 8) + userImag.read();
  }
}

uint16_t GFX4d::getWidgetNumFrames(int widget){
  return gciobjframes[widget];
}

uint32_t GFX4d::getIndexfromString(String strval, uint8_t indx) {
  String tempstrval;
  for(int n = 0; n < 4; n ++){
  tempstrval = tempstrval + strval.charAt(indx + n + 4);
  }
  for(int n = 0; n < 4; n ++){
  tempstrval = tempstrval + strval.charAt(indx + n -1);
  }
  uint32_t tempaddr;
  tempaddr = strtol(&tempstrval[0], NULL, 16);
  return tempaddr;
}

uint32_t GFX4d::getCoordfromString(String strval, uint8_t indx) {
  String tempstrval = "";
  char c;
  for(int n = 0; n < 18; n ++){
  c = strval.charAt(indx + n - 1);
  if(c != char(32) && c != char(0)){
  tempstrval = tempstrval + char(c);
  }
  if(c == char(32)){
  if(tempstrval.length() > 7){
  String tbuild = "";
  for(int o = 0; o < 4; o++){
  tbuild = tbuild + tempstrval.charAt(o + 4);
  }
  tempstrval = tbuild;
  }
  xic = strtol(&tempstrval[0], NULL, 16);
  tempstrval = "";
  }
  if(c == char(0)){
  if(tempstrval.length() > 7){
  String tbuild = "";
  for(int o = 0; o < 4; o++){
  tbuild = tbuild + tempstrval.charAt(o + 4);
  }
  tempstrval = tbuild;
  }
  yic = strtol(&tempstrval[0], NULL, 16);
  break;
  }
  }
  uint32_t tempcoord;
  tempcoord = (xic << 16) & yic;
  return tempcoord;
}

void GFX4d::SmoothScrollSpeed(uint8_t spd){
  ssSpeed = spd;
}

void GFX4d::newLine(int8_t f1, int8_t ts, uint16_t ux){
  fsh1 = f1;
  nl = false;
  int tcy2;
  uint8_t remspc = ts * fsh1;
  if(rotation != 2 && rotation != 3){
  cursor_y += remspc;
  if(cursor_y > 239) cursor_y = 239;
  cursor_x  = ux;
  return;
  }
  if(rotation == 2 || rotation == 3){
  uint16_t offset = 0;
  if(scrolled == false){
  if((cursor_y + remspc )< 320){
  cursor_y += textsizeht * fsh1;
  cursor_x  = ux;
  return;
  }
  }
  if(scrolled){
  uint16_t diff = ts * fsh1;
  int16_t tempy = scrollOffset;
  for(int16_t sn = 0; sn < diff + 1; sn++){
  delay(ssSpeed);  

  if(rotation == 3){
  tcy2 = 320 - scrollOffset;
  } else { 
  tcy2 = scrollOffset;
  }
  int tcy21 = tcy2 - 1;
  if(tcy21 < 0){
  tcy21 = tcy21 + 320;
  }
  if(textbgcolor != textcolor){  
  Hline(0, tcy2, 240, textbgcolor);
  Hline(0, tcy21, 240, textbgcolor);
  } else {
  Hline(0, tcy2, 240, BLACK);
  Hline(0, tcy21, 240, BLACK);
  }
  
  if(rotation == 2){
  if((tempy + sn) > 319){ 
  Scroll(tempy + sn - 320);
  } else {
  Scroll(tempy + sn);
  }
  }
  if(rotation == 3){
  if((tempy - sn) < 0){ 
  Scroll(320 - tempy - sn);
  } else {
  Scroll(tempy - sn);
  }
  }
  offset = sn; 
  }
  if(rotation == 2){
  cursor_y = tempy;
  if(cursor_y > 319){
  cursor_y = cursor_y - 320;
  }
  }
  if(rotation == 3){
  cursor_y = 320 - tempy;
  if(cursor_y > 319){
  cursor_y = cursor_y - 320;
  }
  }
  cursor_x  = ux;
  }
  if(scrolled == false){
  if((cursor_y + remspc) > 319){
  setScrollArea(0, 0);
  int16_t diff = remspc;
  int16_t tempy = scrollOffset;
  for(int16_t sn = 0; sn < diff + 1; sn++){
  delay(ssSpeed); 

  if(rotation == 3){
  tcy2 = 320 - scrollOffset;
  } else { 
  tcy2 = scrollOffset;
  }
  int tcy21 = tcy2 - 1;
  if(tcy21 < 0){
  tcy21 = tcy21 + 320;
  }
  if(textbgcolor != textcolor){  
  Hline(0, tcy2, 240, textbgcolor);
  Hline(0, tcy21, 240, textbgcolor);
  } else {
  Hline(0, tcy2, 240, BLACK);
  Hline(0, tcy21, 240, BLACK);
  }

  if(rotation == 2){
  if((tempy + sn) > 319){ 
  Scroll(tempy + sn - 320);
  } else {
  Scroll(tempy + sn);
  }
  }
  if(rotation == 3){
  if((tempy - sn) < 0){ 
  Scroll(320 - tempy - sn);
  } else {
  Scroll(tempy - sn);
  }
  }
  offset = sn;  
  }
  if(rotation == 2){
  cursor_y = tempy;
  }
  if(rotation == 3){
  cursor_y = 319 - tempy + 1;
  }
  if(cursor_y > 319){
  cursor_y = cursor_y - 320;
  }
  cursor_x = ux;
  }
  }  
  }
}

size_t GFX4d::write(uint8_t c) {
  if(nl){
  newLine(lastfsh, textsizeht, 0);
  }
  if (c == 10) {
  nl = true;
  lastfsh = fsh;
  lastfsw = fsw;
  }
  if (c == 13){
  }
  if(c != 10 && c != 13 && (c > 31 && c < 128)){
  if(fno == 1){   
  drawChar1(cursor_x, cursor_y, c - 32, textcolor, textbgcolor, textsize, textsizeht);
  }    
  if(fno == 2){   
  drawChar2(cursor_x, cursor_y, c - 32, textcolor, textbgcolor, textsize, textsizeht);
  }  
  cursor_x += textsize * (fsw + 1);
  if (wrap && (cursor_x > (width - textsize * (fsw + 1)))) {
  newLine(fsh, textsizeht, 0);
  }
  }
  return 1;
}

void GFX4d::drawChar2(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t sizew, uint8_t sizeht) {
  if(rotation != 3 && rotation != 2){  
  if((x >= width)||(y >= height)||((x + (fsw + 1) * sizew - 1) < 0)||((y + fsh * sizeht - 1) < 0)){  
  return;
  }
  }
  //if(sizew == 1 && sizeht == 1 && (bg != color)){
  //drawChar2tw(x, y, c, color, bg, 1);
  //return ;
  //}
  for (int8_t j = 0; j < 16; j++ ) {
  uint8_t trow;
  trow = font2[(c * 16) + j];
  for (int8_t i = 0; i < (fsw + 1); i++) {     
  if (i == (fsw)){ 
  trow = 0x00;
  } 
  if (trow & 0x80) {
  if (sizew == 1 && sizeht == 1){
  if(y + j > 319 && (rotation == 3 || rotation == 2)){
  PutPixel(x + i, y + j - 320, color);
  } else {
  PutPixel(x + i, y + j, color);
  }
  } else {
  RectangleFilled(x + (i * sizew), y + (j * sizeht), (sizew + x) + (i * sizew) - 1, (sizeht + y) + (j * sizeht) - 1, color);
  } 
  } else if (bg != color) {
  if (sizew == 1 && sizeht == 1){
  if(y + j > 319 && (rotation == 3 || rotation == 2)){
  PutPixel(x + i, y + j - 320, bg);
  } else {
  PutPixel(x + i, y + j, bg);
  }
  } else {
  RectangleFilled(x + (i* sizew), y + (j * sizeht), (sizew + x) + (i* sizew) - 1, (sizeht + y) + (j * sizeht) - 1, bg);
  }
  }
  trow <<= 1;
  }
  }
}

void GFX4d::drawChar2tw(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if((x >= width)||(y >= height)||((x + (8 + 1) * size - 1) < 0)||((y + 16 * size - 1) < 0)){  
  return;
  }
  if(c < 32 && c > 128){
  return;
  }
  uint16_t temppix[257];
  uint16_t pc = 0;
  uint32_t tfval;
  setGRAM(x, y, x + 7, y + 15);
  uint8_t trow = 0x80;
  uint8_t chb;    
  uint16_t c2pos = c * 16;
  for (uint8_t j = 0; j < 16; j++ ) {
  chb = font2[c2pos + j]; 
  for (uint8_t i = 0; i < 8; i++){
  if (chb & trow){ 
  temppix[pc] = color;
  } else {
  temppix[pc] = bg;
  }  
  chb <<= 1;
  pc ++;
  }
  trow = 0x80;
  }
  #ifndef ESP8266
  WrGRAMs16(temppix, 256);
  #else
  WrGRAMs16232(temppix, 128);
  #endif
}

void GFX4d::drawChar1tw(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if((x >= width)||(y >= height)||((x + (fsw + 1) * size - 1) < 0)||((y + fsh * size - 1) < 0)){  
  return;
  }
  if(c < 32 && c > 128){
  return;
  }
  setGRAM(x, y, x + 4, y + 7);
  uint8_t trow = 0x01;
  uint8_t chb; 
  uint8_t chb1;
  uint16_t temppix[40];
  uint8_t pc = 0;  
  for (uint8_t j = 0; j < 8; j++ ) { 
  for (uint8_t i = 0; i < 5; i++){
  chb = font1[(c * 5) + i]; 
  chb1 = chb >> j;  
  if (chb1 & trow){ 
  temppix[pc] = color;
  //WrGRAM16(color);
  } else {
  temppix[pc] = bg;
  //WrGRAM16(bg);
  }
  pc ++;  
  }
  }
  #ifndef ESP8266
  WrGRAMs16(temppix, 160);
  #else
  WrGRAMs16232(temppix, 80);
  //WrGRAMs16(temppix, 128);
  #endif
}

void GFX4d::TWprintln(String istr){
  istr = istr + char(13);
  char tistr;
  uint16_t len = istr.length();
  for(int n = 0; n < len; n++){
  tistr = istr.charAt(n);
  TWwrite(tistr); 
  }
}

void GFX4d::TWprint(String istr){
  char tistr;
  uint16_t len = istr.length();
  for(int n = 0; n < len; n++){
  tistr = istr.charAt(n);
  TWwrite(tistr); 
  }
}

String GFX4d::GetCommand(){
  String tcmdtxt = cmdtxt;
  cmdtxt = "";
  return tcmdtxt;
}

void GFX4d::TWtextcolor(uint16_t twc){
  twcolnum = twc;
}

boolean GFX4d::TWMoveTo(uint8_t twcrx, uint8_t twcry){
  if(twcrx <= chracc && twcry <= chrdwn && chracc > 0 && chrdwn > 0){
  twcurx = txtx + (9 * twcrx);
  twcury = txty + (16 * twcry);
  twxpos = twcrx;
  twypos = twcry;
  return true;
  } else {
  return false;
  }
}

void GFX4d::TWprintAt(uint8_t pax, uint8_t pay, String istr){
  if(TWMoveTo(pax, pay)){
  TWprint(istr);
  }
}

void GFX4d::TWwrite(const char txtinput){
  drawChar2tw(twcurx, twcury, 0, txtf, txtb, 1);
  boolean skip2 = false;
  if(txtinput > 31){
  twtext = twtext + char(txtinput);
  drawChar2tw(twcurx, twcury, txtinput - 32, twcolnum, txtb, 1);
  txtbuf[(chracc * twypos) + twxpos] = txtinput;
  txfcol[(chracc * twypos) + twxpos] = twcolnum;
  twcurx = twcurx + 9;
  twxpos ++;
  if((twcurx + 8 + 1) > (txtw + txtx)){
  twcury = twcury + 16;
  twcurx = txtx;
  twypos ++;
  twxpos = 0;
  }
  }
  if(txtinput == 9){
  uint tcnt = 0;
  uint ccpos = twcurx / 9;
  for(int n = 0; n < (chracc / 10); n++){
  tcnt = tcnt + 14;
  if(tcnt > ccpos){
  for(int o = 0; o < (tcnt - ccpos); o++){
  twtext = twtext + char(32);
  twcurx = twcurx + 9;
  twxpos ++;
  txtbuf[(chracc * twypos) + twxpos] = 32; // 8 + 2
  txfcol[(chracc * twypos) + twxpos] = twcolnum;
  }
  break;
  }
  }
  }
  if(txtinput == 13 || txtinput == 10){
  twcury = twcury + 16;
  twypos ++;
  uint8_t remtxt = chracc - twxpos + 1;
  twcurx = txtx;
  twxpos = 0;
  twcl = twcl + 1;
  if(txtinput == 13){
  cmdtxt = twtext;
  twtext = "";
  }
  for(int n = 0; n < remtxt; n ++){
  txtbuf[(chracc * (twypos - 1)) + (chracc - n)] = char(0);
  } 
  }
  if(txtinput == 8){
  if(twypos < 1 && twxpos < 1){
  return;
  }
  uint16_t lenct = twtext.length();  
  if((twcurx - txtx + 6) < (10) && lenct > 0 && twcury > fsh){
  skip2 = true;
  twcury = twcury - 16;
  twypos --;
  twcurx = txtx + (((txtw / 9) * 9) - 9);
  twxpos = chracc - 1; 
  drawChar2tw(twcurx, twcury, 0, twcolnum, txtb, 1);
  txtbuf[(chracc * twypos) + twxpos] = txtinput;
  txfcol[(chracc * twypos) + twxpos] = twcolnum;
  }
  if(twcurx > txtx && lenct > 0  && skip2 == false){
  twcurx = twcurx - 8 - 1;
  twxpos --;
  drawChar2tw(twcurx, twcury, 0, twcolnum, txtb, 1);
  txtbuf[(chracc * twypos) + twxpos] = txtinput;
  txfcol[(chracc * twypos) + twxpos] = twcolnum;
  }  
  String tempcmd = "";
  for(int n = 0; n < (lenct - 1); n++){
  tempcmd = tempcmd + twtext.charAt(n);
  }
  twtext = tempcmd;
  }
  if((twcury - txty) + 16 > txth){
  uint16_t tempc;
  uint16_t tempp;
  uint16_t tempcpos;
  uint16_t temptwcol;
  boolean drawspc1;
  boolean drawspc2;
  for(int n = 0; n < chrdwn - 1; n ++){
  yield();  
  for(int o = 0; o < chracc; o ++){
  tempcpos = ((n + 1) * chracc) + o;
  tempc = txtbuf[tempcpos];
  tempp = txtbuf[(n * chracc) + o];
  temptwcol = txfcol[tempcpos];
  txtbuf[(n * chracc) + o] = tempc;
  txfcol[(n * chracc) + o] = temptwcol;
  if((tempc < 33) && (tempp < 33)){
  drawspc1 = false; 
  } else {
  drawspc1 = true;
  }
  if(tempc < 32){
  tempc = 32;
  }
  drawChar2tw(txtx + (9 * o), txty + (16 * n), tempc - 32, temptwcol, txtb, 1);
  }
  }  
  twcury = twcury - 16;
  twypos --;
  RectangleFilled(twcurx, twcury, twcurx + (txtw - 1) - 1, twcury + 16, txtb);
  }
  if(twcurson) drawChar2tw(twcurx, twcury, 63, txtf, txtb, 1);
}

void GFX4d::TWcursorOn(bool twco){
  twcurson = twco;
}

void GFX4d::TWcls(){
  RectangleFilled(txtx - 3, txty - 3, (txtx - 3) + (txtw + 2) - 1, (txty - 3) + (txth + 2) - 1, txtb);
  twcurx = txtx;
  twcury = txty;
  twxpos = 0;
  twypos = 0;
  for(int n = 0; n < sizeof(txtbuf); n ++){
  txtbuf[n] = 0;
  }
}

void GFX4d::TWcolor(uint16_t fcol){
  txtf = fcol;
  TWtextcolor(fcol);
}

void GFX4d::TWcolor(uint16_t fcol, uint16_t bcol){
  txtf = fcol;
  txtb = bcol;
  TWtextcolor(fcol);
}

void GFX4d::TextWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t txtcolor, uint16_t txbcolor, uint16_t frcolor){
  for(int n = 0; n < 600; n ++){
  txtbuf[n] = 0;
  }
  twxpos = 0;
  twypos = 0;
  if(w < 24) w = 24;
  if(h < 31) h = 31;
  if(x < 0) x = 0;
  if(y < 0) y = 0;
  if((w + x) > width) w = w - ((w + x) - width);
  if((h + y) > height) h = h - ((h + y) - height);
  txtwin = true;
  TWtextcolor(txtcolor);
  txtf = txtcolor;
  txtb = txbcolor;
  txtx = x + 6;
  twcurx = txtx;
  txty = y + 6;
  twcury = txty;
  txtw = w - 8;
  txth = h - 8; 
  twtext = "";
  twframe = true;
  twframecol = frcolor;
  chracc = txtw / 9;
  chrdwn = txth / 16;
  PanelRecessed(x, y, w, h, frcolor);
  RectangleFilled(x + 3, y + 3, (x + 3) + (w - 6) - 1, (y + 3) + (h - 6) - 1, txbcolor);    
}

void GFX4d::TextWindowRestore(){
  uint16_t chracc = txtw / (fsw +1);
  uint8_t chrdwn = txth / fsh;
  txtwin = true;
  twtext = "";
  uint16_t tcoltw;
  if(twframe){
  PanelRecessed(txtx - 6, txty - 6, txtw + 8 , txth + 8, twframecol);
  RectangleFilled(txtx - 3, txty - 3, (txtx - 3) + (txtw + 2) - 1, (txty - 3) + (txth + 2) - 1, txtb);
  } else {
  RectangleFilled(txtx - 3, txty - 3, (txtx - 3) + (txtw + 2) - 1, (txty - 3) + (txth + 2) - 1, txtb);
  }
  uint16_t tempc;
  for(int n = 0; n < (chrdwn - 1); n ++){
  yield();  
  for(int o = 0; o < chracc; o ++){
  tempc = txtbuf[(n * chracc) + o];
  txtbuf[((n * chracc) +o)] = tempc;
  if(tempc < 32){
  tempc = 32;
  }
  tcoltw = txfcol[(n  * chracc) + o];
  drawChar2tw(txtx + ((fsw + 1) * o), txty + (fsh * n), tempc - 32, tcoltw, txtb, 1);
  }
  }     
}

void GFX4d::TextWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t txtcolor, uint16_t txbcolor){
  for(int n = 0; n < 600; n ++){
  txtbuf[n] = 0;
  }
  if(w < 22) w = 22;
  if(h < 29) h = 29;
  if(x < 0) x = 0;
  if(y < 0) y = 0;
  if((w + x) > width) w = w - ((w + x) - width);
  if((h + y) > height) h = h - ((h + y) - height);
  twxpos = 0;
  twypos = 0;
  TWtextcolor(txtcolor);
  txtwin = true;
  txtf = txtcolor;
  txtb = txbcolor;
  txtx = x + 3;
  twcurx = txtx;
  txty = y + 3;
  twcury = txty;
  txtw = w - 6;
  txth = h - 6; 
  twtext = "";  
  twframe = false;
  chracc = txtw / 9;
  chrdwn = txth / 16;
  RectangleFilled(x, y, x + w - 1, y + h - 1, txbcolor);    
}

void GFX4d::drawChar1(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t sizew, uint8_t sizeht){
  int crad;
  int co;
  if(fstyle == 1 || fstyle == 2 || fstyle == 3) crad = sizew >> 1;
  if(fstyle == DOTMATRIXLED) co = crad * 68 / 100;
  if(sizew > 3)crad --;
  if(rotation != 3 && rotation != 2){
  if((x >= width) || (y >= height) || ((x + (fsw + 1) * sizew - 1) < 0) || ((y + fsh * sizeht - 1) < 0)){
  return;
  }
  }
  for (int8_t i = 0; i < 6; i++ ) {
  uint8_t tcol;
  if (i == (fsw)){ 
  tcol = 0x0;
  } else { 
  tcol = font1[(c * 5) + i];
  }
  for (int8_t j = 0; j < 8; j++) {
  if (i == 5) tcol = 0;
  if (tcol & 0x1) {
  if (sizew == 1 && sizeht == 1){
  if(y + j > 319 && (rotation == 3 || rotation == 2)){
  PutPixel(x + i, y + j - 320, color);
  } else {
  PutPixel(x + i, y + j, color);
  }
  } else { 
  if(fstyle == 0)RectangleFilled(x + (i * sizew), y + (j * sizeht), (sizew + x) + (i * sizew) - 1, (sizeht + y) + (j * sizeht) - 1, color); 
  if(fstyle == 2)Circle(x + (i * sizew) + crad, y + (j * sizeht) + crad, crad, color);
  if(fstyle == 1)CircleFilled(x + (i * sizew) + crad, y + (j * sizeht) + crad, crad, color);
  if(fstyle == 3){
  CircleFilled(x + (i * sizew) + crad, y + (j * sizeht) + crad, crad, color);  
  CircleFilled(x + (i * sizew) + co, y + (j * sizeht) + co, crad/3, WHITE);
  }
  if(fstyle == 4)RectangleFilled(x + (i * sizew), y + (j * sizeht), (sizew + x) + (i * sizew) - 2, (sizeht + y) + (j * sizeht) - 2, color); 
  if(fstyle == 5){
  uint16_t fadcol = color;
  fadcol = HighlightColors(fadcol, 10) & 0xffff; 
  int step = 60/(sizew / 2);
  if(step < 1) step = 1;
  for(int n = sizew / 2; n > -1; n --){
  Rectangle(x + (i * sizew) + n, y + (j * sizeht) + n, (sizew + x) + (i * sizew) - 1 - n, (sizeht + y) + (j * sizeht) - 1 - n, fadcol); 
  /*if(!(n % step))*/fadcol = HighlightColors(fadcol, step) >> 16; 
  }
  }
  }
  } else if (bg != color) {
  if (sizew == 1 && sizeht == 1){
  if(y + j > 319 && (rotation == 3 || rotation == 2)){
  PutPixel(x + i, y + j - 320, bg);
  } else {
  PutPixel(x + i, y + j, bg);
  }
  } else {
  RectangleFilled(x + i * sizew, y + j * sizeht, (sizew + x) + i * sizew - 1, (sizeht + y) + j * sizeht - 1, bg);
  }
  }
  tcol >>= 1;
  }
  }
}

void GFX4d::Arc( int16_t x0, int16_t y0, int16_t r, uint8_t sa, uint16_t color) {
  int16_t c = 1 - r;
  int16_t xx = 1;
  int16_t yy = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  while (x < y) {
  if (c >= 0) {
  y--;
  yy = yy + 2;
  c = c + yy;
  }
  x++;
  xx = xx + 2;
  c = c + xx;
  if(sa & 0x4) {
  PutPixel(x0 + x, y0 + y, color);
  PutPixel(x0 + y, y0 + x, color);
  } 
  if(sa & 0x2) {
  PutPixel(x0 + x, y0 - y, color);
  PutPixel(x0 + y, y0 - x, color);
  }
  if(sa & 0x8) {
  PutPixel(x0 - y, y0 + x, color);
  PutPixel(x0 - x, y0 + y, color);
  }
  if(sa & 0x1) {
  PutPixel(x0 - y, y0 - x, color);
  PutPixel(x0 - x, y0 - y, color);
  }
  }
}

void GFX4d::setGRAM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  sgx = x0;
  sgy = y0;
  sgw = x1 - x0 + 1;
  sgh = y1 - y0 + 1;
  digitalWrite(_cs, LOW);
  uint32_t casCoord = (x0 << 16) + x1;
  uint32_t pasCoord = (y0 << 16) + y1;
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_CASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(casCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_PASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(pasCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_RAMWR);
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, HIGH);
}

void GFX4d::setGRAM_(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  sgx = x0;
  sgy = y0;
  sgw = x1 - x0 + 1;
  sgh = y1 - y0 + 1;
  uint32_t casCoord = (x0 << 16) + x1;
  uint32_t pasCoord = (y0 << 16) + y1;
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_CASET);
  digitalWrite(_dc, HIGH);
  SPI.write32(casCoord);
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_PASET);
  digitalWrite(_dc, HIGH);
  SPI.write32(pasCoord);
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_RAMWR);
  digitalWrite(_dc, HIGH);
}

void GFX4d::WrGRAM(uint32_t color) {
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  #ifndef ESP32 
  SPI.write32(color, true);
  #else
  SPI.write32(color);
  #endif
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::WrGRAM16(uint16_t color) {
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  #ifndef ESP32
  SPI.write16(color, true);
  #else
  SPI.write16(color);
  #endif
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::WrGRAMs16(uint16_t *data, uint16_t l) {
  uint32_t tdw;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  #ifndef ESP32
  while(l--){
  tdw = *data++;
  SPI.write16(tdw, true);
  }
  #else
  SPI.writePixels(data, l);
  #endif
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::WrGRAMs8(uint8_t *data, uint16_t l, byte mul) {
  uint32_t tdw;
  uint16_t tdy;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  while(l){
  if(mul == 2){
  #ifndef ESP32
  if(l >= 4){
  tdw = (*data++ << 24) + (*data++ << 16) + (*data++ << 8) + *data++;
  l-= 4;
  SPI.write32(tdw);
  } else {
  tdy = (*data++ << 8) + *data++;
  l-= 2;
  SPI.write16(tdy);
  }
  #else
  SPI.writeBytes(data, l);
  l = 0;
  #endif
  } else {
  if(l >= 2){
  tdw = (RGB3322565[*data++] << 16) + RGB3322565[*data++];
  l-= 2;
  SPI.write32(tdw);
  } else {
  tdy = RGB3322565[*data++];
  l-= 1;
  SPI.write16(tdy);
  }
  }
  }
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::WrGRAMs(uint32_t *data, uint16_t l) {
  uint32_t tdw;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  while(l--){
  tdw = *data++;
  #ifndef ESP32
  SPI.write32(tdw, true);
  #else
  SPI.write32(tdw);
  #endif
  }
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::PutPixel(int16_t x, int16_t y, uint16_t color) {
  if((x < 0) || (x >= (width -1)) || (y < 0) || (y >= (height - 1))) {
  return;
  }
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_cs, LOW);
  uint32_t casCoord = (x << 16) + (x + 1);
  uint32_t pasCoord = (y << 16) + (y + 1);
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_CASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(casCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_PASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(pasCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_RAMWR);
  digitalWrite(_dc, HIGH);
  #ifndef ESP32
  SPI.write16(color, true);
  #else
  SPI.write16(color);
  #endif  
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::Vline(int16_t x, int16_t y, int16_t h, uint16_t vcolor) {
  if (x >= width || x < 0) return;
  if (h < 0) {
  y += h;
  h *= -1;
  }
  if (y < 0) {
  h += y;
  y = 0;
  }
  if((y + h - 1) >= height)   h = height - y;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_cs, LOW); 
  uint32_t casCoord = (x << 16) + x;
  uint32_t pasCoord = (y << 16) + (y + h - 1);
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_CASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(casCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_PASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(pasCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_RAMWR);
  digitalWrite(_dc, HIGH);
  //WrGRAM16232(vcolor, h);
  if(h < 2){
  SPI.write16(vcolor);
  } else {
  while(h > 0){
  SPI.write32((vcolor << 16) | vcolor);
  h -= 2;
  }
  if(h < 0) SPI.write16(vcolor);
  }
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::Hline(int16_t x, int16_t y, int16_t w, uint16_t hcolor) {
  if (y >= height || y < 0) return;
  if (w < 0) {
  x += w;
  w *= -1;
  }
  if (x < 0) {
  w += x;
  x = 0;
  }
  if ((x+w-1) >= width)  w = width-x;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_cs, LOW); 
  uint32_t casCoord = (x << 16) + (x + w - 1);
  uint32_t pasCoord = (y << 16) + y;
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_CASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(casCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_PASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(pasCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_RAMWR);
  digitalWrite(_dc, HIGH);
  if(w < 2){
  SPI.write16(hcolor);
  } else {
  //WrGRAM16232(hcolor, w);
  while(w > 0){
  SPI.write32((hcolor << 16) | hcolor);
  w -= 2;
  }
  if(w < 0) SPI.write16(hcolor);
  }
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::Cls(uint16_t color){
  RectangleFilled(0, 0, width, height, color);
  cursor_x = 0;
  cursor_y = 0;
  Scroll(0);
  scrolled = false;
  nl = false;
}

void GFX4d::Cls(){
  RectangleFilled(0, 0, width, height, BLACK);
  cursor_x = 0;
  cursor_y = 0;
  Scroll(0);
  scrolled = false;
  nl = false;
}

void GFX4d::FillScreen(uint16_t color){
  RectangleFilled(0, 0, width, height, color);
}

void GFX4d::RectangleFilled(int16_t x, int16_t y, int16_t x1, int16_t y1, uint16_t color) {
  if (x > x1) {
  swap(x, x1);
  }
  if (y > y1) {
  swap(y, y1);
  } 
  if((x >= width) || (y >= height) || (x1 < 0) || (y1 < 0)) {
  return;
  } 
  if(x < 0) x = 0;
  if(y < 0) y = 0;
  if(x1 >= width){
  x1 = width -1;
  }
  if(y1 >= height){
  y1 = height-1;
  }
  int w = x1 - x + 1;
  int h = y1 - y + 1;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_cs, LOW);
  uint32_t casCoord = (x << 16) + (x + w - 1);
  uint32_t pasCoord = (y << 16) + (y + h - 1);
  digitalWrite(_dc, LOW);
  SPI.write(GFX4d_CASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(casCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_PASET);
  digitalWrite(_dc, HIGH);    
  SPI.write32(pasCoord);
  digitalWrite(_dc, LOW);    
  SPI.write(GFX4d_RAMWR);
  digitalWrite(_dc, HIGH);
  int32_t tpwh = w * h;
  //WrGRAM16232(color, tpwh);
  if(tpwh < 2){
  SPI.write16(color);
  } else {
  while(tpwh > 0){
  SPI.write32((color << 16) | color);
  tpwh -= 2;
  }
  if(tpwh < 0) SPI.write16(color);
  }
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void GFX4d::Panel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorp) {
  RectangleFilled(x + 2, y + 2, (x + 2) + (w - 3) - 1, (y + 2) + (h - 3) - 1, colorp);
  uint32_t tcol = bevelColor(colorp);
  uint16_t _dark = tcol >> 16;
  uint16_t _light = tcol & 0xffff;
  Hline(x, y, w, _light);
  Hline(x + 1, y + 1, w - 2, _light);
  Vline(x, y, h, _light);
  Vline(x + 1, y + 1, h - 2, _light);
  Hline(x, y + h - 1, w, _dark);
  Hline(x + 1, y + h - 2, w - 2, _dark);
  Vline(x + w - 1, y, h, _dark);
  Vline(x + w - 2, y + 1, h - 2, _dark);
}

void GFX4d::PanelRecessed(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorpr) {
  RectangleFilled(x, y, x + w - 1, y + h - 1 , colorpr);
  uint32_t tcol = bevelColor(colorpr);
  uint16_t _dark = tcol >> 16;
  uint16_t _light = tcol & 0xffff;
  Hline(x + 2, y + 2, w -4, _dark);
  Vline(x + 2, y + 2, h - 4, _dark);
  Hline(x + 3, y + h - 3, w - 5, _light);
  Vline(x + w - 3, y + 3, h - 5, _light);
}

void GFX4d::Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorb, String btext, int8_t tfont, uint16_t tcolor) {
  boolean nlbckp = nl;
  nl = false;
  bactive[hndl] = true;
  bposx[hndl] = x;
  bposy[hndl] = y;
  bposw[hndl] = w;
  bposh[hndl] = h;
  bposc[hndl] = colorb;
  drawButton(0, x, y, w, h, colorb, btext, tfont, 1, 1, tcolor);
}

void GFX4d::Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb, uint16_t tcolor, int8_t tfont, int8_t tfontsizeh, int8_t tfontsize, String btext) {
  uint8_t sl = btext.length();
  uint8_t fsww;
  uint8_t fshh;
  if(tfont == 1){
  fsww = 6;
  fshh = 8;
  } else {
  fsww = 8;
  fshh = 16;
  }
  uint16_t sw = sl * fsww * tfontsize;
  uint16_t sh = fshh * tfontsizeh;
  drawButton(state, x, y, sw + (19 * tfontsize), sh + (9 * tfontsizeh), colorb, btext, tfont, tfontsize, tfontsizeh, tcolor);
}

void GFX4d::Slider(uint8_t state, int16_t x, int16_t y, int16_t r, int16_t b, uint16_t colorb, uint16_t colort, int16_t scale, int16_t value){
  int w = r - x;
  int h = b - y;
  drawButton(state, x, y, w, h, colorb, "", 1, 1, 1, colorb);
  uint16_t thw;
  uint16_t thh = h - 4;
  uint16_t ra = w - 4;
  thw = ra / 10;
  if(thw < 5){
  thw = 5;
  }
  float rs = (((float)ra - (float)thw) / (float)scale) * (float)value;
  int rsc = (int)rs;
  RectangleFilled(x + 2 + rsc, y + 2, x + 2 + rsc + thw - 1, y + 2 + thh - 1, colort);
  Hline(x + 2 + rsc, y + 2, thw, tlight);
  Vline(x + 2 + rsc, y + 2, thh, tlight);
  Hline(x + 2 + rsc, y + 1 + thh, thw, tdark);
  Vline(x + 2 + rsc + thw, y + 2, thh, tdark);
}

void GFX4d::drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorb, String btext, int8_t tfont, int8_t tfontsize, int8_t tfontsizeht, uint16_t tcolor) {
  boolean twrap = wrap;
  boolean nlbckp = nl;
  nl = false;
  wrap = false;
  int8_t fnobckup = fno;
  int8_t tfw;
  int8_t tfh;
  if(tfont < 2){
  tfw = 6;
  tfh = 8;
  } else {
  tfw = 9;
  tfh = 16;
  }
  uint8_t fsizebckup = textsize;
  uint8_t fsizehbckup = textsizeht;
  textsize = tfontsize;
  textsizeht = tfontsizeht;
  uint16_t tcolorbckup = textcolor;
  uint16_t tcolorbgbckup = textbgcolor;
  uint16_t curxbckup = cursor_x;
  uint16_t curybckup = cursor_y;
  RectangleFilled(x + 2, y + 2, (x + 2) + w - 4, (y + 2) + h - 4, colorb);
  uint32_t tcol = bevelColor(colorb);
  uint16_t _dark = tcol >> 16;
  uint16_t _light = tcol & 0xffff;
  tdark = _dark;
  tlight = _light;
  if(updn == 0){
  Hline(x, y, w, _light);
  Hline(x + 1, y + 1, w - 2, _light);
  Vline(x, y, h, _light);
  Vline(x + 1, y + 1, h - 2, _light);
  Hline(x, y + h - 1, w, _dark);
  Hline(x + 1, y + h - 2, w - 2, _dark);
  Vline(x + w - 1, y, h, _dark);
  Vline(x + w - 2, y + 1, h - 2, _dark);
  }
  if(updn == 1){
  Hline(x, y, w, _dark);
  Hline(x + 1, y + 1, w - 2, _dark);
  Vline(x, y, h, _dark);
  Vline(x + 1, y + 1, h - 2, _dark);
  Hline(x, y + h - 1, w, _light);
  Hline(x + 1, y + h - 2, w - 2, _light);
  Vline(x + w - 1, y, h, _light);
  Vline(x + w - 2, y + 1, h - 2, _light);
  }
  TextColor(tcolor, tcolor);
  Font(tfont);
  uint8_t blen = btext.length();
  if(blen > 0){
  MoveTo(((x + (w / 2) - ((blen * tfw * textsize) / 2)) + updn), ((y + (h / 2) - ((tfh * textsizeht)/ 2)) + 1 + updn));
  print(btext);
  }
  TextColor(tcolorbckup, tcolorbgbckup);
  Font(fnobckup);
  textsize = fsizebckup;
  textsizeht = fsizehbckup;
  MoveTo(curxbckup, curybckup);
  nl = nlbckp;
  wrap = twrap;
}

uint16_t GFX4d::getScrolledY(uint16_t y){
  uint16_t SY = y; 
  if(rotation == 2 || rotation == 3){
  if(rotation == 2 && scrolled == true){
  SY = 320 - scrollOffset + y;
  if(SY > 319){
  SY = SY - 320;
  }
  }
  if(rotation == 3 && scrolled == true){ 
  SY = scrollOffset + y;
  if(SY > 319){
  SY = SY - 320;
  }
  }
  }
  return SY;
}

uint32_t GFX4d::bevelColor(uint16_t colorb){
  return HighlightColors(colorb, 18);
}

uint32_t GFX4d::HighlightColors(uint16_t colorh, int step){
  c565toRGBs(colorh);
  RGB2HLS();
  uint8_t oldred = GFX4d_RED;
  uint8_t oldgreen = GFX4d_GREEN;
  uint8_t oldblue = GFX4d_BLUE;
  uint8_t tl = l;
  uint8_t th = h;
  uint8_t ts = s;
  HLS2RGB(th, tl - step, ts);
  if(GFX4d_RED > oldred){
  GFX4d_RED = 0;
  }
  if(GFX4d_GREEN > oldgreen){
  GFX4d_GREEN = 0;
  }
  if(GFX4d_BLUE > oldblue){
  GFX4d_BLUE = 0;
  }
  uint16_t _dark = RGBs2COL(GFX4d_RED, GFX4d_GREEN, GFX4d_BLUE);
  HLS2RGB(th, tl + step, ts);
  uint16_t _light = RGBs2COL(GFX4d_RED, GFX4d_GREEN, GFX4d_BLUE);
  uint32_t bevcol = (_dark << 16) + _light;
  return bevcol;
}

void GFX4d::ButtonDown(int hndl) {
  uint16_t x = bposx[hndl];
  uint16_t y = bposy[hndl];
  uint16_t w = bposw[hndl];
  uint16_t h = bposh[hndl];
  uint16_t colorbd = bposc[hndl];
  uint32_t tcol = bevelColor(colorbd);
  uint16_t _dark = tcol >> 16;
  uint16_t _light = tcol & 0xffff;
  Hline(x, y, w, colorbd);
  Hline(x + 1, y + 1, w - 2, colorbd);
  Vline(x, y, h, colorbd);
  Vline(x + 1, y + 1, h - 2, colorbd);
  Hline(x, y + h - 1, w, _dark);
  Hline(x + 1, y + h - 2, w - 2, colorbd);
  Vline(x + w - 1, y, h, _dark);
  Vline(x + w - 2, y + 1, h - 2, colorbd);
}

void GFX4d::ButtonUp(int hndl) {
  if(bactive[hndl]){
  uint16_t x = bposx[hndl];
  uint16_t y = bposy[hndl];
  uint16_t w = bposw[hndl];
  uint16_t h = bposh[hndl];
  uint16_t colorbu = bposc[hndl];
  uint32_t tcol = bevelColor(colorbu);
  uint16_t _dark = tcol >> 16;
  uint16_t _light = tcol & 0xffff;
  Hline(x, y, w, _light);
  Hline(x + 1, y + 1, w - 2, _light);
  Vline(x, y, h, _light);
  Vline(x + 1, y + 1, h - 2, _light);
  Hline(x, y + h - 1, w, _dark);
  Hline(x + 1, y + h - 2, w - 2, _dark);
  Vline(x + w - 1, y, h, _dark);
  Vline(x + w - 2, y + 1, h - 2, _dark);
  }
}

void GFX4d::ButtonActive(uint8_t butno, boolean act){
  bactive[butno] = act;
}

void GFX4d::DeleteButton(int hndl){
  if(hndl > 0){
  RectangleFilled(bposx[hndl], bposy[hndl], bposx[hndl] + bposw[hndl] - 1, bposy[hndl] + bposh[hndl] - 1, bposc[hndl]);
  bactive[hndl] = false;
  } else {
  for(int n = 0; n > 128; n++){
  RectangleFilled(bposx[n], bposy[n], bposx[n] + bposw[n] - 1, bposy[n] + bposh[n] - 1, bposc[n]);
  bactive[n] = false;
  }
  }
}

void GFX4d::DeleteButton(int hndl, uint16_t color){
  if(hndl > 0){
  RectangleFilled(bposx[hndl], bposy[hndl], bposx[hndl] + bposw[hndl] - 1, bposy[hndl] + bposh[hndl] - 1, color);
  bactive[hndl] = false;
  } else {
  for(int n = 0; n > 128; n++){
  RectangleFilled(bposx[n], bposy[n], bposx[n] + bposw[n] - 1, bposy[n] + bposh[n] - 1, color);
  bactive[n] = false;
  }
  }
}

void GFX4d::Orbit(int angle, int lngth, int *oxy){
  float sx = cos((angle - 90) * 0.0174532925);
  float sy = sin((angle - 90) * 0.0174532925);
  oxy[0] = (int)(sx * lngth + cursor_x);
  oxy[1] = (int)(sy * lngth + cursor_y);
}

void GFX4d::DeleteButtonBG(int hndl, int objBG){
  if(hndl > 0){
  UserImageDR(objBG, bposx[hndl], bposy[hndl], bposw[hndl], bposh[hndl], bposx[hndl], bposy[hndl]);
  bactive[hndl] = false;
  } else {
  for(int n = 0; n > 128; n++){
  UserImageDR(objBG, bposx[n], bposy[n], bposw[n], bposh[n], bposx[n], bposy[n]);
  bactive[n] = false;
  }
  }
}

void GFX4d::UserImageHide(int hndl){
  UserImageHide(hndl, BLACK);
}

void GFX4d::UserImageHide(int hndl, uint16_t color){
  if(hndl > 0){
  RectangleFilled(tuix[hndl], tuiy[hndl], tuiw[hndl], tuih[hndl], color);
  imageTouchEnable(hndl, false);
  } else {
  for(int n = 0; n > MAX_WIDGETS; n++){
  RectangleFilled(tuix[n], tuiy[n], tuiw[n], tuih[n], color);
  imageTouchEnable(n, false);
  }
  }
}

void GFX4d::UserImageHideBG(int hndl, int objBG){
  if(hndl > 0){
  UserImageDR(objBG, tuix[hndl], tuiy[hndl], tuiw[hndl], tuih[hndl], tuix[hndl], tuiy[hndl]);
  imageTouchEnable(hndl, false);
  } else {
  for(int n = 0; n > MAX_WIDGETS; n++){
  UserImageDR(objBG, tuix[n], tuiy[n], tuiw[n], tuih[n], tuix[n], tuiy[n]);
  imageTouchEnable(n, false);
  }
  }
}

uint8_t GFX4d::CheckButtons(void){
  butchnge = true;
  touch_Update();
  int ret = 255;
  uint8_t tpen = touch_GetPen();
  boolean skip = false;
  uint16_t tx = touch_GetX();
  uint16_t ty = touch_GetY();
  for(int n = 0; n < 128; n++){
  if(bstat[n] == 1 && (tpen == NOTOUCH || tpen == TOUCH_RELEASED)){
  bstat[n] = 0;
  ButtonUp(n);
  skip = true;
  } 
  if(bactive[n] && tpen == TOUCH_PRESSED && skip == false){
  if(tx > bposx[n] && tx < (bposx[n] + bposw[n]) && ty > bposy[n] && ty < (bposy[n] + bposh[n])){
  if(bstat[n] != 1){
  bstat[n] = 1;
  ButtonDown(n);
  oldbut = n;
  ret = n;
  }
  }
  }
  }
  oldtpen = tpen;
  return ret;
  butchnge = false;
}

uint16_t GFX4d::RGBto565(uint8_t rc, uint8_t gc, uint8_t bc) {
  return (((rc & 0xF8) << 8) | ((gc & 0xFC) << 3) | (bc >> 3));
}

void GFX4d::Orientation(uint8_t m) {
  SPI.beginTransaction(spiSettingsD);  
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.write(0x36);
  digitalWrite(_cs, HIGH);
  rotation = m % 4;
  switch (rotation) {
  case 2:
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  SPI.write(0x48);
  digitalWrite(_cs, HIGH);   
  width  = GFX4d_TFTWIDTH;
  height = GFX4d_TFTHEIGHT;
  rotation = 2;
  break;
  case 1:
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  SPI.write(0x28);
  digitalWrite(_cs, HIGH); 
  width  = GFX4d_TFTHEIGHT;
  height = GFX4d_TFTWIDTH;
  rotation = 1;
  break;
  case 3:
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  SPI.write(0x88);
  digitalWrite(_cs, HIGH); 
  width  = GFX4d_TFTWIDTH;
  height = GFX4d_TFTHEIGHT;
  rotation = 3;     
  break;
  case 0:
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  SPI.write(0xE8);
  digitalWrite(_cs, HIGH); 
  width  = GFX4d_TFTHEIGHT;
  height = GFX4d_TFTWIDTH;
  rotation = 0;
  break;
  }
  SPI.endTransaction();
}

void GFX4d::Invert(boolean i) {
  SPI.beginTransaction(spiSettingsD);  
  if(i){
  SetCommand(GFX4d_INVON);
  } else {
  SetCommand(GFX4d_INVOFF);
  }
  SPI.endTransaction();  
}

uint8_t GFX4d::ReadCommand(uint8_t c, uint8_t index) {
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_cs, LOW);
  digitalWrite(_dc, LOW);
  SPI.write(0xD9);
  digitalWrite(_dc, HIGH);
  SPI.write(0x10 + index);
  digitalWrite(_dc, LOW);
  SPI.write(c);
  digitalWrite(_dc, HIGH);
  uint8_t r = SPI.transfer(0x00);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();  
  return r;
}
 
void GFX4d::SetCommand(uint8_t c) {
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW); 
  SPI.write(c);
  digitalWrite(_cs, HIGH);
}

void GFX4d::SetData(uint8_t c) {
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  SPI.write(c);
  digitalWrite(_cs, HIGH);
} 

void GFX4d::SetData(uint8_t * data, uint8_t size) {
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  SPI.writeBytes(data, size);
  digitalWrite(_cs, HIGH);
}

void GFX4d::touch_Set(uint8_t mode) {
  uint8_t dat;
  #ifndef ESP32
  SPI.beginTransaction(spiSettingsT);
  #else
  SPI.beginTransaction(spiSettingsT32);
  #endif
  if(mode == TOUCH_DISABLE){
  tchen = false;
  digitalWrite(_tcs, LOW);
  delayMicroseconds(50);
  SPI.write(0x55);
  delayMicroseconds(50);
  SPI.write(0x01);
  delayMicroseconds(50);  
  SPI.write(DISABLE_TOUCH);
  delayMicroseconds(50); 
  for(int s = 0; s < 4; s++){
  dat = SPI.transfer(0);
  delayMicroseconds(50);
  }
  digitalWrite(_tcs, HIGH);
  }
  if(mode == TOUCH_ENABLE){
  tchen = true;
  digitalWrite(_tcs, LOW);
  SPI.write(0x55);
  delayMicroseconds(50);
  SPI.write(0x01);
  delayMicroseconds(50);
  SPI.write(ENABLE_TOUCH);
  delayMicroseconds(50); 
  for(int s = 0; s < 4; s++){
  dat = SPI.transfer(0);
  delayMicroseconds(50);
  }
  digitalWrite(_tcs, HIGH);
  }
  SPI.endTransaction();
}

boolean GFX4d::touch_Update() {
  if(millis() >= touchTime && (millis() - touchTime) < 18) return false;
  if(tchen == false) return false;
  gciobjtouched = 255;
  boolean tsskip = false;
  uint8_t dattouch[5];
  uint8_t pen = 0;
  int sum = 0;
  uint16_t tx;
  uint16_t ty;
  uint16_t TestNoNewData = 0;  
  boolean NewData = true;
  #ifndef ESP32
  SPI.beginTransaction(spiSettingsT);
  #else
  SPI.beginTransaction(spiSettingsT32); 
  #endif
  #ifndef ESP32
  delayMicroseconds(50);
  #endif
  digitalWrite(_tcs, LOW);
  #ifndef ESP32
  delayMicroseconds(50);
  #endif
  for(int s = 0; s < 5; s++){
  dattouch[s] = SPI.transfer(0x00);
  TestNoNewData = TestNoNewData + dattouch[s];
  delayMicroseconds(50);   
  }
  SPI.endTransaction();
  digitalWrite(_tcs, HIGH);
  for(int n = 0; n < 13; n ++){
  tchbuf[n] = tchbuf[n + 1];
  sum = sum + tchbuf[n];
  }
  tchbuf[13] = dattouch[0];
  sum = sum + tchbuf[13];
  if(sum == 1078 && lastpen == 129) dattouch[0] = 128;
  pen = dattouch[0] & 0x81;
  if(gPEN == TOUCH_RELEASED){
  pen = 128;
  dattouch[0] = 0x80;
  }
  if(dattouch[0] == 77){
  NewData = false;
  if(butchnge){
  gPEN = oldgPEN;
  }
  gTX = oldgTX;
  gTY = oldgTY;
  } else {
  gTX = (dattouch[2] << 7) + (dattouch[1] & 0x7F);
  gTY = (dattouch[4] << 7) + (dattouch[3] & 0x7F); 
  int tstgPEN = dattouch[0];
  if(dattouch[0] > 129 || dattouch[0] < 128) pen = 128; 
  if(pen == 129){
  gPEN = TOUCH_PRESSED;
  tsskip = true;
  lastpen = 129;
  }
  if(lastpen == 129 && pen != 129 && tsskip == false){
  gPEN = TOUCH_RELEASED;
  tsskip = true;
  lastpen = 128;
  }
  if((pen == 128 || pen == 1) && tsskip == false){
  gPEN = NOTOUCH;
  lastpen = 128;
  }
  switch (rotation) {
  case 2:
  gTX = map(gTX, 260, 3820, 0, 240);
  gTY = map(gTY, 400, 3930, 320, 0);
  break;
  case 1:
  tx = gTY;
  ty = gTX;
  gTY = map(ty, 260, 3820, 240, 0);
  gTX = map(tx, 400, 3930, 320, 0);
  break;
  case 3:
  gTX = map(gTX, 240, 3820, 240, 0);
  gTY = map(gTY, 400, 3930, 0, 320);
  break;
  case 0:
  tx = gTY;
  ty = gTX;
  gTY = map(ty, 260, 3820, 0, 240);
  gTX = map(tx, 400, 3930, 0, 320);
  break;
  }
  }
  gciobjtouched = 255;
  if(gciobjnum > 0 && gPEN == TOUCH_PRESSED && NewData){
  for(int n = 0; n < gciobjnum; n++){
  if(gciobjtouchenable[n] == true){
  if(gTX >= tuix[n] && gTX <= (tuix[n] + tuiw[n] -1) && gTY >= tuiy[n] && gTY <= (tuiy[n] + tuih[n] - 1)){
  gciobjtouched = n;
  break; 
  }
  } 
  }
  }
  oldgPEN = gPEN;
  oldgTX = gTX;
  oldgTY = gTY;
  
  touchTime = millis();
  return NewData;
}

void GFX4d::imageTouchEnable(int gcinum, boolean en){
  if(gcinum < 0){
  for(int n = 0; n < MAX_WIDGETS; n++){
  gciobjtouchenable[n] = en;
  }
  } else {
  gciobjtouchenable[gcinum] = en;
  }
}

uint16_t GFX4d::imageTouched(){
  return gciobjtouched;
}

uint16_t GFX4d::touch_GetX(){
  return gTX;
}

uint16_t GFX4d::touch_GetY(){
  return gTY;
}

uint8_t GFX4d::touch_GetPen(){
  return gPEN;
}

void GFX4d::BacklightOn(boolean bl) {
  if(bl){  
  #ifndef ESP32
  analogWrite(0, 1023);
  #else
  ledcWrite(1, 1023);
  #endif
  } else {
  #ifndef ESP32
  analogWrite(0, 0);
  #else
  ledcWrite(1, 0);
  #endif
  }
}

int16_t GFX4d::getHeight(void) {
  return height;
}

int16_t GFX4d::getWidth(void){
  return width;
}

int16_t GFX4d::getX(void){
  return cursor_x;
}

int16_t GFX4d::getY(void){
  return cursor_y;
}

int8_t GFX4d::Orientation(void){
  return rotation;
}

int16_t GFX4d::getScrollOffset(void){
  return scrollOffset;
}

uint16_t GFX4d::read16(void) {
  uint16_t result;
  result = (dataFile.read() << 8) + dataFile.read();
  return result;
}

uint32_t GFX4d::read32(void) {
  uint32_t result;
  result = (dataFile.read() << 24) + (dataFile.read() << 16);
  result = result + (dataFile.read() << 8) + dataFile.read();
return result;
}

void GFX4d::UserCharacter(uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, uint16_t bgcolor) {
  uint8_t top = 0; uint8_t left = 0;
  uint32_t tdw; uint32_t pix1; uint32_t pix2;
  int mx; int my;
  tdw = *data++;
  uint8_t ucwidth = tdw;
  tdw = *data++;
  uint8_t ucheight = tdw;
  uint16_t ucloop = (ucwidth * ucheight) >> 1;
  uint32_t test2 = 0;
  tdw = *data++;
  if(ucx > -1 && ucy > -1 && (ucx + ucwidth - 1) < (width) && (ucy + ucheight - 1) < (height)){
  setGRAM(ucx, ucy, ucx + ucwidth - 1, ucy + ucheight - 1);
  for(int c = 0; c < ucloop; c++){
  test2 = (tdw >> (ucwidth - 1) - left) & 0x1;
  pix1 = bgcolor;
  if(test2 == 1) pix1 = color;
  left++;
  test2 = (tdw >> (ucwidth - 1) - left) & 0x1;
  pix2 = bgcolor;
  if(test2 == 1) pix2 = color;
  left++;
  if(left > (ucwidth - 1)){
  left = 0;
  tdw = *data++;
  }
  WrGRAM((pix1 << 16) + pix2);
  }
  } else {
  for(int c = 0; c < ucloop; c++){
  mx = ucx + left; my = ucy + top;
  test2 = (tdw >> (ucwidth - 1) - left) & 0x1;
  if(mx > -1 && my > -1 && mx < width && my < height){
  if(test2 == 1){
  PutPixel(mx, my, color);
  } else {
  PutPixel(mx, my, bgcolor);
  }
  }
  left++;
  mx = ucx + left;
  test2 = (tdw >> (ucwidth - 1) - left) & 0x1;
  if(mx > -1 && my > -1 && mx < width && my < height){
  if(test2 == 1){
  PutPixel(mx, my, color);
  } else {
  PutPixel(mx, my, bgcolor);
  }
  }
  left++;
  if(left > (ucwidth - 1)){
  left = 0;
  top++;
  tdw = *data++;
  }
  }
  }
}

void GFX4d::UserImageDR(uint16_t ui, int16_t uxpos, int16_t uypos, int16_t uwidth, int16_t uheight, int16_t uix, int16_t uiy){
  if(uxpos >= width || uypos >= height || uxpos < 0 || uypos < 0) return;
  if((uxpos + uwidth - 1) < 0 || (uypos + uheight - 1) < 0) return;
  if((uxpos + uwidth - 1) > width || (uypos + uheight - 1) > height) return;
  boolean setemp = sEnable;
  ScrollEnable(false);
  int gciapos = (ui) * 13;
  uint32_t bgoff;
  if(userImag){
  String resultFO = "Success. ";
  } else {
  return;
  }
  userImag.seek(tuiIndex[ui] + 4);
  if(uxpos + uwidth > tuiw[ui]){
  uwidth = tuiw[ui] - uxpos;
  }
  if(uypos + uheight > tuih[ui]){
  uheight = tuih[ui] - uypos;
  }
  if(uwidth < 2 || uheight < 2){
  return;
  }
  byte mul = 2;
  uint32_t isize = tuiw[ui] * tuih[ui];
  uint32_t isize2 = uwidth * uheight;
  uint32_t uoff;
  if(cdv[ui] == 8)  mul = 1;
  uoff = ((uypos * tuiw[ui]) + uxpos) << (mul - 1);
  bgoff = tuiIndex[ui] + 6 + uoff + 0;
  userImag.seek(bgoff);
  uint32_t ichunk = isize2 << (mul - 1);
  uint16_t steps = ichunk / (uwidth << (mul - 1));
  int bufsize = uwidth << (mul - 1);
  uint8_t buf[bufsize];
  setGRAM(uix, uiy, uix + uwidth  -1, uiy + uheight -1);
  for(int n = 0; n < steps; n++){
  userImag.read(buf, bufsize);
  #ifndef ESP32
  WrGRAMs8(buf, bufsize, mul);
  #else
  uint16_t buf16[bufsize >> 1];
  if(mul == 2){
  for(int b = 0; b < bufsize; b += 2){
  buf16[b >> 1] = (buf[b] << 8) | buf[b + 1]; 
  }
  WrGRAMs16(buf16, bufsize);
  } else {
  WrGRAMs8(buf, bufsize, mul);
  }
  #endif
  bgoff = bgoff + (tuiw[ui] << (mul - 1));
  userImag.seek(bgoff);
  }
}

void GFX4d::UserImagesDR(uint16_t uino, int frames, int16_t uxpos, int16_t uypos, int16_t uwidth, int16_t uheight){
 if(uxpos >= width || uypos >= height || uxpos < 0 || uypos < 0) return;
  if((uxpos + uwidth - 1) < 0 || (uypos + uheight - 1) < 0) return;
  if((uxpos + uwidth - 1) > width || (uypos + uheight - 1) > height) return;
  uint32_t bgoff;
  boolean setemp = sEnable;
  ScrollEnable(false);
  if(uxpos + uwidth > tuiw[uino]){
  uwidth = tuiw[uino] - uxpos;
  }
  if(uypos + uheight > tuih[uino]){
  uheight = tuih[uino] - uypos;
  }
  if(uwidth < 2 || uheight < 2){
  return;
  }
  if(!userImag){
  return;
  }
  byte mul = 2;
  uint32_t isize = tuiw[uino] * tuih[uino];
  uint32_t isize2 = uwidth * uheight;
  uint32_t pos;
  uint32_t uoff;
  if(cdv[uino] == 8)  mul = 1;
  pos = (isize * frames) << (mul - 1);
  uoff = ((uypos * tuiw[uino]) + uxpos) << (mul - 1);
  bgoff = tuiIndex[uino] + 8 + pos + uoff + 0;
  userImag.seek(bgoff);
  uint32_t ichunk = isize2 << (mul - 1);
  uint16_t steps = ichunk / (uwidth << (mul - 1));
  int bufsize = uwidth << (mul - 1);
  uint8_t buf[bufsize];
  if(frames > (gciobjframes[uino]- 1) || frames < 0){
  outofrange(tuix[uino] + uxpos, tuiy[uino] + uypos, uwidth, uheight);
  ScrollEnable(setemp);
  return;
  }
  setGRAM(tuix[uino] + uxpos, tuiy[uino] + uypos, tuix[uino] + uxpos + uwidth  -1, tuiy[uino] + uypos + uheight -1);
  #ifndef ESP8266
  uint16_t pc = 0;
  #endif
  for(int n = 0; n < steps; n++){
  userImag.read(buf, bufsize);
  #ifndef ESP32
  WrGRAMs8(buf, bufsize, mul);
  #else
  if(mul == 2){
  for(int b = 0; b < bufsize; b += 2){
  buf16[pc] = (buf[b] << 8) | buf[b + 1];
  pc ++;
  if(pc == 12000){
  WrGRAMs16(buf16, pc << 1);
  pc = 0;
  } 
  }
  } else {
  WrGRAMs8(buf, bufsize, mul);
  }
  #endif
  bgoff = bgoff + (tuiw[uino] << (mul - 1));
  userImag.seek(bgoff);
  }
  #ifndef ESP8266
  if(mul == 2 && pc > 0) WrGRAMs16(buf16, pc << 1);
  #endif
  ScrollEnable(setemp);
} 

void GFX4d::UserCharacterBG(uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, boolean draw, uint32_t bgindex) {
  UserCharacterBG(data, ucsize, ucx, ucy, color, draw, bgindex, true, 0);
}

void GFX4d::UserCharacterBG(int8_t ui, uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, boolean draw) {
  UserCharacterBG(data, ucsize, ucx, ucy, color, draw, tuiIndex[ui], false, ui);
}

void GFX4d::UserCharacterBG(uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, boolean draw, uint32_t bgindex, bool type, int8_t ui) {
  if((!dataFile && type) || (!userImag && !type)){
  return;
  }
  if(ucx < 0 || ucy < 0) return;
  uint16_t bwidth;
  uint16_t bheight;
  if(type){
  dataFile.seek(bgindex);
  bwidth = (dataFile.read() << 8) + dataFile.read();
  bheight = (dataFile.read() << 8) + dataFile.read();
  } else {
  bwidth = tuiw[ui];
  bheight = tuih[ui];
  }
  uint32_t bgoff = bgindex + 6 + (((ucy * bwidth) + ucx) << 1);
  uint8_t top = 0;
  uint8_t left = 0;
  uint32_t tdw;
  uint32_t pix1;
  uint32_t pix2;
  tdw = *data++;
  uint8_t ucwidth = tdw;
  tdw = *data++;
  uint8_t ucheight = tdw;
  if((ucx + ucwidth - 1) > (width - 1) || (ucy + ucheight - 1) > (height - 1)) return; 
  uint16_t ucloop = (ucwidth * ucheight) >> 1;
  setGRAM(ucx, ucy, ucx + ucwidth - 1, ucy + ucheight - 1);
  uint32_t test = 2 ^ (ucwidth - 1);
  uint32_t test2 = 0;
  uint32_t bgbuf[ucloop];
  tdw = *data++;
  for(int c = 0; c < ucloop; c++){
  test2 = (tdw >> (ucwidth - 1) - left) & 0x1;
  if(test2 == 1 && draw){
  pix1 = color;
  } else {
  if(type){
  dataFile.seek(bgoff + (left << 1));
  pix1 = (dataFile.read() << 8) + dataFile.read();
  } else {
  userImag.seek(bgoff + (left << 1));
  pix1 = (userImag.read() << 8) + userImag.read();
  }
  }
  left++;
  test2 = (tdw >> (ucwidth - 1) - left) & 0x1;
  if(test2 == 1 && draw){
  pix2 = color;
  } else {
  if(type){
  dataFile.seek(bgoff + (left << 1));
  pix2 = (dataFile.read() << 8) + dataFile.read();
  } else {
  userImag.seek(bgoff + (left << 1));
  pix2 = (userImag.read() << 8) + userImag.read();
  }
  }
  left++;
  if(left > (ucwidth - 1)){
  left = 0;
  bgoff = bgoff + (bwidth << 1);
  tdw = *data++;
  uint32_t test = 2 ^ (ucwidth - 1);
  }
  bgbuf[c] = (pix1 << 16) + pix2;
  }
  WrGRAMs(bgbuf, ucloop);
}

void GFX4d::DownloadFile(String WebAddr, String Fname){   
  Download(WebAddr, 0, "", Fname, "");
}

void GFX4d::DownloadFile(String WebAddr, String Fname, String sha1){   
  Download(WebAddr, 0, "", Fname, sha1);
}

void GFX4d::DownloadFile(String Address, uint16_t port, String hfile, String Fname){   
  Download(Address, port, hfile, Fname, "");
}

boolean GFX4d::CheckSD(void){
  return sdok;
}

boolean GFX4d::CheckDL(void){
  return dlok;
}

void GFX4d::Download(String Address, uint16_t port, String hfile, String Fname, String sha1){   
  dlok = false;
  int cstream;
  #ifndef USE_FS
  File Dwnload;
  #else
  fs::File Dwnload;
  #endif
  HTTPClient http;
  if(port > 0){
  http.begin(Address,port,hfile);
  } else {
  if(sha1 == ""){
  http.begin(Address);
  } else {
  #ifndef ESP32
  http.begin(Address, sha1);
  #endif
  }
  }
  int httpCode = http.GET();
  if(httpCode == 404){
  println("File Error");
  return;
  }
  if(sdok == false){
  return;
  }
  #ifndef USE_FS
  if(SD.exists((char*)Fname.c_str())) {
  SD.remove((char*)Fname.c_str());
  }
  Dwnload = SD.open((char*)Fname.c_str(), FILE_WRITE);
  #else
  if(SPIFFS.exists((char*)Fname.c_str())) {
  SPIFFS.remove((char*)Fname.c_str());
  }
  Dwnload = SPIFFS.open((char*)Fname.c_str(), "w");
  #endif
  int32_t lens = http.getSize();
  if(lens == 0){
  return;
  }
  uint8_t buff[128] = { 0 };
  WiFiClient * stream = http.getStreamPtr();
  while(http.connected() && (lens > 0 || lens == -1)) {
  size_t size = stream->available();
  if(size) {
  int cstream = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
  yield();
  if(!Dwnload.write(buff, cstream)){
  return;
  }
  if(lens > 0) {
  lens -= cstream;
  }
  }
  yield();
  }
  Dwnload.close();
  http.end();
  dlok = true;
}

int16_t GFX4d::XYposToDegree(int curX, int curY){
  int delta, deg, adj ;
  if(curY < 0){
  if (curX < 0){
  adj = 1 ;
  deg = 90 ;
  } else {
  adj = 2 ;
  deg = 180 ;
  }
  } else {
  if (curX < 0){
  deg = 0 ;
  adj = 2 ;
  } else {
  deg = 270 ;
  adj = 1 ;
  }
  }
  curX = abs(curX) ;
  curY = abs(curY) ;
  if (curX < curY){
  adj &= 1 ;
  } else {
  adj &= 2 ;
  swap(curX, curY) ;
  }
  delta = at[(curX * 100) / curY] ;
  if (adj){
  deg += 90 - delta ;
  } else {
  deg += delta ;
  }
  return deg ;
}

uint16_t GFX4d::RGBs2COL(uint8_t r, uint8_t g, uint8_t b){
  return (b >> 2) | (g & 0x7E) << 4 | (r & 0x7c) << 9  ;
}

void GFX4d::c565toRGBs(uint16_t i565){
  GFX4d_RED =   (i565 & 0xF800) >> 9 ;
  GFX4d_GREEN = (i565 & 0x07E0) >> 4 ;
  GFX4d_BLUE  = (i565 & 0x001F) << 2 ;
}

void GFX4d::RGB2HLS(){
  uint8_t cMax, cMin, Rdelta, Gdelta, Bdelta, cMpM, cMmM ;
  uint8_t tmaxmin;
  if(GFX4d_RED >= GFX4d_GREEN){
  cMax = GFX4d_RED;
  } else {
  cMax = GFX4d_GREEN;
  }
  if(GFX4d_BLUE >= cMax){
  cMax = GFX4d_BLUE;
  }
  if(GFX4d_RED <= GFX4d_GREEN){
  cMin = GFX4d_RED;
  } else {
  cMin = GFX4d_GREEN;
  }
  if(GFX4d_BLUE <= cMin){
  cMin = GFX4d_BLUE;
  }
  cMpM = cMax + cMin;
  cMmM = cMax - cMin;
  l = ((cMpM * HLSMAX) + RGBMAX) / RGBMAXm2;
  if (cMax == cMin){
  s = 0;
  h = UNDEFINED;
  } else {
  if (l <= (HLSMAX / 2)){
  s = ((cMmM * HLSMAX) + (cMpM / 2) ) / cMpM;
  } else {
  s = ((cMmM * HLSMAX) + ((RGBMAXm2 - cMpM) / 2)) / (RGBMAXm2 - cMpM);
  }
  Rdelta = (((cMax - GFX4d_RED) * HLSMAXd6) + (cMmM / 2)) / cMmM;
  Gdelta = (((cMax - GFX4d_GREEN) * HLSMAXd6) + (cMmM / 2)) / cMmM;
  Bdelta = (((cMax - GFX4d_BLUE) * HLSMAXd6) + (cMmM / 2)) / cMmM;
  if (GFX4d_RED == cMax){
  h = Bdelta - Gdelta ;
  } else if (GFX4d_GREEN == cMax){
  h = HLSMAXd3 + Rdelta - Bdelta ;
  } else {
  h = HLSMAXm2d3 + Gdelta - Rdelta;
  }
  if (h < 0){
  h += HLSMAX;
  }
  if (h > HLSMAX){
  h -= HLSMAX;
  }
  }
}

uint8_t GFX4d::hue_RGB(int Hin, int M1, int M2){
  uint8_t Value ;
  if (Hin < 0){
  Hin += HLSMAX ;
  } else if (Hin > HLSMAX){
  Hin -= HLSMAX ;
  }
  if (Hin < HLSMAXd6){
  Value = M1 + ( (M2 - M1) * Hin + HLSMAXd12) / HLSMAXd6 ;
  } else if (Hin < HLSMAXd2){
  Value = M2 ;
  } else if (Hin < HLSMAXm2d3){
  Value = M1 + ((M2 - M1) * (HLSMAXm2d3 - Hin) + HLSMAXd12) / HLSMAXd6 ;
  } else {
  Value = M1 ;
  }
  return Value ;
}

void GFX4d::HLS2RGB(int H, int L, int S){
  uint8_t M1, M2 ;
  if (S == 0){
  GFX4d_RED = L ;
  GFX4d_GREEN = L ;
  GFX4d_BLUE = L ;
  } else {
  if (L <= HLSMAXd2){
  M2 = (L * (HLSMAX + S) + HLSMAXd2) / HLSMAX ;
  } else {
  M2 = L + S - ((L * S + HLSMAXd2) / HLSMAX) ;
  }
  M1 = 2 * L - M2 ;
  if ((H > HLSMAX ) || (H < 0)){
  h = 0 ;
  }
  GFX4d_RED = hue_RGB(H+HLSMAXd3, M1, M2) ;
  GFX4d_GREEN = hue_RGB(H, M1, M2) ;
  GFX4d_BLUE = hue_RGB(H-HLSMAXd3, M1, M2) ;
  }
}

uint16_t GFX4d::getNumberofObjects(void){
  return gciobjnum;
}

void GFX4d::Close4dGFX(){
  userImag.close();
  gciobjnum = 0;
  imageTouchEnable(-1, false);
}

void GFX4d::WrGRAMs16232(uint16_t *data, uint16_t l) {
  uint32_t tdw;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  while(l){
  tdw = (*data++ << 16) + *data++;
  l-= 2;
  SPI.write32(tdw);
  }
  if(l < 0)SPI.write16(*data++);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
} 

void GFX4d::WrGRAMs16232NT(uint16_t *data, uint16_t l) {
  uint32_t tdw;
  while(l){
  tdw = (*data++ << 16) + *data++;
  l-= 2;
  SPI.write32(tdw);
  }
  if(l < 0)SPI.write16(*data++);
}

void GFX4d::WrGRAMend(){
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
} 

void GFX4d::WrGRAMstart(){
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
} 

void GFX4d::WrGRAM16232(uint16_t mcolor, int32_t l) {
  uint32_t tdw;
  SPI.beginTransaction(spiSettingsD);
  digitalWrite(_dc, HIGH);  
  digitalWrite(_cs, LOW);
  if(l == 1){
  SPI.write16(mcolor);
  } else {
  tdw = (mcolor << 16) | mcolor;
  while(l > 0){
  l-= 2;
  SPI.write32(tdw);
  }
  if(l < 0)SPI.write16(mcolor);
  }
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
} 

uint8_t GFX4d::getFontData(uint8_t fntnum, uint16_t val){
  if(fntnum == 1) return font1[val];
  if(fntnum == 2) return font2[val];
  if(fntnum != 1 || fntnum != 2) return 0;
}

int16_t GFX4d::ImageTouchedAuto(int UpdateImages){
  int itouched;
  bool shifted = false;
  if((shift || caps) && UpdateImages == KEYPAD) shifted = true;
  int butcntrl = 0;
  if(UpdateImages == KEYPAD){
  UpdateImages = DRAW_UPDOWN;
  butcntrl = ((shifted) *2);
  }
  int temppressed = -1;
  if(touch_Update()){
  int state = touch_GetPen();
  if(state == TOUCH_PRESSED){
  itouched = imageTouched();
  if(itouched > -1 && itouched < getNumberofObjects()){
  if(UpdateImages && itouched != pressed){
  if(pressed > -1 && pressed < getNumberofObjects() && gciobjframes[pressed] >= butcntrl) UserImages(pressed, butcntrl);
  if(gciobjframes[itouched] >= 1 + butcntrl){
  UserImages(itouched, 1 + butcntrl) ;
  } else {
  UserImages(itouched, 1) ;
  }
  }
  if(gciobjframes[itouched] >= butcntrl)pressed = itouched;
  }
  return -1;
  }
  if(state == 2 && pressed > -1 && pressed  < getNumberofObjects()){
  if(UpdateImages == DRAW_UPDOWN && gciobjframes[pressed] >= butcntrl){
  UserImages(pressed, butcntrl) ;
  } else {
  UserImages(pressed, 0) ;
  }
  temppressed = pressed;
  pressed = -1;
  return temppressed;
  }
  if(state == 0 && pressed > -1 && pressed < getNumberofObjects()){
  if(UpdateImages == DRAW_UPDOWN && gciobjframes[pressed] >= butcntrl){
  UserImages(pressed, butcntrl) ;
  } else {
  UserImages(pressed, 0) ;
  }
  temppressed = pressed;
  pressed = -1;
  return temppressed;
  }
  if(state == 0 || state == 2){
    return -1;
  }
  } else {
  return -1;
  }
}

void GFX4d::FontStyle(uint8_t ctyp){
  fstyle = ctyp % 6;
}

uint16_t GFX4d::GetSliderValue(uint16_t ui, uint8_t axis, uint16_t uiv, uint16_t ming, uint16_t maxg){
  int wpos;
  int wsiz;
  if(axis == HORIZONTAL_SLIDER){ 
  wpos = uiv - tuix[ui] - ming; //y - 53 ;                        // y - top - 8
  wsiz = tuiw[ui];
  if (wpos < 0)
  wpos = 0;                       // maxvalue-minvalue
  else if (wpos > (wsiz - maxg))                    // height - 17)
  wpos = gciobjframes[ui] - 1 ;
  else
  wpos = (gciobjframes[ui] - 1) * wpos / (wsiz - maxg) ;    // max-min - (max-min) * posn / (height-17)
  return wpos;
  }
  if(axis == VERTICAL_SLIDER){ 
  wpos = uiv - tuiy[ui] - ming;
  wsiz = tuih[ui];
  if (wpos < 0)
  wpos = gciobjframes[ui] - 1;                       // maxvalue-minvalue
  else if (wpos > (wsiz - maxg))                    // height - 17)
  wpos = 0 ;
  else
  wpos = (gciobjframes[ui] - 1) - (gciobjframes[ui] - 1) * wpos / (wsiz - maxg) ;    // max-min - (max-min) * posn / (height-17)
  return wpos;
  }
}

int GFX4d::DecodeKeypad(int kpad, int kpress, byte *kbks, int8_t *kbck){
  int key = -1;
  int kv;
  int koff;
  if (shift) koff = kbck[10];
  if (caps) koff = koff + (2 * kbck[10]);
  if (ctrl) koff = (3 * kbck[10]);
  bool skip = false;
  if(kpress > -1){
  key = kbks[kpress - kpad - 1 + koff];
  kv = (kpress - kpad - 1) % kbck[10];//if(key != 0xff) gfx.TWwrite(key);
  if(key == 0xff && !shift && (kv == kbck[5] || kv == kbck[6])){
  if(!caps)UserImages(kpad,1) ;
  if(caps)UserImages(kpad,0) ;
  shift = true;
  skip = true;
  ctrl = false;
  }
  if(key == 0xff && shift && (kv == kbck[5] || kv == kbck[6]) && !skip){
  if(!caps)UserImages(kpad,0) ;
  if(caps)UserImages(kpad,1) ;
  shift = false;
  ctrl = false;
  }
  skip = false;
  if(key == 0xff && kv == kbck[9] && !caps){
  UserImages(kpad,1) ;
  caps = true;
  skip = true;
  ctrl = false;
  }
  if(key == 0xff && kv == kbck[9] && caps && !skip){
  UserImages(kpad,0) ;
  caps = false;
  ctrl = false;
  }
  skip = false;
  if(key == 0xff && (kv == kbck[7] || kv == kbck[8]) && ctrl == false){
  if(!caps)UserImages(kpad,0) ;
  if(caps)UserImages(kpad,1) ;
  ctrl = true;
  shift = false;
  skip = true;
  }
  if(key == 0xff && (kv == kbck[7] || kv == kbck[8]) && !skip){
  ctrl = false;
  }
  }
  if(key == 0xff) key = -1;
  if(key != -1 && shift){
  if(!caps)UserImages(kpad,0) ;
  if(caps)UserImages(kpad,1) ;
  shift = false;
  ctrl = false;
  }
  return key;
}

void GFX4d::ResetKeypad(){
  shift = false;
  caps = false;
  ctrl = false;
}

bool GFX4d::KeypadStatus(int keyType){
  if(keyType == SHIFT) return shift;
  if(keyType == CAPSLOCK) return caps;
  if(keyType == CTRL) return ctrl;
  return false;
}

bool GFX4d::SpriteInit(uint16_t *sdata, size_t nums) {
  if(msprites < 1) return false;
  uint16_t scount = 0;
  int sdatpos;
  int sprsize;
  uint16_t cdpth = 1;
  uint16_t nextpos = 4;
  while(scount <= nums && cdpth > 0) {
  spriteData[sdatpos] = sdata[scount];
  spriteData[sdatpos + 1] = sdata[scount + 1];
  spriteData[sdatpos + 2] = sdata[scount + 2];
  cdpth = sdata[scount + 3];
  spriteData[sdatpos + 3] = nextpos;
  sprsize = (sdata[scount + 1] * sdata[scount + 2]) >> (cdpth - 1);
  if(cdpth == SPRITE_8BIT && ((sdata[scount + 1] * sdata[scount + 2]) % 2) > 0) sprsize ++; 
  if(cdpth == SPRITE_4BIT && ((sdata[scount + 1] * sdata[scount + 2]) % 4) > 0) sprsize ++;
  nextpos = nextpos + sprsize + 4;
  sdatpos += 4;
  scount = nextpos - 4; 
  }
  yield();
  return true;
}

bool GFX4d::SpriteAdd(int pos, int snum, int x, int y, uint16_t *sdata) {
  if(snum > msprites) return false;
  int spos = spriteData[(snum << 2) + 3];
  byte coldepth = sdata[spos - 1];
  spriteList[(pos << 3)] = 0;
  spriteList[(pos << 3)] |= (coldepth << 1);
  spriteList[(pos << 3) + SPRITE_MEMPOS] = spos;
  spriteList[(pos << 3) + SPRITE_X] = x;
  spriteList[(pos << 3) + SPRITE_Y] = y;
  spriteList[(pos << 3) + SPRITE_WIDTH] = sdata[spos - 3];
  spriteList[(pos << 3) + SPRITE_HEIGHT] = sdata[spos - 2];
  spriteLast[pos << 1] = x;
  spriteLast[(pos << 1) + 1] = y;
  spriteNum[pos] = snum;
  if(pos + 1 > numSprites) numSprites = pos + 1;
  return true;
}

void GFX4d::SpriteAreaSet(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1) {
  spriteArea[0] = x;
  spriteArea[1] = y;
  spriteArea[2] = x1;
  spriteArea[3] = y1;
  saSet = true;
}

void GFX4d::SetSprite(int num, bool active, int x, int y, uint16_t bscolor, uint16_t * sdata) {
  bool delsprite = false;
  int lxy = num << 1;
  int dxy = num << 3;
  int sacoords[6];
  int rx = 0xffff;
  int ry = 0xffff;
  int dsx = spriteList[dxy + SPRITE_X];
  int dsy = spriteList[dxy + SPRITE_Y];
  if(spriteList[(num << 3) + 0] == 1 && active == 0){
  delsprite = true;
  }
  if(spriteList[dxy] == 0){
  spriteLast[lxy] = x;
  spriteLast[lxy + 1] = y;
  }  
  spriteList[dxy] |= active;
  spriteList[dxy + SPRITE_X] = x;
  spriteList[dxy + SPRITE_Y] = y;
  int16_t tsx, tsy, tsx1, tsy1;
  if(!delsprite){
  if(spriteLast[lxy] <= x){
  tsx = spriteLast[lxy];
  tsx1 = x + spriteList[dxy + SPRITE_WIDTH];
  } else {
  tsx1 = spriteLast[lxy]  + spriteList[dxy + SPRITE_WIDTH];
  tsx = x;
  }
  if(spriteLast[lxy + 1] <= y){
  tsy = spriteLast[lxy + 1];
  tsy1 = y + spriteList[dxy + SPRITE_HEIGHT];
  } else {
  tsy1 = spriteLast[lxy + 1] + spriteList[dxy + SPRITE_HEIGHT];
  tsy = y;
  }
  } else {
  tsx = dsx;
  tsy = dsy;
  tsx1 = dsx + spriteList[dxy + SPRITE_WIDTH];
  tsy1 = dsy + spriteList[dxy + SPRITE_HEIGHT];
  }
  spriteLast[lxy] = x;
  spriteLast[lxy + 1] = y;
  if((((tsx1 - tsx) >> 1) + 1 > spriteList[dxy + SPRITE_WIDTH] || ((tsy1 - tsy) >> 1) + 1 > spriteList[dxy + SPRITE_HEIGHT]) && spriteList[dxy + SPRITE_ACTIVE]){
  spriteList[dxy + SPRITE_ACTIVE] |= 0;
  SpriteUpdate(dsx, dsy, dsx + spriteList[dxy + SPRITE_WIDTH], dsy + spriteList[dxy + SPRITE_HEIGHT], bscolor, sdata);
  spriteList[dxy + SPRITE_ACTIVE] |= 1;
  SpriteUpdate(x, y, x + spriteList[dxy + SPRITE_WIDTH], y + spriteList[dxy + SPRITE_HEIGHT], bscolor, sdata);
  } else {
  SpriteUpdate(tsx, tsy, tsx1, tsy1, bscolor, sdata);
  }  
}

void GFX4d::SpriteUpdate(int16_t tsx, int16_t tsy, int16_t tsx1, int16_t tsy1, uint16_t bscolor, uint16_t * sdata){
  saSet = true;
  if(tsx >= width || tsy >= height || tsx1 < 1 || tsy1 < 1) return; 
  if(tsx < 0) tsx = 0;
  if(tsy < 0) tsy = 0;
  if(tsx1 >= width) tsx1 = width - 1;
  if(tsy1 >= height) tsy1 = height - 1;
  spriteArea[0] = tsx;
  spriteArea[1] = tsy;
  spriteArea[2] = tsx1;
  spriteArea[3] = tsy1;
  UpdateSprites(bscolor, sdata); 
}
  
int GFX4d::GetSprite(int snum, int choice){
  return spriteList[(snum << 3) + choice];  
}

void GFX4d::UpdateSprites(uint16_t bscolor, uint16_t * sdata) {
  if (!saSet) return;
  setGRAM(spriteArea[0], spriteArea[1], spriteArea[2], spriteArea[3]);  
  WrGRAMstart();
  byte tog = 0;
  byte cdepth;
  uint16_t sspos = 0;
  int collide;
  int addr;
  int xo, yo;
  uint32_t tdw;
  uint16_t wscolor;
  uint16_t cscolor;
  uint16_t tscolor;
  #ifndef ESP8266
  uint16_t bufsp[spriteArea[2] + 1];
  uint16_t count;
  #endif
  int spriteAPos;
  for (int ns = 0; ns < numSprites; ns ++){
  spriteList[(ns << 3) + 6] = -1;
  spriteList[(ns << 3) + 7] = -1;
  }
  collide = -1;
  for (int y = 0; y < spriteArea[3] - spriteArea[1] + 1; y ++) {
  #ifndef ESP8266
  count = 0;
  #endif
  for (int x = 0; x < spriteArea[2] - spriteArea[0] + 1; x ++) {
  wscolor = bscolor;
  collide = -1;
  for (int chk = 0; chk < numSprites; chk ++) {
  xo = spriteArea[0] + x;
  yo = spriteArea[1] + y;
  addr = chk << 3;
  cdepth = spriteList[addr] >> 1;
  spriteAPos = spriteList[addr + SPRITE_MEMPOS];
  sdetaily = spriteList[addr + SPRITE_Y];
  sdetailh = spriteList[addr + SPRITE_HEIGHT];
  sdetailx = spriteList[addr + SPRITE_X];
  sdetailw = spriteList[addr + SPRITE_WIDTH];
  if ((spriteList[addr] & 0x01) && yo >= sdetaily && yo <=  sdetaily + sdetailh - 1 && xo >= sdetailx && xo <= sdetailx + sdetailw - 1) {
  tscolor = sdata[spriteAPos - 4];
  sspos = ((yo - sdetaily) * sdetailw) + (xo - sdetailx);
  if(cdepth == SPRITE_16BIT) cscolor = sdata[spriteAPos + sspos];
  if(cdepth == SPRITE_8BIT){ 
  uint16_t tcscol = sdata[spriteAPos + (sspos >> 1)];
  cscolor = RGB3322565[(tcscol >> 8 * ((sspos % 2) == 0)) & 0xff];
  }
  if(cdepth == SPRITE_4BIT){ 
  uint16_t tcscol = sdata[spriteAPos + (sspos >> 2)];
  cscolor = palette4bit[(tcscol >> ((3 - (sspos % 4)) * 4)) & 0x0f];
  }
  if (cscolor != tscolor){
  wscolor = cscolor;
  if(collide == -1){
  collide = chk;
  } else if(collide != -1){
  if(spriteList[(collide << 3) + SPRITE_COLLIDE1] == -1){
  spriteList[(collide << 3) + SPRITE_COLLIDE1] = chk;
  spriteList[addr + SPRITE_COLLIDE1] = collide;
  } else {
  spriteList[(collide << 3) + SPRITE_COLLIDE2] = chk;
  spriteList[addr + SPRITE_COLLIDE1] = collide;
  }
  collide = chk;
  }              
  }
  }
  }
  #ifdef ESP8266
  if(tog == 0) tdw = wscolor << 16;
  if(tog == 1){
  SPI.write32(tdw + wscolor);
  }
  tog ^= 1;
  }
  }
  if(tog == 1) SPI.write16(tdw >> 16);
  WrGRAMend();
  yield();
  #else
  bufsp[count] = wscolor;
  count ++;
  }
  SPI.writePixels(bufsp, count * 2);
  }
  WrGRAMend();
  #endif
}

void GFX4d::SetNumberSprites(uint16_t numspr){
  numSprites = numspr;
}

int GFX4d::GetNumberSprites(){
  return numSprites;
}

int GFX4d::SpriteTouched(){
  int tresp = -1;
  int stx;
  int sty;
  if(touch_Update()){
  if(touch_GetPen() == 1){
  stx = touch_GetX();
  sty = touch_GetY();
  for(int nt = numSprites - 1; nt > -1; nt --){
  if(GetSprite(nt, SPRITE_ACTIVE) == 1){
  if(stx >= GetSprite(nt, SPRITE_X) && stx <= (GetSprite(nt, SPRITE_X) + GetSprite(nt, SPRITE_WIDTH)) && sty >= GetSprite(nt, SPRITE_Y) && sty <= (GetSprite(nt, SPRITE_Y) + GetSprite(nt, SPRITE_HEIGHT))){
  tresp = nt;
  break;
  }
  }
  }
  }
  }
  return tresp;
}

 int16_t GFX4d::GetSpriteImageNum(int snum){
   return spriteNum[snum];
 }
 
 uint16_t GFX4d::SpriteGetPixel(int snum, int xo, int yo, uint16_t tcolor, uint16_t * sdata){
   uint16_t cscolor = 0x0000;
   uint16_t rcolor = tcolor;
   uint16_t sspos;
   byte cdepth;
   int chks, chke;
   if(snum < 0){
   chks = 0; chke = numSprites;
   } else {
   chks = snum; chke = snum + 1;
   }
   for (int chk = chks; chk < chke; chk ++) {
   uint16_t addr = chk << 3;
   cdepth = spriteList[addr] >> 1;
   if (spriteList[addr] && yo >= spriteList[addr + SPRITE_Y] && yo <=  spriteList[addr + SPRITE_Y] + spriteList[addr + SPRITE_HEIGHT] - 1 && xo >= spriteList[addr + SPRITE_X] && xo <= spriteList[addr + SPRITE_X] + spriteList[addr + SPRITE_WIDTH] - 1) {
   sspos = ((yo - spriteList[addr + SPRITE_Y]) * spriteList[addr + SPRITE_WIDTH]) + (xo - spriteList[addr + SPRITE_X]);
   if(cdepth == SPRITE_16BIT) cscolor = sdata[spriteList[addr + SPRITE_MEMPOS] + sspos];
   if(cdepth == SPRITE_8BIT){ 
   uint16_t tcscol = sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 1)];
   cscolor = RGB3322565[(tcscol >> 8 * ((sspos % 2) == 0)) & 0xff];
   }
   if(cdepth == SPRITE_4BIT){ 
   uint16_t tcscol = sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 2)];
   cscolor = palette4bit[(tcscol >> ((3 - (sspos % 4)) * 4)) & 0x0f];
   }
   if(cscolor != tcolor) rcolor = cscolor;
   }
   }
   return rcolor;
}

int GFX4d::GetSpritesAt(int xo, int yo, uint16_t tcolor, int * slist, uint16_t * sdata){
   uint16_t cscolor = 0x0000;
   uint16_t sspos;
   int snum = 0;
   int r = -1;
   byte cdepth;
   for (int chk = 0; chk < numSprites; chk ++) {
   slist[chk] = -1;
   uint16_t addr = chk << 3;
   cdepth = spriteList[addr] >> 1;
   if (spriteList[addr] && yo >= spriteList[addr + SPRITE_Y] && yo <=  spriteList[addr + SPRITE_Y] + spriteList[addr + SPRITE_HEIGHT] - 1 && xo >= spriteList[addr + SPRITE_X] && xo <= spriteList[addr + SPRITE_X] + spriteList[addr + SPRITE_WIDTH] - 1) {
   sspos = ((yo - spriteList[addr + SPRITE_Y]) * spriteList[addr + SPRITE_WIDTH]) + (xo - spriteList[addr + SPRITE_X]);
   if(cdepth == SPRITE_16BIT) cscolor = sdata[spriteList[addr + SPRITE_MEMPOS] + sspos];
   if(cdepth == SPRITE_8BIT){ 
   uint16_t tcscol = sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 1)];
   cscolor = RGB3322565[(tcscol >> 8 * ((sspos % 2) == 0)) & 0xff];
   }
   if(cdepth == SPRITE_4BIT){ 
   uint16_t tcscol = sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 2)];
   cscolor = palette4bit[(tcscol >> ((3 - (sspos % 4)) * 4)) & 0x0f];
   }
   if(cscolor != tcolor){
   slist[snum] = chk;
   snum ++;
   r = snum;
   }
   }
   }
   return r;
}

void GFX4d::SpriteEnable(int snum, bool sen){
  spriteList[snum << 3] |= sen;
}

void GFX4d::SpriteSetPalette(int pnumber, uint16_t plcolor){
  palette4bit[pnumber % 16] = plcolor;
}  

uint16_t GFX4d::SpriteGetPalette(int pnumber){
  return palette4bit[pnumber % 16];
}

void GFX4d::SetMaxNumberSprites(uint16_t snos){
  snos = snos << 1;
  uint16_t bytes = (uint16_t)snos << 2; 
  if((spriteData = (int16_t *)malloc(bytes))) {
  memset(spriteData, 0, bytes);
  }
  bytes = (uint16_t)snos << 3; 
  if((spriteList = (int16_t *)malloc(bytes))) {
  memset(spriteList, 0, bytes);
  }
  bytes = (uint16_t)snos << 1; 
  if((spriteLast = (int16_t *)malloc(bytes))) {
  memset(spriteLast, 0, bytes);
  }
  bytes = (uint16_t)snos; 
  if((spriteNum = (int16_t *)malloc(bytes))) {
  memset(spriteNum, -1, bytes);
  }
  msprites = snos >> 1;
}