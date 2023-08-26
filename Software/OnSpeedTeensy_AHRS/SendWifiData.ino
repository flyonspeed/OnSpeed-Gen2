void SendWifiData()
{
 // gloads changed by the IMU interrupt
 // accelerometrs are being updated in an interrupr
accelSumSq=aVertCorr*aVertCorr+aLatCorr*aLatCorr+aFwdCorr*aFwdCorr;
verticalGload=sqrt(abs(accelSumSq));
verticalGload=round(verticalGload * 10.0) / 10.0; // round to 1 decimal place
if (aVertCorr<0) verticalGload*=-1;
 
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

   // efis or VN-300 data 
  calSourceID=efisID; // send efis type to Wifi 
  if (efisID==1)
     {
     // use Vectornav data
     wifiPitch=vnPitch;
     wifiRoll=vnRoll;
     if (TAS>0)
                      {
                      //TAS is being updated in an interrupt  
                      wifiFlightpath=asin(-vnVelNedDown/TAS) * RAD2DEG; // vnVelNedDown is reversed (positive when descending)
                      } else wifiFlightpath=0;
                     
     wifiVSI=-vnVelNedDown*MPS2FPM; // fpm
     wifiIAS=IAS; // IAS is being updated in an interrupt
     } else
            {
            //use parsed efis data
            wifiPitch=efisPitch;
            wifiRoll=efisRoll;
            if (efisTAS>0)
                      {
                      //if efisTAS available
                      // kalmanVSI is being updated in an interrupt
                      wifiFlightpath=asin(kalmanVSI/efisTAS*KTS2MPS) * RAD2DEG; // convert efiVSI from fpm to m/s
                      } else
                            if (TAS>0)
                                {
                                // if TAS available
                                //TAS is being updated in an interrupt
                                wifiFlightpath=asin(kalmanVSI/TAS) * RAD2DEG; // convert efiVSI from fpm to m/s
                                } else wifiFlightpath=0;
            // kalmanVSI is being updated in an interrupt  
            wifiVSI=kalmanVSI*MPS2FPM;
            wifiIAS=efisIAS;
            }             
         
  
  }
   else
        {
        // internal data  
        calSourceID=0;
        // these values are being updated in an interrupt 
        wifiPitch=smoothedPitch; // degrees
        wifiRoll=smoothedRoll; // degrees
        wifiFlightpath=flightPath; // degrees
        wifiVSI=kalmanVSI*MPS2FPM; // fpm
         // send efisIAS if spherical probe is in use otherwise use OnspeedIAS.        
 #ifdef SPHERICAL_PROBE
        wifiIAS=efisIAS;
 #else
        wifiIAS=IAS; //IAS is being updated in an interrupt
 #endif  
        }
unsigned int CRC_len=sprintf(crc_buffer,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.6f,%i,%.2f,%.2f,%.2f,%.2f,%i",wifiAOA,wifiPitch,wifiRoll,wifiIAS,kalmanAlt*M2FT,verticalGload,aLatCorr,alphaVA,LDmaxAOA,onSpeedAOAfast,onSpeedAOAslow,stallWarningAOA,flapsPos,coeffP,dataMark,wifiVSI,wifiFlightpath,gPitch,DecelRate,calSourceID);
CRC=0;
for (unsigned int i=0;i<CRC_len;i++) CRC=CRC+char(crc_buffer[i]); // calculate simple CRC
Serial4.print("$ONSPEED,");
Serial4.print(crc_buffer);
Serial4.printf(",%i\n",CRC);

}
