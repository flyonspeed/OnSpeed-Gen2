float curveCalc(float x, calibrationCurve curve )
{
float result=0.00;
//calculate curve

if (curve.curveType==1) // polynomial
    {
    for (int i=0; i<MAX_CURVE_COEFF;i++)
        {  
        result+=curve.Items[i]*pow(x,MAX_CURVE_COEFF-i-1);
        //Serial.printf("%.2f * pow(%.2f,%i)+",curve.Items[i],x,MAX_CURVE_COEFF-i-1);
        }
        //Serial.printf("=%.2f\n",result);
    } else 
          if (curve.curveType==2)  // logarithmic  ex: 21 * log(x) + 16.45
                {
                result=curve.Items[MAX_CURVE_COEFF-2]*log(x)+curve.Items[MAX_CURVE_COEFF-1]; //use only last two coefficients
                //Serial.printf("%.2f * log(%.2f)+%.2f\n",curve.Items[MAX_CURVE_COEFF-2],x,curve.Items[MAX_CURVE_COEFF-1]);
                }
                  else 
                      if (curve.curveType==3) // exponential ex: 12.5*e^(-1.63x);
                          {                          
                          result=curve.Items[MAX_CURVE_COEFF-2]*exp(curve.Items[MAX_CURVE_COEFF-1]*x);
                          }
return result;
}
