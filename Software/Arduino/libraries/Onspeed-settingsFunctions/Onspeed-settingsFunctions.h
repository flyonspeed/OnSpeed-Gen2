bool stringToBoolean(String configString)
{
if (configString.toInt()==1 || configString=="YES" || configString=="ENABLED" || configString=="ON") return true; else return false;
}

float stringToFloat(String string)
{
return  atof(string.c_str());
}

String floatToString(float value)
{
char floatBuffer[20];
sprintf(floatBuffer,"%.3f",value);
return String (floatBuffer);
}

intArray parseIntCSV(String configString)
{
int commaIndex=0;
intArray result;
for (int i=0; i < MAX_AOA_CURVES; i++)
        {
        commaIndex=configString.indexOf(',');
        if (commaIndex>0)
                 { 
                  result.Items[i]=configString.substring(0,commaIndex).toInt();
                  result.Count=i+1;                  
                  configString=configString.substring(commaIndex+1,configString.length());                  
                 } 
                  else
                      {
                      // last value
                       result.Items[i]=configString.toInt();
                       result.Count=i+1;
                       return result;
                      }
        
        }
return result;        
}

floatArray parseFloatCSV(String configString)
{
int commaIndex=0;
floatArray result;
for (int i=0; i < MAX_AOA_CURVES; i++)
        {
        commaIndex=configString.indexOf(',');
        if (commaIndex>0)
                 { 
                  result.Items[i]=stringToFloat(configString.substring(0,commaIndex));
                  result.Count=i+1;                  
                  configString=configString.substring(commaIndex+1,configString.length());
                 } 
                  else
                      {
                      // last value
                       result.Items[i]=stringToFloat(configString);
                       result.Count=i+1;
                       return result;
                      }
        
        }
return result;        
}

calibrationCurve parseCurveCSV(String configString)
{
int commaIndex=0;
calibrationCurve result;
for (int i=0; i <= MAX_CURVE_COEFF; i++)
        {
        commaIndex=configString.indexOf(',');
        if (commaIndex>0)
                 { 
                  result.Items[i]=stringToFloat(configString.substring(0,commaIndex));
                  configString=configString.substring(commaIndex+1,configString.length());
                 } 
            else
                {
                // last value is curveType                
                result.curveType=configString.toInt();                
                return result;
                }        
        }
return result;        
}

String getConfigValue(String configString,String configName)
{
// parse a config value from config string
int startIndex;
int endIndex;
startIndex=configString.indexOf("<"+configName+">");
endIndex=configString.indexOf("</"+configName+">");
if (startIndex<0 || endIndex<0) return ""; // value not found in config
return configString.substring(startIndex+configName.length()+2,endIndex);
}

String makeConfig(String configName,String configValue)
{
return "<"+configName+">"+configValue+"</"+configName+">\n";
}

String curve2string(calibrationCurve configArray)
{
String result="";  
for (int i=0;i < MAX_CURVE_COEFF;i++)
    {
    result+=floatToString(configArray.Items[i]);
    result+=",";
    }
// add curveType to the end
result+=String(configArray.curveType);
return result;    
}

String float_array2string(floatArray configArray)
{
String result="";  
for (int i=0;i < configArray.Count;i++)
    {
    result+=floatToString(configArray.Items[i]);
    if (i<configArray.Count-1) result+=",";
    }
return result;    
}


String int_array2string(intArray configArray)
{
String result="";  
for (int i=0;i < configArray.Count;i++)
    {
    result+=configArray.Items[i];
    if (i<configArray.Count-1) result+=",";
    }
return result;     
}

