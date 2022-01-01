// IMU functions
void initAccelGyro()
{  
 // enable the three axes of the accelerometer
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG5_XL,  0x38);
 // configure the accelerometer-specify bandwidth selection with Abw
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG6_XL, Aodr << 5 | Ascale << 3 | 0x04 | Abw);
 
 // enable the three axes of the gyroscope
 I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG4, 0x38);
 // configure the gyroscope
 I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG1_G, Godr << 5 | Gscale << 3 | Gbw);

delay(50);
// set FIFO mode
I2CwriteByte(_i2cAddress_AccelGyro, FIFO_CTRL, 0xC0); // write 11000000 to FIFO_CTRL to set FIFO to Continous mode.
//I2CwriteByte(_i2cAddress_AccelGyro, FIFO_CTRL, 0x00); // bypass mode, no FIFO
delay(50);
 // turn on FIFO
I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG9, 0x02); // write 00000010 to CTRL_REG9 to turn on FIFO, unlimited FIFO depth (32 values max)

delay(100);
 
}

void resetAccelGyro()
  {
  // soft reset accelerometer/gyro
  I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG8,  0x01);
  delay(100); 
  }

bool newAccelGyroAvailable()
    {
    uint8_t fifoSRCbyte;
    fifoSRCbyte=I2CreadByte(_i2cAddress_AccelGyro, FIFO_SRC) & 0x3F; // 00111111 - number of bytes available
    if (fifoSRCbyte >0) return true; else return false;
    }

void readAccelGyro(bool tempUpdate=true)
{
 int16_t axRaw,ayRaw,azRaw,gxRaw,gyRaw,gzRaw; 
  uint8_t temp_a[6]; // We'll read six bytes from the accelerometer into temp 
  I2CreadBytes(_i2cAddress_AccelGyro,OUT_X_L_XL, temp_a, 6); // Read 6 bytes, beginning at OUT_X_L_XL  
  axRaw = (temp_a[1] << 8) | temp_a[0]; // Store x-axis values into ax
  ayRaw = (temp_a[3] << 8) | temp_a[2]; // Store y-axis values into ay
  azRaw = (temp_a[5] << 8) | temp_a[4]; // Store z-axis values into az 
     
  uint8_t temp_g[6];  // x/y/z gyro register data stored here
  I2CreadBytes(_i2cAddress_AccelGyro, OUT_X_L_G, temp_g,6);  // Read the six raw data registers sequentially into data array
  
  gxRaw = (temp_g[1] << 8) | temp_g[0]; // Store x-axis values into gx
  gyRaw = (temp_g[3] << 8) | temp_g[2]; // Store y-axis values into gy
  gzRaw = (temp_g[5] << 8) | temp_g[4]; // Store z-axis values into gz 

if (tempUpdate)
    {
    uint8_t temp_t[2]; // temeperature register data
    //I2CreadBytes(_i2cAddress_AccelGyro, OUT_TEMP_L, temp_t,2);  // Read the 2 raw data registers sequentially into data array
    temp_t[0]=I2CreadByte(_i2cAddress_AccelGyro,OUT_TEMP_L);
    temp_t[1]=I2CreadByte(_i2cAddress_AccelGyro,OUT_TEMP_H);
    int16_t tempCount=(temp_t[1] << 8) | temp_t[0]; 
    float imuTempRaw = ((float) (tempCount / LSM9DS1_TEMP_SCALE + LSM9DS1_TEMP_BIAS)); // IMU chip temperature in degrees Celsius   
    imuTempAvg.addValue(imuTempRaw);
    imuTemp=imuTempAvg.getFastAverage();
    //imuTempDerivativeInput=imuTempRaw;
    //imuTempRateAvg.addValue(-imuTempDerivative.Compute()*10.0); //10Hz sample rate on imuTemp, SavGolay derivative filter takes 20-25uSec
    //imuTempRate=imuTempRateAvg.getFastAverage(); 
    #ifdef IMUTEMPDEBUG
    Serial.printf("Temp: %.1f\n",imuTemp);
    #endif    
    }
  // IMU temp compensation, derived from heating the box from -25C to 50C and then cooling it again to 27C
  ax=scaleAccel(axRaw) - 2.996e-07 * imuTemp * imuTemp -2.899e-05 * imuTemp  -0.01741;
  ay=scaleAccel(ayRaw) - 8.759e-07 * imuTemp * imuTemp + 0.0007991 * imuTemp -0.03251;
  az=scaleAccel(azRaw) -3.799e-06 * imuTemp * imuTemp -0.0001109 * imuTemp + 0.001731;
  gx=scaleGyro(gxRaw) + 0.00029 * imuTemp * imuTemp + 0.08007 * imuTemp -2.261;
  gy=scaleGyro(gyRaw) + 0.0004215 * imuTemp * imuTemp + 0.004307 * imuTemp -0.3264;
  gz=scaleGyro(gzRaw) + 0.0004122 * imuTemp * imuTemp + -0.01383 * imuTemp + 0.04016;
}



float scaleAccel(int16_t accel)
{
  // Return the accel raw reading times our pre-calculated g's / (ADC tick):
  return aRes * accel;
}

float scaleGyro(int16_t gyro)
{
  // Return the gyro raw reading times our pre-calculated degrees / (ADC tick):
  return gRes * gyro;
}

