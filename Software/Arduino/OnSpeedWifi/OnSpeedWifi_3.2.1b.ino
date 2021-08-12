
// updated 1/15/2021
// to be compiled on Arduino 1.8.13 and ESP32 Pico Kit board file (v1.0.5)


// fix for Arduino ESP32 on Mac/ Big Sur.
//https://github.com/espressif/arduino-esp32/issues/4408#issuecomment-726271514

// to install xcrun if missing (missing xcrun at: /Library/Developer/CommandLineTools/usr/bin/xcrun), run this in Terminal:
//xcode-select --install


////////////////////////////////////////////////////
// More details at
//      http://www.flyOnSpeed.org
//      and
//      https://github.com/flyonspeed/OnSpeed-Gen2/

/* OnSpeed Wifi - Wifi file server, config manager and debug display for ONSPEED Gen 2 v2,v3 boxes.
 */ 
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <Update.h>
#include <WebSocketsServer.h> // https://github.com/Links2004/arduinoWebSockets
#include <StreamString.h>
#include "html_header.h"
#include "html_decelgauge.h"
#include "javascript_calibration.h"
#include "sg_filter.h"

#define BAUDRATE_WIFI         115200
#define BAUDRATE_WIFI_HS      921600

String wifi_fw="3.2.1b"; // wifi firmware version

const char* ssid = "OnSpeed";
const char* password = "angleofattack";
String clientwifi_ssid="HangarWifi"; // currently not needed
String clientwifi_password="test"; // currently not needed

// initialize live display values
float AOA=0.0;
float PercentLift=0.0;
float IAS=0.0;
float GLoad=0.00;
float CRC;
long liveDataAge=0; // millisec
unsigned long liveDataStartTime=millis();
int serialBufferSize=0;
char serialBuffer[350];

// initialize websocket server (live data display)
WebSocketsServer webSocket = WebSocketsServer(81);

typedef struct
  {
  char filename[11];
  long filesize; 
  } filetype;

String pageHeader;
String pageFooter="</body></html>";
String uploadConfigString;
WebServer server(80);

// initialize config variables
#define MAX_AOA_CURVES    5 // maximum number of AOA curves (flap/gear positions)
#define MAX_CURVE_COEFF   4 // 4 coefficients=3rd degree polynomial function for calibration curves
String dataSource = "SENSORS";

String replayLogFileName = "";

typedef struct  {
  int Count;
  int Items[MAX_AOA_CURVES];
} intArray;

typedef struct  {
  int Count;
  float Items[MAX_AOA_CURVES];
} floatArray;

typedef struct  {
  float Items[MAX_CURVE_COEFF]; // 3rd degree polynomial
  uint8_t curveType; // 1 - polynomial, 2 - logarithmic (linear is 1 degree polynomial), 3 -exponential
} calibrationCurve;

//serial inputs
bool readBoom;
bool readEfisData;
bool casCurveEnabled;
String efisType;

// serial output
String serialOutFormat;
String serialOutPort;

//volume control
bool volumeControl;
int volumeHighAnalog;
int volumeLowAnalog;
int muteAudioUnderIAS;
int defaultVolume;
int volumePercent;
bool audio3D;
bool overgWarning;

// smoothing windows
int aoaSmoothing;
int pressureSmoothing;

intArray flapDegrees;
intArray flapPotPositions;

// calibration curves
calibrationCurve aoaCurve[MAX_AOA_CURVES]; // 3rd degree polynomial equation coefficents
calibrationCurve casCurve; // calibrated airspeed curve

//setpoints
floatArray flapLDMAXAOA;
floatArray flapONSPEEDFASTAOA;
floatArray flapONSPEEDSLOWAOA;
floatArray flapSTALLWARNAOA;

// accelerometer axis
String boxtopOrientation="";
String portsOrientation="";

// load factor limit
float loadLimitPositive;
float loadLimitNegative;

// vno chime
int vnoChimeInterval;
int Vno;
bool vnoChimeEnabled;

// sensor biases
int pFwdBias;
int p45Bias;
float pStaticBias;
float axBias;
float ayBias;
float azBias;
float gxBias;
float gyBias;
float gzBias;
float pitchBias;
float rollBias;

bool sdLoggingConfig;
bool sdLogging; //not used here

String teensyVersion="";


// calibration wizard variables
int acGrossWeight;
int acCurrentWeight;
int acVldmax;

#include <Onspeed-settingsFunctions.h> // library with setting functions


void updateHeader()
{
pageHeader=String(htmlHeader);
// rewrite firmware in the html header
if (teensyVersion=="") teensyVersion=getTeensyVersion();
pageHeader.replace("wifi_fw","Teensy: "+teensyVersion+", Wifi: "+wifi_fw);
// update wifi status in html header
if (WiFi.status() == WL_CONNECTED)
    {
    pageHeader.replace("wifi_status","");
    pageHeader.replace("wifi_network",clientwifi_ssid);
    }
    else
      {
      pageHeader.replace("wifi_status","offline");
      pageHeader.replace("wifi_network","Not Connected");
      }
}

void handleWifiReflash()
  {
  String page="";
  updateHeader();
  page+=pageHeader;
  page+="<br><br><p>Wifi reflash mode disables the Teensy's serial port to enable reflashing the Wifi chip via its microUSB port.\
  <br><br><span style=\"color:red\">This mode is now activated until reboot.</span></p>";
  page+=pageFooter;
  Serial.println("$WIFIREFLASH!");
  server.send(200, "text/html", page); 
  }


void handleSensorConfig()
  {
   String page="";
   float aircraftPitch=0.0;
   float aircraftRoll=0.0;
   float aircraftPalt=0;
   updateHeader();
   page+=pageHeader;
   
  if (server.arg("confirm").indexOf("yes")<0)
    {
    // display confirmation page 
     String configString=getConfigString(); // load current config

   if (configString.indexOf("</CONFIG>")<0)
                {
                // no config string
                page+="<br><br>Could not download configuration from main CPU<br><br>Reload this page to try again.";
                page+=pageFooter;
                server.send(200, "text/html", page);
                return;      
                }
      
     
     loadConfigFromString(configString);
     // get current pitch/roll/altitude from Teensy
    delay(200);
    Serial.flush();
    Serial.println("$PITCHROLLALT!");
    String responseString="";
    unsigned long starttime=millis();
      while (true)
      {
      if (Serial.available()>0) 
          {
           char inChar=Serial.read();
           responseString+=inChar;       
           starttime=millis();       
           if (responseString.indexOf("</RESPONSE>")>0)
                {
                aircraftPitch=getConfigValue(responseString,"PITCH").toFloat();
                aircraftRoll=getConfigValue(responseString,"ROLL").toFloat();
                aircraftPalt=getConfigValue(responseString,"PALT").toFloat();
                break;
                }
          }
      if ((millis()-starttime)>5000)
          {     
          page+="<br>config request timeout<br>";
          break;
          }
      }
     String currentConfig="PfwdBias: "+ String(pFwdBias)+"<br>";
     currentConfig+=" P45Bias: "+ String(p45Bias)+"<br>";
     currentConfig+=" gxBias: "+ String(gxBias)+"<br>";
     currentConfig+=" gyBias: "+ String(gyBias)+"<br>";
     currentConfig+=" gzBias: "+ String(gzBias)+"<br>";
     currentConfig+=" staticBias: "+ String(pStaticBias)+"<br>";
     currentConfig+=" pitchBias: "+ String(pitchBias)+"<br>";
     currentConfig+=" rollBias: "+ String(rollBias)+"<br>";
     currentConfig+=" measured Pitch: "+ String(aircraftPitch)+"<br>";
     currentConfig+=" corrected Pitch: "+ String(aircraftPitch+pitchBias)+"<br>";
     currentConfig+=" measured Roll: "+ String(aircraftRoll)+"<br>";
     currentConfig+=" corrected Roll: "+ String(aircraftRoll+rollBias);
    
    page+="<br><b>Current sensor calibration:</b><br><br>"+currentConfig+"<br>\
    <p style=\"color:black\">This procedure will calibrate the system\'s accelerometers, gyros and pressure sensors.<br><br>\
    <b>Requirements:</b><br><br>\
    1. Do this configuration in no wind condition. Preferably inside a closed hangar, no moving air<br>\
    2. Box orientation is set up properly in <a href=\"aoaconfig\">AOA Configuration</a><br>\
    3. If the aircraft is not in a level attitude enter the current pitch and roll angles below.<br>\
    4. Enter current pressure altitude in feet. (Set your altimeter to 29.92 inHg and read the altitude)\
    <br><br>\
    Calibration will take a few seconds.\
    </p>\
    <br>\
    <br>\
    <form action=\"sensorconfig\" method=\"GET\">\
    <label>Aircraft Pitch (degrees)</label>\
    <input class=\"inputField\" type=\"text\" name=\"aircraftPitch\" value=\""+ String(aircraftPitch+pitchBias)+"\">\
    <br><br>\
    <label>Aircraft Roll (degrees)</label>\
    <input class=\"inputField\" type=\"text\" name=\"aircraftRoll\" value=\""+ String(aircraftRoll+rollBias)+"\">\
    <br><br>\
    <label>Pressure Altitude (feet)</label>\
    <input class=\"inputField\" type=\"text\" name=\"aircraftPalt\" value=\""+ String(aircraftPalt)+"\">\
    <input type=\"hidden\" name=\"confirm\" value=\"yes\">\
    <br><br><br>\
    <button type=\"submit\" class=\"button\">Calibrate Sensors</button>\
    <a href=\"/\">Cancel</a>\
    </form>";
    } else 
          {
          // configure sensors
          if (server.arg("aircraftPitch")!="" && server.arg("aircraftRoll")!="" && server.arg("aircraftPalt")!="")
              {
              aircraftPitch=server.arg("aircraftPitch").toFloat();
              aircraftRoll=server.arg("aircraftRoll").toFloat();
              aircraftPalt=server.arg("aircraftPalt").toFloat();
              } else
                {
                aircraftPitch=0.0;
                aircraftRoll=0.0;
                aircraftPalt=0.0;
                }
                 
          Serial.printf("$SENSORCONFIG<AIRCRAFTPITCH>%.2f</AIRCRAFTPITCH><AIRCRAFTROLL>%.2f</AIRCRAFTROLL><AIRCRAFTPALT>%.2f</AIRCRAFTPALT>!",aircraftPitch,aircraftRoll,aircraftPalt);
          String configResponse;
          unsigned long configStartTime=millis();
           // wait for response
          
          while (true)
            {
            if (Serial.available())
                {
                configResponse=configResponse+char(Serial.read());                
                }
            if (configResponse.indexOf("</SENSORCONFIG>") >= 0)
              {
              page+="<br><br><p>Sensors are now calibrated.</p>";
              page+="<br>The new parameters are:<br><br>";
              configResponse=getConfigValue(configResponse,"SENSORCONFIG");
              page+=configResponse;
              break; 
              } else
                    if (configResponse.indexOf("</SENSORCONFIGERROR>") >= 0) 
                        {
                         page+="<br><br>"+getConfigValue(configResponse,"SENSORCONFIGERROR");
                         page+="<br><br><p>Error: Could not calibrate sensors. Please reboot and try again.</p>";
                         break;
                        }
            if (millis()-configStartTime>30000)
                {
                // format timeout
                page+="<br><br><p>Timeout calibrating sensors. Please reboot and try again.</p>";
                break;
                }
            } // while
          }     
                
   page+=pageFooter;
   server.send(200, "text/html", page);   
  }

void handleUpgrade()
  {
  String page="";
  updateHeader();
  page+=pageHeader;
  page+="<br><br><p>Upgrade Wifi module firmware via binary (.bin) file upload\
  <br><br><br>";
  page+="<form method='POST' action='/upload' enctype='multipart/form-data' id='upload_form'>\
   <input type='file' name='update'><br><br>\
   <input class='redbutton' type='submit' value='Upload'>\
    </form>";
  page+=pageFooter; 
  
  server.send(200, "text/html", page); 
  }

void handleUpgradeSuccess()
  {
  String page="";
  updateHeader();
  page+=pageHeader;
  page+="<br><br><br><br><span style=\"color:black\">Firmware upgrade complete. Wait a few seconds until OnSpeed reboots.</span></p><br><br><br><br>";
  page+="<script>setInterval(function () { document.getElementById('rebootprogress').value+=0.1; document.getElementById('rebootprogress').innerHTML=document.getElementById('rebootprogress').value+'%'}, 10);setTimeout(function () { window.location.href = \"/\";}, 10000);</script>";
  page+="<div align=\"center\"><progress id=\"rebootprogress\" max=\"100\" value=\"0\"> 0% </progress></div>";
  
  page+=pageFooter;   
  server.send(200, "text/html", page); 
  }
   
void handleUpgradeFailure()
  {
  String page="";
  updateHeader();
  page+=pageHeader;
  page+="<br><br><br><br><span style=\"color:red\">Firmware upgrade failed. Power cycle OnSpeed box and try again.</span></p><br><br><br><br>";
   page+=pageFooter;
  server.send(200, "text/html", page);
  }
  
void handleFavicon()
{ 
 server.send(404, "text/plain", "FileNotFound");
}

