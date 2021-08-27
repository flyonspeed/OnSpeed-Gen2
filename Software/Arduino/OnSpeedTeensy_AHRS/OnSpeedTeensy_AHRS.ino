// to be compiled on Arduino 1.8.13 & Teensyduino 1.53

// TODO

// run in Gen1 mode with Efis data (fix OSH code)
// standardize data log output, OnSpeed format
// output fixed width HUD data

////////////////////////////////////////////////////
// More details at
//      http://www.flyOnSpeed.org
//      and
//      https://github.com/flyonspeed/OnSpeed-Gen2/

// reminder: check for division by zero in PCOEFF/CalcAOA

#define VERSION  "v3.2.1b"     // 5/25/2021 switched over to CP3
//"v3.2.1a"     // 3/15/2021 fixed boom curves
//"v3.2"        //static pressure bias, roll bias and cas curve type
//"v3.1.7"      // 12/08/2020 hardcoded better boom curves (4th order polys)
//"v3.1.6"      // 11/29/2020 fix box orientation left/up
//"v3.1.5"      // 11/5/2020 boom debug displays raw counts from boom data
//"v3.1.4"      // 10/23/2020 energy display improvements
//"v3.1.3"    // 10/7/2020 added serial display data smoothing, constrained %lift to 0-99%
//"v3.1.2"    // 9/29/2020 kick the dog during file transfer, fixed verticalG rounding
//"v3.1.1"    // 9/25/2020 kick the dog during sensorconfig so it doesn't reboot the box
//"v3.1"      // 9/14/2020 tuned AHRS, added watchdog timer, fixed interrupt conflict
//"v3.0.95"   // 8/28/2020 modified AHRS centripetal correction again
//"v3.0.9"    // 8/28/2020 fixed AHRS centripetal correction
//"v3.0.7"    // last modified 8/18/2020  added AHRS code (Madgwick filter with centripetal correction)
//"v3.0.65"   // last modified 8/06/2020 added Serial port 1 G3x output for display TTL data TX on v2 boxes (Vac & Lenny).
//"v3.0.6"    // last modified 5/12/2020 replaced Gaussian with exponential moving average filter
//"v3.0.5"    // last modified 4/22/2020 added Vno chime
//"v3.0.4"    // last modified 4/7/2020 modified coefficient of pressure formula
//"v3.0.3"    // last modified 3/15/2020 updated AI-Pitch network parameters
//"v3.0.2"    // last modified 3/10/2020 fixed pitchBias, confirm flap position delete, fixed Glimit audio
//"v3.0.1"    // last modified 3/8/2020 added neural network based pitch calculation
//"v3.0"      // last modified 2/28/2020 added wifi based configuration settings, asymmetric overG warning, sensor configuration, serial data output (G3X format)
//"v2.1.28"   // last modified 2/1/2020 (timed serial read) 
//"v2.1.28"   // last modified 1/31/2020 (added boom CRC)
//"v2.1.27"   // last modified 1/10/2020 (fixed G3x data parsing, crc + oat errors)
//"v2.1.26.5" // last modified 1/9/2020 (implemented ring buffer write for SD card)
//"v2.1.26"   // last modified 1/9/2020 (fixed volume control update rate)
//"v2.1.25"   // last modified 1/6/2020  (added boom junk data filter) 
//"v2.1.24"   // last modified 1/5/2020 (new 3d audio code & curve, new boom parsing code, new volume control code)
//"v2.1.23"   // last modified 1/5/2020 (fixed 3D audio curve going negative)
//"v2.1.22"   // last modified 1/4/2020 (setting mixer gain instead of individual tone volumes)
//"v2.1.21"   // last modified 1/4/2020 (added AUDIOTEST help description, and fixed Volume control sticking, add volume control during Logreplay)
//"v2.1.20"   // last modified 1/1/2020 by Lenny (added AUDIOTEST! command to test left/right audio channels, important for 3D audio)
//"v2.1.19"   // last modified 12/27/2019 by Lenny (added 3D audio. Define an audio curve in aircraft config file)

//interrupt priorities:
//Tones 16
//IMU 128
//Sensors 128
//SerialData 128
                          
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
#define GLIMIT_REPEAT_TIMEOUT   3000 // milliseconds to repeat G limit.
#define ASYMMETRIC_GYRO_LIMIT   15 // degrees/sec rotation on either axis to trigger asymmetric G limits.


// coefficient of pressure formula
//#define PCOEFF(p_fwd,p_45)  p_fwd/(2*p_fwd-p_45); // Lenny's linear CP2, original David F. Rogers formula is Pfwd/P45.
//#define PCOEFF(p_fwd,p_45)  p_fwd/(p_fwd-p_45); // CP1
#define PCOEFF(p_fwd,p_45)  p_45/p_fwd; // CP3 // ratiometric CP. CP1 & CP2 are not ratiometric. Can't divide with P45, it goes through zero on Dynon probe.

// boom curves
#define BOOM_ALPHA_CALC(x)      - 7.9205*pow(10,-13)*x*x*x*x + 1.2918*pow(10,-8)*x*x*x - 7.6702*pow(10,-5)*x*x + 0.23155*x - 323.19; //degrees
#define BOOM_BETA_CALC(x)       4.9108*pow(10,-14)*x*x*x*x - 1.4342*pow(10,-9)*x*x*x + 1.2901*pow(10,-5)*x*x - 6.5900*pow(10,-3)*x - 100.09; //degrees
#define BOOM_STATIC_CALC(x)     0.00012207*(x - 1638)*1000; // millibars
#define BOOM_DYNAMIC_CALC(x)    (0.01525902*(x - 1638)) - 100; // millibars

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
bool casCurveEnabled;


// serial output
String serialOutFormat;
String serialOutPort;
byte serialCRC;

//volume & audio control
bool volumeControl;
bool audio3D;
int volumeHighAnalog=1023;
int volumeLowAnalog=1;
// Min airspeed in knots before the audio tone is turned on
int muteAudioUnderIAS=30;
int volumePercent; // %volume
int defaultVolume;

// timers
unsigned long audio3dLastUpdate=millis();
unsigned long volumeLastUpdate=millis();
unsigned long serialoutLastUpdate=millis();
unsigned long gLimitLastWarning=millis();
unsigned long gLimitLastUpdate=millis();
unsigned long vnoLastChime=millis();
unsigned long vnoChimeLastUpdate=millis();
unsigned long displayDataLastUpdate=millis();
unsigned long lastSDWrite=millis();
unsigned long lastLedUpdate=millis();
unsigned long lastWatchdogRefresh=millis();

#define AUDIO_3D_CURVE(x)         -92.822*x*x + 20.025*x //move audio with the ball, scaling is 0.08 LateralG/ball width
bool overgWarning; // 0 - off, 1 - on
float channelGain=1.0;
float calculatedGLimitPositive;
float calculatedGLimitNegative;


// smoothing windows
int aoaSmoothing=20;  // AOA smoothing window (number of samples to lag)
int pressureSmoothing=15; // median filter window for pressure smoothing/despiking
int accSmoothing=10; // accelerometer smoothing, Simple moving average
int ahrsSmoothing=100; // ahrs smoothing, Exponential
int airspeedSmoothing=50; // airspeed smoothing, 50 sample moving average for 238hz
int serialDisplaySmoothing=200; // smoothing serial display data (LateralG, verticalG, IAS)


intArray flapDegrees;
intArray flapPotPositions;

// calibration curves
calibrationCurve aoaCurve[MAX_AOA_CURVES]; // curve equation coefficents
calibrationCurve boomAlphaCurve;
calibrationCurve boomBetaCurve;
calibrationCurve boomStaticCurve;
calibrationCurve boomDynamicCurve;
calibrationCurve casCurve; // calibrated airspeed curve (polynomial)

//setpoints
floatArray flapLDMAXAOA;
floatArray flapONSPEEDFASTAOA;
floatArray flapONSPEEDSLOWAOA;
floatArray flapSTALLWARNAOA;

//box orientation
String portsOrientation;
String boxtopOrientation;

// load factor limit
float loadLimitPositive;
float loadLimitNegative;

// vno chime
bool vnoChimeEnabled;
unsigned int vnoChimeInterval; // chime interval in seconds
int Vno; // aircraft Vno in kts;

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
float pStaticBias=0;
float gxBias;
float gyBias;
float gzBias;
float pitchBias=0.0;
float rollBias=0.0;


bool sendDisplayData=false;
bool readIMU=true;
int displayDataCounter=0;

// data mark
int dataMark=0;

volatile double coeffP; // coefficient of pressure


// interval timers
#define SENSOR_INTERVAL 20000  // microsecond interval for sensor read (50hz)
#define REPLAY_INTERVAL 20000
#define DISPLAY_INTERVAL 100000 // 100 msec (10Hz)
#define SERIAL_INTERVAL   250 // microsecod interval for Serial data read timer
#define IMU_INTERVAL    4200 // microseconds interval for IMU read

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
  #define BAUDRATE_WIFI_HS      921600    //wifi file transfer only baud rate

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
#define RANGESWEEP_LOW_AOA    4
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
#define CTRL_REG8         0x22
#define CTRL_REG9         0x23
#define STATUS_REG_1      0x27 // status register, is data available?
#define FIFO_CTRL         0x2E // enable FIFO in IMU
#define FIFO_SRC          0x2F // FIFO status register

#define I2C_COMMUNICATION_TIMEOUT 20000  // microseconds

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
//#include <Gaussian.h>         // gaussian lib used for avg out AOA values.
//#include <LinkedList.h>       // linked list is also required.
//#include <GaussianAverage.h>  // more info at https://github.com/ivanseidel/Gaussian
#include <OneButton.h>      // button click/double click detection https://github.com/mathertel/OneButton
#include <RunningMedian.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
#include "RunningAverage.h" //https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningAverage
#include "AudioSampleEnabled.h"
#include "AudioSampleDisabled.h"
#include "AudioSampleOnspeed_left_speaker.h"
#include "AudioSampleOnspeed_right_speaker.h"
#include "AudioSampleDatamark.h"
#include "AudioSampleGlimit.h"
#include "AudioSampleVnochime.h"
#include "Madgwick.h"

Madgwick filter;

volatile float smoothedPitch=0;
volatile float smoothedRoll=0;

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
    
volatile float efisIAS=0.00;
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

