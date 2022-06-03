void checkSerial()
{
 // check for Serial input lockups
      #ifdef EFISDATADEBUG
      Serial.printf("\nloopcount: %i",loopcount);
      Serial.printf("\nchars received: %i",charsreceived);     
      Serial.printf("\nmax available efis characters: %i\n",efisMaxAvailable);
      efisMaxAvailable=0;
      #endif
            
      if (readEfisData)
          {
          //check for efis stream, restart port if not receiving data for half second
          if (millis()-lastReceivedEfisTime>=EFIS_DATA_TIMEOUT)
             {             
             Serial3.end();             
             Serial3.begin(BAUDRATE_EFIS);
             lastReceivedEfisTime=millis();// reset timer 
            
             #ifdef EFISDATADEBUG
            Serial.println("\n Efis data timeout. Restarting serial port 3");
             Serial.printf("\nEfis last transmit: %i", millis()-lastReceivedEfisTime);
             #endif
             }
          }  
     
      if (readBoom)
         {
          //check for efis stream, restart port of not receiving data for half second
          if (millis()-lastReceivedBoomTime>=BOOM_DATA_TIMEOUT)
             {
             Serial1.end();             
             Serial1.begin(BAUDRATE_BOOM);
             lastReceivedBoomTime=millis(); 
             
             #ifdef BOOMDATADEBUG
              Serial.println("\n Boom data timeout. Restarting serial port 1");            
             Serial.printf("\nBoom last transmit: %i", millis()-lastReceivedBoomTime);
             #endif
             }
         }            
      loopcount=0;
      charsreceived=0;     
}
