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

#ifndef _GFX4d_H
#define _GFX4d_H
//#define USE_FS
#include "Arduino.h"
#include "Print.h"
#ifndef USE_FS
//#include <SDFS.h>
#include <SD.h>
#else
#define FS_NO_GLOBALS //allow spiffs to coexist with SD card, define BEFORE including FS.h
#include <FS.h> //spiff file system
#endif

#define MAX_WIDGETS	       400

#ifdef USING_WS4_GCIOBJECTS
#define  GCIOBJSEN
#endif
#define  GFX4d_TFTWIDTH    240
#define  GFX4d_TFTHEIGHT   320

#define  GFX4d_NOP         0x00
#define  GFX4d_SWRESET     0x01
#define  GFX4d_RDDID       0x04
#define  GFX4d_RDDST       0x09

#define  GFX4d_SLPIN       0x10
#define  GFX4d_SLPOUT      0x11
#define  GFX4d_PTLON       0x12
#define  GFX4d_NORON       0x13

#define  GFX4d_RDMODE      0x0A
#define  GFX4d_RDMADCTL    0x0B
#define  GFX4d_RDPIXFMT    0x0C
#define  GFX4d_RDIMGFMT    0x0D
#define  GFX4d_RDSELFDIAG  0x0F

#define  GFX4d_INVOFF      0x20
#define  GFX4d_INVON       0x21
#define  GFX4d_GAMMASET    0x26
#define  GFX4d_DISPOFF     0x28
#define  GFX4d_DISPON      0x29

#define  GFX4d_CASET       0x2A
#define  GFX4d_PASET       0x2B
#define  GFX4d_RAMWR       0x2C
#define  GFX4d_RAMRD       0x2E

#define  GFX4d_PTLAR       0x30
#define  GFX4d_VSCRDEF     0x33
#define  GFX4d_MADCTL      0x36
#define  GFX4d_VSCRSADD    0x37
#define  GFX4d_PIXFMT      0x3A

#define  GFX4d_FRMCTR1     0xB1
#define  GFX4d_FRMCTR2     0xB2
#define  GFX4d_FRMCTR3     0xB3
#define  GFX4d_INVCTR      0xB4
#define  GFX4d_DFUNCTR     0xB6

#define  GFX4d_PWCTR1      0xC0
#define  GFX4d_PWCTR2      0xC1
#define  GFX4d_PWCTR3      0xC2
#define  GFX4d_PWCTR4      0xC3
#define  GFX4d_PWCTR5      0xC4
#define  GFX4d_VMCTR1      0xC5
#define  GFX4d_VMCTR2      0xC7

#define  GFX4d_RDID1       0xDA
#define  GFX4d_RDID2       0xDB
#define  GFX4d_RDID3       0xDC
#define  GFX4d_RDID4       0xDD

#define  GFX4d_GMCTRP1     0xE0
#define  GFX4d_GMCTRN1     0xE1

#define  LANDSCAPE	     0
#define  LANDSCAPE_R     1
#define  PORTRAIT	     2
#define  PORTRAIT_R	     3
#define  SOLID		     0
#define  DOTMATRIXROUND	 1
#define  DOTMATRIXLED    3
#define  DOTMATRIXSQUARE 4
#define  DOTMATRIXFADE   5
#define  SYSTEM		     1
#define  FONT1		     1
#define  FONT2		     2
#define  Pressed		 1
#define  Released	     0
#define  SLIDER_RAISED	 0
#define  SLIDER_SUNKEN	 1
#define  NOTOUCH         0
#define  TOUCH_PRESSED   1
#define  TOUCH_RELEASED  2
#define  TOUCH_STATUS    0
#define  TOUCH_GETX      1
#define  TOUCH_GETY      2
#define  KEYPAD			 3
#define  DRAW_UPDOWN     2
#define  DRAW_DOWNONLY   1
#define  DRAW_NONE       0

