// compiles with ESP32 board library v2.0, does not boot if compiled with previous versions.
// set board type to M5Stack-Core-ESP32

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
 This code is based on the work on Vern Little, see copyright notice above.
 Adapted by/for FlyOnSpeed.org, 2021. lenny@flyonspeed.org
*/

//#define SERIALDATADEBUG // show serial packet debug
//#define IAS_IN_MPH  // uncomment this line for IAS in MPH, otherwise it will display in Kts;

#define firmwareVersion "3.2.2l"

#include <GaugeWidgets.h>
#include <M5Stack.h>
#include <Free_Fonts.h>
#include <SavLayFilter.h>

// includes for web firmware update
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <Preferences.h>
Preferences preferences;

WebServer server(80);

const char* host = "display";
const char* ssid = "OnSpeedDisplay";
const char* password = "angleofattack";
bool fwUpdateMode=false;

#define TFT_GREY 0x7BEF
#define TFT_LIGHT_GREY  0xAD55

TFT_eSprite gdraw = TFT_eSprite(&M5.Lcd);
Gauges myGauges;

float AOAThresholds[8];// old % based tresholds= {0, 39, 41, 55, 65, 66, 79, 80};

//0 - 0
//1 - L/D max -.1
//2 - L/Dmax
//3 - Onspeed Fast
//4 - Onspeed Slow
//5 - Onspeed Slow +.1
//6 - Stall Warning -.1
//7 - Stall Warning


// screen size variables
const uint16_t  WIDTH = 320; //X
const uint16_t  HEIGHT = 240; //Y

// display variables
uint64_t loopTime = millis(), currentMillis, previousMillis = millis(),flashTime=millis(), numbersUpdateTime;
uint64_t serialMillis=millis();
uint64_t gHistoryTime=millis();
uint16_t displayBrightness = 255;
int16_t  displayType;
boolean  numericDisplay;
boolean  flashFlag;
const uint16_t  updateRateGraphics = 100;  //milliseconds  
const uint16_t  updateRateNumbers = 500;  //milliseconds
const uint16_t  flashRate = 250;  //milliseconds
const float aoaSmoothingAlpha=.7; //1 = max smoothing, 0.01 no smoothing.
const float slipSmoothingAlpha=.5; //1 = max smoothing, 0.01 no smoothing. 
const float decelSmoothingAlpha=.04; //1 = max smoothing, 0.01 no smoothing.

const float serialRate= 0.1f; // 10 hz;
//
// AOA widget variables and defaults
//
uint16_t wgtWidth;
uint16_t wgtHeight;
uint16_t wgtX0;
uint16_t wgtY0;

// Attitude indicator variables
int16_t px0 = 159;
int16_t py0 = 119;
int16_t arcSize = 115;
int16_t arcWidth = 15;
int16_t maxDisplay = 360;
int16_t minDisplay = 0;
int16_t startAngle = 0; // -Roll?
int16_t arcAngle = 360;
int16_t clockWise = true;
uint8_t gradMarks = 0;

// Serial data variables
String serialBufferString;
float AOA=0.0;
float SmoothedAOA=0.00;
int PercentLift;
float Pitch=0.0;
float Roll=0.0;
float IAS=0.0;
float Palt=0.0;
float iVSI=0.0;
float VerticalG=1.0;
float LateralG=0.0;
float SmoothedLateralG;
float FlightPath=0.0;
int FlapPos=0;
float TurnRate=0.0;
int OAT=0;
int16_t Slip=0;
float OnSpeedStallWarnAOA=20;
float OnSpeedSlowAOA=15;
float OnSpeedFastAOA=10;
float OnSpeedTonesOnAOA=5;
float gOnsetRate=0.0;
int SpinRecoveryCue=0;
int DataMark;
float DecelRate=0.0;
float SmoothedDecelRate=0.0;
float gHistory[300];
int gHistoryIndex=0;

// number display variables
float displayIAS=0.0;
float displayPalt=0.0;
float displayPitch=0.0;
float displayVerticalG=0.0;
int displayPercentLift=0;
float displayDecelRate=0.0;

double iasDerivativeInput;
SavLayFilter iasDerivative (&iasDerivativeInput, 1, 15);           //Computes the first derivative

unsigned int selectedPort=0; // selected serial port

//
// Instance of main data extraction library
//
void drawAOA(uint16_t X0, uint16_t Y0, uint16_t W, uint16_t H, float AOA, boolean flashFlag, float Array[]); // function to draw AOA widget
void drawSlip (uint16_t X0, uint16_t Y0, uint16_t W, uint16_t H, int16_t Yaw, boolean flashFlag, float Array[]); // function to draw Slip widget

