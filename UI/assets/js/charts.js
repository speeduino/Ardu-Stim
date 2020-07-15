var ctx = document.getElementById('liveChart').getContext('2d');
var ctx1 = document.getElementById('thresholdChart').getContext('2d');

function randomScalingFactor() {
	return (Math.random() > 0.5 ? 1.0 : -1.0) * Math.round(Math.random() * 100);
}

function onRefresh(chart) {
    if(port.isOpen == false) { return; }

    console.log("Open port: " + port.path)

    port.write("G1");
    //port.write(1);

    var knockValue = port.read();
    var threshold = port.read();
    threshold = 150;

    chart.config.data.datasets[0].data.push({
        x: Date.now(),
        y: threshold
        });
        chart.config.data.datasets[1].data.push({
            x: Date.now(),
            y: knockValue
            });
    /*
	chart.config.data.datasets.forEach(function(dataset) {
		dataset.data.push({
			x: Date.now(),
			y: randomScalingFactor()
		});
	});
    */
}

var config = {
	type: 'line',
	data: {
		datasets: [{
			label: 'Knock threshold',
			//backgroundColor: color(chartColors.red).alpha(0.5).rgbString(),
			//borderColor: chartColors.red,
			fill: false,
			lineTension: 0,
			borderDash: [8, 4],
			data: []
		}, {
			label: 'Knock reading',
			//backgroundColor: color(chartColors.blue).alpha(0.5).rgbString(),
			//borderColor: chartColors.blue,
			fill: false,
			cubicInterpolationMode: 'monotone',
			data: []
		}]
	},
	options: {
		title: {
			display: true,
			text: 'Line chart (hotizontal scroll) sample'
		},
		scales: {
			xAxes: [{
				type: 'realtime',
				realtime: {
					duration: 20000,
					refresh: 100,
					delay: 500,
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

myChart = new Chart(ctx, config);