#define  ALICEBLUE	0xF7DF
#define  ANTIQUEWHITE	0xFF5A
#define  AQUA		0x07FF
#define  AQUAMARINE	0x7FFA
#define  AZURE		0xF7FF
#define  BEIGE		0xF7BB
#define  BISQUE		0xFF38
#define  BLACK		0x0000
#define  BLANCHEDALMOND	0xFF59
#define  BLUE		0x001F
#define  BLUEVIOLET	0x895C
#define  BROWN		0xA145
#define  BURLYWOOD	0xDDD0
#define  CADETBLUE	0x5CF4
#define  CHARTREUSE	0x7FE0
#define  CHOCOLATE	0xD343
#define  CORAL		0xFBEA
#define  CORNFLOWERBLUE	0x64BD
#define  CORNSILK	0xFFDB
#define  CRIMSON	0xD8A7
#define  CYAN		0x07FF
#define  DARKBLUE	0x0011
#define  DARKCYAN	0x0451
#define  DARKGOLDENROD	0xBC21
#define  DARKGRAY	0xAD55
#define  DARKGREEN	0x0320
#define  DARKKHAKI	0xBDAD
#define  DARKMAGENTA	0x8811
#define  DARKOLIVEGREEN	0x5345
#define  DARKORANGE	0xFC60
#define  DARKORCHID	0x9999
#define  DARKRED	0x8800
#define  DARKSALMON	0xECAF
#define  DARKSEAGREEN	0x8DF1
#define  DARKSLATEBLUE	0x49F1
#define  DARKSLATEGRAY	0x2A69
#define  DARKTURQUOISE	0x067A
#define  DARKVIOLET	0x901A
#define  DEEPPINK	0xF8B2
#define  DEEPSKYBLUE	0x05FF
#define  DIMGRAY	0x6B4D
#define  DODGERBLUE	0x1C9F
#define  FIREBRICK	0xB104
#define  FLORALWHITE	0xFFDE
#define  FORESTGREEN	0x2444
#define  FUCHSIA	0xF81F
#define  GAINSBORO	0xDEFB
#define  GHOSTWHITE	0xFFDF
#define  GOLD		0xFEA0
#define  GOLDENROD	0xDD24
#define  GRAY		0x8410
#define  GREEN		0x0400
#define  GREENYELLOW	0xAFE5
#define  HONEYDEW	0xF7FE
#define  HOTPINK	0xFB56
#define  INDIANRED	0xCAEB
#define  INDIGO		0x4810
#define  IVORY		0xFFFE
#define  KHAKI		0xF731
#define  LAVENDER	0xE73F
#define  LAVENDERBLUSH	0xFF9E
#define  LAWNGREEN	0x7FE0
#define  LEMONCHIFFON	0xFFD9
#define  LIGHTBLUE	0xAEDC
#define  LIGHTCORAL	0xF410
#define  LIGHTCYAN	0xE7FF
#define  LIGHTGOLD	0xFFDA
#define  LIGHTGREEN	0x9772
#define  LIGHTGREY	0xD69A
#define  LIGHTPINK	0xFDB8
#define  LIGHTSALMON	0xFD0F
#define  LIGHTSEAGREEN	0x2595
#define  LIGHTSKYBLUE	0x867F
#define  LIGHTSLATEGRAY	0x7453
#define  LIGHTSTEELBLUE	0xB63B
#define  LIGHTYELLOW	0xFFFC
#define  LIME		0x07E0
#define  LIMEGREEN	0x3666
#define  LINEN		0xFF9C
#define  MAGENTA	0xF81F
#define  MAROON		0x8000
#define  MEDIUMAQUAMARINE 	0x6675
#define  MEDIUMBLUE	0x0019
#define  MEDIUMORCHID	0xBABA
#define  MEDIUMPURPLE	0x939B
#define  MEDIUMSEAGREEN	0x3D8E
#define  MEDIUMSLATEBLUE	0x7B5D
#define  MEDIUMSPRINGGREEN	0x07D3
#define  MEDIUMTURQUOISE	0x4E99
#define  MEDIUMVIOLETRED	0xC0B0
#define  MIDNIGHTBLUE	0x18CE
#define  MINTCREAM	0xF7FF
#define  MISTYROSE	0xFF3C
#define  MOCCASIN	0xFF36
#define  NAVAJOWHITE	0xFEF5
#define  NAVY		0x0010
#define  OLDLACE	0xFFBC
#define  OLIVE		0x8400
#define  OLIVEDRAB	0x6C64
#define  ORANGE		0xFD20
#define  ORANGERED	0xFA20
#define  ORCHID		0xDB9A
#define  PALEGOLDENROD	0xEF55
#define  PALEGREEN	0x9FD3
#define  PALETURQUOISE	0xAF7D
#define  PALEVIOLETRED	0xDB92
#define  PAPAYAWHIP	0xFF7A
#define  PEACHPUFF	0xFED7
#define  PERU		0xCC27
#define  PINK		0xFE19
#define  PLUM		0xDD1B
#define  POWDERBLUE	0xB71C
#define  PURPLE		0x8010
#define  RED		0xF800
#define  ROSYBROWN	0xBC71
#define  ROYALBLUE	0x435C
#define  SADDLEBROWN	0x8A22
#define  SALMON		0xFC0E
#define  SANDYBROWN	0xF52C
#define  SEAGREEN	0x2C4A
#define  SEASHELL	0xFFBD
#define  SIENNA		0xA285
#define  SILVER		0xC618
#define  SKYBLUE	0x867D
#define  SLATEBLUE	0x6AD9
#define  SLATEGRAY	0x7412
#define  SNOW		0xFFDF
#define  SPRINGGREEN	0x07EF
#define  STEELBLUE	0x4416
#define  TAN		0xD5B1
#define  TEAL		0x0410
#define  THISTLE	0xDDFB
#define  TOMATO		0xFB08
#define  TURQUOISE	0x471A
#define  VIOLET		0xEC1D
#define  WHEAT		0xF6F6
#define  WHITE		0xFFFF
#define  WHITESMOKE	0xF7BE
#define  YELLOW		0xFFE0
#define  YELLOWGREEN	0x9E66

#define	 HLSMAX         127
#define  HLSMAXm2d3     85
#define  HLSMAXd12      10
#define  HLSMAXd6       21
#define  HLSMAXd2       63
#define  HLSMAXd3       42
#define  RGBMAX         127
#define  RGBMAXm2       254
#define  UNDEFINED      85
#define  _redmask    0xF800
#define  _greenmask  0x07E0
#define  _bluemask   0x001F
#define  GET_VERSION                   	0x10   
#define  ENABLE_TOUCH                   	0x12    
#define  DISABLE_TOUCH                  	0x13    
#define  CALIBRATE_MODE                  0x14    
#define  REGISTER_READ                   0x20    
#define  REGISTER_WRITE                  0x21    
#define  REGISTER_START_ADDRESS_REQUEST  0x22    
#define  REGISTERS_WRITE_TO_EEPROM       0x23    
#define  EEPROM_READ                     0x28    
#define  EEPROM_WRITE                  	0x29    
#define  EEPROM_WRITE_TO_REGISTERS       0x2B 
#define  TOUCH_ENABLE                    0x00
#define  TOUCH_DISABLE                   0x01
#define  HORIZONTAL_SLIDER				0x00
#define  VERTICAL_SLIDER					0x01
#define  SHIFT                           0x01
#define  CAPSLOCK                        0x02
#define  CTRL                            0x03
#define  SPRITE_ACTIVE					0x00
#define  SPRITE_MEMPOS                   0x01
#define  SPRITE_X                        0x02
#define  SPRITE_Y                        0x03
#define  SPRITE_WIDTH                    0x04
#define  SPRITE_HEIGHT                   0x05
#define  SPRITE_COLLIDE1                 0x06
#define  SPRITE_COLLIDE2                 0x07
#define  SPRITE_16BIT                    0x01
#define  SPRITE_8BIT                     0x02
#define  SPRITE_4BIT                     0x03


static const uint8_t at[] = {
                 0,  1,  1,  2,  2,  3,  3,  4,  5,  5,
                 6,  6,  7,  7,  8,  9,  9, 10, 10, 11,
                11, 12, 12, 13, 13, 14, 15, 15, 16, 16,
                17, 17, 18, 18, 19, 19, 20, 20, 21, 21,
                22, 22, 23, 23, 24, 24, 25, 25, 26, 26,
                27, 27, 27, 28, 28, 29, 29, 30, 30, 31,
                31, 31, 32, 32, 33, 33, 33, 34, 34, 35,
                35, 35, 36, 36, 37, 37, 37, 38, 38, 38,
                39, 39, 39, 40, 40, 40, 41, 41, 41, 42,
                42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45
};

