const serialport = require('serialport')
const Readline = require('@serialport/parser-readline')
const ByteLength = require('@serialport/parser-byte-length')
const {ipcRenderer} = require("electron")
var port = new serialport('/dev/tty-usbserial1', { autoOpen: false })

var onConnectInterval;
var isConnected=false;

function refreshSerialPorts()
{
    serialport.list((err, ports) => {
        console.log('Serial ports found: ', ports);
        if (err) {
          document.getElementById('serialDetectError').textContent = err.message
          return
        } else {
          document.getElementById('serialDetectError').textContent = ''
        }
      
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
            newOption.value = ports[i].comName;
            newOption.innerHTML = ports[i].comName;
            select.add(newOption);
        }
        var button = document.getElementById("btnConnect")
        if(ports.length > 0) 
        {
            select.selectedIndex = 0;
            button.disabled = false;
        }
        else { button.disabled = true; }
      
      })
}


function openSerialPort()
{
    var e = document.getElementById('portsSelect');
    
    console.log("Opening serial port: ", e.options[e.selectedIndex].value);
    port = new serialport(e.options[e.selectedIndex].value, { baudRate: 115200 }, function (err) {
        if (err) {
          return console.log('Error: ', err.message)
        }
      })

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

function refreshAvailableFirmwares()
{
    //Disable the buttons. These are only re-enabled if the retrieve is successful
    var DetailsButton = document.getElementById("btnDetails");
    var ChoosePortButton = document.getElementById("btnChoosePort");
    DetailsButton.disabled = true;
    ChoosePortButton.disabled = true;

    var request = require('request');
    request.get('http://speeduino.com/fw/versions', {timeout: 10000}, function (error, response, body) 
    {
        select = document.getElementById('versionsSelect');
        if (!error && response.statusCode == 200) {

            var lines = body.split('\n');
            // Continue with your processing here.
            
            for(var i = 0;i < lines.length;i++)
            {
                var newOption = document.createElement('option');
                newOption.value = lines[i];
                newOption.innerHTML = lines[i];
                select.appendChild(newOption);
            }
            select.selectedIndex = 0;

            //Re-enable the buttons
            DetailsButton.disabled = false;
            ChoosePortButton.disabled = false;
        }
        else if(error)
        {
            console.log("Error retrieving available firmwares");
            var newOption = document.createElement('option');
            if(error.code === 'ETIMEDOUT')
            {
                newOption.value = "Connection timed out";
                newOption.innerHTML = "Connection timed out";
            }
            else
            {
                newOption.value = "Cannot retrieve firmware list";
                newOption.innerHTML = "Cannot retrieve firmware list. Check internet connection and restart";
            }
            select.appendChild(newOption);
        }
        else if(response.statusCode == 404)
        {

        }
    }
    );
}

function downloadHex()
{

    var e = document.getElementById('versionsSelect');
    var DLurl = "http://speeduino.com/fw/bin/" + e.options[e.selectedIndex].value + ".hex";
    console.log("Downloading: " + DLurl);
    
    //Download the Hex file
    ipcRenderer.send("download", {
        url: DLurl,
        properties: {directory: "downloads"}
    });

}

function downloadIni()
{

    var e = document.getElementById('versionsSelect');
    var DLurl = "http://speeduino.com/fw/" + e.options[e.selectedIndex].value + ".ini";
    console.log("Downloading: " + DLurl);

    //Download the ini file
    ipcRenderer.send("download", {
        url: DLurl,
        properties: {directory: "downloads"}
    });

}

function uploadFW()
{
    //Jump to the progress section
    window.location.href = "#progress";

    //Start the spinner
    var spinner = document.getElementById('progressSpinner');
    //Remove any old icons
    spinner.classList.remove('fa-pause');
    spinner.classList.remove('fa-check');
    spinner.classList.remove('fa-times');
    spinner.classList.add('fa-spinner');

    var statusText = document.getElementById('statusText');
    var burnPercentText = document.getElementById('burnPercent');
    statusText.innerHTML = "Downloading INI file"
    downloadIni();


    ipcRenderer.on("download complete", (event, file, state) => {
        console.log("Saved file: " + file); // Full file path

        var extension = file.substr(file.length - 3);
        if(extension == "ini")
        {
            statusText.innerHTML = "Downloading firmware"
            downloadHex();
        }
        else if(extension == "hex")
        {
            statusText.innerHTML = "Uploading firmware to board"

            //Retrieve the select serial port
            var e = document.getElementById('portsSelect');
            uploadPort = e.options[e.selectedIndex].value;
            console.log("Using port: " + uploadPort);

            //Begin the upload
            ipcRenderer.send("uploadFW", {
                port: uploadPort,
                firmwareFile: file
            });
        }
        console.log();
    });

    ipcRenderer.on("upload completed", (event, code) => {
        statusText.innerHTML = "Upload to arduino completed successfully!";
        burnPercentText.innerHTML = "";
        spinner.classList.remove('fa-spinner');
        spinner.classList.add('fa-check');
    });

    ipcRenderer.on("upload percent", (event, percent) => {
        burnPercentText.innerHTML = " (" + percent + "%)";
    });

    ipcRenderer.on("upload error", (event, code) => {
        statusText.innerHTML = "Upload to arduino failed";
        //Mke the terminal/error section visible
        document.getElementById('terminalSection').style.display = "block";
        document.getElementById('terminalText').innerHTML = code;
        spinner.classList.remove('fa-spinner');
        spinner.classList.add('fa-times');
    });


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

  if(patternOptionCounter == numPatterns) { port.unpipe(); }
}

function readPattern()
{
  //Read the 0/1/2/3 sequence for the current pattern from the arduino

  
}

function updatePattern()
{
  var patternID = document.getElementById('patternSelect').value;

  const parser = port.pipe(new Readline({ delimiter: '\r\n' }));
  console.log(`Sending 'S' command with pattern ${patternID}`);
  //const parser = port.pipe(new ByteLength({length: 8}))
  //port.write('S'); //Send the command to change the pattern

  var buffer = new Buffer(2);
  buffer[0] = 0x53; // Ascii 'S'
  buffer[1] = parseInt(patternID);
  port.write(buffer); //Send the new pattern ID

  //Request the new pattern
  port.write("P"); //Send the command to read the new pattern out
  parser.on('data', refreshPattern);
  
}

//Callback for the P command
function refreshPattern(data)
{
  console.log(`Received pattern: ${data}`);
  var newPattern = data.split(",");
  redrawGears(newPattern);

  port.unpipe();
}

function updateRPM()
{
  //Send a new fixed RPM value back to the arduino
}

function updateSweepRPM()
{
  //Send new sweep RPM values back to the arduino
}

function setRPMMode()
{
  //Change between pot, fixed and sweep RPM modes
}

function redrawGears(pattern)
{
  var teeth, depth, radius, width;
  //teeth =  toothPattern.length / 2;
  depth = 10;
  radius = 150;
  width = Number("100");
  line = 1;
  var halfspeed = true;
  //var halfspeed = false;

  draw_crank(pattern, depth, radius, width, line, halfspeed);
  draw_cam(pattern, depth, radius, width, line);
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


function onRefresh(chart) 
{
    if(port.isOpen == false) { return; }

    port.write("G1"); //Sends the command to get live data

}

window.onload = function () 
{
    refreshSerialPorts();
    redrawGears(toothPatterns[0]);
    //animateGauges();
};

