void setFrequencytone(uint32_t frequency)
{
 float volume=0;
 float pulse_delay= 1000/pps; 
 float tone_length=pulse_delay-3; // tone_length must be below 100%, otherwise envelopes overlap

if (highTone && tonePlaying==SOLID_TONE)
    {
    // shorter delay when transitioning from solid to high tone
    ToneTimer.end();
    ToneTimer.begin(tonePlayHandler,(pulse_delay/2+1000/LOW_TONE_PPS_MAX/2)*1000); // create half the time of the original waveform + the half of LOW_TONE_PPS_MAX = shorter transition to high tone, better response rate
    } else
      {
      ToneTimer.begin(tonePlayHandler,pulse_delay*1000);
      }
// Serial.printf("millis: %i, freq update: %i, pps: %0.2f\n",millis(),(int)(pulse_delay*1000),pps);
#ifdef TONEDEBUG
  Serial.print(millis());
  Serial.print(", AOA: ");
  Serial.print(AOA);
  Serial.print(", IAS: ");
  Serial.print(IAS);   
  Serial.print(", pulse delay (ms):");
  Serial.print(pulse_delay);
  Serial.print(", pps: ");
  Serial.print(pps);
  Serial.print(", freq: ");
  Serial.print(frequency);
  Serial.print(", switchState:");
  Serial.print(switchState);
  Serial.print(", toneMode:");
  Serial.println(toneMode);
#endif 
 if (!switchState)
    {
    // if audio switch is off don't play any tones
    sinewave1.amplitude(volume);
    tonePlaying=TONE_OFF;
    return;
    }
    
  if(frequency < 20 || frequency > 20000 || toneMode == TONE_OFF) {
    #ifdef TONEDEBUG
    Serial.print("cancel tone: ");Serial.println(frequency);   
    #endif
    sinewave1.amplitude(volume); // turn off pulsed tone
    toneFreq = frequency;
    tonePlaying=TONE_OFF;
    return;
  } 
 if (toneMode==SOLID_TONE)
      {
      volume=SOLID_TONE_VOLUME;      
      if (tonePlaying!=SOLID_TONE) // if solid tone not already playing
          {
          AudioNoInterrupts();
          envelope1.noteOff(); // turn off previous note
          sinewave1.frequency(frequency);
          sinewave1.amplitude(volume);
          AudioInterrupts();
          
          AudioNoInterrupts();          
          envelope1.delay(1000/LOW_TONE_PPS_MAX/2); // this timing provides a smooth transition from low tones into solid and a quick transition from high tones back into to solid
          envelope1.attack(TONE_RAMP_TIME);
          envelope1.hold(0);
          envelope1.decay(0);
          envelope1.sustain(1);
          envelope1.release(TONE_RAMP_TIME);
          envelope1.releaseNoteOn(0);
          envelope1.noteOn();
          AudioInterrupts();
          
          tonePlaying=SOLID_TONE;
          }
      
      } else
      {
      sinewave1.frequency(frequency);
      // set tone volume
      if (!highTone)
                    {
                    // low tone
                    volume=LOW_TONE_VOLUME;                    
                    }
                    else if (highTone)
                                      {
                                      // high tone                                        
                                      if (pps==HIGH_TONE_STALL_PPS) volume=HIGH_TONE_VOLUME_MAX; else volume=constrain(mapfloat(pps,HIGH_TONE_PPS_MIN,HIGH_TONE_PPS_MAX,HIGH_TONE_VOLUME_MIN,HIGH_TONE_VOLUME_MAX),HIGH_TONE_VOLUME_MIN,HIGH_TONE_VOLUME_MAX);                                                                                                                      
                                      }                                                                                                                                                                                                                                    
      
      AudioNoInterrupts();
      envelope1.noteOff(); // turn off previous note
      AudioInterrupts();
      
      AudioNoInterrupts();      
      sinewave1.amplitude(volume);      
      // transition quicker from solid to a high tone
      if (highTone && tonePlaying==SOLID_TONE)
          {
          // shorten the delay on the first high tone  
          envelope1.delay((1000/LOW_TONE_PPS_MAX)/2);
          }
          else 
            {
            envelope1.delay(tone_length/2);
            }
      
      if (highTone && pps==HIGH_TONE_STALL_PPS)
          {
          // change tone ramp time on stall tone
          envelope1.attack(STALL_RAMP_TIME);
          envelope1.hold(tone_length/2-STALL_RAMP_TIME*2);
          envelope1.decay(STALL_RAMP_TIME);
          }
          else
            {
            envelope1.attack(TONE_RAMP_TIME);
            envelope1.hold(tone_length/2-TONE_RAMP_TIME*2);
            envelope1.decay(TONE_RAMP_TIME);
            }
      envelope1.sustain(0);
      envelope1.releaseNoteOn(0);
      envelope1.noteOn();
      AudioInterrupts();
      tonePlaying=PULSE_TONE;
      }     
  toneFreq = frequency;
}

