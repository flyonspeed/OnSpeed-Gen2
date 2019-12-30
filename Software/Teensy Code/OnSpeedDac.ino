// to be compiled on Arduino 1.8.7 & TeensyLoader 1.47

// TODO
// calibrate static pressure
// watchdog
// run in Gen1 mode with Efis data (fix OSH code)
// calibration run counter
// name logfile by date/time when efis data is available
// G3X data
// calibration function array (parameters of polynomial cal functions)
// wifi configuration page
// standardize data log output
// output fixed width HUD data
// AHRS+VVI+Flightpath angle

////////////////////////////////////////////////////
// OnSpeed Gen2 Teensy 3.6 code
// More details at flyOnSpeed.org

#define VERSION         "v2.1.19"  // last modified 12/27/2019 by Lenny (added 3D audio. Define and audio curve in aircraft config file)

// hardware config
#define SDCARD // comment out to disable SD card logging
#define IMU  // use inertial sensor board
//#define BOOM // read and log real-time test boom data via Serial 1 TTL
#define WIFI // use wifi board (wireless data download)
#define BARO // use baro sensor (v1 hardware doesn't have one)
#define SERIALEFISDATA  // read and log serial data via  Serial 3 RS232 (ahrs/aoa/airspeed/altitude data from Dynon EFIS D10, D100, D180, Skyview and Garmin G5)


// debug config. Comment out any of them to disable serial debug output.
//#define SENSORDEBUG // show sensor debug
//#define EFISDATADEBUG // show efis data debug
//#define BOOMDATADEBUG  // show boom data debug
//#define SHOW_SERIAL_DEBUG // Output tone related serial debug infomation.
//#define SDCARDDEBUG  // show SD writing debug info
//#define VOLUMEDEBUG // show volume setting debug
//#define AUDIO_3D_DEBUG // show 3d audio values vs efisLateralG

#include "aircraft_calibration-rv4.h"

// box functionality config
//String DATASOURCE = "TESTPOT"; // potentiometer wiper on Pin 10 of DSUB 15 connector
//String DATASOURCE = "RANGESWEEP";
String DATASOURCE = "SENSORS";
//String DATASOURCE = "REPLAYLOGFILE"; // comment out #SDCARD below when replaying log files! [logfile.csv]

// smoothing windows
#define AOA_SMOOTHING       20  // AOA Gaussian smoothing window (originally 15)
#define PRESSURE_SMOOTHING  15 // median filter window

// interval timers
#define SENSOR_INTERVAL 20000  // microsecond interval for sensor read (50hz)
#define DISPLAY_INTERVAL 100000 // 100 msec (10Hz)

// Min airspeed in knots before the audio tone is turned on
// This is useful because if your on the ground you don't need a beeping in your ear.
#define MUTE_AUDIO_UNDER_IAS  25

// max possible AOA value
#define AOA_MAX_VALUE         40  // was 20 before, but in a sudden stall when the nose quickly goes up, AOA gets larger very quickly and then onspeed goes silent right as the aircraft stalls

 // min possible AOA value
#define AOA_MIN_VALUE         -20

//Tone Pulse Per Sec (PPS)
  #define HIGH_TONE_STALL_PPS   20      // how many PPS to play during stall
  #define HIGH_TONE_PPS_MAX     6.2     
  #define HIGH_TONE_PPS_MIN     1.5     // 1.5
  #define HIGH_TONE_HZ          1600    // freq of high tone  
  #define LOW_TONE_PPS_MAX      8.2
  #define LOW_TONE_PPS_MIN      1.5
  #define LOW_TONE_HZ           400     // freq of low tone
  #define TONE_RAMP_TIME        15      // millisec
  #define STALL_RAMP_TIME       5       // millisec

// serial baud rates
  #define BAUDRATE_CONSOLE      921600
  #define BAUDRATE_BOOM         115200 
  #define BAUDRATE_EFIS         115200
  #define BAUDRATE_WIFI         115200
//  #define BAUDRATE_WIFI_HS      1240000
  #define BAUDRATE_WIFI_HS      921600  //wifi file transfer only baud rate

  // serial data timeouts
  #define EFIS_DATA_TIMEOUT     1000  // milliseconds
  #define BOOM_DATA_TIMEOUT     1000  // milliseconds

// efis serial defines
  #define DYNON_SERIAL_LEN              53  // 53 with live data, 52 with logged data

#define TONE_PIN              30     // or also 29 for dual output
#define TESTPOT_PIN           A20   // pin 39 on Teensy 3.6, pin 10 on DB15
#define VOLUME_PIN            A20  // pin 39 used for audio volume, pin 10 on DB15
#define PIN_LED1              13    // internal LED for showing serial input state.
#define PIN_LED2              5    // external LED for showing AOA status (audio on/off), don't forget to use resistor on LED
#define FLAP_PIN              A2     // flap position switch  (pin 7 on DB15)
#define SWITCH_PIN            2
#define PULSE_TONE            1
#define SOLID_TONE            2
#define TONE_OFF              3
#define STARTUP_TONES_DELAY   120
#define RANGESWEEP_LOW_AOA    4
#define RANGESWEEP_HIGH_AOA   20
#define RANGESWEEP_STEP       .1 // degrees AOA

//﻿DB15 pinout (Gen2 v3 hardware)
//1 - 14V +PWR
//2 - EFIS Serial RX
//3 - PANEL SWITCH
//4 - GPS Serial RX
//5 - LED+ Digital/PWM
//6 - AUDIO RIGHT
//7 - FLAPS Analog IN
//8 - AUDIO LEFT
//9 - OAT Analog IN
//10 - VOLUME Analog IN
//11 - SENSOR PWR 3.3V
//12 - EFIS Serial TX
//13 - BOOM TTL RX
//14 - GND
//15 - AUDIO GND 


// IMU defines
#define LSM9DS1_AccelGyro 0x6B
#define LSM9DS1_Magnet    0x1E
// LSM9DS1 Accel/Gyro (XL/G) Registers
#define CTRL_REG5_XL      0x1F
#define CTRL_REG6_XL      0x20
#define CTRL_REG7_XL      0x21
#define OUT_X_L_XL        0x28

#define I2C_COMMUNICATION_TIMEOUT 1000  // microseconds

#define _GNU_SOURCE

#include <stdint.h>
#include <i2c_t3.h> // multiport i2c (Wire.h needs to redirect here, otherwise it gets duplicated. Make a Wire.h library with an #include <i2c_t3.h> line in it.

// SD card includes
#include <SD.h>  // use local SD.h that redirects to the SdFat library for faster SD card access
SdFat SD;
// SdCardFactory constructs and initializes the appropriate card. Needed for card formatting
SdCardFactory cardFactory;
SdCard* m_card = nullptr;
uint32_t cardSectorCount = 0;
uint8_t  sectorBuffer[512];

//#include "default_config.h"
#include <Audio.h>      // use local Audio.h with play_sd_raw.h & play_sd_wav.h commented out for SDIO compatibility.
#include <Gaussian.h>         // gaussian lib used for avg out AOA values.
#include <LinkedList.h>       // linked list is also required.
#include <GaussianAverage.h>  // more info at https://github.com/ivanseidel/Gaussian
#include <OneButton.h>      // button click/double click detection https://github.com/mathertel/OneButton
#include <RunningMedian.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
#include "AudioSampleEnabled.h"
#include "AudioSampleDisabled.h"
#include "AudioSampleCalibrate_flapsdown.h"
#include "AudioSampleCalibrate_flapsup.h"
#include "AudioSampleCalibration_canceled.h"
#include "AudioSampleCalibration_mode.h"
#include "AudioSampleCalibration_saved.h"

// audio mixer config
AudioSynthWaveformSine   sinewave1;
AudioEffectEnvelope      envelope1;
AudioPlayMemory          voice1;
AudioMixer4              mixer1;
AudioOutputAnalogStereo  dacs;
AudioAmplifier           ampLeft;
AudioAmplifier           ampRight;
AudioConnection          patchCord1(sinewave1, envelope1);
AudioConnection          patchCord2(envelope1, 0, mixer1, 0);
AudioConnection          patchCord3(voice1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, ampLeft);
AudioConnection          patchCord5(mixer1, ampRight);
AudioConnection          patchCord6(ampLeft, 0, dacs, 0);
AudioConnection          patchCord7(ampRight, 0, dacs, 1);

// rangesweep direction
int rangeSweepDirection=1; // positive
    
volatile float efisASI=0.00;
volatile float efisPitch=0.00;
volatile float efisRoll=0.00;
volatile float efisLateralG=0.00;
volatile float efisVerticalG=0.00;
volatile int efisPercentLift=0;
volatile int efisPalt=0;
volatile int efisVSI=0;
volatile float efisTAS;
volatile float efisOAT;
volatile float efisFuelRemaining=0.00;
volatile float efisFuelFlow=0.00;
volatile float efisMAP=0.00;
volatile int efisRPM=0;
volatile int efisPercentPower=0;
volatile int efisHeading=-1;
String efisTime="";
volatile unsigned long efisTimestamp=millis();
String efisType="";

unsigned long lastReceivedEfisTime;
int efis_bufferIndex=0;
String efisBufferString;
char efis_inChar;               // efis serial character
char last_efis_inChar=char(0x00);
volatile int charsreceived=0; // debug
volatile int cachelinecount=0; //debug variable
   

#ifdef BOOM
// boom variables
unsigned long lastReceivedBoomTime;
volatile float boomStatic=0.0;
volatile float boomDynamic=0.0;
volatile float boomAlpha=0.0;
volatile float boomBeta=0.0;
volatile float boomIAS=0.0;
volatile unsigned long boomTimestamp=millis();
// boom parse arrays
int parse_array[4];
int parse_array_index=0;
char parseBuffer[10];
int parseBufferSize=0;
#endif

OneButton Switch(SWITCH_PIN, true);  // pin 2  used for the switch input


File SensorFile;
File ListFile;
char filenameSerial[14];
char filenameSensor[14];
bool sdLogging=false;
int filesendtimer;
bool filesendtimeout;

uint8_t toneState = false;
bool switchState;
unsigned char toneMode = PULSE_TONE;  // current mode of tone.  PULSE_TONE, SOLID_TONE, or TONE_OFF
unsigned char tonePlaying = TONE_OFF;
boolean highTone = false;             // are we playing high tone or low tone?
uint32_t toneFreq = 0;                // store current freq of tone playing.
volatile float pps = 20;                        // store current PPS of tone (used for debuging) 
//float current_delay=1000/pps;                  // 1000/pps, tone timer update rate
float AOA = 0.0;                          // avaraged AOA value is stored here.
float LDmaxAOA=0.00;
float onSpeedAOAfast=0.00;
float onSpeedAOAslow=0.00;
float stallWarningAOA=0.00;
float percentLift=0.0;                     // normalized angle of attack, or lift %
unsigned int ALT = 0;                 // hold ALT (only used for debuging)
float ASI = 0.0;                          // live Air Speed Indicated
float Palt=0.00;                          // pressure altitude
float currentRangeSweepValue=RANGESWEEP_LOW_AOA;
RunningMedian PfwdDivP45Median(PRESSURE_SMOOTHING);
RunningMedian PfwdMedian(PRESSURE_SMOOTHING);
RunningMedian P45Median(PRESSURE_SMOOTHING);
GaussianAverage AOAAvg = GaussianAverage(AOA_SMOOTHING);
//GaussianAverage AOAAvg2 = GaussianAverage(50);

// vars for converting AOA scale value to PPS scale value.
int OldRange,  OldValue;
float NewRange, NewValue;

char inChar;                    // store single serial input char here.
char serialCmdChar;             // usb serial command character
char serialWifiCmdChar;         // wifi serial command character  

char serialBoomChar;            // boom serial character
int serialCmdBufferSize = 0;        // usb serial command buffer size
char serialCmdBuffer[51];       // usb serial command buffer
int serialBufferSize = 0;    // serial buffer size (boom data)
char serialBuffer[51];       // serial 1  buffer (boom data)

