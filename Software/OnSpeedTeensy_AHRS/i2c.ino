uint8_t I2CreadByte(uint8_t i2cAddress, uint8_t registerAddress)
{ 
  unsigned long timeout = I2C_COMMUNICATION_TIMEOUT;
  uint8_t data; // `data` will store the register data  
  // Initialize the Tx buffer
  Wire1.beginTransmission(i2cAddress);

  // Put slave register address in Tx buffer
  Wire1.write(registerAddress);                
  Wire1.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);            
  // Read one byte from slave register address 
  Wire1.requestFrom(i2cAddress, (uint8_t) 1);  

  unsigned long waitStart=micros();
  while ((Wire1.available() < 1) && (micros()-waitStart<timeout))
    {
    checkWatchdog();  
    }
  
  if (micros()-waitStart >=timeout)
          {
          return -1;
          }
  // Fill Rx buffer with result
  data = Wire1.read();                      
  // Return data read from slave register
  return data;                             
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

uint8_t I2CreadBytes(uint8_t i2cAddress, uint8_t registerAddress, uint8_t * dest, uint8_t count)
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
     checkWatchdog();
    }
  
  if (micros()-waitStart >=timeout)
          {
          Serial.println("IMU i2c timeout");
          //I2CwriteByte(i2cAddress,0x22,  0x01); //CTRL_REG8 on LSM9DS1
          checkWatchdog();
          delay(10);
          return -1;
          } 
  for (int i=0; i<count;)
  {    
      dest[i++] = Wire1.read();
  }
  return count;
}

uint8_t I2CreadBytesISM330(uint8_t i2cAddress, uint8_t registerAddress, uint8_t * dest, uint8_t count)
{  
  unsigned long  timeout = I2C_COMMUNICATION_TIMEOUT;
  // Initialize the Tx buffer
  Wire1.beginTransmission(i2cAddress);     
  // Put slave register address in Tx buffer
  Wire1.write(registerAddress);     
  //If false, endTransmission() sends a restart message after transmission. The bus will not be released, 
  //which prevents another master device from transmitting between messages. This allows one master device 
  //to send multiple transmissions while in control. The default value is true.
  Wire1.endTransmission(I2C_STOP,I2C_COMMUNICATION_TIMEOUT);             
  // Read bytes from slave register address
  Wire1.requestFrom(i2cAddress, count,I2C_STOP,I2C_COMMUNICATION_TIMEOUT);   

  unsigned long waitStart=micros();
  while ((Wire1.available() < count) && (micros()-waitStart<timeout))
    {
     checkWatchdog();
    }
  
  if (micros()-waitStart >=timeout)
          {
          Serial.println("IMU i2c timeout");
          checkWatchdog();
          delay(10);
          return -1;
          } 
  for (int i=0; i<count;)
  {    
      dest[i++] = Wire1.read();
  }
  return count;
}


void initI2C()
{
  // set up i2c ports
  Wire.begin (I2C_MASTER,0x00,I2C_PINS_18_19,I2C_PULLUP_EXT,3000000); // SDA0/SCL0 I2C at 3Mhz, PS1 sensor, Pfwd   
  Wire.setOpMode(I2C_OP_MODE_ISR);
  Wire.setDefaultTimeout(2000);

  Wire2.begin(I2C_MASTER,0x00,I2C_PINS_3_4,I2C_PULLUP_EXT,3000000); // SDA2/SCL2 I2C at 3Mhz, PS2 sensor, P45
  Wire2.setOpMode(I2C_OP_MODE_ISR);
  Wire2.setDefaultTimeout(2000);
  
  Wire1.begin(I2C_MASTER,0x00,I2C_PINS_37_38,I2C_PULLUP_EXT,400000); // SDA1/SCL1 at 400Khz, IMU and baro sensor
  Wire1.setOpMode(I2C_OP_MODE_ISR);
  Wire1.setDefaultTimeout(2000);
  //Serial.printf("I2C clock Speed: %i",Wire1.getClock());  
}
