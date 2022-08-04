// to be compiled on Arduino 1.8.16 & Teensyduino 1.55

////////////////////////////////////////////////////
// More details at
//      http://www.flyOnSpeed.org
//      and
//      https://github.com/flyonspeed/OnSpeed-Gen2/


#define VERSION "3.2.3.g1" //Bob's MGL parser fix.
//"3.2.3f" // 7/17/2022 Fixed VSI unit conversion issue and Roll calibration direction.
//"3.2.3e" //7/15/2021 Covid-affected update, proceed with caution. Added checksums to data download. Also change comm speeds to 1,000,000bps
//"3.2.3d" // 7/14/2022 tuned kalmanVSI filter
//"3.2.3c" // 7/12/2022 another switch glitch fix
//"3.2.3b" //mgl updates. VSI.
//"3.2.3a" //interrupt based push button, updated OneButton library
//"3.2.3" // modified and tuned AHRS and iVSI code for the new IMS330 IMU, do not use this code with the old IMS9DS1 IMU, more responsive Datamark button
//"3.2.2r"  6/2/2022//MGL efis input
//"3.2.2q" //disabled IMU gyro LPF1 filter, really disabled high pass filter this time...
//"v3.2.2p" // 6/2/2022 disabled IMU gyro hardware high pass filter
//"v3.2.2o" // 5/28/2022  changed logged pitch and roll resolution to 2 digits, fixed VN-300 parser
//"v3.2.2n" // 5/11/2022  Added code to read new IMU ISM330DHXC, fixed large config file saving issue
//"v3.2.2m" // 3/26/2022 use efisIAS with calibration wizard if equiped with spherical probe
//"v3.2.2j" // 3/15/2022 Added ability to run the calibration wizard with efis data, fixed AFS data parsing glitch. Yeah a second j version :)
//"v3.2.2l"     // 2/9/2022 Added full card erase before card format. Results in much faster data writes. Also fixed SD card init issues after formatting.
//"v3.2.2k"     // 1/27/2022 added NOBOOMCHECKSUM option to accomodate flight test booms with no checksum in their datastream
//"v3.2.2j"     // 12/5/2021 rolled in Spherical probe curves
//"v3.2.2i"     // improved serial data processing, boom/efis
//"v3.2.2h"     // 12/1/2021 fixed serial data processing
//"v3.2.2g"     // added IMU temp logging, fixed wifi transfer issue (update wifi code to same version)
//"v3.2.2f1"    // 10/22/2021 fixed VN-300 parser issue, 
//"v3.2.2f"     // added AHRS corrections for IMU installation errors and flightpath calculation. Calibration Wizard on the Wifi side. Added VN-300 GPS lat/lon to logs. Reconfigure Vn-300 output format, add [Common] Group 1 / "Position" output
// 5/25/2021     //switched over to CP3
//"v3.2.1a"     // 3/15/2021 fixed boom curves
//"v3.2"        // static pressure bias, roll bias and cas curve type
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
//Tones 96
//IMU 128
//Sensors 128

                         
#define DEFAULT_CONFIG  default_config

// data logging frequency
#define LOGDATA_PRESSURE_RATE
//#define LOGDATA_IMU_RATE

// AOA probe type
//#define SPHERICAL_PROBE // uncomment this if using custom OnSpeed spherical head probe.

// imu type
//#define IMUTYPE_LSM9DS1  // original IMU
#define IMUTYPE_ISM330DHCX // new IMU with less temperature drift

// boom type
//#define NOBOOMCHECKSUM    // for booms that don't have a checksum byte in their data stream uncomment this line.

// curves config
#define MAX_AOA_CURVES    5 // maximum number of AOA curves (flap/gear positions)
#define MAX_CURVE_COEFF   4 // 4 coefficients=3rd degree polynomial function for calibration curves

// glimit settings
#define GLIMIT_REPEAT_TIMEOUT   3000 // milliseconds to repeat G limit.
#define ASYMMETRIC_GYRO_LIMIT   15 // degrees/sec rotation on either axis to trigger asymmetric G limits.



// coefficient of pressure formula
#ifdef SPHERICAL_PROBE
  #define PCOEFF(p_fwd,p_45)    atan2(p_45,p_fwd); //spherical CP3
  #define IASCURVE(x)           -3.206e-05*x*x*x+0.008454*x*x+0.3492*x+27.73; // Zlin IAS curve