void handleLive()
  {
  String page="";
  updateHeader();
  page+=pageHeader;
  page+="<style>\n"
"html, body { margin:0; padding: 0 5px; overflow:hidden;min-height:100% }\n"
"#container {width:100%}  \n"
"svg {position:absolute;margin:0 auto; display: block;}\n"
"#datafields {position:relative; margin-top:5px;font-family: \"Arial, Helvetica, sans-serif\";font-size:16px; left:0px;}\n"
"#footer {display: block;;position:fixed; bottom:2px; width: 100%;font-family: \"Arial, Helvetica, sans-serif\";font-size:14px;}\n"
"#footer-warning {text-align: center;margin-left: -10px; width: 100%;font-family: \"Arial, Helvetica, sans-serif\";font-size:12px;background-color:white}\n"
"#status {display:flex;justify-content: space-between;padding-bottom:10px;}\n"
"#switch {margin-right: 20px;}  \n"
"#status-label {align-self: flex-end;}  \n"
"/*! XS */\n"
"@media (orientation: portrait) {\n"
"  svg {  margin: 0 auto 0 0px; }\n"
"}\n"
"@media (orientation: landscape) {\n"
" /* svg { height: 60vh;}*/  \n"
"}\n"
"</style>\n"
"<script language=\"javascript\" type=\"text/javascript\">\n"
" var wsUri = \"ws://192.168.0.1:81\";\n"
" var lastUpdate= Date.now();\n"
" var lastDisplay=Date.now(); \n"
" var AOA=0;\n"
" var IAS=0;\n"
" var PAlt=0;\n"
" var GLoad=1;\n"
" var GLoadLat=0;\n"
" var PitchAngle=0;\n"
" var RollAngle=0;\n"
" var smoothingAlpha=.9;\n"
" var liveConnecting=false;\n"
"setInterval(updateAge,500); \n"
"\n"
"function init()\n"
"  {    \n"
"    writeToStatus(\"CONNECTING...\");\n"
"    connectWebSocket();\n"
"  }\n"
"\n"
"  function connectWebSocket()\n"
"  {\n"
"    liveConnecting=true\n"
"    websocket = new WebSocket(wsUri);\n"
"    websocket.onopen = function(evt) { onOpen(evt) };\n"
"    websocket.onclose = function(evt) { onClose(evt) };\n"
"    websocket.onmessage = function(evt) { onMessage(evt) };\n"
"    websocket.onerror = function(evt) { onError(evt) };\n"
"  }\n"
"\n"
"  function onOpen(evt)\n"
"  {\n"
"    writeToStatus(\"CONNECTED\");\n"
"    liveConnecting=false\n"
"  }\n"
"\n"
"  function onClose(evt)\n"
"  {\n"
"    writeToStatus(\"Reconnecting...\");    \n"
"    setTimeout(connectWebSocket, 1000);  \n"
"  }\n"
"  \n"
" function map(x, in_min, in_max, out_min, out_max)\n"
"{\n"
"if ((in_max - in_min) + out_min ==0) return 0;\n"
"  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;\n"
"}  \n"
" \n"
"  function onMessage(evt)\n"
"  {    \n"
"   // smoother values are display with the formula: value = measurement*alpha + previous value*(1-alpha)   \n"
"   try {\n"
"   console.log(evt.data);\n"
"   OnSpeedArray = evt.data.split(\",\");\n"
"   var OnSpeed = new Object();\n"
"   OnSpeed.AOA= OnSpeedArray[1]; \n"
"   OnSpeed.Pitch= OnSpeedArray[2]; \n"
"   OnSpeed.Roll= OnSpeedArray[3]; \n"
"   OnSpeed.IAS= OnSpeedArray[4]; \n"
"   OnSpeed.PAlt= OnSpeedArray[5]; \n"
"   OnSpeed.verticalGLoad= OnSpeedArray[6]; \n"
"   OnSpeed.lateralGLoad= OnSpeedArray[7]; \n"
"   OnSpeed.alphaVA= OnSpeedArray[8]; \n"
"   OnSpeed.LDmax= OnSpeedArray[9]; \n"
"   OnSpeed.OnspeedFast= OnSpeedArray[10]; \n"
"   OnSpeed.OnspeedSlow= OnSpeedArray[11]; \n"
"   OnSpeed.OnspeedWarn= OnSpeedArray[12]; \n"
"   OnSpeed.flapsPos= OnSpeedArray[13]; \n"
"   OnSpeed.coeffP= OnSpeedArray[14]; \n"
"   OnSpeed.dataMark= OnSpeedArray[15]; \n"
"   OnSpeed.CRC= OnSpeedArray[16]; \n"
"   var crc_string=OnSpeed.AOA+','+OnSpeed.Pitch+','+OnSpeed.Roll+','+OnSpeed.IAS+','+OnSpeed.PAlt+','+OnSpeed.verticalGLoad+','+OnSpeed.lateralGLoad+','+OnSpeed.alphaVA+','+OnSpeed.LDmax+','+OnSpeed.OnspeedFast+','+OnSpeed.OnspeedSlow+','+OnSpeed.OnspeedWarn+','+OnSpeed.flapsPos+','+OnSpeed.coeffP+','+OnSpeed.dataMark\n"
"   console.log(\"CRC\",crc_string);\n"
"   var crc_calc=0;\n"
"   for (i=0;i<crc_string.length;i++)\n"
"       {\n"
"       crc_calc+=crc_string.charCodeAt(i);\n"
"     crc_calc=crc_calc & 0xFF;\n"
"     }\n"
"    if (OnSpeed.CRC==crc_calc)\n"
"    {\n"
"    // CRC ok, use values\n"
"    AOA=(OnSpeed.AOA*smoothingAlpha+AOA*(1-smoothingAlpha)).toFixed(2);\n"
"    IAS=(OnSpeed.IAS*smoothingAlpha+IAS*(1-smoothingAlpha)).toFixed(2);\n"
"    PAlt=(OnSpeed.PAlt*smoothingAlpha+PAlt*(1-smoothingAlpha)).toFixed(2);\n"
"    GLoad=(OnSpeed.verticalGLoad*smoothingAlpha+GLoad*(1-smoothingAlpha)).toFixed(2);\n"
"    GLoadLat=(OnSpeed.lateralGLoad*smoothingAlpha+GLoadLat*(1-smoothingAlpha)).toFixed(2);\n"
"    PitchAngle=(OnSpeed.Pitch*smoothingAlpha+PitchAngle*(1-smoothingAlpha)).toFixed(2);\n"
"    RollAngle=(OnSpeed.Roll*smoothingAlpha+RollAngle*(1-smoothingAlpha)).toFixed(2);\n"
"    LDmax=parseFloat(OnSpeed.LDmax);\n"
"    OnspeedFast=parseFloat(OnSpeed.OnspeedFast);\n"
"    OnspeedSlow=parseFloat(OnSpeed.OnspeedSlow);\n"
"    OnspeedWarn=parseFloat(OnSpeed.OnspeedWarn);\n"
"    lastUpdate=Date.now();   \n"
"    //console.log('log:',AOA,IAS,PAlt,GLoad,GLoadLat,PitchAngle,OnSpeed.LDmax,OnSpeed.OnspeedFast,OnSpeed.OnspeedSlow,OnSpeed.OnspeedWarn);\n"
"    // move AOA line on display    \n"
"   if (AOA<=LDmax)\n"  
"     {\n"
"     var aoaline_y=map(AOA, 0, LDmax, 278, 228);     \n"
"     } else\n"
"     if (AOA>LDmax && AOA<=OnspeedFast)\n"
"       {\n"
"       aoaline_y=map(AOA, LDmax, OnspeedFast, 228, 178);\n"
"       } else\n"
"       if (AOA>OnspeedFast && AOA<=OnspeedSlow)\n"
"         {\n"
"         aoaline_y=map(AOA, OnspeedFast, OnspeedSlow, 178, 113);\n"
"         } else\n"
"             if (AOA>OnspeedSlow)\n"
"               {\n"
"               aoaline_y=map(AOA, OnspeedSlow,OnspeedWarn, 113, 15);\n"
"               console.log('aoaline',aoaline_y,AOA,OnspeedSlow,OnspeedWarn);\n"
"               }\n"
" \n"
"    document.getElementById(\"aoaline\").setAttribute(\"y\", aoaline_y);\n"
"// calc ldmax dot locations\n"
" ldmax_y=228+3;\n"
" document.getElementById(\"ldmaxleft\").setAttribute(\"cy\", ldmax_y);\n"
" document.getElementById(\"ldmaxright\").setAttribute(\"cy\", ldmax_y);\n"
"// show onspeed dot\n"
"// update attitude\n"
"updateAttitude(OnSpeed.Pitch,OnSpeed.Roll);\n"
" if (AOA>OnspeedFast && AOA<=OnspeedSlow) document.getElementById(\"onspeeddot\").style.visibility=\"visible\"; else document.getElementById(\"onspeeddot\").style.visibility=\"hidden\"; \n"
"   \n"
"}  else console.log('CRC error'); \n"
"    \n"
"   } catch (e) {\n"
"           console.log('JSON parsing error:'+e.name+': '+e.message);\n"
"           }         \n"
"   \n"
"   if (Date.now()-lastDisplay>=500)\n"
"       {\n"
"       if (AOA > -20) document.getElementById(\"aoa\").innerHTML=AOA; else document.getElementById(\"aoa\").innerHTML='N/A';\n"
"       document.getElementById(\"ias\").innerHTML=IAS +' kts';\n"
"       document.getElementById(\"palt\").innerHTML=PAlt+' ft';\n"
"       document.getElementById(\"gload\").innerHTML=GLoad+' G';\n"
"       document.getElementById(\"gloadLat\").innerHTML=GLoadLat+' G';\n"
"       document.getElementById(\"pitch\").innerHTML=PitchAngle+'&#176;';\n"
"       document.getElementById(\"roll\").innerHTML=RollAngle+'&#176;';\n"
"       document.getElementById(\"datamark\").innerHTML=OnSpeed.dataMark;\n"
"       document.getElementById(\"flapspos\").innerHTML=OnSpeed.flapsPos;\n"
"       lastDisplay=Date.now(); \n"
"   }\n"
"   \n"
"  updateAge();     \n"
"  }\n"
" \n"
" function onError(evt)\n"
"  {\n"
"    writeToStatus(evt.data);\n"
"  }  \n"
"\n"
"  function updateAge()\n"
"  {\n"
"  var age=((Date.now()-lastUpdate)/1000).toFixed(1);   \n"
"  if (document.getElementById(\"age\"))\n"
"    {\n"
"   document.getElementById(\"age\").innerHTML=age +' sec';\n"
"   if (age>=1)  document.getElementById(\"age\").style=\"color:red\"; else document.getElementById(\"age\").style=\"color:black\";         \n"
"   if (age>=3 && !liveConnecting) init();    \n"
"   }\n"
"  }\n"
" \n"
" function writeToStatus(message)\n"
"  {\n"
"    var status = document.getElementById(\"connectionstatus\");\n"
"    status.innerHTML = message;\n"
"  }  \n"
" function updateAttitude(pitch,roll) {\n"
"    roll=roll*-1;\n"
"    var p = (pitch * 2.50);\n"
"    var t = 'translate(50,50) rotate(' + roll + ') translate(0,' + p + ')' ;\n"
"    var id_roll = document.querySelector('#onspeed-attitude-pos');\n"
"    id_roll.setAttribute('transform',t);\n"
"  var id_dial = document.querySelector('#onspeed-attitude-dial');\n"
"  x = 50;\n"
"    y = 50;\n"
"    var t =  'translate(' + x + ',' + y + ')' + 'rotate(' + roll + ')';\n"
"    id_dial.setAttribute('transform',t);   \n"
"  }\n"
"\n"
"function toggleAOA(state)\n"
"  {\n"
"  console.log(state);\n"
"  if (state)\n"
"      {\n"
"      document.getElementById(\"aoaindexer\").style.visibility=\"visible\";\n"
"      document.getElementById(\"attitude\").style.visibility=\"hidden\";   \n"
"      } else\n"
"            {\n"
"            document.getElementById(\"aoaindexer\").style.visibility=\"hidden\";\n"
"            document.getElementById(\"attitude\").style.visibility=\"visible\";   \n"
"            }\n"
"}            \n"
"updateAttitude(0,0);\n"
"window.addEventListener(\"load\", function() {init();}, false);  \n"
"</script>\n"
"<div id=\"container\">\n"
"  <div id=\"aoaindexer\">\n"
"  <svg version=\"1.1\" viewBox=\"0 0 210 297\" width=\"100%\" style=\"height: 70vh\">\n"
" <g>\n"
"  <circle cx=\"98.171\" cy=\"787.76\" r=\"133.84\" fill=\"#07a33f\" stroke-width=\"1.8089\"/>\n"
"  <rect transform=\"rotate(-15.029)\" x=\"50.364\" y=\"33.747\" width=\"15.615\" height=\"89.228\" fill=\"#cc3837\"/>\n"
"  <rect transform=\"matrix(-.9658 -.2593 -.2593 .9658 0 0)\" x=\"-148.06\" y=\"-19.129\" width=\"15.615\" height=\"89.228\" fill=\"#cc3837\"/>\n"
"  <rect transform=\"matrix(.9658 .2593 .2593 -.9658 0 0)\" x=\"127.53\" y=\"-255.15\" width=\"15.615\" height=\"89.228\" fill=\"#f49421\"/>\n"
"  <rect transform=\"rotate(164.97)\" x=\"-70.153\" y=\"-307.83\" width=\"15.615\" height=\"89.228\" fill=\"#f49421\"/>\n"
"  <path d=\"m101.8 113.26c-17.123 0-31.526 12.231-34.92 28.385 5.203-2.6e-4 10.398 1.6e-4 15.598 0 2.9178-7.8426 10.401-13.371 19.323-13.371 8.922 0 16.413 5.5289 19.336 13.371 5.198 1.6e-4 10.403-2.8e-4 15.602 0-3.397-16.154-17.815-28.385-34.938-28.385zm-35.121 41.774c2.9176 16.746 17.577 29.609 35.121 29.609 17.544 0 32.218-12.863 35.138-29.609-5.1216-2.8e-4 -10.25 1.6e-4 -15.371 0-2.5708 8.4824-10.391 14.574-19.767 14.574-9.3764 0-17.183-6.0913-19.75-14.574-5.1246 1.4e-4 -10.244-2.8e-4 -15.371 0z\" color=\"#000000\" color-rendering=\"auto\" dominant-baseline=\"auto\" fill=\"#07a33f\" image-rendering=\"auto\" shape-rendering=\"auto\" solid-color=\"#000000\" style=\"font-feature-settings:normal;font-variant-alternates:normal;font-variant-caps:normal;font-variant-ligatures:normal;font-variant-numeric:normal;font-variant-position:normal;isolation:auto;mix-blend-mode:normal;paint-order:markers fill stroke;shape-padding:0;text-decoration-color:#000000;text-decoration-line:none;text-decoration-style:solid;text-indent:0;text-orientation:mixed;text-transform:none;white-space:normal\"/>\n"
"  <circle id=\"onspeeddot\" cx=\"101.8\" cy=\"148.91\" r=\"18\" fill=\"#07a33f\" stroke-width=\".81089\" visibility=\"hidden\"/>\n"
" </g>\n"
" <g fill=\"#241f1c\">\n"
"  <rect id=\"aoaline\" x=\"52.187\" y=\"144.91\" width=\"100\" height=\"6.6921\" style=\"paint-order:markers fill stroke\"/>\n"
"  <circle id=\"ldmaxleft\" cx=\"46.801\" cy=\"228\" r=\"3.346\" stroke-width=\"1.0419\"/>\n"
"  <circle id=\"ldmaxright\" cx=\"157.53\" cy=\"228\" r=\"3.346\" stroke-width=\"1.0419\"/>\n"
" </g>\n"
"</svg>\n"
"</div>\n"
"<div id=\"attitude\" style=\"visibility:hidden;\">\n"
"  <svg id=\"onspeed-attitude\" xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"300px\" viewBox=\"0 0 100 100\">\n"
"    <g id=\"onspeed-attitude-pos\" stroke-width=\"0.5\" stroke=\"#fff\" fill=\"#fff\"\n"
"       transform=\"translate(50,50) rotate(0)\"\n"
"       text-anchor='middle' font-family=\"sans-serif\" font-size=\"6\">\n"
"  \n"
"      <rect fill=\"#29B6F6\" stroke-width=\"0\" x=\"-100\" y=\"-200\" width=\"200\" height=\"200\"></rect>\n"
"      <rect fill=\"#8B4513\" stroke-width=\"0\" x=\"-100\" y=\"0\"  width=\"200\" height=\"200\"></rect>\n"
"  \n"
"      <!-- pitch up -->\n"
"      <line x1=\"-8\"  y1=\"-6.25\"   x2=\"8\"  y2=\"-6.25\"/>\n"
"      <line x1=\"-16\" y1=\"-12\"     x2=\"-4\" y2=\"-12\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"-10\">5</text>\n"
"      <line x1=\"4\"   y1=\"-12\"     x2=\"16\" y2=\"-12\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"-18.75\"  x2=\"8\"  y2=\"-18.75\"/>\n"
"      <line x1=\"-16\" y1=\"-25\"     x2=\"-4\" y2=\"-25\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"-23\">10</text>\n"
"      <line x1=\"4\"   y1=\"-25\"     x2=\"16\" y2=\"-25\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"-31.25\"   x2=\"8\"  y2=\"-31.25\"/>\n"
"      <line x1=\"-16\" y1=\"-37.5\"    x2=\"-4\" y2=\"-37.5\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"-35\">15</text>\n"
"      <line x1=\"4\"   y1=\"-37.5\"    x2=\"16\" y2=\"-37.5\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"-43.75\"   x2=\"8\"  y2=\"-43.75\"/>\n"
"      <line x1=\"-16\" y1=\"-50.0\"    x2=\"-4\" y2=\"-50.0\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"-48\">20</text>\n"
"      <line x1=\"4\"   y1=\"-50.0\"    x2=\"16\" y2=\"-50.0\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"-56.25\"   x2=\"8\"  y2=\"-56.25\"/>\n"
"      <line x1=\"-16\" y1=\"-62.5\"    x2=\"-4\" y2=\"-62.5\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"-60.5\">25</text>\n"
"      <line x1=\"4\"   y1=\"-62.5\"    x2=\"16\" y2=\"-62.5\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"-68.75\"   x2=\"8\"  y2=\"-68.75\"/>\n"
"      <line x1=\"-16\" y1=\"-75.0\"    x2=\"-4\" y2=\"-75.0\" />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"-73\">30</text>\n"
"      <line x1=\"4\"   y1=\"-75.0\"    x2=\"16\" y2=\"-75.0\" />\n"
"  \n"
"      <!-- pitch down -->\n"
"      <line x1=\"-8\"  y1=\"6.25\"   x2=\"8\"  y2=\"6.25\"/>\n"
"      <line x1=\"-16\" y1=\"12\"     x2=\"-4\" y2=\"12\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"14\">5</text>\n"
"      <line x1=\"4\"   y1=\"12\"     x2=\"16\" y2=\"12\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"18.75\"   x2=\"8\"  y2=\"18.75\"/>\n"
"      <line x1=\"-16\" y1=\"25\"     x2=\"-4\" y2=\"25\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"27\">10</text>\n"
"      <line x1=\"4\"   y1=\"25\"     x2=\"16\" y2=\"25\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"31.25\"   x2=\"8\"  y2=\"31.25\"/>\n"
"      <line x1=\"-16\" y1=\"37.5\"    x2=\"-4\" y2=\"37.5\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"39\">15</text>\n"
"      <line x1=\"4\"   y1=\"37.5\"    x2=\"16\" y2=\"37.5\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"43.75\"   x2=\"8\"  y2=\"43.75\"/>\n"
"      <line x1=\"-16\" y1=\"50.0\"    x2=\"-4\" y2=\"50.0\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"52\">20</text>\n"
"      <line x1=\"4\"   y1=\"50.0\"    x2=\"16\" y2=\"50.0\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"56.25\"   x2=\"8\"  y2=\"56.25\"/>\n"
"      <line x1=\"-16\" y1=\"62.5\"    x2=\"-4\" y2=\"62.5\"  />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"64.5\">25</text>\n"
"      <line x1=\"4\"   y1=\"62.5\"    x2=\"16\" y2=\"62.5\"  />\n"
"  \n"
"      <line x1=\"-8\"  y1=\"68.75\"   x2=\"8\"  y2=\"68.75\"/>\n"
"      <line x1=\"-16\" y1=\"75.0\"    x2=\"-4\" y2=\"75.0\" />\n"
"      <text stroke-width=\"0.1\" x=\"0\" y=\"77\">30</text>\n"
"      <line x1=\"4\"   y1=\"75.0\"    x2=\"16\" y2=\"75.0\" />\n"
"  \n"
"    </g>\n"
"    <g stroke-width=\"2\" stroke=\"#ff0\">\n"
"    <line x1=\"30\" y1=\"50\" x2=\"43\" y2=\"50\"></line>\n"
"    <line x1=\"42\" y1=\"50\" x2=\"42\" y2=\"53\"></line>\n"
"  \n"
"    <line  x1=\"49\" y1=\"50\" x2=\"51\" y2=\"50\"/>\n"
"  \n"
"    <line x1=\"58\" y1=\"53\" x2=\"58\" y2=\"50\"></line>\n"
"    <line x1=\"57\" y1=\"50\" x2=\"70\" y2=\"50\"></line>\n"
"    </g>  \n"
"  <g id='onspeed-attitude-dial' stroke-width=\"1\" transform=\"translate(50,49) rotate(0)\" stroke=\"#fff\">\n"
"      <line stroke-width='1.5' x1='20.00' y1='-34.64' x2='24.50' y2='-42.44' />\n"
"      <line stroke-width='1.5' x1='34.64' y1='-20.00' x2='42.44' y2='-24.50' />\n"
"      <line stroke-width='1.5' x1='40.00' y1='0.00' x2='49.00' y2='0.00' />\n"
"      <line stroke-width='1.5' x1='-20.00' y1='-34.64' x2='-24.50' y2='-42.44' />\n"
"      <line stroke-width='1.5' x1='-34.64' y1='-20.00' x2='-42.44' y2='-24.50' />\n"
"      <line stroke-width='1.5' x1='-40.00' y1='-0.00' x2='-49.00' y2='-0.00' />\n"
"      <line x1='6.95' y1='-39.39' x2='7.81' y2='-44.32' />\n"
"      <line x1='13.68' y1='-37.59' x2='15.39' y2='-42.29' />\n"
"      <line x1='28.28' y1='-28.28' x2='31.82' y2='-31.82' />\n"
"      <line x1='-6.95' y1='-39.39' x2='-7.81' y2='-44.32' />\n"
"      <line x1='-13.68' y1='-37.59' x2='-15.39' y2='-42.29' />\n"
"      <line x1='-28.28' y1='-28.28' x2='-31.82' y2='-31.82' />\n"
"      <line x1='0.00' y1='-40.00' x2='0.00' y2='-45.00' />\n"
"  \n"
"      <path fill=\"#fff\" d=\"M-4 -50 L4 -50 L 0 -40 z\"></path>\n"
"    </g>  \n"
"  \n"
"    <g stroke-width=\"1\" stroke=\"#ff0\" fill=\"#ff0\">\n"
"      <path d=\"M46 18 L54 18 L50 11 z\"></path>\n"
"    </g>\n"
"\n"
"  </svg>\n"
"  \n"
"  </div>\n"
"\n"
"  <div id=\"datafields\"></span>\n"
"    AOA: <span id=\"aoa\"></span> <br>\n"
"    IAS: <span id=\"ias\"></span> <br>\n"
"    P Alt: <span id=\"palt\"></span> <br>\n"
"    Vert G: <span id=\"gload\"></span> <br>\n"
"    Lat G: <span id=\"gloadLat\"></span> <br>\n"
"    Pitch: <span id=\"pitch\"></span> <br>\n"
"    Roll: <span id=\"roll\"></span> <br>\n"
"    DataMark: <span id=\"datamark\"></span> <br>\n"
"    Flaps: <span id=\"flapspos\"></span> <br>\n"
"    Age: <span id=\"age\"></span> <br>\n"
"  </div>\n"
"  <div id=\"footer\">\n"
"    <div id=\"status\">\n"
"     <div id=\"status-label\">\n"
"       <strong>Status:</strong> <span id=\"connectionstatus\">DISCONNECTED.</span>\n"
"     </div>\n"
"     <div id=\"switch\">\n"
"     <div class=\"switch-field\">\n"
"     <input type=\"radio\" id=\"switchAOA\" name=\"toggleAOA\"  onClick=\"toggleAOA(true)\" value=\"yes\" checked/>\n"
"     <label for=\"switchAOA\">AOA</label>\n"
"     <input type=\"radio\" id=\"switchAHRS\" name=\"toggleAOA\" onClick=\"toggleAOA(false)\" value=\"no\" />\n"
"     <label for=\"switchAHRS\">AHRS</label>\n"
"   </div>\n"
"   </div>\n"
"   </div>\n"
"   \n"
"   <div id=\"footer-warning\">  \n"
"   For diagnostic purposes only. NOT SAFE FOR FLIGHT\n"
"   </div>\n"
"  </div>\n"
"</div>\n"
"<script>init()</script>\n"
;
  page+=pageFooter;
  server.send(200, "text/html", page); 
  }  

