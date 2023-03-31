//
// installation-specific configuration for OnSpeedWifi.ino
// This file localizes values that are typically changed once for an installation
// in a specific aircraft, and rarely touched again. 
//
// If you are contributing code back to the project, and you don't want to send changes
// to this file back to the main repository, add the following line to your .gitignore file: 
// Software\OnSpeedWifi\local_config.h
//
// Alternatively, you can add the same line to .git/info/exclude & run the following command 
// to ignore changes to this file locally, without polluting the project-wide .gitignore file: 
// git update-index --skip-worktree Software\OnSpeedWifi\local_config.h
// reference: https://stackoverflow.com/questions/1753070/how-do-i-configure-git-to-ignore-some-files-locally
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
int acGrossWeight=1800;
int acCurrentWeight=1577;
float acVldmax=85;
float acGlimit=4.4;
