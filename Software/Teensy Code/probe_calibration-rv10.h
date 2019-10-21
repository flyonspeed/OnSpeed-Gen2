// aircraft & probe specific AOA curve calibration functions
#define FLAPSUP_AOA_CURVE(x)     -28.208*x*x + 85.301*x - 49.941; // absolute AOA derived from horsehoe GPS test on 9/25/2019, N311LZ
#define FLAPSMIDDLE_AOA_CURVE(x) -20.679*x*x + 71.473*x - 42.295;
#define FLAPSDOWN_AOA_CURVE(x)   -30.287*x*x + 101.07*x - 61.84;

// test boom calibration
#define BOOM_ALPHA_CALC(x)      0.0264*x -105.837;
#define BOOM_BETA_CALC(x)       0.0242*x -95.7504;
#define BOOM_STATIC_CALC(x)     0.00012207*(x - 1638)*1000; // millibars
#define BOOM_DYNAMIC_CALC(x)    (0.01525902*(x - 1638)) - 100; // millibars