void configureAxes()
{
// get accelerometer axis from box orientation (there must be a better way to do this, but it works for now)
// orientation arrays defined as box orinetation
// configure axes to North East Down (NED) Axis convention
//vertical axis (z down), lateral axis (y right), longitudinal axis (x forward)
// pitchate positive for pitch up
// roll rate positive for right roll
// yaw rate positive for right yaw

verticalGloadAxis="";
lateralGloadAxis="";
forwardGloadAxis="";
String axisMapArray[24][5]={
                  //{portsOrientation,boxtopOrientation,verticalGloadAxis,lateralGloadAxis,forwardGloadAxis}
                  {"FORWARD","LEFT","Y","Z","X"},
                  {"FORWARD","RIGHT","-Y","Z","X"},
                  {"FORWARD","UP","Z","Y","X"}, // Vac's RV-4
                  {"FORWARD","DOWN","-Z","-Y","X"},
                  
                  {"AFT","LEFT","-Y","-Z","-X"},
                  {"AFT","RIGHT","Y","Z","-X"},
                  {"AFT","UP","Z","-Y","-X"}, // bench box
                  {"AFT","DOWN","-Z","Y","-X"},
                  
                  {"LEFT","FORWARD","-Y","-X","Z"},
                  {"LEFT","AFT","Y","-X","-Z"},
                  {"LEFT","UP","Z","-X","Y"}, // Zlin Z-50
                  {"LEFT","DOWN","-Z","-X","Y"},
                  
                  {"RIGHT","FORWARD","Y","X","Z"},
                  {"RIGHT","AFT","-Y","X","-Z"},
                  {"RIGHT","UP","Z","X","-Y"},                  
                  {"RIGHT","DOWN","-Z","X","Y"}, // Tron's RV-8
                  
                  {"UP","FORWARD","X","-Y","Z"},
                  {"UP","AFT","X","Y","-Z"},
                  {"UP","LEFT","X","-Z","-Y"},
                  {"UP","RIGHT","X","Z","Y"}, // Doc's box on Vac's RV-4
                  
                  {"DOWN","FORWARD","-X","Y","Z"},
                  {"DOWN","AFT","-X","-Y","-Z"}, // Lenny's RV-10
                  {"DOWN","LEFT","-X","-Z","Y"},
                  {"DOWN","RIGHT","-X","Z","-Y"}
                  };

for (int i=0;i<24;i++)
    {    
    if (axisMapArray[i][0]==portsOrientation && axisMapArray[i][1]==boxtopOrientation)
        {
        verticalGloadAxis=axisMapArray[i][2];
        lateralGloadAxis=axisMapArray[i][3];
        forwardGloadAxis=axisMapArray[i][4];
        yawGyroAxis=verticalGloadAxis;
        pitchGyroAxis=lateralGloadAxis;
        rollGyroAxis=forwardGloadAxis;
        break; 
        }
    }
#ifdef AXISDEBUG
Serial.printf("portsOrientation: %s\n",portsOrientation.c_str());
Serial.printf("boxtopOrientation: %s\n",boxtopOrientation.c_str());
Serial.printf("Vertical axis: %s\n",verticalGloadAxis.c_str());
Serial.printf("Lateral axis: %s\n",lateralGloadAxis.c_str());
Serial.printf("Forward axis: %s\n",forwardGloadAxis.c_str());                   
Serial.printf("Yaw Gyro axis: %s\n",yawGyroAxis.c_str());
Serial.printf("Pitch Gyro axis: %s\n",pitchGyroAxis.c_str());
Serial.printf("Roll Gyro axis: %s\n",rollGyroAxis.c_str());                   

#endif
}

float getAccelForAxis(String accelAxis)
{
float result=0.0;
if (accelAxis[accelAxis.length()-1] == 'X') result=ax;          
    else
        if (accelAxis[accelAxis.length()-1] == 'Y') result=ay;
            else result=az;
if (accelAxis[0]=='-') result*=-1;
return result;      
}

float getGyroForAxis(String gyroAxis)
{
float result=0.0;
if (gyroAxis[gyroAxis.length()-1] == 'X') result=gx+gxBias;
    else
        if (gyroAxis[gyroAxis.length()-1] == 'Y') result=gy+gyBias;
            else result=gz+gzBias;
if (gyroAxis[0]!='-') result*=-1;
return result;      
}

float getGyroForAxisnoBias(String gyroAxis)
{
float result=0.0;
if (gyroAxis[gyroAxis.length()-1] == 'X') result=gx;          
    else
        if (gyroAxis[gyroAxis.length()-1] == 'Y') result=gy;
            else result=gz;
if (gyroAxis[0]!='-') result*=-1;
return result;
}

void ReadIMU()
{
// read IMU and process AHRS
//check if FIFO is available and then read data
bool tempUpdate=false;
// heartbeat
if (millis()-lastImuTempUpdate>100)
      {
      lastImuTempUpdate=millis(); 
      tempUpdate=true;
      }
readAccelGyro(tempUpdate); // read accelerometers 

//get IMU values in aircraft orientation
Az=getAccelForAxis(verticalGloadAxis);//vertical G (in g)
Ay=getAccelForAxis(lateralGloadAxis);// lateral G
Ax=getAccelForAxis(forwardGloadAxis);// forward G
Gx=getGyroForAxis(rollGyroAxis); // roll rate (in deg/sec)
Gy=getGyroForAxis(pitchGyroAxis); // pitch rate
Gz=getGyroForAxis(yawGyroAxis); // yaw rate

//Serial.printf("%.3f,%.3f,%.3f,%.2f,%.1f\n",Gx,Gy,Gz,imuTemp,imuTempRate);

#ifdef LOGDATA_IMU_RATE
logData();
#endif
processAHRS(); // calculate AHRS
    
}  