static const uint16_t RGB3322565[] = {
    0x0000, 0x000a, 0x0015, 0x001f, 0x0120, 0x012a, 0x0135, 0x013f, 
    0x0240, 0x024a, 0x0255, 0x025f, 0x0360, 0x036a, 0x0375, 0x037f, 
    0x0480, 0x048a, 0x0495, 0x049f, 0x05a0, 0x05aa, 0x05b5, 0x05bf, 
    0x06c0, 0x06ca, 0x06d5, 0x06df, 0x07e0, 0x07ea, 0x07f5, 0x07ff, 
    0x2000, 0x200a, 0x2015, 0x201f, 0x2120, 0x212a, 0x2135, 0x213f, 
    0x2240, 0x224a, 0x2255, 0x225f, 0x2360, 0x236a, 0x2375, 0x237f, 
    0x2480, 0x248a, 0x2495, 0x249f, 0x25a0, 0x25aa, 0x25b5, 0x25bf, 
    0x26c0, 0x26ca, 0x26d5, 0x26df, 0x27e0, 0x27ea, 0x27f5, 0x27ff, 
    0x4800, 0x480a, 0x4815, 0x481f, 0x4920, 0x492a, 0x4935, 0x493f, 
    0x4a40, 0x4a4a, 0x4a55, 0x4a5f, 0x4b60, 0x4b6a, 0x4b75, 0x4b7f, 
    0x4c80, 0x4c8a, 0x4c95, 0x4c9f, 0x4da0, 0x4daa, 0x4db5, 0x4dbf, 
    0x4ec0, 0x4eca, 0x4ed5, 0x4edf, 0x4fe0, 0x4fea, 0x4ff5, 0x4fff, 
    0x6800, 0x680a, 0x6815, 0x681f, 0x6920, 0x692a, 0x6935, 0x693f, 
    0x6a40, 0x6a4a, 0x6a55, 0x6a5f, 0x6b60, 0x6b6a, 0x6b75, 0x6b7f, 
    0x6c80, 0x6c8a, 0x6c95, 0x6c9f, 0x6da0, 0x6daa, 0x6db5, 0x6dbf, 
    0x6ec0, 0x6eca, 0x6ed5, 0x6edf, 0x6fe0, 0x6fea, 0x6ff5, 0x6fff, 
    0x9000, 0x900a, 0x9015, 0x901f, 0x9120, 0x912a, 0x9135, 0x913f, 
    0x9240, 0x924a, 0x9255, 0x925f, 0x9360, 0x936a, 0x9375, 0x937f, 
    0x9480, 0x948a, 0x9495, 0x949f, 0x95a0, 0x95aa, 0x95b5, 0x95bf, 
    0x96c0, 0x96ca, 0x96d5, 0x96df, 0x97e0, 0x97ea, 0x97f5, 0x97ff, 
    0xb000, 0xb00a, 0xb015, 0xb01f, 0xb120, 0xb12a, 0xb135, 0xb13f, 
    0xb240, 0xb24a, 0xb255, 0xb25f, 0xb360, 0xb36a, 0xb375, 0xb37f, 
    0xb480, 0xb48a, 0xb495, 0xb49f, 0xb5a0, 0xb5aa, 0xb5b5, 0xb5bf, 
    0xb6c0, 0xb6ca, 0xb6d5, 0xb6df, 0xb7e0, 0xb7ea, 0xb7f5, 0xb7ff, 
    0xd800, 0xd80a, 0xd815, 0xd81f, 0xd920, 0xd92a, 0xd935, 0xd93f, 
    0xda40, 0xda4a, 0xda55, 0xda5f, 0xdb60, 0xdb6a, 0xdb75, 0xdb7f, 
    0xdc80, 0xdc8a, 0xdc95, 0xdc9f, 0xdda0, 0xddaa, 0xddb5, 0xddbf, 
    0xdec0, 0xdeca, 0xded5, 0xdedf, 0xdfe0, 0xdfea, 0xdff5, 0xdfff, 
    0xf800, 0xf80a, 0xf815, 0xf81f, 0xf920, 0xf92a, 0xf935, 0xf93f, 
    0xfa40, 0xfa4a, 0xfa55, 0xfa5f, 0xfb60, 0xfb6a, 0xfb75, 0xfb7f, 
    0xfc80, 0xfc8a, 0xfc95, 0xfc9f, 0xfda0, 0xfdaa, 0xfdb5, 0xfdbf, 
    0xfec0, 0xfeca, 0xfed5, 0xfedf, 0xffe0, 0xffea, 0xfff5, 0xffff 
};

