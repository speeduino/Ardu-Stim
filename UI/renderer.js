const serialport = require('serialport')
const usb = require('usb').usb;
const Readline = require('@serialport/parser-readline')
const ByteLengthParser = require('@serialport/parser-byte-length')
const InterByteTimeoutParser = require('@serialport/parser-inter-byte-timeout')
const {ipcRenderer} = require("electron")
var port = new serialport('/dev/tty-usbserial1', { autoOpen: false })

const CONFIG_SIZE = 18;
const FW_VERSION = 2;
var onConnectIntervalConfig;
var onConnectIntervalWheels;
var isConnected=false;
var currentRPM = 0;
var rpmRequestPending = false;
var initComplete = false;

function refreshSerialPorts()
{
    serialport.list().then((ports) => {
        console.log('Serial ports found: ', ports);
      
        if (ports.length === 0) { document.getElementById('serialDetectError').textContent = 'No ports discovered'; }
        else { document.getElementById('serialDetectError').textContent = ''; }
      
        select = document.getElementById('portsSelect');

        //Clear the current options
        while (select.options.length > 0) 
        {
            select.remove(0); //Always 0 index (As each time an item is removed, everything shuffles up 1 place)
        }

        //Load the current serial values
        for(var i = 0; i < ports.length; i++)
        {
            var newOption = document.createElement('option');
            newOption.value = ports[i].path;
            newOption.innerHTML = ports[i].path;
            if(ports[i].vendorId == "2341")
            {
              //Arduino Mega device
              if(ports[i].productId == "0010" || ports[i].productId == "0042") 
              { 
                //Mega2560
                newOption.innerHTML = newOption.innerHTML + " (Arduino Mega)"; 
              }
            }
            else if(ports[i].vendorId == "16c0")
            {
              //Teensy
              if(ports[i].productId == "0483")
              {
                //Teensy - Unfortunately all Teensy devices use the same device ID :(
                newOption.innerHTML = newOption.innerHTML + " (Teensy)"; 
              } 
            }
            else if(ports[i].vendorId == "16c0")
            {
            }
            else if(ports[i].vendorId == "1a86")
            {
              //Arduino Nano device
              if(ports[i].productId == "7523") 
              { 
                //Nano
                newOption.innerHTML = newOption.innerHTML + " (Arduino Nano)"; 
              }
            }
            
            select.add(newOption);
            console.log("Vendor: " + ports[i].vendorId);
            console.log("Product: " +ports[i].productId);
        var button = document.getElementById("btnConnect")
        if(ports.length > 0) 
        {
            select.selectedIndex = 0;
            button.disabled = false;
        }
        else { button.disabled = true; }
      }
    })
}


function openSerialPort()
{
    var e = document.getElementById('portsSelect');
    
    console.log("Opening serial port: ", e.options[e.selectedIndex].value);
    port = new serialport(e.options[e.selectedIndex].value, { baudRate: 115200 }, function (err) {
        if (err) {
          window.alert(`Error while opening serial port: ${err.message}`);
          throw err;
        }

        //Drop the modal dialog until connection is complete
        modalLoading.init(true);
        initComplete = false;
      });

    //Update the patterns downdown list
    port.on('open', onSerialConnect);
    //port.on('data', onData);
    //refreshPatternList();

    // Master listener for all serial actions
    // Switches the port into "flowing mode"
    /*
    port.on('data', function (data) 
    {
        //console.log('Data:', data)

        if(data.length < 2) { return; }
        var knockValue = data[0];
        var threshold = data[1];

        liveChart.config.data.datasets[0].data.push({
            x: Date.now(),
            y: threshold
            });
        liveChart.config.data.datasets[1].data.push({
            x: Date.now(),
            y: knockValue
            });
    })
    */

}

function onSerialConnect()
{
  console.log("Serial port opened");
  onConnectIntervalConfig = setInterval(requestConfig, 2000);
  //onConnectIntervalWheels = setInterval(requestPatternList, 3000);

  //Activate the links
  document.getElementById("link_live").href = "#live";
  document.getElementById("link_config").href = "#config";
}