String configurationToString()
{
String configString="<CONFIG>\n";
configString+=makeConfig("AOA_SMOOTHING",String(aoaSmoothing));
configString+=makeConfig("PRESSURE_SMOOTHING",String(pressureSmoothing));

configString+=makeConfig("DATASOURCE",String(dataSource));
configString+=makeConfig("REPLAYLOGFILENAME",String(replayLogFileName));
configString+=makeConfig("FLAPDEGREES",int_array2string(flapDegrees));
configString+=makeConfig("FLAPPOTPOSITIONS",int_array2string(flapPotPositions));

configString+=makeConfig("VOLUMECONTROL",String(volumeControl));
configString+=makeConfig("VOLUME_HIGH_ANALOG",String(volumeHighAnalog));
configString+=makeConfig("VOLUME_LOW_ANALOG",String(volumeLowAnalog));
configString+=makeConfig("VOLUME_DEFAULT",String(defaultVolume));
configString+=makeConfig("3DAUDIO",String(audio3D));
configString+=makeConfig("MUTE_AUDIO_UNDER_IAS",String(muteAudioUnderIAS));
configString+=makeConfig("OVERGWARNING",String(overgWarning));

configString+=makeConfig("SETPOINT_LDMAXAOA",float_array2string(flapLDMAXAOA));
configString+=makeConfig("SETPOINT_ONSPEEDFASTAOA",float_array2string(flapONSPEEDFASTAOA));
configString+=makeConfig("SETPOINT_ONSPEEDSLOWAOA",float_array2string(flapONSPEEDSLOWAOA));
configString+=makeConfig("SETPOINT_STALLWARNAOA",float_array2string(flapSTALLWARNAOA));
// total flap curves
// to get number of curves count flap stops
for (int i=0;i<flapDegrees.Count;i++)
{
configString+=makeConfig("AOA_CURVE_FLAPS"+String(i),curve2string(aoaCurve[i]));
}
// boom curves
configString+=makeConfig("BOOM_ALPHA_CURVE",curve2string(boomAlphaCurve));
configString+=makeConfig("BOOM_BETA_CURVE",curve2string(boomBetaCurve));
configString+=makeConfig("BOOM_STATIC_CURVE",curve2string(boomStaticCurve));
configString+=makeConfig("BOOM_DYNAMIC_CURVE",curve2string(boomDynamicCurve));
// CAS curve
configString+=makeConfig("CAS_CURVE",curve2string(casCurve));

configString+=makeConfig("PORTS_ORIENTATION",portsOrientation);
configString+=makeConfig("BOX_TOP_ORIENTATION",boxtopOrientation);
configString+=makeConfig("EFISTYPE",efisType);

// biases
configString+=makeConfig("PFWD_BIAS",String(pFwdBias));
configString+=makeConfig("P45_BIAS",String(p45Bias));
configString+=makeConfig("PSTATIC_BIAS",floatToString(pStaticBias));
configString+=makeConfig("GX_BIAS",floatToString(gxBias));
configString+=makeConfig("GY_BIAS",floatToString(gyBias));
configString+=makeConfig("GZ_BIAS",floatToString(gzBias));
configString+=makeConfig("PITCH_BIAS",floatToString(pitchBias));


// serial inputs
configString+=makeConfig("BOOM",String(readBoom));
configString+=makeConfig("SERIALEFISDATA",String(readEfisData));

// load limit
configString+=makeConfig("LOADLIMIT",String(loadLimit));

// serial out
configString+=makeConfig("SERIALOUTFORMAT",serialOutFormat);
configString+=makeConfig("SERIALOUTPORT",serialOutPort);

// sdLogging
configString+=makeConfig("SDLOGGING",String(sdLoggingConfig));    

configString+="</CONFIG>";


    
return configString;
}