char listfileFileName[14];      // file name for sd card file listing operations
bool listfileIsDirectory;      

#define MAXSIZE 90              // max length of string
char input[MAXSIZE+1];          // buffer for full serial data input string.
int inputPos = 0;               // current postion of serial data.
char tempBuf[90];               // misc char buffer used for debug
char logBufSerial[50];                // char buffer for serial logging
char sensorCache[32768];             // around 163 lines at 50 characters
int sensorCacheCount=0;
char SDwriteCache[32768];
int SDwriteCacheCount=0;
unsigned long sensorLoggingStartTime;
unsigned long last_time = millis();

// IMU variables
uint8_t _i2cAddress_AccelGyro=LSM9DS1_AccelGyro;
int16_t ax, ay, az;
float aRes=0.000244140625; // 8g / 32768.0
uint8_t Ascale = 3;     // accel full scale, 8G
uint8_t Aodr = 6;   // accel data sample rate,  AODR_952Hz
uint8_t Abw = 3;      // accel data bandwidth,  ABW_50Hz

IntervalTimer ToneTimer;
IntervalTimer SensorTimer; // read sensors
IntervalTimer PotTimer; // read potentiometer           
IntervalTimer RangeSweepTimer; // sweep through AOA range, high to low
IntervalTimer LogTimer;
IntervalTimer LogReplayTimer;
IntervalTimer LiveDisplayTimer;

// volume variables
float LOW_TONE_VOLUME;     
float HIGH_TONE_VOLUME_MIN; 
float SOLID_TONE_VOLUME;
float HIGH_TONE_VOLUME_MAX;
int volPos=0;
int avgSlowVolPos=512;
unsigned long volumeStartTime=millis();
int flapsPos=0;
int flapsIndex=0;
int loopcount=0;
unsigned long looptime=millis();