void setup() {
  
  //
  // initialize the M5Stack object and clear display
  //
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setBrightness (50);
  // mute the speaker (annoying hiss)
  dacWrite (25,0);

  gdraw.setColorDepth(8);
  gdraw.createSprite(WIDTH, HEIGHT);
  gdraw.fillSprite (TFT_BLACK);

// prefill gHistory buffer
for (int i=0;i<300;i++) gHistory[i]=1.00;

// load preferences for Serial port selection)
preferences.begin("OnSpeed", false);
selectedPort=preferences.getUInt("SerialPort", 0);
unsigned long detectSerialStart=millis();
while (selectedPort==0 && millis()-detectSerialStart<30000) // allow 30 seconds to detect serial port
          {
          // check serial port       
          selectedPort=checkSerial();
          // save serial port preference
          if (selectedPort!=0) preferences.putUInt("SerialPort", selectedPort);
          }
preferences.end();

//start selected serials port as Serial2

switch (selectedPort) {
                      case 1: {
                              // TTL input via power board (including v2 Onspeed)
                              Serial2.begin (115200, SERIAL_8N1, 16, 17,false); //GPIO16 is RX, GPIO17 is TX  (Vern's power board)
                              Serial.println("GPIO16 is RX, GPIO17 is TX, TTL");
                              break;
                              }
                      case 2: {
                              // rs232 input via power board (including v3 Onspeed)
                              Serial2.begin (115200, SERIAL_8N1, 16, 17,true); //GPIO16 is RX, GPIO17 is TX  (Vern's power board)   
                              Serial.println("GPIO16 is RX, GPIO17 is TX, RS232");
                              break;
                              }
                      case 3: {
                              // simulator demo M5 with onspeed v3 on pin 9 TTL
                              Serial2.begin (115200, SERIAL_8N1, 22, 17,false); //GPIO22 is RX, GPIO17 is TX  (straight M5) 
                              Serial.println("GPIO22 is RX, GPIO17 is TX, TTL"); 
                              break;
                              }
                      case 0: {
                              gdraw.setColorDepth(8);
                              gdraw.createSprite(WIDTH, HEIGHT);
                              gdraw.fillSprite (TFT_BLACK);
                              gdraw.setFreeFont(FSS12);
                              gdraw.setTextDatum(MC_DATUM);
                              gdraw.setTextColor (TFT_RED);
                              gdraw.drawString("No Serial Stream Detected",160,120);
                              gdraw.setTextColor (TFT_WHITE);
                              gdraw.drawString("Is OnSpeed running?",160,160);
                              gdraw.pushSprite (0, 0);
                              gdraw.deleteSprite();
                              delay(3000);
                              break;
                              }
                    }

Serial.begin(115200); // console serial
  
delay (100);

  //
  // Display splash screen
  //
  gdraw.setFreeFont(FSSB24);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum(MC_DATUM);
  gdraw.drawString("Fly OnSpeed",160,60);

  gdraw.setFreeFont(FSS9);
  gdraw.drawString("version: "+String(firmwareVersion),160,120);
  gdraw.drawString("To upgrade press Center button",160,220);
  gdraw.pushSprite (0, 0);
  gdraw.deleteSprite();
  M5.update();
  // duration of splash screen display, check for center button for fw upgrade
  uint64_t waitTime = millis();
  while (millis()-waitTime<3000)
    {
      M5.update();  
    //Button B held down at bootup
  if (M5.BtnB.isPressed())
      {
      gdraw.setColorDepth(8);
      gdraw.createSprite(WIDTH, HEIGHT);
      gdraw.fillSprite (TFT_BLACK);
      gdraw.setFreeFont(FSSB12);
      gdraw.setTextColor (TFT_WHITE);
      gdraw.setTextDatum(MC_DATUM);
      gdraw.drawString("Firmware Upgrade Server",160,20);

      gdraw.setFreeFont(FSS12);
      gdraw.setTextDatum(ML_DATUM);
      gdraw.drawString("Wifi SSID: "+String(ssid),20,70);
      gdraw.drawString("Password: "+String(password),20,100);
      gdraw.drawString("Browse to:",20,140);
      gdraw.drawString("http://"+String(host)+".local/upgrade",20,170);
      
      gdraw.setTextDatum(ML_DATUM);
      gdraw.drawString("v"+String(firmwareVersion),5,215);

      gdraw.setFreeFont(FSSB12);
      gdraw.setTextColor (TFT_RED);
      gdraw.setTextDatum(MR_DATUM);
      gdraw.drawString("EXIT",280,215);
      
      gdraw.pushSprite (0, 0);
      gdraw.deleteSprite();
      
      WiFi.softAP(ssid, password);
      delay(100); // wait to init softAP  
      
      IPAddress Ip(192, 168, 0, 2);
      IPAddress NMask(255, 255, 255, 0);
      WiFi.softAPConfig(Ip, Ip, NMask);  
      IPAddress myIP = WiFi.softAPIP();
      
      Serial.print("AP IP address: ");
      Serial.println(myIP);
      delay(100);
      
      MDNS.begin(host);
      server.begin();
      
      MDNS.addService("http", "tcp", 80);
      /*handling uploading firmware file */
      server.on("/upgrade", HTTP_GET, handleUpgrade);
      server.on("/", HTTP_GET, handleIndex);
    
      server.on("/upload", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        if (Update.hasError()) handleUpgradeFailure(); else handleUpgradeSuccess(); 
        //server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        // reset serial port preference on upgrade
        preferences.begin("OnSpeed", false);
        preferences.putUInt("SerialPort", 0);
        preferences.end();
        delay(5000);   
        ESP.restart();
        }, []() {
          HTTPUpload& upload = server.upload();
         if (upload.status == UPLOAD_FILE_START) {
            //Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
              //Update.printError(Serial);
                          
            }
              gdraw.setColorDepth(8);
              gdraw.createSprite(WIDTH, HEIGHT);
              gdraw.fillSprite (TFT_BLACK);
              gdraw.setFreeFont(FSSB12);
              gdraw.setTextColor (TFT_WHITE);
              
              gdraw.setTextDatum(MC_DATUM);
              gdraw.drawString("Upgrading Firmware",160,90);
              
              gdraw.setFreeFont(FSS12);
              gdraw.drawString("Please wait...",160,150);
              
              gdraw.pushSprite (0, 0);
              gdraw.deleteSprite();  
          } else if (upload.status == UPLOAD_FILE_WRITE) {
            /* flashing firmware to ESP*/
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
              //Update.printError(Serial);
            }
          } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { //true to set the size to the current progress
              //Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
              //Update.printError(Serial);
            }
          }
        });
    //called when the url is not defined here
  server.onNotFound([]() {
      server.send(404, "text/plain", "FileNotFound");
      });
      fwUpdateMode=true;
      break; // break while loop
      }

    }

} // setup

