void enableWatchdog()
{
noInterrupts();  
WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
delayMicroseconds(1); // Need to wait a bit..
WDOG_STCTRLH = 0x0001; // Enable WDG
WDOG_TOVALL = 5000; // The next 2 lines sets the time-out value in millissec
WDOG_TOVALH = 0;
WDOG_PRESC = 0; // This sets prescale clock so that the watchdog timer ticks at 1kHZ instead of the default 1kHZ/4 = 200 HZ
interrupts();
watchdogEnabled=true;
Serial.println("Watchdog Timer enabled (5 seconds)");
}

void watchdogRefresh()
{
noInterrupts();  
WDOG_REFRESH = 0xA602;
WDOG_REFRESH = 0xB480;
interrupts();
}

void checkWatchdog()
{
if (watchdogEnabled && millis()-lastWatchdogRefresh>500) // refresh watchdog twice per sec
    {    
    watchdogRefresh();
    lastWatchdogRefresh=millis();
    }
}
