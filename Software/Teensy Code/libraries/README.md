The libraries folder includes rewritten library redirects to make thing work properly. Add these to your /Documents/Arduino/libraries folder.

SD and Audio folders

We are using a third party SD library (https://github.com/greiman/SdFat-beta) that supports SDIO access to the SD card, which is much faster.
The original SD.h libary is used in the Teensy 3 code, so we need to modify it to use SDFat-beta instead. Adding these to folders to your libaries folder will take care of that.


Wire folder

We are using a third party i2c library that support multiple i2c ports. Same as above, just add the folder and it will override the original built-in Arduino i2c libraries.
