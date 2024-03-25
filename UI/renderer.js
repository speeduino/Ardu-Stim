const serialport = require('serialport')
const usb = require('usb').usb;
const Readline = require('@serialport/parser-readline')
const ByteLength = require('@serialport/parser-byte-length')
const {ipcRenderer} = require("electron")
var port = new serialport('/dev/tty-usbserial1', { autoOpen: false })

var onConnectInterval;
var isConnected=false;
var currentRPM = 0;
var initComplete = false;

function refreshSerialPorts()
{
    serialport.list().then((ports) => {
        console.log('Serial ports found: ', ports);
      
        if (ports.length === 0) {
          document.getElementById('serialDetectError').textContent = 'No ports discovered'
        }
      
        select = document.getElementById('portsSelect');

        //Clear the current options
        for (i = 0; i <= select.options.length; i++) 
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
              //Arduino device
              if(ports[i].productId == "0010" || ports[i].productId == "0042") 
              { 
                //Mega2560
                newOption.innerHTML = newOption.innerHTML + " (Arduino Mega)"; 
                isMega = true;
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
  onConnectInterval = setInterval(requestPatternList, 3000);

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
        burnPercentText.innerHTML = " (" + percent + "%)";
    });

    ipcRenderer.on("upload error", (event, code) => {
        burnPercentText.innerHTML = "Upload to arduino failed";
        //Mke the terminal/error section visible
        spinner.classList.remove('fa-spinner');
        spinner.classList.add('fa-times');
    });


}

function saveData()
{
  //Request the arduino save the current config
  port.write("c"); //Send the command to perform EEPROM burn
}

var patternOptionCounter = 0;
var numPatterns = 0;
function requestPatternList()
{
  //Clear the interval
  clearInterval(onConnectInterval);

  //Attach the readline parser
  const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

  //Clear the existing list
  var select = document.getElementById('patternSelect')
  for (i = 0; i <= select.options.length; i++) 
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

    //Request the currently selected patter
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
  select.value = patternID;
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
  saveData();

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

  const parser = port.pipe(new Readline({ delimiter: '\r\n' }));
  console.log(`Sending 'M' command to change RPM mode to ${newMode}`);

  var buffer = Buffer.alloc(2);
  buffer[0] = 0x4D; // Ascii 'M'
  buffer[1] = newMode;
  port.write(buffer); //Send the new pattern ID

  //If the new mode is fixed RPM or linear sweep, then send the RPM set values for them
  if(newMode == 0)
  {
    //Sweep RPM mode
    setSweepRPM();

    //Update the text box enablement
    document.getElementById("rpmSweepMin").disabled = false;
    document.getElementById("rpmSweepMax").disabled = false;
    document.getElementById("fixedRPM").disabled = true;
  }
  else if (newMode == 1)
  {
    //Fixed RPM mode
    setFixedRPM();
    
    //Update the text box enablement
    document.getElementById("fixedRPM").disabled = false;
    document.getElementById("rpmSweepMin").disabled = true;
    document.getElementById("rpmSweepMax").disabled = true;
  }
  else if(newMode == 2)
  {
    //Pot mode

    //Update the text box enablement
    document.getElementById("rpmSweepMin").disabled = true;
    document.getElementById("rpmSweepMax").disabled = true;
    document.getElementById("fixedRPM").disabled = true;
  }
    
  
}

function setFixedRPM()
{
  var newRPM = parseInt(document.getElementById('fixedRPM').value);
  //console.log(`Desired RPM: ${newRPM}`);

  var rpmBuffer = Buffer.alloc(3);
  rpmBuffer[0] = 0x66; // Ascii 'f'
  rpmBuffer.writeInt16LE(newRPM, 1);
  //console.log(rpmBuffer);

  port.write(rpmBuffer);
}

function setSweepRPM()
{
  var newRPM_min = parseInt(document.getElementById('rpmSweepMin').value);
  var newRPM_max = parseInt(document.getElementById('rpmSweepMax').value);
  //console.log(`Desired RPM: ${newRPM}`);

  var rpmBuffer = Buffer.alloc(5);
  rpmBuffer[0] = 0x73; // Ascii 's'
  rpmBuffer.writeInt16LE(newRPM_min, 1);
  rpmBuffer.writeInt16LE(newRPM_max, 3);
  //console.log(rpmBuffer);

  port.write(rpmBuffer);
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
  //console.log(`New RPM: ${currentRPM}`);
}

function updateRPM()
{
  console.log("Requesting new RPM");
  port.write("R"); //Request next RPM read
  document.gauges[0].value = currentRPM;
  //console.log(`New gauge RPM: ${document.gauges[0].value}`);
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

