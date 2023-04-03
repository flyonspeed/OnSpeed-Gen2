//
// installation-specific configuration for OnSpeedTeensy_AHRS.ino
// This file localizes values that are typically changed once for an installation
// in a specific aircraft, and rarely touched again. 
//
// SETUP INSTRUCTIONS: (must be completed before compiling OnSpeedTeensy_AHRS.ino)
// 1. Copy this file to local_config.h in the same directory
// 2. Edit values below as appropriate to your installation & save changes
// 3. With future updates to the main repository, compare your local_config.h to the 
//    project's local_config-sample.h to determine if any new settings need to be added 
//    to your local_config.h
//
// If you are contributing code back to the project, your changes to this file will not be sent
// to the main repository due to the following entry in the .gitignore file: 
// Software\OnSpeedTeensy_AHRS\local_config.h
//

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

// OAT sensor available
//#define OAT_AVAILABLE  // DS18B20 sensor on pin 9

//analog resolution (use 13 bit analog resolution, default is 10-bit)
//#define HIGHRES_ANALOGREAD

// box functionality config
//String dataSource = "TESTPOT"; // potentiometer wiper on Pin 10 of DSUB 15 connector
//String dataSource = "RANGESWEEP";
String dataSource = "SENSORS";
//String dataSource = "REPLAYLOGFILE";
String replayLogFileName=""; 

