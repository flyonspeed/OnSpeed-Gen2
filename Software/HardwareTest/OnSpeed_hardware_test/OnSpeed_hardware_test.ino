
// Onspeed Hardware test v1.0
// FlyOnspeed.org
//﻿DB15 pinout (Gen2 v3 hardware)
//1 - 14V +PWR
//2 - EFIS Serial RX [Serial3] (connect to pin 12 for test)
//3 - PANEL SWITCH
//4 - GPS Serial RX [Serial2] (connect to pin 12 for test)
//5 - LED+ Digital/PWM
//6 - AUDIO RIGHT
//7 - FLAPS Analog IN
//8 - AUDIO LEFT
//9 - OAT Analog IN/ Display TTL out [Serial5] (connect to pin 13 for test)
//10 - VOLUME Analog IN
//11 - SENSOR PWR 3.3V
//12 - EFIS Serial TX [Serial3]
//13 - BOOM TTL RX [Serial1]
//14 - GND
//15 - AUDIO GND 

#include <i2c_t3.h> // multiport i2c (Wire.h needs to redirect here, otherwise it gets duplicated. Make a Wire.h library with an #include <i2c_t3.h> line in it.
#include <Audio.h>
#include <SD.h> 
#include "AudioSampleOnspeed_left_speaker.h"
#include "AudioSampleOnspeed_right_speaker.h"

// IMU defines
#define I2C_COMMUNICATION_TIMEOUT 1000  // microseconds
#define LSM9DS1_AccelGyro 0x6B
#define LSM9DS1_Magnet    0x1E
// LSM9DS1 Accel/Gyro (XL/G) Registers
#define CTRL_REG5_XL      0x1F
#define CTRL_REG6_XL      0x20
#define CTRL_REG7_XL      0x21
#define OUT_X_L_XL        0x28
#define CTRL_REG4         0x1E
#define CTRL_REG1_G       0x10
#define OUT_X_L_G         0x18
// IMU variables
uint8_t _i2cAddress_AccelGyro=LSM9DS1_AccelGyro;
volatile int16_t ax, ay, az; // ax -instantaneous
volatile int16_t gx, gy, gz; // gx - instantaneous
float aRes=0.000244140625; // 8g / 32768.0
float gRes=500.0 / 32768.0; // full scale / resolution
uint8_t Ascale = 3;     // accel full scale, 8G
uint8_t Aodr = 6;   // accel data sample rate,  AODR_952Hz
uint8_t Abw = 3;      // accel data bandwidth,  ABW_50Hz
uint8_t Gscale = 1;   // 500 degree/sec
uint8_t Godr = 6;     // GODR_952Hz
uint8_t Gbw = 2;    // 58 Hz at Godr = 952 Hz

#define VOLUME_PIN            A20  // pin 39 used for audio volume, pin 10 on DB15
#define PIN_LED2              5    // external LED for showing AOA status
#define FLAP_PIN              A2     // flap position switch  (pin 7 on DB15)
#define OAT_PIN               A14    // OAT pin
#define SWITCH_PIN            2


// audio setup
AudioPlayMemory          voice1;
AudioMixer4              mixer1;
AudioOutputAnalogStereo  dacs;
AudioAmplifier           ampLeft;
AudioAmplifier           ampRight;
AudioConnection          patchCord3(voice1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, ampLeft);
AudioConnection          patchCord5(mixer1, ampRight);
AudioConnection          patchCord6(ampLeft, 0, dacs, 0);
AudioConnection          patchCord7(ampRight, 0, dacs, 1);

// SD card
File sdFile;

void setup()
{
delay(1000);
pinMode(FLAP_PIN, INPUT_PULLUP);
pinMode(VOLUME_PIN, INPUT_PULLUP);
pinMode(PIN_LED2, OUTPUT);
pinMode(SWITCH_PIN, INPUT_PULLUP);
Serial.begin(115200); // init console serial
delay(500);

// set up i2c ports
Wire.begin (I2C_MASTER,0x00,I2C_PINS_18_19,I2C_PULLUP_EXT,400000); // SDA0/SCL0 I2C at 400kHz, PS1 sensor, Pfwd   
Wire.setOpMode(I2C_OP_MODE_IMM);
Wire.setDefaultTimeout(2000);

Wire2.begin(I2C_MASTER,0x00,I2C_PINS_3_4,I2C_PULLUP_EXT,400000); // SDA1/SCL1 I2C at 400kHz, PS2 sensor, P45
Wire2.setOpMode(I2C_OP_MODE_IMM);
Wire2.setDefaultTimeout(2000);

Wire1.begin();
Wire1.setDefaultTimeout(2000);
initAccelGyro(); //initialize accelerometer & Gyro (IMU)
// set volume
AudioMemory(16);
mixer1.gain(2,5); // channel 2 gain (voice)
}

