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
 
 float wifiAOA;
 float alphaVA=0.00;
 float wifiPitch=0;
 float wifiRoll=0;
 float wifiFlightpath=0;
 float wifiVSI=0;
 float wifiIAS=0;
 
 if (isnan(AOA) || IAS<muteAudioUnderIAS)
    {
    wifiAOA=-100;
    }
    else
        {
        // protect AOA from interrupts overwriting it  
        wifiAOA=AOA;
        }

// send efis data for the following values when using Efis as the calibration source.
// Pitch, Roll, VSI, Flightpath

if (calSource=="EFIS")
  {
  if (efisID==1)
     {
     // use Vectornav data
     wifiPitch=vnPitch;
     wifiRoll=vnRoll;
     if (smoothedTAS>0)
                      {
                      wifiFlightpath=asin(-vnVelNedDown/smoothedTAS) * RAD2DEG; // convert efiVSI from fpm to m/s, vnVelNedDown is reversed (positive when descending)
                      } else wifiFlightpath=0;     
                     
     wifiVSI=-vnVelNedDown*196.85; // fpm
     } else
            {
            //use parsed efis data
            wifiPitch=efisPitch;
            wifiRoll=efisRoll;
            if (smoothedTAS>0)
                      {
                      wifiFlightpath=asin(efisVSI*0.00508/smoothedTAS) * RAD2DEG; // convert efiVSI from fpm to m/s
                      } else wifiFlightpath=0;
            wifiVSI=efisVSI;          
            }             
 // send efisIAS if spherical probe is in use otehrwise use OnspeedIAS.        
 #ifdef SPHERICAL_PROBE
 wifiIAS=efisIAS;
 #else
 wifiIAS=IAS;
 #endif           
  
  }
   else
        {
        wifiPitch=smoothedPitch; // degrees
        wifiRoll=-smoothedRoll; // degrees
        wifiFlightpath=flightPath; // degrees
        wifiVSI=VSI*196.85; // fpm
        wifiIAS=IAS;
        }
      

 sprintf(crc_buffer,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.6f,%i,%.2f,%.2f,%.2f,%.2f",wifiAOA,wifiPitch,wifiRoll,wifiIAS,Palt*3.28084,verticalGload,aLat,alphaVA,LDmaxAOA,onSpeedAOAfast,onSpeedAOAslow,stallWarningAOA,flapsPos,coeffP,dataMark,wifiVSI,wifiFlightpath,gPitch,DecelRate);
 for (unsigned int i=0;i<strlen(crc_buffer);i++) CRC=CRC+char(crc_buffer[i]); // claculate simple CRC
 sprintf(json_buffer,"$ONSPEED,%s,%i\n",crc_buffer,CRC);
 Serial4.print(json_buffer);
 }