void handleGetValue()
{
// get value from teensy
Serial.println("$STOPLIVEDATA!");
delay(200);
Serial.flush();
// get value name
String valueName;
if (server.hasArg("name")) valueName=server.arg("name"); else server.send(200, "text/html", "Missing value name"); // send error message if no value argument
Serial.println("$"+valueName+"!");
String responseString="";
unsigned long starttime=millis();
  while (true)
  {
  if (Serial.available()>0) 
      {
       char inChar=Serial.read();
       responseString+=inChar;       
       starttime=millis();       
       if (responseString.indexOf("</"+valueName+">")>0)
            {
            server.send(200, "text/html", responseString);
            return;
            }
      }
  if ((millis()-starttime)>10000)
      {      
      server.send(408, "text/html", "Timeout");
      return;
      }
  }  
}
  
void handleFormat()
  {
   String page="";
   updateHeader();
   page+=pageHeader;
  if (server.arg("confirm").indexOf("yes")<0)
    {
    // display confirmation page 
    page+="<br><br><p style=\"color:red\">Confirm that you want to format the internal SD card. You will lose all the files currently on the card.</p>\
    <br><br><br>\
    <a href=\"format?confirm=yes\" class=\"button\">Format SD Card</a>\
    <a href=\"/\">Cancel</a>";
    } else 
          {
          // format card page
          Serial.println("$FORMAT!");
           String formatResponse;
           unsigned long formatStartTime=millis();
           // wait for response
          
          while (true)
            {
            if (Serial.available())
                {
                formatResponse=formatResponse+char(Serial.read());                
                }
            if (formatResponse.indexOf("</FORMATDONE>") >= 0)
              {
              page+="<br><br><p>SD card has been formatted.</p>";
              page+="<br>New card size is: ";
              formatResponse=getConfigValue(formatResponse,"FORMATDONE");
              page+=formatResponse;
              break; 
              } else
                    if (formatResponse.indexOf("</FORMATERROR>") >= 0) 
                        {
                         page+="<br><br>"+getConfigValue(formatResponse,"FORMATERROR");
                         page+="<br><br><p>Error: Could not format SD card. Please reboot and try again.</p>";
                         break;
                        }
            if (millis()-formatStartTime>10000)
                {
                // format timeout
                page+="<br><br><p>Timeout formatting SD card. Please reboot and try again.</p>";
                break;
                }
            } // while
          }     
                
   page+=pageFooter;
   server.send(200, "text/html", page);   
  }

