bool loadConfigurationFile(char* filename)
{
String configString="";
// if config file exists on SD card load it, otherwise load the defaultconfig
FsFile configFile;
  configFile = Sd.open(filename, O_READ);
  if (configFile) {
                  while (configFile.available())
                        {
                        configString+=char(configFile.read());                                                                                      
                        }
                  // close the file:
                  configFile.close();
                  return loadConfigFromString(configString);                
                  } else return false;            
}

bool saveConfigurationToFile(char* filename, String configString)
{
FsFile configFile;
configFile = Sd.open(filename, O_WRITE | O_CREAT | O_TRUNC);
if (configFile) {
            configFile.print(configString);
            configFile.close();
            return true;
            } else
              {
              Serial.println("Could not save config file");
              return false;
              }
}


bool loadDefaultConfiguration()
{
// load default config from PROGMEM
// this config will load when there's no config on the SDcard
String configString=String(DEFAULT_CONFIG);
return loadConfigFromString(configString);
}

void LoadConfig()
{
// load configuration
if (sdAvailable && Sd.exists(configFilename))
    {
    // load config from file
    Serial.println("Loading onspeed.cfg configuration");
    configLoaded=loadConfigurationFile(configFilename);
    if (configLoaded) Serial.println("Configuration loaded.");
    } else
        {
        // load default config 
        Serial.println("No onspeed.cfg file on card, loading default configuration");
        configLoaded=loadDefaultConfiguration();
        if (configLoaded) Serial.println("Default configuration loaded.");else Serial.println("Could not load default configuration.");
        } 
}
