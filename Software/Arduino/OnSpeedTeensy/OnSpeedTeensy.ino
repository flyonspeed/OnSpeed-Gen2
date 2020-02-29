// to be compiled on Arduino 1.8.7 & TeensyLoader 1.47

// TODO

// calibrate static pressure
// watchdog
// run in Gen1 mode with Efis data (fix OSH code)
// name logfile by date/time when efis data is available
// standardize data log output, OnSpeed format
// output fixed width HUD data
// Pitch angle filter

////////////////////////////////////////////////////
// OnSpeed Gen2 Teensy 3.6 code
// More details at
//      http://www.flyOnSpeed.org
//      and
//      https://github.com/flyonspeed/OnSpeed-Gen2/

#define VERSION           "v3.0"  // last modified 2/28/2020 by Lenny, added wifi based configuration settings, asymmetric overG warning, sensor configuration, serial data output (G3X format)
                          //"v2.1.28"     // last modified 2/1/2020 (timed serial read) 
                          //"v2.1.28"   // last modified 1/31/2020 (added boom CRC)
                          //"v2.1.27"   // last modified 1/10/2020 (fixed G3x data parsing, crc + oat errors)
                          //"v2.1.26.5" // last modified 1/9/2020 (implemented ring buffer write for SD card)
                          //"v2.1.26"  // last modified 1/9/2020 (fixed volume control update rate)
                          //"v2.1.25"  // last modified 1/6/2020  (added boom junk data filter) 
                          //"v2.1.24"  // last modified 1/5/2020 (new 3d audio code & curve, new boom parsing code, new volume control code)
                          //"v2.1.23"  // last modified 1/5/2020 (fixed 3D audio curve going negative)
                          //"v2.1.22"  // last modified 1/4/2020 (setting mixer gain instead of individual tone volumes)
                          //"v2.1.21"  // last modified 1/4/2020 (added AUDIOTEST help description, and fixed Volume control sticking, add volume control during Logreplay)
                          //"v2.1.20"  // last modified 1/1/2020 by Lenny (added AUDIOTEST! command to test left/right audio channels, important for 3D audio)
                          //"v2.1.19"  // last modified 12/27/2019 by Lenny (added 3D audio. Define an audio curve in aircraft config file)
#define DEFAULT_CONFIG  default_config

// hardware config
#define SDCARD // comment out to disable SD card logging
#define IMU  // use inertial sensor board
#define WIFI // use wifi board (wireless data download)
#define BARO // use baro sensor (v1 hardware doesn't have one)


// curves config
#define MAX_AOA_CURVES    5 // maximum number of AOA curves (flap/gear positions)
#define MAX_CURVE_COEFF   4 // 4 coefficients=3rd degree polynomial function for calibration curves

// glimit settings
#define GLIMIT_REPEAT_TIMEOUT   5000 // milliseconds to repeat G limit.
#define ASYMMETRIC_GYRO_LIMIT   30 // degrees/sec rotation on either axis to trigger asymmetric G limits.

// debug config. Comment out any of them to disable serial debug output.
//#define SENSORDEBUG // show sensor debug
//#define EFISDATADEBUG // show efis data debug
//#define BOOMDATADEBUG  // show boom data debug
//#define TONEDEBUG // show tone related debug info
//#define SDCARDDEBUG  // show SD writing debug info
//#define VOLUMEDEBUG  // show audio volume info
//#define AXISDEBUG //show accelerometer axis configuration


// box functionality config
//String dataSource = "TESTPOT"; // potentiometer wiper on Pin 10 of DSUB 15 connector
//String dataSource = "RANGESWEEP";
String dataSource = "SENSORS";
//String dataSource = "REPLAYLOGFILE";
String replayLogFileName="";

// type definitions
typedef struct  {
  int Count;
  int Items[MAX_AOA_CURVES];
} intArray;

typedef struct  {
  int Count;
  float Items[MAX_AOA_CURVES];
} floatArray;

typedef struct  {
  float Items[MAX_CURVE_COEFF]; // 3rd degree polynomial
  uint8_t curveType; // 1 - polynomial, 2 - logarithmic (linear is 1 degree polynomial), 3 -exponential
} calibrationCurve;

// configfile
char configFilename[14]="onspeed.cfg";
bool configLoaded=false;

//serial inputs
bool readBoom; // boom connected to Serial 1
bool readEfisData; // Serial3, read and log serial data (ahrs/aoa/airspeed/altitude data from Dynon EFIS D10, D100, D180, Skyview/ADVANCED, Garmin G5 and G3X)

// serial output
String serialOutFormat;
String serialOutPort;

//volume & audio control
bool volumeControl;
bool audio3D;
int volumeHighAnalog=1023;
int volumeLowAnalog=1;
// Min airspeed in knots before the audio tone is turned on
int muteAudioUnderIAS=30;
int volumePercent; // %volume
int defaultVolume;
unsigned long audio3dLastUpdate=millis();
unsigned long volumeLastUpdate=millis();
unsigned long serialoutLastUpdate=millis();
unsigned long gLimitLastWarning=millis();
unsigned long gLimitLastUpdate=millis();
#define AUDIO_3D_CURVE(x)         -92.822*x*x + 20.025*x //move audio with the ball, scaling is 0.08 LateralG/ball width
bool overgWarning; // 0 - off, 1 - on
float channelGain=1.0;
float calculatedGLimit;


// smoothing windows
int aoaSmoothing=20;  // AOA Gaussian smoothing window
int pressureSmoothing=15; // median filter window for pressure smoothing/despiking
int accMedianSmoothing=51; // median filter window for accelerations, for calculating pitch
int accSmoothing=20; // accelerometer smoothing, Gaussian
int gyroSmoothing=50; // gyro smoothing, Gaussian
int pitchSmoothing=50; // accPitch smoothing, Gaussian

intArray flapDegrees;
intArray flapPotPositions;

// calibration curves
calibrationCurve aoaCurve[MAX_AOA_CURVES]; // 3rd degree polynomial equation coefficents
calibrationCurve boomAlphaCurve;
calibrationCurve boomBetaCurve;
calibrationCurve boomStaticCurve;
calibrationCurve boomDynamicCurve;
calibrationCurve casCurve; // calibrated airspeed curve

//setpoints
floatArray flapLDMAXAOA;
floatArray flapONSPEEDFASTAOA;
floatArray flapONSPEEDSLOWAOA;
floatArray flapSTALLWARNAOA;

//box orientation
String portsOrientation;
String boxtopOrientation;

// load factor limit
float loadLimit;

// IMU orientation
String verticalGloadAxis="";
String lateralGloadAxis="";
String forwardGloadAxis="";
String pitchGyroAxis="";
String rollGyroAxis="";
String yawGyroAxis="";

// sensor biases
int pFwdBias;
int p45Bias;
float pStaticBias;
float axBias;
float ayBias;
float azBias;
float gxBias;
float gyBias;
float gzBias;
float pitchBias=0.0;

// data mark
int dataMark=0;


// interval timers
#define SENSOR_INTERVAL 20000  // microsecond interval for sensor read (50hz)
#define DISPLAY_INTERVAL 100000 // 100 msec (10Hz)
#define SERIAL_INTERVAL   15000 // microsecod interval for Serial data read timer

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
#define PIN_LED2              5    // external LED for showing AOA status (audio on/off)
#define FLAP_PIN              A2     // flap position switch  (pin 7 on DB15)
#define OAT_PIN               A14    // OAT analog input pin
#define SWITCH_PIN            2
#define PULSE_TONE            1
#define SOLID_TONE            2
#define TONE_OFF              3
#define STARTUP_TONES_DELAY   120
#define RANGESWEEP_LOW_AOA    6
#define RANGESWEEP_HIGH_AOA   15
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
//9 - OAT Analog IN  (or Display Serial out)
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
#define CTRL_REG4         0x1E
#define CTRL_REG1_G       0x10
#define OUT_X_L_G         0x18

#define I2C_COMMUNICATION_TIMEOUT 1000  // microseconds

#define SD_WRITE_BLOCK_SIZE  512  // block size to write on the SD card. 512 is the most efficient
#define LOG_RINGBUFFER_SIZE 32768  // ringbuffer size

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

#include "default_config.h"
#include <Audio.h>      // use local Audio.h with play_sd_raw.h & play_sd_wav.h commented out for SDIO compatibility.
#include <Gaussian.h>         // gaussian lib used for avg out AOA values.
#include <LinkedList.h>       // linked list is also required.
#include <GaussianAverage.h>  // more info at https://github.com/ivanseidel/Gaussian
#include <OneButton.h>      // button click/double click detection https://github.com/mathertel/OneButton
#include <RunningMedian.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
#include "AudioSampleEnabled.h"
#include "AudioSampleDisabled.h"
#include "AudioSampleOnspeed_left_speaker.h"
#include "AudioSampleOnspeed_right_speaker.h"
#include "AudioSampleDatamark.h"
#include "AudioSampleGlimit.h"

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
int efisLateralGColumn=16;
volatile int charsreceived=0; // debug
volatile int cachelinecount=0; //debug variable
volatile bool readingSerial=false; // serial port being read
volatile bool readingSensors=false;
volatile int efisMaxAvailable=0;
volatile int boomMaxAvailable=0;
   

// boom variables
String boomBufferString;
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
 


OneButton Switch(SWITCH_PIN, true);  // pin 2  used for the switch input


File SensorFile;
File ListFile;
char filenameSerial[14];
char filenameSensor[14];
bool sdLogging=false;
bool sdLoggingConfig=false;
bool sdAvailable=false;
int filesendtimer;
bool filesendtimeout;

uint8_t toneState = false;
bool switchState;
unsigned char toneMode = PULSE_TONE;  // current mode of tone.  PULSE_TONE, SOLID_TONE, or TONE_OFF
unsigned char tonePlaying = TONE_OFF;
boolean highTone = false;             // are we playing high tone or low tone?
uint32_t toneFreq = 0;                // store current freq of tone playing.
volatile float pps = 20;              // store current PPS of tone (used for debuging) 
//float current_delay=1000/pps;       // 1000/pps, tone timer update rate
float AOA = 0.0;                      // avaraged AOA value is stored here.
float LDmaxAOA=0.00;
float onSpeedAOAfast=0.00;
float onSpeedAOAslow=0.00;
float stallWarningAOA=0.00;
float percentLift=0.0;                     // normalized angle of attack, or lift %
unsigned int ALT = 0;                      // hold ALT (only used for debuging)
float ASI = 0.0;                          // live Air Speed Indicated
float Palt=0.00;                          // pressure altitude
float currentRangeSweepValue=RANGESWEEP_LOW_AOA;
RunningMedian P45Median(pressureSmoothing);
RunningMedian PfwdMedian(pressureSmoothing);
RunningMedian aVertMedian(accMedianSmoothing);
RunningMedian aLatMedian(accMedianSmoothing);
RunningMedian aFwdMedian(accMedianSmoothing);

GaussianAverage AOAAvg = GaussianAverage(aoaSmoothing);
GaussianAverage aFwdAvg = GaussianAverage(accSmoothing);
GaussianAverage aLatAvg = GaussianAverage(accSmoothing);
GaussianAverage aVertAvg = GaussianAverage(accSmoothing);
GaussianAverage gRollAvg = GaussianAverage(gyroSmoothing);
GaussianAverage gPitchAvg = GaussianAverage(gyroSmoothing);
GaussianAverage gYawAvg = GaussianAverage(gyroSmoothing);
GaussianAverage accPitchAvg =GaussianAverage(pitchSmoothing);