#else
  #define PCOEFF(p_fwd,p_45)  p_45/p_fwd; // CP3 // ratiometric CP. CP1 & CP2 are not ratiometric. Can't divide with P45, it goes through zero on Dynon probe.
#endif

// boom curves
//#define BOOM_ALPHA_CALC(x)      7.0918*pow(10,-13)*x*x*x*x - 1.1698*pow(10,-8)*x*x*x + 7.0109*pow(10,-5)*x*x - 0.21624*x + 310.21; //degrees
//#define BOOM_BETA_CALC(x)       2.0096*pow(10,-13)*x*x*x*x - 3.7124*pow(10,-9)*x*x*x + 2.5497*pow(10,-5)*x*x - 3.7141*pow(10,-2)*x - 72.505; //degrees
//#define BOOM_STATIC_CALC(x)     0.00012207*(x - 1638)*1000; // millibars
//#define BOOM_DYNAMIC_CALC(x)    (0.01525902*(x - 1638)) - 100; // millibars


// log raw counts for boom, no curves
#define BOOM_ALPHA_CALC(x)      x
#define BOOM_BETA_CALC(x)       x
#define BOOM_STATIC_CALC(x)     x
#define BOOM_DYNAMIC_CALC(x)    x

// serial data packet size
#define BOOM_PACKET_SIZE  50
#define EFIS_PACKET_SIZE 512


// debug config. Comment out any of them to disable serial debug output.
//#define SENSORDEBUG // show sensor debug
//#define EFISDATADEBUG // show efis data debug
//define BOOMDATADEBUG  // show boom data debug
//#define TONEDEBUG // show tone related debug info
//#define SDCARDDEBUG  // show SD writing debug info
//#define VOLUMEDEBUG  // show audio volume info
//#define VNDEBUG // show VN-300 debug info
//#define AXISDEBUG //show accelerometer axis configuration
//#define IMUTEMPDEBUG
//#define AGEDEBUG // debug data age (boom,efis [ms])

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
bool readEfisData; // Serial3, read and log serial data (ahrs/aoa/airspeed/altitude data from Dynon EFIS D10, D100, D180, Skyview/ADVANCED, Garmin G5 and G3X, MGL)
bool casCurveEnabled;

int lineCount=0; // log replay variable

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
bool ledOn=false;

// switch state
bool switchDoSingleClick=false;
bool switchDoLongPress=false;

volatile bool timersDisabled=false;

// timers
unsigned long audio3dLastUpdate=millis();
unsigned long volumeLastUpdate=millis();
unsigned long serialoutLastUpdate=millis();
unsigned long gLimitLastWarning=millis();
unsigned long gLimitLastUpdate=millis();
unsigned long vnoLastChime=millis();
unsigned long vnoChimeLastUpdate=millis();
unsigned long wifiDataLastUpdate=millis();
unsigned long lastSDWrite=millis();
unsigned long lastLedUpdate=millis();
unsigned long lastImuTempUpdate=millis();
unsigned long lastDecelUpdate=millis();

volatile unsigned long lastWatchdogRefresh;
volatile bool watchdogEnabled=false;

unsigned long lastReplayOutput=millis();

unsigned long displayDataSendTime=millis();

#define AUDIO_3D_CURVE(x)         -92.822*x*x + 20.025*x //move audio with the ball, scaling is 0.08 LateralG/ball width
bool overgWarning; // 0 - off, 1 - on
float channelGain=1.0;
float calculatedGLimitPositive;
float calculatedGLimitNegative;


// smoothing windows
int aoaSmoothing=20;  // AOA smoothing window (number of samples to lag)
int pressureSmoothing=15; // median filter window for pressure smoothing/despiking

const int accSmoothing=176; // accelerometer smoothing, Simple moving average
const int imuTempSmoothing=20; // imu temperature smoothing, Simple moving average, 10 = 1 second
//const int imuTempRateSmoothing=5; // imu temperature smoothing, Simple moving average
const int gyroSmoothing=30; // gyro smoothing, Simple moving average
const int compSmoothing=20; // acceleration compensation smoothing (linear and centripetal)
const int iasSmoothing=136; // airspeed smoothing, 314 sample moving average for 208hz [optimized for ISM330 IMU]
const int tasSmoothing=10; //[optimized for ISM330 IMU]
const int ahrsSmoothing=50; // ahrs smoothing, Exponential
const int serialDisplaySmoothing=10; // smoothing serial display data (LateralG, verticalG)  10hz data.
const float gyroScaleCorrection=1.14; // [optimized for ISM330 IMU]

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
floatArray flapSTALLAOA;
floatArray flapMANAOA;

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

