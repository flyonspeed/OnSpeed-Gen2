void LogReplay()
{

if (timersDisabled) 
    {
    checkWatchdog();
    return; // don't execute Logreplay if timers are disabled;
    }
char logLine[1024];
char inputChar;
int bufferIndex=0;
String valueArray[60];
int valueIndex=0;
float PfwdSmoothed;
float P45Smoothed;


// read next line in logfile and set airspeed and AOA from it
while (SensorFile.available())
      {
      inputChar=SensorFile.read();             
      if (inputChar != char(0xA))
          {
          logLine[bufferIndex]=inputChar;
          bufferIndex++;
          if (bufferIndex>=1023)
             {
             Serial.println("Buffer overflow while reading log file, skipping to next line");
             bufferIndex=0;
             return;
             }
          } else
                {
                // end of log line, parse it              
                    for (int i=0;i<bufferIndex;i++)
                    {
                    if (logLine[i]==',') valueIndex++; else valueArray[valueIndex]+=logLine[i];
                    //Serial.print(logLine[i]);
                    }
                    //Serial.println();
                   // skip CSV header line
                 if (lineCount==0)
                 {  
                   if (valueArray[0].indexOf("timeStamp")>=0)
                       {
                       Serial.println("Skipping header in logfile.");
                      // find efisLaterG column index (if boom is available it will be offset by 6 columns)
//                      for (int i=0;i<valueIndex;i++)
//                          {
//                          if (valueArray[i].indexOf("efisLateralG")>=0)
//                              {
//                              efisLateralGColumn=i;
//                              break;
//                              }
//                          }
//                       for (int i=0;i<valueIndex;i++)
//                          {
//                          if (valueArray[i].indexOf("VerticalG")>=0)
//                              {
//                              imuIndex=i;
//                              break;
//                              }
//                          }
                      lineCount++;                             
                      return; // skip if log header;
                       }
                 }                       
                  lineCount++;
                  // simulate a sensor read cycle
                  //Pfwd=valueArray[1].toInt(); // bias already removed                  
                  PfwdSmoothed=valueArray[2].toFloat();                  
                  //P45=valueArray[3].toInt(); // bias already removed
                  P45Smoothed=valueArray[4].toFloat();

                  coeffP=PCOEFF(PfwdSmoothed,P45Smoothed);
                  
                  flapsPos=valueArray[9].toInt();                  
                  for (int i=0; i < flapDegrees.Count;i++)
                    {
                    if (flapsPos==flapDegrees.Items[i])
                          {
                          flapsIndex=i;
                          break;
                          }                        
                    }

                  Palt=valueArray[6].toFloat();
                    
                  setAOApoints(flapsIndex);                                    
                  calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd divided by non-bias-corrected P45;                

                  // efis lateralG
                  efisLateralG=valueArray[efisLateralGColumn].toFloat();

                  // efisPitch
                  efisPitch=valueArray[36].toFloat();

                  // efisRoll
                  efisRoll=valueArray[37].toFloat();

                  //efisPalt
                  efisPalt=valueArray[6].toFloat();


                  //VN attitude and VSI
                  vnPitch=valueArray[36].toFloat();
                  vnRoll=valueArray[37].toFloat();
                  vnVelNedDown=valueArray[31].toFloat();
                  
                  
                  // get airspeed
                  IAS=valueArray[7].toFloat();
                  // update TAS
                  TASAvg.addValue((IAS+IAS * Palt / 1000 * 0.02) * 0.514444); // ballpark TAS 2% per thousand feet pressure altitude, in m/sec
                  smoothedTAS=TASAvg.getFastAverage();
                 
                  dataMark=valueArray[10].toInt();

                  kalmanAlt=valueArray[54].toFloat()*0.3048;
                  kalmanVSI=valueArray[55].toFloat()/197;

                  imuIndex=12;                 
                  // get IMU values from log                 
                  Az=valueArray[imuIndex].toFloat(); // vertical G
                  Ay=valueArray[imuIndex+1].toFloat(); // lateralG
                  Ax=valueArray[imuIndex+2].toFloat(); // forward G
                  Gx=valueArray[imuIndex+3].toFloat(); // roll
                  Gy=valueArray[imuIndex+4].toFloat(); // pitch
                  Gz=valueArray[imuIndex+5].toFloat(); // yaw

                  processAHRS();
                  
                  updateTones(); // generate tones   

                  //Serial.printf("%.2f,%.1f,%.2f,%.1f\n",smoothedPitch,efisPitch,smoothedRoll,efisRoll);
                  //Serial.printf("%.2f,%.1f,%.2f,%.1f\n",smoothedPitch,efisPitch,smoothedRoll,efisRoll);
                  //Serial.printf("%.1f,%.1f,%.1f\n",AOA,derivedAOA,flightPath);
                  //Serial.printf("%.1f,%.1f\n",smoothedIAS,smoothedTAS);
                  //Serial.printf("%.1f,%.1f\n",kalmanAlt*3.28084,Palt);
                  //Serial.printf("kalmanVSI: %.1f,kalmanAlt: %.1f,AOA: %.1f\n",kalmanVSI*3.28084,kalmanAlt*3.28084,AOA);
                  //Serial.printf("%.1f,%.1f\n",efisPitch,flightPath);
                  //Serial.printf("%.2f,%.2f\n",earthVertG,AVertCorr-1);
//                  Serial.print("Time: ");
//                  Serial.print(valueArray[0]);
//                  Serial.print(",Pfwd: ");
//                  Serial.print(PfwdSmoothed);
//                  Serial.print(",P45: ");                 
//                  Serial.print(P45Smoothed);
//                  Serial.print(",IAS: ");
//                  Serial.print(IAS);
//                  Serial.print(",AOA: ");
//                  Serial.print(AOA);
//                  Serial.print(", Pitch: ");
//                  Serial.print(smoothedPitch);
//                  Serial.print(", efisPitch: ");
//                  Serial.print(efisPitch);
//                  Serial.print(",Palt: ");
//                  Serial.print(Palt);
//                  Serial.print(",tonemode: ");
//                  // print tone type & pps
//                  if (toneMode==TONE_OFF) Serial.print("TONE_OFF,");
//                    else if (toneMode==PULSE_TONE) Serial.print("PULSE_TONE,");
//                       else if (toneMode==SOLID_TONE) Serial.print("SOLID_TONE,");
//                          else 
//                            {
//                            Serial.print("tonetype: "); 
//                            if (!highTone) Serial.print("LOW_TONE,"); else Serial.print("HIGH_TONE,");
//                            Serial.print("tonepulse: ");
//                            Serial.print(pps);                                                        
//                            Serial.print(",");
//                            }
//                  Serial.print("Flaps: ");      
//                  Serial.print(flapsPos);               
//                  Serial.println();                                                      
                  return;               
                }
                                          
      }
      // reached end of file, close file and turn off timer
          Serial.println();
          Serial.println("Finished replaying file.");
          SensorFile.close();
          delay(5000);
          
          LogReplayTimer.end();
          updateTones(); // to turn off tone at the end;
      
}

