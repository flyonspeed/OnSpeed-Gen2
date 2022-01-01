void processAHRS()
  {
  // diff IAS and then smooth it. Used for forward acceleration correction
  float IASdiff=IAS-prevIAS;
  IASdiffAvg.addValue(IASdiff);
  smoothedIASdiff=IASdiffAvg.getFastAverage();
  prevIAS=IAS;

  TASAvg.addValue((IAS+IAS * Palt / 1000 * 0.02) * 0.514444); // ballpark TAS 2% per thousand feet pressure altitude
  smoothedTAS=TASAvg.getFastAverage();
  
  // update AHRS           

  // correct for installation error
  float installPitchRad=pitchBias * DEG2RAD;
  float installRollRad=rollBias * DEG2RAD;
  float installYawRad=0.0; // assuming zero yaw (twist) on install
 
  float aVertCorr=-Ax * sin(installPitchRad) +Ay * sin(installRollRad) * cos(installPitchRad)+ Az * cos(installRollRad) * cos(installPitchRad);                                         
  
  float aLatCorr= Ax * cos(installPitchRad) * sin(installYawRad) + Ay * (sin(installYawRad) * sin(installPitchRad) * sin(installRollRad) + cos(installYawRad) * cos(installRollRad)) + Az * ( sin(installYawRad) * cos(installRollRad) * sin(installPitchRad)- cos(installYawRad) * sin(installRollRad));
  
  float aFwdCorr= Ax * cos(installPitchRad) * cos(installYawRad) + Ay * (sin(installRollRad) * sin(installPitchRad) * cos(installYawRad) - sin(installYawRad) * cos(installRollRad)) + Az * ( cos(installYawRad)  * cos(installRollRad) * sin(installPitchRad)+sin(installYawRad) * sin(installRollRad));

// average gyro values
  GxAvg.addValue(Gx);
  gRoll=GxAvg.getFastAverage();
  GyAvg.addValue(Gy);
  gPitch=GyAvg.getFastAverage();
  GzAvg.addValue(Gz);
  gYaw=GzAvg.getFastAverage();
                  
  // calculate linear acceleration compensation
  // correct for forward acceleration
  float aFwdCp=-smoothedIASdiff * 0.514444/(1/imuSampleRate) * MPS2G + kalmanVSI * gPitch * DEG2RAD * MPS2G; // // knots to m/sec, 1/238hz (update rate), m/sec2 to g
  //centripetal acceleration in m/sec2 = speed in m/sec * angular rate in radians
  float aLatCp=-smoothedTAS * gYaw * DEG2RAD * MPS2G - kalmanVSI * -gRoll * DEG2RAD * MPS2G; // use smoothed gyro values
  float aVertCp=smoothedTAS * gPitch * DEG2RAD * MPS2G; // TAS knots to m/sec, pitchare in radians, m/sec2 to g
  
  // aVertCorr = install corrected acceleration, unsmoothed
  // aVert= install corrected acceleration, smoothed
  // avertComp = install corrected compensated acceleration, smoothed
  // aVert=avg(AvertCorr)
  // aVertCp= centripetal compensation
  // aVertComp=avg(AvertCorr)+avg(avertCp);
  
 // compensate and smooth Accelerometer values                   
  aFwdCompAvg.addValue(aFwdCp);
  aFwdCorrAvg.addValue(aFwdCorr);
  aFwd=aFwdCorrAvg.getFastAverage(); // corrected, smoothed
  aFwdComp=aFwd+aFwdCompAvg.getFastAverage(); //corrected, compensated, smoothed

  aLatCompAvg.addValue(aLatCp);
  aLatCorrAvg.addValue(aLatCorr);
  aLat=aLatCorrAvg.getFastAverage();
  aLatComp=aLat+aLatCompAvg.getFastAverage();

  aVertCompAvg.addValue(aVertCp);
  aVertCorrAvg.addValue(aVertCorr);
  aVert=aVertCorrAvg.getFastAverage();
  aVertComp=aVert+aVertCompAvg.getFastAverage();

                        
  //Serial.printf("RAW: %0.1f,%0.1f,%0.1f,%0.1f,%0.1f,%0.1f\n",getAccelForAxis(forwardGloadAxis),getAccelForAxis(lateralGloadAxis),getAccelForAxis(verticalGloadAxis),getGyroForAxis(pitchGyroAxis),getGyroForAxis(rollGyroAxis),getGyroForAxis(yawGyroAxis));
  //Serial.printf("RAW: %0.1f,%0.1f,%0.1f,%0.1f,%0.1f,%0.1f\n",aFwdComp,aLatComp,aVertComp,Gx,Gy,Gz);
  filter.updateIMU(gRoll, gPitch, gYaw, aFwdComp, aLatComp, aVertComp);
                      
  // calculate smoothed pitch                      
  float ahrsSmoothingAlpha=2.0/(ahrsSmoothing+1);
  smoothedPitch= -filter.getPitch() * ahrsSmoothingAlpha+(1-ahrsSmoothingAlpha)*smoothedPitch;
  smoothedRoll= filter.getRoll() * ahrsSmoothingAlpha +(1-ahrsSmoothingAlpha)*smoothedRoll;

// MadgWick time is 10-11uS
// update kalman filter
  float q[4];
  filter.getQuaternion(&q[0],&q[1],&q[2],&q[3]);
  // get earth referenced vertical acceleration
  earthVertGTotal+= 2.0f * (q[1]*q[3] - q[0]*q[2]) * aFwdCorr + 2.0f * (q[0]*q[1] + q[2]*q[3]) * aLatCorr + (q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3]) * aVertCorr - 1.0f;
  //Serial.printf("q0: %.2f, q1: %.2f, q2: %.2f, q3: %.2f\n",q[0],q[1],q[2],q[3]);

  earthVertGCount++;
  if (newSensorDataAvailable)
     {
      //calculate avg earth vertical G between baro/sensor reads
      //earthVertG=earthVertGTotal/earthVertGCount;
      earthVertG=2.0f * (q[1]*q[3] - q[0]*q[2]) * aFwdCorr + 2.0f * (q[0]*q[1] + q[2]*q[3]) * aLatCorr + (q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3]) * aVertCorr - 1.0f;
      kalman.Update(Palt * FT2M , earthVertG * MPS2G, 0.02, &kalmanAlt, &kalmanVSI); // altitude in meters, acceleration in m/s^2  (.002 = 50 hz sensor rate)    
      newSensorDataAvailable=false;
      earthVertGTotal=0.0;
      earthVertGCount=0;
     } 
  