function uploadFW()
{

    //Set the status and spinner
    var spinner = document.getElementById('progressSpinner');
    var burnPercentText = document.getElementById('burnPercent');
    burnPercentText.innerHTML = "Preparing to burn firmware...";

    //Remove any old icons
    spinner.classList.remove('fa-pause');
    spinner.classList.remove('fa-check');
    spinner.classList.remove('fa-times');
    spinner.classList.add('fa-spinner');

    //Retrieve the select serial port
    var e = document.getElementById('portsSelect');
    uploadPort = e.options[e.selectedIndex].value;
    console.log("Uploading to port: " + uploadPort);

    //Retrieve the 

    //Begin the upload
    ipcRenderer.send("uploadFW", {
      port: uploadPort,
    });

    ipcRenderer.on("upload completed", (event, code) => {
        burnPercent.innerHTML = "Upload to arduino completed successfully!";
        spinner.classList.remove('fa-spinner');
        spinner.classList.add('fa-check');
    });

    ipcRenderer.on("upload percent", (event, percent) => {
        burnPercentText.innerHTML = "Uploading (" + percent + "% Complete)";
    });

    ipcRenderer.on("old bootloader", (event) => {
      burnPercentText.innerHTML = "No new Nano found. Retrying for old bootloader.";
    });

    ipcRenderer.on("upload error", (event, code) => {
        burnPercentText.innerHTML = "Upload to arduino failed";
        //Mke the terminal/error section visible
        spinner.classList.remove('fa-spinner');
        spinner.classList.add('fa-times');
    });


}

function saveData(showCheck)
{
  //Request the arduino save the current config
  port.write("s"); //Send the command to perform EEPROM burn
  console.log("Sending 's' command to save config ")

  //Check if we redo the checkmark animation
  if(showCheck)
  {
    var checkmark = document.getElementById("saveCheck");
    checkmark.style.animation = 'none';
    checkmark.offsetHeight; /* trigger reflow */
    checkmark.style.opacity = 1; 
    checkmark.style.visibility  = "visible";
    checkmark.style.animation = null; 
  }
}

function requestConfig()
{
  //Clear the interval
  clearInterval(onConnectIntervalConfig);

  //Attach the readline parser

  //Attach the version check parser
  parser = port.pipe(new InterByteTimeoutParser({ maxBufferSize: CONFIG_SIZE, interval: 1500 }));
  parser.on('data', receiveConfig);


  //Request the config from the arduino
  port.write("C");
  console.log("Requesting config");
}

function receiveConfig(data)
{
  console.log("Received config: " + data);
  console.log("Mode: " + data[2]);

  if(data.length == 0) 
  { 
    console.log("TIMEOUT: No config data received");
    alert("Timeout connecting to arduino. Try uploading firmware again.");
    modalLoading.remove();
  }
  if(data.length != CONFIG_SIZE) { console.log("Incorrect amount of config data received"); }

  document.getElementById("patternSelect").value = data[1];
  document.getElementById("rpmSelect").value = data[2];
  document.getElementById("fixedRPM").value = (((data[4] & 0xff) << 8) | (data[3] & 0xff));
  document.getElementById("rpmSweepMin").value = (((data[6] & 0xff) << 8) | (data[5] & 0xff));
  document.getElementById("rpmSweepMax").value = (((data[8] & 0xff) << 8) | (data[7] & 0xff));
  document.getElementById("rpmSweepSpeed").value = (((data[10] & 0xff) << 8) | (data[9] & 0xff));
  document.getElementById("compressionEnable").checked = data[11];
  document.getElementById("compressionMode").value = data[12];
  document.getElementById("compressionRPM").value = (((data[14] & 0xff) << 8) | (data[13] & 0xff));
  document.getElementById("compressionOffset").value = (((data[16] & 0xff) << 8) | (data[15] & 0xff));
  document.getElementById("compressionDynamic").checked = data[17];
  
  port.unpipe();

  if(data[0] == FW_VERSION)
  {
    setRPMMode();
    requestPatternList();

    //Enable or disabled the compression settings
    var compressionState = document.getElementById('compressionEnable').checked
    document.getElementById('compressionDynamic').disabled = !compressionState
    document.getElementById('compressionMode').disabled = !compressionState
    document.getElementById('compressionRPM').disabled = !compressionState
    document.getElementById('compressionOffset').disabled = !compressionState
  }
  else
  {
    console.log("Firmware version mismatch. Expected: ", FW_VERSION, " Received: ", data[0]);
    alert("Firmware version mismatch. Please press the 'Upload Firmware' button to update the firmware.");
    //Drop the modal loading window
    modalLoading.remove();
    window.location.hash = '#connect';
    initComplete = false;
  }
}

