#include <Wire.h>
#include <i2cdetect.h>

const uint8_t first = 0x03;
const uint8_t last = 0x77;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("i2cdetect example\n");
  Serial.printf("Scanning address range %02x-%02x\n\n", first, last);
}

void loop() {
  i2cdetect(first, last);
  delay(2000);
}
