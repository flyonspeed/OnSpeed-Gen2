# SmartEverything ST LSM9DS1  3D accelerometer, 3D gyroscope, 3D magnetometer
----
This is a library for the ST LSM9DS1 component.

The LSM9DS1 is a system-in-package featuring a 3D digital linear acceleration sensor, a 3D
digital angular rate sensor, and a 3D digital magnetic sensor.

The LSM9DS1 has a linear acceleration full scale of ±2g/±4g/±8/±16 g, a
magnetic field full scale of ±4/±8/±12/±16 gauss and an angular rate of
±245/±500/±2000 dps.


[* ST LSM9DS1 Home Page *](http://www.st.com/web/catalog/sense_power/FM89/SC1448/PF259998)

Host control and result reading is performed using an I2C interface, no extra pin are required.

It was principally designed to work with the SmartEverything board, but could
be easily adapt and use on every Arduino and Arduino Certified boards.

Written by Mik <smkk@axelelettronica>.

## Repository Contents
-------------------
* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager.

## Releases
---
#### v1.0.0 First Release
#### v1.1.0 Second Release 18-Dec-2015
* Fixed Issue:<br>
    Fix example: Add Wire.begin() to initiate the Wire library and join the I2C bus.<br>
    Fix bug in acceleration sensor measurement.<br>

## Documentation
--------------
* **[Installing an Arduino Library Guide](http://www.arduino.cc/en/Guide/Libraries#toc3)** - How to install a SmartEverything library on the Arduino IDE using the Library Manager


##  Information
-------------------

Copyright (c) Amel Technology. All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