function sendConfig()
{
  var newRPM = parseInt(document.getElementById('fixedRPM').value);
  //console.log(`Desired RPM: ${newRPM}`);

  var configBuffer = Buffer.alloc(CONFIG_SIZE); // +1 is for the 'c' command character
  configBuffer[0] = 0x63; // 'c' character command
  configBuffer[1] = parseInt(document.getElementById('patternSelect').value);
  configBuffer[2] = parseInt(document.getElementById('rpmSelect').value);
  configBuffer.writeUInt16LE(parseInt(document.getElementById('fixedRPM').value), 3);
  configBuffer.writeUInt16LE(parseInt(document.getElementById('rpmSweepMin').value), 5);
  configBuffer.writeUInt16LE(parseInt(document.getElementById('rpmSweepMax').value), 7);
  configBuffer.writeUInt16LE(parseInt(document.getElementById('rpmSweepSpeed').value), 9);
  configBuffer[11] = document.getElementById('compressionEnable').checked;
  configBuffer[12] = parseInt(document.getElementById('compressionMode').value);
  configBuffer.writeUInt16LE(parseInt(document.getElementById('compressionRPM').value), 13);
  configBuffer.writeUInt16LE(parseInt(document.getElementById('compressionOffset').value), 15);
  configBuffer[16] = document.getElementById('compressionDynamic').checked;

  console.log("Sending full config: ", configBuffer);

  port.write(configBuffer);
}

var patternOptionCounter = 0;
var numPatterns = 0;
function requestPatternList()
{
  //Clear the interval
  clearInterval(onConnectIntervalWheels);

  //Attach the readline parser
  const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

  //Clear the existing list
  var select = document.getElementById('patternSelect')
  while(select.options.length > 0)
  {
      select.remove(0); //Always 0 index (As each time an item is removed, everything shuffles up 1 place)
  }
  patternOptionCounter = 0;
  numPatterns = 0;

  //Request the number of wheels
  console.log("Requesting number of wheels");
  port.write("n");

  //Read the available patterns from the arduino
  console.log("Sending 'L' command");
  //const parser = port.pipe(new ByteLength({length: 8}))
  port.write("L"); //Send the command to issue the pattern name list
  parser.on('data', refreshPatternList);
  //port.on('data', refreshPatternList);
  

}

//Called back after the 'L' command has been received
function refreshPatternList(data) 
{
  //If this is the first line received, the number is the total number of wheels avaialable
  if(numPatterns == 0)
  {
    numPatterns = parseInt(data);
    console.log(`Number of wheels: ${numPatterns}`);
    return;
  }

  console.log(`Adding option #${patternOptionCounter}:\t${data}`)
  var select = document.getElementById('patternSelect')
  var option = document.createElement("option");
  option.text = data;
  option.value = patternOptionCounter;
  
  //Add new item
  select.add(option);

  patternOptionCounter++;

  if(patternOptionCounter == numPatterns)
  { 
    port.unpipe(); 

    //Request the currently selected pattern
    port.write("N"); //Send the command to issue the current pattern number
    const parser = port.pipe(new Readline({ delimiter: '\r\n' })); //Attach the readline parser
    parser.on('data', refreshPatternNumber);
  }
}

//Callback from the 'N' command that returns the number of the selected pattern
function refreshPatternNumber(data)
{
  var select = document.getElementById('patternSelect')
  var patternID = parseInt(data);
  port.unpipe();

  //Temporarily disable the onchange event while we set the initial value
  var changeFunction = select.onchange;
  select.value = patternID;
  select.onchange = changeFunction;

  console.log("Currently selected Pattern: " + patternID);
  updatePatternQueue();
}

