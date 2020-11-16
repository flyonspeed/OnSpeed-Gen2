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
  border-radius:3px;
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
    
    
.form-grid,
.form-option-box,
.round-box {
  display: flex;
  flex-wrap: wrap;
  flex-direction: row;
  background: #000;
  align-content: stretch;
  justify-content: flex-end;
  max-width:500px;
  align-items: center;
}

.round-box {
  background: #fff;
  padding: 15px 10px;
  -webkit-appearance: none;
  -webkit-border-radius: 3px;
  -moz-border-radius: 3px;
  -ms-border-radius: 3px;
  -o-border-radius: 3px;
  border-radius: 3px;
}




.form-divs {
  margin-bottom: 26px;
  padding:0px 5px;
  font-size:12px;
}

* {
  box-sizing: border-box;
}

.form-divs label {
  display: block;
  color: #737373;
  margin-bottom: 8px;
  line-height: 15px;
  font-size:15px;
}

.curvelabel {
margin-bottom: 0px;
}

.border,
.form-divs input,
.form-divs select,
.form-divs textarea {
  border: 1px solid #cccccc;
}

.form-divs input,
.form-divs select,
.form-divs textarea {
  font-size: 14px;
  width: 100%;
  height: 33px;
  padding: 0 8px;
  -webkit-appearance: none; 
  -webkit-border-radius: 3px;
  -moz-border-radius: 3px;
  -ms-border-radius: 3px;
  -o-border-radius: 3px;
  border-radius: 3px;
}

.form-divs select {
-webkit-appearance: menulist;
}

.form-divs.inline-formfield.top-label-gap {
  margin-top: 26px;
}

.flex-col-1 {
  width: 8.33%;
}

.flex-col-2 {
  width: 16.66%;
}

.flex-col-3 {
  width: 24.99%;
}

.flex-col-4 {
  width: 33.32%;
}

.flex-col-5 {
  width: 41.65%;
}

.flex-col-6 {
  width: 49.98%;
}

.flex-col-7 {
  width: 58.31%;
}

.flex-col-8 {
  width: 66.64%;
}

.flex-col-9 {
  width: 74.97%;
}

.flex-col-10 {
  width: 83.3%;
}

.flex-col-11 {
  width: 91.63%;
}

.flex-col-12 {
  width: 99.96%;
}

.flex-col--1 {
  width: 8.33% !important;
}

.flex-col--2 {
  width: 16.66% !important;
}

.flex-col--3 {
  width: 24.99% !important;
}

.flex-col--4 {
  width: 33.32% !important;
}

.flex-col--5 {
  width: 41.65% !important;
}

.flex-col--6 {
  width: 49.98% !important;
}

.flex-col--7 {
  width: 58.31% !important;
}

.flex-col--8 {
  width: 66.64% !important;
}

.flex-col--9 {
  width: 74.97% !important;
}

.flex-col--10 {
  width: 83.3% !important;
}

.flex-col--11 {
  width: 91.63% !important;
}

.flex-col--12 {
  width: 99.96% !important;
}

.quick-help {
  position: relative;
  display: inline-block;
  padding: 0px 8px;
}
.quick-help span {
  display: none;
  min-width: 200px;
  width: auto;
  height: auto;
  -webkit-border-radius: 3px;
  -moz-border-radius: 3px;
  -ms-border-radius: 3px;
  -o-border-radius: 3px;
  border-radius: 3px;
  background-color: #fff;
  box-shadow: 0px 0px 12px 0px rgba(0, 0, 0, 0.2);
  position: absolute;
  top: 14px;
  left: 14px;
  padding: 12px;
}
.quick-help:hover span {
  display: block;
}

.form-divs input.error-field {
  border-color: #c32929;
}

.error {
  border-bottom: solid 1px red;
  color: red;
}

@media screen and (max-width: 768) {
  .flex-col-1 {
    width: 100%;
  }

  .flex-col-2 {
    width: 100%;
  }

  .flex-col-3 {
    width: 100%;
  }

  .flex-col-4 {
    width: 100%;
  }

  .flex-col-5 {
    width: 100%;
  }

  .flex-col-6 {
    width: 100%;
  }

  .flex-col-7 {
    width: 100%;
  }

  .flex-col-8 {
    width: 100%;
  }

  .flex-col-9 {
    width: 100%;
  }

  .flex-col-10 {
    width: 100%;
  }

  .flex-col-11 {
    width: 100%;
  }

  .flex-col-12 {
    width: 100%;
  }
}

.redbutton {
  background-color: red;
  border: none;
  color: black;
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  cursor: pointer;
  border-radius:3px;
}

.greybutton {
  background-color: #ddd;
  border: none;
  color: black;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  cursor: pointer;
}

section {
  border: 1px groove threedface;
  padding: 0px;
  display: flex;
  flex-wrap: wrap;
  flex-direction: row;
  background: #f0f0f5;
  align-content: stretch;
  padding-top:10px;
  margin-top:15px;
  margin-bottom:15px;
  border-radius: 3px;
  align-items: center;  
}

section h2 {
  float: left;
  margin: -22px 0 0;
  padding-left:20px;
  padding-right:20px;
  margin-left:5px;  
  background: #fff;
  font-weight: bold;
  height:22px;
  border-radius: 3px;
  border:1px groove threedface;
  font-size: 15px;
  color: #737373;
}   

.content-container{
  display: flex;
  align-items: flex-end;  
  margin: 0px;
  background:#fff;
}
.upload-btn-wrapper {
  position: relative;
  overflow: hidden;
  display: inline-block;
}
                    
.upload-btn {
  border: none;
  color: black;
  background-color: #ddd;
  border-radius: 3px;
  font-size: 14px;
  text-align: center;
  text-decoration: none;
  height: 33px;
  width: 100%;
  padding: 0 8px;
}
                    
.upload-btn-wrapper input[type=file] {
  font-size: 100px;
  position: absolute;
  left: 0;
  top: 0;
  opacity: 0;
}
.switch-field {
  display: flex;
  /*margin-bottom: 5px;*/
  overflow: hidden;
}
.switch-field input {
  position: absolute !important;
  clip: rect(0, 0, 0, 0);
  height: 1px;
  width: 1px;
  border: 0;
  overflow: hidden;
}
.switch-field label {
  background-color: #e4e4e4;
  color: rgba(0, 0, 0, 0.6);
  font-size: 14px;
  line-height: 1;
  text-align: center;
  padding: 8px 16px;
  margin-right: -1px;
  border: 1px solid rgba(0, 0, 0, 0.2);
  box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.3), 0 1px rgba(255, 255, 255, 0.1);
  transition: all 0.1s ease-in-out;
}
.switch-field label:hover {
  cursor: pointer;
}
.switch-field input:checked + label {
  background-color: #a5dc86;
  box-shadow: none;
}
.switch-field label:first-of-type {
  border-radius: 4px 0 0 4px;
}
.switch-field label:last-of-type {
  border-radius: 0 4px 4px 0;  
}   
</style>
  </head>
<body>
  <div class="header-container">
  <div class="logo"><span style="color:red">on</span>SPEED</div>
  <div class="firmware">wifi_fw</div>
  <!-- <div class="wifi wifi_status" title="wifi_network"></div> -->
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
     <!-- <a href="wifi">Wifi Connection</a> -->
    <a href="sensorconfig">Sensor Configuration</a>
    <a href="aoaconfig">AOA Configuration</a>
    <a href="calwiz">AOA Calibration Wizard</a>
   </div>     
  </li>
  <li><a href="live">LiveView</a></li>   
  </ul>
)=====";
