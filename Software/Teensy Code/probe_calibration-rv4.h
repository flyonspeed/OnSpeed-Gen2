// aircraft & probe specific AOA curve calibration functions
// RV-4 calibrations

//flaps zero
#define FLAPSUP_AOA_CURVE(x)      17.76*log(x) + 2.9743+3.6

//flaps 20
#define FLAPSMIDDLE_AOA_CURVE(x)      0.4565*x*x + 9.4844*x - 6.3

//flaps 40
#define FLAPSDOWN_AOA_CURVE(x)      -9.1295*x*x + 41.373*x - 32.928

// test boom calibration
#define BOOM_ALPHA_CALC(x)      0.0264*x -105.837;
#define BOOM_BETA_CALC(x)       0.0242*x -95.7504;
#define BOOM_STATIC_CALC(x)     0.00012207*(x - 1638)*1000; // millibars
#define BOOM_DYNAMIC_CALC(x)    (0.01525902*(x - 1638)) - 100; // millibars
