# i2cdetect

Arduino library for scanning I2C bus for devices.

Outputs a table similar to the [linux program](http://linux.die.net/man/8/i2cdetect) of the same name.

Each cell will contain one of the following symbols:

* "--" when the address was probed but no chip answered
* An address in hexadecimal, e.g. "1e", when a chip was found at the address

The optional parameters first and last restrict the scanning range (default: from 0x03 to 0x77).

## Example

```
#include <Wire.h>
#include <i2cdetect.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("i2cdetect example\n");
  Serial.print("Scanning address range 0x03-0x77\n\n");
}

void loop() {
  i2cdetect();  // default range from 0x03 to 0x77
  delay(2000);
}
```

## Serial monitor output

```
i2cdetect example
Scanning address range 0x03-0x77

     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- 1e --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

## Tested on devices

* [Arduino UNO R3 - ATmega328P](https://www.arduino.cc/en/Main/ArduinoBoardUno)
* [WeMos D1 mini - ESP8266](http://www.wemos.cc/wiki/doku.php?id=en:d1_mini)
