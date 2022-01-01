const char htmlCalibration[] PROGMEM = R"=====(
<span id="idStartInstructions">Start out at Vmax. Hit Record when ready! 
  <button type="submit" class="button" onClick="recordData(true)">Record</button><br></span>
<span id="idStopInstructions" style="display:none;">Data Recording until stall is detected! <button type="submit" class="button" onclick="recordData(false)">Stop</button><br></span>
<div id="decelgauge" style="float:left">
<style>
body{font-family:'Open Sans',sans-serif}.graph .labels.x-labels{text-anchor:middle}.graph .labels.y-labels{text-anchor:end}.graph{height:350px;width:300px}.graph .grid{stroke:#ccc;stroke-dasharray:0;stroke-width:1}.labels{font-size:13px}.label-title{font-weight:700;font-size:12px;fill:#000}.data{fill:red;stroke-width:1}
</style>
<svg version="1.2" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="graph" aria-labelledby="title" role="img">
  <title id="title">Onspeed Deceleration Gauge</title>
<g class="grid x-grid" id="xGrid">
  <line x1="90" x2="90" y1="5" y2="371"></line>
  <line x1="250" x2="250" y1="5" y2="371"></line>
</g>
<g class="grid y-grid" id="yGrid">
  <line x1="90" x2="250" y1="370" y2="370"></line>
  <line x1="90" x2="250" y1="5" y2="5"></line>
</g> 
<g class="labels y-labels">
  <text x="80" y="11">-4</text>
  <text x="80" y="66">-3</text>
  <text x="80" y="122">-2</text>  
  <text x="80" y="178">-1</text>
  <text x="80" y="234">0</text>
  <text x="80" y="289">1</text>
  <text x="80" y="345">+2</text>
  <text x="50" y="178" class="label-title">Kts/sec</text>
</g>
<g>
<rect x="90" y="5" width="160" height="140" style="fill:rgb(255,0,0);fill-opacity:0.8;" />
<rect x="90" y="145" width="160" height="56" style="fill:rgb(0,255,0);fill-opacity:0.8;" /> 
<rect x="90" y="201" width="160" height="140" style="fill:rgb(255,0,0);fill-opacity:0.8;"/>  
</g>
<g id="decelneedle" transform="translate(0,-186)">
<rect x="100" y="189" width="145" height="5" style="fill:rgb(0,0,0);fill-opacity:1;"/>
<path transform="translate(93,191.5)" d="M1,0 20,-5 20,5 Z" stroke = "black" stroke-width = "2" fill = "black"/>   
</g>  
</svg>
</div>
<br>
<div id="values">
    <div align="left">Flap Pos: <span id="currentFlaps"></span> deg</div><br>
    <div align="left">IAS: <span id="currentIAS"></span> kts</div><br>
    <div align="left">Decel rate: <span id="currentDecel"></span> kts/sec</div>
    <br>
    <div align="left">Smoothing: <span id="currentSmoothing"></span></div>
    <div class="slidecontainer">
      <div>
      <input type="range" min=".01" max=".1" value=".04" step=".01" class="slider" id="smoothingValue" style="width:250px">
      </div>
    </div>  
    <br>
<div id="curveResults" style="display:none">    
    Stall Speed: <span id="idStallSpeed"></span> kts<br>
    CP to AOA Curve: <span id="idCPtoAOACurve"></span><br>
    R<sup>2</sup>: <span id="idCPtoAOAr2"></span><br><br>
   <b>Setpoints:</b><br>
  <div style="padding-left:20px">
    L/Dmax: <span id="idLDmaxSetpoint"></span> deg<br>
    Onspeed Fast: <span id="idOSFastSetpoint"></span> deg<br>
    Onspeed Slow: <span id="idOSSlowSetpoint"></span> deg<br>
    Stall Warning: <span id="idStallWarnSetpoint"></span> deg<br>
  </div>
</div>
    <br>
<div id="saveCalButtons" style="display:none">
<br>
<button type="submit" class="button" style="background-color:#42a7f5;" onClick="saveCalibration()">Save Calibration</button><br></span>
<button type="submit" class="button" style="background-color:#42a7f5;" onClick="saveData()">Save data to File</button><br></span>
</div>

    <div align="left">Status: <span id="connectionstatus"></span></div>
    <br><br>
 </div>
 <div id="CPchart" style="display:none">
 <div class="ct-chart" style="height:300px;display:block"> </div>
</div>
  <script>init()</script>
)=====";
