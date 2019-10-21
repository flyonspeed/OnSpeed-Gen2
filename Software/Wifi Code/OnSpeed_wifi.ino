
/* OnSpeed Wifi - Wifi file server and debug display for ONSPEED Gen 2 v2 box.
 *  by Lenny Iszak, 9/11/2019, lenny@flyonspeed.org
 */
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <Update.h>
#include <WebSocketsServer.h>
#include <StreamString.h>

#define BAUDRATE_WIFI 115200
#define BAUDRATE_WIFI_HS 1240000

String wifi_fw="1.2.12"; // wifi firmware version

const char* ssid = "OnSpeed";
const char* password = "angleofattack";

// initialize live display values
float AOA=0.0;
float PercentLift=0.0;
float IAS=0.0;
float PAlt=0.0;
float GLoad=0.00;
float CRC;
long liveDataAge=0; // millisec
unsigned long liveDataStartTime=millis();
int serialBufferSize=0;
char serialBuffer[200];

// initialize websocket server (live data display)
WebSocketsServer webSocket = WebSocketsServer(81);


typedef struct
  {
  char filename[11];
  long filesize; 
  } filetype;

String pageHeader="<html>\n"
"  <head>\n"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \n"
"    <title>OnSpeed Gen2 WiFi gateway</title>\n"
"    <style>\n"
"      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n"
"     ul {\n"
"  list-style-type: none;\n"
"  margin: 0;\n"
"  padding: 0;\n"
"  overflow: hidden;\n"
"  background-color: #333;\n"
"}\n"
" li {\n"
"   float: left;\n"
"}\n"
"li a, .dropbtn {\n"
"  display: inline-block;\n"
"  color: white;\n"
"  text-align: center;\n"
"  padding: 14px 16px;\n"
"  text-decoration: none;\n"
"}\n"
"li a:hover, .dropdown:hover .dropbtn {\n"
"  background-color: red;\n"
"}\n"
"li.dropdown {\n"
"  display: inline-block;\n"
"}\n"
".dropdown-content {\n"
"  display: none;\n"
"  position: absolute;\n"
"  background-color: #f9f9f9;\n"
"  min-width: 160px;\n"
"  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);\n"
"  z-index: 1;\n"
"}\n"
".dropdown-content a {\n"
"  color: black;\n"
"  padding: 12px 16px;\n"
"  text-decoration: none;\n"
"  display: block;\n"
"  text-align: left;\n"
"}\n"
".dropdown-content a:hover {background-color: #f1f1f1}\n"
".dropdown:hover .dropdown-content {\n"
"  display: block;\n"
"}\n"
".button {\n"
"  background-color: red;\n"
"  border: none;\n"
"  color: white;\n"
"  padding: 15px 32px;\n"
"  text-align: center;\n"
"  text-decoration: none;\n"
"  display: inline-block;\n"
"  font-size: 16px;\n"
"  margin: 4px 25px;\n"
"  cursor: pointer;\n"
"}\n"
"    </style>\n"
"  </head>\n"
"<body>\n"
"  <span style=\"font-size:36px;font-weight:bold;font-family:helvetica;color:black\"><span style=\"color:red\">on</span>SPEED</span>\n"
"  <span style=\"font-size:9px\"> FW:"+wifi_fw+"</span>\n"
"  <ul>\n"
"  <li><a href=\"/\">Home</a></li>\n"
"  <li class=\"dropdown\"> \n"
"   <a href=\"javascript:void(0)\" class=\"dropbtn\">Tools</a>\n"
"   <div class=\"dropdown-content\">\n"
"   <a href=\"format\">Format SD Card</a>\n"
"   <a href=\"upgrade\">Upgrade Wifi module firmware via Wifi upload</a>\n"
"   <a href=\"wifireflash\">Upgrade Wifi module via USB/Arduino</a>\n"
"   <a href=\"reboot\">Reboot</a>\n"
"   </div>\n"
"  </li>\n"
"  <li><a href=\"live\">LiveView</a></li>\n"
"  <li><a href=\"logs\">Log Files</a></li> \n"
"    </ul>\n";
 String pageFooter="</body></html>";   

WebServer server(80);


void handleWifiReflash()
  {
  String page="";
  page+=pageHeader;
  page+="<br><br><p>Wifi reflash mode disables the Teensy's serial port to enable reflashing the Wifi chip via its microUSB port.\
  <br><br><span style=\"color:red\">This mode is now activated until reboot.</span></p>";
  page+=pageFooter;
  Serial.println("$WIFIREFLASH!");
  server.send(200, "text/html", page); 
  }