function readPattern()
{
  //Read the 0/1/2/3 sequence for the current pattern from the arduino

  
}

var patternRow = 0;
var newPattern;
var patternDegrees;

var nextPatternID = null;
var currentPatternID = null;
function updatePatternQueue()
{
  nextPatternID = document.getElementById('patternSelect').value;

  if (currentPatternID === null) {
    updatePattern();
  }
}

function updatePattern()
{
  currentPatternID = nextPatternID;
  nextPatternID = null;

  const parser = port.pipe(new Readline({ delimiter: '\r\n' }));
  console.log(`Sending 'S' command with pattern ${currentPatternID}`);

  var buffer = Buffer.alloc(2);
  buffer[0] = 0x53; // Ascii 'S'
  buffer[1] = parseInt(currentPatternID);
  port.write(buffer); //Send the new pattern ID

  //Send the command to save the pattern to EEPROM
  saveData(false);

  //Request the new pattern
  port.write("P"); //Send the command to read the new pattern out
  parser.on('data', refreshPattern);
}

//Callback for the P command
function refreshPattern(data)
{
  
  if(patternRow == 0)
  {
    //First line sent is the pattern itself
    console.log(`Received pattern: ${data}`);
    newPattern = data.split(",");
    patternRow++;
  }
  else
  {
    //2nd line received is the number of degrees the pattern runs over (360 or 720 usually)
    console.log(`Pattern duration: ${data}`);
    patternDegrees = parseInt(data);
    redrawGears(newPattern, patternDegrees);
    
    patternRow = 0;
    port.unpipe();

    if(initComplete == false)
    {
      //Drop the modal loading window
      modalLoading.remove();
      //Move to the Live tab
      window.location.hash = '#live';
      initComplete = true;
    }

    if (nextPatternID !== null) {
      updatePattern();
    }
    else {
      currentPatternID = null;
    }

  }

}

//Simply redraw the gear pattern using the existing details (Used when the draw style is changed)
function resetGears()
{
  redrawGears(newPattern, patternDegrees);
}

function setRPMMode()
{
  //Change between pot, fixed and sweep RPM modes

  
  var newMode = parseInt(document.getElementById('rpmSelect').value);

  //If the new mode is fixed RPM or linear sweep, then send the RPM set values for them
  if(newMode == 0)
  {
    //Update the text box enablement
    document.getElementById("rpmSweepMin").disabled = false;
    document.getElementById("rpmSweepMax").disabled = false;
    document.getElementById("rpmSweepSpeed").disabled = false;
    document.getElementById("fixedRPM").disabled = true;
  }
  else if (newMode == 1)
  {
    //Update the text box enablement
    document.getElementById("fixedRPM").disabled = false;
    document.getElementById("rpmSweepMin").disabled = true;
    document.getElementById("rpmSweepMax").disabled = true;
    document.getElementById("rpmSweepSpeed").disabled = true;
  }
  else if(newMode == 2)
  {
    //Pot mode

    //Update the text box enablement
    document.getElementById("rpmSweepMin").disabled = true;
    document.getElementById("rpmSweepMax").disabled = true;
    document.getElementById("rpmSweepSpeed").disabled = true;
    document.getElementById("fixedRPM").disabled = true;
  }
  
  if(initComplete) { sendConfig(); }
  
}

