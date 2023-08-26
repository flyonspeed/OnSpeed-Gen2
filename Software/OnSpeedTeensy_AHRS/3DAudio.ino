void Check3DAudio()    
{
  float smoothingFactor=0.1;
  float curveGain;
  float lateralG=aLatCorr;
  int signLateralG= (lateralG > 0) - (lateralG < 0);
  curveGain=AUDIO_3D_CURVE(abs(lateralG));
  if (curveGain>1) curveGain=1;
  if (curveGain<0) curveGain=0;
  curveGain=curveGain*signLateralG;
  channelGain = smoothingFactor * curveGain + (1-smoothingFactor) * channelGain;
  ampLeft.gain(abs(-1+channelGain));
  ampRight.gain(abs(1+channelGain));
  #ifdef AUDIO_3D_DEBUG
  Serial.printf("%0.3fG, Left: %0.3f, Right: %0.3f\n",lateralG,abs(-1+channelGain),abs(1+channelGain));
  #endif
}    