// AHRS variables
volatile float earthVertG=0.0;
volatile bool newSensorDataAvailable=false;

#ifdef IMUTYPE_LSM9DS1
float imuSampleRate=238; // 238Hz. 50hz for replay
#endif
#ifdef IMUTYPE_ISM330DHCX
float imuSampleRate=208; // 208Hz.
#endif

volatile bool sendWifiData=false;
int displayDataCounter=0;
int imuIndex=0; // used for logreplay


// data mark
volatile int dataMark=0;

volatile double coeffP; // coefficient of pressure

#ifdef LOGDATA_PRESSURE_RATE  // sd card write rate
  #define SD_WRITE_TIME 1000
#else
  #define SD_WRITE_TIME 400
#endif



// interval timers
#define SENSOR_INTERVAL 20000  // microsecond interval for sensor read (50hz)
//#define SENSOR_INTERVAL 4201 // 238hz logging
//#define REPLAY_INTERVAL 4201.680672268907563
#define REPLAY_INTERVAL 20000
#ifdef IMUTYPE_LSM9DS1
#define IMU_INTERVAL    4200 // microseconds interval for IMU read
#endif
#ifdef IMUTYPE_ISM330DHCX
#define IMU_INTERVAL    4808 // microseconds interval for IMU read
#endif

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
  #define BAUDRATE_WIFI         1000000 // 921600 is glicthy on this port

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

#ifdef IMUTYPE_LSM9DS1
  #define LSM9DS1_Magnet    0x1E
  // LSM9DS1 Accel/Gyro (XL/G) Registers
  #define CTRL_REG5_XL      0x1F
  #define CTRL_REG6_XL      0x20
  #define CTRL_REG7_XL      0x21
  #define OUT_X_L_XL        0x28
  #define CTRL_REG4         0x1E
  #define CTRL_REG1_G       0x10
  #define OUT_X_L_G         0x18
  #define OUT_TEMP_L        0x15
  #define OUT_TEMP_H        0x16
  #define CTRL_REG8         0x22
  #define CTRL_REG9         0x23
  #define STATUS_REG_1      0x27 // status register, is data available?
  #define FIFO_CTRL         0x2E // enable FIFO in IMU
  #define FIFO_SRC          0x2F // FIFO status register
  #define LSM9DS1_TEMP_SCALE 16.0
  #define LSM9DS1_TEMP_BIAS 25.0
  #define _i2cAddress_AccelGyro 0x6B // chip address
  #define aRes             8.0 / 32768.0 // full scale /resolution (8G)
  #define gRes             245.0 / 32768.0 // full scale / resolution (245 dps)
#endif

#ifdef IMUTYPE_ISM330DHCX
  // define ISM330 registers
  #define _i2CAddressISM330 0x6A // chip address on I2C bus
  #define CTRL1_XL 0x10 // accelerometer control register  
  #define CTRL2_G 0x11 // gyro control register
  #define CTRL3_C 0x12
  #define CTRL4_C 0x13
  #define CTRL9_XL 0x18
  #define CTRL7_G 0x16
  #define CTRL6_C 0x15
  #define OUTX_L_A 0x28 // start of accelerometer output address
  #define OUTX_L_G 0x22 // start of gyro output address
  #define ISM330_OUT_TEMP_L 0x20 // temp output register 
  #define ISM330_OUT_TEMP_H 0x21
  #define FIFO_CTRL4 0x0A
  #define ISM330_TEMP_SCALE 256.0
  #define ISM330_TEMP_BIAS 25.0
  #define aRes             8.0 / 32768.0 // full scale /resolution (8G)
  #define gRes             250.0 / 32768.0 // full scale / resolution (250 dps)

#endif

#define DEG2RAD 0.0174533 // degrees to radians
#define RAD2DEG 57.2958 // radians to degrees
#define G2MPS   9.80665  // g to m/sec^2
#define MPS2G   0.101971621 // m/sec^2 to g
#define FT2M    0.3048 // feet to meters
#define M2FT    3.28084 // meters to feet
#define MPS2FPM 196.85 // m/sec to fpm

#define I2C_COMMUNICATION_TIMEOUT 2000  // microseconds

#define SD_WRITE_BLOCK_SIZE  512  // block size to write on the SD card. 512 is the most efficient
#define LOG_RINGBUFFER_SIZE 32768  // ringbuffer size