// vars for converting AOA scale value to PPS scale value.
int OldRange,  OldValue;
float NewRange, NewValue;

char inChar;                    // store single serial input char here.
char serialCmdChar;             // usb serial command character
char serialWifiCmdChar;         // wifi serial command character  

char serialBoomChar;            // boom serial character
int serialCmdBufferSize = 0;        // usb serial command buffer size
char serialCmdBuffer[51];       // usb serial command buffer

int serialWifiCmdBufferSize = 0;        // usb serial command buffer size
char serialWifiCmdBuffer[2048];       // usb serial command buffer

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
char datalogRingBuffer[LOG_RINGBUFFER_SIZE]; //64Kbytes ringbuffer for datalog  
volatile static int datalogRingBufferStart=0; 
volatile static int datalogRingBufferEnd=0; 
volatile static unsigned int datalogBytesAvailable=0;


// IMU variables
uint8_t _i2cAddress_AccelGyro=LSM9DS1_AccelGyro;
volatile int16_t ax, ay, az; // ax -instantaneous, Ax - smoothed
volatile float aVert, aLat, aFwd; 
volatile int16_t gx, gy, gz; // gx - instantaneous, Gx - smoothed
volatile float gRoll, gYaw, gPitch;
volatile float accPitch=0.0; // accelerometer derived pitch 
float aRes=0.000244140625; // 8g / 32768.0
float gRes=500.0 / 32768.0; // full scale / resolution
uint8_t Ascale = 3;     // accel full scale, 8G
uint8_t Aodr = 6;   // accel data sample rate,  AODR_952Hz
uint8_t Abw = 3;      // accel data bandwidth,  ABW_50Hz
uint8_t Gscale = 1;   // 500 degree/sec
uint8_t Godr = 6;     // GODR_952Hz
uint8_t Gbw = 2;    // 58 Hz at Godr = 952 Hz


IntervalTimer ToneTimer;
IntervalTimer SensorTimer; // read sensors
IntervalTimer SerialDataTimer; // read serial ports
IntervalTimer PotTimer; // read potentiometer           
IntervalTimer RangeSweepTimer; // sweep through AOA range, high to low
IntervalTimer LogReplayTimer;
IntervalTimer LiveDisplayTimer;

// volume variables
float LOW_TONE_VOLUME=.25;      // volume is 0 to 1
float HIGH_TONE_VOLUME_MIN=.25;      // high tone will ramp up from min to max
float SOLID_TONE_VOLUME=.25;
float HIGH_TONE_VOLUME_MAX=1;
int volPos=0;
int avgSlowVolPos=512;
unsigned long volumeStartTime=millis();
int flapsPos=0;
int flapsIndex=0;
int loopcount=0;
unsigned long looptime=millis();

#include <Onspeed-settingsFunctions.h> // library with setting functions

void setup() {
  
delay(2000); // let the aircraft's audio panel boot up
AudioMemory(16);

Serial.print("OnSpeed Gen2 ");
Serial.println(VERSION);
#ifdef SDCARD
// initialize SD card
sdAvailable=SD.begin(SdioConfig(FIFO_SDIO));
#endif
// load configuration
if (sdAvailable && SD.exists(configFilename))
    {
    // load config from file
    Serial.println("Loading onspeed.cfg configuration");
    configLoaded=loadConfigurationFile(configFilename);
    if (configLoaded) Serial.println("Configuration loaded.");
    } else
        {
        // load default config 
        Serial.println("No onspeed.cfg file on card, loading default configuration");
        configLoaded=loadDefaultConfiguration();
        if (configLoaded) Serial.println("Default configuration loaded.");
        }          

// configure accelerometer Axis
  configureAxes();

// volume control defaults
if (!volumeControl) 
    {
    // set default volume when volume potentiometer is disabled
    volumePercent=defaultVolume;
    setVolume(volumePercent);
    }

// create logfile
    if (dataSource=="SENSORS" && sdAvailable && sdLogging)sdAvailable=SD.begin(SdioConfig(FIFO_SDIO));
        {
        Serial.println("SD card initialized. Logging Enabled.");       
        sprintf(filenameSensor,"log_1.csv");        
        int fileCount=1;
        while (SD.exists(filenameSensor))
              {
              fileCount++;  
              sprintf(filenameSensor,"log_%d.csv",fileCount);
              }
        Serial.print("Sensor log file:"); Serial.println(filenameSensor);        
        if (sdLogging)
            {
            SensorFile = SD.open(filenameSensor, FILE_WRITE);
            if (SensorFile) {
                            SensorFile.print("timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,PStatic,Palt,IAS,AngleofAttack,flapsPos,DataMark");
                            #ifdef IMU
                            SensorFile.print(",VerticalG,LateralG,ForwardG,RollRate,PitchRate,YawRate,AccelPitch");
                            #endif        
                            if (readBoom) SensorFile.print(",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge");                     
                            if (readEfisData) SensorFile.print(",efisIAS,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisMagHeading,efisAge,efisTime");          
                            SensorFile.println();
                            SensorFile.close();
                            }  else Serial.print("SensorFile opening error");        
            }
         
        
      }
        displayConsoleHelp();
        delay(100);
 
  // set up i2c ports
  Wire.begin (I2C_MASTER,0x00,I2C_PINS_18_19,I2C_PULLUP_EXT,400000); // SDA0/SCL0 I2C at 400kHz, PS1 sensor, Pfwd   
  Wire.setOpMode(I2C_OP_MODE_IMM);
  Wire.setDefaultTimeout(2000);

  Wire2.begin(I2C_MASTER,0x00,I2C_PINS_3_4,I2C_PULLUP_EXT,400000); // SDA1/SCL1 I2C at 400kHz, PS2 sensor, P45
  Wire2.setOpMode(I2C_OP_MODE_IMM);
  Wire2.setDefaultTimeout(2000);
  
  Wire1.begin();
  Wire1.setDefaultTimeout(2000);

  initAccelGyro(); //initialize accelerometer & Gyro (IMU)

  
  
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
  #endif

  if (readBoom) Serial1.begin(BAUDRATE_BOOM);  //Init hardware serial port (input from BOOM)


  if (readEfisData) Serial3.begin(BAUDRATE_EFIS); //Init hardware serial port (input from EFIS)


  if (serialOutFormat!="NONE" && serialOutPort=="Serial5") 
                          {
                          Serial5.begin(115200);
                          }

    
  digitalWrite(PIN_LED1, 1);
  
  digitalWrite(PIN_LED1, 0);
  
  ToneTimer.priority(16);
  ToneTimer.begin(tonePlayHandler,1000000/pps); // microseconds
  
  SerialDataTimer.priority(250);
  SerialDataTimer.begin(readSerialData,SERIAL_INTERVAL);       

  lastReceivedEfisTime=millis();
  lastReceivedBoomTime=millis();
  

#ifdef WIFI
   LiveDisplayTimer.priority(224);
#endif
  
  
  
  if (dataSource=="SENSORS")
    {
    SensorTimer.priority(255);
    SensorTimer.begin(SensorRead,SENSOR_INTERVAL); // every 20ms       
    } else
          if (dataSource=="TESTPOT")
              {
              PotTimer.begin(PotRead,SENSOR_INTERVAL); // 20ms            
              } else
                    if (dataSource=="RANGESWEEP")
                    {
                    RangeSweepTimer.begin(RangeSweep,200000); // 100ms
                    } else
                          
                          if (dataSource=="REPLAYLOGFILE")
                          {
                          // turn off SD logging
                          sdLogging=false;
                          // check if file exists
                           sprintf(filenameSensor,replayLogFileName.c_str());
                            
                            if (SD.exists(filenameSensor))
                                  {
                                  Serial.print("Replaying data from log file: "); Serial.println(filenameSensor);
                                  } else
                                        {
                                        Serial.printf("ERROR: Could not find %s on the SD card\n",filenameSensor);                      
                                        }                                                   
                            
                            SensorFile = SD.open(filenameSensor);        
                            if (SensorFile)
                                           {                                                                    
                                            LogReplayTimer.priority(240);
                                            LogReplayTimer.begin(LogReplay,SENSOR_INTERVAL); // same interval as SensorRead
                                           } else
                                           {
                                            Serial.printf("ERROR: Could not open %s on the SD card.\n",filenameSensor);
                                           }
                                           
                            Serial.println();               
                                                                                                                  
                          }

 #ifdef SDCARD 
  // start logging timer
  sensorLoggingStartTime=millis();
 #endif 
 // initialize sine wave generators
 AudioNoInterrupts();
 sinewave1.frequency(400);
 sinewave1.amplitude(0);
 AudioInterrupts(); 
}


uint8_t Tone2FlipFlop = false;

void displayConsoleHelp()
{
        Serial.println();
        Serial.println("Accepted commands on this console:\n");
        Serial.println("HELP! - display this command list");
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
        Serial.println("CONFIG! -show current configuration values");
        Serial.println("AUDIOTEST! -left & right audio test"); 
        Serial.println();
}        


void tonePlayHandler(){
    switchCheck(); // check main switch   
    if (!switchState) return; // return if tones are disabled 
  if(toneMode==TONE_OFF) {
    setFrequencytone(0);  // if tone off skip the rest.
    #ifdef TONEDEBUG 
    Serial.println("TONE OFF");
    #endif        
    return;
  }
  if(toneMode==SOLID_TONE) {  // check for a solid tone.
    #ifdef TONEDEBUG 
    Serial.println("SOLID TONE");
    #endif
    setFrequencytone(LOW_TONE_HZ);
    return; // skip the rest
  }
     if(highTone) {
                  setFrequencytone(HIGH_TONE_HZ);
                  #ifdef TONEDEBUG 
                  Serial.println("HIGH TONE");
                  #endif
                  } else {
                          setFrequencytone(LOW_TONE_HZ);
                          #ifdef TONEDEBUG 
                          Serial.println("LOW TONE");
                          #endif
                         } 
                         
}

