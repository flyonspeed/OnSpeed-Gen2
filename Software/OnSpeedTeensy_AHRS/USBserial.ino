void readUSBSerial()
{
// look for serial command
if (Serial.available()>0)
  {  
//LIST!
//DELETE filename!S
//PRINT filename!
//STOP!
//START!
//FORMAT!
//NOLOAD!
// WIFIREFLASH!
// REBOOT!
// FLAPS!
// VOLUME!
// CONFIG!
// AUDIOTEST!
serialCmdChar = Serial.read();    
  if (serialCmdChar!=char(0x21) && serialCmdBufferSize<50)
    {    
    serialCmdBuffer[serialCmdBufferSize]=serialCmdChar;
    serialCmdBufferSize++;
    } else
          {
          // process command          
            if (strstr(serialCmdBuffer, "LIST"))
              {
              // list files
              Serial.println("Files list:");
              ListFile=Sd.open("/", O_READ);
              if (!ListFile) Serial.println("SD card missing or unreadable");
              while(true) {
     
                           FsFile entry =  ListFile.openNextFile();
                           if (! entry) {
                                         // no more files                             
                                         break;
                                        }                           
                           
                           if (!entry.isDirectory())
                              {
                              // only list files in root folder, no directories
                              entry.printName(&Serial);
                              Serial.print("    ");
                              Serial.println(entry.size(), DEC);                              
                              }                             
                           entry.close();
                           checkWatchdog();
                           }
                           
              
              } else
                     if (strstr(serialCmdBuffer, "DELETE"))
                        {
                        // delete file                        
                        memcpy(listfileFileName,serialCmdBuffer+7,serialCmdBufferSize-6);
                        Sd.remove(listfileFileName);
                        Serial.println();
                        Serial.print("Deleted: ");
                        Serial.println(listfileFileName);
                        } else
                              if (strstr(serialCmdBuffer, "PRINT"))
                                 {
                                 // print file contents
                                 memcpy(listfileFileName,serialCmdBuffer+6,serialCmdBufferSize-5);                                 
                                 ListFile=Sd.open(listfileFileName, O_READ);
                                 if (ListFile) {
                                              Serial.println();
                                              Serial.print(listfileFileName);                                              
                                              Serial.println(":");
                                              // read from the file until there's nothing else in it:
                                              while (ListFile.available()) {
                                               checkWatchdog();
                                                      Serial.flush();  
                                                      Serial.write(ListFile.read());                                             
                                              }
                                              // close the file:
                                              ListFile.close();
                                              Serial.println("\nDONE.");
                                            } else {
                                              // if the file didn't open, print an error:
                                              Serial.print("Error opening "); Serial.println(listfileFileName);
                                            }
                                 
                                 } else
                                      if (strstr(serialCmdBuffer, "STOP"))
                                         {
                                         // stop SD logging to take file commands
                                         sdLogging=false;
                                         if (dataSource=="SENSORS")
                                              {
                                              SensorTimer.end();                                                                                     
                                              }
                                         Serial.println("STOPPED SD logging.");
                                         } else
                                              if (strstr(serialCmdBuffer, "START"))
                                                 {
                                                 // start SD logging
                                                 //ToneTimer.begin;
                                                 sdLogging=true;
                                                 if (dataSource=="SENSORS") 
                                                      {                                                      
                                                      SensorTimer.begin(SensorRead,SENSOR_INTERVAL);
                                                      Serial.println("STARTED SD logging.");
                                                      }                                                      
                                                 
                                                 } else
                                                      if (strstr(serialCmdBuffer, "FORMAT"))
                                                          {
                                                          bool orig_sdLogging=sdLogging;
                                                           if (orig_sdLogging) sdLogging=false; // turn off sdLogging                                                           
                                                           timersOff();
                                                           Sd.end();
                                                                                                                    
                                                            ExFatFormatter exFatFormatter;
                                                            FatFormatter fatFormatter;                                                            
                                                            m_card = cardFactory.newCard(SdioConfig(FIFO_SDIO));
                                                            if ((!m_card || m_card->errorCode())) {
                                                                 Serial.print("FORMAT ERROR: Cannot initialize SD card. ");
                                                                 Serial.println(m_card->errorCode());                                                                 
                                                              } else                                                               
                                                                 {
                                                                  cardSectorCount = m_card->sectorCount();
                                                                    // erase card first
                                                                    Serial.println("Erasing card");
                                                                    uint32_t const ERASE_SIZE = 262144L;
                                                                    uint32_t firstBlock = 0;
                                                                    uint32_t lastBlock;
                                                                    do {
                                                                        lastBlock = firstBlock + ERASE_SIZE - 1;
                                                                        if (lastBlock >= cardSectorCount)
                                                                            {
                                                                            lastBlock = cardSectorCount - 1;
                                                                            }
                                                                        if (!m_card->erase(firstBlock, lastBlock))
                                                                            {
                                                                            Serial.println("Card erase failed");
                                                                            }
                                                                      firstBlock += ERASE_SIZE;
                                                                    checkWatchdog();  
                                                                    } while (firstBlock < cardSectorCount);

                                                                  Serial.println("Fromatting card");
                                                                  //Serial.printf("Sectorcount: %i\n",cardSectorCount);
                                                                    // Format exFAT if larger than 32GB.
                                                                    bool rtn = cardSectorCount > 67108864 ?
                                                                      exFatFormatter.format(m_card, sectorBuffer, &Serial) :
                                                                      fatFormatter.format(m_card, sectorBuffer, &Serial);                                                                  
                                                                    if (!rtn) {
                                                                      Serial.println("FORMAT ERROR: Could not format SD card.");
                                                                    } else
                                                                          {
                                                                          Serial.print("SD card format completed. Card size: ");
                                                                          Serial.print(cardSectorCount*5.12e-7);
                                                                          Serial.println("GBytes");
                                                                          delay(300);
                                                                          //reinitialize SD card

                                                                         // initialize card, 
                                                                         byte sdBeginTries=0;
                                                                         sdAvailable=Sd.begin(SdioConfig(FIFO_SDIO));
                                                                         while (!sdAvailable && sdBeginTries<5)
                                                                               {
                                                                               //try up to 5 times
                                                                               Serial.println("Reintializing SD card.");
                                                                               sdAvailable=Sd.begin(SdioConfig(FIFO_SDIO));
                                                                               delay(200);
                                                                               sdBeginTries++;
                                                                               }
                                                                          //sdAvailable=Sd.begin(SdioConfig(FIFO_SDIO));
                                                                          if (!sdAvailable) Serial.println("SD card couldn't be initialized");
                                                                          String configString="";
                                                                          configurationToString(configString);
                                                                          saveConfigurationToFile(configFilename,configString);
                                                                          // card is empty create new log file
                                                                          createLogFile();
                                                                          timersOn();                                                                          
                                                                          }
                                                                                                                                              
                                                                 }                                                           
                                                                         
                                                          if (orig_sdLogging)
                                                                            {
                                                                            //reinitialize card
                                                                            sdLogging=true; // if logging was on before FORMAT turn it back on               
                                                                            }
                                                          
                                                          } else
                                                              if (strstr(serialCmdBuffer, "NOLOAD"))
                                                                  {                                                                  
                                                                  SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                  Serial.println("Getting Pressure sensor bias...");
                                                                  // get Pfwd bias
                                                                  long PfwdTotal=0;
                                                                  long P45Total=0;
                                                                  for (int i=1;i<=1000;i++)
                                                                      {
                                                                      PfwdTotal+=GetPressurePfwd();
                                                                      P45Total+=GetPressureP45();
                                                                      if (i % 250==0) checkWatchdog();
                                                                      delay(10);
                                                                      }
                                                                 Serial.print("Pfwd bias=");
                                                                 Serial.println(PfwdTotal/1000);
                                                                 Serial.print("P45 bias=");
                                                                 Serial.println(P45Total/1000);                                                                                                                                
                                                                  }  else
                                                                     if (strstr(serialCmdBuffer, "WIFIREFLASH"))                                                                            
                                                                            {
                                                                            Serial4.end();
                                                                            pinMode(PIN_A12,INPUT);
                                                                            pinMode(PIN_A13,INPUT);
                                                                            Serial.println("wifi reflash mode activated");
                                                                            } else
                                                                                   if (strstr(serialCmdBuffer, "REBOOT"))                                                                            
                                                                                        {
                                                                                        Serial.println("serial reboot request. Rebooting...");
                                                                                         checkWatchdog();
                                                                                         delay(2000);
                                                                                        _softRestart();
                                                                                        }
                                                                                         else 
                                                                                            if (strstr(serialCmdBuffer, "FLAPS"))                                                                            
                                                                                                {
                                                                                                Serial.print("Current flap potentiometer analog value: ");
                                                                                                int flapPotTotal=0;
                                                                                                for (int i=0;i<100;i++)
                                                                                                  {
                                                                                                  flapPotTotal+=analogRead(FLAP_PIN);
                                                                                                  }
                                                                                                  
                                                                                                Serial.println(flapPotTotal/100);
                                                                                                } else 
                                                                                                      if (strstr(serialCmdBuffer, "VOLUME"))                                                                            
                                                                                                          {
                                                                                                          Serial.print("Current volume potentiometer analog value: ");
                                                                                                          int volumePotTotal=0;
                                                                                                          for (int i=0;i<100;i++)
                                                                                                            {
                                                                                                            volumePotTotal+=analogRead(VOLUME_PIN);
                                                                                                            }                                                                                                            
                                                                                                          Serial.println(volumePotTotal/100);
                                                                                                          }  else
                                                                                                                if (strstr(serialCmdBuffer, "CONFIG"))                                                                            
                                                                                                                    {
                                                                                                                    Serial.println("Current configuration: ");
                                                                                                                    String configString="";                                                                                                       
                                                                                                                    configurationToString(configString);
                                                                                                                    Serial.println(configString);
                                                                                                                    }
                                                                                                                      else                                                                                                          
                                                                                                                          if (strstr(serialCmdBuffer, "AUDIOTEST"))                                                                            
                                                                                                                            {
                                                                                                                            ToneTimer.end(); // turn off ToneTimer
                                                                                                                            SensorTimer.end();
                                                                                                                            Serial.println("Playing Left audio");                                                                                                                  
                                                                                                                             ampLeft.gain(1);
                                                                                                                             ampRight.gain(0);
                                                                                                                             voice1.play(AudioSampleOnspeed_left_speaker);
                                                                                                                              checkWatchdog();
                                                                                                                            delay (2500);
                                                                                                                             checkWatchdog();
                                                                                                                             ampLeft.gain(0);
                                                                                                                             ampRight.gain(1);
                                                                                                                             Serial.println("Playing Right audio");
                                                                                                                             voice1.play(AudioSampleOnspeed_right_speaker);
                                                                                                                              checkWatchdog();
                                                                                                                            delay (2500);
                                                                                                                             checkWatchdog();
                                                                                                                             ampLeft.gain(1);
                                                                                                                             ampRight.gain(1);
                                                                                                                             ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on 
                                                                                                                             SensorTimer.begin(SensorRead,SENSOR_INTERVAL);
                                                                                                                             Serial.println("AUDIOTEST Complete");
                                                                                                                            } else
                                                                                                                                  if (strstr(serialCmdBuffer, "HELP"))                                                                            
                                                                                                                                      {
                                                                                                                                      displayConsoleHelp();  
                                                                                                                                      }
                                                                                                                            

          
          // reset cmdBuffer
          memset(serialCmdBuffer,0,sizeof(serialCmdBuffer));
          serialCmdBufferSize=0;            
          }
  } // if serial.available

}

void displayConsoleHelp()
{
        Serial.println();
        Serial.println("Accepted commands on this console:\n");
        Serial.println("HELP! - display this command list");
        Serial.println("STOP! - stop logging to SD card");
        Serial.println("LIST! - list files on SD card");
        Serial.println("DELETE filename! - delete file on SD card");
        Serial.println("PRINT filename! - display file contents");        
        Serial.println("FORMAT! - format SD card");
        Serial.println("NOLOAD! - show pressure sensor bias");
        Serial.println("START! - start logging to SD card");
        Serial.println("REBOOT! - reboot system");
        Serial.println("WIFIREFLASH!- allow reflashing Wifi chip via USB cable");
        Serial.println("FLAPS! - show current flap position value");
        Serial.println("VOLUME! -show current volume potentiometer value");                
        Serial.println("CONFIG! -show current configuration values");
        Serial.println("AUDIOTEST! -left & right audio test"); 
        Serial.println();
}        