void handleCalWizard() 
  {
   String wizardStep="";
   if (server.hasArg("step") && server.arg("step")!="") wizardStep=server.arg("step");
   String page="";
   updateHeader();   
   page+=pageHeader;   
   page+="<br><b>Calibration Wizard</b><br><br>";
   // start (get parameters for calibration) -> stalls -> flysetpoints -> verifysetpoints -> done (next flap setting).

   if (wizardStep=="")
   {
   page+="This wizard will guide you through the AOA calibration process.<br><br>\
          Here's how it works:<br><br>\
          <b>Step 1.</b><br>\
          Enter aircraft parameters (weight, speeds, etc)<br>\
          <b>Step 2.</b><br>\
          Set your flaps to the position you wish to calibrate<br>\
          <b>Step 3.</b><br>\
          Fly a deceleration run from Vfe to stall (slow down while holding altitude), including stalling the airplane<br>\
          Deceleration needs to be smooth and controlled with the help of realtime on-screen feedback.<br>\
          The system will detect the stall, calculate all the setpoints and the AOA curve.<br>\
          <b>Step 5.</b><br>\
          Verify the quality of the deceleration run (using automated grading) and save the results if they are satisfactory, otherwise fly the calibration run again<br>\
          <b>Step 6.</b><br>\
          Start again from step 2 with the next flap setting.<br>\
          <br><br>\
          Once you are done with all flap settings, verify that the calibration is correct.<br><br>\
          <a href=\"/calwiz?step=start\" class=\"button\">Start Calibration Now</a>\
          <br>\
          ";
   } else
   
   if  (wizardStep=="start")
   {
   page+="Enter the following aircraft parameters:<br><br>\
          <div class=\"content-container\">\
            <form  id=\"id_configWizStartForm\" action=\"calwiz?step=decel\" method=\"POST\">\
            <div class=\"form round-box\">";
            if (0==0)
                {
                page+="<div class=\"form-divs flex-col-12\">\
                <label>Aircraft gross weight (lbs)</label>\
                <input class=\"inputField\" type=\"text\" name=\"acGrossWeight\" value=\""+ String(acGrossWeight)+"\">\
                </div>\
                <div class=\"form-divs flex-col-12\">\
                <label>Aircraft current weight (lbs)</label>\
                <input class=\"inputField\" type=\"text\" name=\"acEmptyWeight\" value=\""+ String(acCurrentWeight)+"\">\
                </div>\
                <div class=\"form-divs flex-col-12\">\
                <label>Best glide airspeed at gross weight (KIAS)</label>\
                <input class=\"inputField\" type=\"text\" name=\"acVldmax\" value=\""+ String(acVldmax)+"\">\
                <br>\
                <br>\
                The above parameters will allow us to calculate your current best glide speed.\
                </div>";
                } else
                      {
                      page+="<div class=\"form-divs flex-col-12\">\
                      <label>Maximum airspeed allowed for 20 degrees of flaps (KIAS)</label>\
                      <input class=\"inputField\" type=\"text\" name=\"acVldmax\" value=\""+ String(acVldmax)+"\">\
                      </div>";
                      }                      
                page+="<br><br><br>\
                <div class=\"form-divs flex-col-6\">\
                <a href=\"/\">Cancel</a>\
                </div>\
                <div class=\"form-divs flex-col-6\">\
                <button type=\"submit\" class=\"button\">Continue</button>\
                </div>\
          </div>\
          </form>\
        </div>";
   } else
   if  (wizardStep=="decel")
   {
   page+="<form  id=\"id_configWizStartForm\" action=\"calwiz?step=flydecel\" method=\"POST\">\
          Get ready to fly a constant altitude deceleration. Do not change flap position at this time.<br>\
          <b>Requirements:</b>\
          <br><br>\
          1. Fly the stall at a steady 1kt/sec deceleration (Onspeed provides feedback)<br>\
          2. Keep the ball in the middle and wings level at all times<br>\
          3. Do not pull up abruptly into the stall. Stall smoothly.<br>\
          <br><br>\
          After detecting the stall OnSpeed will grade it and let you know if it was flown correctly.\
          <br><br>\
          Ready? Hit the Continue button below and then fly max speed (below flap extension speed with flaps down) with the current flap setting.\
          <br><br>\
          <br><br><br>\
          <a href=\"/\">Cancel</a>\
          <button type=\"submit\" class=\"button\">Continue</button>\
          </form>";
   } else
     if  (wizardStep=="flydecel")
         {      
            page+=String(jsSGfilter);
            page+=String(jsCalibration);
            page+="<span id=\"idStartInstructions\">Start out at Vmax. Hit Record when ready!\
            <button type=\"submit\" class=\"button\">Record</button>\
            <br>\
            </span>";            
            //page+="<span id=\"idRecording\">Recording...\
            <button type=\"submit\" class=\"button\">Cancel</button>\
            <br>\
            </span>";
            page+="<div id=\"decelgauge\" style=\"float:left\">";
            page+=String(htmlDecelGauge);
            page+="</div>";
            page+="<br><div align=\"left\">IAS: <span id=\"currentIAS\"></span> kts</div><br>";
            page+="<div align=\"left\">Decel rate: <span id=\"currentDecel\"></span> kts/sec</div><br>";
            page+="<div align=\"left\">Smoothing: <span id=\"currentSmoothing\"></span></div><br>";
            page+="<div class=\"slidecontainer\"><input type=\"range\" min=\".01\" max=\".95\" value=\".1\" step=\".01\" class=\"slider\" id=\"smoothingValue\" style=\"width:300px\"></div>";
            
            
            page+="<br><div align=\"left\">Status: <span id=\"connectionstatus\"></span></div>";
            page+="<br><br>";
            page+="<script>init()</script>";
        }
   
   page+=pageFooter;
   server.send(200, "text/html", page);
  } 

void handleDefaultConfig()
  {
   String page="";
   updateHeader();
   page+=pageHeader;

   if (server.hasArg("confirm") && server.arg("confirm")=="yes")
      {
      //load default config
       server.sendHeader("Location", "/aoaconfig?loadDefault=true");
       server.send(301, "text/html", page);
       return;     
      } else
            {
            // display confirmation page 
            page+="<br><br><p style=\"color:red\">Confirm that you want to load the Default configuration. This will erase all of your current settings.</p>\
            <br><br><br>\
            <a href=\"/defaultconfig?confirm=yes\" class=\"button\">Load Default Config</a>\
            <a href=\"/aoaconfig\">Cancel</a>";
            }  
   page+=pageFooter;
   server.send(200, "text/html", page);
  }

String getConfigString()
{
// get configstring 
Serial.flush();
Serial.println("$SENDCONFIGSTRING!");   
String configString="";
unsigned long starttime=millis();
while (true)
  {
  if (Serial.available()>0) 
      {
       char inChar=Serial.read();
       configString+=inChar;
       starttime=millis();  // reset timeout           
       if (configString.indexOf("</CHECKSUM>")>0)
          {
           String checksumString=getConfigValue(configString,"CHECKSUM");
           String configContent=getConfigValue(configString,"CONFIG");
           // calc CRC 
           int16_t calcCRC; 
           for (unsigned int i=0;i<configContent.length();i++) calcCRC+=configContent[i];
              if (String(calcCRC,HEX)==checksumString)
                 {
                 // checksum match
                  return configString;
                 } else return "BAD CHECKSUM:"+configString; // checksum error
          }
      }
  // timeout
  if ((millis()-starttime)>3000)
      {      
      return "TIMEOUT"; // don't return partial configstring when timeout occurs
      }
  }
return configString;  
}

String getDefaultConfigString()
{
// get configstring 
Serial.flush();
Serial.println("$SENDDEFAULTCONFIGSTRING!");   
String configString="";
unsigned long starttime=millis();
while (true)
  {
  if (Serial.available()>0) 
      {
       char inChar=Serial.read();
       configString+=inChar;
       starttime=millis();  // reset timeout           
       if (configString.indexOf("</CHECKSUM>")>0)
          {
           String checksumString=getConfigValue(configString,"CHECKSUM");
           String configContent=getConfigValue(configString,"CONFIG");
           // calc CRC
           int16_t calcCRC; 
           for (unsigned int i=0;i<configContent.length();i++) calcCRC+=configContent[i];
           if (String(calcCRC,HEX)==checksumString)
                 {
                 // checksum match
                 return configString;
                 } else return ""; // checksum error
          }
      }
  // timeout
  if ((millis()-starttime)>2000)
      {      
      return ""; // don't return partial configstring when timeout occurs
      }
  }
return configString;  
}

bool setConfigString(String configString)
{
Serial.flush();
// add CRC to configString
configString=addCRC(configString);
String stringToSend="$SAVECONFIGSTRING"+configString+"!";
for (unsigned int i=0; i<stringToSend.length();i++)
{
Serial.print(stringToSend[i]);
unsigned long delayTimer=micros();
while (micros()-delayTimer<200) {}; // transmit delay to avoid output buffer overrun
}

unsigned long starttime=millis();
// wait for ACK
String responseString;
while (true)
      {            
      //wait 2 seconds to confirm saveConfigSting
      if (Serial.available()>0)
         {
         char inChar=Serial.read();
         responseString+=inChar;
         if (responseString.indexOf("</CONFIGSAVED>")>=0) return true;
         if (responseString.indexOf("</CONFIGERROR>")>=0) return false;
         }

      // timeout
      if ((millis()-starttime)>3000)
          {            
          return false; // timeout
          }
      }
return false;
}

void handleConfigSave()
{
bool rebootRequired=false;
String errorMessage="";
int flapCurvesCount=0;
if (server.hasArg("aoaSmoothing")) aoaSmoothing=server.arg("aoaSmoothing").toInt();
if (aoaSmoothing!=server.arg("aoaSmoothing").toInt()) rebootRequired=true;

if (server.hasArg("pressureSmoothing")) pressureSmoothing=server.arg("pressureSmoothing").toInt();
if (server.hasArg("dataSource"))
    {
    Serial.print("$DEBUG");
    Serial.print(dataSource);
    Serial.print(":");
    Serial.print(server.arg("dataSource")); 
    Serial.println("!"); 
    if (dataSource!=server.arg("dataSource")) rebootRequired=true;
    dataSource=server.arg("dataSource");    
    }
if (server.hasArg("logFileName")) replayLogFileName=server.arg("logFileName");
for (int flapIndex=0; flapIndex< MAX_AOA_CURVES; flapIndex++)
    {
    if (server.hasArg("flapDegrees"+String(flapIndex)) && !server.hasArg("flapDegreesDelete"+String(flapIndex)))
        {
        // flap curve exists
        // read positions
        flapDegrees.Items[flapCurvesCount]=server.arg("flapDegrees"+String(flapIndex)).toInt();
        flapPotPositions.Items[flapCurvesCount]=server.arg("flapPotPositions"+String(flapIndex)).toInt();
        // read setpoints
        flapLDMAXAOA.Items[flapCurvesCount]=stringToFloat(server.arg("flapLDMAXAOA"+String(flapIndex)));
        flapONSPEEDFASTAOA.Items[flapCurvesCount]=stringToFloat(server.arg("flapONSPEEDFASTAOA"+String(flapIndex)));
        flapONSPEEDSLOWAOA.Items[flapCurvesCount]=stringToFloat(server.arg("flapONSPEEDSLOWAOA"+String(flapIndex)));
        flapSTALLWARNAOA.Items[flapCurvesCount]=stringToFloat(server.arg("flapSTALLWARNAOA"+String(flapIndex)));
        
        // read curve coefficients
        for (int curveIndex=0; curveIndex< MAX_CURVE_COEFF; curveIndex++)
            {
            aoaCurve[flapCurvesCount].Items[curveIndex]=stringToFloat(server.arg("aoaCurve"+String(flapIndex)+"Coeff"+String(curveIndex)));
            aoaCurve[flapCurvesCount].curveType=server.arg("aoaCurve"+String(flapIndex)+"Type").toInt();           
            }            
        flapCurvesCount++;
        }        
    }    
// update flap position & curves count
flapDegrees.Count=flapCurvesCount;
flapLDMAXAOA.Count=flapCurvesCount;
flapONSPEEDFASTAOA.Count=flapCurvesCount;
flapONSPEEDSLOWAOA.Count=flapCurvesCount;
flapSTALLWARNAOA.Count=flapCurvesCount;
flapPotPositions.Count=flapCurvesCount;
// read boom enabled/disabled
if (server.hasArg("readBoom") && server.arg("readBoom")=="1") readBoom=true; else readBoom=false;

if (server.hasArg("casCurveEnabled") && server.arg("casCurveEnabled")=="1") casCurveEnabled=true; else casCurveEnabled=false;

if (server.hasArg("casCurveType"))  casCurve.curveType=server.arg("casCurveType").toInt();


// cas Curve
for (int curveIndex=0; curveIndex< MAX_CURVE_COEFF; curveIndex++)
    {
    casCurve.Items[curveIndex]=stringToFloat(server.arg("casCurveCoeff"+String(curveIndex)));
    }                  
// read portsOrientation
if (server.hasArg("portsOrientation")) portsOrientation=server.arg("portsOrientation");
// read boxtopOrientation
if (server.hasArg("boxtopOrientation")) boxtopOrientation=server.arg("boxtopOrientation");
// read efis enabled/disabled
if (server.hasArg("readEfisData") && server.arg("readEfisData")=="1") readEfisData=true; else readEfisData=false;
// read efis Type
if (server.hasArg("efisType")) efisType=server.arg("efisType");
// read volume control
if (server.hasArg("volumeControl") && server.arg("volumeControl")=="1") volumeControl=true; else volumeControl=false;
//read volumePercent
if (server.hasArg("defaultVolume")) defaultVolume=server.arg("defaultVolume").toInt();
// read low volume position
if (server.hasArg("volumeLowAnalog")) volumeLowAnalog=server.arg("volumeLowAnalog").toInt();
// read high volume position
if (server.hasArg("volumeHighAnalog")) volumeHighAnalog=server.arg("volumeHighAnalog").toInt();
// read muteAudioUnderIAS
if (server.hasArg("muteAudioUnderIAS")) muteAudioUnderIAS=server.arg("muteAudioUnderIAS").toInt();
// read 3d audio enabled/disabled
if (server.hasArg("audio3D") && server.arg("audio3D")=="1") audio3D=true; else audio3D=false;
//overgWarning
if (server.hasArg("overgWarning") && server.arg("overgWarning")=="1") overgWarning=true; else overgWarning=false;
// loadLimit    
if (server.hasArg("loadLimitPositive")) loadLimitPositive=server.arg("loadLimitPositive").toFloat();
if (server.hasArg("loadLimitNegative")) loadLimitNegative=server.arg("loadLimitNegative").toFloat();
// vnochime
if (server.hasArg("vnoChimeEnabled") && (server.arg("vnoChimeEnabled")=="1")) vnoChimeEnabled=true; else vnoChimeEnabled=false;
if (server.hasArg("Vno")) Vno=server.arg("Vno").toInt();
if (server.hasArg("vnoChimeInterval")) vnoChimeInterval=server.arg("vnoChimeInterval").toInt();
// serialOutFormat
if (server.hasArg("serialOutFormat")) serialOutFormat=server.arg("serialOutFormat");
//serialOutPort
if (server.hasArg("serialOutPort")) serialOutPort=server.arg("serialOutPort");
 
// sdLogging
if (server.hasArg("sdLogging") && server.arg("sdLogging")=="1") sdLoggingConfig=true; else sdLoggingConfig=false;

String page="";
String configString="";
updateHeader();
page+=pageHeader;
// save configuration
if (setConfigString(configurationToString()))
    {
    // config set
    page+="<br><br>Configuration saved.";
    if (rebootRequired)
        {
        page+="<br><br>Some of the changes require a system reboot to take effect.<br><br><br><a href=\"reboot?confirm=yes\" class=\"button\">Reboot Now</a>";
        }
    
    } else
      {
      // error setting config
       page+="<br><br>Error: could not save new configuration. Please try again.";
      }
page+=pageFooter;
if (server.hasArg("addFlapPos") && flapDegrees.Count<MAX_AOA_CURVES)
    {
    flapDegrees.Count++;
    server.sendHeader("Location", "/aoaconfig?addFlapPos=true");
    server.send(301, "text/html", page);
    return;
    }   
 server.send(200, "text/html", page);
}