bool efisPacketInProgress=false;
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

   
//vectornav variables
volatile float vnAngularRateRoll=0.00;
volatile float vnAngularRatePitch=0.00;
volatile float vnAngularRateYaw=0.00;
volatile float vnVelNedNorth=0.00;
volatile float vnVelNedEast=0.00;
volatile float vnVelNedDown=0.00;
volatile float vnAccelFwd=0.00;
volatile float vnAccelLat=0.00;
volatile float vnAccelVert=0.00;
volatile float vnYaw=0.00;
volatile float vnPitch=0.00;
volatile float vnRoll=0.00;
volatile float vnLinAccFwd=0.00;
volatile float vnLinAccLat=0.00;
volatile float vnLinAccVert=0.00;
volatile float vnYawSigma=0.00;
volatile float vnRollSigma=0.00;
volatile float vnPitchSigma=0.00;
volatile float vnGnssVelNedNorth=0.00;
volatile float vnGnssVelNedEast=0.00;
volatile float vnGnssVelNedDown=0.00;
volatile byte vnGPSFix=0;
String vnTimeUTC="";
byte vnBuffer[103];
int vnBufferIndex=0;


// boom variables
String boomBufferString;
unsigned long lastReceivedBoomTime;
volatile int boomMaxAvailable=0;
volatile float boomStatic=0.0;
volatile float boomDynamic=0.0;
volatile float boomAlpha=0.0;
volatile float boomBeta=0.0;
volatile float boomIAS=0.0;
volatile unsigned long boomTimestamp=millis();

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
volatile float AOA = 0.0;                      // avaraged AOA value is stored here.
float LDmaxAOA=0.00;
float onSpeedAOAfast=0.00;
float onSpeedAOAslow=0.00;
float stallWarningAOA=0.00;
float percentLift=0.0;                     // normalized angle of attack, or lift %
float IAS = 0.0;                          // live Air Speed Indicated
volatile float smoothedIAS=0.0;                    // smoothed airspeed
volatile float prevIAS=0.0;                        // previous IAS sample (used to calculate acceleartion)
volatile float Palt=0.00;                          // pressure altitude
float currentRangeSweepValue=RANGESWEEP_LOW_AOA;
RunningMedian P45Median(pressureSmoothing);
RunningMedian PfwdMedian(pressureSmoothing);
RunningAverage PfwdAvg(10);
RunningAverage P45Avg(10);
RunningAverage IASAvg(airspeedSmoothing);
RunningAverage aVertAvg(accSmoothing);
RunningAverage aLatAvg(accSmoothing);
RunningAverage aFwdAvg(accSmoothing);
RunningAverage aVertCompAvg(accSmoothing);
RunningAverage aLatCompAvg(accSmoothing);
RunningAverage aFwdCompAvg(accSmoothing);


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

unsigned long sensorLoggingStartTime;
unsigned long last_time = millis();
char datalogRingBuffer[LOG_RINGBUFFER_SIZE]; //64Kbytes ringbuffer for datalog  
volatile static int datalogRingBufferStart=0; 
volatile static int datalogRingBufferEnd=0; 
volatile static unsigned int datalogBytesAvailable=0;


// IMU variables
uint8_t _i2cAddress_AccelGyro=LSM9DS1_AccelGyro;
volatile int16_t ax, ay, az; // ax -instantaneous
volatile float aVert, aLat, aFwd, aVertComp, aLatComp, aFwdComp; 
volatile int16_t gx, gy, gz; // gx - instantaneous
volatile float LateralGSmoothed=0.0;
volatile float PaltSmoothed=0.0;
volatile float VerticalGSmoothed=1.0; // start at 1G;
volatile float gRoll,gPitch,gYaw;
volatile float accPitch=0.0; // smoothed pitch 
volatile float accRoll=0.0; // smoothed pitch 
volatile float gyroPitch=0.0; // for debug
volatile float gyroRoll=0.0; // for debug
volatile float rawPitch=0.0; // raw pitch
float aRes=8.0 / 32768.0; // full scale /resolution (8G)

float gRes=245.0 / 32768.0; // full scale / resolution (245 degrees)

uint8_t Ascale = 3;     // accel =/-8G scale
uint8_t Aodr = B100;   // accel data sample rate,  AODR_238Hz
uint8_t Abw = B10;      // accel data bandwidth 105Hz
uint8_t Gscale = 0;   // 245 degree/sec
uint8_t Godr = B100;     // GODR_238Hz
uint8_t Gbw = B11;    // Gyro bandwith

//uint8_t Ascale = 3;     // accel full scale, 8G
//uint8_t Aodr = 2;   // accel data sample rate,  AODR_59.5Hz
//uint8_t Abw = 0;      // accel data bandwidth,  211 Hz
//uint8_t Gscale = 0;   // 245 degree/sec
//uint8_t Godr = 2;     // GODR_59Hz
//uint8_t Gbw = 0;    // 16 Hz at Godr = 59.5 Hz
//float aRes=0.000244140625; // 8g / 32768.0
//float gRes=500.0 / 32768.0; // full scale / resolution
//uint8_t Ascale = 3;     // accel full scale, 8G
//uint8_t Aodr = 6;   // accel data sample rate,  AODR_952Hz
//uint8_t Abw = 3;      // accel data bandwidth,  ABW_50Hz
//uint8_t Gscale = 1;   // 500 degree/sec
//uint8_t Godr = 6;     // GODR_952Hz
//uint8_t Gbw = 2;    // 58 Hz at Godr = 952 Hz


IntervalTimer ToneTimer;
IntervalTimer SensorTimer; // read sensors
IntervalTimer SerialDataTimer; // read serial ports
IntervalTimer PotTimer; // read potentiometer           
IntervalTimer RangeSweepTimer; // sweep through AOA range, high to low
IntervalTimer LogReplayTimer;
IntervalTimer IMUTimer; // read IMU

// volume variables
float LOW_TONE_VOLUME=.25;      // volume is 0 to 1
float HIGH_TONE_VOLUME_MIN=.25;      // high tone will ramp up from min to max
float SOLID_TONE_VOLUME=.25;
float HIGH_TONE_VOLUME_MAX=1;
int volPos=0;
int avgSlowVolPos=512;
unsigned long volumeStartTime=millis();
int flapsPos=0; // in degrees
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
                            SensorFile.print(",VerticalG,LateralG,ForwardG,RollRate,PitchRate,YawRate,Pitch,Roll");
                            #endif        
                            if (readBoom) SensorFile.print(",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge");                     
                            if (readEfisData)
                                    {
                                    if (efisType.substring(0,3)=="VN-") // VN-300 type data
                                                SensorFile.print(",AngularRateRoll,AngularRatePitch,AngularRateYaw,VelNedNorth,VelNedEast,VelNedDown,AccelFwd,AccelLat,AccelVert,Yaw,Pitch,Roll,LinAccFwd,LinAccLat,LinAccVert,YawSigma,RollSigma,PitchSigma,GnssVelNedNorth,GnssVelNedEast,GnssVelNedDown,GPSFix,DataAge, TimeUTC");
                                                else
                                                    SensorFile.print(",efisIAS,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisMagHeading,efisAge,efisTime");
                                    }               
                            SensorFile.println();
                            SensorFile.close();
                            }  else Serial.print("SensorFile opening error");        
            }
         
        
      }
        displayConsoleHelp();
        delay(100);
 
  // set up i2c ports
  Wire.begin (I2C_MASTER,0x00,I2C_PINS_18_19,I2C_PULLUP_EXT,3000000); // SDA0/SCL0 I2C at 400kHz, PS1 sensor, Pfwd   
  Wire.setOpMode(I2C_OP_MODE_ISR);
  Wire.setDefaultTimeout(2000);

  Wire2.begin(I2C_MASTER,0x00,I2C_PINS_3_4,I2C_PULLUP_EXT,3000000); // SDA2/SCL2 I2C at 400kHz, PS2 sensor, P45
  Wire2.setOpMode(I2C_OP_MODE_ISR);
  Wire2.setDefaultTimeout(2000);
  

  Wire1.begin(I2C_MASTER,0x00,I2C_PINS_37_38,I2C_PULLUP_EXT,3000000); // SDA1/SCL1 at 400Khz, IMU and baro sensor
  Wire1.setOpMode(I2C_OP_MODE_ISR);
  Wire1.setDefaultTimeout(2000);
  //Serial.printf("I2C clock Speed: %i",Wire1.getClock());

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

  if (!readBoom && serialOutFormat!="NONE" &&  serialOutPort=="Serial1")
                        {
                        Serial1.begin(115200); // if boom is not present but display is turn on serial1 (v2 feature only)
                        }

  if (!readEfisData && serialOutFormat!="NONE" &&  serialOutPort=="Serial3")
                        {
                        Serial3.begin(115200); // if efis data is not present but display is turn on serial3 (rs232 levels)
                        }
    
  digitalWrite(PIN_LED1, 1);
  
  digitalWrite(PIN_LED1, 0);
  
  ToneTimer.priority(16);
  ToneTimer.begin(tonePlayHandler,1000000/pps); // microseconds
  
  SerialDataTimer.priority(128);
  SerialDataTimer.begin(readSerialData,SERIAL_INTERVAL);   

#ifdef IMU
  IMUTimer.priority(128);
  IMUTimer.begin(ReadIMU,IMU_INTERVAL);
