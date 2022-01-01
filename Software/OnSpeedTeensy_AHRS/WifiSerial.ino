void readWifiSerial()
{
// read Wifi serial
// look for wifi serial command

if (Serial4.available()>0)
  {
//$LIST!
//$DELETE filename!
//$PRINT filename!
//$STOPLIVEDATA!
//$STARTLIVEDATA!
//$FORMAT!
//$NOLOAD!
//$REBOOT!
//$SENDCONFIGSTRING!
//$SAVECONFIGSTRING<configstring>!
//$VOLUME
//$FLAPS
//$AOA
//$AUDIOTEST
serialWifiCmdChar = Serial4.read();
Serial.print(serialWifiCmdChar);
if (serialWifiCmdBufferSize >=2047)
      {       
      serialWifiCmdBufferSize=0; // don't let the command buffer overflow
      }
  if (serialWifiCmdChar!='!')
    {
    if (serialWifiCmdBufferSize==0 && serialWifiCmdChar!='$')
        {
        //drop character if command doesn't start with $                
        }
         else
              {
              serialWifiCmdBuffer[serialWifiCmdBufferSize]=serialWifiCmdChar;
              serialWifiCmdBufferSize++;
              }
    }          
    if (serialWifiCmdBufferSize>0 && serialWifiCmdChar=='!')
          { 
          // process command          
            if (strstr(serialWifiCmdBuffer, "$LIST"))
              {
              // list files
              timersOff();              
              Serial.println("file list requested on Wifi");
              bool orig_sdLogging=sdLogging;
              if (orig_sdLogging) sdLogging=false; // turn off sdLogging  
              sendWifiSerialString("<filelist>\n");
              ListFile=Sd.open("/", O_READ);
              if (!ListFile) Serial.println("SD card missing or unreadable");
              while(true) {
                          checkWatchdog();
                           FsFile entry =  ListFile.openNextFile();
                           if (!entry) {
                                         // no more files                                                                      
//                                         fileList+="<eof>";
                                         sendWifiSerialString("<eof>\n");
                                         break;
                                        }                           
                           if (!entry.isDirectory())
                              {
                              // only list files in root folder, no directories
                              char entryName[15];
                              entry.getName(entryName, sizeof(entryName));
                              sendWifiSerialString(String(entryName));
                              sendWifiSerialString(":");
                              char entrySize[20];
                              sprintf(entrySize, "%" PRIu64, entry.size());
                              sendWifiSerialString(String(entrySize));
                              sendWifiSerialString("\n");                              
                              }                             
                           entry.close();
                           }                     
               
              if (orig_sdLogging) sdLogging=true; // turn sd logging back on if it was originally on.
              timersOn();
              
              } else
                     if (strstr(serialWifiCmdBuffer, "$DELETE"))
                        {
                        // delete file                        
                        memcpy(listfileFileName,serialWifiCmdBuffer+8,serialWifiCmdBufferSize-7);
                        Sd.remove(listfileFileName);
                        Serial4.println();
                        Serial4.print("Deleted: ");
                        Serial4.println(listfileFileName);
                        } else
                              if (strstr(serialWifiCmdBuffer, "$PRINT"))
                                 {
                                 bool orig_sdLogging=sdLogging;
                                 if (orig_sdLogging)
                                      {
                                      sdLogging=false; // turn off sdLogging                                            
                                      Serial.println("STOPPED SD logging during file transfer.");
                                      }
                                 timersOff();                                                                                                   
                                 // print file contents
                                 Serial.println(serialWifiCmdBuffer);
                                 memcpy(listfileFileName,serialWifiCmdBuffer+7,serialWifiCmdBufferSize-6);                                 
                                 checkWatchdog();
                                 delay(1000);
                                 ListFile=Sd.open(listfileFileName, O_READ);
                                 if (ListFile) {     
                                              // read from the file until the end
                                              int outcount=0;
                                              while (ListFile.available())
                                              {      
                                                     checkWatchdog();                                                                                                                                                                                                                                                                                                               
                                                     // wait for serial packet to leave
                                                     byte fileByte=ListFile.read();
                                                     while(Serial4.availableForWrite()==0) {};                                                                                                                                                               
                                                     Serial4.write(fileByte);
                                                     //Serial4.flush();
                                                     outcount++;                                                    
                                                    if (outcount>=1436) // HTTP_DOWNLOAD_UNIT_SIZE (Webserver.h)
                                                          {                                                            
                                                          checkWatchdog();  
                                                          outcount=0;
                                                          filesendtimer=millis();
                                                          filesendtimeout=false;
                                                          // hold here until an ACK is received on serial.                                                          
                                                          while(true)
                                                                      {
                                                                      checkWatchdog();
                                                                      // check for timeout
                                                                      if (millis()-filesendtimer > 5000)
                                                                        {
                                                                         filesendtimeout=true;                                                                         
                                                                         break;  
                                                                        }
                                                                                                                                          
                                                                      if (Serial4.available() && Serial4.read()=='.')
                                                                        {                                                                                                                                            
                                                                        break; // break out of ACK hold                                                                                                                                               
                                                                        }
                                                                      
                                                                      } // while
                                                         if (filesendtimeout) 
                                                                    {
                                                                      Serial.println("file transfer timeout");
                                                                      break;                                                                                  
                                                                    }                                                               
                                                          }
                                              }
                                              // close the file:                                              
                                              ListFile.close();                                              
                                              
                                              
                                            } else {
                                              // if the file didn't open, print an error
                                              Serial.print("Could not open file: ");
                                              Serial.println(listfileFileName);
                                              Serial4.print("<404>");
                                              
                                            }                                
                                 if (orig_sdLogging)
                                    {
                                    sdLogging=true; // turn sd logging back on if it was originally on.                                                                        
                                    Serial.println("STARTED SD logging.");
                                    }
                                 timersOn();
                                 } else
                                         
                                         if (strstr(serialWifiCmdBuffer, "$STOPLIVEDATA"))
                                         {
                                         //LiveDisplayTimer.end(); // stop display data 
                                         sendWifiData=false;                                        
                                         Serial.println("STOPPED Live Data. (Wifi Request)");
                                         } else
                                              if (strstr(serialWifiCmdBuffer, "$STARTLIVEDATA"))
                                                 {                                                 
                                                 sendWifiData=true;
                                                 Serial.println("STARTED Live Data. (Wifi Request)");
                                                 } else

                                                   if (strstr(serialWifiCmdBuffer, "$FORMAT"))
                                                          {
                                                          // delete all files                        
                                                          Serial.println("Wifi requested SD format");
                                                           bool orig_sdLogging=sdLogging;
                                                           if (orig_sdLogging) sdLogging=false; // turn off sdLogging 


                                                            ExFatFormatter exFatFormatter;
                                                            FatFormatter fatFormatter;                                                            
                                                            m_card = cardFactory.newCard(SdioConfig(FIFO_SDIO));
                                                            if ((!m_card || m_card->errorCode())) {
                                                                 Serial.print("FORMAT ERROR: Cannot initialize SD card. ");
                                                                 Serial.println(m_card->errorCode());
                                                                 Serial4.println("<formaterror></formaterror>");                                                                 
                                                              } else                                                               
                                                                 {
                                                                  cardSectorCount = m_card->sectorCount();
                                                                  //Serial.printf("Sectorcount: %i\n",cardSectorCount);
                                                                    // Format exFAT if larger than 32GB.
                                                                    bool rtn = cardSectorCount > 67108864 ?
                                                                      exFatFormatter.format(m_card, sectorBuffer, &Serial) :
                                                                      fatFormatter.format(m_card, sectorBuffer, &Serial);                                                                  
                                                                    if (!rtn) {
                                                                      Serial.println("FORMAT ERROR: Could not format SD Card.");
                                                                      Serial4.println("<FORMATERROR>Could not format SD Card</FORMATERROR>");
                                                                    } else
                                                                          {
                                                                          Serial.print("SD card format completed. Card size: ");
                                                                          Serial.print(cardSectorCount*5.12e-7);
                                                                          Serial.println("GBytes");
                                                                          Serial4.printf("<FORMATDONE>%.1f GB</FORMATDONE>\n",cardSectorCount*5.12e-7);
                                                                          delay(300);
                                                                          //reinitialize SD card
                                                                          sdAvailable=Sd.begin(SdioConfig(FIFO_SDIO));
                                                                          String configString="";
                                                                          configurationToString(configString);
                                                                          saveConfigurationToFile(configFilename,configString);                                                                                                                                                 
                                                                          }                                                                    
                                                                 }  
                                                                                                                    
                                                          if (orig_sdLogging)
                                                                {
                                                                //reinitialize card
                                                                Sd.begin(SdioConfig(FIFO_SDIO));
                                                                sdLogging=true; // turn sd logging back on if it was originally on.
                                                                }
                                                          } else
                                                              if (strstr(serialWifiCmdBuffer, "$PFWDBIAS"))
                                                                  {                                                                  
                                                                  SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                  Serial.println("Wifi: Getting Pfwd bias...");
                                                                  // get Pfwd bias
                                                                  long PfwdTotal=0;                                                                
                                                                  for (int i=1;i<=500;i++)
                                                                      {
                                                                      PfwdTotal+=GetPressurePfwd();
                                                                      checkWatchdog();
                                                                      delay(2);
                                                                      }
                                                                 Serial4.printf("<PFWDBIAS>%i</PFWDBIAS>",round(PfwdTotal/500));
                                                                 SensorTimer.begin(SensorRead,SENSOR_INTERVAL);                                                                                                                               
                                                                  } else
                                                                      if (strstr(serialWifiCmdBuffer, "$P45BIAS"))
                                                                      {                                                                  
                                                                      SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                      Serial.println("Wifi: Getting P45 bias...");
                                                                      // get Pfwd bias                                                                      
                                                                      long P45Total=0;
                                                                      for (int i=1;i<=500;i++)
                                                                          {                                                                          
                                                                          P45Total+=GetPressureP45();
                                                                          delay(2);
                                                                          checkWatchdog();
                                                                          }
                                                                      Serial4.printf("<P45BIAS>%i</P45BIAS>",round(P45Total/500));
                                                                      SensorTimer.begin(SensorRead,SENSOR_INTERVAL);                                                                                                                               
                                                                      } else
                                                                            if (strstr(serialWifiCmdBuffer, "$FLAPS"))
                                                                                {                                                                                                                                              
                                                                                Serial.println("Wifi: Getting Flap position");
                                                                                int flapPotTotal=0;
                                                                                for (int i=0;i<100;i++)
                                                                                    {
                                                                                    flapPotTotal+=analogRead(FLAP_PIN);
                                                                                    checkWatchdog();
                                                                                    delay(10);
                                                                                    }                                                                                                      
                                                                                Serial4.printf("<FLAPS>%i</FLAPS>",round(flapPotTotal/100));                                                                                                                                
                                                                                } else
                                                                                  if (strstr(serialWifiCmdBuffer, "$VOLUME"))
                                                                                        {                                                                                                                                              
                                                                                        Serial.println("Wifi: Getting Volume position");
                                                                                        int volumePotTotal=0;
                                                                                        for (int i=0;i<100;i++)
                                                                                            {
                                                                                            volumePotTotal+=analogRead(VOLUME_PIN);
                                                                                            checkWatchdog();
                                                                                            delay(10);
                                                                                            }                                                                                                              
                                                                                        Serial4.printf("<VOLUME>%i</VOLUME>",round(volumePotTotal/100));                                                                                                                                
                                                                                        } else                                                                                                          
                                                                                              if (strstr(serialWifiCmdBuffer, "$AUDIOTEST"))                                                                                                                                                                          
                                                                                                {
                                                                                                Serial.println("Wifi: Testing audio");
                                                                                                timersOff();
                                                                                                Serial.println("Wifi: Playing Left audio test");                                                                                                                  
                                                                                                 ampLeft.gain(1);
                                                                                                 ampRight.gain(0);
                                                                                                 voice1.play(AudioSampleOnspeed_left_speaker);
                                                                                                checkWatchdog();
                                                                                                delay (2500);
                                                                                                checkWatchdog();
                                                                                                 ampLeft.gain(0);
                                                                                                 ampRight.gain(1);
                                                                                                 Serial.println("Wifi: Playing Right audio test");
                                                                                                 voice1.play(AudioSampleOnspeed_right_speaker);
                                                                                                checkWatchdog();; 
                                                                                                delay (2500);
                                                                                                checkWatchdog();
                                                                                                 ampLeft.gain(1);
                                                                                                 ampRight.gain(1);
                                                                                                 timersOn();
                                                                                                 Serial.println("Wifi: AUDIOTEST Complete");
                                                                                                 Serial4.println("<AUDIOTEST>Done.</AUDIOTEST>");
                                                                                                } else
                                                                                                      if (strstr(serialWifiCmdBuffer, "$WIFIREFLASH"))                                                                            
                                                                                                        {
                                                                                                        Serial4.end();
                                                                                                        pinMode(PIN_A12,INPUT);
                                                                                                        pinMode(PIN_A13,INPUT);
                                                                                                        Serial.println("wifi reflash mode");
                                                                                                        } else
                                                                                                               if (strstr(serialWifiCmdBuffer, "$REBOOT"))                                                                            
                                                                                                                    {
                                                                                                                    Serial4.println("<REBOOT>OK</REBOOT>");
                                                                                                                    Serial.println("Wifi reboot request. Rebooting...");
                                                                                                                    delay(200);                                                                                       
                                                                                                                    _softRestart();
                                                                                                                    } else
                                                                                                                          if (strstr(serialWifiCmdBuffer, "$SENDCONFIGSTRING"))                                                                    
                                                                                                                              {
                                                                                                                              timersOff();
                                                                                                                              checkWatchdog();                                                                                                       
                                                                                                                              String configString="";
                                                                                                                              configurationToString(configString);
                                                                                                                              // add CRC to configString
                                                                                                                              addCRC(configString);                                                                                                                              
                                                                                                                              sendWifiSerialString(configString);
                                                                                                                              Serial.println("Configstring sent to Wifi");                                                                                                                                                                                                                                                            
                                                                                                                              timersOn();                                                                                                                                                                                                                                                      
                                                                                                                              checkWatchdog();
                                                                                                                              } else
                                                                                                                                    if (strstr(serialWifiCmdBuffer, "$SAVECONFIGSTRING"))
                                                                                                                                          {
                                                                                                                                          timersOff();
                                                                                                                                          sdLogging=false;  
                                                                                                                                          // saveConfigString                                                                                                                                          
                                                                                                                                          String configString=String(serialWifiCmdBuffer);
                                                                                                                                          String checksumString=getConfigValue(configString,"CHECKSUM");                                                                                                                                         
                                                                                                                                          String configContent=getConfigValue(configString,"CONFIG");
                                                                                                                                           // calculate checksum
                                                                                                                                          int16_t calcCRC=0;
                                                                                                                                          for (unsigned int i=0;i<configContent.length();i++) calcCRC+=configContent[i];
                                                                                                                                          if (String(calcCRC,HEX)==checksumString)
                                                                                                                                              {
                                                                                                                                              // checksum ok.
                                                                                                                                              configString="<CONFIG>"+configContent+"</CONFIG>"; // strip checksum tag when saving config
                                                                                                                                              if (loadConfigFromString(configString))
                                                                                                                                                  {                                                                                                                                                
                                                                                                                                                  saveConfigurationToFile(configFilename,configString);
                                                                                                                                                  setVolume(volumePercent);
                                                                                                                                                  configureAxes();
                                                                                                                                                  Serial4.print("<CONFIGSAVED></CONFIGSAVED>"); 
                                                                                                                                                  Serial.println("Wifi: Loaded new configuration");                                                                                                                
                                                                                                                                                  } else Serial4.print("<CONFIGERROR>Could not load configuration</CONFIGERROR>");                                                                                                                                              
                                                                                                                                          
                                                                                                                                                } else
                                                                                                                                                      {
                                                                                                                                                      //checksum failed
                                                                                                                                                      Serial4.print("<CONFIGERROR>Transmission Error. Checksum failed. Try again.</CONFIGERROR>");
                                                                                                                                                      Serial.println("CONFIG checksum failed");
                                                                                                                                                      }
                                                                                                                                          
                                                                                                                                          
                                                                                                                                          timersOn();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
                                                                                                                                          } else
                                                                                                                                                if (strstr(serialWifiCmdBuffer, "$AOA"))
                                                                                                                                                    {
                                                                                                                                                    Serial.println("Wifi: AOA request");  
                                                                                                                                                    float aoaTotal=0.0;  
                                                                                                                                                    // sample aoa for 1 sec
                                                                                                                                                    for (int i=0;i<50;i++)
                                                                                                                                                        {
                                                                                                                                                        aoaTotal+=AOA;
                                                                                                                                                        unsigned long starttime=millis();
                                                                                                                                                        // wait 20ms;
                                                                                                                                                        while (millis()-starttime<=20)
                                                                                                                                                              {
                                                                                                                                                              checkWatchdog();
                                                                                                                                                              }                                                                                                                                                        
                                                                                                                                                         }
                                                                                                                                                        float avgAOA= aoaTotal/50;
                                                                                                                                                    Serial4.printf("<AOA>%.2f</AOA>",avgAOA);
                                                                                                                                                    } else
                                                                                                                                                        if (strstr(serialWifiCmdBuffer, "$SENDDEFAULTCONFIGSTRING"))                                                                            
                                                                                                                                                            {
                                                                                                                                                            timersOff();  
                                                                                                                                                            Serial.println("Wifi: Default Configstring request from wifi");                                                                                                                                                            
                                                                                                                                                            String configString=String(DEFAULT_CONFIG);
                                                                                                                                                            // add CRC to configString
                                                                                                                                                            addCRC(configString);
                                                                                                                                                            checkWatchdog();                                                                                                                             
                                                                                                                                                            sendWifiSerialString(configString);
                                                                                                                                                            timersOn();    
                                                                                                                                                            } else
                                                                                                                                                                if (strstr(serialWifiCmdBuffer, "$SENSORCONFIG"))
                                                                                                                                                                    {
                                                                                                                                                                    // sensor bias configuration
                                                                                                                                                                     float aircraftPitch=stringToFloat(getConfigValue(serialWifiCmdBuffer,"AIRCRAFTPITCH"));
                                                                                                                                                                     float aircraftRoll=stringToFloat(getConfigValue(serialWifiCmdBuffer,"AIRCRAFTROLL"));
                                                                                                                                                                     float aircraftPAlt=stringToFloat(getConfigValue(serialWifiCmdBuffer,"AIRCRAFTPALT"));
                                                                                                                                                                     timersOff();
                                                                                                                                                                     sdLogging=false;
                                                                                                                                                                     Serial.println("Wifi: SensorConfig request");
                                                                                                                                                                     // sample sensors
                                                                                                                                                                     long P45Total=0;
                                                                                                                                                                     long PFwdTotal=0;
                                                                                                                                                                     long PStaticTotal=0;
                                                                                                                                                                     float aVertTotal=0.00;
                                                                                                                                                                     float aLatTotal=0.00;
                                                                                                                                                                     float aFwdTotal=0.00;
                                                                                                                                                                     float gxTotal=0.00;
                                                                                                                                                                     float gyTotal=0.00;
                                                                                                                                                                     float gzTotal=0.00;
                                                                                                                                                                     int sensorReadCount=150;
                                                                                                                                                                     timersOff();                            
                                                                                                                                                                     for (int i=0;i<sensorReadCount;i++)
                                                                                                                                                                          {
                                                                                                                                                                          checkWatchdog();
                                                                                                                                                                          P45Total+=GetPressureP45();
                                                                                                                                                                          PFwdTotal+=GetPressurePfwd();
                                                                                                                                                                          PStaticTotal+=GetStaticPressure();
                                                                                                                                                                          
                                                                                                                                                                          readAccelGyro();
                                                                                                                                                                          aVertTotal+=getAccelForAxis(verticalGloadAxis);                                                                                                                                                                          
                                                                                                                                                                          aLatTotal+=getAccelForAxis(lateralGloadAxis);
                                                                                                                                                                          aFwdTotal+=getAccelForAxis(forwardGloadAxis);
                                                                                                                                                                          gxTotal+=gx;
                                                                                                                                                                          gyTotal+=gy;
                                                                                                                                                                          gzTotal+=gz;
                                                                                                                                                                          delayMicroseconds(4201);                                                                                                                                                                         
                                                                                                                                                                          }
                                                                                                                                                                       timersOn();    
                                                                    
                                                                                                                                                                     // calculate pitch from averaged accelerometer reading
                                                                                                                                                                     float calcPitch=atan2((aFwdTotal/sensorReadCount), sqrt((aLatTotal/sensorReadCount)* (aLatTotal/sensorReadCount) + (aVertTotal/sensorReadCount) *(aVertTotal/sensorReadCount))) * 57.2957;
                                                                                                                                                                     float calcRoll =-atan2((aLatTotal/sensorReadCount), sqrt((aFwdTotal/sensorReadCount)* (aFwdTotal/sensorReadCount) + (aVertTotal/sensorReadCount) *(aVertTotal/sensorReadCount))) * 57.2957;                                                                                                                                                                     
                                                                                                                                                                     // adjust pitch bias (aircraft pitch -calcPitch)
                                                                                                                                                                     pitchBias=aircraftPitch-calcPitch;
                                                                                                                                                                     rollBias=aircraftRoll-calcRoll;
                                                                                                                                                                     Serial.printf("aircraftPitch: %.2f,aircraftRoll: %.2f,calcPitch: %.2f,calcRoll: %.2f",aircraftPitch,aircraftRoll,calcPitch,calcRoll);
                                                                                                                                                                     pFwdBias= int((PFwdTotal/sensorReadCount));
                                                                                                                                                                     p45Bias= int((P45Total/sensorReadCount));
                                                                                                                                                                     float PStatic= float((PStaticTotal/sensorReadCount));
                                                                                                                                                                     float aircraftPressure=29.92125535*pow(((288-0.0065*0.3048*aircraftPAlt)/288),5.2561)*33.8639; // https://www.weather.gov/media/epz/wxcalc/stationPressure.pdf from inHg to milliBars.
                                                                                                                                                                     pStaticBias=aircraftPressure - PStatic;

                                                                                                                                                                     
                                                                                                                                                                     gxBias=-gxTotal/sensorReadCount;
                                                                                                                                                                     gyBias=-gyTotal/sensorReadCount;
                                                                                                                                                                     gzBias=-gzTotal/sensorReadCount;
                                                                                                                                                                     
 
                                                                                                                                                                     String resultString="PfwdBias: "+ String(pFwdBias)+"<br>";
                                                                                                                                                                     resultString+=" P45Bias: "+ String(p45Bias)+"<br>";
                                                                                                                                                                     resultString+=" gxBias: "+ floatToString(gxBias)+"<br>";
                                                                                                                                                                     resultString+=" gyBias: "+ floatToString(gyBias)+"<br>";
                                                                                                                                                                     resultString+=" gzBias: "+ floatToString(gzBias)+"<br>";
                                                                                                                                                                     resultString+=" measured Pitch: "+ String(calcPitch)+"<br>"; 
                                                                                                                                                                     resultString+=" pitchBias: "+ String(pitchBias)+"<br>";
                                                                                                                                                                     resultString+=" measured Roll: "+ String(calcRoll)+"<br>"; 
                                                                                                                                                                     resultString+=" rollBias: "+ String(rollBias)+"<br>"; 
                                                                                                                                                                     resultString+=" staticBias: "+ String(pStaticBias)+"<br>";                                                                                                                                                                     
                                                                                                                                                                     String configString="";
                                                                                                                                                                     configurationToString(configString);
                                                                                                                                                                     saveConfigurationToFile(configFilename,configString);
                                                                                                                                                                     sendWifiSerialString("<SENSORCONFIG>"+resultString+"</SENSORCONFIG>");                                                                                                                                                                     
                                                                                                                                                                     Serial.println("Wifi: SensorConfig complete.");
                                                                                                                                                                     timersOn();
                                                                                                                                                                     sdLogging=true;                                                                                                                                                                
                                                                                                                                                                    } else
                                                                                                                                                                            if (strstr(serialWifiCmdBuffer, "$PITCHROLLALT"))
                                                                                                                                                                                        {
                                                                                                                                                                                        int sensorReadCount=150;
                                                                                                                                                                                        float aVertTotal=0.00;
                                                                                                                                                                                        float aLatTotal=0.00;
                                                                                                                                                                                        float aFwdTotal=0.00;
                                                                                                                                                                                        long PStaticTotal=0;
                                                                                                                                                                                        Serial.println("Wifi: PitchRollAlt request");                                                                                                                                                                                          
                                                                                                                                                                                        timersOff();                                                                                                                                                                                       
                                                                                                                                                                                        for (int i=0;i<sensorReadCount;i++)
                                                                                                                                                                                            {                                                                                                                                                                                            
                                                                                                                                                                                            checkWatchdog();
                                                                                                                                                                                            readAccelGyro();
                                                                                                                                                                                            aVertTotal+=getAccelForAxis(verticalGloadAxis);                                                                                                                                                                          
                                                                                                                                                                                            aLatTotal+=getAccelForAxis(lateralGloadAxis);
                                                                                                                                                                                            aFwdTotal+=getAccelForAxis(forwardGloadAxis);
                                                                                                                                                                                            PStaticTotal+=GetStaticPressure();
                                                                                                                                                                                            delayMicroseconds(4201); // 238Hz
                                                                                                                                                                                            }
                                                                                                                                                                                        timersOn();     
                                                                                                                                                                                        // calculate pitch from averaged accelerometer reading                                                                                                                                                                                       
                                                                                                                                                                                        float calcPitchAngle=calcPitch(aFwdTotal/sensorReadCount, aLatTotal/sensorReadCount, aVertTotal/sensorReadCount);
                                                                                                                                                                                        float calcRollAngle=calcRoll(aFwdTotal/sensorReadCount, aLatTotal/sensorReadCount, aVertTotal/sensorReadCount);
                                                                                                                                                                                        float PStatic= float((PStaticTotal/sensorReadCount));
                                                                                                                                                                                        Palt=145366.45*(1-pow((PStatic+pStaticBias)/1013.25,0.190284)); //Pstatic in milliBars,Palt in feet
                                                                                                                                                                                        Serial4.printf("<RESPONSE><PITCH>%.2f</PITCH><ROLL>%.2f</ROLL><PALT>%.2f</PALT></RESPONSE>",calcPitchAngle,calcRollAngle,Palt);                                                                                                                                                                                     
                                                                                                                                                                                        }  else
                                                                                                                                                                                              if (strstr(serialWifiCmdBuffer, "$VERSION"))
                                                                                                                                                                                                        {
                                                                                                                                                                                                         Serial.println("Wifi: Version request");
                                                                                                                                                                                                         sendWifiSerialString("<VERSION>"+String(VERSION)+"</VERSION>");                                                                                                                                                                                                    
                                                                                                                                                                                                        }
                                                                                                                                                                                                         else                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                             if (strstr(serialWifiCmdBuffer, "$VNOCHIMETEST"))                                                                                                                                                                          
                                                                                                                                                                                                                    {
                                                                                                                                                                                                                    Serial.println("Wifi: VNO chime audiotest");
                                                                                                                                                                                                                    timersOff();                                                                                                                                                                                                         
                                                                                                                                                                                                                    voice1.play(AudioSampleVnochime);
                                                                                                                                                                                                                    checkWatchdog();
                                                                                                                                                                                                                    delay (2500);
                                                                                                                                                                                                                    checkWatchdog();                                                                                                                                                                                                       
                                                                                                                                                                                                                    timersOn();
                                                                                                                                                                                                                    Serial.println("Wifi: VNOCHIMETEST Complete");
                                                                                                                                                                                                                    Serial4.println("<VNOCHIMETEST>Done.</VNOCHIMETEST>");
                                                                                                                                                                                                                    }

          // reset cmdBuffer
          memset(serialWifiCmdBuffer,0,sizeof(serialWifiCmdBuffer));
          serialWifiCmdBufferSize=0;
          }  
  } // if serial.available wifi

}

void sendWifiSerialString(String serialString)
{
  for (unsigned int i=0; i<serialString.length();i++)
  {
  Serial4.flush();
  while(Serial4.availableForWrite()==0) { };
  Serial4.print(serialString[i]);  
  checkWatchdog();
  }
}