void handleConfig()
{
String page="";
String configString="";
updateHeader();
page+=pageHeader;
// get and load configString and then display config in a web page

if (uploadConfigString!="")
    {
    //config file was uploaded  
    if (uploadConfigString.indexOf("<CONFIG>")<0 || uploadConfigString.indexOf("</CONFIG>")<0)
                              {
                              // no config string
                              page+="<br><br>The uploaded file does not contain a valid configuration. Try another file.";                              
                              page+=pageFooter;
                              server.send(200, "text/html", page);
                              return;      
                              }
    configString=uploadConfigString;
    uploadConfigString="";
    page+="<script>alert(\"Configuration loaded.\\n\\nVerify settings and click SAVE for the new configuration to take effect.\")</script>";
    } else 
          {
          if (server.hasArg("loadDefault") && server.arg("loadDefault")=="true") configString=getDefaultConfigString(); // load default config
             else        
                  configString=getConfigString(); // load current config

          if (configString.indexOf("</CONFIG>")<0)
                {
                // no config string
                page+=configString;
                page+="<br><br>Could not download configuration from main CPU<br><br>Reload this page to try again.";
                page+=pageFooter;
                server.send(200, "text/html", page);
                return;      
                }
          }      
loadConfigFromString(configString);
// handle add flap position
if (server.hasArg("addFlapPos") && flapDegrees.Count<MAX_AOA_CURVES)
    {
    // add new flap curve  
    flapDegrees.Count++;
    // add default values for new flap setpoints and curves
    flapDegrees.Items[flapDegrees.Count-1]=0;
    flapPotPositions.Items[flapDegrees.Count-1]=0;
    flapLDMAXAOA.Items[flapDegrees.Count-1]=0.00;
    flapONSPEEDFASTAOA.Items[flapDegrees.Count-1]=0.00;
    flapONSPEEDSLOWAOA.Items[flapDegrees.Count-1]=0.00;
    flapSTALLWARNAOA.Items[flapDegrees.Count-1]=0.00;
    
    for (int curveIndex=0; curveIndex< MAX_CURVE_COEFF; curveIndex++)
            {
            aoaCurve[flapDegrees.Count-1].Items[curveIndex]=0.00;
            aoaCurve[flapDegrees.Count-1].curveType=1; // default to polynomial curve           
            }                       
    }


// get live values script
page+="<script>\
function getValue(senderID, valueName,targetID)\
 {\
  senderOriginalValue=document.getElementById(senderID).value;\
  document.getElementById(senderID).disabled = true;\
  if (valueName==\"AUDIOTEST\" || valueName==\"VNOCHIMETEST\")\
        {\
        document.getElementById(senderID).value=\"Testing...\";\
        } else\
              {\
              document.getElementById(senderID).value=\"Reading...\";\
              }\
  var xmlHttp = new XMLHttpRequest();\
  xmlHttp.onreadystatechange = function() {\
    if (this.readyState == 4 && this.status == 200 && senderID!='') {\
    if (this.responseText.indexOf(\"<\"+valueName+\">\")>=0)\
      {\
      if (valueName==\"AUDIOTEST\" || valueName==\"VNOCHIMETEST\")\
        {\
        document.getElementById(senderID).value=senderOriginalValue;\
        } else\
              {\
               document.getElementById(targetID).value = this.responseText.replace(/(<([^>]+)>)/ig,\"\");\
               document.getElementById(senderID).value=\"Updated!\";\
              }\
      } else\
          {\
           document.getElementById(senderID).value=\"Error!\";\
          }\
    setTimeout(function(){ document.getElementById(senderID).value=senderOriginalValue;document.getElementById(senderID).disabled = false}, 1000);\
      }\
  };\
  xmlHttp.open(\"GET\", \"/getvalue?name=\"+valueName, true);\
  xmlHttp.send();\
  }\
</script>";
//container
page+="<div class=\"content-container\">\
   <form  id=\"id_configForm\" action=\"aoaconfigsave\" method=\"POST\">\
   <div class=\"form round-box\">";
// aoaSmoothing
    page+="<div class=\"form-divs flex-col-12\">\
      <label for=\"id_aoaSmoothing\">AOA Smoothing</label>\
      <input id=\"id_aoaSmoothing\" name=\"aoaSmoothing\" type=\"text\" value=\""+String(aoaSmoothing)+"\"/>\
      </div>";
// pressureSmoothing    
     page+="<div class=\"form-divs flex-col-12\">\
      <label for=\"id_pressureSmoothing\">Pressure Smoothing</label>\
      <input id=\"id_pressureSmoothing\" name=\"pressureSmoothing\" type=\"text\" value=\""+String(pressureSmoothing)+"\" />\
      </div>";
// dataSource
    page+="<div class=\"form-divs flex-col-12\">\
      <label for=\"id_dataSource\">Data Source - Operation mode</label>\
      <select id=\"id_dataSource\" name=\"dataSource\">\
        <option value=\"SENSORS\""; if (dataSource=="SENSORS") page+=" selected"; page+=">Sensors (default)</option>\
        <option value=\"TESTPOT\""; if (dataSource=="TESTPOT") page+=" selected"; page+=">Test Potentiometer</option>\
        <option value=\"RANGESWEEP\""; if (dataSource=="RANGESWEEP") page+=" selected"; page+=">Range Sweep</option>\
        <option value=\"REPLAYLOGFILE\""; if (dataSource=="REPLAYLOGFILE") page+=" selected"; page+=">Replay Log File</option>\
      </select>\
    </div>";
 // logFileToReplay
    String replayLogFileStyle;
    if (dataSource=="REPLAYLOGFILE") replayLogFileStyle="style=\"display:block\""; else replayLogFileStyle="style=\"display:none\"";
    page+="<div class=\"form-divs flex-col-12 replaylogfilesetting\""+replayLogFileStyle+">\
      <label for=\"id_replayLogFile\">Log file to replay</label>\
       <input id=\"id_replayLogFile\" name=\"logFileName\" type=\"text\" value=\""+String(replayLogFileName)+"\" />\
      </div>";
    // flap curves
    for (int flapIndex=0; flapIndex<flapDegrees.Count; flapIndex++)
        {
        // start flap curve section  
        page+="<section id=\"curvesection\">";  
        page+="<h2> Flap Curve "+String(flapIndex+1)+" </h2>\
              <div class=\"form-divs flex-col-12\">\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapDegrees"+String(flapIndex)+"\">Flap Degrees</label>\
              <input id=\"id_flapDegrees"+String(flapIndex)+"\" name=\"flapDegrees"+String(flapIndex)+"\" type=\"text\" value=\""+String(flapDegrees.Items[flapIndex])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapPotPositions"+String(flapIndex)+"\">Sensor Value</label>\
              <input id=\"id_flapPotPositions"+String(flapIndex)+"\" class=\"curve\" name=\"flapPotPositions"+String(flapIndex)+"\" type=\"text\" value=\""+String(flapPotPositions.Items[flapIndex])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapPotPositions"+String(flapIndex)+"Read\">&nbsp</label>\
              <input id=\"id_flapPotPositions"+String(flapIndex)+"Read\" name=\"flapPotPositions"+String(flapIndex)+"Read\" type=\"button\" value=\"Read\" class=\"greybutton\" onclick=\"getValue(this.id,\'FLAPS\',\'id_flapPotPositions"+String(flapIndex)+"\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-8\">\
              <label for=\"id_flapLDMAXAOA"+String(flapIndex)+"\">L/Dmax AOA</label>\
              <input id=\"id_flapLDMAXAOA"+String(flapIndex)+"\" class=\"curve\" name=\"flapLDMAXAOA"+String(flapIndex)+"\" type=\"text\" value=\""+floatToString(flapLDMAXAOA.Items[flapIndex])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapLDMAXAOA"+String(flapIndex)+"Read\">&nbsp</label>\
              <input id=\"id_flapLDMAXAOA"+String(flapIndex)+"Read\" name=\"flapLDMAXAOA"+String(flapIndex)+"Read\" type=\"button\" value=\"Use Live AOA\" class=\"greybutton\" onclick=\"getValue(this.id,\'AOA\',\'id_flapLDMAXAOA"+String(flapIndex)+"\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-8\">\
              <label for=\"id_flapONSPEEDFASTAOA"+String(flapIndex)+"\">OnSpeed Fast AOA</label>\
              <input id=\"id_flapONSPEEDFASTAOA"+String(flapIndex)+"\" name=\"flapONSPEEDFASTAOA"+String(flapIndex)+"\" type=\"text\" value=\""+floatToString(flapONSPEEDFASTAOA.Items[flapIndex])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapONSPEEDFASTAOA"+String(flapIndex)+"Read\">&nbsp</label>\
              <input id=\"id_flapONSPEEDFASTAOA"+String(flapIndex)+"Read\" name=\"flapONSPEEDFASTAOA"+String(flapIndex)+"Read\" type=\"button\" value=\"Use Live AOA\" class=\"greybutton\" onclick=\"getValue(this.id,\'AOA\',\'id_flapONSPEEDFASTAOA"+String(flapIndex)+"\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-8\">\
              <label for=\"id_flapONSPEEDSLOWAOA"+String(flapIndex)+"\">OnSpeed Slow AOA</label>\
              <input id=\"id_flapONSPEEDSLOWAOA"+String(flapIndex)+"\" name=\"flapONSPEEDSLOWAOA"+String(flapIndex)+"\" type=\"text\" value=\""+floatToString(flapONSPEEDSLOWAOA.Items[flapIndex])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapONSPEEDSLOWAOA"+String(flapIndex)+"Read\">&nbsp</label>\
              <input id=\"id_flapONSPEEDSLOWAOA"+String(flapIndex)+"Read\" name=\"flapONSPEEDSLOWAOA"+String(flapIndex)+"Read\" type=\"button\" value=\"Use Live AOA\" class=\"greybutton\" onclick=\"getValue(this.id,\'AOA\',\'id_flapONSPEEDSLOWAOA"+String(flapIndex)+"\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-8\">\
              <label for=\"id_flapSTALLWARNAOA"+String(flapIndex)+"\">Stall Warning AOA</label>\
              <input id=\"id_flapSTALLWARNAOA"+String(flapIndex)+"\" name=\"flapSTALLWARNAOA"+String(flapIndex)+"\" type=\"text\" value=\""+floatToString(flapSTALLWARNAOA.Items[flapIndex])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-4\">\
              <label for=\"id_flapSTALLWARNAOA"+String(flapIndex)+"Read\">&nbsp</label>\
              <input id=\"id_flapSTALLWARNAOA"+String(flapIndex)+"Read\" name=\"flapSTALLWARNAOA"+String(flapIndex)+"Read\" type=\"button\" value=\"Use Live AOA\" class=\"greybutton\" onclick=\"getValue(this.id,\'AOA\',\'id_flapSTALLWARNAOA"+String(flapIndex)+"\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-12\">\
                <label for=\"id_aoaCurve"+String(flapIndex)+"Type\">AOA Curve Type</label>\
                <select id=\"id_aoaCurve"+String(flapIndex)+"Type\" name=\"aoaCurve"+String(flapIndex)+"Type\" onchange=\"curveTypeChange(\'id_aoaCurve"+String(flapIndex)+"Type\',"+String(flapIndex)+")\">\
                  <option value=\"1\""; if (aoaCurve[flapIndex].curveType==1) page+=" selected"; page+=">Polynomial</option>\
                  <option value=\"2\""; if (aoaCurve[flapIndex].curveType==2) page+=" selected"; page+=">Logarithmic</option>\
                  <option value=\"3\""; if (aoaCurve[flapIndex].curveType==3) page+=" selected"; page+=">Exponential</option>\
                </select>\
              </div>\
              <div class=\"form-divs flex-col-12 curvelabel\">\
                <label>AOA Curve</label>\
              </div>\
              <div class=\"form-divs flex-col-2\">\
                <input id=\"id_aoaCurve"+String(flapIndex)+"Coeff0\" class=\"curve\" name=\"aoaCurve"+String(flapIndex)+"Coeff0\" type=\"text\" value=\""+floatToString(aoaCurve[flapIndex].Items[0])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-1\" id=\"id_aoaCurve"+String(flapIndex)+"Param0\">";
              if (aoaCurve[flapIndex].curveType==1) page+=" *X<sup>3</sup>+ "; else
                  if (aoaCurve[flapIndex].curveType==2) page+=" * 0 + "; else
                      if (aoaCurve[flapIndex].curveType==3) page+=" * 0 + ";
              page+="</div>\
              <div class=\"form-divs flex-col-2\">\
                <input id=\"id_aoaCurve"+String(flapIndex)+"Coeff1\" class=\"curve\" name=\"aoaCurve"+String(flapIndex)+"Coeff1\" type=\"text\" value=\""+floatToString(aoaCurve[flapIndex].Items[1])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-1\" id=\"id_aoaCurve"+String(flapIndex)+"Param1\">";
              if (aoaCurve[flapIndex].curveType==1) page+=" *X<sup>2</sup>+ "; else
                  if (aoaCurve[flapIndex].curveType==2) page+=" * 0 + "; else
                      if (aoaCurve[flapIndex].curveType==3) page+=" * 0 + ";
              page+="</div>\
              <div class=\"form-divs flex-col-2\">\
                <input id=\"id_aoaCurve"+String(flapIndex)+"Coeff2\" class=\"curve\" name=\"aoaCurve"+String(flapIndex)+"Coeff2\" type=\"text\" value=\""+floatToString(aoaCurve[flapIndex].Items[2])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-1\" id=\"id_aoaCurve"+String(flapIndex)+"Param2\">";
                if (aoaCurve[flapIndex].curveType==1) page+=" *X<sup></sup>+ "; else
                  if (aoaCurve[flapIndex].curveType==2) page+="*ln(x)+"; else
                      if (aoaCurve[flapIndex].curveType==3) page+="* e^ (";
              page+="</div>\
              <div class=\"form-divs flex-col-2\">\
                <input id=\"id_aoaCurve"+String(flapIndex)+"Coeff3\" class=\"curve\" name=\"aoaCurve"+String(flapIndex)+"Coeff3\" type=\"text\" value=\""+floatToString(aoaCurve[flapIndex].Items[3])+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-1\"  id=\"id_aoaCurve"+String(flapIndex)+"Param3\">";
              if (aoaCurve[flapIndex].curveType==1) page+=""; else
                  if (aoaCurve[flapIndex].curveType==2) page+=""; else
                      if (aoaCurve[flapIndex].curveType==3) page+=" * x)";
              page+="</div>\
                <div class=\"form-divs flex-col-6\"></div>\
               <div class=\"form-divs flex-col-6\">\
              <input id=\"id_flapDegrees"+String(flapIndex)+"Delete\" name=\"flapDegreesDelete"+String(flapIndex)+"\" type=\"submit\" value=\"Delete Flap Position\" class=\"redbutton\" onClick=\"return confirm(\'Are you sure you want to delete this flap position?')\"/>\
              </div>";
              
              // end flap curve section
              page+="</section>";
        }        
            // add flap position
              if (flapDegrees.Count<MAX_AOA_CURVES)
                  {
                    page+="<div class=\"form-divs flex-col-6\">\
                  <input id=\"id_addFlapPos\" name=\"addFlapPos\" type=\"submit\" value=\"Add New Flap Position\" class=\"greybutton\"/>\
                  </div>";
                  }
              
              // boom enable/disable  
              page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_readBoom\">Flight Test Boom</label>\
                <select id=\"id_readBoom\" name=\"readBoom\">\
                  <option value=\"1\""; if (readBoom) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!readBoom) page+=" selected"; page+=">Disabled</option>\
                </select>\
              </div>";
             String casCurveVisibility;
             if (casCurveEnabled) casCurveVisibility="style=\"display:block\""; else casCurveVisibility="style=\"display:none\"";

              // cas curve enable/disable  
              page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_casCurveEnabled\">Airspeed Calibration</label>\
                <select id=\"id_casCurveEnabled\" name=\"casCurveEnabled\">\
                  <option value=\"1\""; if (casCurveEnabled) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!casCurveEnabled) page+=" selected"; page+=">Disabled</option>\
                </select>\
              </div>";

              // airspeed calibration curve type
              page+="<div class=\"form-divs flex-col-12 cascurvesetting\" "+String(casCurveVisibility)+">\
                <label for=\"id_casCurveType\">Airspeed Calibration Curve Type</label>\
                <select id=\"id_casCurveType\" name=\"casCurveType\" onchange=\"cascurveTypeChange(\'id_casCurveType\')\">\
                  <option value=\"1\""; if (casCurve.curveType==1) page+=" selected"; page+=">Polynomial</option>\
                  <option value=\"2\""; if (casCurve.curveType==2) page+=" selected"; page+=">Logarithmic</option>\
                  <option value=\"3\""; if (casCurve.curveType==3) page+=" selected"; page+=">Exponential</option>\
                </select>\
              </div>";                            
              
              // CAS curve
              page+="<div class=\"form-divs flex-col-12 curvelabel cascurvesetting\"  "+String(casCurveVisibility)+">\
                <label>Airspeed Calibration Curve</label>\
              </div>\
              <div class=\"form-divs flex-col-2 cascurvesetting\"  "+String(casCurveVisibility)+">\
                <input id=\"id_casCurveCoeff0\" class=\"curve\" name=\"casCurveCoeff0\" type=\"text\" value=\""+floatToString(casCurve.Items[0])+"\"/>\
              </div>\
              <div id=\"id_casCurveParam0\" class=\"form-divs flex-col-1 cascurvesetting\"  "+String(casCurveVisibility)+">";
              if (casCurve.curveType==1) page+=" *X<sup>3</sup>+ "; else
                  if (casCurve.curveType==2) page+=" * 0 + "; else
                      if (casCurve.curveType==3) page+=" * 0 + ";
              page+="</div>\
              <div class=\"form-divs flex-col-2 cascurvesetting\"  "+String(casCurveVisibility)+">\
                <input id=\"id_casCurveCoeff1\" class=\"curve\" name=\"casCurveCoeff1\" type=\"text\" value=\""+floatToString(casCurve.Items[1])+"\"/>\
              </div>\
              <div id=\"id_casCurveParam1\" class=\"form-divs flex-col-1 cascurvesetting\"  "+String(casCurveVisibility)+">";
              if (casCurve.curveType==1) page+=" *X<sup>2</sup>+ "; else
                  if (casCurve.curveType==2) page+=" * 0 + "; else
                      if (casCurve.curveType==3) page+=" * 0 + ";
              page+="</div>\
              <div class=\"form-divs flex-col-2 cascurvesetting\" "+String(casCurveVisibility)+">\
                <input id=\"id_casCurveCoeff2\" class=\"curve\" name=\"casCurveCoeff2\" type=\"text\" value=\""+floatToString(casCurve.Items[2])+"\"/>\
              </div>\
              <div id=\"id_casCurveParam2\" class=\"form-divs flex-col-1 cascurvesetting\"  "+String(casCurveVisibility)+">";
              if (casCurve.curveType==1) page+=" *X<sup></sup>+ "; else
                  if (casCurve.curveType==2) page+="*ln(x)+"; else
                      if (casCurve.curveType==3) page+="* e^ (";
              page+="</div>\
              <div class=\"form-divs flex-col-2 cascurvesetting\"  "+String(casCurveVisibility)+">\
                <input id=\"id_casCurveCoeff3\" class=\"curve\" name=\"casCurveCoeff3\" type=\"text\" value=\""+floatToString(casCurve.Items[3])+"\"/>\
              </div>\
              <div id=\"id_casCurveParam3\" class=\"form-divs flex-col-1 cascurvesetting\"  "+String(casCurveVisibility)+">";
              if (casCurve.curveType==1) page+=""; else
                  if (casCurve.curveType==2) page+=""; else
                      if (casCurve.curveType==3) page+=" * x)";
              page+="</div>";

              // pressure ports orientation
              page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_portsOrientation\">Pressure ports orientation</label>\
                <select id=\"id_portsOrientation\" name=\"portsOrientation\">\
                  <option value=\"UP\""; if (portsOrientation=="UP") page+=" selected"; page+=">Up</option>\
                  <option value=\"DOWN\""; if (portsOrientation=="DOWN") page+=" selected"; page+=">Down</option>\
                  <option value=\"LEFT\""; if (portsOrientation=="LEFT") page+=" selected"; page+=">Left</option>\
                  <option value=\"RIGHT\""; if (portsOrientation=="RIGHT") page+=" selected"; page+=">Right</option>\
                  <option value=\"FORWARD\""; if (portsOrientation=="FORWARD") page+=" selected"; page+=">Forward</option>\
                  <option value=\"AFT\""; if (portsOrientation=="AFT") page+=" selected"; page+=">Aft</option>\
                </select>\
              </div>";
              
             // box top orientation
              page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_boxtopOrientation\">Box top Orientation</label>\
                <select id=\"id_boxtopOrientation\" name=\"boxtopOrientation\">\
                  <option value=\"UP\""; if (boxtopOrientation=="UP") page+=" selected"; page+=">Up</option>\
                  <option value=\"DOWN\""; if (boxtopOrientation=="DOWN") page+=" selected"; page+=">Down</option>\
                  <option value=\"LEFT\""; if (boxtopOrientation=="LEFT") page+=" selected"; page+=">Left</option>\
                  <option value=\"RIGHT\""; if (boxtopOrientation=="RIGHT") page+=" selected"; page+=">Right</option>\
                  <option value=\"FORWARD\""; if (boxtopOrientation=="FORWARD") page+=" selected"; page+=">Forward</option>\
                  <option value=\"AFT\""; if (boxtopOrientation=="AFT") page+=" selected"; page+=">Aft</option>\
                </select>\
              </div>";

             // efis settings
              page+="<div class=\"form-divs flex-col-6\">\
                <label for=\"id_readEfisData\">Serial EFIS data</label>\
                <select id=\"id_readEfisData\" name=\"readEfisData\">\
                  <option value=\"1\""; if (readEfisData) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!readEfisData) page+=" selected"; page+=">Disabled</option>\
                </select>\
              </div>\
              <div class=\"form-divs flex-col-6 efistypesetting\">\
                <label for=\"id_efisType\">EFIS Type</label>\
                <select id=\"id_efisType\" name=\"efisType\">\
                  <option value=\"DYNOND10\""; if (efisType=="DYNOND10") page+=" selected"; page+=">Dynon D10/D100</option>\
                  <option value=\"ADVANCED\""; if (efisType=="ADVANCED") page+=" selected"; page+=">SkyView/Advanced</option>\
                  <option value=\"GARMING5\""; if (efisType=="GARMING5") page+=" selected"; page+=">Garmin G5</option>\
                  <option value=\"GARMING3X\""; if (efisType=="GARMING3X") page+=" selected"; page+=">Garmin G3X</option>\
                  <option value=\"VN-300\""; if (efisType=="VN-300") page+=" selected"; page+=">VectorNav VN-300 GNSS/INS</option>\
                  <option value=\"VN-100\""; if (efisType=="VN-100") page+=" selected"; page+=">VectorNav VN-100 IMU/AHRS</option>\
                  <option value=\"MGL\""; if (efisType=="MGL") page+=" selected"; page+=">MGL iEFIS</option>\
                </select>\
              </div>";          
              
              // volume control
              String defaultVolumeVisibility;
              String volumeLevelsVisibility;
              String volumeControlWidth;              
              if (!volumeControl)
                      {
                      defaultVolumeVisibility="style=\"display:block\"";
                      volumeLevelsVisibility="style=\"display:none\"";
                      volumeControlWidth="6";                                            
                      } else
                            {
                            defaultVolumeVisibility="style=\"display:none\"";
                            volumeLevelsVisibility="style=\"display:block\"";
                            volumeControlWidth="9";
                            }
              
              page+="<div id=\"volumeControlDiv\" class=\"form-divs flex-col-"+volumeControlWidth+"\">\
                <label for=\"id_volumeControl\">Volume Potentiometer</label>\
                <select id=\"id_volumeControl\" name=\"volumeControl\">\
                  <option value=\"1\""; if (volumeControl) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!volumeControl) page+=" selected"; page+=">Disabled</option>\
                </select>\
              </div>";
              
              page+="<div class=\"form-divs flex-col-3 defaultvolumesetting\" "+String(defaultVolumeVisibility)+">\
                <label for=\"id_defaultVolume\">Volume %</label>\
                <input id=\"id_defaultVolume\" name=\"defaultVolume\" type=\"text\" value=\""+String(defaultVolume)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-3\">\
              <label for=\"id_volumeTestButton\">&nbsp</label>\
              <input id=\"id_volumeTestButton\" name=\"volumeTestButton\" type=\"button\" value=\"Test Audio\" class=\"greybutton\" onclick=\"getValue(this.id,\'AUDIOTEST\',\'\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-4 volumepossetting\" "+String(volumeLevelsVisibility)+">\
                <label for=\"id_volumeLowAnalog\">Low Vol. value</label>\
                <input id=\"id_volumeLowAnalog\" name=\"volumeLowAnalog\" type=\"text\" value=\""+String(volumeLowAnalog)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-2 volumepossetting\" "+String(volumeLevelsVisibility)+">\
              <label for=\"id_volumeLowRead\">&nbsp</label>\
              <input id=\"id_volumeLowRead\" name=\"volumeLowRead\" type=\"button\" value=\"Read\" class=\"greybutton\" onclick=\"getValue(this.id,\'VOLUME\',\'id_volumeLowAnalog\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-4 volumepossetting\" "+String(volumeLevelsVisibility)+">\
                <label for=\"id_volumeHighAnalog\">High Vol. value</label>\
                <input id=\"id_volumeHighAnalog\" name=\"volumeHighAnalog\" type=\"text\" value=\""+String(volumeHighAnalog)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-2 volumepossetting\" "+String(volumeLevelsVisibility)+">\
              <label for=\"id_volumeHighRead\">&nbsp</label>\
              <input id=\"id_volumeHighRead\" name=\"volumeHighRead\" type=\"button\" value=\"Read\" class=\"greybutton\" onclick=\"getValue(this.id,\'VOLUME\',\'id_volumeHighAnalog\')\"/>\
              </div>\
              <div class=\"form-divs flex-col-6\">\
                <label for=\"id_muteAudioUnderIAS\">Mute below IAS (kts)</label>\
                <input id=\"id_muteAudioUnderIAS\" name=\"muteAudioUnderIAS\" type=\"text\" value=\""+String(muteAudioUnderIAS)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-6\">\
                <label for=\"id_slipShiftAudio\">3D Audio</label>\
                <select id=\"id_slipShiftAudio\" name=\"audio3D\">\
                  <option value=\"1\""; if (audio3D) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!audio3D) page+=" selected"; page+=">Disabled</option>\
                </select>\
            </div>";
            // overgG audio warning
              page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_overgWarning\">OverG audio warning</label>\
                <select id=\"id_overgWarning\" name=\"overgWarning\">\
                  <option value=\"1\""; if (overgWarning) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!overgWarning) page+=" selected"; page+=">Disabled</option>\
                </select>\
              </div>";

              String loadLimitVisibility;
              if (overgWarning) loadLimitVisibility="style=\"display:block\""; else loadLimitVisibility="style=\"display:none\"";
              
              // loadLimit    
              page+="<div class=\"form-divs flex-col-6 loadlimitsetting\" "+String(loadLimitVisibility)+">\
                <label for=\"id_loadLimitPositive\">Positive G limit</label>\
                <input id=\"id_loadLimitPositive\" name=\"loadLimitPositive\" type=\"text\" value=\""+String(loadLimitPositive)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-6 loadlimitsetting\" "+String(loadLimitVisibility)+">\
                <label for=\"id_loadLimitNegative\">Negative G limit</label>\
                <input id=\"id_loadLimitNegative\" name=\"loadLimitNegative\" type=\"text\" value=\""+String(loadLimitNegative)+"\"/>\
              </div>";
              
            // vno chime
            String vnoVisibility;
            if (vnoChimeEnabled) vnoVisibility="style=\"display:block\""; else vnoVisibility="style=\"display:none\"";
            page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_vnoChimeEnabled\">Vno warning chime</label>\
                <select id=\"id_vnoChimeEnabled\" name=\"vnoChimeEnabled\">\
                  <option value=\"1\""; if (vnoChimeEnabled) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!vnoChimeEnabled) page+=" selected"; page+=">Disabled</option>\
                </select>\
            </div>\
            <div class=\"form-divs flex-col-5 vnochimesetting\" "+String(vnoVisibility)+">\
                <label for=\"id_Vno\">Aircraft Vno (kts)</label>\
                <input id=\"id_Vno\" name=\"Vno\" type=\"text\" value=\""+String(Vno)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-5 vnochimesetting\" "+String(vnoVisibility)+">\
                <label for=\"id_vnoChimeInterval\">Chime interval (seconds)</label>\
                <input id=\"id_vnoChimeInterval\" name=\"vnoChimeInterval\" type=\"text\" value=\""+String(vnoChimeInterval)+"\"/>\
              </div>\
              <div class=\"form-divs flex-col-2 vnochimesetting\" "+String(vnoVisibility)+">\
                <label for=\"id_chimeTestButton\">&nbsp</label>\
                <input id=\"id_chimeTestButton\" name=\"chimeTestButton\" type=\"button\" value=\"Test\" class=\"greybutton\" onclick=\"getValue(this.id,\'VNOCHIMETEST\',\'\')\"/>\
              </div>";
                              
              page+="<div class=\"form-divs flex-col-12\">\
                <label for=\"id_sdLogging\">SD Card Logging</label>\
                <select id=\"id_sdLogging\" name=\"sdLogging\">\
                  <option value=\"1\""; if (sdLoggingConfig) page+=" selected"; page+=">Enabled</option>\
                  <option value=\"0\""; if (!sdLoggingConfig) page+=" selected"; page+=">Disabled</option>\
                </select>\
              </div>";
              
               // serial output selection    
              page+="<div class=\"form-divs flex-col-6\">\
                <label for=\"id_serialOutFormat\">Serial out format</label>\
                <select id=\"id_serialOutFormat\" name=\"serialOutFormat\">\
                  <option value=\"G3X\""; if (serialOutFormat=="G3X") page+=" selected"; page+=">Garmin G3X</option>\
                  <option value=\"ONSPEED\""; if (serialOutFormat=="ONSPEED" || serialOutFormat=="") page+=" selected"; page+=">OnSpeed</option>\
                </select>\
              </div>";
              page+="<div class=\"form-divs flex-col-6\">\
                <label for=\"id_serialOutPort\">Serial out port</label>\
                <select id=\"id_serialOutPort\" name=\"serialOutPort\">\
                  <option value=\"NONE\""; if (serialOutPort=="NONE") page+=" selected"; page+=">None</option>\
                  <option value=\"Serial1\""; if (serialOutPort=="Serial1") page+=" selected"; page+=">Serial 1 (TTL - pin 12, v2 only!)</option>\
                  <option value=\"Serial3\""; if (serialOutPort=="Serial3") page+=" selected"; page+=">Serial 3 (RS232 - pin 12)</option>\
                  <option value=\"Serial5\""; if (serialOutPort=="Serial5") page+=" selected"; page+=">Serial 5 (TTL - pin 9)</option>\
                </select>\
              </div>";
              // load config file
              page+="<div class=\"form-divs flex-col-4\">\
                    <div class=\"upload-btn-wrapper\">\
                        <button class=\"upload-btn\">Upload File</button>\
                        <input id=\"id_fileUploadInput\" type=\"file\" name=\"configFileName\"/>\
                    </div>\
                </div>";
              
              // load default config
              page+="<div class=\"form-divs flex-col-5\">\
                  <a href=\"/defaultconfig\"><input id=\"id_loadDefaultConfig\" name=\"loadDefaultConfig\" type=\"button\" value=\"Load Defaults\" class=\"greybutton\"/></a>\
                  </div>";
               // save button
              page+="<div class=\"form-divs flex-col-3\">\
                <input class=\"redbutton\" type=\"submit\" name=\"saveSettingsButton\" value=\"Save\" />\
              </div>";
              
         // end of form     
         page+="</div>\
          </form>\
          <form id=\"id_realUploadForm\">\
          </form>\
          </div>";