void updateTones()
{
  if(ASI <= muteAudioUnderIAS) {
  #ifdef TONEDEBUG    
  // show audio muted and debug info.
  sprintf(tempBuf, "AUDIO MUTED: Airspeed to low. Min:%i ASI:%.2f",muteAudioUnderIAS, ASI);
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
Serial.println("Data Mark");
dataMark++;
voice1.play(AudioSampleDatamark);
}


// main loop of app
void loop() { 
loopcount++;


#ifdef SDCARD
// look for serial command
if (Serial.available()>0)
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
// CONFIG!
// AUDIOTEST!
serialCmdChar = Serial.read();    
  if (serialCmdChar!=char(0x21) && serialCmdBufferSize<50)
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
                                              Serial.println("\nDONE.");
                                            } else {
                                              // if the file didn't open, print an error:
                                              Serial.print("Error opening "); Serial.println(listfileFileName);
                                            }
                                 
                                 } else
                                      if (strstr(serialCmdBuffer, "STOP"))
                                         {
                                         // stop SD logging to take file commands
                                         sdLogging=false;
                                         if (dataSource=="SENSORS")
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
                                                 if (dataSource=="SENSORS") 
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
                                                                          delay(300);
                                                                          //reinitialize SD card
                                                                          sdAvailable=SD.begin(SdioConfig(FIFO_SDIO));
                                                                          String configString=configurationToString();
                                                                          saveConfigurationToFile(configFilename,configString); 
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
                                                                                                          }  else
                                                                                                                if (strstr(serialCmdBuffer, "CONFIG"))                                                                            
                                                                                                                    {
                                                                                                                    Serial.println("Current configuration: ");
                                                                                                                                                                                                                           
                                                                                                                    Serial.println(configurationToString());
                                                                                                                    }
                                                                                                                      else                                                                                                          
                                                                                                                          if (strstr(serialCmdBuffer, "AUDIOTEST"))                                                                            
                                                                                                                            {
                                                                                                                            ToneTimer.end(); // turn off ToneTimer
                                                                                                                            SensorTimer.end();
                                                                                                                            Serial.println("Playing Left audio");                                                                                                                  
                                                                                                                             ampLeft.gain(1);
                                                                                                                             ampRight.gain(0);
                                                                                                                             voice1.play(AudioSampleOnspeed_left_speaker);
                                                                                                                            delay (2500);
                                                                                                                             ampLeft.gain(0);
                                                                                                                             ampRight.gain(1);
                                                                                                                             Serial.println("Playing Right audio");
                                                                                                                             voice1.play(AudioSampleOnspeed_right_speaker);
                                                                                                                            delay (2500);
                                                                                                                             ampLeft.gain(1);
                                                                                                                             ampRight.gain(1);
                                                                                                                             ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on 
                                                                                                                             SensorTimer.begin(SensorRead,SENSOR_INTERVAL);
                                                                                                                             Serial.println("AUDIOTEST Complete");
                                                                                                                            } else
                                                                                                                                  if (strstr(serialCmdBuffer, "HELP"))                                                                            
                                                                                                                                      {
                                                                                                                                      displayConsoleHelp();  
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
//$SENDCONFIGSTRING!
//$SAVECONFIGSTRING<configstring>!
//$VOLUME
//$FLAPS
//$AOA
//$AUDIOTEST
serialWifiCmdChar = Serial4.read();
Serial.print(serialWifiCmdChar);
if (serialWifiCmdBufferSize >=2047)
      {       
      serialWifiCmdBufferSize=0; // don't let the command buffer overflow
      }
  if (serialWifiCmdChar!='!')
    {
    if (serialWifiCmdBufferSize==0 && serialWifiCmdChar!='$')
        {
        //drop character if command doesn't start with $                
        }
         else
              {
              serialWifiCmdBuffer[serialWifiCmdBufferSize]=serialWifiCmdChar;
              serialWifiCmdBufferSize++;
              }
    }          
    if (serialWifiCmdBufferSize>0 && serialWifiCmdChar=='!')
          { 
          // process command          
            if (strstr(serialWifiCmdBuffer, "$LIST"))
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
                     if (strstr(serialWifiCmdBuffer, "$DELETE"))
                        {
                        // delete file                        
                        memcpy(listfileFileName,serialWifiCmdBuffer+8,serialWifiCmdBufferSize-7);
                        SD.remove(listfileFileName);
                        Serial4.println();
                        Serial4.print("Deleted: ");
                        Serial4.println(listfileFileName);
                        } else
                              if (strstr(serialWifiCmdBuffer, "$PRINT"))
                                 {
                                 bool orig_sdLogging=sdLogging;
                                 if (orig_sdLogging)
                                      {
                                      sdLogging=false; // turn off sdLogging                                            
                                      Serial.println("STOPPED SD logging during file transfer.");
                                      }
                                 timersOff();                                                                                                   
                                 // print file contents
                                 Serial.println(serialWifiCmdBuffer);
                                 memcpy(listfileFileName,serialWifiCmdBuffer+7,serialWifiCmdBufferSize-6);
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
                                              // if the file didn't open, print an error
                                              Serial.print("Could not open file: ");
                                              Serial.println(listfileFileName);
                                              Serial4.print("<404>");
                                              
                                            }
                                 Serial4.begin(BAUDRATE_WIFI); // reset wifi baudrate to lower speed     
                                 if (orig_sdLogging)
                                    {
                                    sdLogging=true; // turn sd logging back on if it was originally on.                                                                        
                                    Serial.println("STARTED SD logging.");
                                    }
                                 timersOn();
                                 } else
                                         
                                         if (strstr(serialWifiCmdBuffer, "$STOPLIVEDATA"))
                                         {
                                         LiveDisplayTimer.end(); // stop display data                                         
                                         Serial.println("STOPPED Live Data. (Wifi Request)");
                                         } else
                                              if (strstr(serialWifiCmdBuffer, "$STARTLIVEDATA"))
                                                 {
                                                 LiveDisplayTimer.begin(SendDisplayData,DISPLAY_INTERVAL); // start display data
                                                 Serial.println("STARTED Live Data. (Wifi Request)");
                                                 } else

                                                   if (strstr(serialWifiCmdBuffer, "$FORMAT"))
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
                                                                 Serial4.println("<formaterror></formaterror>");                                                                 
                                                              } else                                                               
                                                                 {
                                                                  cardSectorCount = m_card->sectorCount();
                                                                  //Serial.printf("Sectorcount: %i\n",cardSectorCount);
                                                                    // Format exFAT if larger than 32GB.
                                                                    bool rtn = cardSectorCount > 67108864 ?
                                                                      exFatFormatter.format(m_card, sectorBuffer, &Serial) :
                                                                      fatFormatter.format(m_card, sectorBuffer, &Serial);                                                                  
                                                                    if (!rtn) {
                                                                      Serial.println("FORMAT ERROR: Could not format SD Card.");
                                                                      Serial4.println("<FORMATERROR>Could not format SD Card</FORMATERROR>");
                                                                    } else
                                                                          {
                                                                          Serial.print("SD card format completed. Card size: ");
                                                                          Serial.print(cardSectorCount*5.12e-7);
                                                                          Serial.println("GBytes");
                                                                          Serial4.printf("<FORMATDONE>%.1f GB</FORMATDONE>\n",cardSectorCount*5.12e-7);
                                                                          delay(300);
                                                                          //reinitialize SD card
                                                                          sdAvailable=SD.begin(SdioConfig(FIFO_SDIO));
                                                                          String configString=configurationToString();
                                                                          saveConfigurationToFile(configFilename,configString);                                                                                                                                                 
                                                                          }                                                                    
                                                                 }  
                                                                                                                    
                                                          if (orig_sdLogging)
                                                                {
                                                                //reinitialize card
                                                                SD.begin(SdioConfig(FIFO_SDIO));
                                                                sdLogging=true; // turn sd logging back on if it was originally on.
                                                                }
                                                          } else
                                                              if (strstr(serialWifiCmdBuffer, "$PFWDBIAS"))
                                                                  {                                                                  
                                                                  SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                  Serial.println("Wifi: Getting Pfwd bias...");
                                                                  // get Pfwd bias
                                                                  long PfwdTotal=0;                                                                
                                                                  for (int i=1;i<=500;i++)
                                                                      {
                                                                      PfwdTotal+=GetPressurePfwd();
                                                                      delay(2);
                                                                      }
                                                                 Serial4.printf("<PFWDBIAS>%i</PFWDBIAS>",round(PfwdTotal/500));
                                                                 SensorTimer.begin(SensorRead,SENSOR_INTERVAL);                                                                                                                               
                                                                  } else
                                                                      if (strstr(serialWifiCmdBuffer, "$P45BIAS"))
                                                                      {                                                                  
                                                                      SensorTimer.end(); // stop polling the sensors on the timer so we can poll them here
                                                                      Serial.println("Wifi: Getting P45 bias...");
                                                                      // get Pfwd bias                                                                      
                                                                      long P45Total=0;
                                                                      for (int i=1;i<=500;i++)
                                                                          {                                                                          
                                                                          P45Total+=GetPressureP45();
                                                                          delay(2);
                                                                          }
                                                                      Serial4.printf("<P45BIAS>%i</P45BIAS>",round(P45Total/500));
                                                                     SensorTimer.begin(SensorRead,SENSOR_INTERVAL);                                                                                                                               
                                                                      } else
                                                                            if (strstr(serialWifiCmdBuffer, "$FLAPS"))
                                                                                {                                                                                                                                              
                                                                                Serial.println("Wifi: Getting Flap position");
                                                                                int flapPotTotal=0;
                                                                                for (int i=0;i<100;i++)
                                                                                    {
                                                                                    flapPotTotal+=analogRead(FLAP_PIN);
                                                                                    delay(10);
                                                                                    }                                                                                                      
                                                                                Serial4.printf("<FLAPS>%i</FLAPS>",round(flapPotTotal/100));                                                                                                                                
                                                                                } else
                                                                                  if (strstr(serialWifiCmdBuffer, "$VOLUME"))
                                                                                        {                                                                                                                                              
                                                                                        Serial.println("Wifi: Getting Volume position");
                                                                                        int volumePotTotal=0;
                                                                                        for (int i=0;i<100;i++)
                                                                                            {
                                                                                            volumePotTotal+=analogRead(VOLUME_PIN);
                                                                                            delay(10);
                                                                                            }                                                                                                              
                                                                                        Serial4.printf("<VOLUME>%i</VOLUME>",round(volumePotTotal/100));                                                                                                                                
                                                                                        } else                                                                                                          
                                                                                              if (strstr(serialWifiCmdBuffer, "$AUDIOTEST"))                                                                                                                                                                          
                                                                                                {
                                                                                                Serial.println("Wifi: Testing audio");
                                                                                                timersOff();
                                                                                                Serial.println("Wifi: Playing Left audio test");                                                                                                                  
                                                                                                 ampLeft.gain(1);
                                                                                                 ampRight.gain(0);
                                                                                                 voice1.play(AudioSampleOnspeed_left_speaker);
                                                                                                delay (2500);
                                                                                                 ampLeft.gain(0);
                                                                                                 ampRight.gain(1);
                                                                                                 Serial.println("Wifi: Playing Right audio test");
                                                                                                 voice1.play(AudioSampleOnspeed_right_speaker);
                                                                                                delay (2500);
                                                                                                 ampLeft.gain(1);
                                                                                                 ampRight.gain(1);
                                                                                                 timersOn();
                                                                                                 Serial.println("Wifi: AUDIOTEST Complete");
                                                                                                 Serial4.println("<AUDIOTEST>Done.</AUDIOTEST>");
                                                                                                } else
                                                                                                      if (strstr(serialWifiCmdBuffer, "$WIFIREFLASH"))                                                                            
                                                                                                        {
                                                                                                        Serial4.end();
                                                                                                        pinMode(PIN_A12,INPUT);
                                                                                                        pinMode(PIN_A13,INPUT);
                                                                                                        Serial.println("wifi reflash mode");
                                                                                                        } else
                                                                                                               if (strstr(serialWifiCmdBuffer, "$REBOOT"))                                                                            
                                                                                                                    {
                                                                                                                    Serial4.println("<REBOOT>OK</REBOOT>");
                                                                                                                    Serial.println("Wifi reboot request. Rebooting...");
                                                                                                                    delay(200);                                                                                       
                                                                                                                    _softRestart();
                                                                                                                    } else
                                                                                                                          if (strstr(serialWifiCmdBuffer, "$SENDCONFIGSTRING"))                                                                            
                                                                                                                              {                                                                                                                              
                                                                                                                              timersOff();
                                                                                                                              Serial.println("Configstring request from wifi");
                                                                                                                              String configString=configurationToString();
                                                                                                                              // add CRC to configString
                                                                                                                              configString=addCRC(configString);
                                                                                                                              String stringToSend="$SAVECONFIGSTRING"+configString+"!";
                                                                                                                                                                                                                                                                                                                                                                                        
                                                                                                                              for (unsigned int i=0; i<configString.length();i++)
                                                                                                                                  {                                                                                                                                  
                                                                                                                                  Serial4.print(configString[i]); 
                                                                                                                                  unsigned long delayTimer=micros();                                                                                                                                  
                                                                                                                                  while (micros()-delayTimer<200) {}; // transmit delay to avoid output buffer overrun
                                                                                                                                  }
                                                                                                                              Serial.println("Configstring sent to Wifi");
                                                                                                                             timersOn();                                                                                                                            
                                                                                                                              } else
                                                                                                                                    if (strstr(serialWifiCmdBuffer, "$SAVECONFIGSTRING"))
                                                                                                                                          {
                                                                                                                                          timersOff();
                                                                                                                                          sdLogging=false;  
                                                                                                                                          // saveConfigString
                                                                                                                                          String configString=String(serialWifiCmdBuffer);
                                                                                                                                          String checksumString=getConfigValue(configString,"CHECKSUM");                                                                                                                                         
                                                                                                                                          String configContent=getConfigValue(configString,"CONFIG");
                                                                                                                                           // calculate checksum
                                                                                                                                          int16_t calcCRC=0;
                                                                                                                                          for (unsigned int i=0;i<configContent.length();i++) calcCRC+=configContent[i];
                                                                                                                                          if (String(calcCRC,HEX)==checksumString)
                                                                                                                                              {
                                                                                                                                              // checksum ok.
                                                                                                                                              configString="<CONFIG>"+configContent+"</CONFIG>"; // strip checksum tag when saving config
                                                                                                                                              if (loadConfigFromString(configString))
                                                                                                                                                  {                                                                                                                                                
                                                                                                                                                  saveConfigurationToFile(configFilename,configString);
                                                                                                                                                  setVolume(volumePercent);
                                                                                                                                                  configureAxes();
                                                                                                                                                  Serial4.print("<CONFIGSAVED></CONFIGSAVED>"); 
                                                                                                                                                  Serial.println("Wifi: Loaded new configuration");                                                                                                                
                                                                                                                                                  } else Serial4.print("<CONFIGERROR>Could not load configuration</CONFIGERROR>");                                                                                                                                              
                                                                                                                                          
                                                                                                                                                } else
                                                                                                                                                      {
                                                                                                                                                      //checksum failed
                                                                                                                                                      Serial4.print("<CONFIGERROR>Transmission Error. Checksum failed. Try again.</CONFIGERROR>");
                                                                                                                                                      }
                                                                                                                                          
                                                                                                                                          
                                                                                                                                          timersOn();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
                                                                                                                                          } else
                                                                                                                                                if (strstr(serialWifiCmdBuffer, "$AOA"))
                                                                                                                                                    {
                                                                                                                                                    Serial.println("Wifi: AOA request");  
                                                                                                                                                    float aoaTotal=0.0;  
                                                                                                                                                    // sample aoa for 1 sec
                                                                                                                                                    for (int i=0;i<50;i++)
                                                                                                                                                        {
                                                                                                                                                        aoaTotal+=AOA;
                                                                                                                                                        unsigned long starttime=millis();
                                                                                                                                                        // wait 20ms;
                                                                                                                                                        while (millis()-starttime<=20) {};                                                                                                                                                        
                                                                                                                                                        }
                                                                                                                                                        float avgAOA= aoaTotal/50;
                                                                                                                                                    Serial4.printf("<AOA>%.2f</AOA>",avgAOA);
                                                                                                                                                    } else
                                                                                                                                                        if (strstr(serialWifiCmdBuffer, "$SENDDEFAULTCONFIGSTRING"))                                                                            
                                                                                                                                                            {
                                                                                                                                                            timersOff();  
                                                                                                                                                            Serial.println("Wifi: Default Configstring request from wifi");                                                                                                                                                            
                                                                                                                                                            String configString=String(DEFAULT_CONFIG);
                                                                                                                                                            // add CRC to configString
                                                                                                                                                            configString=addCRC(configString);
                                                                                                                                                                                                                                                                                         
                                                                                                                                                            for (unsigned int i=0; i<configString.length();i++)
                                                                                                                                                                {
                                                                                                                                                                Serial4.print(configString[i]);
                                                                                                                                                                unsigned long delayTimer=micros();
                                                                                                                                                                while (micros()-delayTimer<200) {}; // transmit to delay to avoid output buffer overrun
                                                                                                                                                                }
                                                                                                                                                            timersOn();    
                                                                                                                                                            } else
                                                                                                                                                                if (strstr(serialWifiCmdBuffer, "$SENSORCONFIG"))
                                                                                                                                                                    {
                                                                                                                                                                    // sensor bias configuration
                                                                                                                                                                     pitchBias=stringToFloat(getConfigValue(serialWifiCmdBuffer,"PITCHBIAS"));
                                                                                                                                                                     timersOff();
                                                                                                                                                                     sdLogging=false;
                                                                                                                                                                     Serial.println("Wifi: SensorConfig request");
                                                                                                                                                                     // sample sensors
                                                                                                                                                                     long P45Total=0;
                                                                                                                                                                     long PFwdTotal=0;
                                                                                                                                                                     float axTotal=0.00;
                                                                                                                                                                     float ayTotal=0.00;
                                                                                                                                                                     float azTotal=0.00;
                                                                                                                                                                     float gxTotal=0.00;
                                                                                                                                                                     float gyTotal=0.00;
                                                                                                                                                                     float gzTotal=0.00;
                                                                                                                                                                     int sensorReadCount=150;                             
                                                                                                                                                                     for (int i=0;i<sensorReadCount;i++)
                                                                                                                                                                          {
                                                                                                                                                                          P45Total+=GetPressureP45();
                                                                                                                                                                          PFwdTotal+=GetPressurePfwd();
                                                                                                                                                                          readAccelGyro();
                                                                                                                                                                          axTotal+=scaleAccel(ax);                                                                                                                                                                          
                                                                                                                                                                          ayTotal+=scaleAccel(ay);
                                                                                                                                                                          azTotal+=scaleAccel(az);
                                                                                                                                                                          gxTotal+=scaleGyro(gx);
                                                                                                                                                                          gyTotal+=scaleGyro(gy);
                                                                                                                                                                          gzTotal+=scaleGyro(gz);
                                                                                                                                                                          delay(30);
                                                                                                                                                                          }
                                                                                                                                                                     pFwdBias= int((PFwdTotal/sensorReadCount));
                                                                                                                                                                     p45Bias= int((P45Total/sensorReadCount));
                                                                                                                                                                     gxBias=-gxTotal/sensorReadCount;
                                                                                                                                                                     gyBias=-gyTotal/sensorReadCount;
                                                                                                                                                                     gzBias=-gzTotal/sensorReadCount;

                                                                                                                                                                     // bias the vertical axis to 1G and the rest to 0G.
                                                                                                                                                                     if (verticalGloadAxis[verticalGloadAxis.length()-1]=='X')
                                                                                                                                                                          {
                                                                                                                                                                          axBias=1-abs(axTotal/sensorReadCount);
                                                                                                                                                                          ayBias=-ayTotal/sensorReadCount;
                                                                                                                                                                          azBias=-azTotal/sensorReadCount;                                                                                                                                                                          
                                                                                                                                                                          } else 
                                                                                                                                                                                if (verticalGloadAxis[verticalGloadAxis.length()-1]=='Y')
                                                                                                                                                                                    {
                                                                                                                                                                                    axBias=-axTotal/sensorReadCount;
                                                                                                                                                                                    ayBias=1-abs(ayTotal/sensorReadCount);
                                                                                                                                                                                    azBias=-azTotal/sensorReadCount;
                                                                                                                                                                                    } else 
                                                                                                                                                                                          {
                                                                                                                                                                                          axBias=-axTotal/sensorReadCount;
                                                                                                                                                                                          ayBias=-ayTotal/sensorReadCount;
                                                                                                                                                                                          azBias=1-abs(azTotal/sensorReadCount);
                                                                                                                                                                                          }
                                                                                                                                                                                                                                                                                                                             


                                                                                                                                                                     String resultString="PfwdBias: "+ String(pFwdBias)+"<br>";
                                                                                                                                                                     resultString+=" P45Bias: "+ String(p45Bias)+"<br>";
                                                                                                                                                                     resultString+=" axBias: "+ String(axBias)+"<br>";
                                                                                                                                                                     resultString+=" ayBias: "+ String(ayBias)+"<br>";
                                                                                                                                                                     resultString+=" azBias: "+ String(azBias)+"<br>";
                                                                                                                                                                     resultString+=" gxBias: "+ String(gxBias)+"<br>";
                                                                                                                                                                     resultString+=" gyBias: "+ String(gyBias)+"<br>";
                                                                                                                                                                     resultString+=" gzBias: "+ String(gzBias)+"<br>";
                                                                                                                                                                     resultString+=" pitchBias: "+ String(pitchBias)+"<br>";                                                                                                                                                                     
                                                                                                                                                                     String configString=configurationToString();
                                                                                                                                                                     saveConfigurationToFile(configFilename,configString);
                                                                                                                                                                     Serial4.println("<SENSORCONFIG>"+resultString+"</SENSORCONFIG>");
                                                                                                                                                                     Serial.println("Wifi: SensorConfig complete.");
                                                                                                                                                                     timersOn();
                                                                                                                                                                     sdLogging=true;                                                                                                                                                                
                                                                                                                                                                    }                                                                                                                                                                                                                                                         

          // reset cmdBuffer
          memset(serialWifiCmdBuffer,0,sizeof(serialWifiCmdBuffer));
          serialWifiCmdBufferSize=0;
          }  
  } // if serial.available wifi