static const uint8_t font1[] = {

	0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char
	0x00, 0x06, 0x5F, 0x06, 0x00,      // Code for char !
	0x07, 0x03, 0x00, 0x07, 0x03,      // Code for char "
	0x24, 0x7E, 0x24, 0x7E, 0x24,      // Code for char #
	0x24, 0x2B, 0x6A, 0x12, 0x00,      // Code for char $
	0x63, 0x13, 0x08, 0x64, 0x63,      // Code for char %
	0x36, 0x49, 0x56, 0x20, 0x50,      // Code for char &
	0x00, 0x07, 0x03, 0x00, 0x00,      // Code for char '
	0x00, 0x3E, 0x41, 0x00, 0x00,      // Code for char (
	0x00, 0x41, 0x3E, 0x00, 0x00,      // Code for char )
	0x08, 0x3E, 0x1C, 0x3E, 0x08,      // Code for char *
	0x08, 0x08, 0x3E, 0x08, 0x08,      // Code for char +
	0x00, 0xE0, 0x60, 0x00, 0x00,      // Code for char ,
	0x08, 0x08, 0x08, 0x08, 0x08,      // Code for char -
	0x00, 0x60, 0x60, 0x00, 0x00,      // Code for char .
	0x20, 0x10, 0x08, 0x04, 0x02,      // Code for char /
	0x3E, 0x51, 0x49, 0x45, 0x3E,      // Code for char 0
	0x00, 0x42, 0x7F, 0x40, 0x00,      // Code for char 1
	0x62, 0x51, 0x49, 0x49, 0x46,      // Code for char 2
	0x22, 0x49, 0x49, 0x49, 0x36,      // Code for char 3
	0x18, 0x14, 0x12, 0x7F, 0x10,      // Code for char 4
	0x2F, 0x49, 0x49, 0x49, 0x31,      // Code for char 5
	0x3C, 0x4A, 0x49, 0x49, 0x30,      // Code for char 6
	0x01, 0x71, 0x09, 0x05, 0x03,      // Code for char 7
	0x36, 0x49, 0x49, 0x49, 0x36,      // Code for char 8
	0x06, 0x49, 0x49, 0x29, 0x1E,      // Code for char 9
	0x00, 0x6C, 0x6C, 0x00, 0x00,      // Code for char :
	0x00, 0xEC, 0x6C, 0x00, 0x00,      // Code for char ;
	0x08, 0x14, 0x22, 0x41, 0x00,      // Code for char <
	0x24, 0x24, 0x24, 0x24, 0x24,      // Code for char =
	0x00, 0x41, 0x22, 0x14, 0x08,      // Code for char >
	0x02, 0x01, 0x59, 0x09, 0x06,      // Code for char ?
	0x3E, 0x41, 0x5D, 0x55, 0x1E,      // Code for char @
	0x7E, 0x11, 0x11, 0x11, 0x7E,      // Code for char A
	0x7F, 0x49, 0x49, 0x49, 0x36,      // Code for char B
	0x3E, 0x41, 0x41, 0x41, 0x22,      // Code for char C
	0x7F, 0x41, 0x41, 0x41, 0x3E,      // Code for char D
	0x7F, 0x49, 0x49, 0x49, 0x41,      // Code for char E
	0x7F, 0x09, 0x09, 0x09, 0x01,      // Code for char F
	0x3E, 0x41, 0x49, 0x49, 0x7A,      // Code for char G
	0x7F, 0x08, 0x08, 0x08, 0x7F,      // Code for char H
	0x00, 0x41, 0x7F, 0x41, 0x00,      // Code for char I
	0x30, 0x40, 0x40, 0x40, 0x3F,      // Code for char J
	0x7F, 0x08, 0x14, 0x22, 0x41,      // Code for char K
	0x7F, 0x40, 0x40, 0x40, 0x40,      // Code for char L
	0x7F, 0x02, 0x04, 0x02, 0x7F,      // Code for char M
	0x7F, 0x02, 0x04, 0x08, 0x7F,      // Code for char N
	0x3E, 0x41, 0x41, 0x41, 0x3E,      // Code for char O
	0x7F, 0x09, 0x09, 0x09, 0x06,      // Code for char P
	0x3E, 0x41, 0x51, 0x21, 0x5E,      // Code for char Q
	0x7F, 0x09, 0x09, 0x19, 0x66,      // Code for char R
	0x26, 0x49, 0x49, 0x49, 0x32,      // Code for char S
	0x01, 0x01, 0x7F, 0x01, 0x01,      // Code for char T
	0x3F, 0x40, 0x40, 0x40, 0x3F,      // Code for char U
	0x1F, 0x20, 0x40, 0x20, 0x1F,      // Code for char V
	0x3F, 0x40, 0x3C, 0x40, 0x3F,      // Code for char W
	0x63, 0x14, 0x08, 0x14, 0x63,      // Code for char X
	0x07, 0x08, 0x70, 0x08, 0x07,      // Code for char Y
	0x71, 0x49, 0x45, 0x43, 0x00,      // Code for char Z
	0x00, 0x7F, 0x41, 0x41, 0x00,      // Code for char [
	0x02, 0x04, 0x08, 0x10, 0x20,      // Code for <BackSlash>
	0x00, 0x41, 0x41, 0x7F, 0x00,      // Code for char ]
	0x04, 0x02, 0x01, 0x02, 0x04,      // Code for char ^
	0x80, 0x80, 0x80, 0x80, 0x80,      // Code for char _
	0x00, 0x03, 0x07, 0x00, 0x00,      // Code for char `
	0x20, 0x54, 0x54, 0x54, 0x78,      // Code for char a
	0x7F, 0x44, 0x44, 0x44, 0x38,      // Code for char b
	0x38, 0x44, 0x44, 0x44, 0x28,      // Code for char c
	0x38, 0x44, 0x44, 0x44, 0x7F,      // Code for char d
	0x38, 0x54, 0x54, 0x54, 0x08,      // Code for char e
	0x08, 0x7E, 0x09, 0x09, 0x00,      // Code for char f
	0x18, 0xA4, 0xA4, 0xA4, 0x7C,      // Code for char g
	0x7F, 0x04, 0x04, 0x78, 0x00,      // Code for char h
	0x00, 0x00, 0x7D, 0x40, 0x00,      // Code for char i
	0x40, 0x80, 0x84, 0x7D, 0x00,      // Code for char j
	0x7F, 0x10, 0x28, 0x44, 0x00,      // Code for char k
	0x00, 0x00, 0x7F, 0x40, 0x00,      // Code for char l
	0x7C, 0x04, 0x18, 0x04, 0x78,      // Code for char m
	0x7C, 0x04, 0x04, 0x78, 0x00,      // Code for char n
	0x38, 0x44, 0x44, 0x44, 0x38,      // Code for char o
	0xFC, 0x44, 0x44, 0x44, 0x38,      // Code for char p
	0x38, 0x44, 0x44, 0x44, 0xFC,      // Code for char q
	0x44, 0x78, 0x44, 0x04, 0x08,      // Code for char r
	0x08, 0x54, 0x54, 0x54, 0x20,      // Code for char s
	0x04, 0x3E, 0x44, 0x24, 0x00,      // Code for char t
	0x3C, 0x40, 0x20, 0x7C, 0x00,      // Code for char u
	0x1C, 0x20, 0x40, 0x20, 0x1C,      // Code for char v
	0x3C, 0x60, 0x30, 0x60, 0x3C,      // Code for char w
	0x6C, 0x10, 0x10, 0x6C, 0x00,      // Code for char x
	0x9C, 0xA0, 0x60, 0x3C, 0x00,      // Code for char y
	0x64, 0x54, 0x54, 0x4C, 0x00,      // Code for char z
	0x08, 0x3E, 0x41, 0x41, 0x00,      // Code for char {
	0x00, 0x00, 0x77, 0x00, 0x00,      // Code for char |
	0x00, 0x41, 0x41, 0x3E, 0x08,      // Code for char }
	0x02, 0x01, 0x02, 0x01, 0x00,      // Code for char ~
	0x06, 0x09, 0x09, 0x06, 0x00       // Code for <Degrees>
};