void loop() {
  M5.update();
  if (fwUpdateMode)
      {
      server.handleClient();
      if (M5.BtnC.wasPressed())
        {
         fwUpdateMode=false;         
         WiFi.softAPdisconnect (true);
        }
      return;
      }
  
  SerialRead(); // get serial data



  //
  // Change display brightness and display format using panel buttons.
  //
  if (M5.BtnC.wasPressed() && (displayBrightness > 0)) {
    displayBrightness *= 2; // brightness up
  }

  if (M5.BtnA.wasPressed() && (displayBrightness <= 255)) {
    displayBrightness /= 2; // brightness down
  }

  displayBrightness=constrain(displayBrightness,1,255);
  
  M5.Lcd.setBrightness (displayBrightness);
  
  if (M5.BtnB.wasPressed()) {
    gdraw.setColorDepth(8);
    gdraw.createSprite(WIDTH, HEIGHT);
    gdraw.fillSprite (TFT_BLACK);
	displayType ++;
    if (displayType > 4) displayType = 0; // type of display
  }

 // update G history buffer
 if (millis()-gHistoryTime>200)
    {
    gHistory[gHistoryIndex]=VerticalG;
    if (gHistoryIndex<299)  gHistoryIndex++; else gHistoryIndex=0;
    gHistoryTime=millis();
    }


if (millis() > (loopTime + updateRateGraphics))
{      loopTime = millis();

      gdraw.setColorDepth(8);
      gdraw.createSprite(WIDTH, HEIGHT);
      gdraw.fillSprite (TFT_BLACK);

  // update numbers at a slower rate so they are readable
  if (millis()-numbersUpdateTime>updateRateNumbers)
      {
      displayIAS=IAS;
            #ifdef IAS_IN_MPH
              displayIAS=IAS*1.15078;
            #endif  

      displayPalt=Palt;
      displayPitch=Pitch;
      displayVerticalG=VerticalG;
      displayPercentLift=PercentLift;
      displayDecelRate=SmoothedDecelRate;
      numbersUpdateTime=millis();
      }
	/*
	 Main AOA alarm detection & update AOA display
	 */ 	  
	switch (displayType) {
      case 0: {     // 0 default indicator with numeric display
          wgtWidth = 102;
          wgtHeight = 192;
          wgtX0 = (WIDTH - wgtWidth) / 2; wgtY0 = 0;
		      numericDisplay = true;
          displayAOA();
          break;
          
        }
      case 1: {
          // display Attitude Indicator
           AiGraph (px0, py0, arcSize, arcWidth, maxDisplay, minDisplay, startAngle, arcAngle, clockWise,
           gradMarks, int(Pitch), int(Roll), 360, FlightPath);
          
          // update numeric displays
          // Update airspeed numeric display
          // print labels
          gdraw.setFreeFont(FSS12);
          
          gdraw.setCursor(5, 60);
          gdraw.setTextColor (TFT_GREY);
          gdraw.print ("IAS");
          
          gdraw.setCursor(5, 230);
          gdraw.setTextColor (TFT_LIGHT_GREY);
          gdraw.print("G");
          
          gdraw.setCursor(263, 60);          
          gdraw.setTextColor (TFT_GREY);
          gdraw.print("ALT");
          
          gdraw.setCursor(260, 230);
          gdraw.setTextColor (TFT_LIGHT_GREY);
          gdraw.print("AOA");

          // update numeric pitch display
          // same font as labels
          // dark background for pitch readability
          gdraw.fillRoundRect(55,129,56,21,3,TFT_DARKGREY);
          
          gdraw.setTextColor (TFT_WHITE);
          char PitchStr[4];
          sprintf(PitchStr,"%1.1f", displayPitch);
          gdraw.setTextDatum(MR_DATUM);
          gdraw.drawString(PitchStr,100,138);
          // draw degree symbol
          gdraw.drawCircle (106, 132, 0.50f * 5, TFT_WHITE);
          
          gdraw.setFreeFont(FSSB18);
          gdraw.setTextColor (TFT_BLACK);
          gdraw.setCursor(5, 30);
          gdraw.print(int(displayIAS));
  
          // Update G-force numeric display
          //gdraw.setFreeFont(FSSB18);
          gdraw.setTextColor (TFT_WHITE);
          gdraw.setCursor(5, 200);
          gdraw.printf ("%1.1f", displayVerticalG);          
          
          
          // Update pressure altitude numeric display
          //gdraw.setFreeFont(FSSB18);
          gdraw.setTextColor (TFT_BLACK);
          gdraw.setTextDatum(MR_DATUM);
          char PressAltStr[5];
          sprintf(PressAltStr,"%5.0f", displayPalt);
          gdraw.drawString(PressAltStr,309,18);


          // Update AOA numeric display
          //gdraw.setFreeFont(FSSB18);
          gdraw.setTextColor (TFT_WHITE);
          gdraw.setCursor(269, 200);
          gdraw.printf ("%02d", displayPercentLift);                    
          

          // Update ball display on attitude page
          // Increase sensitivity of slip indicator
          drawSlip(80, 204, 160, 20, Slip, false, AOAThresholds);

          // iVSI
          // draw iVSI line
          if (iVSI!=0.0)
             {              
             int vsiHeight=abs(int(iVSI*120/600));
             vsiHeight=constrain(vsiHeight,0,120);
             int vsiTop;
             if (iVSI>0) vsiTop=119-vsiHeight;
                 else
                      vsiTop=119;
             gdraw.fillRect(313,vsiTop,7,vsiHeight,TFT_ORANGE);
             }
          // vsi ladder, every 20 pixels
          for (int i=19;i<220;i=i+20)
              {
              gdraw.drawLine (313, i, 319, i, TFT_BLACK);
              }
          //zero line    
          gdraw.drawLine (306, 118, 312, 118, TFT_BLACK);
          gdraw.drawLine (306, 119, 312, 119, TFT_BLACK);
          gdraw.drawLine (306, 120, 312, 120, TFT_BLACK);
          
          break;       
      }
      
      case 2: {     // 2 narrow AOA and slip indicator
          wgtWidth = 102;
          wgtHeight = 192;
          wgtX0 = (WIDTH - wgtWidth) / 2; wgtY0 = 0;
		      numericDisplay = false;
          displayAOA();
          break;
        }

      case 3: {     // decel gauge          
          displayDecelGauge();
          break;
        }
      case 4: {
            displayGloadHistory();
            break;
        }
      default: break;
    }

      
	  // Look for serial link failure
        // Draw red lines across display
     if (millis()-serialMillis>300)
        {   
            gdraw.fillSprite (TFT_BLACK);
            gdraw.drawLine (0, 0, 319, 239, TFT_RED); // center
           
            gdraw.drawLine (0, 1, 318, 239, TFT_RED); // left            
            gdraw.drawLine (0, 2, 317, 239, TFT_RED); 
            gdraw.drawLine (0, 3, 316, 239, TFT_RED);      
            gdraw.drawLine (0, 4, 315, 239, TFT_RED);          

            gdraw.drawLine (1, 0, 319, 238, TFT_RED); // right
            gdraw.drawLine (2, 0, 319, 237, TFT_RED);
            gdraw.drawLine (3, 0, 319, 236, TFT_RED);
            gdraw.drawLine (4, 0, 319, 235, TFT_RED);         
            
            gdraw.drawLine (0, 239, 319, 0, TFT_RED); // center
            
            gdraw.drawLine (0, 238, 318, 0, TFT_RED); // left
            gdraw.drawLine (0, 237, 317, 0, TFT_RED);
            gdraw.drawLine (0, 236, 316, 0, TFT_RED);
            gdraw.drawLine (0, 235, 315, 0, TFT_RED);            
            
            gdraw.drawLine (1, 239, 319, 1, TFT_RED); // right
            gdraw.drawLine (2, 239, 319, 2, TFT_RED);
            gdraw.drawLine (3, 239, 319, 3, TFT_RED);
            gdraw.drawLine (4, 239, 319, 4, TFT_RED);

            gdraw.setFreeFont(FSSB18);
            gdraw.setTextColor (TFT_WHITE);
            gdraw.setTextDatum(MC_DATUM);
            gdraw.fillRect(100,100,120,40,TFT_BLACK);
            gdraw.drawString("NO DATA",160,120);
            
            gdraw.pushSprite (0, 0);
            gdraw.deleteSprite();
            return;
        }
      
      }

    if (millis()-flashTime>=flashRate)
        {
        flashFlag = !flashFlag;
        flashTime=millis();
        }
    gdraw.pushSprite (0, 0);
    gdraw.deleteSprite();
  }

