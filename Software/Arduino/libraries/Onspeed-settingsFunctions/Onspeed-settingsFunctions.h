// ver 3.2.20,  modified 5/31/2021
// Onspeed config library, shared between OnspeedTeensy & OnspeedWifi
// flyonspeed.org

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
// save config float values with 4 digit precision
sprintf(floatBuffer,"%.4f",value);
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

floatArray parseFloatCSV(String configString, int limit=MAX_AOA_CURVES)
{
int commaIndex=0;
floatArray result;
for (int i=0; i < limit; i++)
        {
        if (configString=="")
            {
            result.Items[i]=0;
            result.Count=i+1;            
            } else
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
String resultString="<";
resultString.concat(configName);
resultString.concat(">");
resultString.concat(configValue);
resultString.concat("</");
resultString.concat(configName);
resultString.concat(">\n");
return resultString;
//return "<"+configName+">"+configValue+"</"+configName+">\n";
}

String curve2string(calibrationCurve configArray)
{
String result="";  
for (int i=0;i < MAX_CURVE_COEFF;i++)
    {
    result.concat(floatToString(configArray.Items[i]));
    result.concat(",");
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
    result.concat(floatToString(configArray.Items[i]));
    if (i<configArray.Count-1) result.concat(",");
    }
return result;    
}


String int_array2string(intArray configArray)
{
String result="";  
for (int i=0;i < configArray.Count;i++)
    {
    result.concat(configArray.Items[i]);
    if (i<configArray.Count-1) result.concat(",");
    }
return result;     
}

void configurationToString(String &configString)
{
configString="<CONFIG>\n";
configString.concat(makeConfig("AOA_SMOOTHING",String(aoaSmoothing)));
configString.concat(makeConfig("PRESSURE_SMOOTHING",String(pressureSmoothing)));

configString.concat(makeConfig("DATASOURCE",String(dataSource)));
configString.concat(makeConfig("REPLAYLOGFILENAME",String(replayLogFileName)));
configString.concat(makeConfig("FLAPDEGREES",int_array2string(flapDegrees)));
configString.concat(makeConfig("FLAPPOTPOSITIONS",int_array2string(flapPotPositions)));

configString.concat(makeConfig("VOLUMECONTROL",String(volumeControl)));
configString.concat(makeConfig("VOLUME_HIGH_ANALOG",String(volumeHighAnalog)));
configString.concat(makeConfig("VOLUME_LOW_ANALOG",String(volumeLowAnalog)));
configString.concat(makeConfig("VOLUME_DEFAULT",String(defaultVolume)));
configString.concat(makeConfig("3DAUDIO",String(audio3D)));
configString.concat(makeConfig("MUTE_AUDIO_UNDER_IAS",String(muteAudioUnderIAS)));
configString.concat(makeConfig("OVERGWARNING",String(overgWarning)));

configString.concat(makeConfig("SETPOINT_LDMAXAOA",float_array2string(flapLDMAXAOA)));
configString.concat(makeConfig("SETPOINT_ONSPEEDFASTAOA",float_array2string(flapONSPEEDFASTAOA)));
configString.concat(makeConfig("SETPOINT_ONSPEEDSLOWAOA",float_array2string(flapONSPEEDSLOWAOA)));
configString.concat(makeConfig("SETPOINT_STALLWARNAOA",float_array2string(flapSTALLWARNAOA)));
configString.concat(makeConfig("SETPOINT_STALLAOA",float_array2string(flapSTALLAOA)));
configString.concat(makeConfig("SETPOINT_MANAOA",float_array2string(flapMANAOA)));
// total flap curves
// to get number of curves count flap stops
for (int i=0;i<flapDegrees.Count;i++)
{
configString.concat(makeConfig("AOA_CURVE_FLAPS"+String(i),curve2string(aoaCurve[i])));
}
// CAS curve
configString.concat(makeConfig("CAS_CURVE",curve2string(casCurve)));
configString.concat(makeConfig("CAS_ENABLED",String(casCurveEnabled)));


configString.concat(makeConfig("PORTS_ORIENTATION",portsOrientation));
configString.concat(makeConfig("BOX_TOP_ORIENTATION",boxtopOrientation));
configString.concat(makeConfig("EFISTYPE",efisType));

// calibration data source
configString.concat(makeConfig("CALWIZ_SOURCE",calSource));

// biases
configString.concat(makeConfig("PFWD_BIAS",String(pFwdBias)));
configString.concat(makeConfig("P45_BIAS",String(p45Bias)));
configString.concat(makeConfig("PSTATIC_BIAS",floatToString(pStaticBias)));
configString.concat(makeConfig("GX_BIAS",floatToString(gxBias)));
configString.concat(makeConfig("GY_BIAS",floatToString(gyBias)));
configString.concat(makeConfig("GZ_BIAS",floatToString(gzBias)));
configString.concat(makeConfig("PITCH_BIAS",floatToString(pitchBias)));
configString.concat(makeConfig("ROLL_BIAS",floatToString(rollBias)));

// serial inputs
configString.concat(makeConfig("BOOM",String(readBoom)));
configString.concat(makeConfig("SERIALEFISDATA",String(readEfisData)));

// load limit
configString.concat(makeConfig("LOADLIMITPOSITIVE",String(loadLimitPositive)));
configString.concat(makeConfig("LOADLIMITNEGATIVE",String(loadLimitNegative)));

// vno chime
configString.concat(makeConfig("VNO",String(Vno)));
configString.concat(makeConfig("VNO_CHIME_INTERVAL",String(vnoChimeInterval)));
configString.concat(makeConfig("VNO_CHIME_ENABLED",String(vnoChimeEnabled)));

// serial out
configString.concat(makeConfig("SERIALOUTFORMAT",serialOutFormat));
configString.concat(makeConfig("SERIALOUTPORT",serialOutPort));

// sdLogging
configString.concat(makeConfig("SDLOGGING",String(sdLoggingConfig)));    

configString.concat("</CONFIG>");
    
//return configString;
return;    
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
flapSTALLAOA=parseFloatCSV(getConfigValue(configString,"SETPOINT_STALLAOA"),flapDegrees.Count); // flapSTALLAOA is only availabel after calibration wizard run, limit it to number of flap curves
flapMANAOA=parseFloatCSV(getConfigValue(configString,"SETPOINT_MANAOA"),flapDegrees.Count); // flapMANAOA is only availabel after calibration wizard run,limit it to number of flap curves

// aoa curves: AOA_CURVE_FLAPS0, AOA_CURVE_FLAPS1,...
for (int i=0;i<flapDegrees.Count;i++)
{
aoaCurve[i]=parseCurveCSV(getConfigValue(configString,"AOA_CURVE_FLAPS"+String(i)));
}

//CAS curve
casCurve=parseCurveCSV(getConfigValue(configString,"CAS_CURVE"));
casCurveEnabled=stringToBoolean(getConfigValue(configString,"CAS_ENABLED"));

portsOrientation=getConfigValue(configString,"PORTS_ORIENTATION");
boxtopOrientation=getConfigValue(configString,"BOX_TOP_ORIENTATION");
efisType=getConfigValue(configString,"EFISTYPE");
if (efisType=="VN-300") efisID=1; else
    if (efisType=="ADVANCED") efisID=2; else
        if (efisType=="DYNOND10") efisID=3; else
            if (efisType=="GARMING5") efisID=4; else
                    if (efisType=="GARMING3X") efisID=5; else efisID=0;
    
// calibration data source
calSource=getConfigValue(configString,"CALWIZ_SOURCE");

// biases
pFwdBias=getConfigValue(configString,"PFWD_BIAS").toInt();
p45Bias=getConfigValue(configString,"P45_BIAS").toInt();
pStaticBias=stringToFloat(getConfigValue(configString,"PSTATIC_BIAS"));
gxBias=stringToFloat(getConfigValue(configString,"GX_BIAS"));
gyBias=stringToFloat(getConfigValue(configString,"GY_BIAS"));
gzBias=stringToFloat(getConfigValue(configString,"GZ_BIAS"));
pitchBias=stringToFloat(getConfigValue(configString,"PITCH_BIAS"));
rollBias=stringToFloat(getConfigValue(configString,"ROLL_BIAS"));

// serial inputs
readBoom=stringToBoolean(getConfigValue(configString,"BOOM"));
readEfisData=stringToBoolean(getConfigValue(configString,"SERIALEFISDATA"));

// serial output
serialOutFormat=getConfigValue(configString,"SERIALOUTFORMAT");
serialOutPort=getConfigValue(configString,"SERIALOUTPORT");

// load limit
loadLimitPositive=getConfigValue(configString,"LOADLIMITPOSITIVE").toFloat();
loadLimitNegative=getConfigValue(configString,"LOADLIMITNEGATIVE").toFloat();


// vno chime
Vno=getConfigValue(configString,"VNO").toInt();
vnoChimeInterval=getConfigValue(configString,"VNO_CHIME_INTERVAL").toInt();
vnoChimeEnabled=stringToBoolean(getConfigValue(configString,"VNO_CHIME_ENABLED"));

// SD card logging    
sdLoggingConfig=stringToBoolean(getConfigValue(configString,"SDLOGGING"));
sdLogging=sdLoggingConfig; 
return true;  
}

void addCRC(String &configString)
{
int16_t calcCRC=0;
String configContent=getConfigValue(configString,"CONFIG");
for (unsigned int i=0;i<configContent.length();i++) calcCRC+=configContent[i];
// add CRC
configString.concat("<CHECKSUM>");
configString.concat(String(calcCRC, HEX));
configString.concat("</CHECKSUM>");
return;
}