void loop() {
Serial.println("\nOnSpeed Hardware Test\n");
hardwareTest();
delay(5000);
}

void hardwareTest()
{
analogWrite(PIN_LED2,150); // turn on LED
switchTest();
pressureTest();
imuTest();
serialTest();
potTest();
audioTest();
sdCardTest();
analogWrite(PIN_LED2,0); // turn off LED
}

void switchTest()
{
if (digitalRead(SWITCH_PIN)==HIGH) Serial.println("Switch: Not pressed"); else Serial.println("Switch: Pressed");
}

void pressureTest()
{
if (GetPressurePfwd()!=-1) Serial.println("PFWD: PASS"); else Serial.println("PFWD: FAIL");
if (GetPressureP45()!=-1) Serial.println("P45: PASS"); else Serial.println("P45: FAIL");
if (GetStaticPressure()!=-1) Serial.println("PStatic: PASS"); else Serial.println("PStatic: FAIL");
}

void imuTest()
{
if (readAccelGyro()) Serial.println("IMU: PASS"); else Serial.println("IMU: FAIL");
}

void serialTest()
{
String serialString;
Serial3.begin(115200); // efis
Serial2.begin(115200); // gps
Serial3.print("TEST");
unsigned long serialStartTime=millis();
while (true)
  {
  if (Serial2.available()>0)
    {
    serialString+=Serial2.read();    
    if (serialString.indexOf("TEST"))
        {
        Serial.println("Efis Serial TX -> GPS Serial RX: PASS");  
        break;
        }
    }
  if (millis() - serialStartTime>1000)
      {
      Serial.println("Efis Serial TX -> GPS Serial RX: FAILED");  
      break;
      }
  }      
serialString="";      
while (true)
  {
  if (Serial3.available()>0)
    {
    serialString+=Serial3.read();    
    if (serialString.indexOf("TEST"))
        {
       Serial.println("Efis Serial TX -> Efis Serial RX: PASS");
       break;  
        }
    }
  if (millis() - serialStartTime>1000)
      {
      Serial.println("Efis Serial TX -> Efis Serial RX: FAILED");  
      break;
      }
  }    
Serial2.end();
Serial3.end();
// test Serial 1 and 5
Serial5.begin(115200); // display output
Serial1.begin(115200);  // boom
Serial5.print("TTLTEST");
serialStartTime=millis();
while (true)
  {
  if (Serial1.available()>0)
    {
    serialString+=Serial1.read();    
    if (serialString.indexOf("TTLTEST"))
        {
       Serial.println("Display Serial TX -> BOOM Serial RX: PASS");
       break;  
        }
    }
  if (millis() - serialStartTime>1000)
      {
      Serial.println("Display Serial TX -> BOOM Serial RX: FAILED");  
      break;
      }
  }
Serial1.end();
Serial5.end();   
}  

void potTest()
{
// read volume and flap pots
Serial.printf("Volume pot analog value: %i\n",analogRead(VOLUME_PIN));
Serial.printf("Flap pot analog value: %i\n",analogRead(FLAP_PIN));  
}

void audioTest()
{
 Serial.println("Testing Left audio channel");
 voice1.play(AudioSampleOnspeed_left_speaker); 
 delay (2500);
 Serial.println("Testing Right audio channel");
 voice1.play(AudioSampleOnspeed_right_speaker);
 delay (2500);
}

