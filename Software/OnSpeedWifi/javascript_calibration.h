const char jsCalibration[] PROGMEM = R"=====(
<script language="javascript" type="text/javascript">
 var wsUri = "ws://192.168.0.1:81";
 var lastUpdate= Date.now();
 var lastDisplay=Date.now(); 
 var OSFastMultiplier=1.35;
 var OSSlowMultiplier=1.25;
 var StallWarnMargin=5; // knots
 var LDmaxIAS=100; // will be calculated later based on flap position
 var AOA=0;
 var IASsmoothed=0;
 var IAS=0;
 var prevIAS=0;
 var PAlt=0;
 var GLoad=1;
 var GLoadLat=0;
 var PitchAngle=0;
 var RollAngle=0;
 var smoothingAlpha=.9;
 var smoothingAlphaFwdAcc=.1;
 var liveConnecting=false;
 var iasArray=[];
 var flightPath=0;
 var iVSI=0;
 var derivedAOA=0;
 var pitchRate=0;
 var decelRate=0;
 var smoothDecelRate=-1.0;
 var cP=0;
 var flapsPos=0;
 var flapsPosCalibrated=0;
 var dataRecording=false;
 var flightData = new Object();
 flightData.IAS=[];
 flightData.DerivedAOA=[];
 flightData.CP=[];
 flightData.PitchRate=[];
 flightData.smoothedIAS=[];
 flightData.smoothedCP=[];
 flightData.Pitch=[];
 flightData.Flightpath=[];
 flightData.DecelRate=[];
 var CPtoAOAcurve="";
 var CPtoAOAr2="";
 var LDmaxSetpoint=0;
 var OSFastSetpoint=0;                        
 var OSSlowSetpoint=0;
 var StallWarnSetpoint=0;
 var calDate;
 var stallIAS;
 var resultCPtoAOA; // CP to AOA regression curve


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
   OnSpeed.kalmanVSI=OnSpeedArray[16];
   OnSpeed.flightPath=OnSpeedArray[17];
   OnSpeed.PitchRate= OnSpeedArray[18];
   OnSpeed.DecelRate= OnSpeedArray[19];
   OnSpeed.calSourceID= OnSpeedArray[20];
   OnSpeed.CRC= OnSpeedArray[21];
   OnSpeed.Timestamp = new Date().getTime();
   
   var crc_string=OnSpeed.AOA+','+OnSpeed.Pitch+','+OnSpeed.Roll+','+OnSpeed.IAS+','+OnSpeed.PAlt+','+OnSpeed.verticalGLoad+','+OnSpeed.lateralGLoad+','+OnSpeed.alphaVA+','+OnSpeed.LDmax+','+OnSpeed.OnspeedFast+','+OnSpeed.OnspeedSlow+','+OnSpeed.OnspeedWarn+','+OnSpeed.flapsPos+','+OnSpeed.coeffP+','+OnSpeed.dataMark+','+OnSpeed.kalmanVSI+','+OnSpeed.flightPath+','+OnSpeed.PitchRate+','+OnSpeed.DecelRate+','+OnSpeed.calSourceID;
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
    IAS=parseFloat(OnSpeed.IAS);
    IASsmoothed=(OnSpeed.IAS*smoothingAlpha+IASsmoothed*(1-smoothingAlpha)).toFixed(2);

    PAlt=(OnSpeed.PAlt*smoothingAlpha+PAlt*(1-smoothingAlpha)).toFixed(2);
    GLoad=(OnSpeed.verticalGLoad*smoothingAlpha+GLoad*(1-smoothingAlpha)).toFixed(2);
    GLoadLat=(OnSpeed.lateralGLoad*smoothingAlpha+GLoadLat*(1-smoothingAlpha)).toFixed(2);
    PitchAngle=parseFloat(OnSpeed.Pitch);
    RollAngle=parseFloat(OnSpeed.Roll);
    LDmax=parseFloat(OnSpeed.LDmax);
    OnspeedFast=parseFloat(OnSpeed.OnspeedFast);
    OnspeedSlow=parseFloat(OnSpeed.OnspeedSlow);
    OnspeedWarn=parseFloat(OnSpeed.OnspeedWarn);
    smoothingAlphaFwdAcc=parseFloat(document.getElementById("smoothingValue").value);
    document.getElementById("currentSmoothing").innerHTML = smoothingAlphaFwdAcc.toFixed(2);

    iVSI=parseFloat(OnSpeed.kalmanVSI);
    flightPath=parseFloat(OnSpeed.flightPath);
    derivedAOA=PitchAngle-flightPath;
    cP=parseFloat(OnSpeed.coeffP);
    pitchRate=parseFloat(OnSpeed.PitchRate);
    decelRate=parseFloat(OnSpeed.DecelRate);
    flapsPos=OnSpeed.flapsPos;
    flapIndex=0;
    
    for (i=0; i<flapDegrees.Count; i++)
         {
         if (flapDegrees[i]==flapsPos)
            {
            flapIndex=i;
            break;  
            }
         }
    if (flapIndex==0) {
                      LDmaxIAS=Math.sqrt(acCurrentWeight/acGrossWeight)*acVldmax;
                      //Best glide weight correction= Sqrt(current weight / gross weight) * glide speed at gross weight.                      
                      }                          
    smoothDecelRate=decelRate*smoothingAlphaFwdAcc+smoothDecelRate*(1-smoothingAlphaFwdAcc);
    //FwdAccel= document.getElementById("myRange").value;
    //if (FwdAccel<-1) decelTranslate=constrain(map(FwdAccel, -3, -1, -186, -18),-186,-18); else decelTranslate=constrain(map(FwdAccel, -1, 2, -18, 150),-18,150);
    decelTranslate= constrain(56*smoothDecelRate + 38,-186,150);
    // update decel needle
    document.getElementById("decelneedle").setAttribute("transform", "translate(0," + decelTranslate + ")");
    document.getElementById("currentFlaps").innerHTML = flapsPos;
    document.getElementById("currentIAS").innerHTML = IASsmoothed;
    document.getElementById("currentDecel").innerHTML=smoothDecelRate.toFixed(1);
    prevIAS=IAS;
        // 0= IMU, 1=VN-300, 2=AFS/SkyView, 3=Dynon D10, 4=G5, 5=G3X, 6=MGL
    switch(parseInt(OnSpeed.calSourceID)) {
    case 0:
    calSource="Internal IMU";
    break;
    case 1: calSource="VectorNav VN-200/300";
      break;
    case 2: calSource="SkyView/Advanced";
      break;
    case 3: calSource="Dynon D10/D100";
      break;
    case 4: calSource="Garmin G5";
      break;
    case 5: calSource="Garmin G3X";
      break;
    case 6: calSource="MGL iEFIS";
      break;      
    default:
      calSource='N/A';
    }
    writeToCalibrationSource(calSource); 

    if (dataRecording)
        {
        // save incoming data in arrays
        flightData.IAS.push(IAS);
        flightData.DerivedAOA.push(derivedAOA);
        flightData.CP.push(cP);
        flightData.PitchRate.push(pitchRate);
        flightData.Pitch.push(PitchAngle);
        flightData.Flightpath.push(flightPath);
        flightData.DecelRate.push(decelRate);
        flightData.Timestamp.push(OnSpeed.Timestamp);
        
        // Current trigger is 5 deg/sec in either direction
        if (Math.abs((pitchRate)) > 5) recordData(false);
        }


    lastUpdate=Date.now();
    } else console.log("CRC error");
} catch (e)
           {
           console.log('JSON parsing error:'+e.name+': '+e.message);
           }
} // function   