#endif


if (millis()-looptime > 1000)
      {
      //float calcPitch=atan2((- aFwd), sqrt(aLat* aLat + aVert *aVert)) * 57.2957;
      //Serial.printf("calcPitch: %.2f\n",calcPitch);        
      #ifdef EFISDATADEBUG
      Serial.printf("\nloopcount: %i",loopcount);
      Serial.printf("\nchars received: %i",charsreceived);     
      Serial.printf("\nmax available efis characters: %i",efisMaxAvailable);
      efisMaxAvailable=0;
      #endif
            
      if (readEfisData)
          {
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
          }  
     
      if (readBoom)
         {
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
         }    
        
      looptime=millis();
      loopcount=0;
      charsreceived=0;
      SensorWriteSD(); 
      }

//3D audio
if (audio3D && (millis()-audio3dLastUpdate)>100)
    {
    float smoothingFactor=0.1;
    float curveGain;
    noInterrupts();
    float lateralG=aLat;
    interrupts();
    int signLateralG= (lateralG > 0) - (lateralG < 0);
    curveGain=AUDIO_3D_CURVE(abs(lateralG));
    if (curveGain>1) curveGain=1;
    if (curveGain<0) curveGain=0;
    curveGain=curveGain*signLateralG;
    channelGain = smoothingFactor * curveGain + (1-smoothingFactor) * channelGain;
    ampLeft.gain(abs(-1+channelGain));
    ampRight.gain(abs(1+channelGain));
    #ifdef AUDIO_3D_DEBUG
    Serial.printf("%0.3fG, Left: %0.3f, Right: %0.3f\n",lateralG,abs(-1+channelGain),abs(1+channelGain));
    #endif
    audio3dLastUpdate=millis();    
    }