void displayAOA()
      {
      // update AOA display
      // build Setpoint array
      AOAThresholds[0]=0.0001;
      AOAThresholds[1]=OnSpeedTonesOnAOA-0.1;
      AOAThresholds[2]=OnSpeedTonesOnAOA;
      AOAThresholds[3]=OnSpeedFastAOA;
      AOAThresholds[4]=OnSpeedSlowAOA;
      AOAThresholds[5]=OnSpeedSlowAOA+0.1;
      AOAThresholds[6]=OnSpeedStallWarnAOA-0.1;
      AOAThresholds[7]=OnSpeedStallWarnAOA;      
      
      drawAOA(wgtX0, wgtY0, wgtWidth, wgtHeight, SmoothedAOA, flashFlag, AOAThresholds);

      gdraw.setFreeFont(FSSB18);
      gdraw.setTextColor (TFT_WHITE);
      gdraw.setCursor(140, 27);
      gdraw.printf ("%02d", displayPercentLift);

      if (numericDisplay) {
        // Update airspeed numeric display
        gdraw.setFreeFont(FSS18);
        
        gdraw.setCursor(5, 90);
        gdraw.setTextColor (TFT_GREEN);
        gdraw.print ("IAS ");
        gdraw.setCursor(278, 90);
        gdraw.setTextColor (TFT_GREEN);
        gdraw.print("G");

        
        gdraw.setFreeFont(FSSB18);
        // update IAS numeric display
        gdraw.setTextColor (TFT_WHITE);
        gdraw.setCursor(7, 130);
        gdraw.print (int(displayIAS));

        // Update G-force numeric display
        gdraw.setFreeFont(FSSB18);
        gdraw.setTextColor (TFT_WHITE);
        //gdraw.setCursor(235, 130);
        //gdraw.printf ("%+1.1f", displayVerticalG);
        char GStr[5];
        sprintf(GStr,"%+1.1f", displayVerticalG);
        gdraw.setTextDatum(MR_DATUM);
        gdraw.drawString(GStr,305,118);

        // flaps
        gdraw.fillCircle (23, 204, 16, TFT_GREY);       
        //top, bottom, right
        int cX=23;
        int cY=204;
        int Radius=16;
        int triangleTopX=int(cX+sin(FlapPos*PI/180)*Radius);
        int triangleTopY=int(cY-cos(FlapPos*PI/180)*Radius);
        int triangleBottomX=int(cX-sin(FlapPos*PI/180)*Radius);
        int triangleBottomY=int(cY+cos(FlapPos*PI/180)*Radius);
        int triangleRightX=int(cX+cos(FlapPos*PI/180)*(Radius+33));
        int triangleRightY=int(cY+sin(FlapPos*PI/180)*(Radius+33));
        gdraw.fillTriangle(triangleTopX, triangleTopY, triangleBottomX, triangleBottomY, triangleRightX, triangleRightY, TFT_GREY);
        gdraw.drawPixel(triangleRightX, triangleRightY, TFT_BLACK); // blunt the flap tip 1 pixel
        //gdraw.fillCircle (23, 204, 14, TFT_BLACK);

        // show flap stops
        gdraw.drawPixel(72, 204, TFT_WHITE);
        gdraw.drawPixel(71, 212, TFT_WHITE);
        gdraw.drawPixel(69, 220, TFT_WHITE);
        gdraw.drawPixel(65, 228, TFT_WHITE);
        gdraw.drawPixel(60, 235, TFT_WHITE);
      
        // show numeric flap angle
        gdraw.setFreeFont(FSS12);
          gdraw.setTextColor (TFT_WHITE);
          gdraw.setTextDatum(MC_DATUM);
          char FlapsChar[2];
          sprintf(FlapsChar,"%i", FlapPos);
          gdraw.drawString(FlapsChar,cX,cY);                
      }

      // Update ball display

      drawSlip(80, 204, 160, 34, Slip, flashFlag, AOAThresholds);

      // Update gOnset rates
          // draw gOnset line
          if (gOnsetRate!=0.0)
             {              
             int gOnsetHeight=abs(int(gOnsetRate*120/2));
             gOnsetHeight=constrain(gOnsetHeight,0,120);
             int gOnsetTop;
             if (gOnsetRate>0) gOnsetTop=119-gOnsetHeight;
                 else
                      gOnsetTop=119;
             gdraw.fillRect(313,gOnsetTop,7,gOnsetHeight,TFT_YELLOW);
             }
          // vsi ladder, every 20 pixels
          for (int i=15;i<226;i=i+15)
              {
              gdraw.drawLine (313, i, 319, i, TFT_GREY);
              }
          //zero line    
          gdraw.drawLine (306, 118, 312, 118, TFT_GREY);
          gdraw.drawLine (306, 119, 312, 119, TFT_GREY);
          gdraw.drawLine (306, 120, 312, 120, TFT_GREY);
      

      }


