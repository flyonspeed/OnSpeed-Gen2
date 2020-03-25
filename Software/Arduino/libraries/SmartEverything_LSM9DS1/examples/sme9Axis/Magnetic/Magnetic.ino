/*
    smeMagnetic Library - Localization Information

    This example print the 3Axis X,Y,Z information

    created 27 May 2015
    by Seve (seve@axelelettronica.it)

    This example is in the public domain
    https://github.com/ameltech

    LSM9DS1  more information available here:
    http://www.stmicroelectronics.com.cn/web/catalog/sense_power/FM89/SC1448/SC1448/PF259998
 */

#include <Arduino.h>
#include <Wire.h>

#include <LSM9DS1.h>


// the setup function runs once when you press reset or power the board
void setup() {
    Wire.begin();
    smeMagnetic.begin();
    SerialUSB.begin(115200);
}

void printAxis(int x, int y, int z) {
    SerialUSB.print("  X = ");
    SerialUSB.print(x, DEC);
    SerialUSB.print("     Y = ");
    SerialUSB.print(y, DEC);
    SerialUSB.print("     Z = ");
    SerialUSB.println(z, DEC);
}

// the loop function runs over and over again forever
void loop() {

    int x = 0;
    int y = 0;
    int z = 0;

    x = smeMagnetic.readX();
    y = smeMagnetic.readY();
    z = smeMagnetic.readZ();

    SerialUSB.print("Magnetometer  [mgauss]  :");
    printAxis(x, y, z);

    ledBlueLight(LOW);
    delay(100);

    ledBlueLight(HIGH);    // turn the LED on
    delay(500);            // wait for a second

}