#define _GNU_SOURCE

#include <stdint.h>
#include <i2c_t3.h> // multiport i2c (Wire.h needs to redirect here, otherwise it gets duplicated. Make a Wire.h library with an #include <i2c_t3.h> line in it.

// SD card includes
#include "SdFat.h" // https://github.com/greiman/SdFat  v.2.1.0
SdFat Sd;
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
#include "MadgwickFusion.h"
#include "KalmanFilter.h"
#include <SavLayFilter.h>

Madgwick filter;
KalmanFilter kalman;
//double imuTempDerivativeInput;
//SavLayFilter imuTempDerivative (&imuTempDerivativeInput, 1, 15); //Computes the first derivative

double iasDerivativeInput;
SavLayFilter iasDerivative (&iasDerivativeInput, 1, 15);           //Computes the first derivative


volatile float kalmanAlt=0; // meters, Kalman filtered pressure altitude
volatile float kalmanVSI=0; // m/sec, Kalman filtered instantaneous vertical speed


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
volatile float DecelRate=0.00;
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
int efisID=0; // 0= none, 1=VN-300, 2=AFS/SkyView, 3=Dynon D10, 4=G5, 5=G3X, 6=MGL
String calSource="";
bool efisPacketInProgress=false;
unsigned long lastReceivedEfisTime;
int efis_bufferIndex=0;
String efisBufferString="";

char efis_inChar;               // efis serial character
char last_efis_inChar=char(0x00);
byte previousEfisByte=0x00;
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
volatile double vnGnssLat=0.00L; // 8 byte double
volatile double vnGnssLon=0.00L;
String vnTimeUTC="";
byte vnBuffer[127];
byte vnBufferIndex=0;
int mglMsgLen=0;

// pressure variables
volatile int Pfwd;
volatile float PfwdPascal;
volatile float Pstatic=0.00;
volatile int P45;
volatile float PfwdSmoothed;
volatile float P45Smoothed;

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


FsFile SensorFile;
FsFile ListFile;
char filenameSerial[14];
char filenameSensor[14];
bool sdLogging=false;
bool sdLoggingConfig=false;
bool sdAvailable=false;
int filesendtimer;
bool filesendtimeout;

volatile uint8_t toneState = false;
volatile bool switchState;
volatile byte toneMode = PULSE_TONE;  // current mode of tone.  PULSE_TONE, SOLID_TONE, or TONE_OFF
volatile byte tonePlaying = TONE_OFF;
volatile boolean highTone = false;             // are we playing high tone or low tone?
volatile uint32_t toneFreq = 0;                // store current freq of tone playing.
volatile float pps = 20;              // store current PPS of tone (used for debuging) 
//float current_delay=1000/pps;       // 1000/pps, tone timer update rate
volatile float AOA = 0.0;                      // avaraged AOA value is stored here.
volatile float flightPath=0.0;
volatile float derivedAOA=0.0;
volatile float LDmaxAOA=0.00;
volatile float onSpeedAOAfast=0.00;
volatile float onSpeedAOAslow=0.00;
volatile float stallWarningAOA=0.00;
volatile float stallAOA=0.00;
volatile float maneuveringAOA=0.00;

volatile float percentLift=0.0;                     // normalized angle of attack, or lift %
volatile float IAS = 0.0;                          // live Air Speed Indicated
volatile float smoothedIAS=0.0;                    // smoothed airspeed
volatile float smoothedIASdiff=0.0;                    // smoothed airspeed
volatile float smoothedTAS=0.0;                    // smoothed airspeed
volatile float prevIAS=0.0;                        // previous IAS sample (used to calculate acceleartion)
volatile float Palt=0.00;                          // pressure altitude

float currentRangeSweepValue=RANGESWEEP_LOW_AOA;
RunningMedian P45Median(pressureSmoothing);
RunningMedian PfwdMedian(pressureSmoothing);
RunningMedian BaroMedianMillibars(pressureSmoothing);
RunningAverage PfwdAvg(10);
RunningAverage P45Avg(10);
RunningAverage IASdiffAvg(iasSmoothing);
RunningAverage TASAvg(tasSmoothing);
//RunningAverage aVertAvg(accSmoothing);
//RunningAverage aLatAvg(accSmoothing);
//RunningAverage aFwdAvg(accSmoothing);
RunningAverage GxAvg(gyroSmoothing);
RunningAverage GyAvg(gyroSmoothing);
RunningAverage GzAvg(gyroSmoothing);
//RunningAverage aVertCompAvg(compSmoothing);
//RunningAverage aLatCompAvg(compSmoothing);
//RunningAverage aFwdCompAvg(compSmoothing);
RunningAverage aVertCorrAvg(accSmoothing);
RunningAverage aLatCorrAvg(accSmoothing);
RunningAverage aFwdCorrAvg(accSmoothing);
RunningAverage imuTempAvg(imuTempSmoothing);
//RunningAverage imuTempRateAvg(imuTempRateSmoothing);


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
char serialWifiCmdBuffer[3072];       // usb serial command buffer