static const uint8_t font2[] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
192, 192, 192, 192, 192, 192, 192, 192, 192, 0, 192, 192, 192, 0, 0, 0,
216, 216, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
108, 108, 108, 108, 254, 254, 108, 254, 254, 108, 108, 108, 108, 0, 0, 0, 
24, 24, 124, 254, 210, 248, 124, 22, 150, 254, 124, 24, 24, 0, 0, 0, 
0, 198, 198, 12, 12, 24, 24, 48, 48, 96, 96, 198, 198, 0, 0, 0,
56, 124, 108, 124, 56, 120, 108, 205, 197, 198, 206, 125, 125, 0, 0, 0, 
0, 192, 192, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
48, 48, 96, 96, 192, 192, 192, 192, 192, 192, 96, 96, 48, 48, 0, 0, 
192, 192, 96, 96, 48, 48, 48, 48, 48, 48, 96, 96, 192, 192, 0, 0, 
0, 0, 198, 198, 108, 56, 254, 254, 56, 108, 198, 198, 0, 0, 0, 0, 
0, 0, 48 ,48, 48, 48, 252, 252, 48, 48, 48, 48, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 224, 224, 96, 192, 128, 0, 
0, 0, 0, 0, 0, 0, 248, 248, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 224, 224, 224, 0, 0, 0, 0, 
0, 0, 6, 6, 12, 12, 24, 24, 48, 48, 96, 96, 192, 192, 0, 0, 
124, 254, 198, 198, 206, 206, 222, 246, 230, 230, 198, 254, 124, 0, 0, 0, 
48, 112, 112, 48, 48, 48, 48, 48, 48, 48, 48, 252, 252, 0, 0, 0, 
124, 254, 198, 6, 6, 62, 124, 224, 192, 192, 192, 254, 254, 0, 0, 0,
124, 254, 198, 6, 6, 30, 28, 6, 6, 6, 198, 254, 124, 0, 0, 0, 
12, 28, 28, 60, 108, 108, 204, 204, 254, 254, 12, 12, 12, 0, 0, 0, 
252, 252, 192, 192, 192, 248, 252, 14, 6, 6, 198, 254, 124, 0, 0, 0, 
124, 254, 198, 192, 192, 248, 252, 206, 198, 198, 230, 254, 124, 0, 0, 0, 
254, 254, 198, 12, 12, 24, 24, 48, 48, 48, 48, 48, 48, 0, 0, 0, 
124, 254, 198, 198, 198, 254, 124, 198, 198, 198, 198, 254, 124, 0, 0, 0, 
124, 254, 198, 198, 198, 254, 126, 6, 6, 6, 6, 254, 124, 0, 0, 0, 
0, 0, 0, 192, 192, 0, 0, 0, 192, 192, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 96, 96, 0, 0, 0, 96, 96, 192, 128, 0, 0, 0, 0, 
24, 24, 48, 48, 96, 96, 192, 192, 96, 96, 48, 48, 24, 24, 0, 0, 
0, 0, 0, 0, 248, 248, 0, 0, 248, 248, 0, 0, 0, 0, 0, 0, 
192, 192, 96, 96, 48, 48, 24, 24, 48, 48, 96, 96, 192, 192, 0, 0, 
124, 254, 198, 6, 14, 28, 56, 48, 48, 0, 0, 48, 48, 0, 0, 0, 
0, 0, 0, 0, 28, 34, 78, 82, 82, 82, 78, 32, 31, 0, 0, 0,  
16, 56, 108, 108, 198, 198, 254, 254, 198, 198, 198, 198, 198, 0, 0, 0, 
252, 254, 198, 198, 198, 254, 252, 198, 198, 198, 198, 254, 252, 0, 0, 0, 
124, 254, 198, 192, 192, 192, 192, 192, 192, 192, 198, 254, 124, 0, 0, 0, 
248, 252, 206, 198, 198, 198, 198, 198, 198, 198, 206, 252, 248, 0, 0, 0, 
254, 254, 192, 192, 192, 248, 248, 192, 192, 192, 192, 254, 254, 0, 0, 0, 
254, 254, 192, 192, 192, 248, 248, 192, 192, 192, 192, 192, 192, 0, 0, 0, 
56, 124, 230, 192, 192, 192, 222, 222, 198, 198, 238, 124, 56, 0, 0, 0, 
198, 198, 198, 198, 198, 254, 254, 198, 198, 198, 198, 198, 198, 0, 0, 0, 
48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 0, 0, 0, 
6, 6, 6, 6, 6, 6, 6, 6, 6, 198, 238, 124, 56, 0, 0, 0, 
198, 198, 204, 204, 216, 248, 240, 216, 216, 204, 204, 198, 198, 0, 0, 0, 
192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 254, 254, 0, 0, 0, 
195, 231, 231, 255, 255, 255, 219, 219, 219, 219, 219, 195, 195, 0, 0, 0, 
198, 198, 230, 230, 230, 246, 246, 222, 222, 206, 206, 198, 198, 0, 0, 0, 
56, 124, 238, 198, 198, 198, 198, 198, 198, 198, 238, 124, 56, 0, 0, 0, 
248, 252, 206, 198, 206, 252, 248, 192, 192, 192, 192, 192, 192, 0, 0, 0, 
56, 124, 238, 198, 198, 198, 198, 198, 214, 222, 204, 126, 58, 0, 0, 0, 
248, 252, 206, 198, 206, 252, 248, 216, 216, 204, 204, 198, 198, 0, 0, 0, 
60, 126, 230, 192, 224, 120, 60, 14, 6, 6, 206, 252, 120, 0, 0, 0, 
252, 252, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 0, 0, 0, 
198, 198, 198, 198, 198, 198, 198, 198, 198, 198, 198, 254, 124, 0, 0, 0, 
198, 198, 198, 198, 198, 198, 198, 108, 108, 56, 56, 16, 16, 0, 0, 0, 
195, 219, 219, 219, 219, 219, 255, 255, 255, 231, 231, 195, 195, 0, 0, 0, 
198, 198, 108, 108, 56, 56, 16, 56, 56, 108, 108, 198, 198, 0, 0, 0, 
204, 204, 204, 204, 120, 120, 48, 48, 48, 48, 48, 48, 48, 0, 0, 0, 
254, 254, 12, 24, 24, 48, 48, 96, 96, 192, 192, 254, 254, 0, 0, 0, 
240, 240, 192, 192, 192, 192, 192, 192, 192, 192, 192, 240, 240, 0, 0, 0, 
128, 192, 192, 96, 32, 48, 16, 24, 8, 12, 4, 6, 2, 0, 0, 0, 
240, 240, 48, 48, 48, 48, 48, 48, 48, 48, 48, 240, 240, 0, 0, 0, 
16, 16, 56, 56, 108, 108, 198, 198, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 
0, 192, 192, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 120, 252, 204, 124, 252, 204, 204, 254, 119, 0, 0, 0, 
192, 192, 192, 192, 248, 252, 204, 204, 204, 204, 204, 252, 248, 0, 0, 0, 
0, 0, 0, 0, 120, 252, 196, 192, 192, 192, 196, 252, 120, 0, 0, 0, 
12, 12, 12, 12, 124, 252, 204, 204, 204, 204, 204, 252, 124, 0, 0, 0, 
0, 0, 0, 0, 120, 252, 204, 252, 248, 192, 196, 252, 120, 0, 0, 0, 
12, 60, 48, 48, 252, 252, 48, 48, 48, 48, 48, 48, 48, 0, 0, 0, 
0, 0, 0, 0, 120, 252, 204, 204, 204, 252, 124, 12, 140, 252, 120, 0, 
192, 192, 192, 192, 248, 252, 204, 204, 204, 204, 204, 204, 204, 0, 0, 0, 
48, 48, 0, 0, 112, 112, 48, 48, 48, 48, 48, 120, 120, 0, 0, 0, 
24, 0, 0, 0, 24, 24, 24, 24, 24, 24, 24, 24, 152, 248, 112, 0, 
192, 192, 196, 204, 216, 240, 224, 240, 216, 200, 204, 198, 198, 0, 0, 0, 
224, 224, 96, 96, 96, 96, 96, 96, 96, 96, 96, 240, 240, 0, 0, 0, 
0, 0, 0, 0, 236, 254, 214, 214, 214, 198, 198, 198, 198, 0, 0, 0, 
0, 0, 0, 0, 248, 252, 204, 204, 204, 204, 204, 204, 204, 0, 0, 0, 
0, 0, 0, 0, 120, 252, 204, 204, 204, 204, 204, 252, 120, 0, 0, 0, 
0, 0, 0, 0, 248, 252, 204, 204, 204, 204, 252, 248, 192, 192, 192, 0, 
0, 0, 0, 0, 124, 252, 204, 204, 204, 204, 252, 124, 12, 12, 6, 0, 
0, 0, 0, 0, 216, 252, 238, 198, 192, 192, 192, 192, 192, 0, 0, 0, 
0, 0, 0, 0, 120, 252, 196, 240, 120, 12, 140, 252, 120, 0, 0, 0, 
48, 48, 48, 48, 252, 252, 48, 48, 48, 48, 48, 28, 12, 0, 0, 0, 
0, 0, 0, 0, 204, 204, 204, 204, 204, 204, 204, 252, 120, 0, 0, 0, 
0, 0, 0, 0, 198, 198, 198, 108, 108, 56, 56, 16, 16, 0, 0, 0, 
0, 0, 0, 0, 198, 198, 198, 214, 214, 124, 108, 108, 40, 0, 0, 0, 
0, 0, 0, 0, 198, 198, 108, 124, 56, 124, 108, 198, 198, 0, 0, 0, 
0, 0, 0, 0, 204, 204, 204, 204, 204, 252, 124, 12, 140, 252, 120, 0, 
0, 0, 0, 0, 252, 252, 24, 24, 48, 48, 96, 252, 252, 0, 0, 0,
0, 12, 24, 24, 24, 24, 24, 48, 24, 24, 24, 24, 24, 12, 0, 0,
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 0,
0, 48, 24, 24, 24, 24, 24, 12, 24, 24, 24, 24, 24, 48, 0, 0,
0, 0, 0, 0, 0, 96, 226,158,12,0, 0, 0, 0, 0, 0, 0,
120, 252, 204, 204, 204, 204, 252, 120, 0, 0, 0, 0, 0, 0, 0, 0
};


