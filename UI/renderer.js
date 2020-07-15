const serialport = require('serialport')
const {ipcRenderer} = require("electron")
var port = new serialport('/dev/tty-usbserial1', { autoOpen: false })

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
    

    port = new serialport(e.options[e.selectedIndex].value, { baudRate: 9600 }, function (err) {
        if (err) {
          return console.log('Error: ', err.message)
        }
      })

    
    // Master listener for all serial actions
    // Switches the port into "flowing mode"
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

    //Start the live chart
    liveChartConfig.options.scales.xAxes[0].realtime.pause = false;
	window.liveChart.update({duration: 0});
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


function onRefresh(chart) {
    if(port.isOpen == false) { return; }

    port.write("G1"); //Sends the command to get live data

}

var chartColors = {
	red: 'rgb(255, 99, 132)',
	orange: 'rgb(255, 159, 64)',
	yellow: 'rgb(255, 205, 86)',
	green: 'rgb(75, 192, 192)',
	blue: 'rgb(54, 162, 235)',
	purple: 'rgb(153, 102, 255)',
	grey: 'rgb(201, 203, 207)'
}; 
var color = Chart.helpers.color;

var liveChartConfig = {
	type: 'line',
	data: {
		datasets: [{
			label: 'Knock threshold',
			backgroundColor: color(chartColors.red).alpha(0.5).rgbString(),
			borderColor: chartColors.red,
			fill: false,
			lineTension: 0,
			borderDash: [8, 4],
            data: [],
            pointRadius: 0
		}, {
			label: 'Knock reading',
			backgroundColor: color(chartColors.blue).alpha(0.5).rgbString(),
			borderColor: chartColors.green,
			fill: false,
			cubicInterpolationMode: 'monotone',
            data: [],
            pointRadius: 0
		}]
	},
	options: {
		title: {
			display: false,
			text: 'Live knock readings'
		},
		scales: {
			xAxes: [{
				type: 'realtime',
				realtime: {
					duration: 20000,
					refresh: 400,
					delay: 800,
					onRefresh: onRefresh
				}
			}],
			yAxes: [{
				scaleLabel: {
					display: true,
					labelString: 'Knock'
				}
			}]
		},
		tooltips: {
			mode: 'nearest',
			intersect: false
		},
		hover: {
			mode: 'nearest',
			intersect: false
		}
	}
};

window.onload = function () {
    refreshSerialPorts();


    var ctx = document.getElementById('liveChart').getContext('2d');
    var ctx1 = document.getElementById('thresholdChart').getContext('2d');
    liveChart = new Chart(ctx, liveChartConfig);

    //Pause the live chart (Get's unpaused when connection is established)
    liveChartConfig.options.scales.xAxes[0].realtime.pause = true;
	window.liveChart.update({duration: 0});
};