int serialBufferSize = 0;    // serial buffer size (boom data)
char serialBuffer[51];       // serial 1  buffer (boom data)

char listfileFileName[14];      // file name for sd card file listing operations
bool listfileIsDirectory;      

#define MAXSIZE 90              // max length of string
char input[MAXSIZE+1];          // buffer for full serial data input string.
int inputPos = 0;               // current postion of serial data.
char tempBuf[90];               // misc char buffer used for debug
char logBufSerial[50];                // char buffer for serial logging

unsigned long last_time = millis();
char datalogRingBuffer[LOG_RINGBUFFER_SIZE]; //64Kbytes ringbuffer for datalog
volatile static int datalogRingBufferStart=0; 
volatile static int datalogRingBufferEnd=0; 
volatile static unsigned int datalogBytesAvailable=0;


// IMU variables

volatile float ax, ay, az; // ax -instantaneous
volatile float aVert, aLat, aFwd, aVertComp, aLatComp, aFwdComp;
volatile float gx, gy, gz; // gx - instantaneous
volatile float imuTemp;
//volatile float imuTempRate;
volatile float Ax,Ay,Az,Gx,Gy,Gz=0.0; // IMU values in aircraft orientation

volatile float LateralGSmoothed=0.0;
volatile float PaltSmoothed=0.0;
volatile float VerticalGSmoothed=1.0; // start at 1G;
volatile float IASsmoothed=0.0;
volatile float gRoll,gPitch,gYaw;
volatile float accPitch=0.0; // smoothed pitch 
volatile float accRoll=0.0; // smoothed pitch 
volatile float gyroPitch=0.0; // for debug
volatile float gyroRoll=0.0; // for debug
volatile float rawPitch=0.0; // raw pitch


//uint8_t Ascale = 0;     // accel = +/-2G scale
uint8_t Ascale = 3;     // accel = +/-8G scale
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
volatile int flapsPos=0; // in degrees
volatile int flapsIndex=0;
int loopcount=0;
unsigned long looptime=millis();

#include <Onspeed-settingsFunctions.h> // library with setting functions

void readAccelGyro(bool tempUpdate);