class GFX4d : public Print {
 public:

#ifndef USE_FS
File dataFile;
File userImag;
File userDat;
#else
fs::File dataFile;
fs::File userImag;
fs::File userDat;
#endif

GFX4d();

  void     begin(void),
           setGRAM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),
           WrGRAM(uint32_t color),
           WrGRAMs(uint32_t *data, uint16_t l),
           WrGRAM16(uint16_t color),
           WrGRAMs16(uint16_t *data, uint16_t l),
           WrGRAMs8(uint8_t *data, uint16_t l, byte mul),
	       WrGRAMs16232(uint16_t *data, uint16_t l),
	       WrGRAMs16232(uint16_t *data, uint16_t l, uint8_t *odata, int x, int y),
           WrGRAM16232(uint16_t mcolor, int32_t l),
           WrGRAMs16232NT(uint16_t *data, uint16_t l),
           WrGRAMend(),
           WrGRAMstart(),
           FillScreen(uint16_t color),
           PutPixel(int16_t x, int16_t y, uint16_t color),
           Font(uint8_t f),
           Contrast(int ctrst),
	       Vline(int16_t x, int16_t y, int16_t h, uint16_t vcolor),
           Hline(int16_t x, int16_t y, int16_t w, uint16_t hcolor),
           RectangleFilled(int16_t x, int16_t y, int16_t x1, int16_t y1, uint16_t color),
           MoveTo(int16_t x, int16_t y),
           newLine(int8_t f1, int8_t ts, uint16_t ux),
           Open4dGFX(String file4d),
           DownloadFile(String WebAddr, String Fname),
           DownloadFile(String WebAddr, String Fname, String sha1),
           DownloadFile(String Address, uint16_t port, String hfile, String Fname),
           Download(String Address, uint16_t port, String hfile, String Fname, String sha1),
           ScrollEnable(boolean se),
           touch_Set(uint8_t mode),
           UserImage(uint16_t ui),
           UserImage(uint16_t ui, int altx, int alty),
           UserImageDR(uint16_t ui, int16_t uxpos, int16_t uypos, int16_t width, int16_t height, int16_t tuix, int16_t tuiy),
           UserImages(uint16_t uisnb, int16_t framenb),
           UserImages(uint16_t uis, int16_t frame, int offset),
           UserImages(uint16_t uis, int16_t frame, int offset, int16_t altx, int16_t alty),
           UserImages(uint16_t uisnb, int16_t framenb, int16_t newx, int16_t newy),
           UserImagesDR(uint16_t uino, int frames, int16_t uxpos, int16_t uypos, int16_t uwidth, int16_t uheight),
           LedDigitsDisplay(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks),
           LedDigitsDisplay(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks, int16_t altx, int16_t alty),
           LedDigitsDisplaySigned(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks),
           LedDigitsDisplaySigned(int16_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks, int16_t altx, int16_t alty),
           DrawWidget(uint32_t Index, int16_t uix, int16_t uiy, int16_t uiw, int16_t uih, uint16_t frame, int16_t bar, bool images, uint8_t cdv),
           DrawImage(uint32_t Index, int16_t uix, int16_t uiy),
           UserCharacter(uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, uint16_t bgcolor),
           UserCharacterBG(uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, 
boolean draw, uint32_t bgindex),
           UserCharacterBG(int8_t ui, uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, boolean draw),
           UserCharacterBG(uint32_t *data, uint8_t ucsize, int16_t ucx, int16_t ucy, uint16_t color, boolean draw, uint32_t bgindex, bool type, int8_t ui),
           PrintImage(uint16_t iIndex),
           PrintImageFile(String ifile),
           PrintImageWifi(String Address, uint16_t port, String hfile),
           PrintImageWifi(String WebAddr),
           PrintImageWifi(String Address, uint16_t port, String hfile, String SHA1),
           PrintImageWifi(String WebAddr, String SHA1),
           ImageWifi(boolean local, String Address, uint16_t port, String hfile, String SHA1),
           imageTouchEnable(int gcinum, boolean en),
           TextSize(uint8_t s),
           TextColor(uint16_t c),
           TextColor(uint16_t c, uint16_t b),
           TextWrap(boolean w),
	       BacklightOn(boolean bl),
           drawChar2tw(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size),
           drawChar2(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size, uint8_t sizeht),           
           drawChar1tw(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size),
           drawChar1(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size, uint8_t sizeht),
           Arc( int16_t x0, int16_t y0, int16_t r, uint8_t sa, uint16_t color),
           Orientation(uint8_t r),
           Cls(uint16_t color),
           Cls(),
           SmoothScrollSpeed(uint8_t spd),
           Triangle(int16_t x0,int16_t y0,int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color),        
           TriangleFilled(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3, uint16_t c),
           RoundRectFilled(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color),
           RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color),
           Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           Panel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           PanelRecessed(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           TextWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t txtcolor, uint16_t txbcolor, uint16_t frcolor),
           TextWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t txtcolor, uint16_t txbcolor),
           TextWindowRestore(),
           TWwrite(const char txtinput),
           TWprint(String istr),
           TWprintln(String istr),
           TWprintAt(uint8_t pax, uint8_t pay, String istr),
           TWcls(),
           TWcursorOn(bool twco),
           TWcolor(uint16_t fcol),
           TWcolor(uint16_t fcol, uint16_t bcol),
           DeleteButton(int hndl, uint16_t color),
           DeleteButton(int hndl),
           DeleteButtonBG(int hndl, int objBG),
           UserImageHide(int hndl),
           UserImageHide(int hndl, uint16_t color),
           UserImageHideBG(int hndl, int objBG),
           Orbit(int angle, int lngth, int *oxy),
           ButtonActive(uint8_t butno, boolean act),
           ButtonUp(int hndl),
           ButtonDown(int hndl),
           Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, String btext, int8_t tfont, uint16_t tcolor),
           Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb, uint16_t tcolor, int8_t tfont, int8_t tfontsizeh, int8_t tfontsize, String btext),
           drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorb, String btext, int8_t tfont, int8_t tfontsize, int8_t tfontsizeh, uint16_t tcolor),
           Slider(uint8_t state, int16_t x, int16_t y, int16_t r, int16_t b, uint16_t colorb, uint16_t colort, int16_t scale, int16_t value),
           Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),           
           ArcFilled(int16_t xa,int16_t ya,int16_t r,uint8_t sa,int16_t ea,uint16_t color),
           CircleFilled(int16_t xc, int16_t yc, int16_t r, uint16_t color),
           Circle(int16_t xc, int16_t yc, int16_t r, uint16_t color), 
           Ellipse(int16_t xc, int16_t yc, int16_t rx, int16_t ry, uint16_t color),
           EllipseFilled(int16_t xc, int16_t yc, int16_t rx, int16_t ry, uint16_t color),
           c565toRGBs(uint16_t i565),
           RGB2HLS(),
           Close4dGFX(),
           HLS2RGB(int H, int L, int S),
           TWtextcolor(uint16_t twc),
           outofrange(int16_t euix, int16_t euiy, int16_t euiw, int16_t euih),
           FontStyle(uint8_t ctyp),
		   SpriteAreaSet(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1),
		   SetSprite(int num, bool active, int x, int y, uint16_t bscolor, uint16_t * sdata),
		   UpdateSprites(uint16_t bscolor, uint16_t * sdata),
		   SetNumberSprites(uint16_t numspr),
		   SpriteEnable(int snum, bool sen),
		   SpriteUpdate(int16_t tsx, int16_t tsy, int16_t tsx1, int16_t tsy1, uint16_t bscolor, uint16_t * sdata),
           SetMaxNumberSprites(uint16_t snos),
		   ResetKeypad(),
	       Invert(boolean i);
    
  uint16_t RGBto565(uint8_t r, uint8_t g, uint8_t b);
  uint16_t RGBs2COL(uint8_t r, uint8_t g, uint8_t b);
  uint8_t  hue_RGB(int Hin, int M1, int M2);
  int16_t  getHeight(void);
  int16_t  getWidth(void);
  uint16_t getWidgetNumFrames(int widget);
  int8_t   FontHeight(void);
  int8_t   Orientation(void);
  int8_t   Font(void); 
  int16_t  getY(void);
  int16_t  getX(void);
  int16_t  ImageTouchedAuto(int UpdateImages);
  uint16_t getScrolledY(uint16_t y);
  int16_t  getScrollOffset(void);
  uint16_t read16(void);
  uint32_t read32(void);
  uint32_t bevelColor(uint16_t colorb);
  uint32_t HighlightColors(uint16_t colorh, int step);
  uint16_t oldgTX, oldgTY, oldgPEN;
  uint16_t gTX, gTY, gPEN;
  uint16_t touch_GetX(void);
  uint16_t touch_GetY(void);
  uint8_t  touch_GetPen(void);
  uint16_t imageTouched(void);
  uint8_t  CheckButtons(void);
  uint16_t getNumberofObjects(void);
  uint8_t  getFontData(uint8_t fntnum, uint16_t val);
  String   GetCommand();
  int      DecodeKeypad(int kpad, int kpress, byte *kbks, int8_t *kbck);
  bool     KeypadStatus(int keyType);
  bool     SpriteInit(uint16_t *sdata, size_t nums);
  bool	   SpriteAdd(int pos, int snum, int x, int y, uint16_t *sdata);
  boolean  CheckSD(void);
  boolean  CheckDL(void);
  boolean  touch_Update(void);
  boolean  TWMoveTo(uint8_t twcrx, uint8_t twcry);
  void     Scroll(uint16_t VSP);
  void     setScrollArea(uint16_t TFA, uint16_t BFA);  
  uint8_t  getValfromString(String strval, uint8_t indx);
  int16_t  XYposToDegree(int curX, int curY);
  uint32_t getIndexfromString(String strval, uint8_t indx);
  uint32_t getCoordfromString(String strval, uint8_t indx);
  uint8_t  ReadCommand(uint8_t reg, uint8_t index);
  uint16_t GetSliderValue(uint16_t ui, uint8_t axis, uint16_t uiv, uint16_t ming, uint16_t maxg);
  uint16_t SpriteGetPixel(int snum, int xo, int yo, uint16_t tcolor, uint16_t * sdata);
  uint16_t SpriteGetPalette(int pnumber);
  int      GetSpritesAt(int xo, int yo, uint16_t tcolor, int * slist, uint16_t * sdata);
  int      SpriteTouched();
  int      GetSprite(int snum, int choice);
  int      GetNumberSprites();
  int16_t  GetSpriteImageNum(int snum);
  void     SetCommand(uint8_t c);
  void     SetData(uint8_t d);
  void     SetData(uint8_t * data, uint8_t size);
  void     SpriteSetPalette(int pnumber, uint16_t plcolor);

  uint8_t  GFX4d_RED;
  uint8_t  GFX4d_BLUE;
  uint8_t  GFX4d_GREEN;
  uint16_t palette4bit[16] = {BLACK, MAROON, GREEN, OLIVE, NAVY, PURPLE, TEAL, SILVER, GRAY, RED, LIME, YELLOW, BLUE, FUCHSIA, AQUA, WHITE};

  virtual  size_t write(uint8_t);