void tonePlayHandler(){
    if (!switchState)
            {
            return; // return if tones are disabled 
            }
  if(toneMode==TONE_OFF) {
    setFrequencytone(0);  // if tone off skip the rest.
    #ifdef TONEDEBUG 
    Serial.println("TONE OFF");
    #endif
    return;
  }
  if(toneMode==SOLID_TONE) {  // check for a solid tone.
    #ifdef TONEDEBUG 
    Serial.println("SOLID TONE");
    #endif
    setFrequencytone(LOW_TONE_HZ);
    return; // skip the rest
  }
     if(highTone) {
                  setFrequencytone(HIGH_TONE_HZ);
                  #ifdef TONEDEBUG 
                  Serial.println("HIGH TONE");
                  #endif
                  } else {
                          setFrequencytone(LOW_TONE_HZ);
                          #ifdef TONEDEBUG 
                          Serial.println("LOW TONE");
                          #endif
                         } 
}


void updateTones()
{
  if(IAS <= muteAudioUnderIAS) {
  #ifdef TONEDEBUG    
  // show audio muted and debug info.
  sprintf(tempBuf, "AUDIO MUTED: Airspeed to low. Min:%i IAS:%.2f",muteAudioUnderIAS, IAS);
  Serial.println(tempBuf);
  #endif 
  toneMode = TONE_OFF;
  setPPSTone(20); // set the update rate to LOW_TONE_PPS_MAX if no tone is playing to pick up a pulsed tone quickly
  return;
  }
  // check AOA value and set tone and pauses between tones according to 
  if(AOA >= stallWarningAOA) {
    // play 20 pps HIGH tone
    highTone = true;
    setPPSTone(HIGH_TONE_STALL_PPS);
    toneMode = PULSE_TONE;
    } else if(AOA > (onSpeedAOAslow))
            {
            // play HIGH tone at Pulse Rate 1.5 PPS to 6.2 PPS (depending on AOA value)
            highTone = true;
            toneMode = PULSE_TONE;
            NewValue=mapfloat(AOA,onSpeedAOAslow,stallWarningAOA,HIGH_TONE_PPS_MIN,HIGH_TONE_PPS_MAX);
            setPPSTone(NewValue); // when transitioning from solid to high tone make the first one shorter
            } else if(AOA >= (onSpeedAOAfast))
                   {
                    // play a steady LOW tone
                    highTone = false;
                    toneMode = SOLID_TONE;
                    setPPSTone(LOW_TONE_PPS_MAX);// set PPS to LOW_TONE_PPS_MAX for a higher update rate when coming out of it
                   } else if (AOA >= LDmaxAOA && LDmaxAOA<onSpeedAOAfast)
                          {  // if L/D max AOA is higher than OnSpeedfast, skip the low tone. This usually happens with full flaps.
                          toneMode = PULSE_TONE;
                          highTone = false;
                          // play LOW tone at Pulse Rate 1.5 PPS to 8.2 PPS (depending on AOA value)
                          NewValue=mapfloat(AOA,LDmaxAOA,onSpeedAOAfast,LOW_TONE_PPS_MIN,LOW_TONE_PPS_MAX);
                          setPPSTone(NewValue);
                          } else {                          
                                 toneMode = TONE_OFF;
                                 setPPSTone(HIGH_TONE_STALL_PPS);  // high update rate pps 
                                 }    
}

void setPPSTone(float newPPS) {
   pps=newPPS;   
}
