void processAHRS()
  {
  // diff IAS and then smooth it. Used for forward acceleration correction
  float IASdiff=IAS-prevIAS;
  IASdiffAvg.addValue(IASdiff);
  smoothedIASdiff=IASdiffAvg.getFastAverage();
  prevIAS=IAS;

  TASAvg.addValue(IAS*(1+ Palt / 1000 * 0.02) * 0.514444); 
  smoothedTAS=TASAvg.getFastAverage();
  
  // update AHRS           

  // correct for installation error
  float installPitchRad=pitchBias * DEG2RAD;
  float installRollRad=rollBias * DEG2RAD;
  float installYawRad=0.0; // assuming zero yaw (twist) on install
 
  float aVertCorr=-Ax * sin(installPitchRad) +Ay * sin(installRollRad) * cos(installPitchRad)+ Az * cos(installRollRad) * cos(installPitchRad);                                         
  
  float aLatCorr= Ax * cos(installPitchRad) * sin(installYawRad) + Ay * (sin(installYawRad) * sin(installPitchRad) * sin(installRollRad) + cos(installYawRad) * cos(installRollRad)) + Az * ( sin(installYawRad) * cos(installRollRad) * sin(installPitchRad)- cos(installYawRad) * sin(installRollRad));
  
  float aFwdCorr= Ax * cos(installPitchRad) * cos(installYawRad) + Ay * (sin(installRollRad) * sin(installPitchRad) * cos(installYawRad) - sin(installYawRad) * cos(installRollRad)) + Az * ( cos(installYawRad)  * cos(installRollRad) * sin(installPitchRad)+sin(installYawRad) * sin(installRollRad));

 // scale correct gyro rates
  float rollRate=Gx*gyroScaleCorrection;
  float pitchRate=Gy; // no correction needed
  float yawRate=Gz*gyroScaleCorrection;

  // calculate installation corrected gyro rates
  float rollRateCorr=rollRate * cos(installPitchRad) * cos(installYawRad) + pitchRate * ( cos(installYawRad) * sin(installRollRad) * sin(installPitchRad) * - sin(installYawRad) * cos(installRollRad) ) + yawRate * ( cos(installYawRad) * cos(installRollRad) * sin(installPitchRad) + sin(installYawRad) * sin(installRollRad));
  float pitchRateCorr=rollRate * cos(installPitchRad) * sin(installYawRad) + pitchRate * ( sin(installYawRad) * sin(installRollRad) * sin(installPitchRad) + cos(installYawRad) * cos(installRollRad)) + yawRate * ( sin(installYawRad) * cos(installRollRad) * sin(installPitchRad) - cos(installYawRad) * sin(installRollRad));
  float yawRateCorr=rollRate * -sin(installPitchRad) + pitchRate * sin(installRollRad) * cos(installPitchRad) + yawRate * cos(installPitchRad) * cos(installRollRad); 

// average gyro values, not used for AHRS
  GxAvg.addValue(rollRateCorr);
  gRoll=GxAvg.getFastAverage();
  GyAvg.addValue(pitchRateCorr);
  gPitch=GyAvg.getFastAverage();
  GzAvg.addValue(yawRateCorr);
  gYaw=GzAvg.getFastAverage();

                  
  // calculate linear acceleration compensation
  // correct for forward acceleration
  float aFwdCp=smoothedIASdiff * 0.514444/(1/imuSampleRate) * MPS2G; // knots to m/sec, 1/208hz (update rate), m/sec2 to g
  //centripetal acceleration in m/sec2 = speed in m/sec * angular rate in radians
  float aLatCp=smoothedTAS * yawRateCorr * DEG2RAD * MPS2G;
  float aVertCp=smoothedTAS * pitchRateCorr * DEG2RAD * MPS2G; // TAS knots to m/sec, pitchrate in radians, m/sec2 to g
  
  // aVertCorr = install corrected acceleration, unsmoothed
  // aVert= install corrected acceleration, smoothed
  // avertComp = install corrected compensated acceleration, smoothed
  // aVert=avg(AvertCorr)
  // aVertCp= centripetal compensation
  // aVertComp=avg(AvertCorr)+avg(avertCp);
  
 // smooth Accelerometer values and add compensation
  aFwdCorrAvg.addValue(aFwdCorr);
  aFwd=aFwdCorrAvg.getFastAverage(); // corrected, smoothed
  aFwdComp=aFwd-aFwdCp; //corrected, smoothed and compensated

  aLatCorrAvg.addValue(aLatCorr);
  aLat=aLatCorrAvg.getFastAverage();
  aLatComp=aLat-aLatCp;

  aVertCorrAvg.addValue(aVertCorr);
  aVert=aVertCorrAvg.getFastAverage();
  aVertComp=aVert+aVertCp;
                        
  filter.updateIMU(rollRateCorr, pitchRateCorr, yawRateCorr, aFwdComp, aLatComp, aVertComp);
                      
  // calculate smoothed pitch
  float ahrsSmoothingAlpha=2.0/(ahrsSmoothing+1);
  smoothedPitch= -filter.getPitch() * ahrsSmoothingAlpha+(1-ahrsSmoothingAlpha)*smoothedPitch;
  smoothedRoll= -filter.getRoll() * ahrsSmoothingAlpha +(1-ahrsSmoothingAlpha)*smoothedRoll;


// calculate VSI and flightpath
  float q[4];
  filter.getQuaternion(&q[0],&q[1],&q[2],&q[3]);
  // get earth referenced vertical acceleration
  earthVertG= 2.0f * (q[1]*q[3] - q[0]*q[2]) * aFwdCorr + 2.0f * (q[0]*q[1] + q[2]*q[3]) * aLatCorr + (q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3]) * aVertCorr - 1.0f;

  kalman.Update(Palt * FT2M , earthVertG * MPS2G, 1/208, &kalmanAlt, &kalmanVSI); // altitude in meters, acceleration in m/s^2

  // zero VSI when airspeed is not yet alive
  if (IAS<25) kalmanVSI=0;
  
// calculate flight path and derived AOA                   
  if (smoothedTAS!=0) flightPath=asin(kalmanVSI/smoothedTAS) * RAD2DEG; // TAS in m/s, radians to degrees
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