//
// Draw AOA indicator
//
void drawAOA(uint16_t X0, uint16_t Y0, uint16_t W, uint16_t H, float AOA, boolean flashFlag, float Array[]) {
  X0 = X0 + W / 2; Y0 = Y0 + H / 2; // Adjust datum to center of widget

  uint16_t Colour = TFT_DARKGREY;
  gdraw.drawRoundRect (X0 - W / 2 , Y0 - H / 2 , W , H, 5, Colour); // Gauge bounding box
  gdraw.drawRoundRect (X0 - W / 2 + 1 , Y0 - H / 2 + 1 , W - 2 , H - 2, 5, Colour); // Gauge bounding box

  int16_t Px0 = -W / 12, Py0 = -H / 4;
  int16_t Px1 = +W / 12, Py1 = H / 4 ;



  /*
     Top red chevron
  */
  Colour = TFT_DARKGREY;  
  if (AOA > Array[4]) Colour = TFT_RED;
  if (flashFlag && AOA >= Array [7]) Colour = TFT_DARKGREY;

  float Theta = PI / 8, cosTheta = cos(Theta), sinTheta = sin(Theta);

  int16_t XA0 = (Px0 * cosTheta - Py0 * sinTheta) + X0 + W / 4;
  int16_t YA0 = (Px0 * sinTheta + Py0 * cosTheta) + Y0 - H / 4;

  int16_t XA1 = (Px1 * cosTheta - Py0 * sinTheta) + X0 + W / 4;
  int16_t YA1 = (Px1 * sinTheta + Py0 * cosTheta) + Y0 - H / 4;

  int16_t XA2 = (Px1 * cosTheta - Py1 * sinTheta) + X0 + W / 4;
  int16_t YA2 = (Px1 * sinTheta + Py1 * cosTheta) + Y0 - H / 4;

  int16_t XA3 = (Px0 * cosTheta - Py1 * sinTheta) + X0 + W / 4;
  int16_t YA3 = (Px0 * sinTheta + Py1 * cosTheta) + Y0 - H / 4;


  gdraw.fillTriangle(XA0, YA0, XA1, YA1, XA3, YA3, Colour);
  gdraw.fillTriangle(XA1, YA1, XA2, YA2, XA3, YA3, Colour);

  Theta = -PI / 8, cosTheta = cos(Theta), sinTheta = sin(Theta);

  XA0 = (Px0 * cosTheta - Py0 * sinTheta) + X0 - W / 4;
  YA0 = (Px0 * sinTheta + Py0 * cosTheta) + Y0 - H / 4;

  XA1 = (Px1 * cosTheta - Py0 * sinTheta) + X0 - W / 4;
  YA1 = (Px1 * sinTheta + Py0 * cosTheta) + Y0 - H / 4;

  XA2 = (Px1 * cosTheta - Py1 * sinTheta) + X0 - W / 4;
  YA2 = (Px1 * sinTheta + Py1 * cosTheta) + Y0 - H / 4;

  XA3 = (Px0 * cosTheta - Py1 * sinTheta) + X0 - W / 4;
  YA3 = (Px0 * sinTheta + Py1 * cosTheta) + Y0 - H / 4;

  gdraw.fillTriangle(XA0, YA0, XA1, YA1, XA3, YA3, Colour);
  gdraw.fillTriangle(XA1, YA1, XA2, YA2, XA3, YA3, Colour);

  /*
     Bottom chevron
  */
  Colour = TFT_DARKGREY;
  if (AOA >= Array [1] ) Colour = TFT_ORANGE;

  Theta = PI / 8, cosTheta = cos(Theta), sinTheta = sin(Theta);

  XA0 = (Px0 * cosTheta - Py0 * sinTheta) + X0 - W / 4;
  YA0 = (Px0 * sinTheta + Py0 * cosTheta) + Y0 + H / 4;

  XA1 = (Px1 * cosTheta - Py0 * sinTheta) + X0 - W / 4;
  YA1 = (Px1 * sinTheta + Py0 * cosTheta) + Y0 + H / 4;

  XA2 = (Px1 * cosTheta - Py1 * sinTheta) + X0 - W / 4;
  YA2 = (Px1 * sinTheta + Py1 * cosTheta) + Y0 + H / 4;

  XA3 = (Px0 * cosTheta - Py1 * sinTheta) + X0 - W / 4;
  YA3 = (Px0 * sinTheta + Py1 * cosTheta) + Y0 + H / 4;

  gdraw.fillTriangle(XA0, YA0, XA1, YA1, XA3, YA3, Colour);
  gdraw.fillTriangle(XA1, YA1, XA2, YA2, XA3, YA3, Colour);

  Theta = -PI / 8, cosTheta = cos(Theta), sinTheta = sin(Theta);

  XA0 = (Px0 * cosTheta - Py0 * sinTheta) + X0 + W / 4;
  YA0 = (Px0 * sinTheta + Py0 * cosTheta) + Y0 + H / 4;

  XA1 = (Px1 * cosTheta - Py0 * sinTheta) + X0 + W / 4;
  YA1 = (Px1 * sinTheta + Py0 * cosTheta) + Y0 + H / 4;

  XA2 = (Px1 * cosTheta - Py1 * sinTheta) + X0 + W / 4;
  YA2 = (Px1 * sinTheta + Py1 * cosTheta) + Y0 + H / 4;

  XA3 = (Px0 * cosTheta - Py1 * sinTheta) + X0 + W / 4;
  YA3 = (Px0 * sinTheta + Py1 * cosTheta) + Y0 + H / 4;

  gdraw.fillTriangle(XA0, YA0, XA1, YA1, XA3, YA3, Colour);
  gdraw.fillTriangle(XA1, YA1, XA2, YA2, XA3, YA3, Colour);


  /*
     Draw black surround for inner circles
  */
  uint16_t bullsEye = H * (65 - 55-2) / 200;
  gdraw.fillCircle (X0, Y0, bullsEye + H / 12, TFT_BLACK);

  /*
     Draw green circle.
  */
  Colour = TFT_DARKGREY;
  if (AOA >= Array[3]) Colour = TFT_GREEN;
  gdraw.fillCircle (X0, Y0, bullsEye + H / 16, Colour);

  /*
     Draw small black surround and small circle
  */
  gdraw.fillCircle (X0, Y0, bullsEye + 5, TFT_BLACK);
  gdraw.fillRect (X0 - W / 3, Y0 - H / 48, 2 * W / 3, H / 24, TFT_BLACK);



  /*
    OnSpeed indication
  */
  Colour = TFT_BLACK;
  if (AOA >= Array [3] && AOA <= Array [4] ) Colour = TFT_GREEN;
  gdraw.fillCircle (X0, Y0, bullsEye + 2, Colour);


 
  /*
    Index pointer
  */
  Colour = TFT_WHITE;
  gdraw.fillRect (X0 - W / 2, mapAOA2Display(AOA, Array), W, H / 24, Colour);
  gdraw.drawRect (X0 - W / 2, mapAOA2Display(AOA, Array), W, H / 24, TFT_BLACK);

  
  /*
     Draw marker dots for maximum climb rate
  */
  gdraw.fillCircle (X0 - W / 2, (HEIGHT - 39 * HEIGHT / 100), H / 24, TFT_BLACK);
  gdraw.fillCircle (X0 + W / 2 - 1, (HEIGHT - 39 * HEIGHT / 100), H / 24, TFT_BLACK);
  gdraw.fillCircle (X0 - W / 2, (HEIGHT - 39 * HEIGHT / 100), H / 32, TFT_WHITE);
  gdraw.fillCircle (X0 + W / 2 - 1, (HEIGHT - 39 * HEIGHT / 100), H / 32, TFT_WHITE);
  }

