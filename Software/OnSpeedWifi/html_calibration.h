const char htmlCalibration[] PROGMEM = R"=====(
<div style="float:left;margin-bottom:20px;width:350px">
  <br><b>Calibration Wizard</b><br><br>
  <div id="idStartInstructions">
    Decelerate from Vmax (or Vfe). Hit Record when ready!
    <div align="center" style="margin-top:20px">
    <button type="submit" class="wifibutton" onClick="recordData(true)">Record</button>
    </div>  
  </div>
  <div id="idStopInstructions" style="display:none;">
    Recording until stall is detected...
    <div align="center" style="margin-top:20px">
    <button type="submit" class="button" style="width:220px" onclick="recordData(false)">Stop</button>
    </div>  
  </div>
</div>
<style>
body {font-family:'Open Sans',sans-serif}.graph .labels.x-labels{text-anchor:middle}.graph .labels.y-labels{text-anchor:end}
.graph{height:350px;width:210px}.graph .grid{stroke:#ccc;stroke-dasharray:0;stroke-width:1}.labels{font-size:13px}.label-title{font-weight:700;font-size:12px;fill:#000}.data{fill:red;stroke-width:1}
</style>
<div id="maintable" style="display:table;margin-top:20px;width:350px">
      <div id="decelgauge" style="display:table-cell">   
          <svg version="1.2" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="graph" aria-labelledby="title" role="img">
            <title id="title">Onspeed Deceleration Gauge</title>
          <g class="labels y-labels">
            <text x="20" y="11">-4</text>
            <text x="20" y="66">-3</text>
            <text x="20" y="122">-2</text>  
            <text x="20" y="178">-1</text>
            <text x="20" y="234">0</text>
            <text x="20" y="289">1</text>
            <text x="20" y="345">+2</text>
            <!--<text x="50" y="178" class="label-title">Kts/sec</text>-->
          </g>
          <g>
          <rect x="30" y="5" width="160" height="140" style="fill:rgb(255,0,0);fill-opacity:0.8;" />
          <rect x="30" y="145" width="160" height="56" style="fill:rgb(0,255,0);fill-opacity:0.8;" /> 
          <rect x="30" y="201" width="160" height="140" style="fill:rgb(255,0,0);fill-opacity:0.8;"/>  
          </g>
          <g id="decelneedle" transform="translate(0,-186)">
          <rect x="40" y="189" width="145" height="5" style="fill:rgb(0,0,0);fill-opacity:1;"/>
          <path transform="translate(33,191.5)" d="M1,0 20,-5 20,5 Z" stroke = "black" stroke-width = "2" fill = "black"/>   
          </g>  
          </svg>
      </div>
      <div id="values" style="display:table-cell;vertical-align: top;margin-top:10px;width:150">
          <div align="left">Flap Pos: <span id="currentFlaps"></span> deg</div><br>
          <div align="left">IAS: <span id="currentIAS"></span> kts</div><br>
          <div align="left">DecelRate: <span id="currentDecel"></span> kts/s</div><br>
          <div align="left">Smoothing: <span id="currentSmoothing"></span></div>
        <div class="slidecontainer">
          <div>
          <input type="range" min=".02" max=".5" value=".1" step=".01" class="slider" id="smoothingValue" style="width:150px">
          </div>
        <div><div style="float:left;font-size:9px">SMOOTH</div><div style="float:right;font-size:9px">RESPONSIVE</div></div>  
        </div>
      <br>
      <br>        
          <div align="left">Status: <span id="connectionstatus"></span></div>
      </div>
 

</div>
<div id="curveResults" style="display: none;margin-left:65px;margin-top:10px">
    <b>Calibration Results:</b><br><br> 
    Stall Speed: <span id="idStallSpeed"></span> kts<br>
    CP to AOA Curve: <span id="idCPtoAOACurve"></span><br>
    R<sup>2</sup>: <span id="idCPtoAOAr2"></span><br><br>
   <b>Setpoints:</b><br>
      L/Dmax: <span id="idLDmaxSetpoint"></span> deg<br>
      Onspeed Fast: <span id="idOSFastSetpoint"></span> deg<br>
      Onspeed Slow: <span id="idOSSlowSetpoint"></span> deg<br>
      Stall Warning: <span id="idStallWarnSetpoint"></span> deg<br>
      Maneuvering Angle: <span id="idManeuveringSetpoint"></span> deg<br>
      Stall Angle: <span id="idStallSetpoint"></span> deg<br>
</div>
<div id="saveCalButtons" style="display: none;margin-left:40px">
<br>
<button type="submit" class="button" style="background-color:#42a7f5;" onClick="saveCalibration()">Save Calibration</button></span>
<button type="submit" class="button" style="background-color:#42a7f5;" onClick="saveData()">Save data to File</button><br></span>
</div>
<br>
 <div id="CPchart" style="display:none">
 <div class="ct-chart" style="height:300px;display:block"> </div>
</div>
  <script>init()</script>
)=====";
