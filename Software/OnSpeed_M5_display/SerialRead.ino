 void SerialRead()
 {
 char inChar;
 if (Serial2.available())
    {                                                 
    inChar=Serial2.read();
    if (inChar=='#')
        {
        // reset RX buffer  
        serialBufferString=inChar;
        return;
        }
    if  (serialBufferString.length()>80)
        {
        // prevent buffer overflow;  
        serialBufferString=""; 
        Serial.println("Serial data buffer overflow");
        Serial.println(serialBufferString);
        return;
        }
    if (serialBufferString.length()>0)
        {            
         serialBufferString+=inChar;
         
         if (serialBufferString.length()==80 && serialBufferString[0]=='#' && serialBufferString[1]=='1' && inChar== char(0x0A) ) // ONSPEED protocol
             {
              #ifdef SERIALDATADEBUG
              Serial.println(serialBufferString);
              #endif
             // parse Onspeed data stream
             String parseString;
             //calculate CRC
             int calcCRC=0;                           
             for (int i=0;i<=75;i++) calcCRC+=serialBufferString[i];
             calcCRC=calcCRC & 0xFF;
             if (calcCRC==(int)strtol(&serialBufferString.substring(76, 78)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                {
                  // CRC passed                                                                                                                           
                 parseString=serialBufferString.substring(2, 6);
                 Pitch=parseString.toFloat()/10;
                 
                 parseString=serialBufferString.substring(6, 11);
                 Roll=parseString.toFloat()/10;                                    

                 parseString=serialBufferString.substring(11, 15);
                 IAS=parseString.toFloat()/10;
                 
                 parseString=serialBufferString.substring(15, 21);
                 Palt=parseString.toFloat();
                 
                 parseString=serialBufferString.substring(21, 26);                 
                 TurnRate=parseString.toFloat()/10;
                 
                 parseString=serialBufferString.substring(26, 29);
                 LateralG=parseString.toFloat()/100;             
                 
                 parseString=serialBufferString.substring(29, 32);
                 VerticalG=parseString.toFloat()/10;
                 
                 parseString=serialBufferString.substring(32, 34);
                 PercentLift=parseString.toInt();

                 parseString=serialBufferString.substring(34, 38);
                 AOA=parseString.toFloat()/10;               
                 
                 parseString=serialBufferString.substring(38, 42);
                 iVSI=parseString.toFloat()*10;

                 parseString=serialBufferString.substring(42, 45);
                 OAT=parseString.toInt();

                 parseString=serialBufferString.substring(45, 49);
                 FlightPath=parseString.toFloat()/10;

                 parseString=serialBufferString.substring(49, 52);
                 FlapPos=parseString.toInt();

                 parseString=serialBufferString.substring(52, 56);
                 OnSpeedStallWarnAOA=parseString.toFloat()/10;

                 parseString=serialBufferString.substring(56, 60);
                 OnSpeedSlowAOA=parseString.toFloat()/10;

                 parseString=serialBufferString.substring(60, 64);
                 OnSpeedFastAOA=parseString.toFloat()/10;

                 parseString=serialBufferString.substring(64, 68);
                 OnSpeedTonesOnAOA=parseString.toFloat()/10;

                 parseString=serialBufferString.substring(68, 72);
                 gOnsetRate=parseString.toFloat()/100;

                 parseString=serialBufferString.substring(72, 74);
                 SpinRecoveryCue=parseString.toInt();     

                 parseString=serialBufferString.substring(74, 76);
                 DataMark=parseString.toInt();

                 serialBufferString="";

                 // smooth the noisier inputs

                 if (AOA==-100) AOA=0.0;  // don't display invalid values;
                                  
                 SmoothedLateralG = SmoothedLateralG * slipSmoothingAlpha+(1-slipSmoothingAlpha)*LateralG;
                 Slip = int(SmoothedLateralG * 34 / 0.075); //.075g=half ball, .15g= 1 ball
                 Slip=constrain(Slip,-99,99);
                 SmoothedAOA = SmoothedAOA * aoaSmoothingAlpha+(1-aoaSmoothingAlpha)*AOA;
                   // compute IAS derivative (deceleration)
                  iasDerivativeInput=IAS;
                  DecelRate=-iasDerivative.Compute();
                  DecelRate=DecelRate/serialRate;
                  SmoothedDecelRate=DecelRate*decelSmoothingAlpha+SmoothedDecelRate*(1-decelSmoothingAlpha);

                                                                
                 #ifdef SERIALDATADEBUG
                 Serial.printf("ONSPEED data: Millis %i, IAS %.2f, Pitch %.1f, Roll %.1f, LateralG %.2f, VerticalG %.2f, Palt %0.1f, iVSI %.1f, AOA: %.1f", millis()-serialMillis, IAS, Pitch, Roll, LateralG, VerticalG, Palt, iVSI,SmoothedAOA);
                 Serial.println();
                 #endif                                                                                                      
                 serialMillis=millis();
                 }                                                 
                 else Serial.println("ONSPEED CRC Failed");
                                                                                             
              }
         } // line end    
  } // available

 }

/*
  void g3xRead()
 {
 char inChar;
 if (Serial2.available())
    {                                                 
    inChar=Serial2.read();
    if  (serialBufferString.length()>=150)
        {
        serialBufferString=""; // prevent buffer overflow;
        Serial.printf("Serial data buffer overflow");
        Serial.println(serialBufferString);
        }
    if ((serialBufferString.length()>=0 && inChar== char(0x0A)))
        {            
         serialBufferString+=inChar;
         if (serialBufferString.length()==59 && serialBufferString[0]=='=' && serialBufferString[1]=='1')
             {
             // parse Onspeed data stream
             String parseString;
             //calculate CRC
             int calcCRC=0;                           
             for (int i=0;i<=54;i++) calcCRC+=serialBufferString[i];                     
             calcCRC=calcCRC & 0xFF;
             if (calcCRC==(int)strtol(&serialBufferString.substring(55, 57)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                {
                  // CRC passed                                                                                                          
                 parseString=serialBufferString.substring(23, 27);
                 if (parseString!="____") IAS=parseString.toFloat()/10;
                 parseString=serialBufferString.substring(11, 15);
                 if (parseString!="____") Pitch=parseString.toFloat()/10;
                 parseString=serialBufferString.substring(15, 20);
                 if (parseString!="_____") Roll=parseString.toFloat()/10;                                                    
                 parseString=serialBufferString.substring(20, 23);
                 //if (parseString!="___") efisHeading=parseString.toInt();
                 parseString=serialBufferString.substring(37, 40);
                 if (parseString!="___") LateralG=parseString.toFloat()/100;                            
                 parseString=serialBufferString.substring(40, 43);
                 if (parseString!="___") VerticalG=parseString.toFloat()/10;
                 parseString=serialBufferString.substring(27, 33);
                 if (parseString!="______") Palt=parseString.toInt(); // feet
                 parseString=serialBufferString.substring(45, 49);
                 if (parseString!="____") iVSI=parseString.toInt()*10; //10 fpm
                                                          
                 #ifdef SERIALDATADEBUG
                 Serial.printf("ONSPEED data: Millis %i, IAS %.2f, Pitch %.2f, Roll %.2f, LateralG %.2f, VerticalG %.2f, Palt %i, VSI %i", millis()-serialMillis, IAS, Pitch, Roll, LateralG, VerticalG, Palt, iVSI);                        
                 Serial.println();
                 #endif
                 serialMillis=millis();                                                                                                
                 }                                                 
                 else Serial.println("ONSPEED CRC Failed");
                                                                                             
              } else Serial.println("ONSPEED Serial line wrong length");
         } // line end    
  } // available

 }
 */