function redrawGears(pattern, degrees)
{
  var teeth, depth, radius, width;
  //teeth =  toothPattern.length / 2;

  radius = 150;
  width = Number("100");

  var halfspeed = false;
  if(degrees == 720) { halfspeed = true; }
  //var halfspeed = false;

  var background = document.getElementById('canvas-background-colour');
  var style = document.getElementById('wheelDisplaySelect').value
  var crank = document.getElementById('crank');
  var cam = document.getElementById('cam');
  //style = 1;
  if(style == 0)
  {
    crank.width = 420;
    crank.height = 300;
    cam.width = 420;
    cam.height = 300;
    depth = 10;
    line = 1;
    background.style.backgroundColor = "#0071b8";
    draw_crank_gear(pattern, depth, radius, width, line, halfspeed);
    draw_cam_gear(pattern, depth, radius, width, line);
  }
  else
  {
    crank.width = 840;
    crank.height = 150;
    cam.width = 840;
    cam.height = 150;
    depth = 80;
    line = 3;
    background.style.backgroundColor = "#000000";
    draw_crank_scope(pattern, depth, radius, width, line, halfspeed);
    draw_cam_scope(pattern, depth, radius, width, line);
  }

  
}

/*
var timers = [];
function animateGauges() {
  document.gauges.forEach(function(gauge) {
      timers.push(setInterval(function() {
          gauge.value = Math.random() *
              (gauge.options.maxValue - gauge.options.minValue) / 4 +
              gauge.options.minValue / 4;
      }, gauge.animation.duration + 50));
  });
}
*/

var RPMInterval = 0;
function enableRPM()
{
  console.log("Enabling RPM reads");
  if(RPMInterval == 0)
  {
    RPMInterval = setInterval(updateRPM, 100);
    const parser = port.pipe(new Readline({ delimiter: '\r\n' }));
    parser.on('data', receiveRPM);
    rpmRequestPending = false;
  }
  
}

function disableRPM()
{
  console.log("Deactivating RPM reads");
  clearInterval(RPMInterval);
  RPMInterval = 0;
  port.unpipe();
  port.read(); //Flush the port
}

function receiveRPM(data)
{
  console.log(`Received RPM: ${data}`);
  currentRPM = parseInt(data);
  rpmRequestPending = false;
  //console.log(`New RPM: ${currentRPM}`);
}

function toggleCompression()
{
  var state = document.getElementById('compressionEnable').checked

  document.getElementById('compressionDynamic').disabled = !state
  document.getElementById('compressionMode').disabled = !state
  document.getElementById('compressionRPM').disabled = !state
  document.getElementById('compressionOffset').disabled = !state

  sendConfig();
}

function updateRPM()
{
  if(rpmRequestPending == false)
  {
    console.log("Requesting new RPM");
    port.write("R"); //Request next RPM read
    document.gauges[0].value = currentRPM;
    rpmRequestPending = true;
    //console.log(`New gauge RPM: ${document.gauges[0].value}`);
  }
}

async function checkForUpdates()
{
    let current_version = await ipcRenderer.invoke("getAppVersion");
    document.getElementById('versionSpan').innerHTML = current_version;

    var url = "https://api.github.com/repos/speeduino/Ardu-Stim/releases/latest";

    //document.getElementById('detailsHeading').innerHTML = version;
    
    fetch(url)
      .then(function (response) {
        if (response.ok) {
            return response.json();
        }
        return Promise.reject(response);
      })
      .then(function (json) {
        latest_version = json.tag_name.substring(0);

        var semver = require('semver');
        if(semver.gt(latest_version, current_version))
        {
            //New version has been found
            document.getElementById('update_url').setAttribute("href", json.html_url);
            document.getElementById('update_text').style.display = "block";
        }
      })
      .catch(function (err) {
        console.log("Error checking for updates.", err);
      });

}

function liveShowHide(mutationsList, observer) {
  mutationsList.forEach(mutation => {
    if (mutation.attributeName === 'style') {
      if (mutation.target.style.display === 'none') {
        disableRPM();
      }
      else {
        enableRPM();
      }
    }
  })
}

window.onload = function () 
{
    refreshSerialPorts();
    redrawGears(toothPatterns[0]);
    window.location.hash = '#connect';
    //window.location.hash = '#live';
    checkForUpdates();
    //animateGauges();

    //Enable and disabled retrieval of RPM when viewing live panel
    const liveShowHideObserver = new MutationObserver(liveShowHide);
    liveShowHideObserver.observe(
      document.getElementById('live'),
      { attributes: true }
    );

    usb.on('attach', refreshSerialPorts);
    usb.on('detach', refreshSerialPorts);
};