function onError(evt)
  {
    console.log(evt.data);    
    writeToStatus(evt.data);
    //console.error("WebSocket error observed:", evt);
  }

function writeToStatus(message)
 {
   var status = document.getElementById("connectionstatus");
   status.innerHTML = message;
}

function writeToCalibrationSource(calSource)
  {
    var status = document.getElementById("calibrationsource");
    status.innerHTML = calSource;
  }   

function recordData(on)
{
dataRecording=on;
calDate = new Date();
if (on)
        {
        console.log("Recording Start, flaps "+flapsPos);
        document.getElementById("idStartInstructions").style.display = "none";
        document.getElementById("idStopInstructions").style.display = "block";
        // hide chart and results
        document.getElementById('CPchart').style="display:none";
        document.getElementById('curveResults').style.display="none";
        document.getElementById('saveCalButtons').style.display="none";        
        // init data recording arrays
         flightData.IAS=[];
         flightData.DerivedAOA=[];
         flightData.CP=[];
         flightData.PitchRate=[];
         flightData.smoothedIAS=[];
         flightData.smoothedCP=[];
         flightData.Pitch=[];
         flightData.Flightpath=[];
         flightData.DecelRate=[];
         flightData.Timestamp=[];         
        }
        else
            {
            console.log("Recording Stop");
            document.getElementById("idStartInstructions").style.display = "block";
            document.getElementById("idStopInstructions").style.display = "none";
            // calculate smoothed IAS and CP, find max CP and min IAS.
            flapsPosCalibrated=flapsPos;
            flightData.smoothedIAS[0]=flightData.IAS[0];
            flightData.smoothedCP[0]=flightData.CP[0];
            var stallCP=0;
            stallIAS=100;
            var stallIndex=0;
            for (i=1;i<flightData.IAS.length;i++)
                {
                flightData.smoothedIAS[i]=flightData.IAS[i]*.98+flightData.smoothedIAS[i-1]*.02;
                flightData.smoothedCP[i]=flightData.CP[i]*.90+flightData.smoothedCP[i-1]*.10;
                if (flightData.smoothedCP[i]>stallCP)
                    {
                    stallCP=flightData.smoothedCP[i];
                    stallIAS=flightData.smoothedIAS[i];
                    stallIndex=i;
                    }
                }
            // calculate manuvering speed
            ManeuveringIAS=stallIAS*Math.sqrt(acGlimit);
        
            console.log('Stall_CP='+stallCP+', Stall_IAS='+stallIAS);
            // calculate polynomial regressions for CP to Derived AOA for curve, and for IAS to AOA for setpoints.
            // prepare data points for regression
            var dataCPtoAOA=[];
            var dataIAStoAOA=[];
            var dataIAS=[]; // IAS linear regression to verify that IAS is decreasing
            for (i=0;i<=stallIndex;i++)
                {
                dataCPtoAOA.push([flightData.smoothedCP[i],flightData.DerivedAOA[i]]);
                dataIAStoAOA.push([flightData.IAS[i],flightData.DerivedAOA[i]]);
                dataIAS.push([i,flightData.IAS[i]]);
                }
            const resultIAS = regression.polynomial(dataIAS, { order: 1, precision:2 });   
            if (stallCP==0)
                    {
                    alert("Stall not detected, try again, pitch down for stall recovery");
                    console.log("Stall not detected, try again, pitch down for stall recovery");
                    }                    
                  else
                      if (resultIAS.equation[0]<0)
                          {
                            //airspeed is decreasing
                            resultCPtoAOA = regression.polynomial(dataCPtoAOA, { order: 2, precision:4 });
                            //CPtoAOAcurve=resultCPtoAOA.string;
                            CPtoAOAcurve="AOA = "+resultCPtoAOA.equation[0].toFixed(4)+" * CP^2 ";
                            if (resultCPtoAOA.equation[1]>0) CPtoAOAcurve=CPtoAOAcurve+"+";
                            CPtoAOAcurve=CPtoAOAcurve+resultCPtoAOA.equation[1].toFixed(4)+" * CP ";
                            if (resultCPtoAOA.equation[2]>0) CPtoAOAcurve=CPtoAOAcurve+"+";
                            CPtoAOAcurve=CPtoAOAcurve+resultCPtoAOA.equation[2].toFixed(4);
                            CPtoAOAr2=resultCPtoAOA.r2;
                            const resultIAStoAOA = regression.polynomial(dataIAStoAOA, { order: 2, precision:4 });
                            // update LDmaxIAS
                            if (flapIndex>0) LDmaxIAS=flightData.IAS[0]; // assign first seen airspeed (presumably Vfe) to LDmaxIAS when flaps are down.
                            // calculate setpoint AOAs
                            LDmaxCP=CPfromIAS(LDmaxIAS);
                            LDmaxSetpoint=(resultCPtoAOA.equation[0]*LDmaxCP*LDmaxCP+resultCPtoAOA.equation[1]*LDmaxCP+resultCPtoAOA.equation[2]).toFixed(2);
                            OSFastCP=CPfromIAS(stallIAS*OSFastMultiplier);
                            OSFastSetpoint=(resultCPtoAOA.equation[0]*OSFastCP*OSFastCP+resultCPtoAOA.equation[1]*OSFastCP+resultCPtoAOA.equation[2]).toFixed(2);
                            OSslowCP=CPfromIAS(stallIAS*OSSlowMultiplier);                            
                            OSSlowSetpoint=(resultCPtoAOA.equation[0]*OSslowCP*OSslowCP+resultCPtoAOA.equation[1]*OSslowCP+resultCPtoAOA.equation[2]).toFixed(2);
                            StallWarnCP=CPfromIAS(stallIAS+StallWarnMargin);
                            StallWarnSetpoint=(resultCPtoAOA.equation[0]*StallWarnCP*StallWarnCP+resultCPtoAOA.equation[1]*StallWarnCP+resultCPtoAOA.equation[2]).toFixed(2);
                            ManeuveringCP=CPfromIAS(ManeuveringIAS);
                            ManeuveringSetpoint=(resultCPtoAOA.equation[0]*ManeuveringCP*ManeuveringCP+resultCPtoAOA.equation[1]*ManeuveringCP+resultCPtoAOA.equation[2]).toFixed(2);                            
                            StallSetpoint=(resultCPtoAOA.equation[0]*stallCP*stallCP+resultCPtoAOA.equation[1]*stallCP+resultCPtoAOA.equation[2]).toFixed(2);                            
                            console.log("CPtoAOA:",resultCPtoAOA);
                          // build scatterplot data

                              var chartData=new Object();
                                  chartData.series=[];
                                  chartData.series[0]=new Object();
                                  chartData.series[0].name="measuredAOA";
                                  chartData.series[0].data=[];

                                  chartData.series[1]=new Object();
                                  chartData.series[1].name="predictedAOA";
                                  chartData.series[1].data=[];
                              
                          for (i=stallIndex; i>0;i--)
                              {
                              dataPoint=new Object();
                              dataPoint.x=flightData.smoothedCP[i];
                              dataPoint.y=flightData.DerivedAOA[i];
                              chartData.series[0].data.push(dataPoint);
                              dataPoint=new Object();
                              dataPoint.x=flightData.smoothedCP[i];
                              dataPoint.y=(resultCPtoAOA.equation[0]*flightData.smoothedCP[i]*flightData.smoothedCP[i]+resultCPtoAOA.equation[1]*flightData.smoothedCP[i]+resultCPtoAOA.equation[2]).toFixed(2);
                              chartData.series[1].data.push(dataPoint);
                              } 
                          console.log(chartData);
                          // update chart
                          var options = {
                                          showLine: false,
                                          axisX: {
                                            labelInterpolationFnc: function(value, index) {
                                            return index % 100 === 0 ? value : null;
                                            //  return value;
                                            },
                                            type: Chartist.AutoScaleAxis,
                                            //onlyInteger: true,
                                          },
//                                          plugins: [
//                                                    Chartist.plugins.ctAxisTitle({
//                                                      axisX: {
//                                                        axisTitle: 'CP',
//                                                        axisClass: 'ct-axis-title',
//                                                        offset: {
//                                                          x: 0,
//                                                          y: 50
//                                                        },
//                                                        textAnchor: 'middle'
//                                                      },
//                                                      axisY: {
//                                                        axisTitle: 'AOA',
//                                                        axisClass: 'ct-axis-title',
//                                                        offset: {
//                                                          x: 0,
//                                                          y: 0
//                                                        },
//                                                        textAnchor: 'middle',
//                                                        flipTitle: false
//                                                      }
//                                                    })
//                                                  ],
                                         series:{
                                          'measuredAOA':{
                                            showPoint: true,
                                            showLine: false
                                          },
                                          'predictedAOA':{
                                            showPoint: false,
                                            showLine: true,
                                            lineSmooth: Chartist.Interpolation.simple() 
                                          } 
                                         } 
                                        };
                            var responsiveOptions = [
                                          ['screen and (min-width: 640px)', {
                                            axisX: {
                                              labelInterpolationFnc: function(value, index) {
                                                //return index % 20 === 0 ? value : null;
                                                return value;        
                                              }
                                            }
                                          }]
                                        ];
                            //show chart and results                                                      
                            document.getElementById('idStallSpeed').innerHTML=stallIAS.toFixed(2);
                            document.getElementById('idCPtoAOACurve').innerHTML=CPtoAOAcurve;                            
                            document.getElementById('idLDmaxSetpoint').innerHTML=LDmaxSetpoint;
                            document.getElementById('idOSFastSetpoint').innerHTML=OSFastSetpoint;
                            document.getElementById('idOSSlowSetpoint').innerHTML=OSSlowSetpoint;
                            document.getElementById('idStallWarnSetpoint').innerHTML=StallWarnSetpoint;
                            document.getElementById('idManeuveringSetpoint').innerHTML=ManeuveringSetpoint;
                            document.getElementById('idStallSetpoint').innerHTML=StallSetpoint;
                            document.getElementById('idCPtoAOAr2').innerHTML=CPtoAOAr2;
                            document.getElementById('CPchart').style.display="block";
                            document.getElementById('curveResults').style.display="block";
                            document.getElementById('saveCalButtons').style.display="block";  


                            new Chartist.Line('.ct-chart', chartData, options, responsiveOptions);
                          
                          } else
                                {
                                alert("Airspeed is increasing, try again");
                                console.log("Airspeed is increasing, try again");
                                }
            } 
}

