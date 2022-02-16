/**
   SavLayFilter Basic Example
   Creates a generic sin wave and adds noise from a floating analogpin
   Two filters are then run on it with a large windowsize and a small windowsize
   It is best to open Serial plotter for best results

   For non-Teensy users, elapsedMillis is required
   It can be downloaded from: https://github.com/pfeerick/elapsedMillis/archive/master.zip
   
   Author: James Deromedi
   License: MIT License
*/

#include <SavLayFilter.h>

float phase = 0.0;
float twopi = 3.14159 * 2;
elapsedMicros usec = 0;
double outputValue;

SavLayFilter smallFilter (&outputValue, 0, 5);             //Cubic smoothing with windowsize of 5
SavLayFilter largeFilter (&outputValue, 0, 25);            //Cubic smoothing with windowsize of 25

//==================================================================================================
void setup() {
  Serial.begin(9800);
}//END SETUP

//==================================================================================================
void loop() {
  float noise = analogRead(A1);                             //Creates random noise from a floating pin
  outputValue = (sin(phase) * 1000.0 + 2000.0) + noise;     //Creates sin wave pattern with A = 1000 and shifted up by 2000
  phase = phase + 0.02;                                     //Shifts the sin wave
  if (phase >= twopi) phase = 0;                            //Resets the phase
  while (usec < 30000) ;                                    //Wait to make printing easier to read
  usec = usec - 30000;                                      //Keeps timer from overflowing

  Serial.print(outputValue);                                //Raw Value [Blue line]
  Serial.print(",");
  Serial.print(smallFilter.Compute());                      //Smoothed value of smaller window [Orange line]
  Serial.print(",");
  Serial.println(largeFilter.Compute());                    //Smoothed value of smaller window [Red line]
}//END LOOP