// volume control
if (volumeControl && millis()-volumeLastUpdate>200) // update every 200ms
    {
    float volumeSmoothingFactor=0.5;
    volPos = volumeSmoothingFactor * analogRead(VOLUME_PIN) + (1-volumeSmoothingFactor) * volPos;    
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
    //float toneGain=mapfloat(volPos,volumeLowAnalog,volumeHighAnalog,0,2);
    //float voiceGain=mapfloat(volPos,volumeLowAnalog,volumeHighAnalog,0,10);
    volumePercent=mapfloat(volPos,volumeLowAnalog,volumeHighAnalog,0,100);
    setVolume(volumePercent);   
    volumeLastUpdate=millis();    
    }

// g limit warning
if (overgWarning && millis()-gLimitLastWarning>GLIMIT_REPEAT_TIMEOUT && millis()-gLimitLastUpdate>=100)
    {
    gLimitLastUpdate=millis();  
    if (gRoll>=ASYMMETRIC_GYRO_LIMIT || gYaw>=ASYMMETRIC_GYRO_LIMIT)
        {
        calculatedGLimit=loadLimit*0.666;
        } else calculatedGLimit=loadLimit;
    if (aVert > calculatedGLimit)
        {
        gLimitLastWarning=millis();  
        voice1.play(AudioSampleEnabled);
        }
    }
      

// write serialout data
if (serialOutPort!="NONE" && millis()-serialoutLastUpdate>200) // update every 200ms
    {
          // send serial data if enabled
          if (serialOutFormat == "G3X")
              {
              // send G3X formatted data
              char g3xoutString[59];
              float(percentLift);
              float verticalGload=aVert;
              float lateralGload=aLat;
              percentLift=AOA* 60/((onSpeedAOAfast+onSpeedAOAslow)/2); // onSpeedAOA (middle of the onspeed band); // scale/align percent lift so Onspeed = 60% lift   
              sprintf(g3xoutString,"=1100000000____________%04u%+06i____%+03i%+03i%02u__________",unsigned(ASI*10),int(Palt),int(lateralGload*100),int(verticalGload*10),unsigned(percentLift));             
              int8_t calcCRC=0;
              for (int i=0;i<=54;i++) calcCRC+=g3xoutString[i];
              if (serialOutPort=="Serial5")
                  {                 
                  Serial5.print(g3xoutString);    
                  Serial5.print(calcCRC,HEX);
                  Serial5.print("\n");
                  //Serial.println(g3xoutString);
                  } else
                        if (serialOutPort=="Serial2")
                            {
                             if (!Serial2) 
                                {
                                Serial2.begin(115200);
                                }
                            Serial2.print(g3xoutString);    
                            Serial2.print(calcCRC,HEX);
                            Serial2.println();
                            }
                  
              } else
                    if (serialOutFormat == "ONSPEED")
                        {
                        // send ONSPEED formatted data
                        }
    }
    
   
}  // loop