#endif

  lastReceivedEfisTime=millis();
  lastReceivedBoomTime=millis();
  
    
  if (dataSource=="SENSORS")
    {
    SensorTimer.priority(128);
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
                          // turn off SD loggingff
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
                                            LogReplayTimer.begin(LogReplay,REPLAY_INTERVAL);
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
 filter.begin(238); // start Madgwick filter at 238Hz
 enableWatchdog(); // start watchdog timer
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
  if(IAS <= muteAudioUnderIAS) {
  #ifdef TONEDEBUG    
  // show audio muted and debug info.
  sprintf(tempBuf, "AUDIO MUTED: Airspeed to low. Min:%i IAS:%.2f",muteAudioUnderIAS, IAS);
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

#ifdef WIFI
readWifiSerial();
#endif


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
                                 ListFile=SD.open(listfileFileName);
                                 if (ListFile) {
                                              Serial.println();
                                              Serial.print(listfileFileName);                                              
                                              Serial.println(":");
                                              // read from the file until there's nothing else in it:
                                              while (ListFile.available()) {
                                               watchdogRefresh();
                                                      Serial.flush();  
                                                      Serial.write(ListFile.read());                                             
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
                                                                                         watchdogRefresh();
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
                                                                                                                              watchdogRefresh();
                                                                                                                            delay (2500);
                                                                                                                             watchdogRefresh();
                                                                                                                             ampLeft.gain(0);
                                                                                                                             ampRight.gain(1);
                                                                                                                             Serial.println("Playing Right audio");
                                                                                                                             voice1.play(AudioSampleOnspeed_right_speaker);
                                                                                                                              watchdogRefresh();
                                                                                                                            delay (2500);
                                                                                                                             watchdogRefresh();
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





if (millis()-looptime > 1000)
      {   
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
             lastReceivedEfisTime=millis();// reset timer 
            
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
             lastReceivedBoomTime=millis(); 
             
             #ifdef BOOMDATADEBUG
              Serial.println("\n Boom data timeout. Restarting serial port 1");            
             Serial.printf("\nBoom last transmit: %i", millis()-lastReceivedBoomTime);
             #endif
             }
         }    
        
      looptime=millis();
      loopcount=0;
      charsreceived=0;     
      }

// SD card write once/sec
if (millis()-lastSDWrite>=1000)
  {
   lastSDWrite=millis(); 
   SensorWriteSD();
   //Serial.printf("ACC: fwd: %.2f,lat: %.2f ,vert: %.2f\n",aFwd,aLat,aVert);
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
if ( overgWarning && millis()-gLimitLastUpdate>=100)
    { 
    if ( millis()-gLimitLastWarning>GLIMIT_REPEAT_TIMEOUT)
        {
        noInterrupts(); 
        if (gRoll>=ASYMMETRIC_GYRO_LIMIT || gYaw>=ASYMMETRIC_GYRO_LIMIT)
            {
            calculatedGLimitPositive=loadLimitPositive*0.666;
            calculatedGLimitNegative=loadLimitNegative*0.666;
            } else
                  {
                  calculatedGLimitPositive=loadLimitPositive;
                  calculatedGLimitNegative=loadLimitNegative;
                  }
        interrupts();
        noInterrupts();
        if (aVert >= calculatedGLimitPositive || aVert <= calculatedGLimitNegative)
            {
            gLimitLastWarning=millis();  
            voice1.play(AudioSampleGlimit);
            }
        interrupts();    
        }
     gLimitLastUpdate=millis();
    }     

// vno chime
  if ( vnoChimeEnabled && millis()-vnoChimeLastUpdate>=100)
      {
        
        if (millis()-vnoLastChime>vnoChimeInterval*1000)
              {             
              noInterrupts();
              if (IAS>Vno)
                 {
                  vnoLastChime=millis();
                  voice1.play(AudioSampleVnochime);
                 }
              interrupts();   
              }
      vnoChimeLastUpdate=millis();        
      }
      

// write serialout data
if (serialOutPort!="NONE" && millis()-serialoutLastUpdate>200) // update every 200ms
    {
          // send serial data if enabled
          if (serialOutFormat == "G3X")
              {
              // send G3X formatted data
              char g3xoutString[59];
              int percentLift;
              float totalGload;
              float smoothingAlpha=2.0/(serialDisplaySmoothing+1);
              float IASsmoothed;

              noInterrupts(); 
              IASsmoothed=smoothedIAS; // send IAS in Kts
              //IASsmoothed=smoothedIAS * 1.15078; // send IAS in MPH
              interrupts();
              noInterrupts();
              if (PaltSmoothed==0) PaltSmoothed=Palt; else PaltSmoothed=Palt * smoothingAlpha/10+ (1-smoothingAlpha/10)*PaltSmoothed; // increased smoothing needed
              interrupts();
              
              // gloads changed by the IMU interrupt              
              noInterrupts();
              totalGload=sqrt(abs(aVert*aVert+aLat*aLat+aFwd*aFwd));
              totalGload=ceil(totalGload * 10.0) / 10.0; // round to 1 decimal place
              if (aVert<0) totalGload*=-1;
              interrupts();
              VerticalGSmoothed=totalGload * smoothingAlpha+ (1-smoothingAlpha)*VerticalGSmoothed;
              
              noInterrupts();                           
              LateralGSmoothed=aLat * smoothingAlpha+ (1-smoothingAlpha)*LateralGSmoothed;
              interrupts();
              // don't output precentLift at low speeds.
              if (IASsmoothed>=muteAudioUnderIAS)
                  {              
                  noInterrupts();
                  percentLift=AOA* 60/((onSpeedAOAfast+onSpeedAOAslow)/2); // onSpeedAOA (middle of the onspeed band); // scale/align percent lift so Onspeed = 60% lift
                  interrupts();
                  constrain(percentLift,0,99);
                  } else
                        {
                        percentLift=0;                        
                        }
              noInterrupts();
              int serialPitch=int((smoothedPitch+pitchBias)*10);
              interrupts();
              noInterrupts();
              int serialRoll=int(smoothedRoll*10);
              interrupts();
              
              sprintf(g3xoutString,"=1100000000%+04i%+05i___%04u%+06i____%+03i%+03i%02u__________",serialPitch,serialRoll,unsigned(IASsmoothed*10),int(PaltSmoothed),int(LateralGSmoothed*100),int(VerticalGSmoothed*10),unsigned(percentLift));             
              serialCRC=0x00;
              for (int i=0;i<=54;i++) serialCRC+=g3xoutString[i];
              //serialCRC= serialCRC & 0xFF;
              if (serialOutPort=="Serial5")
                  {                 
                  Serial5.print(g3xoutString); 
                  Serial5.print(serialCRC,HEX);
                  Serial5.println();                  
                  } else
                        if (serialOutPort=="Serial3")
                            {
                             if (!Serial3) 
                                {
                                Serial3.begin(115200);
                                }
                            Serial3.print(g3xoutString);    
                            Serial3.print(serialCRC,HEX);
                            Serial3.println();
                            } else
                                  if (serialOutPort=="Serial1")
                                      {
                                       if (!Serial1) 
                                          {
                                          Serial1.begin(115200);
                                          }
                                      Serial1.print(g3xoutString);    
                                      Serial1.print(serialCRC,HEX);
                                      Serial1.println();
                                      }
                  
              } else
                    if (serialOutFormat == "ONSPEED")
                        {
                        // send ONSPEED formatted data
                        }
    }
    

if (sendDisplayData && millis()-displayDataLastUpdate>100) // update every 100ms (10Hz)
    {
    SendDisplayData();
    displayDataLastUpdate=millis();
    }

if (millis()-lastWatchdogRefresh>500) // refresh watchdog twice per sec
    {    
    watchdogRefresh();
    lastWatchdogRefresh=millis();
    }
}  // loop


void readSerialData()
{
if (readingSerial) 
  {
  return;
  }
readingSerial=true; 
if (readEfisData)
    {
    if (efisType.substring(0,3)=="VN-") // VN-300 type data
      {
              // parse VN-300 and VN-100 data
             unsigned long packetStartTime=micros();
             while (Serial3.available()>0 && micros()-packetStartTime<10) // timeout after 10uSec
                  {
                  // receive one line of data and break                                    
                  byte vn_inByte=Serial3.read();
                  lastReceivedEfisTime=millis();
                  charsreceived++;
                  if (vn_inByte == 0xFA )
                      {
                      efisPacketInProgress=true;                     
                      vnBuffer[0]=0xFA;
                      vnBufferIndex=1; // reset buffer index when sync byte is received
                      continue;
                      }
                  if (vnBufferIndex<103 && efisPacketInProgress)
                      {
                      // add character to buffer
                      vnBuffer[vnBufferIndex]=vn_inByte;
                      vnBufferIndex++;
                      if (vnBufferIndex==103)
                          {
                          // got full packet, check header
                          byte packetHeader[8]={0xFA,0x19,0xA0,0x01,0x91,0x00,0x42,0x01};
                          if (memcmp(vnBuffer,packetHeader,8)!=0)
                                {
                                // bad packet header, dump packet
                                efisPacketInProgress=false;
                                Serial.println("bad VN packet header");
                                continue;
                                }
                          // check CRC
                          uint16_t vnCrc = 0;
                          
                          for (int i = 1; i < 103; i++) // startign after the sync byte
                              {
                                vnCrc = (uint16_t) (vnCrc >> 8) | (vnCrc << 8);
                            
                                vnCrc ^= (uint8_t) vnBuffer[i];
                                vnCrc ^= (uint16_t) (((uint8_t) (vnCrc & 0xFF)) >> 4);
                                vnCrc ^= (uint16_t) ((vnCrc << 8) << 4);
                                vnCrc ^= (uint16_t) (((vnCrc & 0xFF) << 4) << 1);
                              }
                           if (vnCrc!=0)
                                {
                                // bad CRC, dump packet  
                                efisPacketInProgress=false;
                                Serial.println("bad VN packet CRC");
                                continue;                                                                                               
                                }

                          // process packet data
//                          for (int i=0;i<vnBufferIndex;i++)
//                              {
//                              Serial.print(vnBuffer[i],HEX);
//                              Serial.print(" ");
//                              }
                             // common
                             vnAngularRateRoll=array2float(vnBuffer,8);
                             vnAngularRatePitch=array2float(vnBuffer,12);
                             vnAngularRateYaw=array2float(vnBuffer,16);
                             
                             vnVelNedNorth=array2float(vnBuffer,20);
                             vnVelNedEast=array2float(vnBuffer,24);
                             vnVelNedDown=array2float(vnBuffer,28);
                             
                             vnAccelFwd=array2float(vnBuffer,32);
                             vnAccelLat=array2float(vnBuffer,36);
                             vnAccelVert=array2float(vnBuffer,40);

                            
                             // gnss
                             //vnTimeUTC= vnBuffer,44 (+8 bytes);
                             //int8_t vnYear=int8_t(vnBuffer[44]);
                             //uint8_t vnMonth=uint8_t(vnBuffer[45]);
                             //uint8_t vnDay=uint8_t(vnBuffer[46]);
                             uint8_t vnHour=uint8_t(vnBuffer[47]);
                             uint8_t vnMin=uint8_t(vnBuffer[48]);
                             uint8_t vnSec=uint8_t(vnBuffer[49]);                             
                             //uint16_t vnFracSec=(vnBuffer[51] << 8) | vnBuffer[50]; // gps fractional seconds only update at GPS update rates, 5Hz. We'll calculate our own
                             // calculate fractional seconds 1/100
                             String vnFracSec=String(int(millis()/10));
                             vnFracSec=vnFracSec.substring(vnFracSec.length()-2);
                             vnTimeUTC=String(vnHour)+":"+String(vnMin)+":"+String(vnSec)+"."+vnFracSec;
                             vnGPSFix=vnBuffer[52];
                             vnGnssVelNedNorth=array2float(vnBuffer,53);
                             vnGnssVelNedEast=array2float(vnBuffer,57);
                             vnGnssVelNedDown=array2float(vnBuffer,61);
                             
                             // attitude
                             vnYaw=array2float(vnBuffer,65);
                             vnPitch=array2float(vnBuffer,69);
                             vnRoll=array2float(vnBuffer,73);
                             
                             vnLinAccFwd=array2float(vnBuffer,77);
                             vnLinAccLat=array2float(vnBuffer,81);
                             vnLinAccVert=array2float(vnBuffer,85);
                             
                             vnYawSigma=array2float(vnBuffer,89);
                             vnRollSigma=array2float(vnBuffer,93);
                             vnPitchSigma=array2float(vnBuffer,97);
                             
                             efisTimestamp=millis();
                             
                             #ifdef EFISDATADEBUG
                             Serial.printf("\nvnAngularRateRoll: %.2f,vnAngularRatePitch: %.2f,vnAngularRateYaw: %.2f,vnVelNedNorth: %.2f,vnVelNedEast: %.2f,vnVelNedDown: %.2f,vnAccelFwd: %.2f,vnAccelLat: %.2f,vnAccelVert: %.2f,vnYaw: %.2f,vnPitch: %.2f,vnRoll: %.2f,vnLinAccFwd: %.2f,vnLinAccLat: %.2f,vnLinAccVert: %.2f,vnYawSigma: %.2f,vnRollSigma: %.2f,vnPitchSigma: %.2f,vnGnssVelNedNorth: %.2f,vnGnssVelNedEast: %.2f,vnGnssVelNedDown: %.2f,vnGPSFix: %i,TimeUTC: %s\n",vnAngularRateRoll,vnAngularRatePitch,vnAngularRateYaw,vnVelNedNorth,vnVelNedEast,vnVelNedDown,vnAccelFwd,vnAccelLat,vnAccelVert,vnYaw,vnPitch,vnRoll,vnLinAccFwd,vnLinAccLat,vnLinAccVert,vnYawSigma,vnRollSigma,vnPitchSigma,vnGnssVelNedNorth,vnGnssVelNedEast,vnGnssVelNedDown,vnGPSFix,vnTimeUTC.c_str());
                             #endif
                          efisPacketInProgress=false;    
                          }                        
                      } // else if buffer overflow don't do anything               
                 } // while
       }  else
                    {                    
                    //read EFIS data
                    int efisCharCount=0;
                     while (Serial3.available()>0 && efisCharCount<1024)
                        {
                        efisCharCount++;
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
                                               //float efisIAS                            
                                               parseString=efisBufferString.substring(23, 27); 
                                               if (parseString!="XXXX") efisIAS=parseString.toFloat()/10; else efisIAS=-1; // knots
                                               //float efisPitch                             
                                               parseString=efisBufferString.substring(11, 15);
                                               if (parseString!="XXXX") efisPitch=parseString.toFloat()/10; else efisPitch=-100; // degrees
                                               // float efisRoll
                                               parseString=efisBufferString.substring(15, 20);
                                               if (parseString!="XXXXX") efisRoll=parseString.toFloat()/10; else efisRoll=-100; // degrees
                                              // float MagneticHeading                                   
                                               parseString=efisBufferString.substring(20, 23);
                                               if (parseString!="XXX") efisHeading=parseString.toInt(); else efisHeading=-1;                             
                                               // float efisLateralG
                                               parseString=efisBufferString.substring(37, 40);
                                               if (parseString!="XXX") efisLateralG=parseString.toFloat()/100; else efisLateralG=-100;
                                               // float efisVerticalG                                   
                                               parseString=efisBufferString.substring(40, 43);
                                               if (parseString!="XXX") efisVerticalG=parseString.toFloat()/10; else efisVerticalG=-100;
                                               // int efisPercentLift                 
                                               parseString=efisBufferString.substring(43, 45);
                                               if (parseString!="XX") efisPercentLift=parseString.toInt(); else efisPercentLift=-1; // 00 to 99, percentage of stall angle.
                                               // int efisPalt
                                               parseString=efisBufferString.substring(27, 33); 
                                               if (parseString!="XXXXXX") efisPalt=parseString.toInt(); else efisPalt=-10000; // feet
                                               // int efisVSI                 
                                               parseString=efisBufferString.substring(45, 49); 
                                               if (parseString!="XXXX") efisVSI=parseString.toInt()*10; else efisVSI=-10000; // feet/min 
                                               //float efisTAS;
                                               parseString=efisBufferString.substring(52, 56);
                                               if (parseString!="XXXX") efisTAS=parseString.toFloat()/10; else efisTAS=-1; // kts             
                                               //float efisOAT;
                                               parseString=efisBufferString.substring(49, 52);
                                               if (parseString!="XXX") efisOAT=parseString.toFloat(); else efisTAS=-100; // Celsius
                                               // String efisTime
                                               efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);                             
                                               efisTimestamp=millis();
                                               #ifdef EFISDATADEBUG
                                               Serial.printf("SKYVIEW ADAHRS: efisIAS %.2f, efisPitch %.2f, efisRoll %.2f, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i, efisTAS %.2f, efisOAT %.2f, efisHeading %i ,efisTime %s\n", efisIAS, efisPitch, efisRoll, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisHeading, efisTime.c_str());                        
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
                                                       if (parseString!="XXX") efisFuelRemaining=parseString.toFloat()/10; else efisFuelRemaining=-1; // gallons
                                                       //float efisFuelFlow=0.00;
                                                       parseString=efisBufferString.substring(29, 32);
                                                       if (parseString!="XXX") efisFuelFlow=parseString.toFloat()/10; else efisFuelFlow=-1; // gph
                                                       //float efisMAP=0.00;
                                                       parseString=efisBufferString.substring(26, 29);
                                                       if (parseString!="XXX") efisMAP=parseString.toFloat()/10; else efisMAP=-1; //inHg
                                                       // int efisRPM=0;
                                                       parseString=efisBufferString.substring(18, 22);
                                                       if (parseString!="XXXX") efisRPM=parseString.toInt(); else efisRPM=-1;
                                                       // int efisPercentPower=0;                                     
                                                       parseString=efisBufferString.substring(217, 220);
                                                       if (parseString!="XXX") efisPercentPower=parseString.toInt(); else efisPercentPower=-1;                                                            
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
                                                           efisIAS=parseString.toFloat()/10*1.94384; // m/s to knots
                                                           parseString=efisBufferString.substring(8, 12);
                                                           efisPitch=parseString.toFloat()/10;
                                                           parseString=efisBufferString.substring(12, 17);
                                                           efisRoll=parseString.toFloat()/10;
                                                           parseString=efisBufferString.substring(33, 36);
                                                           efisLateralG=parseString.toFloat()/100;
                                                           parseString=efisBufferString.substring(36, 39);
                                                           efisVerticalG=parseString.toFloat()/10;
                                                           parseString=efisBufferString.substring(39, 41);
                                                           efisPercentLift=parseString.toInt(); // 00 to 99, percentage of stall angle
                                                           parseString=efisBufferString.substring(45,47);
                                                           long statusBitInt = strtol(&parseString[1], NULL, 16);                                                                                 
                                                           if (bitRead(statusBitInt, 0))
                                                                {
                                                                // when bitmask bit 0 is 1, grab pressure altitude and VSI, otherwise use previous value (skip turn rate and density altitude)
                                                                parseString=efisBufferString.substring(24, 29);
                                                                efisPalt=parseString.toInt()*3.28084; // meters to feet
                                                                parseString=efisBufferString.substring(29, 33);
                                                                efisVSI= int(parseString.toFloat()/10*60); // feet/sec to feet/min
                                                                }                                                               
                                                           efisTimestamp=millis();
                                                           efisTime=efisBufferString.substring(0, 2)+":"+efisBufferString.substring(2, 4)+":"+efisBufferString.substring(4, 6)+"."+efisBufferString.substring(6, 8);                                        
                                                           #ifdef EFISDATADEBUG                     
                                                           Serial.printf("D10: efisIAS %.2f, efisPitch %.2f, efisRoll %.2f, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i, efisTime %s\n", efisIAS, efisPitch, efisRoll, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTime.c_str());                                                                 
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
                                                                       if (parseString!="____") efisIAS=parseString.toFloat()/10;
                                                                       parseString=efisBufferString.substring(11, 15);
                                                                       if (parseString!="____") efisPitch=parseString.toFloat()/10;
                                                                       parseString=efisBufferString.substring(15, 20);
                                                                       if (parseString!="_____") efisRoll=parseString.toFloat()/10;                                                    
                                                                       parseString=efisBufferString.substring(20, 23);
                                                                       if (parseString!="___") efisHeading=parseString.toInt();
                                                                       parseString=efisBufferString.substring(37, 40);
                                                                       if (parseString!="___") efisLateralG=parseString.toFloat()/100;                            
                                                                       parseString=efisBufferString.substring(40, 43);
                                                                       if (parseString!="___") efisVerticalG=parseString.toFloat()/10;
                                                                       parseString=efisBufferString.substring(27, 33);
                                                                       if (parseString!="______") efisPalt=parseString.toInt(); // feet
                                                                       parseString=efisBufferString.substring(45, 49);
                                                                       if (parseString!="____") efisVSI=parseString.toInt()*10; //10 fpm
                                                                       efisTimestamp=millis();
                                                                       efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);   
                                                                       #ifdef EFISDATADEBUG
                                                                       Serial.printf("G5 data: efisIAS %.2f, efisPitch %.2f, efisRoll %.2f, efisHeading %i, efisLateralG %.2f, efisVerticalG %.2f, efisPalt %i, efisVSI %i,efisTime %s", efisIAS, efisPitch, efisRoll, efisHeading, efisLateralG, efisVerticalG,efisPalt,efisVSI,efisTime.c_str());                        
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
                                                                       if (parseString!="____") efisIAS=parseString.toFloat()/10;
                                                                       parseString=efisBufferString.substring(11, 15);
                                                                       if (parseString!="____") efisPitch=parseString.toFloat()/10;
                                                                       parseString=efisBufferString.substring(15, 20);
                                                                       if (parseString!="_____") efisRoll=parseString.toFloat()/10;                                                    
                                                                       parseString=efisBufferString.substring(20, 23);
                                                                       if (parseString!="___") efisHeading=parseString.toInt();
                                                                       parseString=efisBufferString.substring(37, 40);
                                                                       if (parseString!="___") efisLateralG=parseString.toFloat()/100;
                                                                       parseString=efisBufferString.substring(40, 43);
                                                                       if (parseString!="___") efisVerticalG=parseString.toFloat()/10;
                                                                       parseString=efisBufferString.substring(43, 45);
                                                                       if (parseString!="__") efisPercentLift=parseString.toInt();
                                                                       parseString=efisBufferString.substring(27, 33);
                                                                       if (parseString!="______") efisPalt=parseString.toInt(); // feet
                                                                       parseString=efisBufferString.substring(49, 52);
                                                                       if (parseString!="___") efisOAT=parseString.toInt(); 
                                                                       parseString=efisBufferString.substring(45, 49); // celsius
                                                                       if (parseString!="____") efisVSI=parseString.toInt()*10; //10 fpm                                                     
                                                                       efisTimestamp=millis();                                                       
                                                                       efisTime=efisBufferString.substring(3, 5)+":"+efisBufferString.substring(5, 7)+":"+efisBufferString.substring(7, 9)+"."+efisBufferString.substring(9, 11);                                        
                                                                       #ifdef EFISDATADEBUG
                                                                       Serial.printf("G3X Attitude data: efisIAS %.2f, efisPitch %.2f, efisRoll %.2f, efisHeading %i, efisLateralG %.2f, efisVerticalG %.2f, efisPercentLift %i, efisPalt %i, efisVSI %i,efisTime %s", efisIAS, efisPitch, efisRoll, efisHeading, efisLateralG, efisVerticalG, efisPercentLift,efisPalt,efisVSI,efisTime.c_str());                        
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
                                                                      if (parseString!="___") efisFuelRemaining=parseString.toFloat()/10;
                                                                      parseString=efisBufferString.substring(29, 32);
                                                                      if (parseString!="___") efisFuelFlow=parseString.toFloat()/10;                                                    
                                                                      parseString=efisBufferString.substring(26, 29);
                                                                      if (parseString!="___") efisMAP=parseString.toFloat()/10;
                                                                      parseString=efisBufferString.substring(18, 22);
                                                                      if (parseString!="____") efisRPM=parseString.toInt();                                                    
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
                        else
                            if (efisType=="VN-300")
                                {
                                
                                }
                      
                                                                   
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
                  } // read efis type data
                
             
} // readEfisData
 

if (readBoom)
{
  // look for serial data from boom
  int boomCharCount=0;
  while (Serial1.available()>0 && boomCharCount<1024)
  {
  boomCharCount++;
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
                   
                    boomStatic=BOOM_STATIC_CALC(parseArray[0].toInt());
                    boomDynamic=BOOM_DYNAMIC_CALC(parseArray[1].toInt());                    
                    if (boomDynamic*100/1.225*2>0) boomIAS=sqrt(boomDynamic*100/1.225*2)* 1.94384; else boomIAS=0;
                    boomAlpha=BOOM_ALPHA_CALC(parseArray[2].toInt());
                    boomBeta=BOOM_BETA_CALC(parseArray[3].toInt());
                    
                    boomTimestamp=millis();
                    #ifdef BOOMDATADEBUG
                    Serial.printf("BOOM: boomStatic %.2f, boomDynamic %.2f, boomAlpha %.2f, boomBeta %.2f, boomIAS %.2f\n", boomStatic, boomDynamic, boomAlpha, boomBeta, boomIAS);
                    //Serial.printf("BOOM: boomStatic %i, boomDynamic %i, boomAlpha %i, boomBeta %i,\n", parseArray[0].toInt(), parseArray[1].toInt(), parseArray[2].toInt(), parseArray[3].toInt());
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

void readWifiSerial()
{
// read Wifi serial
// look for wifi serial command
int wifiCharCount=0;
while (Serial4.available()>0 && wifiCharCount<512)
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
wifiCharCount++;
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
                          watchdogRefresh();
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
                                        watchdogRefresh();                                           
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
                                 Serial4.begin(BAUDRATE_WIFI_HS);
                                 watchdogRefresh();
                                 delay(1000); // wait a bit after changing baud rates
                                 watchdogRefresh();                                 
                                 ListFile=SD.open(listfileFileName);
                                 if (ListFile) {     
                                              // read from the file until the end
                                              int outcount=0;
                                              unsigned long watchdogTimeout=millis();
                                              while (ListFile.available())
                                              {                                                                                                                                                                                                                                                                                                              
                                                     Serial4.flush(); //wait until output buffer is ready
                                                     Serial4.write(ListFile.read());
                                                     outcount++;                                                    
                                                    if (outcount>=2048)
                                                          {
                                                          outcount=0;
                                                          filesendtimer=millis();
                                                          filesendtimeout=false;
                                                          // hold here until an ACK is received on serial.  
                                                          while(true)
                                                                      {
                                                                      if ((millis()-watchdogTimeout) >1000)
                                                                                  {
                                                                                  watchdogRefresh();
                                                                                  watchdogTimeout=millis();
                                                                                  } 
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
                                         //LiveDisplayTimer.end(); // stop display data 
                                         sendDisplayData=false;                                        
                                         Serial.println("STOPPED Live Data. (Wifi Request)");
                                         } else
                                              if (strstr(serialWifiCmdBuffer, "$STARTLIVEDATA"))
                                                 {
                                                 //LiveDisplayTimer.begin(SendDisplayData,DISPLAY_INTERVAL); // start display data
                                                 sendDisplayData=true;
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
                                                                      watchdogRefresh();
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
                                                                          watchdogRefresh();
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
                                                                                    watchdogRefresh();
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
                                                                                            watchdogRefresh();
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
                                                                                                watchdogRefresh();
                                                                                                delay (2500);
                                                                                                watchdogRefresh();
                                                                                                 ampLeft.gain(0);
                                                                                                 ampRight.gain(1);
                                                                                                 Serial.println("Wifi: Playing Right audio test");
                                                                                                 voice1.play(AudioSampleOnspeed_right_speaker);
                                                                                                watchdogRefresh(); 
                                                                                                delay (2500);
                                                                                                watchdogRefresh();
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
                                                                                                                                                      Serial.println("CONFIG checksum failed");
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
                                                                                                                                                        while (millis()-starttime<=20)
                                                                                                                                                              {
                                                                                                                                                              watchdogRefresh();
                                                                                                                                                              }                                                                                                                                                        
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
                                                                                                                                                                while (micros()-delayTimer<200)
                                                                                                                                                                      {
                                                                                                                                                                      watchdogRefresh();
                                                                                                                                                                      }; // transmit to delay to avoid output buffer overrun
                                                                                                                                                                }
                                                                                                                                                            timersOn();    
                                                                                                                                                            } else
                                                                                                                                                                if (strstr(serialWifiCmdBuffer, "$SENSORCONFIG"))
                                                                                                                                                                    {
                                                                                                                                                                    // sensor bias configuration
                                                                                                                                                                     float aircraftPitch=stringToFloat(getConfigValue(serialWifiCmdBuffer,"AIRCRAFTPITCH"));
                                                                                                                                                                     float aircraftRoll=stringToFloat(getConfigValue(serialWifiCmdBuffer,"AIRCRAFTROLL"));
                                                                                                                                                                     float aircraftPAlt=stringToFloat(getConfigValue(serialWifiCmdBuffer,"AIRCRAFTPALT"));
                                                                                                                                                                     timersOff();
                                                                                                                                                                     sdLogging=false;
                                                                                                                                                                     Serial.println("Wifi: SensorConfig request");
                                                                                                                                                                     // sample sensors
                                                                                                                                                                     long P45Total=0;
                                                                                                                                                                     long PFwdTotal=0;
                                                                                                                                                                     long PStaticTotal=0;
                                                                                                                                                                     float aVertTotal=0.00;
                                                                                                                                                                     float aLatTotal=0.00;
                                                                                                                                                                     float aFwdTotal=0.00;
                                                                                                                                                                     float gxTotal=0.00;
                                                                                                                                                                     float gyTotal=0.00;
                                                                                                                                                                     float gzTotal=0.00;
                                                                                                                                                                     int sensorReadCount=150;                             
                                                                                                                                                                     for (int i=0;i<sensorReadCount;i++)
                                                                                                                                                                          {
                                                                                                                                                                          watchdogRefresh();
                                                                                                                                                                          P45Total+=GetPressureP45();
                                                                                                                                                                          PFwdTotal+=GetPressurePfwd();
                                                                                                                                                                          PStaticTotal+=GetStaticPressure();
                                                                                                                                                                          readIMU=false; // disable IMU updates
                                                                                                                                                                          delay(10);
                                                                                                                                                                          readAccelGyro();
                                                                                                                                                                          aVertTotal+=getAccelForAxis(verticalGloadAxis);                                                                                                                                                                          
                                                                                                                                                                          aLatTotal+=getAccelForAxis(lateralGloadAxis);
                                                                                                                                                                          aFwdTotal+=getAccelForAxis(forwardGloadAxis);
                                                                                                                                                                          gxTotal+=scaleGyro(gx);
                                                                                                                                                                          gyTotal+=scaleGyro(gy);
                                                                                                                                                                          gzTotal+=scaleGyro(gz);
                                                                                                                                                                          delay(25);
                                                                                                                                                                          readIMU=true;
                                                                                                                                                                          }
                                                                    
                                                                                                                                                                     // calculate pitch from averaged accelerometer reading
                                                                                                                                                                     float calcPitch=-atan2((- aFwdTotal/sensorReadCount), sqrt((aLatTotal/sensorReadCount)* (aLatTotal/sensorReadCount) + (aVertTotal/sensorReadCount) *(aVertTotal/sensorReadCount))) * 57.2957;
                                                                                                                                                                     float calcRoll = atan2((aLatTotal/sensorReadCount) , (aVertTotal/sensorReadCount)) * 57.3;
                                                                                                                                                                     // adjust pitch bias (aircraft pitch -calcPitch)
                                                                                                                                                                     pitchBias=aircraftPitch-calcPitch;
                                                                                                                                                                     rollBias=aircraftRoll-calcRoll;
                                                                                                                                                                     pFwdBias= int((PFwdTotal/sensorReadCount));
                                                                                                                                                                     p45Bias= int((P45Total/sensorReadCount));
                                                                                                                                                                     float PStatic= float((PStaticTotal/sensorReadCount));
                                                                                                                                                                     float aircraftPressure=29.92125535*pow(((288-0.0065*0.3048*aircraftPAlt)/288),5.2561)*33.8639; // https://www.weather.gov/media/epz/wxcalc/stationPressure.pdf from inHg to milliBars.
                                                                                                                                                                     pStaticBias=aircraftPressure - PStatic;

                                                                                                                                                                     
                                                                                                                                                                     gxBias=-gxTotal/sensorReadCount;
                                                                                                                                                                     gyBias=-gyTotal/sensorReadCount;
                                                                                                                                                                     gzBias=-gzTotal/sensorReadCount;
                                                                                                                                                                     
 
                                                                                                                                                                     String resultString="PfwdBias: "+ String(pFwdBias)+"<br>";
                                                                                                                                                                     resultString+=" P45Bias: "+ String(p45Bias)+"<br>";
                                                                                                                                                                     resultString+=" gxBias: "+ floatToString(gxBias)+"<br>";
                                                                                                                                                                     resultString+=" gyBias: "+ floatToString(gyBias)+"<br>";
                                                                                                                                                                     resultString+=" gzBias: "+ floatToString(gzBias)+"<br>";
                                                                                                                                                                     resultString+=" measured Pitch: "+ String(calcPitch)+"<br>"; 
                                                                                                                                                                     resultString+=" pitchBias: "+ String(pitchBias)+"<br>";
                                                                                                                                                                     resultString+=" measured Roll: "+ String(calcRoll)+"<br>"; 
                                                                                                                                                                     resultString+=" rollBias: "+ String(rollBias)+"<br>"; 
                                                                                                                                                                     resultString+=" staticBias: "+ String(pStaticBias)+"<br>";                                                                                                                                                                     
                                                                                                                                                                     String configString=configurationToString();
                                                                                                                                                                     saveConfigurationToFile(configFilename,configString);
                                                                                                                                                                     Serial4.println("<SENSORCONFIG>"+resultString+"</SENSORCONFIG>");
                                                                                                                                                                     Serial.println("Wifi: SensorConfig complete.");
                                                                                                                                                                     timersOn();
                                                                                                                                                                     sdLogging=true;                                                                                                                                                                
                                                                                                                                                                    } else
                                                                                                                                                                            if (strstr(serialWifiCmdBuffer, "$PITCHROLLALT"))
                                                                                                                                                                                        {
                                                                                                                                                                                        int sensorReadCount=150;
                                                                                                                                                                                        float aVertTotal=0.00;
                                                                                                                                                                                        float aLatTotal=0.00;
                                                                                                                                                                                        float aFwdTotal=0.00;
                                                                                                                                                                                        long PStaticTotal=0;
                                                                                                                                                                                        Serial.println("Wifi: PitchRollAlt request");                                                                                                                                                                                          
                                                                                                                                                                                        for (int i=0;i<sensorReadCount;i++)
                                                                                                                                                                                            {                                                                                                                                                                                            
                                                                                                                                                                                            watchdogRefresh();
                                                                                                                                                                                            aVertTotal+=getAccelForAxis(verticalGloadAxis);                                                                                                                                                                          
                                                                                                                                                                                            aLatTotal+=getAccelForAxis(lateralGloadAxis);
                                                                                                                                                                                            aFwdTotal+=getAccelForAxis(forwardGloadAxis);
                                                                                                                                                                                            PStaticTotal+=GetStaticPressure();
                                                                                                                                                                                            delay(25);
                                                                                                                                                                                            }
                                                                                                                                                                                        float calcPitch=-atan2((- aFwdTotal/sensorReadCount), sqrt((aLatTotal/sensorReadCount)* (aLatTotal/sensorReadCount) + (aVertTotal/sensorReadCount) *(aVertTotal/sensorReadCount))) * 57.2957;
                                                                                                                                                                                        float calcRoll = atan2((aLatTotal/sensorReadCount) , (aVertTotal/sensorReadCount)) * 57.3;
                                                                                                                                                                                        float PStatic= float((PStaticTotal/sensorReadCount));
                                                                                                                                                                                        Palt=145366.45*(1-pow((PStatic+pStaticBias)/1013.25,0.190284)); //Pstatic in milliBars,Palt in feet
                                                                                                                                                                                        Serial4.printf("<RESPONSE><PITCH>%.2f</PITCH><ROLL>%.2f</ROLL><PALT>%.2f</PALT></RESPONSE>",calcPitch,calcRoll,Palt);                                                                                                                                                                                     
                                                                                                                                                                                        }  else
                                                                                                                                                                                              if (strstr(serialWifiCmdBuffer, "$VERSION"))
                                                                                                                                                                                                        {
                                                                                                                                                                                                         Serial.println("Wifi: Version request");
                                                                                                                                                                                                         Serial4.println("<VERSION>"+String(VERSION)+"</VERSION>"); 
                                                                                                                                                                                                        }
                                                                                                                                                                                                         else                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                             if (strstr(serialWifiCmdBuffer, "$VNOCHIMETEST"))                                                                                                                                                                          
                                                                                                                                                                                                                    {
                                                                                                                                                                                                                    Serial.println("Wifi: VNO chime audiotest");
                                                                                                                                                                                                                    timersOff();                                                                                                                                                                                                         
                                                                                                                                                                                                                    voice1.play(AudioSampleVnochime);
                                                                                                                                                                                                                    watchdogRefresh();
                                                                                                                                                                                                                    delay (2500);
                                                                                                                                                                                                                    watchdogRefresh();                                                                                                                                                                                                          
                                                                                                                                                                                                                    timersOn();
                                                                                                                                                                                                                    Serial.println("Wifi: VNOCHIMETEST Complete");
                                                                                                                                                                                                                    Serial4.println("<VNOCHIMETEST>Done.</VNOCHIMETEST>");
                                                                                                                                                                                                                    }

          // reset cmdBuffer
          memset(serialWifiCmdBuffer,0,sizeof(serialWifiCmdBuffer));
          serialWifiCmdBufferSize=0;
          }  
  } // if serial.available wifi

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
  Serial.print(IAS);   
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
if (readingSensors) 
      {
      Serial.print("^");
      return;
      }
readingSensors=true;   
// reads sensors 50 times/second
int Pfwd;
float PfwdPascal;
float Pstatic=0.00;
int P45;
float PfwdSmoothed;
float P45Smoothed;
Pfwd=GetPressurePfwd()-pFwdBias;
P45=GetPressureP45()-p45Bias;
#ifdef BARO
Pstatic=GetStaticPressure();
Palt=145366.45*(1-pow((Pstatic+pStaticBias)/1013.25,0.190284)); //Pstatic in milliBars,Palt in feet
#endif

flapsIndex=getFlapsIndex();
flapsPos=flapDegrees.Items[flapsIndex];
setAOApoints(flapsIndex);

// median filter pressure then a simple moving average
PfwdMedian.add(Pfwd);
PfwdAvg.addValue(PfwdMedian.getMedian());
PfwdSmoothed=PfwdAvg.getFastAverage();

P45Median.add(P45);
P45Avg.addValue(P45Median.getMedian());
P45Smoothed=P45Avg.getFastAverage();

//coeffP=PCOEFF(PfwdSmoothed,P45Smoothed);
calcAOA(PfwdSmoothed,P45Smoothed); // calculate AOA based on Pfwd divided by non-bias-corrected P45;


// calculate airspeed
PfwdPascal=((PfwdSmoothed+pFwdBias - 0.1*16383) * 2/(0.8*16383) -1) * 6894.757;
if (PfwdPascal>0)
    {
    IAS=sqrt(2*PfwdPascal/1.225)* 1.94384; // knots // physics based calculation
    if (casCurveEnabled) IAS=curveCalc(IAS,casCurve);  // use CAS correction curve if enabled 
    }
    else IAS=0;


unsigned long timeStamp=millis(); // save timestamp for logging
updateTones();


#ifdef SENSORDEBUG
    char debugSensorBuffer[500];
    sprintf(debugSensorBuffer, "timeStamp: %lu,Pfwd: %i,PfwdSmoothed: %.2f,P45: %i,P45Smoothed: %.2f,Pstatic: %.2f,Palt: %.2f,IAS: %.2f,AOA: %.2f,flapsPos: %i,VerticalG: %.2f,LateralG: %.2f,ForwardG: %.2f,RollRate: %.2f,PitchRate: %.2f,YawRate: %.2f, AccelPitch %.2f",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,IAS,AOA,flapsPos,aVert,aLat,aFwd,gRoll,gPitch,gYaw,accPitch);

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
          charsAdded+=sprintf(logLine, "%lu,%i,%.2f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i",timeStamp,Pfwd,PfwdSmoothed,P45,P45Smoothed,Pstatic,Palt,IAS,AOA,flapsPos,dataMark);
          #ifdef IMU
          charsAdded+= sprintf(logLine+charsAdded, ",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.1f,%.1f",getAccelForAxis(verticalGloadAxis),getAccelForAxis(lateralGloadAxis),getAccelForAxis(forwardGloadAxis),getGyroForAxis(rollGyroAxis),getGyroForAxis(pitchGyroAxis),getGyroForAxis(yawGyroAxis),smoothedPitch+pitchBias,smoothedRoll);
          
          #endif
          if (readBoom)
            { 
            int boomAge=millis()-boomTimestamp;
            charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i",boomStatic,boomDynamic,boomAlpha,boomBeta,boomIAS,boomAge); 
            }         
          if (readEfisData)                         
            {
            int efisAge=millis()-efisTimestamp;  
              if (efisType.substring(0,3)=="VN-") // not VN-300 type data
                  {
                               //Serial.printf("\nvnAngularRateRoll: %.2f,vnAngularRatePitch: %.2f,vnAngularRateYaw: %.2f,vnVelNedNorth: %.2f,vnVelNedEast: %.2f,vnVelNedDown: %.2f,vnAccelFwd: %.2f,vnAccelLat: %.2f,vnAccelVert: %.2f,vnYaw: %.2f,vnPitch: %.2f,vnRoll: %.2f,vnLinAccFwd: %.2f,vnLinAccLat: %.2f,vnLinAccVert: %.2f,vnYawSigma: %.2f,vnRollSigma: %.2f,vnPitchSigma: %.2f,vnGnssVelNedNorth: %.2f,vnGnssVelNedEast: %.2f,vnGnssVelNedDown: %.2f,vnGPSFix: %i,TimeUTC: %s\n",vnAngularRateRoll,vnAngularRatePitch,vnAngularRateYaw,vnVelNedNorth,vnVelNedEast,vnVelNedDown,vnAccelFwd,vnAccelLat,vnAccelVert,vnYaw,vnPitch,vnRoll,vnLinAccFwd,vnLinAccLat,vnLinAccVert,vnYawSigma,vnRollSigma,vnPitchSigma,vnGnssVelNedNorth,vnGnssVelNedEast,vnGnssVelNedDown,vnGPSFix,vnTimeUTC.c_str());
                  
                  charsAdded+=sprintf(logLine+charsAdded,",%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%s",vnAngularRateRoll,vnAngularRatePitch,vnAngularRateYaw,vnVelNedNorth,vnVelNedEast,vnVelNedDown,vnAccelFwd,vnAccelLat,vnAccelVert,vnYaw,vnPitch,vnRoll,vnLinAccFwd,vnLinAccLat,vnLinAccVert,vnYawSigma,vnRollSigma,vnPitchSigma,vnGnssVelNedNorth,vnGnssVelNedEast,vnGnssVelNedDown,vnGPSFix,efisAge,vnTimeUTC.c_str());                  
                  } else                  
                        {
                        charsAdded+=sprintf(logLine+charsAdded, ",%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%i,%i,%i,%s",efisIAS,efisPitch,efisRoll,efisLateralG,efisVerticalG,efisPercentLift,efisPalt,efisVSI,efisTAS,efisOAT,efisFuelRemaining,efisFuelFlow,efisMAP,efisRPM,efisPercentPower,efisHeading,efisAge,efisTime.c_str());
                        }
            }
                           
          sprintf(logLine+charsAdded,"\n");
  //      Serial.print("8");        
        datalogRingBufferAdd(logLine);
        }  
  #endif
  
  //Serial.println(sensorCacheCount);
 // breathing LED
  if (millis()-lastLedUpdate>100)
      {
      if (switchState )
          {
          float ledBrightness = 15+(exp(sin(millis()/2000.0*PI)) - 0.36787944)*63.81; // funky sine wave, https://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
          analogWrite(PIN_LED2, ledBrightness);         
          } else analogWrite(PIN_LED2,0);
       lastLedUpdate=millis();    
      }     
      
readingSensors=false;
//Serial.println(micros()-sensorstarttime);    
//Serial.println(">");
}

void datalogRingBufferAdd(char * logBuffer)
{ 
        
// add data to ringbuffer and update indexes  
if ((LOG_RINGBUFFER_SIZE - datalogBytesAvailable) < strlen(logBuffer))
      {
      // ring buffer full;
      //Serial.print("ring buffer full");
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

//Serial.printf("%i}",datalogBytesAvailable);
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
        //Serial.println(SDwriteCache);
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
setAOApoints(2); // flaps down
IAS=50; // to turn on the tones
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
IAS=50; // to turn on the tones 
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
String valueArray[60];
int valueIndex=0;
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
                    }
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
                  //Pfwd=valueArray[1].toInt(); // bias already removed                  
                  PfwdSmoothed=valueArray[2].toFloat();                  
                  //P45=valueArray[3].toInt(); // bias already removed
                  P45Smoothed=valueArray[4].toFloat();

                  //coeffP=PCOEFF(PfwdSmoothed,P45Smoothed);
                  
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

                  // efisPitch
                  efisPitch=valueArray[19].toFloat();

                  // efisRoll
                  efisRoll=valueArray[20].toFloat();
                  
                  // get airspeed
                  IAS=valueArray[7].toFloat();

                  //  AHRS debug code below

/*
                  
                  // smooth IAS
                  //if (airspeedSmoothing==0) iasSmoothingAlpha=1.0; else iasSmoothingAlpha=2.0/(airspeedSmoothing+1.0);
                  //smoothedIAS=IAS * iasSmoothingAlpha + (1-iasSmoothingAlpha) * smoothedIAS;
                  //IASAvg.addValue(IAS50hzSample);
                  //smoothedIAS=IASAvg.getFastAverage();
                  
                  //IASAvg238Hz.addValue(IAS);
                  //float smoothedIAS238Hz=IASAvg238Hz.getFastAverage();
                  //float IASdiff238Hz=smoothedIAS238Hz-prevIAS238hz;
                  //prevIAS238hz=smoothedIAS238Hz;

                  // first average IAS at 50hz, then average and diff at 238hz

                  
                  IASAvg50hz.addValue(smoothedIAS);
                  float IAS50hzSmoothed=IASAvg50hz.getFastAverage();
                  IASdiff=IAS50hzSmoothed-prevIAS;
                  prevIAS=IAS50hzSmoothed;
                  
                  
                  // get pressure altitude 
                  Palt=valueArray[6].toFloat();

                  az_logged=stringToFloat(valueArray[11]);
                  ay_logged=stringToFloat(valueArray[12]);
                  ax_logged=stringToFloat(valueArray[13]);
                  gx_logged=stringToFloat(valueArray[14]);
                  gy_logged=stringToFloat(valueArray[15]);
                  gz_logged=stringToFloat(valueArray[16]);
                                  
                  

                 
                      // calculate TAS
                      float smoothedTAS=IAS50hzSmoothed+IAS50hzSmoothed * Palt / 1000 * 0.02; // ballpark TAS 2% per thousand feet pressure altitude
                  
                      // calculate centripetal turning compensation
                      //centripetal acceleration in m/sec2 = speed in m/sec * angular rate in radians    
                      float aVertCp=smoothedTAS*0.514444 * gy_logged * 0.01745329252 * 0.10197162129779; // knots to m/sec, degrees to radians, m/sec2 to g
                      float aLatCp=smoothedTAS*0.514444 * gz_logged * 0.01745329252 * 0.10197162129779;
                      
                      // correct for forward acceleration
                      //float aFwdCp=(smoothedIAS-prevIAS)*0.514444/0.02 * 0.10197162129779; // // knots to m/sec, 1/238 (update rate), m/sec2 to g
                      float aFwdCp=IASdiff*0.514444/0.0042 * 0.10197162129779; // // knots to m/sec, 1/50 (update rate), m/sec2 to g                                            
                      //float aFwdCp238Hz=IASdiff238Hz*0.514444/0.0042 * 0.10197162129779;// 238hz IAS correction
                      // calculate and smooth Accelerometer values
                      
                      aVertAvg.addValue(az_logged-aVertCp);
                      aVert=aVertAvg.getFastAverage();

                      aLatAvg.addValue(ay_logged+aLatCp);                      
                      aLat=aLatAvg.getFastAverage();

                      aFwdAvg.addValue(ax_logged-aFwdCp);
                      aFwd=aFwdAvg.getFastAverage();
                                       
                      // calculate compensated accelerations
                      // centripetal
                      // forward accel/decel calculated from airspeed
                      //float aFwdComp=aFwd-aFwdCp;
                      //float aLatComp=aLat+aLatCp;
                      //float aVertComp=aVert-aVertCp;

                      
                      //Serial.printf("RAW: %0.1f,%0.1f,%0.1f,%0.1f,%0.1f,%0.1f\n",getAccelForAxis(forwardGloadAxis),getAccelForAxis(lateralGloadAxis),getAccelForAxis(verticalGloadAxis),getGyroForAxis(pitchGyroAxis),getGyroForAxis(rollGyroAxis),getGyroForAxis(yawGyroAxis));
                      //Serial.printf("RAW: %0.1f,%0.1f,%0.1f,%0.1f,%0.1f,%0.1f\n",aFwd,aLat,aVert,gRoll,gPitch,gYaw);
                    // Update the filter
                      //Serial.printf("%.3f,%.3f,%.4f,%.4f\n", IAS50hzSmoothed,smoothedIAS238Hz, aFwdCp,aFwdCp238Hz);
                      filter.updateIMU(-gx_logged, -gy_logged, -gz_logged, aFwd, aLat, aVert); // use smoothed gyros and smoothed+compensated accelerometers
                      
                      // calculate smoothed pitch
                      float ahrsSmoothingAlpha;
                      if (ahrsSmoothing==0) ahrsSmoothingAlpha=1.0; else ahrsSmoothingAlpha=2.0/(ahrsSmoothing+1);
                      smoothedPitch= - filter.getPitch() * ahrsSmoothingAlpha+(1-ahrsSmoothingAlpha)*smoothedPitch;
                      smoothedRoll= filter.getRoll() * ahrsSmoothingAlpha +(1-ahrsSmoothingAlpha)*smoothedRoll;
                  
                      if (efisPitch!=-100)
                          {
                          rmse_total=rmse_total+ pow(-filter.getPitch()+6.531-efisPitch,2);
                          rmse_count++;
                          }

                  Serial.printf("%.2f,%.2f,%.2f,%.2f,%.4f,%.4f,%.4f\n",smoothedPitch+6.531,efisPitch,smoothedRoll,efisRoll, aFwd, aLat, aVert);
                  return; // only interested in AHRS right now

*/   
                  
                  updateTones(); // generate tones                
                  Serial.print("Time: ");
                  Serial.print(valueArray[0]);
                  Serial.print(",Pfwd: ");
                  Serial.print(PfwdSmoothed);
                  Serial.print(",P45: ");                 
                  Serial.print(P45Smoothed);
                  Serial.print(",IAS: ");
                  Serial.print(IAS);
                  Serial.print(",AOA: ");
                  Serial.print(AOA);
                  Serial.print(", Pitch: ");
                  Serial.print(accPitch);
                  Serial.print(", efisPitch: ");
                  Serial.print(efisPitch);
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
 // soft reboot accelerometer/gyro
 //I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG8,  0x01);
 //delay(200);
  
 // enable the three axes of the accelerometer
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG5_XL,  0x38);
 // configure the accelerometer-specify bandwidth selection with Abw
 I2CwriteByte(_i2cAddress_AccelGyro,CTRL_REG6_XL, Aodr << 5 | Ascale << 3 | 0x04 | Abw);
 
 // enable the three axes of the gyroscope
 I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG4, 0x38);
 // configure the gyroscope
 I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG1_G, Godr << 5 | Gscale << 3 | Gbw);

delay(50);
// set FIFO mode
I2CwriteByte(_i2cAddress_AccelGyro, FIFO_CTRL, 0xC0); // write 11000000 to FIFO_CTRL to set FIFO to Continous mode.

delay(50);
 // turn on FIFO
I2CwriteByte(_i2cAddress_AccelGyro, CTRL_REG9, 0x02); // write 00000010 to CTRL_REG9 to turn on FIFO, unlimited FIFO depth (32 values max)

delay(100);
 
}

