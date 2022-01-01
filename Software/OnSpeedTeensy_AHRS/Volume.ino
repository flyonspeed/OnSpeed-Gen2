void checkVolume()
{
float volumeSmoothingFactor=0.5;
volPos = volumeSmoothingFactor * analogRead(VOLUME_PIN) + (1-volumeSmoothingFactor) * volPos;    
#ifdef VOLUMEDEBUG
int volPosThreshold=2;
if (millis()-volumeStartTime>250)
    {
    float percentChange=abs((avgSlowVolPos-volPos)/10.24);
    if (percentChange>volPosThreshold)
      {
      Serial.println(volPos);
      Serial.println(percentChange);
      }
    avgSlowVolPos=volPos;
    volumeStartTime=millis();
    }
#endif             
//float toneGain=mapfloat(volPos,volumeLowAnalog,volumeHighAnalog,0,2);
//float voiceGain=mapfloat(volPos,volumeLowAnalog,volumeHighAnalog,0,10);
volumePercent=mapfloat(volPos,volumeLowAnalog,volumeHighAnalog,0,100);
setVolume(volumePercent);
}

void setVolume(int volumePercent)
{
mixer1.gain(2,10*volumePercent/100.00); // channel 2 gain (voice)
mixer1.gain(0,volumePercent/100.00);  // channel 0 gain (tones)
#ifdef VOLUMEDEBUG
Serial.printf("Volume: %i, VoiceGain: %.2f, ToneGain: %.2f\n",volumePercent,float(10*volumePercent/100.00),float(volumePercent/100.00));
#endif
}
