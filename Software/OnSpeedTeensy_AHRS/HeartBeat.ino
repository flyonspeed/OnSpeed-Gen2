void heartBeat()
{
if (switchState )
          {
          if (ledOn)
              {
              analogWrite(PIN_LED2, 0);
              ledOn=false;
              }
              else
                  {
                  analogWrite(PIN_LED2, 200);
                  ledOn=true;                  
                  }                
          } else analogWrite(PIN_LED2,0);
}


     // breathing LED (more CPU intensive than flashing)
    //  if (millis()-lastLedUpdate>100)
    //      {
    //      if (switchState )
    //          {
    //          float ledBrightness = 15+(exp(sin(millis()/2000.0*PI)) - 0.36787944)*63.81; // funky sine wave, https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    //          analogWrite(PIN_LED2, ledBrightness);         
    //          } else analogWrite(PIN_LED2,0);
    //       lastLedUpdate=millis();    
    //      } 
