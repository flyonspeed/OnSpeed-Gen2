float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
if ((in_max - in_min) + out_min ==0) return 0;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void _softRestart() 
{
  Serial.end();  //clears the serial monitor  if used
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
//#else  // __ARM__
extern char *___brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - ___-brkval;
#else  // __arm__
  return ___brkval ? &top - ___brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


float array2float(byte buffer[], int startIndex)
{
    float out;
    memcpy(&out, &buffer[startIndex], sizeof(float));
    return out;
}

long double array2double(byte buffer[], int startIndex)
{
    long double out;
    memcpy(&out, &buffer[startIndex], sizeof(double));
    return out;
}

int convertUnSignedIntFrom4Bytes(char data[], int startIndex) {
    return (data[startIndex + 3] << 24) | (data[startIndex + 2] << 16) | (data[startIndex + 1] << 8) | data[startIndex];
}
int convertUnSignedIntFrom2Bytes(char data[], int startIndex) {
    return ((data[startIndex+1 ] << 8) | (data[startIndex] & 0xFF));
}
int convertSignedIntFrom2Bytes(char data[], int startIndex) {
  return (short)(((data[startIndex+1 ]) & 0xFF) << 8 | (data[startIndex]) & 0xFF);
}


void configChecksum(String &configString, String &checksumString)
{
int16_t calcCRC=0;
String configContent=getConfigValue(configString,"CONFIG");
if (configContent.length()==0)
    {
    Serial.print(configString);
    }
for (unsigned int i=0;i<configContent.length();i++)
    {
    calcCRC+=configContent[i];
    checkWatchdog();
    }
// add CRC
checksumString="<CHECKSUM>";
checksumString.concat(String(calcCRC, HEX));
checksumString.concat("</CHECKSUM>");
return;
}
