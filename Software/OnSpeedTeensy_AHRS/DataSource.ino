void setDataSourceMode()
{

 Serial.print("Data source: ");Serial.println(dataSource);
  if (dataSource=="SENSORS")
    {    
    SensorTimer.begin(SensorRead,SENSOR_INTERVAL); // every 20ms       
    } else
          if (dataSource=="TESTPOT")
              {
              PotTimer.begin(PotRead,SENSOR_INTERVAL); // 20ms            
              } else
                    if (dataSource=="RANGESWEEP")
                    {
                    RangeSweepTimer.begin(RangeSweep,200000); // 100ms
                    } else
                          
                          if (dataSource=="REPLAYLOGFILE")
                          {
                          // turn off SD loggingff
                          sdLogging=false;
                          IMUTimer.end();// turn off IMU timer if logreplay is running
                          // check if file exists
                           sprintf(filenameSensor,replayLogFileName.c_str());
                            
                            if (Sd.exists(filenameSensor))
                                  {
                                  Serial.print("Replaying data from log file: "); Serial.println(filenameSensor);
                                  } else
                                        {
                                        Serial.printf("ERROR: Could not find %s on the SD card\n",filenameSensor);                      
                                        }                                                   
                            
                            SensorFile = Sd.open(filenameSensor, O_READ);
                            if (SensorFile)
                                           {

                                            LogReplayTimer.priority(240);
                                            imuSampleRate=208; //238hz update rate for replaying logged data
                                            LogReplayTimer.begin(LogReplay,REPLAY_INTERVAL);
                                           } else
                                           {
                                            Serial.printf("ERROR: Could not open %s on the SD card.\n",filenameSensor);
                                           }
                                           
                            Serial.println();               
                                                                                                                  
                          }
}                          