// calculate flight path and derived AOA                   
  if (smoothedTAS!=0) flightPath=asin(kalmanVSI/smoothedTAS) * RAD2DEG; // TAS in m/s, radians to degrees
      else flightPath=0;
  derivedAOA=smoothedPitch-flightPath;

  // kalman time is 1-2uS
  //if (millis()-lastReplayOutput>=100)
  //    {
  //    noInterrupts();
      //Serial.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",efisPitch,smoothedPitch,efisRoll,smoothedRoll,AOA,derivedAOA,Palt);
      //Serial.printf("Ax: %.2f, Ay: %.2f, Az: %.2f, Gx: %.2f, Gy: %.2f, Gz: %.2f,\n",Ax,Ay,Az,Gx,Gy,Gz);
      //Serial.printf("Ax: %.2f, AxCorr: %.2f, Ay: %.2f, AyCorr: %.2f, Az: %.2f AzCorr: %.2f\n",Ax,aFwdCorr,Ay,aLatCorr,Az,aVertCorr);
      //Serial.printf("Ax: %.2f, AxCp: %.2f, Ay: %.2f, AyCp: %.2f, Az: %.2f AzCp: %.2f\n",Ax,aFwdComp,Ay,aLatComp,Az,aVertComp);
      //Serial.printf("Gx: %.2f, GxCorr: %.2f, Gy: %.2f, GyCorr: %.2f, Gz:  %.2f, GzCorr: %.2f\n",Gx,RollRateCorrRad* 57.2957,Gy,PitchRateCorrRad* 57.2957,Gy,YawRateCorrRad* 57.2957);

      //Serial.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",Gx,RollRateCorrRad* 57.2957,Gy,PitchRateCorrRad* 57.2957,Gy,YawRateCorrRad* 57.2957);
      
      //Serial.printf("%.2f,%.2f\n",-filter.getPitch(),filter.getRoll());

      //float calcPitch=atan2(Ax, sqrt(Ay* Ay + Az *Az)) * 57.2957;
      //float calcRoll = -atan2(Ay, sqrt(Ax* Ax + Az *Az)) * 57.2957;
      //Serial.printf("%.2f,%.2f\n",calcPitch,calcRoll);
      //Serial.printf("%.2f,%.2f\n",calcPitch,smoothedPitch);
      //Serial.printf("%.2f,%.2f\n",calcRoll,smoothedRoll);
      //float calcPitch=atan2((aFwd), sqrt((aLat)* (aLat) + (aVert) *(aVert))) * 57.2957;
      //Serial.printf("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",-filter.getPitch(),efisPitch,calcPitch,aFwd,aFwdCp, aFwdComp,aLat,aLatCp, aLatComp,aVert,aVertCp, aVertComp,gRoll,gPitch,gYaw,imuSampleRate);
      //Serial.printf("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",-filter.getPitch(),filter.getRoll(),efisPitch,calcPitch,Gx,Gy,Gz);
      //Serial.printf("earthVertG: %.2f\n",earthVertG);
      //Serial.printf("kalmanAlt: %.2f,kalmanVSI: %.2f\n",kalmanAlt,kalmanVSI);
      //Serial.printf("%.6f\n",Ax);
//      interrupts();
  //    lastReplayOutput=millis();
  //    }
  
}

float calcPitch(float aFwd,float aLat,float aVert)
{
return atan2(aFwd, sqrt(aLat* aLat + aVert *aVert)) * RAD2DEG;
}

float calcRoll(float aFwd,float aLat,float aVert)
{
return atan2(aLat, sqrt(aFwd* aFwd + aVert *aVert)) * RAD2DEG;  
}