private:
  String   dat4d;
  String   gci4d;
  uint16_t tdark;
  uint16_t tlight;
  uint16_t wi;
  uint16_t hi;
  uint16_t ddoscol;
  uint16_t scrollOffset;
  uint8_t  spiread(void);
  uint8_t  ssSpeed;
  inline void setGRAM_(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  boolean  sEnable;
  boolean  hwSPI;
  boolean  twcurson;
  boolean  shift;
  boolean  caps;
  boolean  ctrl;
  boolean  ddos;
  int8_t   fsh;
  int8_t   fsh1;
  int8_t   fsw;
  int8_t   fno;
  int8_t   oldtpen;
  int8_t   lastfsh;
  int8_t   lastfsw;
  int8_t   _cs, _dc, _rst, _mosi, _miso, _sclk, _disp, _tcs, _sd;

protected:
  
  uint32_t krepeat;
  uint32_t tuiIndex[MAX_WIDGETS];    
  int16_t
           tuix[MAX_WIDGETS],
           tuiy[MAX_WIDGETS],
           tuiw[MAX_WIDGETS],
           tuih[MAX_WIDGETS],
           pressed = -1,
           sdetaila,
           sdetaily,
           sdetailh,
           sdetailx,
           sdetailw,
           *spriteData,
           *spriteList,
	       *spriteLast,
	       *spriteNum,
           width, height, cursor_x, cursor_y ,scrollpos, xic, yic;
  uint16_t
#ifndef ESP8266
           buf16[12001],
#endif
	       gciobjframes[MAX_WIDGETS],
           twframecol,
           textcolor, textbgcolor,
           chracc, chrdwn,
           sgx,
           sgy,
           sgw,
           sgh,
           txtf, txtb,
           txtx,
           txty,
           txtw,
           txth, 
           twcurx,
           twcury,
           twcolnum,
           bposx[128],
           bposy[128],
           bposw[128],
           bposh[128],
           bposc[128],
           txfcol[600],
	       spriteArea[4],
	       numSprites,
           butdelay,
		   gciobjnum,
           gciobjtouched;
  uint8_t
           cdv[MAX_WIDGETS],
           msprites,
		   fstyle,
           lastpen,
           prevpen,
           twxpos,
           twypos,
           pencount,
           oldbut,
           twcl,
           bstat[128],
           txtbuf[600],
           tchbuf[15],
           textsize,
           textsizeht,
           rotation,
           piflag,
           pdfix,
           h,l,s, 
           _lastbut,
           _but;
  boolean
           saSet,
           tchen,
           uimage,
           twframe,
           butchnge,
           dlok,
           sdok,
           gciobjtouchenable[MAX_WIDGETS],
           scrolled,
           nl,
           txtwin,
           wrap,
           bactive[128];
  String
           cmdtxt,
           twtext;
  long
           touchTime;
};

#endif