/*
   Draw slip indicator
*/
void drawSlip (uint16_t X0, uint16_t Y0, uint16_t W, uint16_t H,  int16_t Slip, boolean flashFlag,  float Array[]) {
  // drawSlip(80, 215, 160, 20, Slip, false, AOAThresholds);
  uint16_t CenterX = X0 + W / 2;
  uint16_t CenterY = Y0 + H / 2;
  /*
     Add ball graphic
  */
  uint16_t Colour = TFT_GREEN;
  if (flashFlag && (abs(Slip) >= 30)  && AOA >= Array[7]) Colour = TFT_BLACK;
  if (!flashFlag && (abs(Slip) >= 30) && AOA >= Array[7]) Colour = TFT_RED;

  gdraw.fillCircle (CenterX + Slip * (W - H - 1) / 99 / 2, CenterY, H / 2 - 1, Colour);

  /*
     Draw slip indicator tick marks in foreground
  */
  gdraw.fillRect (CenterX - H / 2 - 9, Y0, 10, H, TFT_BLACK);
  gdraw.fillRect (CenterX - H / 2 - 7, Y0, 6, H, TFT_WHITE);
  gdraw.fillRect (CenterX + H / 2, Y0, 10, H, TFT_BLACK);
  gdraw.fillRect (CenterX + H / 2 + 2, Y0, 6, H, TFT_WHITE);
}

void AiGraph (int16_t px0, int16_t py0, int16_t arcSize, int16_t arcWidth, int16_t maxDisplay, int16_t minDisplay,
              int16_t startAngle, int16_t arcAngle, bool clockWise, uint8_t gradMarks,
              int16_t pitch, int16_t roll, int16_t yaw, float flightPathAngle) {        
  
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

    gdraw.fillSprite (TFT_CYAN);
    arcSize = 160;
   
  /*
  Establish a wide horizontal baseline segment
  */ 
    float xRotate = (2.0f * (float)WIDTH) * cos (roll * DEG_TO_RAD); 
    float yRotate = (2.0f * (float)WIDTH) * sin (roll * DEG_TO_RAD); 

  /*
  Adjust it for roll and pitch
  */
    float pxc = px0 + pitch * HEIGHT/80 * sin (roll * DEG_TO_RAD ); 
    float pyc = py0 + pitch * HEIGHT/80 * cos (roll * DEG_TO_RAD ); 

    float px1 = pxc - xRotate;
    float py1 = pyc + yRotate;;;
    
    float px2 = pxc + xRotate;
    float py2 = pyc - yRotate;

  /*  
   Compute offset parallel line segment to establish a wide bar
  */
    float px3 = px1 + 3 * HEIGHT * cos(roll * DEG_TO_RAD - HALF_PI);
    float py3 = py1 - 3 * HEIGHT * sin(-roll * DEG_TO_RAD - HALF_PI);
                      
    float px4 = px2 - 3 * HEIGHT * cos(roll * DEG_TO_RAD + HALF_PI);
    float py4 = py2 + 3 * HEIGHT * sin(roll * DEG_TO_RAD + HALF_PI);
 
/*
 Fill the bar.  Will be automatically clipped outside of screen bounds
*/ 
    
    gdraw.fillTriangle (px1, py1, px2, py2, px3, py3, 0x8281); // brown color for ground
    gdraw.fillTriangle (px3, py3, px4, py4, px2, py2, 0x8281);
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
  
  gdraw.fillCircle (px0, py0, 2 * HEIGHT/80, TFT_YELLOW);   // 2 degree radius circle
  gdraw.drawCircle (px0, py0, 2 * HEIGHT/80, TFT_BLACK);  
  
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

  /*
  Draw FlightPath marker
  */
   // 120 -screen center
  int fpY=120-(FlightPath-Pitch)*120/40; // 40 degrees of pitch per half screen height
  //if (fpY<0) fpY=0;
  //if (fpY>239) fpY=239;
  fpY=constrain(fpY,0,239);
  int fpX=159; // screen center
  // circle
  gdraw.drawCircle (fpX, fpY, 12, TFT_MAGENTA);
  gdraw.drawCircle (fpX, fpY, 13, TFT_MAGENTA);
  gdraw.drawCircle (fpX, fpY, 14, TFT_MAGENTA);
  // left line
  gdraw.drawLine (fpX-33, fpY-1, fpX-14, fpY-1, TFT_MAGENTA);
  gdraw.drawLine (fpX-33, fpY, fpX-14, fpY, TFT_MAGENTA);
  gdraw.drawLine (fpX-33, fpY+1, fpX-14, fpY+1, TFT_MAGENTA);

  // right line
  gdraw.drawLine (fpX+33, fpY-1, fpX+14, fpY-1, TFT_MAGENTA);
  gdraw.drawLine (fpX+33, fpY, fpX+14, fpY, TFT_MAGENTA);
  gdraw.drawLine (fpX+33, fpY+1, fpX+14, fpY+1, TFT_MAGENTA);

  // top line
  gdraw.drawLine (fpX-1, fpY-14, fpX-1, fpY-33, TFT_MAGENTA);
  gdraw.drawLine (fpX, fpY-14, fpX, fpY-33, TFT_MAGENTA);
  gdraw.drawLine (fpX+1, fpY-14, fpX+1, fpY-33, TFT_MAGENTA);

  
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
  
  float pxc = px0 + pitch * HEIGHT/80  * sin (roll * DEG_TO_RAD ); 
  float pyc = py0 + pitch * HEIGHT/80 * cos (roll * DEG_TO_RAD ); 

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
     
    px3 = px1 - (i * HEIGHT/80) * cos(roll * DEG_TO_RAD - HALF_PI);
    py3 = py1 + (i * HEIGHT/80) * sin(-roll * DEG_TO_RAD - HALF_PI);
                         
    px4 = px2 + (i * HEIGHT/80) * cos(roll * DEG_TO_RAD + HALF_PI);
    py4 = py2 - (i * HEIGHT/80) * sin(roll * DEG_TO_RAD + HALF_PI);

    gdraw.drawLine (px3, py3, px4, py4, TFT_BLACK); 
}

    px1 = pxc - xRotate*2.0f; 
    py1 = pyc + yRotate*2.0f;  
                         
    px2 = pxc + xRotate*2.0f;
    py2 = pyc - yRotate*2.0f; 

  for (int16_t i = -90; i <= 90; i+= scale){

// Marks every 5 degrees
     
    px3 = px1 - (i * HEIGHT/80) * cos(roll * DEG_TO_RAD - HALF_PI);
    py3 = py1 + (i * HEIGHT/80) * sin(-roll * DEG_TO_RAD - HALF_PI);
                         
    px4 = px2 + (i * HEIGHT/80) * cos(roll * DEG_TO_RAD + HALF_PI);
    py4 = py2 - (i * HEIGHT/80) * sin(roll * DEG_TO_RAD + HALF_PI);

    gdraw.setCursor(px4, py4);
    gdraw.drawLine (px3, py3, px4, py4, TFT_BLACK); 
    
    px4 += xRotate*0.75f; 
    py4 -= yRotate*0.75f; 
    //myGauges.printNum ("123456789", 160, 120, 8, 12, roll, TFT_BLACK, MR_DATUM);
    myGauges.printNum (String (i)+"o", px4, py4, 8, 12, roll, TFT_BLACK, ML_DATUM);
  }    
} // end pitchGraph

