var teeth, depth, radius;
var toothPattern1 = [1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, /* teeth 11-15 */
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, /* teeth 16-20 */
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, /* teeth 21-25 */
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, /* teeth 26-30 */
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, /* teeth 31-35 */
  0, 0
];
var toothPattern2 = [1, 0, 1, 0, 1, 0, 1, 0];
var toothPattern3 = [0,0,0,0,0,0,0,0,0,0,2,2,
      2,2,2,2,2,2,2,2,2,3,3,2,
      2,3,3,2,2,3,3,2,2,3,3,2,
      2,2,2,2,2,2,2,2,2,2,2,3,
      3,3,3,3,1,1,1,1,1,1,1,0,
      0,1,1,0,0,1,1,0,0,1,1,0,
      0,0,0,0,0,0,0,0,0,0,2,2,
      2,2,2,2,2,2,2,2,2,3,3,2,
      2,3,3,2,2,3,3,2,2,3,3,2,
      2,2,2,2,0,0,0,0,0,0,0,1,
      1,1,1,1,1,1,1,1,1,1,1,0,
      0,1,1,0,0,1,1,0,0,1,1,0];
var toothPattern4 = [2,2,2,2,2,2,2,2,2,2,2,0,
      0,0,0,0,0,0,0,0,0,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,2,2,2,3,3,3,
      3,3,3,3,3,3,3,3,1,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,2,3,3,3,
      3,3,3,3,3,3,3,3,3,3,3,2];
 var toothPattern5 = [1,0,1,0,1,0,1,0,1,0, /* Teeth 1-5 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 6-10  */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */
     1,0,1,0,3,2,3,2,1,0, /* Teeth 31-35, second trigger during 33-34 */
     0,0,                 /* 36th tooth MISSING */
     1,0,1,0,1,0,1,0,1,0, /* Revolution 2  Teeth 1-5 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 6-10 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */
     1,0,1,0,1,0,1,0,0,0, /* Teeth 31-35 */
     0,0];

  var toothPatterns = [
                      toothPattern1,
                      toothPattern2,
                      toothPattern3,
                      toothPattern4,
                      toothPattern5,
                      ];

function arcToRad(num) {
  return (360 / num) * (Math.PI / 180) / 2;
}

function draw_crank_gear(toothPattern, depth, radius, width, line, halfspeed) {
  var canvas = document.getElementById('crank');
  var c = canvas.getContext('2d');

  c.clearRect(0, 0, canvas.width, canvas.height);

  var x = canvas.width / 2;
  var y = canvas.height / 2
  var lastEnd = 0;
  var teeth = toothPattern.length / 2;
  if(halfspeed) { teeth = teeth / 2; }
  var firstEnd

  depth = (radius / 100) * depth;

  c.beginPath();
  //Draw an inner ring
  c.arc(x, y, radius/5, 0, 2 * Math.PI);
  c.moveTo(x+radius-depth, y); //Move to the starting point
  
  for (var i = 0; i <= (teeth * 2)-1; i++) 
  {

    var start, end, newEnd, diff;
    diff = ((((100 - width) / 100) / 2) * arcToRad(teeth));
    newEnd = lastEnd + arcToRad(teeth);

    start = lastEnd + diff;
    end = newEnd - diff;

    /*
    * Values to use within a wheel definition. 
    * 0 means no tooth on any wheel definition
    * 1 means crank tooth
    * 2 means cam1 tooth
    * 4 means cam2 tooth
    * 
    * combinations of numbers mean all of the related teeth are present, 
    * eg  3 means crank and cam1, 5 means crank and cam2, 6 means cam1 and cam2, 7 means crank, cam1 and cam2 
    */
    if (toothPattern[i] == 1 || toothPattern[i] == 3 || toothPattern[i] == 5 || toothPattern[i] == 7) 
    {
      c.arc(x, y, radius, start, end, false);
    } else 
    {
      c.arc(x, y, (radius - depth), start, end, false);
    }
    lastEnd = newEnd;
  }
  //Draw the final connecting line
  //c.lineTo(x+radius, y);
  
  c.strokeStyle = '#fefefe';
  c.fillStyle = "rgba( 51,141,198, 0.8 )";
  c.fill();
  c.lineWidth = line;
  c.stroke();
}

function draw_cam_gear(toothPattern, depth, radius, width, line) {
  var canvas = document.getElementById('cam');
  var c = canvas.getContext('2d');

  c.clearRect(0, 0, canvas.width, canvas.height);

  var x = canvas.width / 2;
  var y = canvas.height / 2
  var lastEnd = 0;
  var teeth = toothPattern.length / 2;

  depth = (radius / 100) * depth;

  c.beginPath();
  //Draw an inner ring
  c.arc(x, y, radius/5, 0, 2 * Math.PI);
  c.moveTo(x+radius-depth, y); //Move to the starting point
  
  for (var i = 0; i <= (teeth * 2)-1; i++) 
  {

    var start, end, newEnd, diff;
    diff = ((((100 - width) / 100) / 2) * arcToRad(teeth));
    newEnd = lastEnd + arcToRad(teeth);

    start = lastEnd + diff;
    end = newEnd - diff;

    /*
    * Values to use within a wheel definition. 
    * 0 means no tooth on any wheel definition
    * 1 means crank tooth
    * 2 means cam1 tooth
    * 4 means cam2 tooth
    * 
    * combinations of numbers mean all of the related teeth are present, 
    * eg  3 means crank and cam1, 5 means crank and cam2, 6 means cam1 and cam2, 7 means crank, cam1 and cam2 
    */
    if (toothPattern[i] == 2 || toothPattern[i] == 3 || toothPattern[i] == 6 || toothPattern[i] == 7) 
    {
      c.arc(x, y, radius, start, end, false);
    } else 
    {
      c.arc(x, y, (radius - depth), start, end, false);
    }
    lastEnd = newEnd;
  }
  //Draw the final connecting line
  //c.lineTo(x+radius, y);
  
  c.strokeStyle = '#fefefe';
  c.fillStyle = "rgba( 51,141,198, 0.8 )";
  c.fill();
  c.lineWidth = line;
  c.stroke();
}