// javascript code
        page+="<script>";
// hide efisType when efis data is disabled
        page+="document.getElementById(\'id_readEfisData\').onchange = function() {\
          if (document.getElementById(\'id_readEfisData\').value==1)\
          {\
          [].forEach.call(document.querySelectorAll(\'.efistypesetting\'), function (el) {el.style.visibility = \'visible\';});\
          } else\
              {\
              [].forEach.call(document.querySelectorAll(\'.efistypesetting\'), function (el) {el.style.visibility = \'hidden\';});\
              }\
        };";

// hide CAS curve when CAS Curve is disabled
        page+="document.getElementById(\'id_casCurveEnabled\').onchange = function() {\
          if (document.getElementById(\'id_casCurveEnabled\').value==1)\
          {\
          [].forEach.call(document.querySelectorAll(\'.cascurvesetting\'), function (el) {el.style.display = \'block\';});\
          } else\
              {\
              [].forEach.call(document.querySelectorAll(\'.cascurvesetting\'), function (el) {el.style.display = \'none\';});\
              }\
        };";
        
// hide volume levels and show default volume when volume control is disabled          
        page+="document.getElementById(\'id_volumeControl\').onchange = function() {\
          if (document.getElementById(\'id_volumeControl\').value==1)\
            {\
            [].forEach.call(document.querySelectorAll(\'.volumepossetting\'), function (el) {el.style.display = \'block\';});\
            [].forEach.call(document.querySelectorAll(\'.defaultvolumesetting\'), function (el) {el.style.display = \'none\';});\
            document.getElementById(\'volumeControlDiv\').classList.remove('flex-col-6');\
            document.getElementById(\'volumeControlDiv\').classList.add('flex-col-9');\
            } else\
                {\
                [].forEach.call(document.querySelectorAll(\'.volumepossetting\'), function (el) {el.style.display = \'none\';});\
                [].forEach.call(document.querySelectorAll(\'.defaultvolumesetting\'), function (el) {el.style.display = \'block\';});\
                document.getElementById(\'volumeControlDiv\').classList.remove('flex-col-9');\
                document.getElementById(\'volumeControlDiv\').classList.add('flex-col-6');\
                }\
        };";