void displayDecelGauge()
{  
  // draw gauge background 
  gdraw.fillRoundRect(109,1,102,210,5,TFT_RED);
  gdraw.fillRect(109,87,102,36,TFT_GREEN);  
  gdraw.drawRoundRect(109,1,102,210,5,TFT_LIGHT_GREY);

  int decelIndex=int(35.143*SmoothedDecelRate+141.48-3.5); // 3.5 is half the indexer pointer height
  decelIndex=constrain(decelIndex,2,205);
  
  // draw index pointer
  gdraw.fillRect (109, decelIndex, 102, 7, TFT_WHITE);
  gdraw.drawRect (109, decelIndex, 102, 7, TFT_BLACK);

  // gauge numbers
  gdraw.setFreeFont(FSS9);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum(MR_DATUM);
  gdraw.drawString("-1",95,106);
  gdraw.drawString("-2",95,72);
  gdraw.drawString("-3",95,36);
  gdraw.drawString("0",95,141);
  gdraw.drawString("1",95,177);

  // pips
  gdraw.drawLine (99, 106, 107, 106, TFT_LIGHT_GREY);
  gdraw.drawLine (99, 72, 107, 72, TFT_LIGHT_GREY);
  gdraw.drawLine (99, 36, 107, 36, TFT_LIGHT_GREY);
  gdraw.drawLine (99, 141, 107, 141, TFT_LIGHT_GREY);
  gdraw.drawLine (99, 177, 107, 177, TFT_LIGHT_GREY);
    
  // iVSI
          // draw iVSI line
          if (iVSI!=0.0)
             {              
             int vsiHeight=abs(int(iVSI*120/600));
             vsiHeight=constrain(vsiHeight,0,120);
             int vsiTop;
             if (iVSI>0) vsiTop=119-vsiHeight;
                 else
                      vsiTop=119;
             gdraw.fillRect(313,vsiTop,7,vsiHeight,TFT_ORANGE);
             }
          // vsi ladder, every 20 pixels
          for (int i=19;i<220;i=i+20)
              {
              gdraw.drawLine (313, i, 319, i, TFT_LIGHT_GREY);
              }
          //zero line    
          gdraw.drawLine (306, 118, 312, 118, TFT_LIGHT_GREY);
          gdraw.drawLine (306, 119, 312, 119, TFT_LIGHT_GREY);
          gdraw.drawLine (306, 120, 312, 120, TFT_LIGHT_GREY);
  
  // Update ball display
  drawSlip(80, 215, 160, 20, Slip, false, AOAThresholds);

  // Update airspeed numeric display
  gdraw.setFreeFont(FSS18);        
  gdraw.setCursor(5, 90);
  gdraw.setTextColor (TFT_GREEN);
  gdraw.print ("IAS");
  gdraw.setTextColor (TFT_GREEN);
  gdraw.setTextDatum(TR_DATUM);
  gdraw.drawString("Kt/s",305,65);

  
  gdraw.setFreeFont(FSSB18);
  // update IAS numeric display
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setCursor(7, 130);
  gdraw.print (int(displayIAS));

  // Update G-force numeric display
  gdraw.setFreeFont(FSSB18);
  gdraw.setTextColor (TFT_WHITE);
  char DecelStr[5];
  sprintf(DecelStr,"%+1.1f", displayDecelRate);
  gdraw.setTextDatum(MR_DATUM);
  gdraw.drawString(DecelStr,305,118);
  
}