bool newAccelGyroAvailable()
    {
    uint8_t fifoSRCbyte;
    fifoSRCbyte=I2CreadByte(_i2cAddress_AccelGyro, FIFO_SRC) & 0x3F; // 00111111 - number of bytes available
    if (fifoSRCbyte >0) return true; else return false;
    }

void readAccelGyro()
{
  uint8_t temp_a[6]; // We'll read six bytes from the accelerometer into temp 
  I2CreadBytes(_i2cAddress_AccelGyro,OUT_X_L_XL, temp_a, 6); // Read 6 bytes, beginning at OUT_X_L_XL  
  ax = (temp_a[1] << 8) | temp_a[0]; // Store x-axis values into ax
  ay = (temp_a[3] << 8) | temp_a[2]; // Store y-axis values into ay
  az = (temp_a[5] << 8) | temp_a[4]; // Store z-axis values into az 
     
  uint8_t temp_g[6];  // x/y/z gyro register data stored here
  I2CreadBytes(_i2cAddress_AccelGyro, OUT_X_L_G, temp_g,6);  // Read the six raw data registers sequentially into data array
  
  gx = (temp_g[1] << 8) | temp_g[0]; // Store x-axis values into gx
  gy = (temp_g[3] << 8) | temp_g[2]; // Store y-axis values into gy
  gz = (temp_g[5] << 8) | temp_g[4]; // Store z-axis values into gz  

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
          Serial.println("i2c timeout");
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
float smoothingAlpha;
float pCoeff;

if (Pfwd>0)
    {
    pCoeff=PCOEFF(Pfwd,P45);
    } else 
          {
          AOA=AOA_MIN_VALUE;
          return;
          }
          
// calculate and smooth AOA
if (aoaSmoothing==0) smoothingAlpha=1.0; else smoothingAlpha=1.0/aoaSmoothing;
AOA=curveCalc(pCoeff,aoaCurve[flapsIndex]) * smoothingAlpha + (1-smoothingAlpha) * AOA; // smoothing is defined by samples of lag, alpha=1/samples
if (AOA>AOA_MAX_VALUE) AOA=AOA_MAX_VALUE; else if (AOA<AOA_MIN_VALUE) AOA=AOA_MIN_VALUE;
if (isnan(AOA)) AOA=AOA_MIN_VALUE;
}