void readSerialData()
{
if (readingSerial) return;
readingSerial=true; 
if (readEfisData)
    {
    //read EFIS data
     while (Serial3.available()>0)
          {
          efisMaxAvailable=max(Serial3.available(),efisMaxAvailable);          
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
    if (efisType=="ADVANCED")
        {        
                          if (efisBufferString.length()==74 && efisBufferString[0]=='!' && efisBufferString[1]=='1')
                             {
                             // parse Skyview AHRS data                            
                             String parseString;
                             //calculate CRC
                             int calcCRC=0;
                             for (int i=0;i<=69;i++) calcCRC+=efisBufferString[i];                     
                             calcCRC=calcCRC & 0xFF;
                             if (calcCRC==(int)strtol(&efisBufferString.substring(70, 72)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                {
                                 //float efisASI                            
                                 parseString=efisBufferString.substring(23, 27); 
                                 noInterrupts();
                                 if (parseString!="XXXX") efisASI=parseString.toFloat()/10; else efisASI=-1; // knots
                                 interrupts();
                                 //float efisPitch                             
                                 parseString=efisBufferString.substring(11, 15);
                                 noInterrupts();                             
                                 if (parseString!="XXXX") efisPitch=parseString.toFloat()/10; else efisPitch=-100; // degrees
                                 interrupts();
                                 // float efisRoll
                                 parseString=efisBufferString.substring(15, 20);
                                 noInterrupts();                             
                                 if (parseString!="XXXXX") efisRoll=parseString.toFloat()/10; else efisRoll=-100; // degrees
                                 interrupts();
                                // float MagneticHeading                                   
                                 parseString=efisBufferString.substring(20, 23);
                                 noInterrupts();                             
                                 if (parseString!="XXX") efisHeading=parseString.toInt(); else efisHeading=-1;                             
                                 interrupts();
                                 // float efisLateralG
                                 parseString=efisBufferString.substring(37, 40);
                                 noInterrupts();
                                 if (parseString!="XXX") efisLateralG=parseString.toFloat()/100; else efisLateralG=-100;
                                 interrupts();
                                 // float efisVerticalG                                   
                                 parseString=efisBufferString.substring(40, 43);
                                 noInterrupts();
                                 if (parseString!="XXX") efisVerticalG=parseString.toFloat()/10; else efisVerticalG=-100;
                                 interrupts();
                                 // int efisPercentLift                 
                                 parseString=efisBufferString.substring(43, 45);
                                 noInterrupts();
                                 if (parseString!="XX") efisPercentLift=parseString.toInt(); else efisPercentLift=-1; // 00 to 99, percentage of stall angle.
                                 interrupts();
                                 // int efisPalt
                                 parseString=efisBufferString.substring(27, 33); 
                                 noInterrupts();
                                 if (parseString!="XXXXXX") efisPalt=parseString.toInt(); else efisPalt=-10000; // feet
                                 interrupts();
                                 // int efisVSI                 
                                 parseString=efisBufferString.substring(45, 49); 
                                 noInterrupts();
                                 if (parseString!="XXXX") efisVSI=parseString.toInt()*10; else efisVSI=-10000; // feet/min 
                                 interrupts();
                                 //float efisTAS;
                                 parseString=efisBufferString.substring(52, 56);
                                 noInterrupts();
                                 if (parseString!="XXXX") efisTAS=parseString.toFloat()/10; else efisTAS=-1; // kts             
                                 interrupts();
                                 //float efisOAT;
                                 parseString=efisBufferString.substring(49, 52);
                                 noInterrupts();
                                 if (parseString!="XXX") efisOAT=parseString.toFloat(); else efisTAS=-100; // Celsius
                                 interrupts();                             
                                 // String efisTime
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
                                     String parseString;
                                     //calculate CRC
                                     int calcCRC=0;
                                     for (int i=0;i<=220;i++) calcCRC+=efisBufferString[i];
                                     calcCRC=calcCRC & 0xFF;
                                     if (calcCRC==(int)strtol(&efisBufferString.substring(221, 223)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                        {
                                       
                                        //float efisFuelRemaining=0.00;
                                         parseString=efisBufferString.substring(44, 47);
                                         noInterrupts();
                                         if (parseString!="XXX") efisFuelRemaining=parseString.toFloat()/10; else efisFuelRemaining=-1; // gallons
                                         interrupts();
                                         //float efisFuelFlow=0.00;
                                         parseString=efisBufferString.substring(29, 32);
                                         noInterrupts();
                                         if (parseString!="XXX") efisFuelFlow=parseString.toFloat()/10; else efisFuelFlow=-1; // gph
                                         interrupts();
                                         //float efisMAP=0.00;
                                         parseString=efisBufferString.substring(26, 29);
                                         noInterrupts();
                                         if (parseString!="XXX") efisMAP=parseString.toFloat()/10; else efisMAP=-1; //inHg
                                         interrupts();
                                         // int efisRPM=0;
                                         parseString=efisBufferString.substring(18, 22);
                                         noInterrupts();
                                         if (parseString!="XXXX") efisRPM=parseString.toInt(); else efisRPM=-1;
                                         interrupts();
                                         // int efisPercentPower=0;                                     
                                         parseString=efisBufferString.substring(217, 220);
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
        } // end efisType ADVANCED
        else
          if (efisType=="DYNOND10") 
             {             
                                    if (efisBufferString.length()==DYNON_SERIAL_LEN)
                                        {
                                        // parse Dynon data
                                         String parseString;
                                         //calculate CRC
                                         int calcCRC=0;
                                         for (int i=0;i<=48;i++) calcCRC+=efisBufferString[i];                     
                                         calcCRC=calcCRC & 0xFF;
                                         if (calcCRC==(int)strtol(&efisBufferString.substring(49, 51)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                            {
                                            // CRC passed
                                             parseString=efisBufferString.substring(20, 24);                                                                                                      
                                             noInterrupts();
                                             efisASI=parseString.toFloat()/10*1.94384; // m/s to knots
                                             interrupts();                                         
                                             parseString=efisBufferString.substring(8, 12);
                                             noInterrupts();
                                             efisPitch=parseString.toFloat()/10;
                                             interrupts();                                         
                                             parseString=efisBufferString.substring(12, 17);
                                             noInterrupts();
                                             efisRoll=parseString.toFloat()/10;
                                             interrupts();                                         
                                             parseString=efisBufferString.substring(33, 36);
                                             noInterrupts();
                                             efisLateralG=parseString.toFloat()/100;
                                             interrupts();                                         
                                             parseString=efisBufferString.substring(36, 39);
                                             noInterrupts();
                                             efisVerticalG=parseString.toFloat()/10;
                                             interrupts();                                         
                                             parseString=efisBufferString.substring(39, 41);
                                             noInterrupts();
                                             efisPercentLift=parseString.toInt(); // 00 to 99, percentage of stall angle
                                             interrupts();  
                                             parseString=efisBufferString.substring(45,47);
                                             long statusBitInt = strtol(&parseString[1], NULL, 16);                                                                                 
                                             if (bitRead(statusBitInt, 0))
                                                  {
                                                  // when bitmask bit 0 is 1, grab pressure altitude and VSI, otherwise use previous value (skip turn rate and density altitude)
                                                  parseString=efisBufferString.substring(24, 29);
                                                  noInterrupts();
                                                  efisPalt=parseString.toInt()*3.28084; // meters to feet
                                                  interrupts();
                                                  parseString=efisBufferString.substring(29, 33);
                                                  noInterrupts();
                                                  efisVSI= int(parseString.toFloat()/10*60); // feet/sec to feet/min
                                                  interrupts();
                                                  }                                                               
                                             efisTimestamp=millis();
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
             } // end efisType DYNOND10
             else
                if (efisType=="GARMING5")
                    {
                                            
                                          
                                          if (efisBufferString.length()==59 && efisBufferString[0]=='=' && efisBufferString[1]=='1' && efisBufferString[2]=='1')
                                                     {
                                                     // parse G5 data
                                                     String parseString;
                                                     //calculate CRC
                                                     int calcCRC=0;                           
                                                     for (int i=0;i<=54;i++) calcCRC+=efisBufferString[i];                     
                                                     calcCRC=calcCRC & 0xFF;
                                                     if (calcCRC==(int)strtol(&efisBufferString.substring(55, 57)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                                        {
                                                          // CRC passed                                                                                                          
                                                         parseString=efisBufferString.substring(23, 27);
                                                         noInterrupts();
                                                         if (parseString!="____") efisASI=parseString.toFloat()/10;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(11, 15);
                                                         noInterrupts();
                                                         if (parseString!="____") efisPitch=parseString.toFloat()/10;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(15, 20);
                                                         noInterrupts();
                                                         if (parseString!="_____") efisRoll=parseString.toFloat()/10;                                                    
                                                         interrupts();
                                                         parseString=efisBufferString.substring(20, 23);
                                                         noInterrupts();
                                                         if (parseString!="___") efisHeading=parseString.toInt();
                                                         interrupts();                                                     
                                                         parseString=efisBufferString.substring(37, 40);
                                                         noInterrupts();
                                                         if (parseString!="___") efisLateralG=parseString.toFloat()/100;                            
                                                         interrupts();
                                                         parseString=efisBufferString.substring(40, 43);
                                                         noInterrupts();
                                                         if (parseString!="___") efisVerticalG=parseString.toFloat()/10;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(27, 33);
                                                         noInterrupts();
                                                         if (parseString!="______") efisPalt=parseString.toInt(); // feet
                                                         interrupts();
                                                         parseString=efisBufferString.substring(45, 49);
                                                         noInterrupts();
                                                         if (parseString!="____") efisVSI=parseString.toInt()*10; //10 fpm
                                                         efisTimestamp=millis();
                                                         efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);   
                                                         interrupts();                                                   
                                                         #ifdef EFISDATADEBUG
                                                         Serial.printf("G5 data: efisASI %.2f, efisPitch %.2f, efisRoll %.2f, efisHeading %i, efisLateralG %.2f, efisVerticalG %.2f, efisPalt %i, efisVSI %i,efisTime %s", efisASI, efisPitch, efisRoll, efisHeading, efisLateralG, efisVerticalG,efisPalt,efisVSI,efisTime.c_str());                        
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
                    } // efisType GARMING5
                    else
                     if (efisType=="GARMING3X")
                     {
                                                    
                                          // parse G3X attitude data, 10hz
                                          if (efisBufferString.length()==59 && efisBufferString[0]=='=' && efisBufferString[1]=='1' && efisBufferString[2]=='1')
                                                     {
                                                     // parse G3X data
                                                     String parseString;
                                                     //calculate CRC
                                                     int calcCRC=0;                           
                                                     for (int i=0;i<=54;i++) calcCRC+=efisBufferString[i];                     
                                                     calcCRC=calcCRC & 0xFF;
                                                     if (calcCRC==(int)strtol(&efisBufferString.substring(55, 57)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
                                                        {
                                                        // CRC passed                                                                                                          
                                                         parseString=efisBufferString.substring(23, 27);
                                                         noInterrupts();
                                                         if (parseString!="____") efisASI=parseString.toFloat()/10;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(11, 15);
                                                         noInterrupts();
                                                         if (parseString!="____") efisPitch=parseString.toFloat()/10;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(15, 20);
                                                         noInterrupts();
                                                         if (parseString!="_____") efisRoll=parseString.toFloat()/10;                                                    
                                                         interrupts();
                                                         parseString=efisBufferString.substring(20, 23);
                                                         noInterrupts();                                                     
                                                         if (parseString!="___") efisHeading=parseString.toInt();
                                                         interrupts();                                                     
                                                         parseString=efisBufferString.substring(37, 40);
                                                         noInterrupts();
                                                         if (parseString!="___") efisLateralG=parseString.toFloat()/100;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(40, 43);
                                                         noInterrupts();                                                     
                                                         if (parseString!="___") efisVerticalG=parseString.toFloat()/10;
                                                         interrupts();
                                                         parseString=efisBufferString.substring(43, 45);
                                                         noInterrupts();
                                                         if (parseString!="__") efisPercentLift=parseString.toInt();
                                                         interrupts();
                                                         parseString=efisBufferString.substring(27, 33);
                                                         noInterrupts();
                                                         if (parseString!="______") efisPalt=parseString.toInt(); // feet
                                                         interrupts();
                                                         parseString=efisBufferString.substring(49, 52);
                                                         noInterrupts();
                                                         if (parseString!="___") efisOAT=parseString.toInt(); 
                                                         interrupts();
                                                         parseString=efisBufferString.substring(45, 49); // celsius
                                                         noInterrupts();
                                                         if (parseString!="____") efisVSI=parseString.toInt()*10; //10 fpm                                                     
                                                         efisTimestamp=millis();                                                       
                                                         efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);                                        
                                                         interrupts();                                                   
                                                         #ifdef EFISDATADEBUG
                                                         Serial.printf("G3X Attitude data: efisASI %.2f, efisPitch %.2f, efisRoll %.2f, efisHeading %i, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i,efisTime %s", efisASI, efisPitch, efisRoll, efisHeading, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTime.c_str());                        
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
                                                     String parseString;
                                                     //calculate CRC
                                                     int calcCRC=0;                           
                                                     for (int i=0;i<=216;i++) calcCRC+=efisBufferString[i];
                                                     calcCRC=calcCRC & 0xFF;
                                                     if (calcCRC==(int)strtol(&efisBufferString.substring(217, 219)[0],NULL,16)) // convert from hex back into integer for camparison, issue with missing leading zeros when comparing hex formats
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
                                                        if (parseString!="____") efisRPM=parseString.toInt();                                                    
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
                                                      
        } // end efisType GARMING3X
                                                     
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
} // readEfisData

if (readBoom)
{
  // look for serial data from boom
  while (Serial1.available()>0)
  {
  boomMaxAvailable=max(Serial1.available(),boomMaxAvailable);     
  String parseArray[4];
  int parseArrayIndex=0;  
  serialBoomChar = Serial1.read();
  lastReceivedBoomTime=millis();
  if (boomBufferString.length()>=50) boomBufferString=""; // prevent serial buffer overflow                 
  if ((boomBufferString.length()>0 || serialBoomChar=='$'))  // if empty accept only $ character (start of boom data line)
    {    
     if (serialBoomChar == char(0x0A))
        {            
            // we have the full line, verify and parse the data               
            // verify if line starting with $
              if (boomBufferString[0]=='$' && boomBufferString.length()>=21) // actual data starts at 21
              {
               //calculate CRC
              int calcCRC=0;
              for (unsigned int i=0;i<boomBufferString.length()-4;i++)
                  {                  
                  calcCRC+=boomBufferString[i];
                  }
              calcCRC=calcCRC & 0xFF;
              if (calcCRC==(int)strtol(&boomBufferString.substring(boomBufferString.length()-3, boomBufferString.length()-1)[0],NULL,16)) // convert from hex back into integer for camparison
                  {                
                  // parse data
                  for (unsigned int k=21;k<boomBufferString.length()-4;k++)
                        {                    
                        if (boomBufferString[k]==',') parseArrayIndex++; else parseArray[parseArrayIndex]+=boomBufferString[k];
                        if (parseArrayIndex>=4) break;
                        } // for                                 
                     noInterrupts();
                     boomStatic=curveCalc(parseArray[0].toInt(),boomStaticCurve);
                     interrupts();
                     noInterrupts();
                     boomDynamic=curveCalc(parseArray[1].toInt(),boomDynamicCurve);
                     if (boomDynamic*100/1.225*2>0) boomIAS=sqrt(boomDynamic*100/1.225*2)* 1.94384; else boomIAS=0;
                     interrupts();
                     noInterrupts();
                     boomAlpha=curveCalc(parseArray[2].toInt(),boomAlphaCurve);
                     interrupts();
                     noInterrupts();
                     boomBeta=curveCalc(parseArray[3].toInt(),boomBetaCurve);    
                     interrupts();
                     noInterrupts();
                     boomTimestamp=millis();
                     interrupts();
                     #ifdef BOOMDATADEBUG
                     noInterrupts();                 
                     Serial.printf("BOOM: boomStatic %.2f, boomDynamic %.2f, boomAlpha %.2f, boomBeta %.2f, boomIAS %.2f\n", boomStatic, boomDynamic, boomAlpha, boomBeta, boomIAS);
                     interrupts();
                     #endif
                   } else 
                        {
                        #ifdef BOOMDATADEBUG
                        Serial.println("BOOM: Bad CRC");
                        #endif  
                        // bad CRC    
                        }   
                    
             } // if (boomBufferString[0]='$')
            
            // drop buffer after parsing
            boomBufferString="";
            
            } else boomBufferString+=serialBoomChar;            
            
      } // if (boomBufferString.length()>0)
          #ifdef BOOMDATADEBUG
          else Serial.print(serialBoomChar); // display # for dumped characters
          #endif
  } // serial1.available
 } // if BOOM
readingSerial=false;            
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
#ifdef TONEDEBUG
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
 if (!switchState)
    {
    // if audio switch is off don't play any tones
    sinewave1.amplitude(volume);
    tonePlaying=TONE_OFF;
    return;
    }
    
  if(frequency < 20 || frequency > 20000 || toneMode == TONE_OFF) {
    #ifdef TONEDEBUG
    Serial.print("cancel tone: ");Serial.println(frequency);   
    #endif
    sinewave1.amplitude(volume); // turn off pulsed tone
    toneFreq = frequency;
    tonePlaying=TONE_OFF;
    return;
  } 
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
      sinewave1.frequency(frequency);
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
if (readingSensors) return;
readingSensors=true;   
//unsigned long sensorstarttime=micros();  
// reads sensors 50 times/second
int Pfwd;
float PfwdPascal;
float Pstatic=0.00;
int P45;
float PfwdSmoothed;
float P45Smoothed;
float aVertSmoothed;
float aLatSmoothed;
float aFwdSmoothed;
Pfwd=GetPressurePfwd()-pFwdBias;
P45=GetPressureP45()-p45Bias;
#ifdef BARO
Pstatic=GetStaticPressure();
Palt=145366*(1-pow(Pstatic/1013.2,0.190284));
#endif
#ifdef IMU
readAccelGyro(); // read accelerometers

// median filter them first to get rid of spikes
aVertMedian.add(getAccelForAxis(verticalGloadAxis));
aVertSmoothed=aVertMedian.getMedian();

aLatMedian.add(getAccelForAxis(lateralGloadAxis));
aLatSmoothed=aLatMedian.getMedian();

aFwdMedian.add(getAccelForAxis(forwardGloadAxis));
aFwdSmoothed=aFwdMedian.getMedian();

// average accelerometer and gyro values        
aVertAvg+=aVertSmoothed; aVert=aVertAvg.process().mean;
aLatAvg+=aLatSmoothed; aLat=aLatAvg.process().mean;
aFwdAvg+=aFwdSmoothed; aFwd=aFwdAvg.process().mean;

gRollAvg+=getGyroForAxis(rollGyroAxis); gRoll=gRollAvg.process().mean;
gPitchAvg+=getGyroForAxis(pitchGyroAxis); gPitch=gPitchAvg.process().mean;
gYawAvg+=getGyroForAxis(yawGyroAxis); gYaw=gYawAvg.process().mean;

// calculate accPitch
accPitchAvg+=-(atan2((- aFwd), sqrt(aLat* aLat + aVert *aVert)) * 57.2957);
accPitch=accPitchAvg.process().mean+pitchBias; // get Gaussian smoothed pitch and add bias (degrees).
#endif

flapsIndex=getFlapsIndex();
flapsPos=flapDegrees.Items[flapsIndex];
setAOApoints(flapsIndex);

PfwdMedian.add(Pfwd);
PfwdSmoothed=PfwdMedian.getMedian();

P45Median.add(P45);
P45Smoothed=P45Median.getMedian();

calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd divided by non-bias-corrected P45;

// calculate airspeed
PfwdPascal=((PfwdSmoothed+pFwdBias - 0.1*16383) * 2/(0.8*16383) -1) * 6894.757;
if (PfwdPascal>0)
    {
    ASI=sqrt(2*PfwdPascal/1.225)* 1.94384; // knots // physics based calculation
    ASI=curveCalc(ASI,casCurve);
    }
    else ASI=0;

unsigned long timeStamp=millis(); // save timestamp for logging
updateTones();

#ifdef SENSORDEBUG
    char debugSensorBuffer[500];
    sprintf(debugSensorBuffer, "timeStamp: %lu,Pfwd: %i,PfwdSmoothed: %.2f,P45: %i,P45Smoothed: %.2f,Pstatic: %.2f,Palt: %.2f,ASI: %.2f,AOA: %.2f,flapsPos: %i,VerticalG: %.2f,LateralG: %.2f,ForwardG: %.2f,RollRate: %.2f,PitchRate: %.2f,YawRate: %.2f, AccelPitch %.2f",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos,aVert,aLat,aFwd,gRoll,gPitch,gYaw,accPitch);

    if (readBoom)
      {
      sprintf(debugSensorBuffer, ",boomStatic: %.2f,boomDynamic: %.2f,boomAlpha: %.2f,boomBeta: %.2f\n",boomStatic,boomDynamic,boomAlpha,boomBeta);
      }  
    Serial.println(debugSensorBuffer);
#endif    

#ifdef SDCARD
char logLine[2048];
int charsAdded=0;
if (sdLogging)
        {
          charsAdded+=sprintf(logLine, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,ASI,AOA,flapsPos,dataMark);
          #ifdef IMU
          charsAdded+= sprintf(logLine+charsAdded, ",%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.2f",aVert,aLat,aFwd,gRoll,gPitch,gYaw,accPitch);
          #endif
          if (readBoom)
            { 
            int boomAge=millis()-boomTimestamp;
            charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge); 
            }         
          if (readEfisData)
            {
            int efisAge=millis()-efisTimestamp;                       
            charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%i,%s",efisASI,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisHeading,efisAge,efisTime.c_str());
            }          
          sprintf(logLine+charsAdded,"\n");
                
        datalogRingBufferAdd(logLine);
        }
    
  #endif

  
  //Serial.println(sensorCacheCount);
 // breathing LED
if (switchState)
    {
    float ledBrightness = 15+(exp(sin(millis()/2000.0*PI)) - 0.36787944)*63.81; // funky sine wave, https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    analogWrite(PIN_LED2, ledBrightness);
    } else analogWrite(PIN_LED2,0);
readingSensors=false;    
}

void datalogRingBufferAdd(char * logBuffer)
{ 
// add data to ringbuffer and update indexes  
if ((LOG_RINGBUFFER_SIZE - datalogBytesAvailable) < strlen(logBuffer))
      {
      // ring buffer full;
      return;
      }
int size_part1=minimum(strlen(logBuffer), LOG_RINGBUFFER_SIZE - datalogRingBufferEnd);
int size_part2=strlen(logBuffer)-size_part1;      
memcpy(datalogRingBuffer+datalogRingBufferEnd, logBuffer,size_part1);
memcpy(datalogRingBuffer+0, logBuffer + size_part1, size_part2);
// update ringbuffer end
datalogRingBufferEnd=(datalogRingBufferEnd+strlen(logBuffer)) % LOG_RINGBUFFER_SIZE; //wrap around to zero
datalogBytesAvailable+=strlen(logBuffer);
//Serial.printf("%lu: %i added, %s\n",micros(),strlen(logBuffer),logBuffer);
}

int minimum(int a,int b)
{
if (a<b) return a; else return b; 
}

void SensorWriteSD()
{
if (!sdLogging) return; 

char SDwriteCache[SD_WRITE_BLOCK_SIZE];  
#ifdef SDCARDDEBUG
int writeCount=0;
unsigned long maxWriteTime=0;
unsigned long avgWriteTime=0;
unsigned long minWriteTime=1000000;
unsigned long openTime;
unsigned long timerstart=micros();
unsigned long sdStart=timerstart;
unsigned long closeTime;
int SDwriteBytesCount=0;
#endif

#ifdef SDCARD
    #ifdef SDCARDDEBUG
    Serial.printf("Bytes available: %i\n",datalogBytesAvailable);
    #endif
    if (datalogBytesAvailable < SD_WRITE_BLOCK_SIZE)
        {
        return;
        }
    
    // Serial.println("start OpenFile");
    #ifdef SDCARDDEBUG
    timerstart=micros();
    #endif
    SensorFile = SD.open(filenameSensor, FILE_WRITE);
    if (!SensorFile)         
        {
        Serial.println("cannot open log file for writing");                                  
        return;
        }
    #ifdef SDCARDDEBUG
    openTime=micros()-timerstart;
    #endif
                    
    while (datalogBytesAvailable > SD_WRITE_BLOCK_SIZE)
        {                       
        //move sensor cache into write cache
        int size_part1 = minimum(SD_WRITE_BLOCK_SIZE, LOG_RINGBUFFER_SIZE - datalogRingBufferStart);
        int size_part2 = SD_WRITE_BLOCK_SIZE - size_part1;
        
        memcpy(SDwriteCache, datalogRingBuffer+ datalogRingBufferStart, size_part1);
        memcpy(SDwriteCache + size_part1, datalogRingBuffer+0, size_part2);
        // update ringbuffer start
        datalogRingBufferStart=(datalogRingBufferStart+SD_WRITE_BLOCK_SIZE) % LOG_RINGBUFFER_SIZE;
        #ifdef SDCARDDEBUG
        timerstart=micros();
        #endif
        
        SensorFile.write(SDwriteCache,SD_WRITE_BLOCK_SIZE);        
        noInterrupts();
        datalogBytesAvailable-=SD_WRITE_BLOCK_SIZE;
        interrupts();     
        
        #ifdef SDCARDDEBUG
        SDwriteBytesCount+=SD_WRITE_BLOCK_SIZE;
        maxWriteTime=max(maxWriteTime,micros()-timerstart);
        minWriteTime=min(minWriteTime,micros()-timerstart);
        avgWriteTime=avgWriteTime+micros()-timerstart;
        writeCount++;                    
        #endif 
                                                                              
        } // while
        
        #ifdef SDCARDDEBUG
        avgWriteTime= round(avgWriteTime/writeCount);
        timerstart=micros();                
        #endif
        
        SensorFile.close(); // this takes long sometimes      
        
        #ifdef SDCARDDEBUG
        closeTime=micros()-timerstart;
        unsigned long totalTime=micros()-sdStart;                
        Serial.printf("Open: %i, Max: %i, Min: %i, Avg: %i, Close: %i, Total: %i",openTime,maxWriteTime, minWriteTime, avgWriteTime, closeTime, totalTime);
        #endif
           
        #ifdef SDCARDDEBUG                                  
        Serial.println();
        Serial.print(SDwriteBytesCount);Serial.println(" bytes.");
        SDwriteBytesCount=0; // reset write chache
        #endif                                  
            
        #ifdef TONEDEBUG 
        Serial.print("free memory: ");Serial.print(freeMemory());
        #endif                
            
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
          
  
        Serial.println("\nSwitch on");
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
return mapfloat(adcvalue, volumeLowAnalog, volumeHighAnalog, 0, 25);
}


void LogReplay()
{
char logLine[512];
char inputChar;
int bufferIndex=0;
String valueArray[45];
int valueIndex=0;

int Pfwd;
float PfwdPascal;

int P45;
float PfwdSmoothed;
float P45Smoothed;
// read next line in logfile and set airspeed and AOA from it

while (SensorFile.available())
      {
      inputChar=SensorFile.read();             
      if (inputChar != char(0xA))
          {
          logLine[bufferIndex]=inputChar;
          bufferIndex++;
          if (bufferIndex>=511)
             {
             Serial.println("Buffer overflow while reading log file, skipping to next line");
             bufferIndex=0;
             return;
             }
          } else
                {
                // end of log line, parse it              
                    for (int i=0;i<bufferIndex;i++)
                    {
                    if (logLine[i]==',') valueIndex++; else valueArray[valueIndex]+=logLine[i];
                    Serial.print(logLine[i]);
                    }
                    Serial.println();
                   // skip CSV header line
                   if (valueArray[0].indexOf("timeStamp")>=0)
                       {
                       Serial.println("Skipping header in logfile.");
                      // find efisLaterG column index (if boom is available it will be offset by 6 columns)
                      for (int i=0;i<valueIndex;i++)
                          {
                          if (valueArray[i].indexOf("efisLateralG")>=0)
                              {
                              efisLateralGColumn=i;
                              break;
                              }
                          }                       
                      return; // skip if log header;
                       }                  
                       
                  // simulate a sensor read cycle
                  Pfwd=valueArray[1].toInt(); // bias already removed                  
                  PfwdSmoothed=valueArray[2].toFloat();                  
                  P45=valueArray[3].toInt(); // bias already removed
                  P45Smoothed=valueArray[4].toFloat();
                  flapsPos=valueArray[9].toInt();                  
                  for (int i=0; i < flapDegrees.Count;i++)
                    {
                    if (flapsPos==flapDegrees.Items[i])
                          {
                          flapsIndex=i;
                          break;
                          }                        
                    }
                    
                  setAOApoints(flapsIndex);                                    
                  calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd divided by non-bias-corrected P45;                

                  // efis lateralG
                  efisLateralG=valueArray[efisLateralGColumn].toFloat();

                  
                  // calculate airspeed
                  PfwdPascal=((PfwdSmoothed+pFwdBias - 0.1*16383) * 2/(0.8*16383) -1) * 6894.757;
                  if (PfwdPascal>0)
                      {
                      ASI=sqrt(2*PfwdPascal/1.225)* 1.94384; // knots // physics based calculation
                      }
                      else ASI=0;                                                        
                  #ifdef BARO
                  Palt=valueArray[6].toFloat();
                  //Palt=145366*(1-pow(Pstatic/1013.2,0.190284));
                  #endif

                  #ifdef IMU
                  aVert=valueArray[11].toFloat();
                  aLat=valueArray[12].toFloat();
                  aFwd=valueArray[13].toFloat();
                  gRoll=valueArray[14].toFloat();
                  gPitch=valueArray[15].toFloat();
                  gYaw=valueArray[16].toFloat();
                  
                  // calculate accPitch
                  accPitchAvg+=-(atan2((- aFwd), sqrt(aLat* aLat + aVert *aVert)) * 57.2957);
                  accPitch=accPitchAvg.process().mean+pitchBias; // get Gaussian smoothed pitch and add bias (degrees).
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
                  Serial.print(",tonemode: ");
                  // print tone type & pps
                  if (toneMode==TONE_OFF) Serial.print("TONE_OFF,");
                    else if (toneMode==PULSE_TONE) Serial.print("PULSE_TONE,");
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

void initAccelGyro()
{
 // enable the three axes of the accelerometer
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG5_XL,  0x38);
 // configure the accelerometer-specify bandwidth selection with Abw
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG6_XL, Aodr << 5 | Ascale << 3 | 0x04 | Abw);
 
 // enable the three axes of the gyroscope
 I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG4, 0x38);
 // configure the gyroscope
 I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG1_G, Godr << 5 | Gscale << 3 | Gbw);
 
}

void readAccelGyro()
{
  uint8_t temp_a[6]; // We'll read six bytes from the accelerometer into temp 
  I2CreadBytes(_i2cAddress_AccelGyro,OUT_X_L_XL, temp_a, 6); // Read 6 bytes, beginning at OUT_X_L_XL  
  ax = (temp_a[1] << 8) | temp_a[0]; // Store x-axis values into ax
  ay = (temp_a[3] << 8) | temp_a[2]; // Store y-axis values into ay
  az = (temp_a[5] << 8) | temp_a[4]; // Store z-axis values into az  
  //Serial.printf("ax:%.2f,ay:%.2f,az:%.2f\n",ax*aRes,ay*aRes,az*aRes);
  uint8_t temp_g[6];  // x/y/z gyro register data stored here
  I2CreadBytes(_i2cAddress_AccelGyro, OUT_X_L_G, temp_g,6);  // Read the six raw data registers sequentially into data array
  gx = (temp_g[1] << 8) | temp_g[0]; // Store x-axis values into gx
  gy = (temp_g[3] << 8) | temp_g[2]; // Store y-axis values into gy
  gz = (temp_g[5] << 8) | temp_g[4]; // Store z-axis values into gz  
  //Serial.printf("gx:%.2f,gy:%.2f,gz:%.2f\n",gx*gRes,gy*gRes,gz*gRes);
  // add biases   
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

float scaleAccel(int16_t accel)
{
  // Return the accel raw reading times our pre-calculated g's / (ADC tick):
  return aRes * accel;
}

float scaleGyro(int16_t accel)
{
  // Return the gyro raw reading times our pre-calculated degrees / (ADC tick):
  return gRes * accel;
}



void calcAOA (float Pfwd, float P45)
{  

float pCoeff;
if (P45!=0)   // can't divide with zero
          {
          pCoeff=Pfwd/(Pfwd-P45);
          } else            
            {
            AOA=AOA_MIN_VALUE;  // would result in division by zero. This won't really happen as we use the mid-band of the P45 sensor.
            return;
            }                      
          AOAAvg+=curveCalc(pCoeff,aoaCurve[flapsIndex]);         
          
          AOA=AOAAvg.process().mean;
          if (AOA>AOA_MAX_VALUE) AOA=AOA_MAX_VALUE; else if (AOA<AOA_MIN_VALUE) AOA=AOA_MIN_VALUE;
          if (isnan(AOA)) AOA=AOA_MIN_VALUE;
      
}




void SendDisplayData()
{
 char json_buffer[386]; 
 char crc_buffer[128];
 byte CRC=0;
 float verticalGload=sqrt(aVert*aVert+aLat*aLat+aFwd*aFwd);
 float lateralGload=aLat;

 float displayAOA;
 float displayPalt;
 float alphaVA=0.00;
 if (isnan(AOA) || ASI<muteAudioUnderIAS)
    {
    displayAOA=-100;    
    }
    else
        {
        noInterrupts();  
        displayAOA=AOA;
        interrupts();        
        }
 if (isnan(Palt)) displayPalt=-1.00; else displayPalt=Palt;
 float displayPitch=accPitch;
 //flapsPercent=(float)flapsPos/(flapDegrees.Items[flapDegrees.Count-1]-flapDegrees.Items[0])*100; //flap angle / flap total travel *100 (needed for displaying partial flap donut on display)
 sprintf(crc_buffer,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",displayAOA,displayPitch,ASI,displayPalt,verticalGload,lateralGload,alphaVA,LDmaxAOA,onSpeedAOAfast,onSpeedAOAslow,stallWarningAOA);
 for (unsigned int i=0;i<strlen(crc_buffer);i++) CRC=CRC+char(crc_buffer[i]); // claculate simple CRC
 sprintf(json_buffer, "{\"type\":\"ONSPEED\",\"AOA\":\"%.2f\",\"Pitch\":\"%.2f\",\"IAS\":\"%.2f\",\"PAlt\":\"%.2f\",\"verticalGLoad\":\"%.2f\",\"lateralGLoad\":\"%.2f\",\"alphaVA\":\"%.2f\",\"LDmax\":\"%.2f\",\"OnspeedFast\":\"%.2f\",\"OnspeedSlow\":\"%.2f\",\"OnspeedWarn\":\"%.2f\",\"CRC\":\"%i\"}",displayAOA,displayPitch,ASI,displayPalt,verticalGload,lateralGload,alphaVA,LDmaxAOA,onSpeedAOAfast,onSpeedAOAslow,stallWarningAOA,CRC);
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
if (flapPotPositions.Count<=1)  return 0;

int i=0; 
int betweenZone=0;
bool reverseOrder=false;

if (flapPotPositions.Items[0]>flapPotPositions.Items[flapPotPositions.Count-1]) reverseOrder=true;

for (i = 1; i < flapPotPositions.Count; i++)
    {  
    betweenZone=(flapPotPositions.Items[i]+flapPotPositions.Items[i-1])/2;
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
LDmaxAOA=flapLDMAXAOA.Items[flapIndex];
onSpeedAOAfast=flapONSPEEDFASTAOA.Items[flapIndex];
onSpeedAOAslow=flapONSPEEDSLOWAOA.Items[flapIndex];
stallWarningAOA=flapSTALLWARNAOA.Items[flapIndex];
}


bool loadConfigurationFile(char* filename)
{
String configString="";
// if config file exists on SD card load it, otherwise load the defaultconfig
File configFile;
  configFile = SD.open(filename, FILE_READ);
  if (configFile) {
                  while (configFile.available())
                        {
                        configString+=char(configFile.read());                                                                                      
                        }
                  // close the file:
                  configFile.close();                                    
                  return loadConfigFromString(configString);                
                  } else return false;            
}

bool saveConfigurationToFile(char* filename, String configString)
{
File configFile;  
configFile = SD.open(filename, O_WRITE | O_CREAT | O_TRUNC);
if (configFile) {
            configFile.print(configString);
            configFile.close();
            return true;
            } else
              {
              Serial.println("Could not save config file");
              return false;
              }
}


bool loadDefaultConfiguration()
{
// load default config from PROGMEM
// this config will load when there's no config on the SDcard
String configString=String(DEFAULT_CONFIG);
return loadConfigFromString(configString);
}


float curveCalc(float x, calibrationCurve curve )
{
float result=0.00;
//calculate curve

if (curve.curveType==1) // polynomial
    {
    for (int i=0; i<MAX_CURVE_COEFF;i++)
        {  
        result+=curve.Items[i]*pow(x,MAX_CURVE_COEFF-i-1);
        //Serial.printf("%.2f * pow(%.2f,%i)+",curve.Items[i],x,MAX_CURVE_COEFF-i-1);
        }
        //Serial.printf("=%.2f\n",result);
    } else 
          if (curve.curveType==2)  // logarithmic  ex: 21 * log(x) + 16.45
                {
                result=curve.Items[MAX_CURVE_COEFF-2]*log(x)+curve.Items[MAX_CURVE_COEFF-1]; //use only last two coefficients
                }
                  else 
                      if (curve.curveType==3) // exponential ex: 12.5*e^(-1.63x);
                          {                          
                          result=curve.Items[MAX_CURVE_COEFF-2]*exp(curve.Items[MAX_CURVE_COEFF-1]*x);
                          }
return result;
}

void configureAxes()
{
// get accelerometer axis from box orientation (there must be a better way to do this, but it works for now)
// orientation arrays defined as box orinetation, vertical axis (z up), lateral axis (y right), longitudinal axis (x forward)
verticalGloadAxis="";
lateralGloadAxis="";
forwardGloadAxis="";
String axisMapArray[24][5]={
                  {"FORWARD","LEFT","-Y","-Z","X"},
                  {"FORWARD","RIGHT","Y","Z","X"},
                  {"FORWARD","UP","Z","-Y","X"},
                  {"FORWARD","DOWN","-Z","Y","X"},
                  
                  {"AFT","LEFT","Y","-Z","-X"},
                  {"AFT","RIGHT","-Y","Z","-X"},
                  {"AFT","UP","Z","Y","-X"},
                  {"AFT","DOWN","-Z","-Y","-X"},
                  
                  {"LEFT","FORWARD","Y","-X","Z"},
                  {"LEFT","AFT","-Y","-X","-Z"},
                  {"LEFT","UP","Z","-X","-Y"},
                  {"LEFT","DOWN","-Z","-X","Y"},
                  
                  {"RIGHT","FORWARD","-Y","X","Z"},
                  {"RIGHT","AFT","Y","X","-Z"},
                  {"RIGHT","UP","Z","X","Y"},
                  {"RIGHT","DOWN","-Z","X","-Y"},
                  
                  {"UP","FORWARD","X","Y","Z"},
                  {"UP","AFT","X","-Y","-Z"},
                  {"UP","LEFT","X","-Z","Y"},
                  {"UP","RIGHT","X","Z","-Y"},
                  
                  {"DOWN","FORWARD","-X","-Y","Z"},
                  {"DOWN","AFT","-X","Y","-Z"},
                  {"DOWN","LEFT","-X","-Z","-Y"},
                  {"DOWN","RIGHT","-X","Z","Y"}
                  };

for (int i=0;i<24;i++)
    {    
    if (axisMapArray[i][0]==portsOrientation && axisMapArray[i][1]==boxtopOrientation)
        {
        verticalGloadAxis=axisMapArray[i][2];
        lateralGloadAxis=axisMapArray[i][3];
        forwardGloadAxis=axisMapArray[i][4];
        yawGyroAxis=verticalGloadAxis;
        pitchGyroAxis=lateralGloadAxis;
        rollGyroAxis=forwardGloadAxis;
        break; 
        }
    }
#ifdef AXISDEBUG
Serial.printf("Vertical axis: %s\n",verticalGloadAxis.c_str());
Serial.printf("Lateral axis: %s\n",lateralGloadAxis.c_str());
Serial.printf("Forward axis: %s\n",forwardGloadAxis.c_str());                   
Serial.printf("Yaw Gyro axis: %s\n",yawGyroAxis.c_str());
Serial.printf("Pitch Gyro axis: %s\n",pitchGyroAxis.c_str());
Serial.printf("Roll Gyro axis: %s\n",rollGyroAxis.c_str());                   

#endif
}

float getAccelForAxis(String accelAxis)
{
float result=0.0;
if (accelAxis[accelAxis.length()-1] == 'X') result=scaleAccel(ax)+axBias;          
    else
        if (accelAxis[accelAxis.length()-1] == 'Y') result=scaleAccel(ay)+ayBias;
            else result=scaleAccel(az)+azBias;
if (accelAxis[0]=='-') result*=-1;
return result;      
}

float getGyroForAxis(String gyroAxis)
{
float result=0.0;
if (gyroAxis[gyroAxis.length()-1] == 'X') result=scaleGyro(gx)+gxBias;          
    else
        if (gyroAxis[gyroAxis.length()-1] == 'Y') result=scaleGyro(gy)+gyBias;
            else result=scaleGyro(gz)+gzBias;
if (gyroAxis[0]=='-') result*=-1;
return result;      
}

void setVolume(int volumePercent)
{
mixer1.gain(2,10*volumePercent/100.00); // channel 2 gain (voice)
mixer1.gain(0,volumePercent/100.00);  // channel 0 gain (tones)
#ifdef VOLUMEDEBUG
Serial.printf("Volume: %i, VoiceGain: %.2f, ToneGain: %.2f\n",volumePercent,float(10*volumePercent/100.00),float(volumePercent/100.00));
#endif
}

void timersOff()
{
//turn off Timers;
ToneTimer.end(); 
SensorTimer.end();  
SerialDataTimer.end();
LiveDisplayTimer.end();
}

void timersOn()
{
SensorTimer.begin(SensorRead,SENSOR_INTERVAL);   
ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on
SerialDataTimer.begin(readSerialData,SERIAL_INTERVAL); // turn Serial timer back on
} 
