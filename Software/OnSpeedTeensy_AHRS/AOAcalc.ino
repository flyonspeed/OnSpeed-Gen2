void calcAOA (float Pfwd, float P45)
{  
float smoothingAlpha;
if (Pfwd>0)
    {
    coeffP=PCOEFF(Pfwd,P45);
    } else 
          {
          AOA=AOA_MIN_VALUE;
          coeffP=0;
          return;
          }
          
// calculate and smooth AOA
if (aoaSmoothing==0) smoothingAlpha=1.0; else smoothingAlpha=1.0/aoaSmoothing;
AOA=curveCalc(coeffP,aoaCurve[flapsIndex]) * smoothingAlpha + (1-smoothingAlpha) * AOA; // smoothing is defined by samples of lag, alpha=1/samples
if (AOA>AOA_MAX_VALUE) AOA=AOA_MAX_VALUE; else if (AOA<AOA_MIN_VALUE) AOA=AOA_MIN_VALUE;
if (isnan(AOA)) AOA=AOA_MIN_VALUE;
}



void setAOApoints(int flapIndex)
{
LDmaxAOA=flapLDMAXAOA.Items[flapIndex];
onSpeedAOAfast=flapONSPEEDFASTAOA.Items[flapIndex];
onSpeedAOAslow=flapONSPEEDSLOWAOA.Items[flapIndex];
stallWarningAOA=flapSTALLWARNAOA.Items[flapIndex];
}
