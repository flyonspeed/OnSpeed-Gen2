/**
   SavLayFilter Derivative Example
   Creates a generic sin wave
   A filter is then run on it to compute the first derivative or slope of the sin wave
   The derivative is normalized by multipling by the amplitude of the sin wave
   There is a known issue that when the sin wave completes a period there is a dip in the derivative
   It is best to open Seral Plotter for best results
   Author: James Deromedi
   License: MIT License
*/
#include <SavLayFilter.h>

float phase = 0.0;
float twopi = 3.14159 * 2;
elapsedMicros usec = 0;

double outputValue;
int windowsize = 5;
int convoluteTable = 0;

SavLayFilter firstDerivative (&outputValue, 1, 7);           //Computes the first derivative

void setup() {
  Serial.begin(9800);
}

void loop() {
  outputValue = (sin(phase) * 10.0);                        //Creates sin wave pattern with A = 50
  phase = phase + 0.01;                                     //Shifts the sin wave
  if (phase >= twopi) phase = 0;                            //Resets the phase
  while (usec < 30000) ;                                    //Wait to make printing easier to read
  usec = usec - 30000;                                      //Keeps timer from overflowing

  Serial.print(outputValue);                                //Raw Value [Blue line]
  Serial.print(",");
  Serial.println(10*firstDerivative.Compute());             //First derivative of the sin wave [Orange Line]

}