void sdCardTest()
{
#ifdef SdFat_h 
SdFat SD;  
#endif
if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("SD Card: FAIL");
    return;
  }
 sdFile = SD.open("/"); 
 if (sdFile)
      {
      Serial.println("SD Card: PASS");  
      sdFile.close();      
      }
      else
          Serial.println("SD Card: FAIL");  
}

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
while ((Wire2.available() < 2) && (micros()-waitStart<timeout)){}
      
  if (micros()-waitStart >=timeout)
          {
          return -1;
          }
    reading = Wire2.read(); // byte 1
    reading = reading & mask;
    reading = reading << 8; //
    reading |= Wire2.read(); // read byte 2 //Serial.print(reading);
    //Serial.print(",");
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
while ((Wire.available() < 2) && (micros()-waitStart<timeout)){}
        
if (micros()-waitStart >=timeout)
          {           
          return -1;
          }
    reading = Wire.read(); // byte 1
    reading = reading & mask;
    reading = reading << 8; //
    reading |= Wire.read(); // read byte 2 //Serial.print(reading);
    return reading;
}

float GetStaticPressure()
{
// static pressure in mbars
uint8_t address = 120;
uint32_t result;
uint8_t v1, v2;
float millibars;
float rawPressure;
unsigned long timeout = I2C_COMMUNICATION_TIMEOUT;
Wire1.requestFrom(address, (uint8_t) 4,I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
unsigned long waitStart=micros();
while ((Wire1.available() < 4) && (micros()-waitStart<timeout)){}
            
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
}

bool readAccelGyro()
{
  uint8_t temp_a[6]; // We'll read six bytes from the accelerometer into temp 
  if (I2CreadBytes(_i2cAddress_AccelGyro,OUT_X_L_XL, temp_a, 6)<0) return false; // Read 6 bytes, beginning at OUT_X_L_XL  
  ax = (temp_a[1] << 8) | temp_a[0]; // Store x-axis values into ax
  ay = (temp_a[3] << 8) | temp_a[2]; // Store y-axis values into ay
  az = (temp_a[5] << 8) | temp_a[4]; // Store z-axis values into az  
  //Serial.printf("ax:%.2f,ay:%.2f,az:%.2f\n",ax*aRes,ay*aRes,az*aRes);
  uint8_t temp_g[6];  // x/y/z gyro register data stored here
  if (I2CreadBytes(_i2cAddress_AccelGyro, OUT_X_L_G, temp_g,6)<0) return false;  // Read the six raw data registers sequentially into data array
  gx = (temp_g[1] << 8) | temp_g[0]; // Store x-axis values into gx
  gy = (temp_g[3] << 8) | temp_g[2]; // Store y-axis values into gy
  gz = (temp_g[5] << 8) | temp_g[4]; // Store z-axis values into gz  
  //Serial.printf("gx:%.2f,gy:%.2f,gz:%.2f\n",gx*gRes,gy*gRes,gz*gRes);
  return true;
  
}

int I2CreadBytes(uint8_t i2cAddress, uint8_t registerAddress, uint8_t * dest, uint8_t count)
{  
  unsigned long  timeout = I2C_COMMUNICATION_TIMEOUT;
  // Initialize the Tx buffer
  Wire1.beginTransmission(i2cAddress);     
  // Next send the register to be read. OR with 0x80 to indicate multi-read.
  // Put slave register address in Tx buffer
  Wire1.write(registerAddress | 0x80);     
  //If false, endTransmission() sends a restart message after transmission. The bus will not be released, 
  //which prevents another master device from transmitting between messages. This allows one master device 
  //to send multiple transmissions while in control. The default value is true.
  Wire1.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);             
  // Read bytes from slave register address
  Wire1.requestFrom(i2cAddress, count,I2C_STOP,I2C_COMMUNICATION_TIMEOUT);   

  unsigned long waitStart=micros();
  while ((Wire1.available() < count) && (micros()-waitStart<timeout))
         {
         }  
 if (micros()-waitStart >=timeout)
          {
          return -1;
          } 
  for (int i=0; i<count;)
  {    
      dest[i++] = Wire1.read();   
  }
  return count;
}

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
 
}

void I2CwriteByte(uint8_t i2cAddress, uint8_t registerAddress, uint8_t data)
{
  // Initialize the Tx buffer
  Wire1.beginTransmission(i2cAddress);
  // Put slave register address in Tx buffer
  Wire1.write(registerAddress);          
  // Put data in Tx buffer
  Wire1.write(data);                 
  // Send the Tx buffer
  Wire1.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);
     
}