void PotRead()
{
float smoothingAlpha=0.04;  
float testAOA=getTestAOA();
AOA=testAOA*smoothingAlpha+AOA*(1-smoothingAlpha); // get and smooth potentiometer AOA (using flap pot input)
setAOApoints(0); // flaps up
IAS=50; // to turn on the tones
updateTones();
//Serial.printf("TestAOA: %0.2f, AOA: %0.2f\n",testAOA, AOA);
// breathing LED
  {
  if (millis()-lastLedUpdate>50)
      {
      if (switchState )
          {
          float ledBrightness = 15+(exp(sin(millis()/2000.0*PI)) - 0.36787944)*63.81; // funky sine wave, https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
          analogWrite(PIN_LED2, ledBrightness);         
          } else analogWrite(PIN_LED2,0);
       lastLedUpdate=millis();    
      }  
  }

}


void RangeSweep()  
{ 
if (rangeSweepDirection>0)  
    { 
    if (currentRangeSweepValue<RANGESWEEP_HIGH_AOA) currentRangeSweepValue+=RANGESWEEP_STEP; else if (currentRangeSweepValue>=RANGESWEEP_HIGH_AOA) rangeSweepDirection=-1;  
    } else  
          { 
           if (currentRangeSweepValue>RANGESWEEP_LOW_AOA) currentRangeSweepValue-=RANGESWEEP_STEP; else if (currentRangeSweepValue<=RANGESWEEP_LOW_AOA) rangeSweepDirection=1;  
          } 
AOA=currentRangeSweepValue; 
setAOApoints(0); // flaps down  
IAS=50; // to turn on the tones 
updateTones();  
}

float getTestAOA()
{
float adcvalue=0;
for (int i=0; i<5;i++)
    {
    adcvalue+=analogRead(FLAP_PIN);
    }
adcvalue=adcvalue/5;
return constrain(mapfloat(adcvalue, 47, 138, 0, 20),0,20);

}
