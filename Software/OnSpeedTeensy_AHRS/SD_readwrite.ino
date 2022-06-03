void SensorWriteSD()
{
if (!sdLogging) return;
char SDwriteCache[SD_WRITE_BLOCK_SIZE];  
#ifdef SDCARDDEBUG
int writeCount=0;
unsigned long maxWriteTime=0;
unsigned long avgWriteTime=0;
unsigned long minWriteTime=1000000;
unsigned long openTime;
unsigned long timerstart=micros();
unsigned long sdStart=timerstart;
unsigned long closeTime;
int SDwriteBytesCount=0;
#endif


    #ifdef SDCARDDEBUG
    Serial.printf("Bytes available: %i\n",datalogBytesAvailable);
    #endif
    if (datalogBytesAvailable < SD_WRITE_BLOCK_SIZE)
        {
        return;
        }
    // Serial.println("start OpenFile");
    #ifdef SDCARDDEBUG
    timerstart=micros();
    #endif
    SensorFile = Sd.open(filenameSensor, O_WRITE | O_APPEND);
    if (!SensorFile)         
        {
        Serial.print("cannot open log file ");Serial.print(filenameSensor);Serial.println(" for writing");
        return;
        }
    #ifdef SDCARDDEBUG
    openTime=micros()-timerstart;
    #endif
                    
    while (datalogBytesAvailable > SD_WRITE_BLOCK_SIZE)
        {                       
        //move sensor cache into write cache
        int size_part1 = minimum(SD_WRITE_BLOCK_SIZE, LOG_RINGBUFFER_SIZE - datalogRingBufferStart);
        int size_part2 = SD_WRITE_BLOCK_SIZE - size_part1;
        
        memcpy(SDwriteCache, datalogRingBuffer+ datalogRingBufferStart, size_part1);
        memcpy(SDwriteCache + size_part1, datalogRingBuffer+0, size_part2);
        // update ringbuffer start
        datalogRingBufferStart=(datalogRingBufferStart+SD_WRITE_BLOCK_SIZE) % LOG_RINGBUFFER_SIZE;
        #ifdef SDCARDDEBUG
        timerstart=micros();
        #endif
        Serial.println("SDwrite");
        SensorFile.write(SDwriteCache,SD_WRITE_BLOCK_SIZE);
        //Serial.println(SDwriteCache);
        datalogBytesAvailable-=SD_WRITE_BLOCK_SIZE;
        
        #ifdef SDCARDDEBUG
        SDwriteBytesCount+=SD_WRITE_BLOCK_SIZE;
        maxWriteTime=max(maxWriteTime,micros()-timerstart);
        minWriteTime=min(minWriteTime,micros()-timerstart);
        avgWriteTime=avgWriteTime+micros()-timerstart;
        writeCount++;                    
        #endif 
        checkWatchdog();                                                                      
        } // while
        
        #ifdef SDCARDDEBUG
        avgWriteTime= round(avgWriteTime/writeCount);
        timerstart=micros();                
        #endif
        checkWatchdog();
        Serial.println("SDclosing"); 
        SensorFile.close(); // this takes long sometimes
        Serial.println("SDclosed"); 
        checkWatchdog();        
       
        #ifdef SDCARDDEBUG
        closeTime=micros()-timerstart;
        unsigned long totalTime=micros()-sdStart;                
        Serial.printf("Open: %i, Max: %i, Min: %i, Avg: %i, Close: %i, Total: %i",openTime,maxWriteTime, minWriteTime, avgWriteTime, closeTime, totalTime);
        #endif
           
        #ifdef SDCARDDEBUG                                  
        Serial.println();
        Serial.print(SDwriteBytesCount);Serial.println(" bytes.");
        SDwriteBytesCount=0; // reset write chache
        #endif                                  
            
        #ifdef TONEDEBUG 
        Serial.print("free memory: ");Serial.print(freeMemory());
        #endif                              
}

void datalogRingBufferAdd(char * logBuffer)
{ 
        
// add data to ringbuffer and update indexes  
if ((LOG_RINGBUFFER_SIZE - datalogBytesAvailable) < strlen(logBuffer))
      {
      // ring buffer full;
      //Serial.print("ring buffer full");
      return;
      }
int size_part1=minimum(strlen(logBuffer), LOG_RINGBUFFER_SIZE - datalogRingBufferEnd);
int size_part2=strlen(logBuffer)-size_part1;      
memcpy(datalogRingBuffer+datalogRingBufferEnd, logBuffer,size_part1);
memcpy(datalogRingBuffer+0, logBuffer + size_part1, size_part2);
// update ringbuffer end
datalogRingBufferEnd=(datalogRingBufferEnd+strlen(logBuffer)) % LOG_RINGBUFFER_SIZE; //wrap around to zero
datalogBytesAvailable+=strlen(logBuffer);
//Serial.printf("%lu: %i added, %s\n",micros(),strlen(logBuffer),logBuffer);

//Serial.printf("%i}",datalogBytesAvailable);
}

int minimum(int a,int b)
{
if (a<b) return a; else return b; 
}
