void timersOff()
{
//turn off Timers;
Serial.println("Timers OFF");
IMUTimer.end();
SensorTimer.end();  
ToneTimer.end();
timersDisabled=true;
// wait for timers to finish running
unsigned long delayTimer=millis();
while (millis()-delayTimer<250)
      {
      checkWatchdog();
      }
}

void timersOn()
{
Serial.println("Timers ON");
// wait before starting timers
unsigned long delayTimer=millis();
while (millis()-delayTimer<250)
      {
      checkWatchdog();
      }
IMUTimer.begin(ReadIMU,IMU_INTERVAL);
SensorTimer.begin(SensorRead,SENSOR_INTERVAL);   
ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on
timersDisabled=false;
}
