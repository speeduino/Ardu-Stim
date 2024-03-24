<div align="center">

![Twisted-Builds-Logo-Color](https://github.com/twisted-builds-llc/Ardu-Stim/assets/58715445/bbe519b5-b80e-47e9-80ce-855e0538daa5)


##### This is the Twisted Builds LLC fork of the Speeduino fork of the ardustim engine simulator.
</div>

## Ardustim

Ardustim is an engine simulator built on the Arduino platform. It produces simulated crank and cam signals that can be used for testing aftermarket or factory ECUs as well as being a useful tool for the development of products or firmware for these.

This version is a fork of the Josh Stewarts Speeduino arduistim [https://github.com/speeduino/Ardu-Stim](https://github.com/speeduino/Ardu-Stim) and that one was a fork of the original by David Andruczyk [https://gitlab.com/libreems-suite/ardu-stim](https://gitlab.com/libreems-suite/ardu-stim) and is intended to provide a more modern, cross platform GUI as well as continued expansion of the trigger pattern library. It was primarily developed for use by the Speeduino community, but can be utilised for testing virtually any aftermarket ECU system. Dale Follett of Twisted Builds LLC has customized this fork for use with the Twisted Builds LLC version of the engine simulator, and have added multiple wheel patterns as requested by customers and users. If you are on any other software version of this ardu-stim software, please feel free to copy/paste any of the wheel patterns I have added and use them as you see fit. There is no guarentee on any of the wheels used or added to this code base, however at time of upload they appear to be correct. Please use and test at your own risk.

## Wiring

- **Arduino Uno**
  - pin `8` will provide the `crank` or primary wheel signal
  - pin `9` will provide the `cam` or secondary wheel signal
- **Arduino Mega**
  - pin `53` will provide the `crank` or primary wheel signal
  - pin `52` will provide the `cam` or secondary wheel signal

Example for `Arduino Uno` connected to `Speeduino v0.4 Series` board with `Arduino Mega`:

![ArduStim wiring](docs/uno-v04-wiring.png)

## Build

The source code can be built in either PlatformIO or the Arduino IDE and does not have any dependencies on 3rd party libraries that were used in the original version of Ardustim (Eg SerialUI)

Simply open the `ardustim` sub-folder in PlatformIO or the Arduino IDE and it should compile up.

Intended hardware platform is the Arduino Nano or Diecimila.

## Installing GUI from Source

### Pre-Requisites

- NPM - https://www.npmjs.com/get-npm
- Python 2.x
- Git

### GUI Installation steps

```bash
$ git clone https://github.com/speeduino/Ardu-Stim.git
$ cd Ardu-Stim/UI
$
$ npm install electron-rebuild -g
$ npm install
$ npm start
```
