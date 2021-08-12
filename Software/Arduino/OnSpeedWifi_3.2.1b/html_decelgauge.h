const char htmlDecelGauge[] PROGMEM = R"=====(
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
)=====";
