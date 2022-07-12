void writeSerialData()
{
if (serialOutPort!="NONE" && millis()-serialoutLastUpdate>100) // update every 100ms, 10Hz
    {
    char serialOutString[78];  
          // send serial data if enabled
          
              // send G3X formatted data
              
              int percentLift;
              float displayAOA;
              float displayIAS;
              float smoothingAlpha=2.0/(serialDisplaySmoothing+1);
              int displayVerticalG;
#ifdef SPHERICAL_PROBE
  displayIAS=efisIAS;
#else
  displayIAS=IAS;
#endif      
              if (PaltSmoothed==0) PaltSmoothed=Palt*3.28084; else PaltSmoothed=Palt*3.28084 * smoothingAlpha/10+ (1-smoothingAlpha/10)*PaltSmoothed; // increased smoothing needed
              VerticalGSmoothed=aVert * smoothingAlpha+ (1-smoothingAlpha)*VerticalGSmoothed;
              displayVerticalG=ceil(VerticalGSmoothed * 10.0);
              
              LateralGSmoothed=aLat * smoothingAlpha+ (1-smoothingAlpha)*LateralGSmoothed;
              // don't output precentLift at low speeds.
              if (displayIAS>=muteAudioUnderIAS)
                  {
                  displayAOA=AOA;                
                  // scale percent lift
                  if (AOA<LDmaxAOA) percentLift=map(AOA,0,LDmaxAOA,0,50);
                      else
                          if (AOA>=LDmaxAOA && AOA<=onSpeedAOAfast) percentLift=map(AOA,LDmaxAOA,onSpeedAOAfast,50,55);
                              else
                                  if (AOA>onSpeedAOAfast && AOA<=onSpeedAOAslow) percentLift=map(AOA,onSpeedAOAfast,onSpeedAOAslow,55,66);
                                      else 
                                          if (AOA>onSpeedAOAslow && AOA<=stallWarningAOA) percentLift=map(AOA,onSpeedAOAslow,stallWarningAOA,66,90);
                                              else
                                                  percentLift=map(AOA,stallWarningAOA,stallWarningAOA*100/90,90,100);
                  percentLift=constrain(percentLift,0,99);                                                                                
                  } else
                        {
                        percentLift=0;
                        displayAOA=0;                        
                        }

              if (serialOutFormat == "G3X")
              {
              sprintf(serialOutString,"=1100000000%+04i%+05i___%04u%+06i____%+03i%+03i%02u__________",int(smoothedPitch*10),-int(smoothedRoll*10),unsigned(displayIAS*10),int(PaltSmoothed),int(-LateralGSmoothed*100),displayVerticalG,unsigned(percentLift));
              serialCRC=0x00;
              for (int i=0;i<=54;i++) serialCRC+=serialOutString[i];
              } else
                    if (serialOutFormat == "ONSPEED")
                        {
                        // 0 - Escape character  '#'
                        // 1 - Sentence ID '1'
                        // 2 - Pitch, 4 bytes, 0.1 degree, positive = up
                        // 6 - Roll, 5 bytes, 0.1 degree, positive = right
                        // 11 - IAS, 4 bytes, 0.1 kts
                        // 15 - PALT, 6 bytes, 1 ft
                        // 21 - Rate of Turn,  5 bytes, .1 deg/sec, positive = right
                        // 26 - Lateral G, 3 bytes , 0.01g, positive = leftward
                        // 29 - VerticalG, 3 bytes, 0.1g, positive = upward
                        // 32 - Percent Lift, 2 bytes, 00-99%
                        // 34 - AOA Degrees, 4 bytes, 0.1 degree
                        // 38 - iVSI, 4 bytes, 10 fpm, positive up [-999;999]
                        // 42 - OAT, 3 bytes, 1 deg C
                        // 45 - FlightPath angle, 4 bytes, 0.1 degree
                        // 49 - Flaps Pos, 3 bytes, 1 degree, with +/-sign
                        // 52 - StallWarn AOA, 4 bytes, 0.1 degrees
                        // 56 - OnSpeedSlow AOA, 4 bytes, 0.1 degrees
                        // 60 - OnSpeedFast AOA, 4 bytes, 0.1 degrees
                        // 64 - Tones On AOA, 4 bytes, 0.1 degrees
                        // 68 - G onset rate, 4 bytes, 0.01 G/sec
                        // 72 - Spin Recovery Cue, 2 bytes, -1/0/+1
                        // 74 - DataMark, 2 bytes
                        // 76 - Checksum, 2 bytes, ASCII HEX, sum of all previous bytes
                        // 78 - CR/LF,2 bytes, 0x0D 0x0A


//                        #
//                        1
//                        %+04i  Pitch
//                        %+05i  Roll
//                        %04u   IAS
//                        %+06i  PALT
//                        %+04i  Rate of Turn
//                        %+03i  LateralG
//                        %+03i  VerticalG
//                        %02i   Percent Lift
//                        %+04i  AOA Degrees
//                        %+04i  iVSI
//                        %+03i  OAT
//                        %+04i  FlightPath
//                        %+03i  Flaps
//                        %+04i  StallWarn
//                        %+04i  OnSpeedSlow
//                        %+04i  OnSpeedFast
//                        %+04i  Tones On
//                        %+04i  G onset rate
//                        %+02i  Spin Recovery Cue
//                        %02u   DataMark

                        
                        int gOnsetRate=0;
                        int spinRecoveryCue=0;
                        int OATc=0;
                        //Serial.printf("AOA: %.1f,pecentlift: %i\n",displayAOA,percentLift);
                        sprintf(serialOutString,"#1%+04i%+05i%04u%+06i%+05i%+03i%+03i%02u%+04i%+04i%+03i%+04i%+03i%+04i%+04i%+04i%+04i%+04i%+02i%02u",int(smoothedPitch*10),int(-smoothedRoll*10),unsigned(displayIAS*10),int(PaltSmoothed),int(gYaw*10),int(-LateralGSmoothed*100),displayVerticalG,unsigned(percentLift),int(displayAOA*10),int(floor(VSI*196.85/10)),int(OATc),int (flightPath*10),int(flapsPos),int(stallWarningAOA*10),int(onSpeedAOAslow*10),int(onSpeedAOAfast*10),int(LDmaxAOA*10),int(gOnsetRate*100),int(spinRecoveryCue),unsigned(dataMark));                        
                        serialCRC=0x00;
                        for (int i=0;i<=75;i++) serialCRC+=serialOutString[i];                       
                        }
              
              //serialCRC= serialCRC & 0xFF;
              if (serialOutPort=="Serial5")
                  {                 
                  Serial5.print(serialOutString); 
                  //Serial5.print(serialCRC,HEX);
                  Serial5.printf("%02X",serialCRC);
                  Serial5.println();
                  } else
                        if (serialOutPort=="Serial3")
                            {
                             if (!Serial3) 
                                {
                                Serial3.begin(115200);
                                }
                            Serial3.print(serialOutString);    
                            Serial3.printf("%02X",serialCRC);
                            Serial3.println();
                            } else
                                  if (serialOutPort=="Serial1")
                                      {
                                       if (!Serial1) 
                                          {
                                          Serial1.begin(115200);
                                          }
                                      Serial1.print(serialOutString);
                                      Serial1.printf("%02X",serialCRC);
                                      Serial1.println();
                                      }
                  
    serialoutLastUpdate=millis();                    
    }
}
