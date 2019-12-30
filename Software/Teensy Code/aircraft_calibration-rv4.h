// aircraft specific calibration settings for RV-4

// flap positions

// flap position definitions, degrees and potentiometer positions.
// It's important to that both arrays have the same number of elements!
// if you only have one flap position use these values:
// int flapDegrees[]={0];
// int flapPotPositions[]={0};

int flapDegrees[]={0,20,40};
int flapPotPositions[]={202,445,818};

// volume settings
#define VOLUMECONTROL // control audio volume with potentiometer, disable if not installed
#define VOLUME_HIGH_ANALOG           1
#define VOLUME_LOW_ANALOG         1023
#define AUDIO_3D
#define AUDIO_3D_CURVE(x)         -0.184*log(x) - 0.2816


// AOA setpoints for the different flap settings. Number of points must match number of flap position settings

float flapLDMAXAOA[]=        {7.41,8.00,8.5};
float flapONSPEEDFASTAOA[]=  {12.2,12.04,12.0}; // high speed, low aoa
float flapONSPEEDSLOWAOA[]=  {15.0,15.8,15.8}; // low speed, high aoa
float flapSTALLWARNAOA[]=    {16.82,18.90,20};



// aircraft & probe specific AOA curve calibration functions
// RV-4 Dynon Curve

//flaps zero
//y = -9.2263x2 + 41.011x - 26.317
#define FLAPSUP_AOA_CURVE(x)      -12.683*x*x + 49.324*x - 30.901

//flaps 20
#define FLAPSMIDDLE_AOA_CURVE(x)     -7.3513*x*x + 35.777*x - 21.868

//flaps 40
#define FLAPSDOWN_AOA_CURVE(x)     -6.4183*x*x + 34.104*x - 21.437


//RV-10 boom curve
// test boom calibration
#define BOOM_ALPHA_CALC(x)      0.0264*x -105.837;
#define BOOM_BETA_CALC(x)       0.0242*x -95.7504;
#define BOOM_STATIC_CALC(x)     0.00012207*(x - 1638)*1000; // millibars
#define BOOM_DYNAMIC_CALC(x)    (0.01525902*(x - 1638)) - 100; // millibars

//#define CAS_CURVE(x)  1.0286*x - 1.5711; // airspeed calibration curve, flight test derived
#define CAS_CURVE(x)  x; // use this if CAS curve is not available

// accelerometer orientation
#define VERTICAL_GLOAD_AXIS     "-X"  // X, Y or Z, negative for inverted values

// available efis type
//#define EFISTYPE_ADVANCED
//#define EFISTYPE_DYNONSKYVIEW
#define EFISTYPE_DYNOND10
//#define EFISTYPE_GARMING5
//#define EFISTYPE_GARMING3X

// set bias for pressure sensors (use NOLOAD command in terminal to get values for your sensors!)
//#define PFWD_BIAS           8092 // Vac's silver  box
//#define P45_BIAS            8094 // Vac's silver box
#define PFWD_BIAS             8099  // Vac's v2 box
#define P45_BIAS              8115  //Vac's v2 box