void setup() {
 delay(2000); // let the aircraft's audio panel boot up

  AudioMemory(16);
// volume control
#ifdef VOLUMECONTROL
  readVolume();  
#else
LOW_TONE_VOLUME=.25;      // volume is 0 to 1
HIGH_TONE_VOLUME_MIN=.25;      // high tone will ramp up from min to max
SOLID_TONE_VOLUME=.25;
HIGH_TONE_VOLUME_MAX=1;
mixer1.gain(2,10); // amplify channel 2 (voice)
#endif
  
  // turn off initial sounds
  Serial.print("OnSpeed Gen2 ");
  Serial.println(VERSION);
// init SD card
#ifdef SDCARD


    if (SD.begin(SdioConfig(FIFO_SDIO))) {    
        sprintf(filenameSensor,"log_1.csv");        
        int fileCount=1;
        while (SD.exists(filenameSensor))
              {
              fileCount++;  
              sprintf(filenameSensor,"log_%d.csv",fileCount);
              }
        Serial.print("Sensor log file:"); Serial.println(filenameSensor);
        Serial.println();
        sdLogging=true;
        Serial.println("SD card initialized. Accepted commands:");
        Serial.println("STOP! - stop logging to SD card");
        Serial.println("LIST! - list files on SD card");
        Serial.println("DELETE filename! - delete file on SD card");
        Serial.println("PRINT filename! - display file contents");        
        Serial.println("FORMAT! - format SD card");
        Serial.println("NOLOAD! - show pressure sensor bias");
        Serial.println("START! - start logging to SD card");
        Serial.println("REBOOT! - reboot system");
        Serial.println("WIFIREFLASH!- allow reflashing Wifi chip via USB cable");
        Serial.println("FLAPS! - show current flap position value");
        Serial.println("VOLUME! -show current volume potentiometer value");                
        Serial.println();
        SensorFile = SD.open(filenameSensor, FILE_WRITE);
        if (SensorFile) {
          SensorFile.print("timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,PStatic,Palt,IAS,AngleofAttack,flapsPos");
          #ifdef IMU
          SensorFile.print(",Ax,Ay,Az");
          #endif        
          #ifdef BOOM        
          SensorFile.print(",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge");
          #endif           
          #ifdef SERIALEFISDATA
          SensorFile.print(",efisIAS,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisMagHeading,efisAge,efisTime");
          #endif
          SensorFile.println();
          SensorFile.close();
         //LogTimer.priority(250);
         //LogTimer.begin(SensorWriteSD,2000000); // write log file once every 2 seconds
        }  else Serial.print("SensorFile opening error");
       
        //AOA, IAS, ALT, pps,
        
      } else
            {
            Serial.println("SD card initialization failed! Logging not available."); 
            }
  
  #endif // sdcard

  // set up i2c ports
  Wire.begin (I2C_MASTER,0x00,I2C_PINS_18_19,I2C_PULLUP_EXT,400000); // SDA0/SCL0 I2C at 400kHz, PS1 sensor, Pfwd   
  Wire.setOpMode(I2C_OP_MODE_IMM);
  Wire.setDefaultTimeout(2000);

  Wire2.begin(I2C_MASTER,0x00,I2C_PINS_3_4,I2C_PULLUP_EXT,400000); // SDA1/SCL1 I2C at 400kHz, PS2 sensor, P45
  Wire2.setOpMode(I2C_OP_MODE_IMM);
  Wire2.setDefaultTimeout(2000);
  
  Wire1.begin();
  Wire1.setDefaultTimeout(2000);

  initAccel(); //initialize accelerometer (IMU)

  
  
  pinMode(TONE_PIN, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(FLAP_PIN, INPUT_PULLUP);
  pinMode(TESTPOT_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  //attachInterrupt(SWITCH_PIN, switchCheck, CHANGE);
  Switch.attachClick(SwitchSingleClick);
  Switch.attachLongPressStart(SwitchLongPress);
  
  // default switch state
  //turn audio switch/led on on power-up
  switchState=false;
  switchOnOff();
  
  
  Serial.begin(BAUDRATE_CONSOLE);   //Init hardware serial port (ouput to computer for debug)

  #ifdef WIFI
  Serial4.begin(BAUDRATE_WIFI);
  //Serial4.begin(921600); // comm with wifi board
  #endif

  #ifdef BOOM
  Serial1.begin(BAUDRATE_BOOM);  //Init hardware serial port (input from BOOM)
  #endif

  #ifdef SERIALEFISDATA
  Serial3.begin(BAUDRATE_EFIS);
  #endif
    
  digitalWrite(PIN_LED1, 1);
  
  digitalWrite(PIN_LED1, 0);
  
  ToneTimer.priority(16);
  ToneTimer.begin(tonePlayHandler,1000000/pps); // microseconds     
  
  #ifdef SERIALEFISDATA
  lastReceivedEfisTime=millis();
  #endif
  #ifdef BOOM
  lastReceivedBoomTime=millis();
  #endif


#ifdef WIFI
   LiveDisplayTimer.priority(224);
#endif
  
  
  
  if (DATASOURCE=="SENSORS")
    {
    SensorTimer.priority(128);
    SensorTimer.begin(SensorRead,SENSOR_INTERVAL); // every 20ms       
    } else
          if (DATASOURCE=="TESTPOT")
              {
              PotTimer.begin(PotRead,SENSOR_INTERVAL); // 20ms            
              } else
                    if (DATASOURCE=="RANGESWEEP")
                    {
                    RangeSweepTimer.begin(RangeSweep,200000); // 100ms
                    } else
                          
                          if (DATASOURCE=="REPLAYLOGFILE")
                          {
                          // turn off SD logging
                          sdLogging=false;
                          // check if file exists
                           sprintf(filenameSensor,"logfile.csv");
                            
                            if (SD.exists(filenameSensor))
                                  {
                                  Serial.print("Replaying data from log file: "); Serial.println(filenameSensor);
                                  } else
                                        {
                                        Serial.println("ERROR: Could not find logfile.csv on the SD card.");
                                        }                                                   
                            
                            SensorFile = SD.open(filenameSensor);        
                            if (SensorFile)
                                           {                                                                    
                                            LogReplayTimer.priority(240);
                                            LogReplayTimer.begin(LogReplay,SENSOR_INTERVAL); // same interval as SensorRead
                                           } else
                                           {
                                            Serial.println("ERROR: Could not open logfile.csv on the SD card.");
                                           }
                                           
                          //  Serial.println("Log file opened.");               
                                                                                                                  
                          }

 #ifdef SDCARD 
  // start logging timer
  sensorLoggingStartTime=millis();
 #endif 
 // initialize sine wave generators
 AudioNoInterrupts();
 sinewave1.frequency(400);
 sinewave1.amplitude(0);
 //sinewave1.begin(WAVEFORM_SINE);
 //sinewave_solid.begin(WAVEFORM_SINE);
 AudioInterrupts(); 
}


uint8_t Tone2FlipFlop = false;


void tonePlayHandler(){
    switchCheck(); // check main switch   
    if (!switchState) return; // return if tones are disabled 
  if(toneMode==TONE_OFF) {
    setFrequencytone(0);  // if tone off skip the rest.
    #ifdef SHOW_SERIAL_DEBUG 
    Serial.println("TONE OFF");
    #endif        
    return;
  }
  if(toneMode==SOLID_TONE) {  // check for a solid tone.
    #ifdef SHOW_SERIAL_DEBUG 
    Serial.println("SOLID TONE");
    #endif
    setFrequencytone(LOW_TONE_HZ);
    return; // skip the rest
  }
     if(highTone) {
                  setFrequencytone(HIGH_TONE_HZ);
                  #ifdef SHOW_SERIAL_DEBUG 
                  Serial.println("HIGH TONE");
                  #endif
                  } else {
                          setFrequencytone(LOW_TONE_HZ);
                          #ifdef SHOW_SERIAL_DEBUG 
                          Serial.println("LOW TONE");
                          #endif
                         } 
                         
}

void updateTones()
{
  if(ASI <= MUTE_AUDIO_UNDER_IAS) {
  #ifdef SHOW_SERIAL_DEBUG    
  // show audio muted and debug info.
  sprintf(tempBuf, "AUDIO MUTED: Airspeed too low. Min:%i ASI:%.2f",MUTE_AUDIO_UNDER_IAS, ASI);
  Serial.println(tempBuf);
  #endif 
  toneMode = TONE_OFF;
  setPPSTone(20); // set the update rate to LOW_TONE_PPS_MAX if no tone is playing to pick up a pulsed tone quickly
  return;
  }
  // check AOA value and set tone and pauses between tones according to 
  if(AOA >= stallWarningAOA) {
    // play 20 pps HIGH tone
    highTone = true;
    setPPSTone(HIGH_TONE_STALL_PPS);
    toneMode = PULSE_TONE;
    } else if(AOA > (onSpeedAOAslow))
            {
            // play HIGH tone at Pulse Rate 1.5 PPS to 6.2 PPS (depending on AOA value)
            highTone = true;
            toneMode = PULSE_TONE;
            NewValue=mapfloat(AOA,onSpeedAOAslow,stallWarningAOA,HIGH_TONE_PPS_MIN,HIGH_TONE_PPS_MAX);
            setPPSTone(NewValue); // when transitioning from solid to high tone make the first one shorter
            } else if(AOA >= (onSpeedAOAfast))
                   {
                    // play a steady LOW tone
                    highTone = false;
                    toneMode = SOLID_TONE;
                    setPPSTone(LOW_TONE_PPS_MAX);// set PPS to LOW_TONE_PPS_MAX for a higher update rate when coming out of it
                   } else if (AOA >= LDmaxAOA && LDmaxAOA<onSpeedAOAfast)
                          {  // if L/D max AOA is higher than OnSpeedfast, skip the low tone. This usually happens with full flaps.
                          toneMode = PULSE_TONE;
                          highTone = false;
                          // play LOW tone at Pulse Rate 1.5 PPS to 8.2 PPS (depending on AOA value)
                          NewValue=mapfloat(AOA,LDmaxAOA,onSpeedAOAfast,LOW_TONE_PPS_MIN,LOW_TONE_PPS_MAX);
                          setPPSTone(NewValue);
                          } else {                          
                                 toneMode = TONE_OFF;
                                 setPPSTone(HIGH_TONE_STALL_PPS);  // high update rate pps 
                                 }    
}

void setPPSTone(float newPPS) {
   noInterrupts();
   pps=newPPS;   
   interrupts();  
}



void switchCheck()
{
Switch.tick();
}

void SwitchSingleClick()
{
switchOnOff();
}

void SwitchLongPress()
{
Serial.println("Switch Longpress");
}


// main loop of app
void loop() { 
loopcount++;


readSerialData();


#ifdef SDCARD
// look for serial command
if (Serial.available())
  {  
//LIST!
//DELETE filename!S
//PRINT filename!
//STOP!
//START!
//FORMAT!
//NOLOAD!
// WIFIREFLASH!
// REBOOT!
// FLAPS!
// VOLUME!
serialCmdChar = Serial.read();    
  if (serialCmdChar!=char(0x21) && serialCmdChar!=char(0x0D) && serialCmdChar!=char(0x0A) && serialCmdBufferSize<50)
    {    
    serialCmdBuffer[serialCmdBufferSize]=serialCmdChar;
    serialCmdBufferSize++;
    } else
          {
          // process command          
            if (strstr(serialCmdBuffer, "LIST"))
              {
              // list files
              Serial.println("Files list:");
              ListFile=SD.open("/");
              if (!ListFile) Serial.println("SD card missing or unreadable");
              while(true) {
     
                           File entry =  ListFile.openNextFile();
                           if (! entry) {
                                         // no more files                             
                                         break;
                                        }                           
                           
                           if (!entry.isDirectory())
                              {
                              // only list files in root folder, no directories
                              entry.printName(&Serial);
                              Serial.print("    ");
                              Serial.println(entry.size(), DEC);                              
                              }                             
                           entry.close();
                           }
                           
              
              } else
                     if (strstr(serialCmdBuffer, "DELETE"))
                        {
                        // delete file                        
                        memcpy(listfileFileName,serialCmdBuffer+7,serialCmdBufferSize-6);
                        SD.remove(listfileFileName);
                        Serial.println();
                        Serial.print("Deleted: ");
                        Serial.println(listfileFileName);
                        } else
                              if (strstr(serialCmdBuffer, "PRINT"))
                                 {
                                 // print file contents
                                 memcpy(listfileFileName,serialCmdBuffer+6,serialCmdBufferSize-5);
                                 unsigned long transmitStartTime;
                                 transmitStartTime=micros();
                                 unsigned long transmitDelay=ceil(10000000/BAUDRATE_CONSOLE+0.5);
                                 ListFile=SD.open(listfileFileName);
                                 if (ListFile) {
                                              Serial.println();
                                              Serial.print(listfileFileName);                                              
                                              Serial.println(":");
                                              // read from the file until there's nothing else in it:
                                              while (ListFile.available()) {
                                               if (micros()-transmitStartTime>transmitDelay)
                                                      {
                                                      Serial.write(ListFile.read());
                                                      transmitStartTime=micros();
                                                      }                                               
                                              }
                                              // close the file:
                                              ListFile.close();
                                              Serial.println("DONE.");
                                            } else {
                                              // if the file didn't open, print an error:
                                              Serial.print("Error opening "); Serial.println(listfileFileName);
                                            }
                                 
                                 } else
                                      if (strstr(serialCmdBuffer, "STOP"))
                                         {
                                         // stop SD logging to take file commands
                                         sdLogging=false;
                                         if (DATASOURCE=="SENSORS")
                                              {
                                              SensorTimer.end();                                                                                     
                                              }
                                         Serial.println("STOPPED SD logging.");
                                         } else
                                              if (strstr(serialCmdBuffer, "START"))
                                                 {
                                                 // start SD logging
                                                 //ToneTimer.begin;
                                                 sdLogging=true;
                                                 if (DATASOURCE=="SENSORS") 
                                                      {                                                      
                                                      SensorTimer.begin(SensorRead,SENSOR_INTERVAL);
                                                      Serial.println("STARTED SD logging.");
                                                      }                                                      
                                                 
                                                 } else
                                                      if (strstr(serialCmdBuffer, "FORMAT"))
                                                          {
                                                          bool orig_sdLogging=sdLogging;
                                                           if (orig_sdLogging) sdLogging=false; // turn off sdLogging                                                           
                                                                                                                    
                                                            ExFatFormatter exFatFormatter;
                                                            FatFormatter fatFormatter;                                                            
                                                            m_card = cardFactory.newCard(SdioConfig(FIFO_SDIO));
                                                            if ((!m_card || m_card->errorCode())) {
                                                                 Serial.print("FORMAT ERROR: Cannot initialize SD card. ");
                                                                 Serial.println(m_card->errorCode());                                                                 
                                                              } else                                                               
                                                                 {
                                                                  cardSectorCount = m_card->sectorCount();
                                                                  //Serial.printf("Sectorcount: %i\n",cardSectorCount);
                                                                    // Format exFAT if larger than 32GB.
                                                                    bool rtn = cardSectorCount > 67108864 ?
                                                                      exFatFormatter.format(m_card, sectorBuffer, &Serial) :
                                                                      fatFormatter.format(m_card, sectorBuffer, &Serial);                                                                  
                                                                    if (!rtn) {
                                                                      Serial.println("FORMAT ERROR: Could not format SD card.");
                                                                    } else
                                                                          {
                                                                          Serial.print("SD card format completed. Card size: ");
                                                                          Serial.print(cardSectorCount*5.12e-7);
                                                                          Serial.println("GBytes");
                                                                          }                                                                    
                                                                 }  
                                                           
                                                                         
                                                          if (orig_sdLogging)
                                                                            {
                                                                            //reinitialize card
                                                                            SD.begin(SdioConfig(FIFO_SDIO));
                                                                            sdLogging=true; // if logging was on before FORMAT turn it back on               
                                                                            }
                                                          
                                                          } else
                                                              if (strstr(serialCmdBuffer, "NOLOAD"))
                                                                  {                                                                  
                                                                  SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                  Serial.println("Getting Pressure sensor bias...");
                                                                  // get Pfwd bias
                                                                  long PfwdTotal=0;
                                                                  long P45Total=0;
                                                                  for (int i=1;i<=1000;i++)
                                                                      {
                                                                      PfwdTotal+=GetPressurePfwd();
                                                                      P45Total+=GetPressureP45();
                                                                      delay(10);
                                                                      }
                                                                 Serial.print("Pfwd bias=");
                                                                 Serial.println(PfwdTotal/1000);
                                                                 Serial.print("P45 bias=");
                                                                 Serial.println(P45Total/1000);                                                                                                                                
                                                                  }  else
                                                                     if (strstr(serialCmdBuffer, "WIFIREFLASH"))                                                                            
                                                                            {
                                                                            Serial4.end();
                                                                            pinMode(PIN_A12,INPUT);
                                                                            pinMode(PIN_A13,INPUT);
                                                                            Serial.println("wifi reflash mode activated");
                                                                            } else
                                                                                   if (strstr(serialCmdBuffer, "REBOOT"))                                                                            
                                                                                        {
                                                                                        Serial.println("serial reboot request. Rebooting...");
                                                                                         delay(2000);
                                                                                        _softRestart();
                                                                                        }
                                                                                         else 
                                                                                            if (strstr(serialCmdBuffer, "FLAPS"))                                                                            
                                                                                                {
                                                                                                Serial.print("Current flap potentiometer analog value: ");
                                                                                                int flapPotTotal=0;
                                                                                                for (int i=0;i<100;i++)
                                                                                                  {
                                                                                                  flapPotTotal+=analogRead(FLAP_PIN);
                                                                                                  }
                                                                                                  
                                                                                                Serial.println(flapPotTotal/100);
                                                                                                } else 
                                                                                                      if (strstr(serialCmdBuffer, "VOLUME"))                                                                            
                                                                                                          {
                                                                                                          Serial.print("Current volume potentiometer analog value: ");
                                                                                                          int volumePotTotal=0;
                                                                                                          for (int i=0;i<100;i++)
                                                                                                            {
                                                                                                            volumePotTotal+=analogRead(VOLUME_PIN);
                                                                                                            }                                                                                                            
                                                                                                          Serial.println(volumePotTotal/100);
                                                                                                          }

                                                                                                                                                                          

          
          // reset cmdBuffer
          memset(serialCmdBuffer,0,sizeof(serialCmdBuffer));
          serialCmdBufferSize=0;    
          }
  } // if serial.available
#endif // sdcard


#ifdef WIFI
// look for wifi serial command
if (Serial4.available())
  {
//$LIST!
//$DELETE filename!
//$PRINT filename!
//$STOPLIVEDATA!
//$STARTLIVEDATA!
//$FORMAT!
//$NOLOAD!
//$REBOOT!
serialWifiCmdChar = Serial4.read();
Serial.print(serialWifiCmdChar);
if (serialCmdBufferSize >=50)
      {
      memset(serialCmdBuffer,0,sizeof(serialCmdBuffer));  
      serialCmdBufferSize=0; // don't let the command buffer overflow
      }
  if (serialWifiCmdChar!=char(0x21) && serialWifiCmdChar!=char(0x0D) && serialWifiCmdChar!=char(0x0A) && serialCmdBufferSize<50)
    {    
    serialCmdBuffer[serialCmdBufferSize]=serialWifiCmdChar;
    serialCmdBufferSize++;
    } else
          {            
          // process command          
            if (strstr(serialCmdBuffer, "$LIST"))
              {
              // list files
              ToneTimer.end(); // turn off ToneTimer
              Serial.println("file list requested on Wifi");
              bool orig_sdLogging=sdLogging;
              if (orig_sdLogging) sdLogging=false; // turn off sdLogging  
              String(fileList)="";
              fileList+="<filelist>";
              ListFile=SD.open("/");
              if (!ListFile) Serial.println("SD card missing or unreadable");
              while(true) {
     
                           File entry =  ListFile.openNextFile();
                           if (! entry) {
                                         // no more files                                                                      
                                         fileList+="<eof>";
                                         break;
                                        }                           
                           
                           if (!entry.isDirectory())
                              {
                              // only list files in root folder, no directories
                              char entryName[15];
                              entry.getName(entryName, sizeof(entryName));
                              fileList+=entryName;
                              fileList+=":";
                              fileList+=entry.size();
                              fileList+="\r\n";
                              }                             
                           entry.close();
                           }
                  int outcount=0;
                  for (unsigned int i=0;i<fileList.length();i++)
                      {
                       Serial4.print(fileList[i]);
                       outcount++;
                       if (outcount>=20)
                            {
                            outcount=0;
                            filesendtimer=millis();
                            filesendtimeout=false;
                            //ToneTimer.priority(255); // deprioritize tone timer to achieve a high TX rate on Serial4.
                            while(true)
                                        {
                                        // check for timeout
                                        if (millis()-filesendtimer > 3000) // 3 seconds timeout
                                          {
                                           filesendtimeout=true;
                                           break;  
                                          }
                                        // hold here until an ACK is received on serial.
                                        if (Serial4.available() && Serial4.read()=='.')
                                          {
                                          break;                                                                    
                                          }
                                        
                                        }
                            //ToneTimer.priority(10);            
                            if (filesendtimeout) 
                                {
                                  break;
                                  Serial.println("File list transfer timeout");            
                                }
                            }            
                       
                      }                        
               
              if (orig_sdLogging) sdLogging=true; // turn sd logging back on if it was originally on.
              ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on
              
              } else
                     if (strstr(serialCmdBuffer, "$DELETE"))
                        {
                        // delete file                        
                        memcpy(listfileFileName,serialCmdBuffer+8,serialCmdBufferSize-7);
                        SD.remove(listfileFileName);
                        Serial4.println();
                        Serial4.print("Deleted: ");
                        Serial4.println(listfileFileName);
                        } else
                              if (strstr(serialCmdBuffer, "$PRINT"))
                                 {
                                 bool orig_sdLogging=sdLogging;
                                 if (orig_sdLogging)
                                      {
                                      sdLogging=false; // turn off sdLogging  
                                      SensorTimer.end(); // turn of SensorTimer                                      
                                      Serial.println("STOPPED SD logging.");
                                      }
                                 ToneTimer.end(); // turn off ToneTimer                                                                                                   
                                 // print file contents
                                 memcpy(listfileFileName,serialCmdBuffer+7,serialCmdBufferSize-6);
                                 unsigned long transmitStartTime;
                                 transmitStartTime=micros();
                                 Serial4.begin(BAUDRATE_WIFI_HS);
                                 delay(1000); // wait a bit after changing baud rates
                                 unsigned long transmitDelay=ceil(10000000/BAUDRATE_WIFI_HS);
                                 ListFile=SD.open(listfileFileName);
                                 if (ListFile) {     
                                              // read from the file until the end
                                              int outcount=0;
                                              while (ListFile.available())
                                              {                                                                                                                                                   
                                                     if (micros()-transmitStartTime<transmitDelay) continue; // obey baud rate limit                                                                                                       
                                                     Serial4.write(ListFile.read());
                                                     transmitStartTime=micros();
                                                     outcount++;                                                    
                                                    if (outcount>=1024)
                                                          {
                                                          outcount=0;
                                                          filesendtimer=millis();
                                                          filesendtimeout=false;
                                                          // hold here until an ACK is received on serial.  
                                                          while(true)
                                                                      {
                                                                      // check for timeout
                                                                      if (millis()-filesendtimer > 5000)
                                                                        {
                                                                         filesendtimeout=true;                                                                         
                                                                         break;  
                                                                        }  
                                                                                                                                          
                                                                      if (Serial4.available() && Serial4.read()=='.')
                                                                        {                                                                        
                                                                        break; // break out of ACK hold                                                                                                                                               
                                                                        }
                                                                      
                                                                      }
                                                         if (filesendtimeout) 
                                                                    {
                                                                      break;
                                                                      Serial.println("file list transfer timeout");            
                                                                    }                                                               
                                                          }
                                              }
                                              // close the file:                                              
                                              ListFile.close();
                                              
                                              
                                              
                                            } else {
                                              // if the file didn't open, print an error:
                                              Serial4.print("<404>");
                                            }
                                 Serial4.begin(BAUDRATE_WIFI); // reset wifi baudrate to lower speed     
                                 if (orig_sdLogging)
                                    {
                                    sdLogging=true; // turn sd logging back on if it was originally on.                                                                        
                                    Serial.println("STARTED SD logging.");
                                    }
                                 SensorTimer.begin(SensorRead,SENSOR_INTERVAL);   
                                 ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on
                                 } else
                                         
                                          if (strstr(serialCmdBuffer, "$STOPLIVEDATA"))
                                         {
                                       
                                         LiveDisplayTimer.end(); // stop display data                                         
                                         Serial.println("STOPPED Live Data. (Wifi Request)");
                                         } else
                                              if (strstr(serialCmdBuffer, "$STARTLIVEDATA"))
                                                 {
                                                 LiveDisplayTimer.begin(SendDisplayData,DISPLAY_INTERVAL); // start display data
                                                 Serial.println("STARTED Live Data. (Wifi Request)");
                                                 } else

                                                   if (strstr(serialCmdBuffer, "$FORMAT"))
                                                          {
                                                          // delete all files                        
                                                          Serial.println("Wifi requested SD format");
                                                           bool orig_sdLogging=sdLogging;
                                                           if (orig_sdLogging) sdLogging=false; // turn off sdLogging 


                                                            ExFatFormatter exFatFormatter;
                                                            FatFormatter fatFormatter;                                                            
                                                            m_card = cardFactory.newCard(SdioConfig(FIFO_SDIO));
                                                            if ((!m_card || m_card->errorCode())) {
                                                                 Serial.print("FORMAT ERROR: Cannot initialize SD card. ");
                                                                 Serial.println(m_card->errorCode());
                                                                 Serial4.println("<formaterror>");                                                                 
                                                              } else                                                               
                                                                 {
                                                                  cardSectorCount = m_card->sectorCount();
                                                                  //Serial.printf("Sectorcount: %i\n",cardSectorCount);
                                                                    // Format exFAT if larger than 32GB.
                                                                    bool rtn = cardSectorCount > 67108864 ?
                                                                      exFatFormatter.format(m_card, sectorBuffer, &Serial) :
                                                                      fatFormatter.format(m_card, sectorBuffer, &Serial);                                                                  
                                                                    if (!rtn) {
                                                                      Serial.println("FORMAT ERROR: Could not format SD card.");
                                                                    } else
                                                                          {
                                                                          Serial.print("SD card format completed. Card size: ");
                                                                          Serial.print(cardSectorCount*5.12e-7);
                                                                          Serial.println("GBytes");
                                                                          Serial4.printf("<formatdone %.1f GB>\n",cardSectorCount*5.12e-7);
                                                                          
                                                                          }                                                                    
                                                                 }  
                                                                                                                    
                                                          if (orig_sdLogging)
                                                                {
                                                                //reinitialize card
                                                                SD.begin(SdioConfig(FIFO_SDIO));
                                                                sdLogging=true; // turn sd logging back on if it was originally on.
                                                                }
                                                          } else
                                                              if (strstr(serialCmdBuffer, "$NOLOAD"))
                                                                  {                                                                  
                                                                  SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                  Serial4.println("Getting Pressure sensor bias...");
                                                                  // get Pfwd bias
                                                                  long PfwdTotal=0;
                                                                  long P45Total=0;
                                                                  for (int i=1;i<=1000;i++)
                                                                      {
                                                                      PfwdTotal+=GetPressurePfwd();
                                                                      P45Total+=GetPressureP45();
                                                                      delay(10);
                                                                      }
                                                                 Serial4.print("Pfwd bias=");
                                                                 Serial4.println(PfwdTotal/1000);
                                                                 Serial4.print("P45 bias=");
                                                                 Serial4.println(P45Total/1000);                                                                                                                                
                                                                  } else
                                                                        
                                                                        if (strstr(serialCmdBuffer, "$WIFIREFLASH"))                                                                            
                                                                            {
                                                                            Serial4.end();
                                                                            pinMode(PIN_A12,INPUT);
                                                                            pinMode(PIN_A13,INPUT);
                                                                            Serial.println("wifi reflash mode");
                                                                            } else
                                                                                   if (strstr(serialCmdBuffer, "$REBOOT"))                                                                            
                                                                                        {
                                                                                        Serial.println("Wifi reboot request. Rebooting...");                                                                                       
                                                                                        _softRestart();
                                                                                        }
                                                                            

          // reset cmdBuffer
          memset(serialCmdBuffer,0,sizeof(serialCmdBuffer));
          serialCmdBufferSize=0;    
          }  
  } // if serial.available wifi

#endif



if (millis()-looptime > 1000) // check serial ports every second (reset connection if needed) and write to SD card
      {      
      #ifdef EFISDATADEBUG
      Serial.printf("\nloopcount: %i",loopcount);
      Serial.printf("\nchars received: %i",charsreceived);
      #endif
            
      #ifdef SERIALEFISDATA
          //check for efis stream, restart port of not receiving data for half second
          if (millis()-lastReceivedEfisTime>=EFIS_DATA_TIMEOUT)
             {             
             Serial3.end();             
             Serial3.begin(BAUDRATE_EFIS);             
             #ifdef EFISDATADEBUG
             Serial.println("\n Efis data timeout. Restarting serial port 3");
             Serial.printf("\nEfis last transmit: %i", millis()-lastReceivedEfisTime);             
             #endif
             }
      #endif
      #ifdef BOOM
          //check for efis stream, restart port of not receiving data for half second
          if (millis()-lastReceivedBoomTime>=BOOM_DATA_TIMEOUT)
             {             
             Serial1.end();             
             Serial1.begin(BAUDRATE_BOOM);           
             #ifdef BOOMDATADEBUG 
             Serial.println("\n Boom data timeout. Restarting serial port 1");            
             Serial.printf("\nBoom last transmit: %i", millis()-lastReceivedBoomTime);
             #endif
             }
      #endif
      
      looptime=millis();
      loopcount=0;
      charsreceived=0;
      SensorWriteSD();     
      }

}  // loop




void readSerialData()
{

#ifdef SERIALEFISDATA
//read EFIS data
 while (Serial3.available()>0)
      {       
      efis_inChar=Serial3.read();
      lastReceivedEfisTime=millis(); 
      charsreceived++;      
      if  (efisBufferString.length()>=230) efisBufferString=""; // prevent buffer overflow;     
      if ((efisBufferString.length()>0 || last_efis_inChar== char(0x0A)))  // data line terminats with 0D0A, when buffer is empty look for 0A in the incoming stream and dump everything else
          {            
          efisBufferString+=efis_inChar;                               
          if (efis_inChar == char(0x0A))
                      {
                      // end of line
#ifdef EFISTYPE_ADVANCED                 
                      if (efisBufferString.length()==74 && efisBufferString[0]=='!' && efisBufferString[1]=='1')
                         {
                         // parse Skyview AHRS data
                         efisType="SKYVIEW";
                         String parseString;
                         //calculate CRC
                         int calcCRC=0;
                         for (int i=0;i<=69;i++) calcCRC+=efisBufferString[i];                     
                         calcCRC=calcCRC & 0xFF;
                         if (calcCRC==(int)strtol(&efisBufferString.substring(70, 72)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                            {
                             //float efisASI                            
                             parseString=efisBufferString.substring(23, 27); 
                             yield();
                             noInterrupts();
                             if (parseString!="XXXX") efisASI=parseString.toFloat()/10; else efisASI=-1; // knots
                             interrupts();
                             //float efisPitch                             
                             parseString=efisBufferString.substring(11, 15);
                             yield();
                             noInterrupts();                             
                             if (parseString!="XXXX") efisPitch=parseString.toFloat()/10; else efisPitch=-100; // degrees
                             interrupts();
                             // float efisRoll
                             parseString=efisBufferString.substring(15, 20);
                             yield();
                             noInterrupts();                             
                             if (parseString!="XXXXX") efisRoll=parseString.toFloat()/10; else efisRoll=-100; // degrees
                             interrupts();
                            // float MagneticHeading                                   
                             parseString=efisBufferString.substring(20, 23);
                             yield();
                             noInterrupts();                             
                             if (parseString!="XXX") efisHeading=parseString.toInt(); else efisHeading=-1;                             
                             interrupts();
                             // float efisLateralG
                             parseString=efisBufferString.substring(37, 40);
                             yield();
                             noInterrupts();
                             if (parseString!="XXX") efisLateralG=parseString.toFloat()/100; else efisLateralG=-100;
                             interrupts();
                             // float efisVerticalG                                   
                             parseString=efisBufferString.substring(40, 43);
                             yield();
                             noInterrupts();
                             if (parseString!="XXX") efisVerticalG=parseString.toFloat()/10; else efisVerticalG=-100;
                             interrupts();
                             // int efisPercentLift                 
                             parseString=efisBufferString.substring(43, 45);
                             yield();
                             noInterrupts();
                             if (parseString!="XX") efisPercentLift=parseString.toInt(); else efisPercentLift=-1; // 00 to 99, percentage of stall angle.
                             interrupts();
                             // int efisPalt
                             parseString=efisBufferString.substring(27, 33); 
                             yield();
                             noInterrupts();
                             if (parseString!="XXXXXX") efisPalt=parseString.toInt(); else efisPalt=-10000; // feet
                             interrupts();
                             // int efisVSI                 
                             parseString=efisBufferString.substring(45, 49); 
                             yield();
                             noInterrupts();
                             if (parseString!="XXXX") efisVSI=parseString.toInt()*10; else efisVSI=-10000; // feet/min 
                             interrupts();
                             //float efisTAS;
                             parseString=efisBufferString.substring(52, 56);
                             yield();
                             noInterrupts();
                             if (parseString!="XXXX") efisTAS=parseString.toFloat()/10; else efisTAS=-1; // kts             
                             interrupts();
                             //float efisOAT;
                             parseString=efisBufferString.substring(49, 52);
                             yield();
                             noInterrupts();
                             if (parseString!="XXX") efisOAT=parseString.toFloat(); else efisTAS=-100; // Celsius
                             interrupts();                             
                             // String efisTime
                             yield();
                             noInterrupts();
                             efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);                             
                             efisTimestamp=millis();
                             interrupts();                             
                             #ifdef EFISDATADEBUG
                             Serial.printf("SKYVIEW ADAHRS: efisASI %.2f, efisPitch %.2f, efisRoll %.2f, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i, efisTAS %.2f, efisOAT %.2f, efisHeading %i ,efisTime %s\n", efisASI, efisPitch, efisRoll, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisHeading, efisTime.c_str());                        
                             #endif
                             #ifdef SDCARDDEBUG
                             Serial.print(".");
                             #endif
                                                                            
                            } 
                            #ifdef EFISDATADEBUG
                            else Serial.println("SKYVIEW ADAHRS CRC Failed");
                            #endif
                            
                         } else
                         
                         if (efisBufferString.length()==225 && efisBufferString[0]=='!' && efisBufferString[1]=='3')
                                 {
                                 // parse Skyview EMS data
                                 efisType="SKYVIEW";
                                 String parseString;
                                 //calculate CRC
                                 int calcCRC=0;
                                 for (int i=0;i<=220;i++) calcCRC+=efisBufferString[i];
                                 calcCRC=calcCRC & 0xFF;
                                 if (calcCRC==(int)strtol(&efisBufferString.substring(221, 223)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                    {
                                   
                                    //float efisFuelRemaining=0.00;
                                     parseString=efisBufferString.substring(44, 47);
                                     yield();
                                     noInterrupts();
                                     if (parseString!="XXX") efisFuelRemaining=parseString.toFloat()/10; else efisFuelRemaining=-1; // gallons
                                     interrupts();
                                     //float efisFuelFlow=0.00;
                                     parseString=efisBufferString.substring(29, 32);
                                     yield();
                                     noInterrupts();
                                     if (parseString!="XXX") efisFuelFlow=parseString.toFloat()/10; else efisFuelFlow=-1; // gph
                                     interrupts();
                                     //float efisMAP=0.00;
                                     parseString=efisBufferString.substring(26, 29);
                                     yield();
                                     noInterrupts();
                                     if (parseString!="XXX") efisMAP=parseString.toFloat()/10; else efisMAP=-1; //inHg
                                     interrupts();
                                     // int efisRPM=0;
                                     parseString=efisBufferString.substring(18, 22);
                                     yield();
                                     noInterrupts();
                                     if (parseString!="XXXX") efisRPM=parseString.toInt(); else efisRPM=-1;
                                     interrupts();
                                     // int efisPercentPower=0;                                     
                                     parseString=efisBufferString.substring(217, 220);
                                     yield();
                                     noInterrupts();                                     
                                     if (parseString!="XXX") efisPercentPower=parseString.toInt(); else efisPercentPower=-1;                                                            
                                     interrupts();
                                     #ifdef EFISDATADEBUG
                                     Serial.printf("SKYVIEW EMS: efisFuelRemaining %.2f, efisFuelFlow %.2f, efisMAP %.2f, efisRPM %i, efisPercentPower %i\n", efisFuelRemaining, efisFuelFlow, efisMAP, efisRPM, efisPercentPower);                                     
                                     #endif
                                     #ifdef SDCARDDEBUG
                                    Serial.print(".");
                                    #endif
                                                         
                                    } 
                                    #ifdef EFISDATADEBUG
                                    else Serial.println("SKYVIEW EMS CRC Failed");
                                    #endif
                                    
                                 
                                 }
#endif
#ifdef EFISTYPE_DYNOND10                          
                                 
                         
                                if (efisBufferString.length()==DYNON_SERIAL_LEN)
                                    {
                                    // parse Dynon data
                                     efisType="DYNOND10";
                                     String parseString;
                                     //calculate CRC
                                     int calcCRC=0;
                                     for (int i=0;i<=48;i++) calcCRC+=efisBufferString[i];                     
                                     calcCRC=calcCRC & 0xFF;
                                     if (calcCRC==(int)strtol(&efisBufferString.substring(49, 51)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                        {
                                        // CRC passed
                                         parseString=efisBufferString.substring(20, 24);                                                                                                      
                                         yield();
                                         noInterrupts();
                                         efisASI=parseString.toFloat()/10*1.94384; // m/s to knots
                                         interrupts();                                         
                                         parseString=efisBufferString.substring(8, 12);
                                         yield();
                                         noInterrupts();
                                         efisPitch=parseString.toFloat()/10;
                                         interrupts();                                         
                                         parseString=efisBufferString.substring(12, 17);
                                         yield();
                                         noInterrupts();
                                         efisRoll=parseString.toFloat()/10;
                                         interrupts();                                         
                                         parseString=efisBufferString.substring(33, 36);
                                         yield();
                                         noInterrupts();
                                         efisLateralG=parseString.toFloat()/100;
                                         interrupts();                                         
                                         parseString=efisBufferString.substring(36, 39);
                                         yield();
                                         noInterrupts();
                                         efisVerticalG=parseString.toFloat()/10;
                                         interrupts();                                         
                                         parseString=efisBufferString.substring(39, 41);
                                         yield();
                                         noInterrupts();
                                         efisPercentLift=parseString.toInt(); // 00 to 99, percentage of stall angle
                                         interrupts();  
                                         parseString=efisBufferString.substring(45,47);
                                         long statusBitInt = strtol(&parseString[1], NULL, 16);                                                                                 
                                         if (bitRead(statusBitInt, 0))
                                              {
                                              // when bitmask bit 0 is 1, grab pressure altitude and VSI, otherwise use previous value (skip turn rate and density altitude)
                                              parseString=efisBufferString.substring(24, 29);
                                              yield();
                                              noInterrupts();
                                              efisPalt=parseString.toInt()*3.28084; // meters to feet
                                              interrupts();
                                              parseString=efisBufferString.substring(29, 33);
                                              yield();
                                              noInterrupts();
                                              efisVSI= int(parseString.toFloat()/10*60); // feet/sec to feet/min
                                              interrupts();
                                              }                                                               
                                         efisTimestamp=millis();
                                         yield();
                                         noInterrupts();
                                         efisTime=efisBufferString.substring(0, 2)+":"+efisBufferString.substring(2, 4)+":"+efisBufferString.substring(4, 6)+"."+efisBufferString.substring(6, 8);                                        
                                         interrupts();
                                         #ifdef EFISDATADEBUG                     
                                         Serial.printf("D10: efisASI %.2f, efisPitch %.2f, efisRoll %.2f, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i, efisTime %s\n", efisASI, efisPitch, efisRoll, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTime.c_str());                                                                 
                                         #endif
                                         #ifdef SDCARDDEBUG
                                         Serial.print(".");
                                         #endif                                         
                                        }
                                        #ifdef EFISDATADEBUG
                                        else Serial.println("D10 CRC Failed");
                                        #endif
                             
                                      }
#endif
#ifdef EFISTYPE_GARMING5                                    
                                      
                                      if (efisBufferString.length()==59 && efisBufferString[0]=='=' && efisBufferString[1]=='1' && efisBufferString[2]=='1')
                                                 {
                                                 // parse G5 data
                                                 efisType="GARMING5";
                                                 String parseString;
                                                 //calculate CRC
                                                 int calcCRC=0;                           
                                                 for (int i=0;i<=54;i++) calcCRC+=efisBufferString[i];                     
                                                 calcCRC=calcCRC & 0xFF;
                                                 if (calcCRC==(int)strtol(&efisBufferString.substring(55, 57)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                                    {
                                                      // CRC passed                                                                                                          
                                                     parseString=efisBufferString.substring(23, 27);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="____") efisASI=parseString.toFloat()/10;
                                                     interrupts();
                                                     parseString=efisBufferString.substring(11, 15);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="____") efisPitch=parseString.toFloat()/10;
                                                     interrupts();
                                                     parseString=efisBufferString.substring(15, 20);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="_____") efisRoll=parseString.toFloat()/10;                                                    
                                                     interrupts();
                                                     parseString=efisBufferString.substring(20, 23);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="___") efisHeading=parseString.toInt();
                                                     interrupts();                                                     
                                                     parseString=efisBufferString.substring(37, 40);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="___") efisLateralG=parseString.toFloat()/100;                            
                                                     interrupts();
                                                     parseString=efisBufferString.substring(40, 43);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="___") efisVerticalG=parseString.toFloat()/10;
                                                     interrupts();
                                                     parseString=efisBufferString.substring(27, 33);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="______") efisPalt=parseString.toInt(); // feet
                                                     interrupts();
                                                     parseString=efisBufferString.substring(45, 49);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="____") efisVSI=parseString.toInt()*10; //10 fpm
                                                     efisTimestamp=millis();
                                                     efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);   
                                                     interrupts();                                                   
                                                     #ifdef EFISDATADEBUG
                                                     Serial.printf("G5 data: efisASI %.2f, efisPitch %.2f, efisRoll %.2f, efisHeading %i, efisLateralG %.2f, efisVerticalG %.2f, efisPalt %i, efisVSI %i,efisTime %s", efisASI, efisPitch, efisRoll, efisHeading, efisLateralG, efisVerticalG,efisPalt,efisVSI,efisTime);                        
                                                     Serial.println();
                                                     #endif
                                                     #ifdef SDCARDDEBUG
                                                     Serial.print(".");
                                                     #endif                                                      
                                                     }
                                                     #ifdef EFISDATADEBUG
                                                     else Serial.println("G5 CRC Failed");
                                                     #endif                                                                             
                                                  }
  #endif 
#ifdef EFISTYPE_GARMING3X                                    
                                      // parse G3X attitude data, 10hz
                                      if (efisBufferString.length()==59 && efisBufferString[0]=='=' && efisBufferString[1]=='1' && efisBufferString[2]=='1')
                                                 {
                                                 // parse G3X data
                                                 efisType="GARMING3X";
                                                 String parseString;
                                                 //calculate CRC
                                                 int calcCRC=0;                           
                                                 for (int i=0;i<=54;i++) calcCRC+=efisBufferString[i];                     
                                                 calcCRC=calcCRC & 0xFF;
                                                 if (calcCRC==(int)strtol(&efisBufferString.substring(55, 57)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                                    {
                                                    // CRC passed                                                                                                          
                                                     parseString=efisBufferString.substring(23, 27);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="____") efisASI=parseString.toFloat()/10;
                                                     interrupts();
                                                     parseString=efisBufferString.substring(11, 15);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="____") efisPitch=parseString.toFloat()/10;
                                                     interrupts();
                                                     parseString=efisBufferString.substring(15, 20);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="_____") efisRoll=parseString.toFloat()/10;                                                    
                                                     interrupts();
                                                     parseString=efisBufferString.substring(20, 23);
                                                     yield();
                                                     noInterrupts();                                                     
                                                     if (parseString!="___") efisHeading=parseString.toInt();
                                                     interrupts();                                                     
                                                     parseString=efisBufferString.substring(37, 40);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="___") efisLateralG=parseString.toFloat()/100;                            
                                                     interrupts();
                                                     parseString=efisBufferString.substring(40, 43);
                                                     yield();
                                                     noInterrupts();                                                     
                                                     if (parseString!="___") efisVerticalG=parseString.toFloat()/10;
                                                     interrupts();
                                                     parseString=efisBufferString.substring(43, 45);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="__") efisPercentLift=parseString.toInt();
                                                     interrupts();
                                                     parseString=efisBufferString.substring(27, 33);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="______") efisPalt=parseString.toInt(); // feet
                                                     interrupts();
                                                     parseString=efisBufferString.substring(45, 49);
                                                     yield();
                                                     noInterrupts();
                                                     if (parseString!="____") efisVSI=parseString.toInt()*10; //10 fpm                                                     
                                                     efisTimestamp=millis();                                                       
                                                     efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);                                        
                                                     interrupts();                                                   
                                                     #ifdef EFISDATADEBUG
                                                     Serial.printf("G3X Attitude data: efisASI %.2f, efisPitch %.2f, efisRoll %.2f, efisHeading %i, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i,efisTime %s", efisASI, efisPitch, efisRoll, efisHeading, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTime);                        
                                                     Serial.println();
                                                     #endif
                                                     #ifdef SDCARDDEBUG
                                                     Serial.print(".");
                                                     #endif                                                      
                                                     }
                                                     #ifdef EFISDATADEBUG
                                                     else Serial.println("G3X Attitude CRC Failed");
                                                     #endif                                                                             
                                                  }
                                           // parse G3X engine data, 5Hz
                                      if (efisBufferString.length()==221 && efisBufferString[0]=='=' && efisBufferString[1]=='3' && efisBufferString[2]=='1') 
                                      {                                      
                                                 efisType="GARMING3X";
                                                 String parseString;
                                                 //calculate CRC
                                                 int calcCRC=0;                           
                                                 for (int i=0;i<=216;i++) calcCRC+=efisBufferString[i];
                                                 calcCRC=calcCRC & 0xFF;
                                                 if (calcCRC==(int)strtol(&efisBufferString.substring(219, 221)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                                    { 
                                                    //float efisFuelRemaining=0.00;
                                                    parseString=efisBufferString.substring(44, 47);
                                                    noInterrupts();
                                                    if (parseString!="___") efisFuelRemaining=parseString.toFloat()/10;
                                                    interrupts(); 
                                                    parseString=efisBufferString.substring(29, 32);
                                                    noInterrupts();
                                                    if (parseString!="___") efisFuelFlow=parseString.toFloat()/10;                                                    
                                                    interrupts();
                                                    parseString=efisBufferString.substring(26, 29);
                                                    noInterrupts();
                                                    if (parseString!="___") efisMAP=parseString.toFloat()/10;
                                                    interrupts(); 
                                                    parseString=efisBufferString.substring(18, 22);
                                                    noInterrupts();
                                                    if (parseString!="____") efisRPM==parseString.toInt();                                                    
                                                    interrupts(); 
                                                    #ifdef EFISDATADEBUG
                                                    Serial.printf("G3X EMS: efisFuelRemaining %.2f, efisFuelFlow %.2f, efisMAP %.2f, efisRPM %i\n", efisFuelRemaining, efisFuelFlow, efisMAP, efisRPM);                                     
                                                    #endif
                                                    #ifdef SDCARDDEBUG
                                                    Serial.print(".");
                                                    #endif
                                                    }
                                                     #ifdef EFISDATADEBUG
                                                     else Serial.println("G3X EMS CRC Failed");
                                                     #endif                                     
                                      }
                                                  
  #endif 
                                                 
                      efisBufferString="";  // reset buffer                                               
                      }
        } // 0x0A first
         #ifdef EFISDATADEBUG
         else
              {
              // show dropped characters  
              Serial.print("@");
              Serial.print(efis_inChar);
              }         
         #endif // efisdatadebug
        last_efis_inChar=efis_inChar;
      }

      
