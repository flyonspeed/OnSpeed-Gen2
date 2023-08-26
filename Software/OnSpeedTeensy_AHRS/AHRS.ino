void processAHRS()
  {
  #ifdef OAT_AVAILABLE
  ISA_temp_k=15-Temp_rate*Palt+Kelvin;
  OAT_k= OAT + Kelvin;
  DA = Palt+(ISA_temp_k/Temp_rate)*(1-pow(ISA_temp_k/OAT_k,0.2349690));
  TAS=KTS2MPS*IAS/pow(1-6.8755856*pow(10,-6) * DA,2.12794); // formulas from https://edwilliams.org/avform147.htm#Mach   // m/sec
  #else
  TAS=IAS*(1+ Palt / 1000 * 0.02) * KTS2MPS; // m/sec
  #endif

  // diff IAS and then smooth it. Used for forward acceleration correction
  TASdiff=TAS-prevTAS;
  prevTAS=TAS;
  TASdiffSmoothed=iasSmoothing*TASdiff+(1-iasSmoothing)*TASdiffSmoothed;

  // all TAS are in m/sec at this point
    
  // update AHRS           

  // correct for installation error
  installPitchRad=pitchBias * DEG2RAD;
  installRollRad=rollBias * DEG2RAD;
  installYawRad=0.0; // assuming zero yaw (twist) on install


  // calculate installation corrected gyro rates
  rollRateCorr=Gx * cos(installPitchRad) * cos(installYawRad) + Gy * ( cos(installYawRad) * sin(installRollRad) * sin(installPitchRad) * - sin(installYawRad) * cos(installRollRad) ) + Gz * ( cos(installYawRad) * cos(installRollRad) * sin(installPitchRad) + sin(installYawRad) * sin(installRollRad));
  pitchRateCorr=Gx * cos(installPitchRad) * sin(installYawRad) + Gy * ( sin(installYawRad) * sin(installRollRad) * sin(installPitchRad) + cos(installYawRad) * cos(installRollRad)) + Gz * ( sin(installYawRad) * cos(installRollRad) * sin(installPitchRad) - cos(installYawRad) * sin(installRollRad));
  yawRateCorr=Gx * -sin(installPitchRad) + Gy * sin(installRollRad) * cos(installPitchRad) + Gz * cos(installPitchRad) * cos(installRollRad); 
  
  // displacement from CG calculation is omitted
  aVertCorr=-Ax * sin(installPitchRad) + Ay * sin(installRollRad) * cos(installPitchRad) + Az * cos(installRollRad) * cos(installPitchRad);                                         
  aLatCorr= Ax * cos(installPitchRad) * sin(installYawRad) + Ay * (sin(installYawRad) * sin(installPitchRad) * sin(installRollRad) + cos(installYawRad) * cos(installRollRad)) + Az * ( sin(installYawRad) * cos(installRollRad) * sin(installPitchRad)- cos(installYawRad) * sin(installRollRad));
  aFwdCorr= Ax * cos(installPitchRad) * cos(installYawRad) + Ay * (sin(installRollRad) * sin(installPitchRad) * cos(installYawRad) - sin(installYawRad) * cos(installRollRad)) + Az * ( cos(installYawRad)  * cos(installRollRad) * sin(installPitchRad)+sin(installYawRad) * sin(installRollRad));



// average gyro values, not used for AHRS
  GxAvg.addValue(rollRateCorr);
  gRoll=GxAvg.getFastAverage();
  GyAvg.addValue(pitchRateCorr);
  gPitch=GyAvg.getFastAverage();
  GzAvg.addValue(yawRateCorr);
  gYaw=GzAvg.getFastAverage();

                  
  // calculate linear acceleration compensation
  // correct for forward acceleration
  aFwdCp=(TASdiffSmoothed)/(1/imuSampleRate) * MPS2G; //1/208hz (update rate), m/sec2 to g
  //centripetal acceleration in m/sec2 = speed in m/sec * angular rate in radians
  aLatCp=TAS * yawRateCorr * DEG2RAD * MPS2G;
  aVertCp=TAS * pitchRateCorr * DEG2RAD * MPS2G; // TAS knots to m/sec, pitchrate in radians, m/sec2 to g
  
  // aVertCorr = install corrected acceleration, unsmoothed
  // aVert= install corrected acceleration, smoothed
  // avertComp = install corrected compensated acceleration, smoothed
  // aVert=avg(AvertCorr)
  // aVertCp= centripetal compensation
  // aVertComp=avg(AvertCorr)+avg(avertCp);
  
 // smooth Accelerometer values and add compensation
  //aFwdCorrAvg.addValue(aFwdCorr);
  //aFwd=aFwdCorrAvg.getFastAverage(); // corrected, smoothed
  aFwdSmoothed=accSmoothing*aFwdCorr+(1-accSmoothing)*aFwdSmoothed;
  aFwdComp=aFwdSmoothed-aFwdCp; //corrected, smoothed and compensated
  
  aLatSmoothed=accSmoothing*aLatCorr+(1-accSmoothing)*aLatSmoothed;
  aLatComp=aLatSmoothed-aLatCp; //corrected, smoothed and compensated
  
  aVertSmoothed=accSmoothing*aVertCorr+(1-accSmoothing)*aVertSmoothed;
  aVertComp=aVertSmoothed+aVertCp; //corrected, smoothed and compensated
                        
  filter.updateIMU(rollRateCorr, pitchRateCorr, yawRateCorr, aFwdComp, aLatComp, aVertComp);
                          
  smoothedPitch= -filter.getPitch();
  smoothedRoll= -filter.getRoll();


// calculate VSI and flightpath
  filter.getQuaternion(&q[0],&q[1],&q[2],&q[3]);
  // get earth referenced vertical acceleration
  earthVertG= 2.0f * (q[1]*q[3] - q[0]*q[2]) * aFwdCorr + 2.0f * (q[0]*q[1] + q[2]*q[3]) * aLatCorr + (q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3]) * aVertCorr - 1.0f;

  kalman.Update(Palt * FT2M , earthVertG * G2MPS, float(1/imuSampleRate), &kalmanAlt, &kalmanVSI); // altitude in meters, acceleration in m/s^2

  // zero VSI when airspeed is not yet alive
  if (IAS<25) kalmanVSI=0;
  
// calculate flight path and derived AOA                   
  if (IAS!=0) flightPath=asin(kalmanVSI/TAS) * RAD2DEG; // TAS in m/s, radians to degrees
      else flightPath=0;
  derivedAOA=smoothedPitch-flightPath;
    
}

float calcPitch(float aFwd,float aLat,float aVert)
{
return atan2(aFwd, sqrt(aLat* aLat + aVert *aVert)) * RAD2DEG;
}

float calcRoll(float aFwd,float aLat,float aVert)
{
return atan2(aLat, sqrt(aFwd* aFwd + aVert *aVert)) * RAD2DEG;  
}
