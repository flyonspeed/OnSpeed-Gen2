#ifndef TDNN_CLASSIFIER_H_
#define TDNN_CLASSIFIER_H_

#include "CircularBuffer.h"
#include "Data.h"

class TDNNClassifier {

public:
  TDNNClassifier() : _num(0) {}
  float *Predict(Data data);

private:
  CircularBuffer<Data, 100> _input;
  int _num;

  struct XStep {
    float xoffset[8];
    float gain[8];
    float ymin;
  };

  struct YStep {
    float xoffset[1];
    float gain[1];
    float ymin;
  };

  static const XStep X1_STEP1;
  static const YStep Y1_STEP1;
  static const float B1net[20];
  static const float B2net[1];
  static const float IW1_1[20][800];
  static const float LW2_1[1][20];
};

#endif