void handleUpgrade()
  {
  String page="";
  page+=pageHeader;
  page+="<br><br><p>Upgrade Wifi module firmware via binary (.bin) file upload\
  <br><br><br>";
  page+="<form method='POST' action='/upload' enctype='multipart/form-data' id='upload_form'>\
   <input type='file' name='update'>\
   <input type='submit' value='Upload'>\
    </form>";
  page+=pageFooter; 
  
  server.send(200, "text/html", page); 
  }

void handleUpgradeSuccess()
  {
  String page="";
  page+=pageHeader;
  page+="<br><br><br><br><span style=\"color:black\">Firmware upgrade complete. Wait a few seconds until OnSpeed reboots.</span></p><br><br><br><br>";
   page+=pageFooter;   
  server.send(200, "text/html", page); 
  }
   
void handleUpgradeFailure()
  {
  String page="";
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
  page+=pageHeader;
  page+="<style>\n"
"html, body { margin:0; padding: 0 5px; overflow:hidden;min-height:100% }\n"
"#container {width:100%}  \n"
"svg {position:absolute;margin:0 auto; display: block;height: 70vh; width:100%}\n"
"#datafields {position:relative; margin-top:5px;font-family: \"Arial, Helvetica, sans-serif\";font-size:16px; left:0px;}\n"
"#footer-warning {display: block;text-align: center;position:fixed;bottom: 0px; margin-left:-10px; height:20px; width: 100%;font-family: \"Arial, Helvetica, sans-serif\";font-size:12px;background-color:white}\n"
"/*! XS */\n"
"@media (orientation: portrait) {\n"
"  svg {  margin: 0 auto 0 20px; }\n"
"}\n"
"@media (orientation: landscape) {\n"
"  svg { height: 60vh;}  \n"
"}\n"
"</style>\n"
"<script language=\"javascript\" type=\"text/javascript\">\n"
" var wsUri = \"ws://onspeed.local:81\";\n"
" var lastUpdate= Date.now();\n"
" var lastDisplay=Date.now(); \n"
" var AOA=0;\n"
" var PercentLift=0;\n"
" var IAS=0;\n"
" var PAlt=0;\n"
" var GLoad=1;\n"
" var smoothingAlpha=.8;\n"
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
"  }\n"
"\n"
"  function onClose(evt)\n"
"  {\n"
"    writeToStatus(\"DISCONNECTED. Reconnecting...\");    \n"
"    setTimeout(connectWebSocket, 2000);  \n"
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
"   OnSpeed = JSON.parse(evt.data);\n"
"   var crc_string=OnSpeed.AOA.toFixed(2)+','+OnSpeed.PercentLift.toFixed(2)+','+OnSpeed.IAS.toFixed(2)+','+OnSpeed.PAlt.toFixed(2)+','+OnSpeed.GLoad.toFixed(2)+','+OnSpeed.FlapsPercent+','+OnSpeed.PercentLiftLDmax.toFixed(2);\n"
"   var crc_calc=0;\n"
"   for (i=0;i<crc_string.length;i++)\n"
"       {\n"
"       crc_calc+=crc_string.charCodeAt(i);\n"
"     crc_calc=crc_calc & 0xFF;\n"
"     }\n"
"    if (OnSpeed.CRC=crc_calc)\n"
"    {\n"
"    // CRC ok, use values\n"
"    AOA=(OnSpeed.AOA*smoothingAlpha+AOA*(1-smoothingAlpha)).toFixed(2);\n"
"    PercentLift=OnSpeed.PercentLift.toFixed(2);\n"
"    IAS=(OnSpeed.IAS*smoothingAlpha+IAS*(1-smoothingAlpha)).toFixed(2);\n"
"    PAlt=(OnSpeed.PAlt*smoothingAlpha+PAlt*(1-smoothingAlpha)).toFixed(2);\n"
"    GLoad=(OnSpeed.GLoad*smoothingAlpha+GLoad*(1-smoothingAlpha)).toFixed(2);\n"
" FlapsPercent=OnSpeed.FlapsPercent;\n"
" PercentLiftLDmax=OnSpeed.PercentLiftLDmax;\n"
"    lastUpdate=Date.now();   \n"
"    // move AOA line on display    \n"
"   if (PercentLift>=0 && PercentLift<55)\n"
"     {\n"
"     var aoaline_y=map(PercentLift, 0, 55, 278, 178);     \n"
"     } else\n"
"     if (PercentLift>=55 && PercentLift<=60)\n"
"       {\n"
"       aoaline_y=map(PercentLift, 55, 60, 178, 145);\n"
"       } else\n"
"       if (PercentLift>60 && PercentLift<=65)\n"
"         {\n"
"         aoaline_y=map(PercentLift, 60, 65, 145, 113);  \n"
"         } else \n"
"           aoaline_y=map(PercentLift, 65, 100, 113, 15);\n"
" \n"
"    document.getElementById(\"aoaline\").setAttribute(\"y\", aoaline_y);\n"
"// calc ldmax dot locations\n"
" ldmax_y=map(PercentLiftLDmax, 0, 55,  -281.5, -181.2);\n"
" document.getElementById(\"ldmaxleft\").setAttribute(\"cy\", ldmax_y);\n"
" document.getElementById(\"ldmaxright\").setAttribute(\"cy\", ldmax_y);\n"
"// scale flaps dot\n"
" document.getElementById(\"flaps\").setAttribute(\"r\",FlapsPercent*18/100); // scale green flaps dot\n"
"   \n"
"} // if CRC\n"
"    \n"
"   } catch (e) {\n"
"           console.log('JSON parsing error:'+e.name+': '+e.message);\n"
"           }         \n"
"   \n"
"   if (Date.now()-lastDisplay>=500)\n"
"       {\n"
"       if (AOA > -20) document.getElementById(\"aoa\").innerHTML=AOA; else document.getElementById(\"aoa\").innerHTML='N/A';\n"
"       if (PercentLift>=0) document.getElementById(\"liftpercent\").innerHTML=PercentLift; else document.getElementById(\"liftpercent\").innerHTML='N/A';\n"
"       document.getElementById(\"ias\").innerHTML=IAS +' kts';\n"
"       document.getElementById(\"palt\").innerHTML=PAlt+' ft';\n"
"       document.getElementById(\"gload\").innerHTML=GLoad+' G';      \n"
"   lastDisplay=Date.now(); \n"
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
"   }\n"
"  }\n"
" \n"
" function writeToStatus(message)\n"
"  {\n"
"    var status = document.getElementById(\"status\");\n"
"    status.innerHTML = message;\n"
"  }  \n"
"\n"
"window.addEventListener(\"load\", function() {init();}, false);  \n"
"</script>\n"
"<div id=\"container\">\n"
"  <svg version=\"1.1\" viewBox=\"0 0 210 297\">\n"
" <g>\n"
"  <circle cx=\"98.171\" cy=\"787.76\" r=\"133.84\" fill=\"#07a33f\" stroke-width=\"1.8089\"/>\n"
"  <rect transform=\"rotate(-15.029)\" x=\"50.364\" y=\"33.747\" width=\"15.615\" height=\"89.228\" fill=\"#cc3837\"/>\n"
"  <rect transform=\"matrix(-.9658 -.2593 -.2593 .9658 0 0)\" x=\"-148.06\" y=\"-19.129\" width=\"15.615\" height=\"89.228\" fill=\"#cc3837\"/>\n"
"  <rect transform=\"matrix(.9658 .2593 .2593 -.9658 0 0)\" x=\"127.53\" y=\"-255.15\" width=\"15.615\" height=\"89.228\" fill=\"#f49421\"/>\n"
"  <rect transform=\"rotate(164.97)\" x=\"-70.153\" y=\"-307.83\" width=\"15.615\" height=\"89.228\" fill=\"#f49421\"/>\n"
"  <path d=\"m101.8 113.26c-17.123 0-31.526 12.231-34.92 28.385 5.203-2.6e-4 10.398 1.6e-4 15.598 0 2.9178-7.8426 10.401-13.371 19.323-13.371 8.922 0 16.413 5.5289 19.336 13.371 5.198 1.6e-4 10.403-2.8e-4 15.602 0-3.397-16.154-17.815-28.385-34.938-28.385zm-35.121 41.774c2.9176 16.746 17.577 29.609 35.121 29.609 17.544 0 32.218-12.863 35.138-29.609-5.1216-2.8e-4 -10.25 1.6e-4 -15.371 0-2.5708 8.4824-10.391 14.574-19.767 14.574-9.3764 0-17.183-6.0913-19.75-14.574-5.1246 1.4e-4 -10.244-2.8e-4 -15.371 0z\" color=\"#000000\" color-rendering=\"auto\" dominant-baseline=\"auto\" fill=\"#07a33f\" image-rendering=\"auto\" shape-rendering=\"auto\" solid-color=\"#000000\" style=\"font-feature-settings:normal;font-variant-alternates:normal;font-variant-caps:normal;font-variant-ligatures:normal;font-variant-numeric:normal;font-variant-position:normal;isolation:auto;mix-blend-mode:normal;paint-order:markers fill stroke;shape-padding:0;text-decoration-color:#000000;text-decoration-line:none;text-decoration-style:solid;text-indent:0;text-orientation:mixed;text-transform:none;white-space:normal\"/>\n"
"  <circle id=\"flaps\" cx=\"101.8\" cy=\"148.91\" r=\"0\" fill=\"#07a33f\" stroke-width=\".81089\" visibility=\"visible\"/>\n"
" </g>\n"
" <g fill=\"#241f1c\">\n"
"  <rect id=\"aoaline\" x=\"52.187\" y=\"144.91\" width=\"100\" height=\"6.6921\" style=\"paint-order:markers fill stroke\"/>\n"
"  <circle id=\"ldmaxleft\" transform=\"scale(1,-1)\" cx=\"46.801\" cy=\"-208.55\" r=\"3.346\" stroke-width=\"1.0419\"/>\n"
"  <circle id=\"ldmaxright\" transform=\"scale(1,-1)\" cx=\"157.53\" cy=\"-208.55\" r=\"3.346\" stroke-width=\"1.0419\"/>\n"
" </g>\n"
"</svg>\n"
"  <div id=\"datafields\"> Status: <span id=\"status\"></span> <br>\n"
"    AOA: <span id=\"aoa\"></span> <br>\n"
"    Lift %: <span id=\"liftpercent\"></span> <br>\n"
"    IAS: <span id=\"ias\"></span> <br>\n"
"    P Alt: <span id=\"palt\"></span> <br>\n"
"    G Load: <span id=\"gload\"></span> <br>\n"
"    Age: <span id=\"age\"></span> <br>\n"
"  </div>\n"
"  <div id=\"footer-warning\">For diagnostic purposes only. NOT SAFE FOR FLIGHT</div>\n"
"</div>\n";
  page+=pageFooter;
  server.send(200, "text/html", page); 
  }  
  
