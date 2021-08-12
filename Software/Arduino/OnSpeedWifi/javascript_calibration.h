const char jsCalibration[] PROGMEM = R"=====(
<script language="javascript" type="text/javascript">
 var wsUri = "ws://192.168.0.1:81";
 var lastUpdate= Date.now();
 var lastDisplay=Date.now(); 
 var AOA=0;
 var IAS=0;
 var prevIAS=0;
 var FwdAccel=0;
 var PAlt=0;
 var GLoad=1;
 var GLoadLat=0;
 var PitchAngle=0;
 var RollAngle=0;
 var smoothingAlpha=.9;
 var smoothingAlphaFwdAcc=.1;
 var liveConnecting=false;
 var iasArray=[];
//setInterval(updateAge,500);

function init()
  {    
    writeToStatus("CONNECTING...");
    connectWebSocket();
  }

  function connectWebSocket()
  {
    liveConnecting=true
    websocket = new WebSocket(wsUri);
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    websocket.onerror = function(evt) { onError(evt) };
  }

  function onOpen(evt)
  {
    writeToStatus("CONNECTED");
    liveConnecting=false
  }

  function onClose(evt)
  {
    writeToStatus("Reconnecting...");    
    setTimeout(connectWebSocket, 1000);  
  }
  
 function map(x, in_min, in_max, out_min, out_max)
{
if ((in_max - in_min) + out_min ==0) return 0;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}  

 function constrain(x,out_min,out_max)
 {
 if (x<out_min) x=out_min; else if (x>out_max) x=out_max;
 return x;
 }
  function onMessage(evt)
  {    
   // smoother values are display with the formula: value = measurement*alpha + previous value*(1-alpha)   
   try {
   //console.log(evt.data);
   OnSpeedArray = evt.data.split(",");
   var OnSpeed = new Object();
   OnSpeed.AOA= OnSpeedArray[1]; 
   OnSpeed.Pitch= OnSpeedArray[2]; 
   OnSpeed.Roll= OnSpeedArray[3]; 
   OnSpeed.IAS= OnSpeedArray[4]; 
   OnSpeed.PAlt= OnSpeedArray[5]; 
   OnSpeed.verticalGLoad= OnSpeedArray[6]; 
   OnSpeed.lateralGLoad= OnSpeedArray[7]; 
   OnSpeed.alphaVA= OnSpeedArray[8]; 
   OnSpeed.LDmax= OnSpeedArray[9]; 
   OnSpeed.OnspeedFast= OnSpeedArray[10]; 
   OnSpeed.OnspeedSlow= OnSpeedArray[11]; 
   OnSpeed.OnspeedWarn= OnSpeedArray[12]; 
   OnSpeed.flapsPos= OnSpeedArray[13]; 
   OnSpeed.coeffP= OnSpeedArray[14]; 
   OnSpeed.dataMark= OnSpeedArray[15]; 
   OnSpeed.CRC= OnSpeedArray[16]; 
   var crc_string=OnSpeed.AOA+','+OnSpeed.Pitch+','+OnSpeed.Roll+','+OnSpeed.IAS+','+OnSpeed.PAlt+','+OnSpeed.verticalGLoad+','+OnSpeed.lateralGLoad+','+OnSpeed.alphaVA+','+OnSpeed.LDmax+','+OnSpeed.OnspeedFast+','+OnSpeed.OnspeedSlow+','+OnSpeed.OnspeedWarn+','+OnSpeed.flapsPos+','+OnSpeed.coeffP+','+OnSpeed.dataMark
   //console.log("CRC",crc_string);
   var crc_calc=0;
   for (i=0;i<crc_string.length;i++)
       {
       crc_calc+=crc_string.charCodeAt(i);
       crc_calc=crc_calc & 0xFF;
        }
   if (OnSpeed.CRC==crc_calc)
    {
    // CRC ok, use values
    AOA=(OnSpeed.AOA*smoothingAlpha+AOA*(1-smoothingAlpha)).toFixed(2);
    IAS=(OnSpeed.IAS*smoothingAlpha+IAS*(1-smoothingAlpha)).toFixed(2);
    PAlt=(OnSpeed.PAlt*smoothingAlpha+PAlt*(1-smoothingAlpha)).toFixed(2);
    GLoad=(OnSpeed.verticalGLoad*smoothingAlpha+GLoad*(1-smoothingAlpha)).toFixed(2);
    GLoadLat=(OnSpeed.lateralGLoad*smoothingAlpha+GLoadLat*(1-smoothingAlpha)).toFixed(2);
    PitchAngle=(OnSpeed.Pitch*smoothingAlpha+PitchAngle*(1-smoothingAlpha)).toFixed(2);
    RollAngle=(OnSpeed.Roll*smoothingAlpha+RollAngle*(1-smoothingAlpha)).toFixed(2);
    LDmax=parseFloat(OnSpeed.LDmax);
    OnspeedFast=parseFloat(OnSpeed.OnspeedFast);
    OnspeedSlow=parseFloat(OnSpeed.OnspeedSlow);
    OnspeedWarn=parseFloat(OnSpeed.OnspeedWarn);
    //FwdAccel=((OnSpeed.IAS-prevIAS)*10*smoothingAlphaFwdAcc+FwdAccel*(1-smoothingAlphaFwdAcc)).toFixed(1); // receiving 10 samples per sec
    smoothingAlphaFwdAcc=parseFloat(document.getElementById("smoothingValue").value);
    document.getElementById("currentSmoothing").innerHTML = smoothingAlphaFwdAcc.toFixed(2);
    
    if (iasArray.length<15)
        {
        iasArray.push(IAS); // add new
        FwdAccel=0; // wait until we have enough data for the derivative
        } else
              {        
              iasArray.shift(); // remove first
              iasArray.push(IAS); // add new
              var options = {windowSize: 15, derivative: 1, polynomial: 3};
              fwdAccelArray=SavitzkyGolay(iasArray, 0.1, options); //10hz time interval is .1 seconds.
              FwdAccel=fwdAccelArray.pop()*smoothingAlphaFwdAcc+FwdAccel*(1-smoothingAlphaFwdAcc); // get last element
              }
    
    //FwdAccel= document.getElementById("myRange").value;
    //if (FwdAccel<-1) decelTranslate=constrain(map(FwdAccel, -3, -1, -186, -18),-186,-18); else decelTranslate=constrain(map(FwdAccel, -1, 2, -18, 150),-18,150);
    decelTranslate= constrain(56*FwdAccel + 38,-186,150);
    // update decel needle
    document.getElementById("decelneedle").setAttribute("transform", "translate(0," + decelTranslate + ")");
    document.getElementById("currentIAS").innerHTML = IAS;
    document.getElementById("currentDecel").innerHTML=FwdAccel.toFixed(1);
    prevIAS=IAS;
    lastUpdate=Date.now();
    }
} catch (e)
           {
           console.log('JSON parsing error:'+e.name+': '+e.message);
           }
} // function   

function writeToStatus(message)
 {
   var status = document.getElementById("connectionstatus");
   status.innerHTML = message;
}
</script>   

)=====";