function CPfromIAS(IAS)
  {
  for (i=1;i<flightData.IAS.length;i++)
                {
                if (flightData.IAS[i] <= IAS) return flightData.CP[i];
                }
  return 0;
  }

function saveData()
{  
var fileContent="";
fileContent+=";Calibration run Date/Time="+calDate+"\n";
fileContent+=";Flap position="+flapsPos+" deg\n";
fileContent+=";StallSpeed="+stallIAS.toFixed(2)+" kts\n";
fileContent+=";AOA Setpoint angles:\n";
fileContent+=";LDmaxSetpoint="+LDmaxSetpoint+"\n";
fileContent+=";OSFastSetpoint="+OSFastSetpoint+"\n";
fileContent+=";OSSlowSetpoint="+OSSlowSetpoint+"\n";
fileContent+=";StallWarnSetpoint="+StallWarnSetpoint+"\n";
fileContent+=";ManeuveringSetpoint="+ManeuveringSetpoint+"\n";
fileContent+=";StallAngle="+StallSetpoint+"\n";
fileContent+=";CPtoAOACurve: "+CPtoAOAcurve+"\n";
fileContent+=";CPtoAOAr2="+CPtoAOAr2+"\n";
fileContent+=";\n";
fileContent+=";Data:\n";
fileContent+= "Timestamp,IAS,CP,DerivedAOA,Pitch,FlightPath,DecelRate\n";
for (i=0;i<=flightData.IAS.length-1 ;i++)
                {
                fileContent+=flightData.Timestamp[i]+","+flightData.IAS[i]+","+flightData.CP[i]+","+flightData.DerivedAOA[i]+","+flightData.Pitch[i]+","+flightData.Flightpath[i]+","+flightData.DecelRate[i]+"\n";
                }
var bb = new Blob([fileContent ], { type: 'application/csv' });
var a = document.createElement('a');
a.download = 'calibration-flap'+flapsPos+'_'+calDate.toISOString().substring(0, 10)+'-'+calDate.getHours()+'_'+calDate.getMinutes()+'.csv';
a.href = window.URL.createObjectURL(bb);
a.click();
}