void handleFormat()
  {
   String page="";
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
          
          while (millis()-formatStartTime<5000)
            {
            if (Serial.available())
                {
                formatResponse=formatResponse+char(Serial.read());
                
                }
            }
                      
          if (formatResponse.indexOf("<formatdone") >= 0)
            {
            page+="<br><br><p>SD card has been formatted.</p>";
            page+="<br>New card size is: ";
            formatResponse=formatResponse.substring(11);
            formatResponse.replace(">","");
            page+=formatResponse; 
            }
            else
                {
                 page+="<br><br>"+formatResponse;
                 page+="<br><br><p>Error: Could not format SD card. Please reboot and try again.</p>";
                }
          }     
                
   page+=pageFooter;
   server.send(200, "text/html", page);   
  }


void handleReboot()
  {
   String page="";
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
          page+="<br><br><p>OnSpeed is rebooting. You can reconnect in a few seconds.</p>";
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
   page+=pageHeader;
   page+="<p>Available log files:</p>";  
  page+=filenameList;  
  page+=pageFooter;
  
  server.send(200, "text/html", page);
}

bool handleIndex()
{
  String page;
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
  server.send(200, "text/csv", ""); // send empty string (headers only)
  
  Serial.end();
  delay(100);
  Serial.begin(BAUDRATE_WIFI_HS); // increase wifi baudrate for file transfer
  delay(200);
  unsigned long starttime=millis();
  long bytecount=0;
  String sendBuffer;
  while (bytecount<contentLength)
  {
  if (Serial.available()) 
      {
       char inChar=Serial.read();
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
       if (bytecount % 1024==0)
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
      //Serial.println("file read timeout");      
      //Serial.println((millis()-starttime));
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
  // put your setup code here, to run once:
// Serial.begin(921600);
Serial.begin(115200);


 Serial.setDebugOutput(false);

 //WIFI INIT
 //Serial.println("Starting Access Point");
 WiFi.softAP(ssid, password);
  delay(100); // wait to init softAP
  
  IPAddress Ip(192, 168, 0, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  
  IPAddress myIP = WiFi.softAPIP();
 // Serial.print("AP IP address: ");
 // Serial.println(myIP);

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

void loop() {
  server.handleClient();
  webSocket.loop();
  
// parse live data from the Teensy via Serial
if (Serial.available())
  {
  // data format
  // $ONSPEED,AOA,PercentLift,IAS,PAlt,GLoad,CRC   
   char serialChar = Serial.read();   
  if (serialChar!='{' && serialChar!=char(0x0D) && serialChar!=char(0x0A) && serialBufferSize<200)
    {    
    serialBuffer[serialBufferSize]=serialChar;
    serialBufferSize++;
    } else
          {
           if (serialChar=='{')  // catch start of json data line
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
            // verify if line starting with $$ONSPEED
            if (dataString.startsWith("{\"type\":\"ONSPEED\""))
              {              
              // send to websocket if there are any clients waiting
              if (webSocket.connectedClients(false)>0)  webSocket.broadcastTXT(dataString);
              }            
            // drop buffer after sending
            serialBufferSize=0;
            }
           if (serialBufferSize>=200)
              {
              // too much junk received, drop buffer
              serialBufferSize=0;               
              }
          }


  
  }
 
}
