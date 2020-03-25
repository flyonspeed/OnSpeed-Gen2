/*
i2cdetect.cpp - Arduino library for scanning I2C bus for devices
*/

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Wire.h"
#include "i2cdetect.h"

void i2cdetect(uint8_t first, uint8_t last) {
  uint8_t i, address, error;
  char buff[10];

  // table header
  Serial.print("   ");
  for (i = 0; i < 16; i++) {
    //Serial.printf("%3x", i);
    sprintf(buff, "%3x", i);
    Serial.print(buff);
  }

  // table body
  // addresses 0x00 through 0x77
  for (address = 0; address <= 119; address++) {
    if (address % 16 == 0) {
      //Serial.printf("\n%#02x:", address & 0xF0);
      sprintf(buff, "\n%02x:", address & 0xF0);
      Serial.print(buff);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {
        // device found
        //Serial.printf(" %02x", address);
        sprintf(buff, " %02x", address);
        Serial.print(buff);
      } else if (error == 4) {
        // other error
        Serial.print(" XX");
      } else {
        // error = 2: received NACK on transmit of address
        // error = 3: received NACK on transmit of data
        Serial.print(" --");
      }
    } else {
      // address not scanned
      Serial.print("   ");
    }
  }
  Serial.println("\n");
}

void i2cdetect() {
  i2cdetect(0x03, 0x77);  // default range
}