function saveCalibration()
{
const userAnswer = confirm("Confirm saving this calibration to system settings?");
if (!userAnswer) return;
  
params="flapsPos="+flapsPosCalibrated+"&curve0="+resultCPtoAOA.equation[0]+"&curve1="+resultCPtoAOA.equation[1]+"&curve2="+resultCPtoAOA.equation[2]+"&LDmaxSetpoint="+LDmaxSetpoint+"&OSFastSetpoint="+OSFastSetpoint+"&OSSlowSetpoint="+OSSlowSetpoint+"&StallWarnSetpoint="+StallWarnSetpoint+"&ManeuveringSetpoint="+ManeuveringSetpoint+"&StallSetpoint="+StallSetpoint;
var xhr = new XMLHttpRequest();
xhr.open("POST", "/calwiz?step=save", true);
xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
xhr.onreadystatechange = function() {//Call a function when the state changes.
    if(xhr.readyState == 4 && xhr.status == 200) {
        console.log(params);
        alert(xhr.responseText);
    }
}
xhr.send(params);
}

function saveScreenshot()
{
  const width = document.documentElement.scrollWidth;
  const height = document.documentElement.scrollHeight;

  const canvas = document.createElement("canvas");
  canvas.width = width;
  canvas.height = height;

  var context = canvas.getContext('2d');

    
domvas.toImage(document.getElementById("content"), function() {
      context.drawImage(this, 0, 0);
      const link = document.createElement("a");
      const imageURL = canvas.toDataURL("image/png"); 
      link.href = imageURL;
      const timestamp = new Date().toISOString();
      link.download = 'calibration-flap'+flapsPos+'_'+calDate.toISOString().substring(0, 10)+'-'+calDate.getHours()+'_'+calDate.getMinutes()+'.png';

      link.click();
    });

}

</script>
)=====";