// hide load limit when overG warning is disabled
      page+="document.getElementById(\'id_overgWarning\').onchange = function() {\
          if (document.getElementById(\'id_overgWarning\').value==1)\
          {\
          [].forEach.call(document.querySelectorAll(\'.loadlimitsetting\'), function (el) {el.style.display = \'block\';});\
          } else\
              {\
              [].forEach.call(document.querySelectorAll(\'.loadlimitsetting\'), function (el) {el.style.display = \'none\';});\
              }\
        };";       

// hide Vno settings if Vno chime is disabed
  page+="document.getElementById(\'id_vnoChimeEnabled\').onchange = function() {\
          if (document.getElementById(\'id_vnoChimeEnabled\').value==1)\
            {\
            [].forEach.call(document.querySelectorAll(\'.vnochimesetting\'), function (el) {el.style.display = \'block\';});\
            }\
             else\
                 {\
                 [].forEach.call(document.querySelectorAll(\'.vnochimesetting\'), function (el) {el.style.display = \'none\';});\
                 }\
          };";        
                   
// hide log file when replaylogfile not set
        page+="document.getElementById(\'id_dataSource\').onchange = function() {\
          if (document.getElementById(\'id_dataSource\').value==\'REPLAYLOGFILE\')\
          {\
          [].forEach.call(document.querySelectorAll(\'.replaylogfilesetting\'), function (el) {el.style.display = \'block\';});\
          } else\
              {\
              [].forEach.call(document.querySelectorAll(\'.replaylogfilesetting\'), function (el) {el.style.display = \'none\';});\
              }\
        };";
// upload config file
// some Javascript trickery here, we're creating a second file upload form (id_realUploadForm) and submitting that with JS.
        page+="document.getElementById(\'id_fileUploadInput\').onchange = function() {\
            if (document.getElementById(\'id_fileUploadInput\').value.indexOf(\".cfg\")>0)\
                {\
                 document.getElementById(\'id_realUploadForm\').appendChild(document.getElementById(\'id_fileUploadInput\'));\
                 document.getElementById(\'id_realUploadForm\').action=\"/aoaconfigupload\";\
                 document.getElementById(\'id_realUploadForm\').enctype=\"multipart/form-data\";\
                 document.getElementById(\'id_realUploadForm\').method=\"POST\";\
                 document.getElementById(\'id_realUploadForm\').submit();\
                }\
                  else alert(\"Please upload a config file with .cfg extension!\");\
        };";
// curve type parameter change
          page+="function curveTypeChange(senderId,curveId)\
               {\
               if (document.getElementById(senderId).value==1)\
                {\
                document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+0).innerHTML=\' *X<sup>3</sup>+ \';\
                document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+1).innerHTML=\' *X<sup>2</sup>+ \';\
                document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+2).innerHTML=\' *X<sup></sup>+ \';\
                document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+3).innerHTML=\'\';\
                } else\
                    if (document.getElementById(senderId).value==2)\
                       {\
                       document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+0).innerHTML=\' * 0 + \';\
                       document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+1).innerHTML=\' * 0 + \';\
                       document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+2).innerHTML=\'*ln(x)+\';\
                       document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+3).innerHTML=\'\';\
                       } else\
                          if (document.getElementById(senderId).value==3)\
                            {\
                            document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+0).innerHTML=\' * 0 + \';\
                            document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+1).innerHTML=\' * 0 + \';\
                            document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+2).innerHTML=\'* e^ (\';\
                            document.getElementById(\'id_aoaCurve\'+curveId+\'Param\'+3).innerHTML=\' * x)\';\
                            }\
               }";
// cas curve type parameter change
          page+="function cascurveTypeChange(senderId)\
               {\
               if (document.getElementById(senderId).value==1)\
                {\
                document.getElementById(\'id_casCurveParam0\').innerHTML=\' *X<sup>3</sup>+ \';\
                document.getElementById(\'id_casCurveParam1\').innerHTML=\' *X<sup>2</sup>+ \';\
                document.getElementById(\'id_casCurveParam2\').innerHTML=\' *X<sup></sup>+ \';\
                document.getElementById(\'id_casCurveParam3\').innerHTML=\'\';\
                } else\
                    if (document.getElementById(senderId).value==2)\
                       {\
                       document.getElementById(\'id_casCurveParam0\').innerHTML=\' * 0 + \';\
                       document.getElementById(\'id_casCurveParam1\').innerHTML=\' * 0 + \';\
                       document.getElementById(\'id_casCurveParam2\').innerHTML=\'*ln(x)+\';\
                       document.getElementById(\'id_casCurveParam3\').innerHTML=\'\';\
                       } else\
                          if (document.getElementById(senderId).value==3)\
                            {\
                            document.getElementById(\'id_casCurveParam0\').innerHTML=\' * 0 + \';\
                            document.getElementById(\'id_casCurveParam1\').innerHTML=\' * 0 + \';\
                            document.getElementById(\'id_casCurveParam2\').innerHTML=\'* e^ (\';\
                            document.getElementById(\'id_casCurveParam3\').innerHTML=\' * x)\';\
                            }\
               }";

               
// disable Delete Flap Position on Enter key
          page+="document.getElementById(\"id_configForm\").onkeypress = function(e)\
              {\
              var key = e.charCode || e.keyCode || 0;\
              if (key == 13) {\
                            e.preventDefault();\
                           }\
              }\
         </script>";
         

// add html footer
   page+=pageFooter;
   server.send(200, "text/html", page);
}

void handleConfigUpload()
{
String page="";
updateHeader();
page+=pageHeader;  
// check if a configfile was uploaded
HTTPUpload& uploadFile = server.upload();
if(uploadFile.status == UPLOAD_FILE_START)
      {
      uploadConfigString="";
      } else   
            if (uploadFile.status == UPLOAD_FILE_WRITE)
              {
              for (unsigned int i;i<uploadFile.currentSize;i++)
                {
                uploadConfigString+=char(uploadFile.buf[i]);
                }
              } else  if (uploadFile.status == UPLOAD_FILE_END)
                          {
                          //done uploading, verify content 
                          handleConfig();                                 
                          }
}


