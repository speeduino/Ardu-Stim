const { app, BrowserWindow, ipcMain } = require('electron')
const {spawn} = require('child_process');
const {execFile} = require('child_process');

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let win

var avrdudeErr = "";
var avrdudeIsRunning = false;

function createWindow () {
  // Create the browser window.
  windowWidth = 1024;
  windowHeight = 700;
  if(process.platform == "win32") 
  {
    windowWidth = 1037;
    windowHeight = 725;
  }

  win = new BrowserWindow({
    width: windowWidth,
    height: windowHeight, 
    backgroundColor: '#312450', 
    webPreferences: {
      contextIsolation: false,
      nodeIntegration: true,
      backgroundThrottling: false,
    },
  });

  // auto hide menu bar (Win, Linux)
  win.setMenuBarVisibility(false);
  win.setAutoHideMenuBar(true);

  // remove completely when app is packaged (Win, Linux)
  if (app.isPackaged) {
    win.removeMenu();
  }

  // and load the index.html of the app.
  win.loadFile('index.html')

  // Open the DevTools.
  //win.webContents.openDevTools()

  // Emitted when the window is closed.
  win.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    win = null
  })

  // Open links in external browser
  win.webContents.setWindowOpenHandler(({ url }) => {
    if (url.startsWith('https:')) {
      require('electron').shell.openExternal(url);
    }
    return { action: 'deny' };
  });
}

app.allowRendererProcessReuse = false;

// Register handler before app.on/createWindow as this is used during window creation
ipcMain.handle('getAppVersion', async (e) => {
  return app.getVersion();
});

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow)

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', () => {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (win === null) {
    createWindow()
  }
})

ipcMain.on('uploadFW', (e, args) => {

  if(avrdudeIsRunning == true) { return; }
  avrdudeIsRunning = true; //Indicate that an avrdude process has started
  var platform;

  var burnStarted = false;
  var burnPercent = 0;

  //All Windows builds use the 32-bit binary
  if(process.platform == "win32") 
  { 
    platform = "avrdude-windows"; 
  }
  //All Mac builds use the 64-bit binary
  else if(process.platform == "darwin") 
  { 
    platform = "avrdude-darwin-x86_64";
  }
  else if(process.platform == "linux") 
  { 
    if(process.arch == "x32") { platform = "avrdude-linux_i686"; }
    else if(process.arch == "x64") { platform = "avrdude-linux_x86_64"; }
    else if(process.arch == "arm") { platform = "avrdude-armhf"; }
    else if(process.arch == "arm64") { platform = "avrdude-aarch64"; }
  }

  var executableName = __dirname + "/bin/" + platform + "/avrdude";
  executableName = executableName.replace('app.asar',''); //This is important for allowing the binary to be found once the app is packaed into an asar
  var configName = executableName + ".conf";
  if(process.platform == "win32") { executableName = executableName + '.exe'; } //This must come after the configName line above

  //var firmwareFile = __dirname + "/firmwares/m328p.hex";
  var firmwareFile = __dirname + "/firmwares/nano.hex";
  firmwareFile = firmwareFile.replace('app.asar',''); //This is important for allowing the binary to be found once the app is packaed into an asar

  var hexFile = 'flash:w:' + firmwareFile + ':i';

  //var execArgs = ['-v', '-pm328p', '-C', configName, '-carduino', '-b 57600', '-P', args.port, '-D', '-U', hexFile];
  var execArgs = ['-v', '-patmega328p', '-C', configName, '-carduino', '-b 115200', '-D', '-P', args.port, '-D', '-U', hexFile];
  var execArgs_old = ['-v', '-patmega328p', '-C', configName, '-carduino', '-b 57600', '-D', '-P', args.port, '-D', '-U', hexFile];

  console.log(executableName);
  //const child = spawn(executableName, execArgs);
  const child = execFile(executableName, execArgs);
  var child_oldbootloader;

  function logAvrdudeStdout(data) { console.log(`avrdude stdout:\n${data}`); }
  child.stdout.on('data', (data) => { logAvrdudeStdout(data); });
  

  function logAvrdudeStderr(data) 
  { 
    console.log(`avrdude stderr: ${data}`); 

    //Check if avrdude has started the actual burn yet, and if so, track the '#' characters that it prints. Each '#' represents 1% of the total burn process (50 for write and 50 for read)
    if (burnStarted == true)
    {
      if(data=="#") { burnPercent += 1; }
      e.sender.send( "upload percent", burnPercent );
    }
    else
    {
      //This is a hack, but basically watch the output from avrdude for the term 'Writing | ', everything after that is the #s indicating 1% of burn. 
      if(data.substr(data.length - 10) == "Writing | ")
      {
        burnStarted = true;
      }
    }
  }

  function onChildExit(code, attemptNum)
  {
    avrdudeIsRunning = false;
    if (code !== 0) 
    {
      if(attemptNum !== 0)
      {
        console.log(`avrdude process exited with code ${code}`);
        e.sender.send( "upload error", avrdudeErr );
        avrdudeErr = "";
      }
      else
      {
        console.log(`1st attempt with avrdude process exited with code ${code}. Will retry for old bootloader`);
      }
    }
    else
    {
      e.sender.send( "upload completed", code );
    }
  }

  child.stderr.on('data', (data) => 
  {
    avrdudeErr = avrdudeErr + data;

    //This is a hack to try and detect old nano bootloader vs new. There does not seem to be a 'nice' way to do this that I can find. 
    if(data.includes("resp=0x00") || data.includes("resp=0x01"))
    {
      avrdudeRetry = true;
      child.kill();
      console.log("Burn failed. Retrying for old bootloader.");
      child_oldbootloader = execFile(executableName, execArgs_old);
      e.sender.send( "old bootloader")
      child_oldbootloader.stdout.on('data', (data) => { logAvrdudeStdout(data); });
      child_oldbootloader.stderr.on('data', (data) => { logAvrdudeStderr(data); });
      child_oldbootloader.on('close', (code) => { onChildExit(code, 1); });
      //avrdudeRetry = false;
    }
    logAvrdudeStderr(data);
  });

  child.on('error', (err) => {
    console.log('Failed to start subprocess.');
    console.log(err);
    avrdudeIsRunning = false;
  });

  child.on('close', (code) => { onChildExit(code, 0); });
});

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