#endif

#ifdef BOOM
// look for serial command
while (Serial1.available())
  {
  serialBoomChar = Serial1.read();
  lastReceivedBoomTime=millis();
  if (serialBufferSize>=50) serialBufferSize=0; // prevent serial buffer overflow
              
  if (serialBoomChar!='$' && serialBoomChar!=char(0x0D) && serialBoomChar!=char(0x0A) && serialBufferSize<50 && serialBufferSize>0 && serialBuffer[0]=='$')
    {    
    serialBuffer[serialBufferSize]=serialBoomChar;
    serialBufferSize++;
    } else
          {
           if (serialBoomChar=='$')
              {
              // start of new line, drop the buffer
              serialBufferSize=0;
              serialBuffer[serialBufferSize]=serialBoomChar; // add the $ sign
              serialBufferSize++;
              } else
            if (serialBoomChar==char(0x0A))
            {
            // we have the full line, verify and parse the data   
            //serialString="";
            //String serialString(serialBuffer);
            // verify if line starting with $AIRDAQ
            //if (serialString.startsWith("$AIRDAQ,64.12.B8,ADC,"))
            if (strstr(serialBuffer, "$AIRDAQ,64.12.B8,ADC,"))
              {
              // parse data
              parse_array_index=0;
              parseBufferSize=0;
              for (int k=21;k<serialBufferSize;k++)
                  {                    
                  if (serialBuffer[k]!=',' && k != (serialBufferSize-1) && parse_array_index<4)
                     {
                     // add character to parsed buffer
                     parseBuffer[parseBufferSize]=serialBuffer[k];
                     parseBufferSize++;                                     
                     } else
                     {
                     //store parsed buffer in array and drop buffer                     
                     parse_array[parse_array_index]=atoi(parseBuffer);                        
                     parse_array_index++;
                     //Serial.print(atoi(parseBuffer));
                     //Serial.print(":");                                              
                     parseBufferSize=0;
                     }
                  }
             // if we have all 4 values, register them in their variables
             if (parse_array_index==4 && parse_array[0]<=16383 && parse_array[1]<=16383 && parse_array[2]<=16383 && parse_array[3]<=16383 && parse_array[0]>0 && parse_array[1]>0 && parse_array[2]>0 && parse_array[3]>0)
                {
                 noInterrupts();
                 boomStatic=BOOM_STATIC_CALC(parse_array[0]);
                 boomDynamic=BOOM_DYNAMIC_CALC(parse_array[1]);
                 boomAlpha=BOOM_ALPHA_CALC(parse_array[2]);
                 boomBeta=BOOM_BETA_CALC(parse_array[3]);                 
                 boomTimestamp=millis();
                 if (boomDynamic*100/1.225*2>0) boomIAS=sqrt(boomDynamic*100/1.225*2)* 1.94384; else boomIAS=0;
                 interrupts();
                 
                 
                 #ifdef BOOMDATADEBUG                 
                 Serial.printf("BOOM: boomStatic %.2f, boomDynamic %.2f, boomAlpha %.2f, boomBeta %.2f, boomIAS %.2f", boomStatic, boomDynamic, boomAlpha, boomBeta, boomIAS);                        
                 Serial.println();               
                 #endif
                }
             }
            
            // drop buffer after parsing
            serialBufferSize=0;
            }
          }
  }
