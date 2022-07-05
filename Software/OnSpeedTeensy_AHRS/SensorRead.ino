int GetPressureP45()
{
 // read sensor P1
int pressureAddress = 40; //1001000 written as decimal number
int mask = 63; //(0 0 1 1 1 1 1 1 )
//int maskstatus = 192; //(1 1 0 0 0 0 0 0 )
int reading; 
unsigned long timeout = I2C_COMMUNICATION_TIMEOUT; 
//Send a request
//Start talking to the device at the specified address
Wire2.beginTransmission(pressureAddress);
//Send a bit asking for register zero, the data register
Wire2.write(0);
//Complete Transmission
Wire2.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
//Request 2 Bytes from the specified address
Wire2.requestFrom(pressureAddress, 2,I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
//Serial.print(micros()-start); Serial.print(",");
unsigned long waitStart=micros();
while ((Wire2.available() < 2) && (micros()-waitStart<timeout)) {}    
if (micros()-waitStart >=timeout)
          {
          return -1;
          }
    reading = Wire2.read(); // byte 1
    reading = reading & mask;
    reading = reading << 8; //
    reading |= Wire2.read(); // read byte 2 //Serial.print(reading);
    return reading;
return -1;    
}
int GetPressurePfwd()
{
// read sensor P2 
int pressureAddress = 40; //1001000 written as decimal number
int mask = 63; //(0 0 1 1 1 1 1 1 )
//int maskstatus = 192; //(1 1 0 0 0 0 0 0 ) 
int reading;
unsigned long timeout = I2C_COMMUNICATION_TIMEOUT;
//Send a request
//Start talking to the device at the specified address
Wire.beginTransmission(pressureAddress);
//Send a bit asking for register zero, the data register
Wire.write(0);
Wire.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
//Request 2 Bytes from the specified address
Wire.requestFrom(pressureAddress, 2,I2C_STOP,I2C_COMMUNICATION_TIMEOUT);

unsigned long waitStart=micros();
while ((Wire.available() < 2) && (micros()-waitStart<timeout)) {}
        
  if (micros()-waitStart >=timeout)
          {           
          return -1;
          }
    reading = Wire.read(); // byte 1
    reading = reading & mask;
    reading = reading << 8; //
    reading |= Wire.read(); // read byte 2 //Serial.print(reading);   
    return reading;
return -1;
}

float GetStaticPressure()
{
// static pressure in mbars
uint8_t address = 120;
uint32_t result;
uint8_t v1, v2;
float millibars;
int rawPressure;
unsigned long timeout = I2C_COMMUNICATION_TIMEOUT;
Wire1.requestFrom(address, (uint8_t) 4,I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
unsigned long waitStart=micros();
while ((Wire1.available() < 4) && (micros()-waitStart<timeout)) {}
            
  if (micros()-waitStart >=timeout)
    {
    return -1;
    }
   v1 = Wire1.read();
   v2 = Wire1.read();
   result = (((uint32_t) (v1 & 0x3f)) << 24) | ((uint32_t) v2 << 16);
   v1 = Wire1.read();
   v2 = Wire1.read();
   result |= ((((uint16_t) v1) << 8) | v2) >> 5;
   Wire1.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
  //millibars=float(result >> 16) * 1.6 / 16383*1000;

  rawPressure=result>>16;
  millibars=(rawPressure-1638.4)/8192*1000; // transfer function 80/10, Page 11 of datasheet for SSCSRNN1.6BA7A3 sensor.
  return millibars; // results in millibars
 return -1;    
}

void SensorRead()
{
if (readingSensors) 
      {
      return;
      }
readingSensors=true;   
// reads sensors 50 times/second
Pfwd=GetPressurePfwd()-pFwdBias;
P45=GetPressureP45()-p45Bias;
Pstatic=GetStaticPressure();
Palt=145366.45*(1-pow((Pstatic+pStaticBias)/1013.25,0.190284)); //Pstatic in milliBars,Palt in feet

flapsIndex=getFlapsIndex();
flapsPos=flapDegrees.Items[flapsIndex];
setAOApoints(flapsIndex);

// median filter pressure then a simple moving average
PfwdMedian.add(Pfwd);
PfwdAvg.addValue(PfwdMedian.getMedian());
PfwdSmoothed=PfwdAvg.getFastAverage();

P45Median.add(P45);
P45Avg.addValue(P45Median.getMedian());
P45Smoothed=P45Avg.getFastAverage();

calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd/P45;

// calculate airspeed
  PfwdPascal=((PfwdSmoothed+pFwdBias - 0.1*16383) * 2/(0.8*16383) -1) * 6894.757;
  if (PfwdPascal>0)
      {
      IAS=sqrt(2*PfwdPascal/1.225)* 1.94384; // knots // physics based calculation
      #ifdef SPHERICAL_PROBE
        IAS=IASCURVE(IAS); // for now use a hardcoded IAS curve for a spherical probe. CAS curve parameters can only take 4 decimals. Not accurate enough.
      #else  
      if (casCurveEnabled) IAS=curveCalc(IAS,casCurve);  // use CAS correction curve if enabled 
      #endif
      
      }
      else IAS=0;

#ifdef LOGDATA_PRESSURE_RATE
    logData();
#endif
updateTones();


#ifdef SENSORDEBUG
    char debugSensorBuffer[500];
    int lineLength=sprintf(debugSensorBuffer, "timeStamp: %lu,Pfwd: %i,PfwdSmoothed: %.2f,P45: %i,P45Smoothed: %.2f,Pstatic: %.2f,Palt: %.2f,IAS: %.2f,AOA: %.2f,flapsPos: %i,VerticalG: %.2f,LateralG: %.2f,ForwardG: %.2f,RollRate: %.2f,PitchRate: %.2f,YawRate: %.2f, AccelPitch %.2f",millis(),Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,IAS,AOA,flapsPos,aVert,aLat,aFwd,gRoll,gPitch,gYaw,accPitch);

    if (readBoom)
      {
      sprintf(debugSensorBuffer+lineLength, ",boomStatic: %.2f,boomDynamic: %.2f,boomAlpha: %.2f,boomBeta: %.2f\n",boomStatic,boomDynamic,boomAlpha,boomBeta);
      }  
    Serial.println(debugSensorBuffer);
#endif    

  
//Serial.println(sensorCacheCount);

      
readingSensors=false;
//Serial.println(micros()-sensorstarttime);    
}
