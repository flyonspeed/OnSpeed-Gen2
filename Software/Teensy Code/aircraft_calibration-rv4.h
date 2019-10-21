// aircraft specific calibration settings for RV-4

// flap positions

// flap position definitions, degrees and potentiometer positions.
// It's important to that both arrays have the same number of elements!
// if you only have one flap position use these values:
// int flapDegrees[]={0];
// int flapPotPositions[]={0};

int flapDegrees[]={0,20,40};
int flapPotPositions[]={202,445,818};

// volume position
#define VOLUME_HIGH_ANALOG           1
#define VOLUME_LOW_ANALOG         1023

// AOA setpoints for the different flap settings. Number of points must match number of flap position settings
float flapLDMAXAOA[]=    {8.40,5.80,4.83};
float flapONSPEEDAOA[]=  {13.55,8.35,8.02};
float flapSTALLWARNAOA[]={16.72,14.13,12.98};


// accelerometer orientation
#define VERTICAL_GLOAD_AXIS     "-X"  // X, Y or Z, negative for inverted values