#endif
            
}

void setFrequencytone(uint32_t frequency)
{
 float volume=0;
 noInterrupts();
 float pulse_delay= 1000/pps; 
 interrupts();
 float tone_length=pulse_delay-3; // tone_length must be below 100%, otherwise envelopes overlap

if (highTone && tonePlaying==SOLID_TONE)
    {
    // shorter delay when transitioning from solid to high tone
    ToneTimer.end();
    ToneTimer.begin(tonePlayHandler,(pulse_delay/2+1000/LOW_TONE_PPS_MAX/2)*1000); // create half the time of the original waveform + the half of LOW_TONE_PPS_MAX = shorter transition to high tone, better response rate
    } else
      {
      ToneTimer.begin(tonePlayHandler,pulse_delay*1000);
      }
// Serial.printf("millis: %i, freq update: %i, pps: %0.2f\n",millis(),(int)(pulse_delay*1000),pps);
#ifdef SHOW_SERIAL_DEBUG
  Serial.print(millis());
  Serial.print(", AOA: ");
  Serial.print(AOA);
  Serial.print(", IAS: ");
  Serial.print(ASI);   
  Serial.print(", pulse delay (ms):");
  Serial.print(pulse_delay);
  Serial.print(", pps: ");
  Serial.print(pps);
  Serial.print(", freq: ");
  Serial.print(frequency);
  Serial.print(", switchState:");
  Serial.print(switchState);
  Serial.print(", toneMode:");
  Serial.println(toneMode);
#endif

#ifdef AUDIO_3D  // if 3D audio is enabled modify the gains of each channel to follow slip ball
if (abs(efisLateralG)>=0.005)
    {
    float channelGain=AUDIO_3D_CURVE(efisLateralG); //-0.184*log(efisLateralG) - 0.2816;
    if (efisLateralG<0)
       {
       // left ball deflection

       AudioNoInterrupts();
       ampLeft.gain(1+(1-channelGain));
       ampRight.gain(channelGain);
       AudioInterrupts();  
       #ifdef AUDIO_3D_DEBUG
       Serial.printf("%0.3fG, Left: %0.3f, Right: %0.3f\n",efisLateralG,1+(1-channelGain),channelGain);
       #endif     
       } else
            {
            // right ball deflection  
            AudioNoInterrupts();
            ampRight.gain(1+(1-channelGain));
            ampLeft.gain(channelGain);
            AudioInterrupts();
            #ifdef AUDIO_3D_DEBUG  
            Serial.printf("%0.3fG, Left: %0.3f, Right: %0.3f\n",efisLateralG,channelGain,1+(1-channelGain));
            #endif
            }
    }  else
        {
         AudioNoInterrupts(); 
         ampLeft.gain(1.0);
         ampRight.gain(1.0);         
         AudioInterrupts();
         #ifdef AUDIO_3D_DEBUG  
         Serial.printf("%0.3fG, Left: %0.3f, Right: %0.3f\n",efisLateralG,1,1);
         #endif
        }
#endif // AUDIO_3D



 
 if (!switchState)
    {
    // if audio switch is off don't play any tones
    AudioNoInterrupts();
    sinewave1.amplitude(volume);
    AudioInterrupts();
    tonePlaying=TONE_OFF;
    return;
    }
    
  if(frequency < 20 || frequency > 20000 || toneMode == TONE_OFF) {
    #ifdef SHOW_SERIAL_DEBUG
    Serial.print("cancel tone: ");Serial.println(frequency);   
    #endif
    AudioNoInterrupts();
    sinewave1.amplitude(volume); // turn off pulsed tone
    AudioInterrupts();
    toneFreq = frequency;
    tonePlaying=TONE_OFF;
    return;
  } 
//Serial.println(pps);  
 if (toneMode==SOLID_TONE)
      {
      volume=SOLID_TONE_VOLUME;      
      if (tonePlaying!=SOLID_TONE) // if solid tone not already playing
          {
          AudioNoInterrupts();
          envelope1.noteOff(); // turn off previous note
          sinewave1.frequency(frequency);
          sinewave1.amplitude(volume);
          AudioInterrupts();
          
          AudioNoInterrupts();          
          envelope1.delay(1000/LOW_TONE_PPS_MAX/2); // this timing provides a smooth transition from low tones into solid and a quick transition from high tones back into to solid
          envelope1.attack(TONE_RAMP_TIME);
          envelope1.hold(0);
          envelope1.decay(0); 
          envelope1.sustain(1);
          envelope1.release(TONE_RAMP_TIME);
          envelope1.releaseNoteOn(0);
          envelope1.noteOn();
          AudioInterrupts();
          
          tonePlaying=SOLID_TONE;
          }
      
      } else
      {
      AudioNoInterrupts();      
      sinewave1.frequency(frequency);
      AudioInterrupts();
      // set tone volume
      if (!highTone)
                    {
                    // low tone
                    volume=LOW_TONE_VOLUME;                    
                    }
                    else if (highTone)
                                      {
                                      // high tone                                        
                                      if (pps==HIGH_TONE_STALL_PPS) volume=HIGH_TONE_VOLUME_MAX; else volume=constrain(mapfloat(pps,HIGH_TONE_PPS_MIN,HIGH_TONE_PPS_MAX,HIGH_TONE_VOLUME_MIN,HIGH_TONE_VOLUME_MAX),HIGH_TONE_VOLUME_MIN,HIGH_TONE_VOLUME_MAX);                                                                                                                      
                                      }                                                                                                                                                                                                                                    
      
      AudioNoInterrupts();
      envelope1.noteOff(); // turn off previous note
      AudioInterrupts();
      
      AudioNoInterrupts();      
      sinewave1.amplitude(volume);      
      // transition quicker from solid to a high tone
      if (highTone && tonePlaying==SOLID_TONE)
          {
          // shorten the delay on the first high tone  
          envelope1.delay((1000/LOW_TONE_PPS_MAX)/2);
          }
          else 
            {
            envelope1.delay(tone_length/2);
            }
      
      if (highTone && pps==HIGH_TONE_STALL_PPS)
          {
          // change tone ramp time on stall tone
          envelope1.attack(STALL_RAMP_TIME);
          envelope1.hold(tone_length/2-STALL_RAMP_TIME*2);
          envelope1.decay(STALL_RAMP_TIME);
          }
          else
            {
            envelope1.attack(TONE_RAMP_TIME);
            envelope1.hold(tone_length/2-TONE_RAMP_TIME*2);
            envelope1.decay(TONE_RAMP_TIME);
            }
      envelope1.sustain(0);
      envelope1.releaseNoteOn(0);
      envelope1.noteOn();
      AudioInterrupts();
      tonePlaying=PULSE_TONE;
      }     
  toneFreq = frequency;
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
return -1;
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
 return -1;    
}