void handleWifiSettings()
  {
   String page="";
   int rSSI;
   int signalStrength;
   //add header before sending page, with updated Wifi Status
  if (server.arg("disconnect").indexOf("yes")>=0)
      {     
      // if connected disconnect
                if (WiFi.status() == WL_CONNECTED)
                    {
                    WiFi.disconnect(true);
                    delay(100);
                    }
      }

  
  
  if (server.arg("connect").indexOf("yes")<0)
    {
    // display connection status and available networks
     if (WiFi.status() == WL_CONNECTED)
                    {
                    page+="<br><br>Wifi Status: Connected to <strong>"+clientwifi_ssid+"</strong>";
                    page+="<br>";
                    page+="IP Address: "+ WiFi.localIP().toString()+"<br><br>";
                    page+="<div align=\"center\"><a href=\"wifi?disconnect=yes\" class=\"button\">Disconnect</a></div><br><br>";
                    }
                    else
                        {
                        page+="<br><br><span style=\"color:red\">Wifi Status: Disconnected</span>";
                        }
    page+="<br><br>Tap a Wifi network below to connect to it:<br><br>\n";
    int n = WiFi.scanNetworks();
    if (n == 0) {
                page+="<br><br>No Wifi networks found.<br><br>Reload this page to scan again.";
                }
                else
                    {
                     page+="<div align=\"center\">\n";
                     for (int i = 0; i < n; ++i)
                          {                         
                          
                          rSSI=WiFi.RSSI(i);
                          if (rSSI > -50) signalStrength=4; else if (rSSI <= -50 && rSSI > -60) signalStrength=3; else if (rSSI <= -60 && rSSI >= -70) signalStrength=2; else signalStrength=1;
                          page+="<a href=\"wifi?connect=yes&ssid="+WiFi.SSID(i);
                          if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) page+="&auth=yes"; else page+="&auth=no";
                          page+="\" class=\"wifibutton\">"+WiFi.SSID(i)+"<i class=\"icon__signal-strength signal-"+String(signalStrength)+"\"><span class=\"bar-1\"></span><span class=\"bar-2\"></span><span class=\"bar-3\"></span><span class=\"bar-4\"></span></i>";
                          if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) page+="&#128274"; else page+="&#128275";
                          page+="</a>";
                          page+="<br>";                          
                          }
                    page+="</div>\n";      
                    page+="<br><br>\n";
                    page+="&#128274 - password required to connect\n";
                    }
    } else 
          {
          // connect to network
          if (server.arg("auth").indexOf("yes")>=0)
            {
            // require network password
            page+="<br><br>Enter password for "+server.arg("ssid");
            page+="<br><br>";
            page+="<div align=\"center\"> <form action=\"wifi?connect=yes&ssid="+server.arg("ssid")+"\" method=\"POST\"> <input type=\"text\" class=\"inputField\" name=\"password\" placeholder=\"Enter password\"><br><br>\
            <button type=\"submit\" class=\"button\">Connect</button> </form> <br></div>";
            }
             else
                {
                // connect to network
                clientwifi_ssid=server.arg("ssid");
                clientwifi_password=server.arg("password");
                // convert string to char*
                char __clientwifi_ssid[clientwifi_ssid.length()+1];
                char __clientwifi_password[clientwifi_password.length()+1];
                clientwifi_ssid.toCharArray(__clientwifi_ssid,sizeof(__clientwifi_ssid));
                clientwifi_password.toCharArray(__clientwifi_password,sizeof(__clientwifi_password));
                Serial.println("Connecting to Wifi");
                // if connected disconnect
                if (WiFi.status() == WL_CONNECTED)
                    {                
                      unsigned long timeStart=millis();
                      WiFi.disconnect(true);
                      // wait for disconnect to complete
                      while (WiFi.status() == WL_CONNECTED)
                          {                   
                          delay(100);
                          if (millis()-timeStart>20000) break;
                          }
                    }
                // connect to network    
                WiFi.begin(__clientwifi_ssid, __clientwifi_password);
                delay(100);
                unsigned long timeStart=millis();
                while (WiFi.status()!= WL_CONNECTED)
                      {
                      delay(500);
                      if (millis()-timeStart>20000) break;
                      }
                      
//                page+="password:"+clientwifi_password;
                if (WiFi.status() == WL_CONNECTED)
                    {
                    Serial.println("Connected");
                    page+="<br><br>Connected to network: <strong>"+clientwifi_ssid+"</strong>";
                    page+="<br><br>";
                    page+="IP Address: "+WiFi.localIP().toString();
                    }
                    else
                        {
                        page+="<br><br><span style=\"color:red\">Error: Could not connect to network: <strong>"+clientwifi_ssid+"</strong></span>";
                        }
                }
          }     
   updateHeader();
   page=pageHeader+page;                
   page+=pageFooter;
   server.send(200, "text/html", page);   
  }  


void handleReboot()
  {
   String page="";
   updateHeader();
   page+=pageHeader;    
  if (server.arg("confirm").indexOf("yes")<0)
    {
    // display confirmation page 
    page+="<br><br><p style=\"color:red\">Confirm that you want to reboot OnSpeed.</p>\
    <br><br><br>\
    <a href=\"reboot?confirm=yes\" class=\"button\">Reboot</a>\
    <a href=\"/\">Cancel</a>";
    page+=pageFooter;
    server.send(200, "text/html", page);
    } else 
          {
          // reboot system
          Serial.println("$REBOOT!"); // send reboot command to Teensy

          // wait for ACK
          String rebootResponse="";
          unsigned long rebootStartTime=millis();
          while (true)
                {
                if (Serial.available()>0) 
                    {
                     char inChar=Serial.read();
                     rebootResponse+=inChar;                                  
                     if (rebootResponse.indexOf("<REBOOT>OK</REBOOT>")>=0)                     
                        {
                        page+="<br><br><p>OnSpeed is rebooting. Wait a few seconds to reconnect.</p>";
                        page+="<script>setTimeout(function () { window.location.href = \"/\";}, 7000);</script>"; 
                        break;
                        }                                          
                    }
                // timeout
                if ((millis()-rebootStartTime)>3000)
                    {      
                    // reboot timeout
                     page+="<br><br><p>Haven't heard back from the main CPU. Please power cycle to reboot.</p>";
                     page+="Response: "+rebootResponse;
                    break; 
                    }
                }
          
          
          page+=pageFooter;
          server.send(200, "text/html", page);
          delay(1000);          
          ESP.restart();
          }     
  }


void handleLogs() {
Serial.println("$STOPLIVEDATA!");
delay(1000);
Serial.flush();
Serial.println("$LIST!");
    
String content=""; 

unsigned long starttime=millis();
long bytecount=0;
  while (true)
  {
  if (Serial.available()) 
      {
       char inChar=Serial.read();
       content+=inChar;
       bytecount++;
       starttime=millis();       
       //counter++;
        if (bytecount % 20==0)
        {
        //Serial.println(micros()-startmicros);
        Serial.print('.');
        } 
      if (content.indexOf("<eof>")>0) break;
      }
  if ((millis()-starttime)>2000)
      {      
      // SD read timeout 
      String page=pageHeader;
      page+="<br><br>Couldn't get list of files on the Teensy's SD card. TIMEOUT while communicating with the Teensy via serial. Reboot and try again"
      "<br><br>";
      page+=pageFooter;
      server.send(200, "text/html", page);       
      return;
      }
  }
String filenameList="";
if (content.indexOf("<filelist>")>=0 && content.indexOf("<eof>")>=0)
  { 
  bool isFilename=true;
  // process content
  String filename="";
  String filesize="";
  for (int i=10;i<content.length();i++)
      {
      if (content[i]!=0x0D && content[i]!=0x0A)
        {
         if (content[i]==':') {
                              isFilename=false;
                              } else
                                    {                                        
                                    if (isFilename) filename+=content[i]; else filesize+=content[i];
                                    }
        
        } else          
          if (filename.length()>0)
              {
              filesize+=content[i];
              isFilename=true;
              filenameList+="<a href=\"";
              filenameList+=filename;
              filenameList+="?length=";
              filenameList+=filesize;
              filenameList+="\">";
              filenameList+=filename;
              filenameList+="</a>";
              filenameList+=" ";
              char filesize_char[12];
              sprintf(filesize_char, "%'ld",filesize.toInt());              
              filenameList+=formatBytes(filesize.toInt());
              filenameList+="<br>";
              filename="";
              filesize="";
              }
      }
  
  } else filenameList="No files found.";
    
  String page;
  updateHeader();
  page+=pageHeader;
  page+="<p>Available log files:</p>";  
  page+=filenameList;  
  page+=pageFooter;
  
  server.send(200, "text/html", page);
}

bool handleIndex()
{
  String page;
   updateHeader();
   page+=pageHeader;
   page+="<br><br>\n"
"<strong>Welcome to the OnSpeed Wifi gateway.</strong><br><br>\n"
"General usage guidelines:<br>\n"
"<br>\n"
"- Connect from one device at a time<br>\n"
"- Visit one page at a time<br>\n"
"- During log file downloads data recording and tones are disabled<br>\n"
"- LiveView is for debugging purposes only. Do not use for flight.<br>\n";  
  page+=pageFooter;
  
  server.send(200, "text/html", page);
}


bool handleFileRead(String path) {  
 
  String lengthString=server.arg("length"); 
  if (lengthString=="")
      {
      server.send(500, "text/plain", "Invalid Request");
                  return true;
      }
  String fileName=path.substring(1,path.indexOf("?")-1);
  unsigned long contentLength=atol(lengthString.c_str());
  
  Serial.print("$PRINT ");
  Serial.print(fileName); // skip the slash in front of the file
  Serial.println("!"); // finish the print command to grab the file via serial 
  server.setContentLength(contentLength);
  server.sendHeader("Content-Disposition", "attachment; filename="+fileName);
  if (fileName.indexOf(".csv")>=0) server.send(200, "text/csv", ""); // send empty string (headers only)
      else server.send(200, "text/plain", "");
  
  Serial.end();
  delay(100);
  Serial.begin(BAUDRATE_WIFI_HS); // increase wifi baudrate for file transfer
  Serial.setRxBufferSize(2048);
  delay(200);
  unsigned long starttime=millis();
  long bytecount=0;
  String sendBuffer;
  while (bytecount<contentLength)
  {
  if (Serial.available()) 
      {
       char inChar=Serial.read();
       if (inChar==0xFF) continue;
       sendBuffer+=inChar;       
       bytecount++;
       if (bytecount==contentLength && sendBuffer.indexOf("<404>")>=0)
                  {
                  // file does not exist
                  server.send(404, "text/plain", "File Not Found");
                  Serial.end();
                  Serial.begin(BAUDRATE_WIFI);
                  delay(200);
                  return true;
                  } 
       if (bytecount % 2048==0)
        {
        server.sendContent(sendBuffer);
        Serial.print('.'); // send an ACK back to Teensy
        sendBuffer="";          
        }
       starttime=millis();
      }
  if ((millis()-starttime)>5000)
      {      
      // SD read timeout 
      Serial.end();
      Serial.begin(BAUDRATE_WIFI);
      delay(200);
      return false;
      }  
  }
  Serial.end();
  Serial.begin(BAUDRATE_WIFI);   // decrease wifi baudrate after file transfer        
  delay(200);
  if (sendBuffer.length()>0) server.sendContent(sendBuffer); // send the remaining characters in buffer
  return true;
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

void setup() {
Serial.begin(BAUDRATE_WIFI);
Serial.setDebugOutput(false);

 //WIFI INIT
 //Serial.println("Starting Access Point");
 WiFi.mode(WIFI_AP_STA);
 //WiFi.disconnect();
 WiFi.softAP(ssid, password);
 delay(100); // wait to init softAP  
 IPAddress Ip(192, 168, 0, 1);
 IPAddress NMask(255, 255, 255, 0);
 WiFi.softAPConfig(Ip, Ip, NMask);  
 IPAddress myIP = WiFi.softAPIP();
 // Serial.print("AP IP address: ");
 // Serial.println(myIP);
 delay(100);

  //SERVER INIT
  
  //index page
  server.on("/", HTTP_GET, handleIndex);

  //format card page
  server.on("/format", HTTP_GET, handleFormat);
  
  server.on("/reboot", HTTP_GET, handleReboot);

  server.on("/wifireflash", HTTP_GET, handleWifiReflash);

  server.on("/upgrade", HTTP_GET, handleUpgrade);

  server.on("/live", HTTP_GET, handleLive);
  
  server.on("/favicon.ico", HTTP_GET, handleFavicon);
  
  server.on("/logs", HTTP_GET, handleLogs);

  server.on("/wifi", HTTP_GET, handleWifiSettings);

  server.on("/sensorconfig", HTTP_GET, handleSensorConfig); 

  server.on("/aoaconfig", HTTP_GET, handleConfig); 
  
  server.on("/aoaconfigupload", HTTP_POST, []() {server.send(200);},handleConfigUpload);
  
  server.on("/calwiz", HTTP_GET, handleCalWizard);
    server.on("/calwiz", HTTP_POST, handleCalWizard);  
    
  server.on("/aoaconfigsave", HTTP_POST, handleConfigSave);

  server.on("/defaultconfig", HTTP_GET, handleDefaultConfig);

  server.on("/getvalue",HTTP_GET,handleGetValue);

  /*handling uploading firmware file */
  server.on("/upload", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    if (Update.hasError()) handleUpgradeFailure(); else handleUpgradeSuccess(); 
    //server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(5000);   
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
   if (upload.status == UPLOAD_FILE_START) {
      //Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        //Update.printError(Serial);
      }
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
     if (!handleFileRead(server.uri())) 
      server.send(404, "text/plain", "FileNotFound");
      });
  
  // start server
  server.begin(); 
  //start websockets (live data display)
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  if (MDNS.begin("onspeed")) MDNS.addService("http", "tcp", 80);  
  WiFi.setTxPower(WIFI_POWER_2dBm);
} // setup


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            if (webSocket.connectedClients(true)==0) Serial.println("$STOPLIVEDATA!"); // tell Teensy to stop sending live data if there are no more connected clients
            break;
        case WStype_CONNECTED:
            {
            Serial.println("$STARTLIVEDATA!"); // tell Teensy to start sending live data                         
            }
            break;
           } // switch

}

String getTeensyVersion()
{
// get current version from teensy    
    Serial.flush();
    Serial.println("$VERSION!");
    String responseString="";
    unsigned long starttime=millis();
      while (true)
      {
      if (Serial.available()>0) 
          {
           char inChar=Serial.read();
           responseString+=inChar;       
           starttime=millis();       
           if (responseString.indexOf("</VERSION>")>0)
                {
                return getConfigValue(responseString,"VERSION");
                }
          }
      if ((millis()-starttime)>2000)
          {      
          return "";
          }
      }
}      

void loop() {
  server.handleClient();
  webSocket.loop();
  
// parse live data from the Teensy via Serial
if (Serial.available())
  {
  // data format
  // $ONSPEED,AOA,PercentLift,IAS,PAlt,GLoad,CRC   
  int maxCharsInLine=350;
  char serialChar = Serial.read();   
  if (serialChar!='$' && serialChar!=char(0x0D) && serialChar!=char(0x0A) && serialBufferSize < maxCharsInLine)
    {    
    serialBuffer[serialBufferSize]=serialChar;
    serialBufferSize++;
    } else
          {
           if (serialChar=='$')  // catch start of json data line
              {
              // start of new line, drop the buffer
              serialBufferSize=0;
              serialBuffer[serialBufferSize]=serialChar; // add the $ sign
              serialBufferSize++;
              } else
            if (serialChar==char(0x0A))
            {
            // we have the full line
            String dataString;
            for (int k=0;k<serialBufferSize;k++)
                {
                dataString+=serialBuffer[k];
                }            
            // verify if line starting with $ONSPEED
            if (dataString.startsWith("$ONSPEED"))
              {              
              // send to websocket if there are any clients waiting
              if (webSocket.connectedClients(false)>0)  webSocket.broadcastTXT(dataString);
              }            
            // drop buffer after sending
            serialBufferSize=0;
            }
           if (serialBufferSize >= maxCharsInLine)
              {
              // too much junk received, drop buffer
              serialBufferSize=0;               
              }
          }
  
  }
 
}
