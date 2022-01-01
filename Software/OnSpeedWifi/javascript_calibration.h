const char jsCalibration[] PROGMEM = R"=====(
<script language="javascript" type="text/javascript">
 var wsUri = "ws://192.168.0.1:81";
 var lastUpdate= Date.now();
 var lastDisplay=Date.now(); 
 var OSFastMultiplier=1.35;
 var OSSlowMultiplier=1.25;
 var StallWarnMultiplier=1.10;
 var LDmaxIAS=100; // will be calculated later based on flap position
 var AOA=0;
 var IASsmoothed=0;
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
 var flightPath=0;
 var iVSI=0;
 var derivedAOA=0;
 var pitchRate=0;
 var cP=0;
 var flapsPos=0;
 var dataRecording=false;
 var flightData = new Object();
 flightData.IAS=[];
 flightData.DerivedAOA=[];
 flightData.CP=[];
 flightData.PitchRate=[];
 flightData.smoothedIAS=[];
 flightData.smoothedCP=[];
 var CPtoAOAcurve="";
 var CPtoAOAr2="";
 var LDmaxSetpoint=0;
 var OSFastSetpoint=0;                        
 var OSSlowSetpoint=0;
 var StallWarnSetpoint=0;
 var calDate;
 var stallIAS;


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
   OnSpeed.CRC= OnSpeedArray[19]; 
   

   var crc_string=OnSpeed.AOA+','+OnSpeed.Pitch+','+OnSpeed.Roll+','+OnSpeed.IAS+','+OnSpeed.PAlt+','+OnSpeed.verticalGLoad+','+OnSpeed.lateralGLoad+','+OnSpeed.alphaVA+','+OnSpeed.LDmax+','+OnSpeed.OnspeedFast+','+OnSpeed.OnspeedSlow+','+OnSpeed.OnspeedWarn+','+OnSpeed.flapsPos+','+OnSpeed.coeffP+','+OnSpeed.dataMark+','+OnSpeed.kalmanVSI+','+OnSpeed.flightPath+','+OnSpeed.PitchRate;
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
    IAS=OnSpeed.IAS;
    IASsmoothed=(OnSpeed.IAS*smoothingAlpha+IASsmoothed*(1-smoothingAlpha)).toFixed(2);

    PAlt=(OnSpeed.PAlt*smoothingAlpha+PAlt*(1-smoothingAlpha)).toFixed(2);
    GLoad=(OnSpeed.verticalGLoad*smoothingAlpha+GLoad*(1-smoothingAlpha)).toFixed(2);
    GLoadLat=(OnSpeed.lateralGLoad*smoothingAlpha+GLoadLat*(1-smoothingAlpha)).toFixed(2);
    PitchAngle=(OnSpeed.Pitch*smoothingAlpha+PitchAngle*(1-smoothingAlpha)).toFixed(2);
    RollAngle=(OnSpeed.Roll*smoothingAlpha+RollAngle*(1-smoothingAlpha)).toFixed(2);
    LDmax=parseFloat(OnSpeed.LDmax);
    OnspeedFast=parseFloat(OnSpeed.OnspeedFast);
    OnspeedSlow=parseFloat(OnSpeed.OnspeedSlow);
    OnspeedWarn=parseFloat(OnSpeed.OnspeedWarn);
    smoothingAlphaFwdAcc=parseFloat(document.getElementById("smoothingValue").value);
    document.getElementById("currentSmoothing").innerHTML = smoothingAlphaFwdAcc.toFixed(2);

    iVSI=(OnSpeed.kalmanVSI*smoothingAlpha+iVSI*(1-smoothingAlpha)).toFixed(2);
    flightPath=(OnSpeed.flightPath*smoothingAlpha+flightPath*(1-smoothingAlpha)).toFixed(2);
    derivedAOA=(PitchAngle-flightPath);
    cP=parseFloat(OnSpeed.coeffP);
    pitchRate=parseFloat(OnSpeed.PitchRate);
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
                          
    //console.log("flapIndex:",flapIndex);
    if (iasArray.length<15)
        {
        iasArray.push(IAS); // add new, use raw unsmoothed IAS.
        FwdAccel=0; // wait until we have enough data for the derivative
        } else
              {        
              iasArray.shift(); // remove first
              iasArray.push(IAS); // add new
              var options = {windowSize: 15, derivative: 1, polynomial: 2};
              fwdAccelArray=SavitzkyGolay(iasArray, 0.1, options); //10hz time interval is .1 seconds.
              FwdAccel=fwdAccelArray.pop()*smoothingAlphaFwdAcc+FwdAccel*(1-smoothingAlphaFwdAcc); // get last element
              }
    
    //FwdAccel= document.getElementById("myRange").value;
    //if (FwdAccel<-1) decelTranslate=constrain(map(FwdAccel, -3, -1, -186, -18),-186,-18); else decelTranslate=constrain(map(FwdAccel, -1, 2, -18, 150),-18,150);
    decelTranslate= constrain(56*FwdAccel + 38,-186,150);
    // update decel needle
    document.getElementById("decelneedle").setAttribute("transform", "translate(0," + decelTranslate + ")");
    document.getElementById("currentFlaps").innerHTML = flapsPos;
    document.getElementById("currentIAS").innerHTML = IASsmoothed;
    document.getElementById("currentDecel").innerHTML=FwdAccel.toFixed(1);
    prevIAS=IAS;

    if (dataRecording)
        {
        // save incoming data in arrays
        flightData.IAS.push(IAS);
        flightData.DerivedAOA.push(derivedAOA);
        flightData.CP.push(cP);
        flightData.PitchRate.push(pitchRate);
        if (pitchRate < -10) recordData(false);
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
        document.getElementById('curveResults').style="display:none";
        document.getElementById('saveCalButtons').style="display:none";        
        // init data recording arrays
         flightData.IAS=[];
         flightData.DerivedAOA=[];
         flightData.CP=[];
         flightData.PitchRate=[];
        }
        else
            {
            console.log("Recording Stop");
            document.getElementById("idStartInstructions").style.display = "block";
            document.getElementById("idStopInstructions").style.display = "none";
            // calculate smoothed IAS and CP, find max CP and min IAS.
            flightData.smoothedIAS[0]=flightData.IAS[0];
            flightData.smoothedCP[0]=flightData.CP[0];
            var stallCP=0;
            stallIAS=100;
            var stallIndex=0;
            for (i=1;i<flightData.IAS.length;i++)
                {
                //document.write(flightData.IAS[i]+', '+flightData.DerivedAOA[i]+', '+flightData.CP[i]+', '+flightData.PitchRate[i]+'<br>');
                flightData.smoothedIAS[i]=flightData.IAS[i]*.8+flightData.smoothedIAS[i-1]*.2;
                flightData.smoothedCP[i]=flightData.CP[i]*.83+flightData.smoothedCP[i-1]*.17;
                if (flightData.smoothedCP[i]>stallCP)
                    {
                    stallCP=flightData.smoothedCP[i];
                    stallIAS=flightData.smoothedIAS[i];
                    stallIndex=i;
                    }
                }
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
            if (stallCP==0) console.log("Stall not detected, try again, pitch down for stall recovery");
                  else
                      if (resultIAS.equation[0]<0)
                          {
                            //airspeed is decreasing
                            const resultCPtoAOA = regression.polynomial(dataCPtoAOA, { order: 2, precision:4 });
                            CPtoAOAcurve=resultCPtoAOA.string;
                            CPtoAOAr2=resultCPtoAOA.r2;
                            const resultIAStoAOA = regression.polynomial(dataIAStoAOA, { order: 2, precision:4 });
                            // update LDmaxIAS
                            if (flapIndex>0) LDmaxIAS=flightData.IAS[0]; // assign first seen airspeed (presumably Vfe) to LDmaxIAS when flaps are down.
                            // calculate setpoints
                            console.log("LDmaxIASsetpoint",LDmaxIAS);                            
                            LDmaxSetpoint=(resultIAStoAOA.equation[0]*LDmaxIAS*LDmaxIAS+resultIAStoAOA.equation[1]*LDmaxIAS+resultIAStoAOA.equation[2]).toFixed(2);
                            OSFastSetpoint=(resultIAStoAOA.equation[0]*(stallIAS*OSFastMultiplier)*(stallIAS*OSFastMultiplier)+resultIAStoAOA.equation[1]*(stallIAS*OSFastMultiplier)+resultIAStoAOA.equation[2]).toFixed(2);                            
                            OSSlowSetpoint=(resultIAStoAOA.equation[0]*(stallIAS*OSSlowMultiplier)*(stallIAS*OSSlowMultiplier)+resultIAStoAOA.equation[1]*(stallIAS*OSSlowMultiplier)+resultIAStoAOA.equation[2]).toFixed(2);
                            StallWarnSetpoint=(resultIAStoAOA.equation[0]*(stallIAS*StallWarnMultiplier)*(stallIAS*StallWarnMultiplier)+resultIAStoAOA.equation[1]*(stallIAS*StallWarnMultiplier)+resultIAStoAOA.equation[2]).toFixed(2);
                            console.log(resultCPtoAOA);
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
                                            //labelInterpolationFnc: function(value, index) {
                                              //return index % 20 === 0 ? value : null;
                                            //  return value;
                                            //}
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
                            document.getElementById('idCPtoAOAr2').innerHTML=CPtoAOAr2;
                            document.getElementById('CPchart').style="display:block";
                            document.getElementById('curveResults').style="display:block";
                            document.getElementById('saveCalButtons').style="display:block";  


                            new Chartist.Line('.ct-chart', chartData, options, responsiveOptions);
                          
                          } else
                                console.log("Airspeed is increasing, try again");
            } 
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
fileContent+=";CPtoAOACurve="+CPtoAOAcurve+"\n";
fileContent+=";CPtoAOAr2="+CPtoAOAr2+"\n";
fileContent+=";\n";
fileContent+=";Data:\n";
fileContent+= "IAS,CP,DerivedAOA,PitchRate\n";
for (i=0;i<=flightData.IAS.length-1 ;i++)
                {
                fileContent+=flightData.IAS[i]+","+flightData.CP[i]+","+flightData.DerivedAOA[i]+","+flightData.PitchRate[i]+"\n";
                }
var bb = new Blob([fileContent ], { type: 'application/csv' });
var a = document.createElement('a');
a.download = 'calibration-flap'+flapsPos+'_'+calDate.toISOString().substring(0, 10)+'-'+calDate.getHours()+'_'+calDate.getMinutes()+'.csv';
a.href = window.URL.createObjectURL(bb);
a.click();
}
</script>
)=====";
