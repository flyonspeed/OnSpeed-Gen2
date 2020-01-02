const char htmlHeader[] PROGMEM = R"=====(
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
    <title>OnSpeed Gen2 WiFi gateway</title>
    <style>
body {
background-color: #cccccc;
font-family: Arial, Helvetica, Sans-Serif; Color: #000088;
}
ul {
  list-style-type: none;
  margin: 0;
  padding: 0;
  overflow: hidden;
  background-color: #333;
}
li {
   float: left;
}
li a, .dropbtn {
  display: inline-block;
  color: white;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
}
li a:hover, .dropdown:hover .dropbtn {
  background-color: red;
}
li.dropdown {
  display: inline-block;
}
.dropdown-content {
  display: none;
  position: absolute;
  background-color: #f9f9f9;
  min-width: 160px;
  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
  z-index: 1;
}
.dropdown-content a {
  color: black;
  padding: 12px 16px;
  text-decoration: none;
  display: block;
  text-align: left;
}
.dropdown-content a:hover {background-color: #f1f1f1}
.dropdown:hover .dropdown-content {
  display: block;
}
.button {
  background-color: red;
  border: none;
  color: white;
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 25px;
  cursor: pointer;
}
.inputField {
  width: 245px;
  height: 40px;
  margin: 0 .25rem;
  min-width: 125px;
  border: 1px solid #eee;
  border-radius: 5px;
  transition: border-color .5s ease-out;
  font-size: 16px;
  padding-left:10px;
}
.wifi {
  padding: 2px;
  margin-left: auto;
}
.wifi, .wifi:before {
  display: inline-block;
  border: 8px double transparent;
  border-top-color: #42a7f5;
  border-radius: 25px;
}
.wifi:before {
  content: '';
  width: 0; height: 0;
}
.offline,.offline:before{
border-top-color:#999999;
}
.header-container{
display: flex;
align-items: flex-end;  
margin: 0px;
}
.firmware{
font-size:9px;
margin-bottom: 6px;
margin-left: 10px;
}
.logo{
font-size:36px;
font-weight:bold;
font-family:helvetica;
color:black;
}
.wifibutton{
  background-color:#42a7f5;
  border:none;
  color:white;
  padding:12px 20px;
  text-align:center;
  text-decoration:none;
  display:inline-block;
  font-size:16px;
  margin:10px 25px;
  cursor:pointer;
  min-width:220px
}
.icon__signal-strength{
  display:inline-flex;
  align-items:flex-end;
  justify-content:flex-end;
  width:auto;
  height:15px;
  padding:10px;
}
.icon__signal-strength span{
  display:inline-block;
  width:4px;
  margin-left:2px;
  transform-origin:100% 100%;
  background-color:#fff;
  border-radius:2px;
  animation-iteration-count:1;
  animation-timing-function:cubic-bezier(.17,.67,.42,1.3);
  animation-fill-mode:both;
  animation-play-state:paused
}
.icon__signal-strength .bar-1{
  height:25%;
  animation-duration:0.3s;
  animation-delay:0.1s
}
.icon__signal-strength .bar-2{
  height:50%;
  animation-duration:0.25s;
  animation-delay:0.2s
}
.icon__signal-strength .bar-3{
  height:75%;
  animation-duration:0.2s;
  animation-delay:0.3s
}
.icon__signal-strength.bar-4{
  height:100%;
  animation-duration:0.15s;
  animation-delay:0.4s
}
.signal-0 .bar-1,.signal-0 .bar-2,.signal-0 .bar-3,.signal-0 .bar-4{
  opacity:.2
}
.signal-1 .bar-2,.signal-1 .bar-3,.signal-1.bar-4{
  opacity:.2
}
.signal-2 .bar-3,.signal-2 .bar-4{
  opacity:.2
}
.signal-3 .bar-4{
  opacity:.2
} 
</style>
  </head>
<body>
  <div class="header-container">
  <div class="logo"><span style="color:red">on</span>SPEED</div>
  <div class="firmware"> FW:wifi_fw</div>
  <div class="wifi wifi_status" title="wifi_network"></div>
  </div>
  <ul>
  <li><a href="/">Home</a></li>
  <li class="dropdown"> 
   <a href="javascript:void(0)" class="dropbtn">Tools</a>
   <div class="dropdown-content">
    <a href="logs">Log Files</a>
    <a href="format">Format SD Card</a>
    <a href="upgrade">Upgrade Wifi module</a>
    <a href="reboot">Reboot System</a>
  </div>    
  </li>
  <li class="dropdown"> 
   <a href="javascript:void(0)" class="dropbtn">Settings</a>
   <div class="dropdown-content">
    <a href="wifi">Wifi Settings</a>
    <a href="aoaconfig">AOA Configuration</a>
    <a href="calwiz">AOA Calibration Wizard</a>
   </div>     
  </li>
  <li><a href="live">LiveView</a></li>   
  </ul>
)=====";