void SendDisplayData()
{
 char json_buffer[285];
 char crc_buffer[250];
 byte CRC=0;
 // gloads changed by the IMU interrupt
 noInterrupts();
 float accelSumSq=aVert*aVert+aLat*aLat+aFwd*aFwd;
 interrupts();
 float verticalGload=sqrt(abs(accelSumSq));
 verticalGload=round(verticalGload * 10.0) / 10.0; // round to 1 decimal place
 if (aVert<0) verticalGload*=-1;
 
 float displayAOA;
 float displayPalt;
 float alphaVA=0.00;
 if (isnan(AOA) || IAS<muteAudioUnderIAS)
    {
    displayAOA=-100;    
    }
    else
        {
        // protect AOA from interrupts overwriting it  
        displayAOA=AOA;
        }
 if (isnan(Palt)) displayPalt=-1.00; else displayPalt=Palt;
 interrupts();
 
 //flapsPercent=(float)flapsPos/(flapDegrees.Items[flapDegrees.Count-1]-flapDegrees.Items[0])*100; //flap angle / flap total travel *100 (needed for displaying partial flap donut on display)
 noInterrupts();
 sprintf(crc_buffer,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.6f,%i",displayAOA,smoothedPitch+pitchBias,smoothedRoll,IAS,displayPalt,verticalGload,aLat,alphaVA,LDmaxAOA,onSpeedAOAfast,onSpeedAOAslow,stallWarningAOA,flapsPos,coeffP,dataMark);
 interrupts();
 for (unsigned int i=0;i<strlen(crc_buffer);i++) CRC=CRC+char(crc_buffer[i]); // claculate simple CRC
 sprintf(json_buffer,"$ONSPEED,%s,%i",crc_buffer,CRC);
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
                //Serial.printf("%.2f * log(%.2f)+%.2f\n",curve.Items[MAX_CURVE_COEFF-2],x,curve.Items[MAX_CURVE_COEFF-1]);
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
                  //{"LEFT","UP","Z","-X","-Y"},
                  {"LEFT","UP","Z","X","Y"},
                  {"LEFT","DOWN","-Z","-X","Y"},
                  
                  {"RIGHT","FORWARD","-Y","X","Z"},
                  {"RIGHT","AFT","Y","X","-Z"},
                  //{"RIGHT","UP","Z","X","Y"},
                  {"RIGHT","UP","Z","-X","-Y"},                  
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
if (accelAxis[accelAxis.length()-1] == 'X') result=scaleAccel(ax);          
    else
        if (accelAxis[accelAxis.length()-1] == 'Y') result=scaleAccel(ay);
            else result=scaleAccel(az);
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
if (gyroAxis[0]!='-') result*=-1;
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
Serial.println("Timers OFF");
ToneTimer.end(); 
SensorTimer.end();  
SerialDataTimer.end();
IMUTimer.end();
}

void timersOn()
{
Serial.println("Timers ON");
IMUTimer.begin(ReadIMU,IMU_INTERVAL);
SensorTimer.begin(SensorRead,SENSOR_INTERVAL);   
ToneTimer.begin(tonePlayHandler,1000000/pps); // turn ToneTimer back on
SerialDataTimer.begin(readSerialData,SERIAL_INTERVAL); // turn Serial timer back on
}

void ReadIMU()
{
// read IMU and process AHRS
#ifdef IMU  

//check if FIFO is available and then read data

if (readIMU && newAccelGyroAvailable())
    {     
    //unsigned long startTime=micros();
    readAccelGyro(); // read accelerometers
    
   // first average IAS at 50hz, then average and diff at 238hz
                  
                  IASAvg.addValue(IAS);
                  smoothedIAS=IASAvg.getFastAverage();
                  float IASdiff=smoothedIAS-prevIAS;
                  prevIAS=smoothedIAS;
                                  
                  // update AHRS                 

                 
                      // calculate TAS
                      float smoothedTAS=smoothedIAS+smoothedIAS * Palt / 1000 * 0.02; // ballpark TAS 2% per thousand feet pressure altitude
                  
                      // calculate centripetal turning compensation
                      //centripetal acceleration in m/sec2 = speed in m/sec * angular rate in radians    
                      float aVertCp=smoothedTAS*0.514444 * getGyroForAxis(pitchGyroAxis) * 0.01745329252 * 0.10197162129779; // knots to m/sec, degrees to radians, m/sec2 to g
                      float aLatCp=smoothedTAS*0.514444 * getGyroForAxis(yawGyroAxis) * 0.01745329252 * 0.10197162129779;
                      
                      // correct for forward acceleration
                      float aFwdCp=IASdiff*0.514444/0.0042 * 0.10197162129779; // // knots to m/sec, 1/238hz (update rate), m/sec2 to g
                      // compensate and smooth Accelerometer values
                      
                      aVertCompAvg.addValue(getAccelForAxis(verticalGloadAxis)-aVertCp);
                      aVertComp=aVertCompAvg.getFastAverage();
                      aVertAvg.addValue(getAccelForAxis(verticalGloadAxis));
                      aVert=aVertAvg.getFastAverage();


                      aLatCompAvg.addValue(getAccelForAxis(lateralGloadAxis)+aLatCp);                      
                      aLatComp=aLatCompAvg.getFastAverage();
                      aLatAvg.addValue(getAccelForAxis(lateralGloadAxis));                      
                      aLat=aLatAvg.getFastAverage();

                      aFwdCompAvg.addValue(getAccelForAxis(forwardGloadAxis)-aFwdCp);
                      aFwdComp=aFwdCompAvg.getFastAverage();
                      aFwdAvg.addValue(getAccelForAxis(forwardGloadAxis));
                      aFwd=aFwdAvg.getFastAverage();
                                            
                      //Serial.printf("RAW: %0.1f,%0.1f,%0.1f,%0.1f,%0.1f,%0.1f\n",getAccelForAxis(forwardGloadAxis),getAccelForAxis(lateralGloadAxis),getAccelForAxis(verticalGloadAxis),getGyroForAxis(pitchGyroAxis),getGyroForAxis(rollGyroAxis),getGyroForAxis(yawGyroAxis));
                      //Serial.printf("RAW: %0.1f,%0.1f,%0.1f,%0.1f,%0.1f,%0.1f\n",aFwd,aLat,aVert,gRoll,gPitch,gYaw);
                    // Update the filter
                      filter.updateIMU(-getGyroForAxis(rollGyroAxis), -getGyroForAxis(pitchGyroAxis), -getGyroForAxis(yawGyroAxis), aFwdComp, aLatComp, aVertComp); // use raw gyros and smoothed+compensated accelerometers
                      
                      // calculate smoothed pitch                      
                      float ahrsSmoothingAlpha=2.0/(ahrsSmoothing+1);
                      smoothedPitch= - filter.getPitch() * ahrsSmoothingAlpha+(1-ahrsSmoothingAlpha)*smoothedPitch;
                      smoothedRoll= filter.getRoll() * ahrsSmoothingAlpha +(1-ahrsSmoothingAlpha)*smoothedRoll;
    }// else Serial.println("no IMU data");
    
#endif
}

void enableWatchdog()
{
WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
delayMicroseconds(1); // Need to wait a bit..
WDOG_STCTRLH = 0x0001; // Enable WDG
WDOG_TOVALL = 5000; // The next 2 lines sets the time-out value in millissec
WDOG_TOVALH = 0;
WDOG_PRESC = 0; // This sets prescale clock so that the watchdog timer ticks at 1kHZ instead of the default 1kHZ/4 = 200 HZ
Serial.println("Watchdog Timer enabled (5 seconds)");
}

void watchdogRefresh()
{
noInterrupts();
WDOG_REFRESH = 0xA602;
WDOG_REFRESH = 0xB480;
interrupts();
}

float array2float(byte buffer[], int startIndex)
{
    float out;
    memcpy(&out, &buffer[startIndex], sizeof(float));
    return out;
}


 
