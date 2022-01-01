void SendWifiData()
{
 char json_buffer[285];
 char crc_buffer[250];
 byte CRC=0;
 // gloads changed by the IMU interrupt
 float accelSumSq=aVert*aVert+aLat*aLat+aFwd*aFwd;
 float verticalGload=sqrt(abs(accelSumSq));
 verticalGload=round(verticalGload * 10.0) / 10.0; // round to 1 decimal place
 if (aVert<0) verticalGload*=-1;
 
 float displayAOA;
 float alphaVA=0.00;
 if (isnan(AOA) || IAS<muteAudioUnderIAS)
    {
    displayAOA=-100;
    }
    else
        {
        // protect AOA from interrupts overwriting it  
        displayAOA=AOA;
        }
 
 //flapsPercent=(float)flapsPos/(flapDegrees.Items[flapDegrees.Count-1]-flapDegrees.Items[0])*100; //flap angle / flap total travel *100 (needed for displaying partial flap donut on display)
 sprintf(crc_buffer,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.6f,%i,%.2f,%.2f,%.2f",displayAOA,smoothedPitch,-smoothedRoll,IAS,kalmanAlt*3.28084,verticalGload,aLat,alphaVA,LDmaxAOA,onSpeedAOAfast,onSpeedAOAslow,stallWarningAOA,flapsPos,coeffP,dataMark,kalmanVSI*196.85,flightPath,gPitch);
 for (unsigned int i=0;i<strlen(crc_buffer);i++) CRC=CRC+char(crc_buffer[i]); // claculate simple CRC
 sprintf(json_buffer,"$ONSPEED,%s,%i\n",crc_buffer,CRC);
 Serial4.print(json_buffer);
// for (unsigned int i=0; i<strlen(json_buffer);i++)
//      {                                                                                                                                        
//      Serial4.flush(); // prevent output buffer overrun
//      Serial4.print(json_buffer[i]); 
//      }
 }
