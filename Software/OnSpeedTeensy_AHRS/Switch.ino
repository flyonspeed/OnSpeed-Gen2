void switchOnOff()
     {     
     if (!switchState)
        {              
        // turn on
        analogWrite(PIN_LED2,200);
        // play turn on sound        
        voice1.play(AudioSampleEnabled);
        delay (1500);
        switchState=true;
          
  
        Serial.println("\nSwitch on");
        } else
              {              
              // turn off                
              analogWrite(PIN_LED2,0);
              // play turn off sound
              switchState=false;                
              voice1.play(AudioSampleDisabled);
              //delay(1500);getTestAOA
              Serial.println("Switch off");
              }
     }

void switchCheck()
{
Switch.tick();
}

void SwitchSingleClick()
{
switchOnOff();
}

void SwitchLongPress()
{
Serial.println("Data Mark");
dataMark++;
voice1.play(AudioSampleDatamark);
}