void setup() {
delay(100);
attachInterrupt(SWITCH_PIN,switchCheck,CHANGE); // switch interrupt
initI2C(); // initialize i2c ports
initAccelGyro(); //initialize accelerometer & Gyro (IMU)  
Serial.print("OnSpeed Gen2 ");
Serial.println(VERSION);
Serial.println("Warming up IMU...");
delay(100);
unsigned long imuWarmupTime=millis();
//warmup IMU;
while (millis()-imuWarmupTime<2500)
    {
    readAccelGyro(true); // get temps too  
    delayMicroseconds(4808);
    }
AudioMemory(16);

// initialize SD card
sdAvailable=Sd.begin(SdioConfig(FIFO_SDIO));
// load configuration
LoadConfig();

//dataSource = "REPLAYLOGFILE";
//replayLogFileName="log.csv"; 
                 
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
   if (sdLogging) createLogFile();
#ifdef LOGDATA_PRESSURE_RATE  // sd card write rate
  Serial.println("Logging at 50Hz");
#else
  Serial.printf("Logging at %iHz\n",int(imuSampleRate));
#endif
   
    delay(100);

  // get initial pressure altitude
  Pstatic=GetStaticPressure();
  Palt=145366.45*(1-pow((Pstatic+pStaticBias)/1013.25,0.190284)); //Pstatic in milliBars,Palt in feet
  // initialize pitch and roll
  readAccelGyro(true);
  smoothedPitch=calcPitch(getAccelForAxis(forwardGloadAxis),getAccelForAxis(lateralGloadAxis), getAccelForAxis(verticalGloadAxis))+pitchBias;
  smoothedRoll=calcRoll(getAccelForAxis(forwardGloadAxis),getAccelForAxis(lateralGloadAxis), getAccelForAxis(verticalGloadAxis))+rollBias;    
  pinMode(TONE_PIN, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(FLAP_PIN, INPUT_PULLUP);
  pinMode(TESTPOT_PIN, INPUT);
  //pinMode(SWITCH_PIN, INPUT_PULLUP);

  Switch.setPressTicks(1000); // long press time
  Switch.attachClick(SwitchSingleClick);
  Switch.attachLongPressStart(SwitchLongPress);
   
  Serial.begin(BAUDRATE_CONSOLE);   //Init hardware serial port (ouput to computer for debug)

  Serial4.begin(BAUDRATE_WIFI);

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

  // initialize filters
 // MadGwick attitude filter
 filter.begin(imuSampleRate,-smoothedPitch,smoothedRoll); // start Madgwick filter at 238Hz for LSM9DS1 and 208Hz for ISM330DHXC

 // kalman altitude filter
 kalman.Configure(7.243, 15.8993, 5.6296e-09, Palt * FT2M,0.00,0.00); // configure the Kalman filter (Smooth altitude and IVSI from Baro + accelerometers)
 // optimized values: 33.9534. 25.004, 0.13843
 
 // set interrupt priorities
  
  ToneTimer.priority(96);
  ToneTimer.begin(tonePlayHandler,1000000/pps); // microseconds  

  // initialize sine wave generators
  AudioNoInterrupts();
  sinewave1.frequency(400);
  sinewave1.amplitude(0);
  AudioInterrupts();

  IMUTimer.priority(128);
  IMUTimer.begin(ReadIMU,IMU_INTERVAL);

  SensorTimer.priority(128);

  lastReceivedEfisTime=millis();
  lastReceivedBoomTime=millis();
  
  setDataSourceMode(); // box operating mode (sensors {default}), simulator, logreplay, etc)

  displayConsoleHelp();
  
  lastWatchdogRefresh=millis();
  enableWatchdog(); // start watchdog timer

  //turn audio switch/led on on power-up
  switchState=false;
  switchOnOff();
  Serial.println("System ready.\n");

 }

// main loop

void loop() {

loopcount++;

readWifiSerial();
readUSBSerial();
readEfisSerial();
readBoomSerial();
// write serialout data
writeSerialData();
// check for Serial input lockups
if (millis()-looptime > 1000)
    {
    //Serial.printf("\nloopcount: %i",loopcount);
    checkSerial();    
    looptime=millis();
    }
// SD card write once/sec
if (millis()-lastSDWrite>=SD_WRITE_TIME)
  {
   lastSDWrite=millis(); 
   SensorWriteSD();
   //Serial.printf("ACC: fwd: %.2f,lat: %.2f ,vert: %.2f\n",aFwd,aLat,aVert);
  }
//3D audio
if (audio3D && (millis()-audio3dLastUpdate)>100)
    {
    Check3DAudio();
    audio3dLastUpdate=millis();    
    }
// volume control
if (volumeControl && millis()-volumeLastUpdate>200) // update every 200ms
    {
    checkVolume();
    volumeLastUpdate=millis();    
    }
// g limit warning
if ( overgWarning && millis()-gLimitLastUpdate>=100)
    { 
    if ( millis()-gLimitLastWarning>GLIMIT_REPEAT_TIMEOUT)
        {
        checkGlimit();   
        }
     gLimitLastUpdate=millis();
    }     
// vno chime
  if ( vnoChimeEnabled && millis()-vnoChimeLastUpdate>=100)
      {
       checkVnoChime(); 
       vnoChimeLastUpdate=millis();        
      }

// calculate decel rate
if (millis()-lastDecelUpdate>100)
    {
#ifdef SPHERICAL_PROBE
iasDerivativeInput=efisIAS;    
#else
    iasDerivativeInput=IAS;
#endif  
    DecelRate=-iasDerivative.Compute();
    DecelRate=DecelRate*10;
    lastDecelUpdate=millis(); 
    }
      
// wifi data
if (sendWifiData && millis()-wifiDataLastUpdate>98) // update every 100ms (10Hz) (89ms to avoid processing delays)
    {
    SendWifiData();
    wifiDataLastUpdate=millis();
    }
    
// heartbeat
if (millis()-lastLedUpdate>300)
      {
      heartBeat();
      lastLedUpdate=millis(); 
      }

switchCheck(); // check main switch
      
// watchdog
checkWatchdog();
}  // loop

 