bool loadConfigFromString(String configString)
{
aoaSmoothing=getConfigValue(configString,"AOA_SMOOTHING").toInt();
pressureSmoothing=getConfigValue(configString,"PRESSURE_SMOOTHING").toInt();
muteAudioUnderIAS=getConfigValue(configString,"MUTE_AUDIO_UNDER_IAS").toInt();
dataSource=getConfigValue(configString,"DATASOURCE");
replayLogFileName=getConfigValue(configString,"REPLAYLOGFILENAME");    
// flap positions
flapDegrees=parseIntCSV(getConfigValue(configString,"FLAPDEGREES"));

flapPotPositions=parseIntCSV(getConfigValue(configString,"FLAPPOTPOSITIONS"));
// volume
volumeControl=stringToBoolean(getConfigValue(configString,"VOLUMECONTROL"));
volumeHighAnalog=getConfigValue(configString,"VOLUME_HIGH_ANALOG").toInt();
volumeLowAnalog=getConfigValue(configString,"VOLUME_LOW_ANALOG").toInt();
defaultVolume=getConfigValue(configString,"VOLUME_DEFAULT").toInt();
if (!volumeControl)
                  {
                  volumePercent=defaultVolume;
                  }
audio3D=stringToBoolean(getConfigValue(configString,"3DAUDIO"));
overgWarning=stringToBoolean(getConfigValue(configString,"OVERGWARNING"));

// setpoints
flapLDMAXAOA=parseFloatCSV(getConfigValue(configString,"SETPOINT_LDMAXAOA"));
flapONSPEEDFASTAOA=parseFloatCSV(getConfigValue(configString,"SETPOINT_ONSPEEDFASTAOA"));
flapONSPEEDSLOWAOA=parseFloatCSV(getConfigValue(configString,"SETPOINT_ONSPEEDSLOWAOA"));
flapSTALLWARNAOA=parseFloatCSV(getConfigValue(configString,"SETPOINT_STALLWARNAOA"));

// aoa curves: AOA_CURVE_FLAPS0, AOA_CURVE_FLAPS1,...
for (int i=0;i<flapDegrees.Count;i++)
{
aoaCurve[i]=parseCurveCSV(getConfigValue(configString,"AOA_CURVE_FLAPS"+String(i)));
}

// boom curves
boomAlphaCurve=parseCurveCSV(getConfigValue(configString,"BOOM_ALPHA_CURVE"));
boomBetaCurve=parseCurveCSV(getConfigValue(configString,"BOOM_BETA_CURVE"));
boomStaticCurve=parseCurveCSV(getConfigValue(configString,"BOOM_STATIC_CURVE"));
boomDynamicCurve=parseCurveCSV(getConfigValue(configString,"BOOM_DYNAMIC_CURVE"));

//CAS curve
casCurve=parseCurveCSV(getConfigValue(configString,"CAS_CURVE"));

portsOrientation=getConfigValue(configString,"PORTS_ORIENTATION");
boxtopOrientation=getConfigValue(configString,"BOX_TOP_ORIENTATION");
efisType=getConfigValue(configString,"EFISTYPE");

// biases
pFwdBias=getConfigValue(configString,"PFWD_BIAS").toInt();
p45Bias=getConfigValue(configString,"P45_BIAS").toInt();
pStaticBias=stringToFloat(getConfigValue(configString,"PSTATIC_BIAS"));
gxBias=stringToFloat(getConfigValue(configString,"GX_BIAS"));
gyBias=stringToFloat(getConfigValue(configString,"GY_BIAS"));
gzBias=stringToFloat(getConfigValue(configString,"GZ_BIAS"));
pitchBias=stringToFloat(getConfigValue(configString,"PITCH_BIAS"));

// serial inputs
readBoom=stringToBoolean(getConfigValue(configString,"BOOM"));
readEfisData=stringToBoolean(getConfigValue(configString,"SERIALEFISDATA"));

// serial output
serialOutFormat=getConfigValue(configString,"SERIALOUTFORMAT");
serialOutPort=getConfigValue(configString,"SERIALOUTPORT");

// load limit
loadLimit=getConfigValue(configString,"LOADLIMIT").toFloat();

// SD card logging    
sdLoggingConfig=stringToBoolean(getConfigValue(configString,"SDLOGGING"));
sdLogging=sdLoggingConfig; 
return true;  
}

String addCRC(String configString)
{
int16_t calcCRC=0;
String configContent=getConfigValue(configString,"CONFIG");
for (unsigned int i=0;i<configContent.length();i++) calcCRC+=configContent[i];                     
// add CRC
configString+="<CHECKSUM>"+String(calcCRC, HEX)+"</CHECKSUM>";
return configString;
}