//
// installation-specific configuration for OnSpeedWifi.ino
// This file localizes values that are typically changed once for an installation
// in a specific aircraft, and rarely touched again. 
//
// SETUP INSTRUCTIONS: (must be completed before compiling OnSpeedWifi.ino)
// 1. Copy this file to local_config.h in the same directory
// 2. Edit values below as appropriate to your installation & save changes
// 3. With future updates to the main repository, compare your local_config.h to the 
//    project's local_config-sample.h to determine if any new settings need to be added 
//    to your local_config.h
//
// If you are contributing code back to the project, your changes to this file will not be sent
// to the main repository due to the following entry in the .gitignore file: 
// Software\OnSpeedWifi\local_config-sample.h
//

const char* ssid = "OnSpeed";
const char* password = "angleofattack";
String clientwifi_ssid="HangarWifi"; // currently not needed
String clientwifi_password="test"; // currently not needed

// initialize config variables
#define MAX_AOA_CURVES    5 // maximum number of AOA curves (flap/gear positions)
#define MAX_CURVE_COEFF   4 // 4 coefficients=3rd degree polynomial function for calibration curves
String dataSource = "SENSORS";

// calibration wizard variables
int acGrossWeight=2700;
int acCurrentWeight=2500;
float acVldmax=91;
float acGlimit=3.8;
