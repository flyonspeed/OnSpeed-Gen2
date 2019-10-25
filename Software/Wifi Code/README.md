This is the code for the ESP32 Pico Kit D4 Wifi chip. 

To be compiled with Arduino 1.8.7 and the ESP32 Arduino board manager file from Espressif Systems v 1.0.4 (select the ESP32 Pico KIT board)

Add this to the Additonal Board Manager file sin Arduino preferences:
https://dl.espressif.com/dl/package_esp32_index.json

The https://github.com/Links2004/arduinoWebSockets library is also needed.

After programming the chip, find the "OnSpeed" wifi hotspot, connect to it, and point your browser to http://onspeed.local

The Onspeed "website" will show up where you can download files, use an AOA indexer display for testing, format the SD card, etc

NOTE: Connect to the Wifi hotspot with one client at a time (PC, phone, anythign with Wifi and a web browser). Only do one thing at a time. It's sort of a "one lane" interface for now.
