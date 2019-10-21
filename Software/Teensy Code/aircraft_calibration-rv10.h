// aircraft specific calibration settings for RV-10

// flap position definitions, degrees and potentiometer positions.
// It's important to that both arrays have the same number of elements!
// if you only have one flap position use these values:
// int flapDegrees[]={0];
// int flapPotPositions[]={0};

int flapDegrees[]={0,15,30};
int flapPotPositions[]={675,391,191};

// volume position
//#define VOLUME_HIGH_ANALOG           1023
//#define VOLUME_LOW_ANALOG         1

// AOA setpoints for the different flap settings. Number of points must match number of flap position settings
float flapLDMAXAOA[]=    {8.446,10.421,13.814};// !L/Dmax out of range here
float flapONSPEEDAOA[]=  {9.333,11.789,13.246};
float flapSTALLWARNAOA[]={13.579,16.195,19.749};

float ONSPEED_BAND_HIGH=1; //degrees AOA slop for the solid tone, low airspeed, high angle
float ONSPEED_BAND_LOW=0.5; //degrees AOA slop for the solid tone, high airspeed, low angle

#define CAS_CURVE(x)  1.0286*x - 1.5711; // airspeed calibration curve, flight test derived

//#define CAS_CURVE(x)  x; // use this if CAS curve is not available

// accelerometer orientation, box orientation as installed in the aircraft
#define VERTICAL_GLOAD_AXIS     "-X"  // X, Y or Z, negative for inverted values

// available efis type
#define EFISTYPE_ADVANCED
//#define EFISTYPE_DYNONSKYVIEW
//#define EFISTYPE_DYNOND10
//#define EFISTYPE_GARMING5
//#define EFISTYPE_GARMING3X

// set bias for pressure sensors (use NOLOAD command in terminal to get values for your sensors!)
#define PFWD_BIAS             8125  // Lenny's v2 box
#define P45_BIAS              8142  // Lenny's v2 box
//#define PFWD_BIAS           8093  // red box
//#define P45_BIAS            8072  // red box
//#define PFWD_BIAS           8092 // Vac's silver  box
//#define P45_BIAS            8094 // Vac's silver box
//#define PFWD_BIAS             8099  // Vac's v2 box
//#define P45_BIAS              8115  //Vac's v2 box
