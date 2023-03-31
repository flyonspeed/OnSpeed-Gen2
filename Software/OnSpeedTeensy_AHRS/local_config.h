//
// installation-specific configuration for OnSpeedTeensy_AHRS.ino
// This file localizes values that are typically changed once for an installation
// in a specific aircraft, and rarely touched again. 
//
// If you are contributing code back to the project, and you don't want to send changes
// to this file back to the main repository, add the following line to your .gitignore file: 
// Software\OnSpeedTeensy_AHRS\local_config.h
//
// Alternatively, you can add the same line to .git/info/exclude & run the following command 
// to ignore changes to this file locally, without polluting the project-wide .gitignore file: 
// git update-index --skip-worktree Software\OnSpeedTeensy_AHRS\local_config.h
// reference: https://stackoverflow.com/questions/1753070/how-do-i-configure-git-to-ignore-some-files-locally
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

