void checkGlimit()
{
if (gRoll>=ASYMMETRIC_GYRO_LIMIT || gYaw>=ASYMMETRIC_GYRO_LIMIT)
  {
  calculatedGLimitPositive=loadLimitPositive*0.666;
  calculatedGLimitNegative=loadLimitNegative*0.666;
  } else
        {
        calculatedGLimitPositive=loadLimitPositive;
        calculatedGLimitNegative=loadLimitNegative;
        }
if (aVert >= calculatedGLimitPositive || aVert <= calculatedGLimitNegative)
  {
  gLimitLastWarning=millis();  
  voice1.play(AudioSampleGlimit);
  }
}