void SensorRead()
{ 
//unsigned long sensorstarttime=micros();  
// reads sensors 50 times/second
int Pfwd;
float PfwdPascal;
float Pstatic=0.00;
int P45;
float PfwdSmoothed;
float P45Smoothed;
Pfwd=GetPressurePfwd()-PFWD_BIAS;
P45=GetPressureP45()-P45_BIAS;
#ifdef BARO
Pstatic=GetStaticPressure();
Palt=145366*(1-pow(Pstatic/1013.2,0.190284));
#endif
#ifdef IMU
readAccel(); // read accelerometers
#endif

flapsIndex=getFlapsIndex();
flapsPos=flapDegrees[flapsIndex];
setAOApoints(flapsIndex);

PfwdMedian.add(Pfwd);
PfwdSmoothed=PfwdMedian.getMedian();

P45Median.add(P45);
P45Smoothed=P45Median.getMedian();

calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd divided by non-bias-corrected P45;

// calculate airspeed
PfwdPascal=((PfwdSmoothed+PFWD_BIAS - 0.1*16383) * 2/(0.8*16383) -1) * 6894.757;
if (PfwdPascal>0)
    {
    ASI=sqrt(2*PfwdPascal/1.225)* 1.94384; // knots // physics based calculation
    ASI=CAS_CURVE(ASI);   
    }
    else ASI=0;
unsigned long timeStamp=millis(); // save timestamp for logging
updateTones();

#ifdef SENSORDEBUG
    char debugSensorBuffer[500];
    #ifdef BOOM
    Serial.println("timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos,boomStatic,boomDynamic,boomAlpha,boomBeta");
    sprintf(debugSensorBuffer, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.2f,%.2f,%.2f,%.2f\n",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos,boomStatic,boomDynamic,boomAlpha,boomBeta);    
    #else
    Serial.println("timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos,Gx,Gy,Gz");
    sprintf(debugSensorBuffer, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.2f,%.2f,%.2f\n",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos,calcAccel(ax),calcAccel(ay),calcAccel(az));
    #endif   
    Serial.println(debugSensorBuffer);
#endif    

#ifdef SDCARD
char logBufSensor[2048];  // char buffer for sensor log line
char logLineEnd[3];

    if (sdLogging)
        {                     
        sprintf(logBufSensor, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos);
        #ifdef BOOM
        char logBufBoom[100];
        int boomAge=millis()-boomTimestamp;
        sprintf(logBufBoom, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge);
        #endif

        #ifdef IMU
        char logBufImu[50];
        sprintf(logBufImu, ",%.3f,%.3f,%.3f",calcAccel(ax),calcAccel(ay),calcAccel(az)); // calcAccel()
        #endif

        #ifdef SERIALEFISDATA
        char logBufEfis[256];
        int efisAge=millis()-efisTimestamp;
        sprintf(logBufEfis, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i, %.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%i,%s",efisASI,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisHeading,efisAge,efisTime.c_str()); 
        #endif

        sprintf(logLineEnd,"\n");                             

        //copy sensor data to sensorCache, turn off interrupts
        //Serial.print("<");
        //for (k=0;k<strlen(logBufSensor);k++)
        //  {
        //  sensorCache[sensorCacheCount]=logBufSensor[k];
        //  sensorCacheCount++;          
        //  }
        memcpy(sensorCache+sensorCacheCount,logBufSensor, strlen(logBufSensor));
        sensorCacheCount=sensorCacheCount+strlen(logBufSensor);
        
        #ifdef IMU
        //copy IMU data to sensorCache
        memcpy(sensorCache+sensorCacheCount,logBufImu, strlen(logBufImu));
        sensorCacheCount=sensorCacheCount+strlen(logBufImu);
        #endif 
         
        #ifdef BOOM
        //copy boom data to sensorCache
        memcpy(sensorCache+sensorCacheCount,logBufBoom, strlen(logBufBoom));
        sensorCacheCount=sensorCacheCount+strlen(logBufBoom);  
         #endif 

         #ifdef SERIALEFISDATA
        memcpy(sensorCache+sensorCacheCount,logBufEfis, strlen(logBufEfis));
        sensorCacheCount=sensorCacheCount+strlen(logBufEfis);
         #endif  
                
        memcpy(sensorCache+sensorCacheCount,logLineEnd, strlen(logLineEnd));
        sensorCacheCount=sensorCacheCount+strlen(logLineEnd);        
         cachelinecount++; 
        //Serial.print(">");
         //Serial.println(cachelinecount);
        }
  #endif

  
  //Serial.println(sensorCacheCount);
 // breathing LED
if (switchState)
    {
    float ledBrightness = 15+(exp(sin(millis()/2000.0*PI)) - 0.36787944)*63.81; // funky sine wave, https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    analogWrite(PIN_LED2, ledBrightness);
    } else analogWrite(PIN_LED2,0);
#ifdef VOLUMECONTROL
readVolume(); // update volume control
#endif
//Serial.printf("SS: %i\n",micros()-sensorstarttime);

}

void SensorWriteSD()
{
//Serial.println();Serial.print("loopcount: "); Serial.println(loopcount);
#ifdef SDCARD
    
    if (sdLogging && sensorCacheCount>0)
        {
        
        #ifdef SDCARDDEBUG
        Serial.println();
        int writeCount=0;
        unsigned long maxWriteTime=0;
        unsigned long avgWriteTime=0;
        unsigned long minWriteTime=1000000;
        unsigned long openTime;
        unsigned long timerstart=micros();
        unsigned long sdStart=timerstart;
        unsigned long closeTime;
        #endif
        
        //move sensor cache into write cache
        noInterrupts();
        memcpy(SDwriteCache,sensorCache,sensorCacheCount);
        SDwriteCacheCount=sensorCacheCount;                                         
        sensorCacheCount=0; // reset sensor log cache        
        interrupts();
        cachelinecount=0;   // debug variable
        // Serial.println("start OpenFile");
            #ifdef SDCARDDEBUG
            timerstart=micros();
            #endif
            SensorFile = SD.open(filenameSensor, FILE_WRITE);
            #ifdef SDCARDDEBUG
            openTime=micros()-timerstart;
            #endif
            if (SensorFile)
                {                  
                int SDcacheIndex=0;
                int toWriteCount=0;
                while (SDcacheIndex<SDwriteCacheCount)
                    {
                    if (SDwriteCacheCount-SDcacheIndex>512)
                        {
                        toWriteCount=512;                           
                        }
                        else 
                            {
                            toWriteCount=SDwriteCacheCount-SDcacheIndex;
                            }
                    #ifdef SDCARDDEBUG
                    timerstart=micros();
                    #endif                    
                    SensorFile.write(SDwriteCache+SDcacheIndex,toWriteCount);
                    #ifdef SDCARDDEBUG
                    maxWriteTime=max(maxWriteTime,micros()-timerstart);
                    minWriteTime=min(minWriteTime,micros()-timerstart);
                    avgWriteTime=avgWriteTime+micros()-timerstart;
                    writeCount++;                    
                    #endif
                    //Serial.printf(" %i,",micros()-timerstart);
                    //timerstart=micros();
                    //Serial.printf("serial available: %i \n", Serial3.available()); 
                    SDcacheIndex=SDcacheIndex+toWriteCount;
                    readSerialData();
                    }
               
                #ifdef SDCARDDEBUG
                avgWriteTime= round(avgWriteTime/writeCount);                
                timerstart=micros();
                #endif
                SensorFile.close();
                #ifdef SDCARDDEBUG
                closeTime=micros()-timerstart;
                unsigned long totalTime=micros()-sdStart;                
                Serial.printf("Open: %i, Max: %i, Min: %i, Avg: %i, Close: %i, Total: %i",openTime,maxWriteTime, minWriteTime, avgWriteTime, closeTime, totalTime);
                #endif
                }
                //#ifdef SHOW_SERIAL_DEBUG
                else
                  {                  
                  Serial.print("cannot write log file");                                  
                  }
                //#endif    
           
            #ifdef SDCARDDEBUG                                  
            Serial.println();
            Serial.print(SDwriteCacheCount);Serial.println(" bytes.");
            SDwriteCacheCount=0; // reset write chache
            #endif                                  
            
        #ifdef SHOW_SERIAL_DEBUG 
        Serial.print("free memory: ");Serial.print(freeMemory());
        Serial.print(",SDcache waiting: ");Serial.println(sensorCacheCount);
         #endif                
        }     
  #endif
}

void PotRead()
{
float testAOA=getTestAOA();
AOA=testAOA;
//AOAAvg+=testAOA;
//AOA=AOAAvg.process().mean;
setAOApoints(2); // flaps down
ASI=50; // to turn on the tones
updateTones();
//Serial.printf("TestAOA: %0.2f, AOA: %0.2f\n",testAOA, AOA);
}


void RangeSweep()
{

if (rangeSweepDirection>0)
    {
    if (currentRangeSweepValue<RANGESWEEP_HIGH_AOA) currentRangeSweepValue+=RANGESWEEP_STEP; else if (currentRangeSweepValue>=RANGESWEEP_HIGH_AOA) rangeSweepDirection=-1;
    } else
          {
           if (currentRangeSweepValue>RANGESWEEP_LOW_AOA) currentRangeSweepValue-=RANGESWEEP_STEP; else if (currentRangeSweepValue<=RANGESWEEP_LOW_AOA) rangeSweepDirection=1;
          }

AOA=currentRangeSweepValue;
setAOApoints(0); // flaps down
ASI=50; // to turn on the tones
updateTones();
}


void switchOnOff()
     {     
     if (!switchState)
        {              
        // turn on
        analogWrite(PIN_LED2,200);
        // play turn on sound        
        voice1.play(AudioSampleEnabled);
        delay (1500);
        switchState=true;
          
  
        Serial.println("Switch on");
        } else
              {              
              // turn off                
              analogWrite(PIN_LED2,0);
              // play turn off sound
              switchState=false;                
              voice1.play(AudioSampleDisabled);
              //delay(1500);
              Serial.println("Switch off");
              }
     }


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
if ((in_max - in_min) + out_min ==0) return 0;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float getTestAOA()
{
float adcvalue=0;
for (int i=0; i<5;i++)
    {
    adcvalue+=analogRead(TESTPOT_PIN);
    }
adcvalue=adcvalue/5;
 return mapfloat(adcvalue, 0, 1024, 0, 25);
}


void LogReplay()
{
char logLine[384];
char inputChar;
int bufferIndex=0;
String valueArray[45];
int valueIndex=0;

int Pfwd;
float PfwdPascal;

int P45;
float PfwdSmoothed;
float P45Smoothed;
#ifdef BARO
float Pstatic=0.00;
#endif
// read next line in logfile and set airspeed and AOA from it

while (SensorFile.available())
      {
      inputChar=SensorFile.read();             
      if (inputChar != char(0xD) && inputChar != char(0xA))
          {
          logLine[bufferIndex]=inputChar;
          bufferIndex++;
          if (bufferIndex==383)
             {
             Serial.println("Buffer overflow while reading log file, skipping to next line");
             return;
             }
          } else
                {
                // end of log line, parse it                 
                    for (int i=0;i<bufferIndex;i++)
                    {
                    if (logLine[i]==',') valueIndex++; else valueArray[valueIndex]+=logLine[i];
                    }
                   if (valueArray[0]=="timeStamp")
                       {
                       Serial.println("Skipping header in logfile."); 
                       return; // skip if log header;
                       }
                  // simulate a sensor read cycle
                  Pfwd=valueArray[1].toInt(); // bias already removed                  
                  PfwdSmoothed=valueArray[2].toFloat();                  
                  P45=valueArray[3].toInt(); // bias already removed
                  P45Smoothed=valueArray[4].toFloat();
                  flapsPos=valueArray[9].toInt();                  
                  for (unsigned int i=0; i < (sizeof(flapDegrees) / sizeof(flapDegrees[0]));i++)
                    {
                    if (flapsPos==flapDegrees[i])
                          {
                          flapsIndex=i;
                          break;
                          }                        
                    }
                  setAOApoints(flapsIndex);                                    
                  calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd divided by non-bias-corrected P45;                

                  // efis lateralG
                  efisLateralG=valueArray[16].toFloat();
                  
                  // calculate airspeed
                  PfwdPascal=((PfwdSmoothed+PFWD_BIAS - 0.1*16383) * 2/(0.8*16383) -1) * 6894.757;
                  if (PfwdPascal>0)
                      {
                      ASI=sqrt(2*PfwdPascal/1.225)* 1.94384; // knots // physics based calculation
                      }
                      else ASI=0;                                                        
                  #ifdef BARO
                  Pstatic=valueArray[6].toFloat();
                  Palt=145366*(1-pow(Pstatic/1013.2,0.190284));
                  #endif

                  #ifdef IMU
                  ax=valueArray[10].toFloat()/aRes;
                  ay=valueArray[11].toFloat()/aRes;
                  az=valueArray[12].toFloat()/aRes;
                  #endif                 
                  
                  updateTones(); // generate tones                
                  Serial.print("Time: ");
                  Serial.print(valueArray[0]);
                  Serial.print(",Pfwd: ");
                  Serial.print(Pfwd);
                  Serial.print(",P45: ");                 
                  Serial.print(P45);
                  Serial.print(",ASI: ");
                  Serial.print(ASI);
                  Serial.print(",AOA: ");
                  Serial.print(AOA);
                  #ifdef BARO
                  Serial.print(",Palt: ");
                  Serial.print(Palt);
                  #endif
                  Serial.print(", tonemode: ");
                  // print tone type & pps
                  if (toneMode==TONE_OFF) Serial.print("TONE_OFF,");
                     else if (toneMode==SOLID_TONE) Serial.print("SOLID_TONE,");
                          else 
                            {
                            Serial.print("tonetype: "); 
                            if (!highTone) Serial.print("LOW_TONE,"); else Serial.print("HIGH_TONE,");
                            Serial.print("tonepulse: ");
                            Serial.print(pps);                                                        
                            Serial.print(",");
                            }
                  Serial.print("Flaps: ");      
                  Serial.print(flapsPos);               
                  Serial.println();                                    
                  return;
                }                          
      }
      // reached end of file, close file and turn off timer
          Serial.println();
          Serial.println("Finished replaying file.");
          SensorFile.close();
          
          LogReplayTimer.end();
          updateTones(); // to turn off tone at the end;
      
}

// IMU functions

void initAccel()
{
 // enable the three axes of the accelerometer
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG5_XL,  0x38);
 // configure the accelerometer-specify bandwidth selection with Abw
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG6_XL, Aodr << 5 | Ascale << 3 | 0x04 | Abw);
}

void readAccel()
{
  uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp 
  I2CreadBytes(_i2cAddress_AccelGyro,OUT_X_L_XL, temp, 6); // Read 6 bytes, beginning at OUT_X_L_XL  
  ax = (temp[1] << 8) | temp[0]; // Store x-axis values into ax
  ay = (temp[3] << 8) | temp[2]; // Store y-axis values into ay
  az = (temp[5] << 8) | temp[4]; // Store z-axis values into az
}

float calcAccel(int16_t accel)
{
  // Return the accel raw reading times our pre-calculated g's / (ADC tick):
  return aRes * accel;
}

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


void calcAOA (float Pfwd, float P45)
{  
float pCoeff;
if (P45!=0)   // can't divide with zero
          {
          pCoeff=Pfwd/(Pfwd-P45);
          } else            
            {
            AOA=-100;  // would result in division by zero. This won't really happen as we use the mid-band of the P45 sensor.
            return;
            }            

    if (flapsPos==flapDegrees[0])
        {
        // use flaps up curve
        AOAAvg+=FLAPSUP_AOA_CURVE(pCoeff);
        AOA=AOAAvg.process().mean;
        if (AOA>AOA_MAX_VALUE) AOA=AOA_MAX_VALUE; else if (AOA<AOA_MIN_VALUE) AOA=AOA_MIN_VALUE;
        } else
              if (flapsPos==flapDegrees[1])
                  {
                  // use flaps middle curve
                  AOAAvg+=FLAPSMIDDLE_AOA_CURVE(pCoeff);
                  AOA=AOAAvg.process().mean;
                  if (AOA>AOA_MAX_VALUE) AOA=AOA_MAX_VALUE; else if (AOA<AOA_MIN_VALUE) AOA=AOA_MIN_VALUE;
                  } else          
                    if (flapsPos==flapDegrees[2])
                        {
                        // use flaps down curve
                        AOAAvg+=FLAPSDOWN_AOA_CURVE(pCoeff);
                        AOA=AOAAvg.process().mean;
                        if (AOA>AOA_MAX_VALUE) AOA=AOA_MAX_VALUE; else if (AOA<AOA_MIN_VALUE) AOA=AOA_MIN_VALUE;
                        } 
}

void SendDisplayData()
{
 char json_buffer[200]; 
 char crc_buffer[100];
 byte CRC=0;
 float gLoad=0.0;
 if (VERTICAL_GLOAD_AXIS[strlen(VERTICAL_GLOAD_AXIS)-1] =='X')
        {
         gLoad=calcAccel(ax);
         }
          else
              if (VERTICAL_GLOAD_AXIS[strlen(VERTICAL_GLOAD_AXIS)] =='Y')
                  {
                   gLoad=calcAccel(ay);
                  }      
                    else {
                          gLoad=calcAccel(az);
                          };
 if (VERTICAL_GLOAD_AXIS[0]=='-') gLoad=gLoad+2;  // reverse accelerometer axis
 // $ONSPEED,AOA,PercentLift,IAS,PAlt,GLoad,CRC 
 float displayAOA;
 float displayPercentLift;
 float percentLiftLDmax;
 float displayPalt;
 int flapsPercent;
  if (isnan(AOA) || ASI<MUTE_AUDIO_UNDER_IAS)
    {
    displayAOA=-100;
    displayPercentLift=-1;
    percentLiftLDmax=0;
    }
    else
        {
        displayAOA=AOA;
        displayPercentLift=AOA* 60/(onSpeedAOAfast+onSpeedAOAslow)/2; // onSpeedAOA (middle of the onspeed band); // scale/align percent lift so Onspeed = 60% lift
        percentLiftLDmax=LDmaxAOA * 60 / (onSpeedAOAfast+onSpeedAOAslow)/2;
        }
 if (isnan(Palt)) displayPalt=-1.00; else displayPalt=Palt;

flapsPercent=(float)flapsPos/(flapDegrees[sizeof(flapDegrees)/sizeof(int)-1]-flapDegrees[0])*100; //flap angle / flap total travel *100 (needed for displaying partial flap donut on display)


 
 sprintf(crc_buffer,"%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.2f",displayAOA,displayPercentLift,ASI,displayPalt,gLoad,flapsPercent,percentLiftLDmax);
 for (unsigned int i=0;i<strlen(crc_buffer);i++) CRC=CRC+char(crc_buffer[i]); // claculate simple CRC
 sprintf(json_buffer, "{\"type\":\"ONSPEED\",\"AOA\":%.2f,\"PercentLift\":%.2f,\"IAS\":%.2f,\"PAlt\":%.2f,\"GLoad\":%.2f,\"FlapsPercent\":%i,\"PercentLiftLDmax\":%.2f,\"CRC\":%i}",displayAOA,displayPercentLift,ASI,displayPalt,gLoad,flapsPercent,percentLiftLDmax,CRC);
 Serial4.println(json_buffer);
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
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

int getFlapsIndex()
{
int pos=analogRead(FLAP_PIN);
// set it to flap zero if there are no multiple positions available
if (sizeof(flapPotPositions) / sizeof(flapPotPositions[0]) ==1)  return 0;

unsigned int i=0; 
int betweenZone=0;
bool reverseOrder=false;

if (flapPotPositions[0]>flapPotPositions[(sizeof(flapPotPositions) / sizeof(flapPotPositions[0]))-1]) reverseOrder=true;

for (i = 1; i < (sizeof(flapPotPositions) / sizeof(flapPotPositions[0])); i++)
    {  
    betweenZone=(flapPotPositions[i]+flapPotPositions[i-1])/2;
    if (!reverseOrder) 
        {                     
        if (pos<=betweenZone)
                            {                            
                            return (i-1);        
                            }
        } else 
          {
          // reverse order
          if (pos>=betweenZone)
                              {
                              return (i-1);
                              }
          }
    }
    return (i-1);
} 

void setAOApoints(int flapIndex)
{
LDmaxAOA=flapLDMAXAOA[flapIndex];
onSpeedAOAfast=flapONSPEEDFASTAOA[flapIndex];
onSpeedAOAslow=flapONSPEEDSLOWAOA[flapIndex];
stallWarningAOA=flapSTALLWARNAOA[flapIndex];
}

#ifdef VOLUMECONTROL
void readVolume()
{
    volPos=analogRead(VOLUME_PIN); 
#ifdef VOLUMEDEBUG
int volPosThreshold=2;
if (millis()-volumeStartTime>250)
    {
    float percentChange=abs((avgSlowVolPos-volPos)/10.24);
    if (percentChange>volPosThreshold)
      {
      Serial.println(volPos);
      Serial.println(percentChange);
      }
    avgSlowVolPos=volPos;
    volumeStartTime=millis();
    }
#endif    
        
LOW_TONE_VOLUME=mapfloat(volPos,VOLUME_LOW_ANALOG,VOLUME_HIGH_ANALOG,0,0.50);
HIGH_TONE_VOLUME_MIN=mapfloat(volPos,VOLUME_LOW_ANALOG,VOLUME_HIGH_ANALOG,0,0.50);      // high tone will ramp up from min to max
SOLID_TONE_VOLUME=mapfloat(volPos,VOLUME_LOW_ANALOG,VOLUME_HIGH_ANALOG,0,0.50);
HIGH_TONE_VOLUME_MAX=mapfloat(volPos,VOLUME_LOW_ANALOG,VOLUME_HIGH_ANALOG,0,1);
float voiceGain=mapfloat(volPos,VOLUME_LOW_ANALOG,VOLUME_HIGH_ANALOG,0,10);
mixer1.gain(2,voiceGain); // amplify channel 2 (voice)
}
#endif