void displayGloadHistory()
{

// 1G line
gdraw.drawLine(19,133,319,133,TFT_WHITE);

//vertical line
gdraw.drawLine(19,0,19,239,TFT_WHITE);

gdraw.drawLine(19,27,319,27,TFT_GREY);
gdraw.drawLine(19,53,319,53,TFT_GREY);
gdraw.drawLine(19,80,319,80,TFT_GREY);
gdraw.drawLine(19,106,319,106,TFT_GREY);

gdraw.drawLine(19,160,319,160,TFT_GREY);
gdraw.drawLine(19,186,319,186,TFT_GREY);
gdraw.drawLine(19,213,319,213,TFT_GREY);

// pips
  gdraw.setFreeFont(FSS12);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.setTextDatum(MR_DATUM);
  gdraw.drawString("5",5,27);
  gdraw.drawString("4",5,53);
  gdraw.drawString("3",5,80);
  gdraw.drawString("2",5,106);
  gdraw.drawString("1",5,133);
  gdraw.drawString("0",5,160);
  gdraw.drawString("-1",3,186);
  gdraw.drawString("-2",3,213);

  gdraw.setFreeFont(FSS12);
  gdraw.setTextDatum(MC_DATUM);
  gdraw.drawString("G-LOAD [1 min]",160,8);

// draw gHistory

int gDisplayIndex=gHistoryIndex;
uint16_t gColor;
for (int i=319;i>19;i--)
    {
    int gHeight=160-int(gHistory[gDisplayIndex]*26.67);
    gHeight=constrain(gHeight,0,239);
    
    if (gHistory[gDisplayIndex]>=1) gColor=TFT_GREEN; else if (gHistory[gDisplayIndex]<1 && gHistory[gDisplayIndex]>=0) gColor=TFT_YELLOW; else gColor=TFT_RED;
   
    gdraw.fillCircle (i, gHeight, 2, gColor);
    
    if (gDisplayIndex<299) gDisplayIndex++; else gDisplayIndex=0;
    }
}

int mapAOA2Display(float AOA, float Array[])
{

if (AOA<=Array[0]) return 192; // display bottom
   else
       if (AOA>Array[0] && AOA<=Array[2]) return map2int(AOA,Array[0],Array[2],192,148); // display bottom, L/Dmax
           else
                 if (AOA>Array[2] && AOA<=Array[3]) return map2int(AOA,Array[2],Array[3],148,115); // L/Dmax, onspeed fast
                    else
                          if (AOA>Array[3] && AOA<=Array[4]) return map2int(AOA,Array[3],Array[4],115,78); // onspeed fast, onspeed slow
                              else
                                    if (AOA>Array[4] && AOA<=Array[7]) return map2int(AOA,Array[4],Array[7],78,1);
                                        else return 1; // top of gauge
//     AOAThresholds[0]=0
//      AOAThresholds[2]=OnSpeedTonesOnAOA;
//      AOAThresholds[3]=OnSpeedFastAOA;
//      AOAThresholds[4]=OnSpeedSlowAOA;
//      AOAThresholds[7]=OnSpeedStallWarnAOA;
}

int map2int(float AOA, float inLow, float inHigh,int outLow,int outHigh)
{
int Result;
Result=round((float)(AOA - inLow) * (outHigh - outLow) / (float)(inHigh - inLow) + outLow);
return Result;
}


void handleUpgrade()
  {
  String page="";
  page+="<html><body><br><br><p>Upgrade display firmware via binary (.bin) file upload\
  <br><br>Note: Please make sure you are uploading the M5 .bin file not the PicoKit .bin!<br><br>";
  page+="<form method='POST' action='/upload' enctype='multipart/form-data' id='upload_form'>\
   <input type='file' name='update'><br><br>\
   <input class='redbutton' type='submit' value='Upload'>\
    </form></body></html>";
  
  server.send(200, "text/html", page); 
  }

void handleUpgradeSuccess()
  {
  String page="";
  page+="<html><body><br><br><br><br><span style=\"color:black\">Firmware upgrade complete. Wait a few seconds until the display reboots.</span></p><br><br><br><br>";
  page+="<script>setInterval(function () { document.getElementById('rebootprogress').value+=0.1; document.getElementById('rebootprogress').innerHTML=document.getElementById('rebootprogress').value+'%'}, 10);setTimeout(function () { window.location.href = \"/\";}, 10000);</script>";
  page+="<div align=\"center\"><progress id=\"rebootprogress\" max=\"100\" value=\"0\"> 0% </progress></div></body></html>";  
  server.send(200, "text/html", page); 
  }
   
void handleUpgradeFailure()
  {
  String page="";
  page+="<html><body><br><br><br><br><span style=\"color:red\">Firmware upgrade failed. Power cycle the display and try again.</span></p><br><br><br><br></body></html>";
  server.send(200, "text/html", page);
  }

void handleIndex()
  {
  String page="";
  page+="<html><body><br><br><br><br><span style=\"color:black\"><b>OnSpeed M5 Display Upgrade server</b></span></p><br><br><a href=\"/upgrade\">Upgrade now</a><br><br></body></html>";
  server.send(200, "text/html", page);
  }

unsigned int checkSerial()
{
  gdraw.setColorDepth(8);
  gdraw.createSprite(WIDTH, HEIGHT);
  gdraw.fillSprite (TFT_BLACK);
  gdraw.setFreeFont(FSS12);
  gdraw.setTextDatum(MC_DATUM);
  gdraw.setTextColor (TFT_WHITE);
  gdraw.drawString("Looking for Serial data",160,120);
  gdraw.drawString("Please wait...",160,190);
  gdraw.pushSprite (0, 0);
  gdraw.deleteSprite();
  String serialString;
                              
  // TTL input (including v2 Onspeed with vern's power board)
  Serial2.begin (115200, SERIAL_8N1, 16, 17,false); //GPIO16 is RX, GPIO17 is TX  (Vern's power board)
  serialString=readSerialbytes();
  Serial2.end();
  if (serialString.indexOf("#1")>=0) return 1;
  
  // rs232 input via power board (including v3 Onspeed)
  Serial2.begin (115200, SERIAL_8N1, 16, 17,true); //GPIO16 is RX, GPIO17 is TX  (Vern's power board)
  serialString=readSerialbytes();
  Serial2.end();
  if (serialString.indexOf("#1")>=0) return 2;
  
  // simulator demo M5 with onspeed v3 on pin 9 TTL
  Serial2.begin (115200, SERIAL_8N1, 22, 17,false); //GPIO22 is RX, GPIO17 is TX  (straight M5)
  serialString=readSerialbytes();
  Serial2.end();
  if (serialString.indexOf("#1")>=0) return 3;
  
  return 0; 
}

String readSerialbytes()
{
String ResultString="";  
char inChar;
unsigned long readSerialTimeout=millis();
while (millis()-readSerialTimeout<5000 && ResultString.length()<200)
  {
  if (Serial2.available())
      {
      inChar=Serial2.read();
      ResultString+=inChar;
      }
  }
Serial.println(ResultString);  
return ResultString;
}
