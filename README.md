<div align="center">

<img src="https://github.com/speeduino/wiki.js/raw/master/img/Speeduino%20logo_med.png" alt="Speeduino" width="600" />

[![Release](https://img.shields.io/github/release/speeduino/Ardu-Stim.svg)](https://github.com/speeduino/Ardu-Stim/releases/latest)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](https://github.com/speeduino/Ardu-Stim/blob/master/LICENSE)
[![https://img.shields.io/discord/879495735912071269 ](https://img.shields.io/discord/879495735912071269?label=Discord&logo=Discord)](https://discord.gg/YWCEexaNDe)

##### This is the Speeduino fork of the ardustim engine simulator.
</div>

## Ardu-Stim

Ardustim is an engine simulator built on the Arduino platform. It produces simulated crank and cam signals that can be used for testing aftermarket ECUs as well as being a useful tool for the development of firmware for these. It supports a large number of simulated rtigger patterns as well as multiple options for output speed (Eg Using an external pot, a fixed value or a sweep range)

<div align="center"><img src="https://github.com/speeduino/Ardu-Stim/raw/master/docs/demo.gif" alt="Ardu-Stim Demo" width="80%" /></div>

This version is a fork of the original by David Andruczyk [https://gitlab.com/libreems-suite/ardu-stim](https://gitlab.com/libreems-suite/ardu-stim) and is intended to provide a more modern, cross platform GUI as well as continued expansion of the trigger pattern library. It was primarily developed for use by the Speeduino community, but can be utilised for testing virtually any aftermarket ECU system

It is designed to run on an Arduino Nano, but will also work with Arduino Uno and Mega boards. 

## Wiring

- **Arduino Nano or Uno**
  - pin `8` will provide the `crank` or primary wheel signal
  - pin `9` will provide the `cam` or secondary wheel signal
  - Pin `10` will provide a `2nd cam` or tertiary wheel signal. This is for simulating some dual cam patterns
- **Arduino Mega**
  - pin `53` will provide the `crank` or primary wheel signal
  - pin `52` will provide the `cam` or secondary wheel signal
  - Pin `51` will provide a `2nd cam` or tertiary wheel signal. This is for simulating some dual cam patterns

Example for `Arduino Nano` connected to `Speeduino v0.4` ECU:

<div align="center"><img src="https://github.com/speeduino/Ardu-Stim/raw/master/docs/nano-v0.4-wiring.png" alt="Ardu-Stim Wiring" width="80%" /></div>

### RPM Potentiometer

An optional potentiometer can be added to control the RPM value (With the relevant RPM mode selected). This should be connect to pin A0 if in use. 

## Installing and Using

Ardu-Stim is distributed as a ready-to-run binary for Windows, Mac (Intel and Arm) and linux (AppImage) so no installation is required. Simply down the latest release (https://github.com/speeduino/Ardu-Stim/releases/latest) and 

### First time Connection
The first time you connect Ardu-Stim to an Arduino Nano board, you need to upload the included firmware to it. Plug the Nano into your PC and then select the port from the list. Press the upload firmware button and wait for this to complete

<div align="center"><img src="https://github.com/speeduino/Ardu-Stim/raw/master/docs/upload-firmware.png" alt="Ardu-Stim Wiring" width="80%" /></div>

**Note:** This only needs to be performed with a new Arduino Nano or if upgrading from an earlier version. The automated uploading of firmware is ONLY available for Arduino Nano boards. If using another board you will need to manually compile and upload the firmware (Not the GUI)

## Firmware Build

Optionally, the firmware source code can be built in either PlatformIO or the Arduino IDE and does not have any dependencies on 3rd party libraries that were used in the original version of Ardustim (Eg SerialUI)

Simply open the `ardustim` sub-folder in PlatformIO or the Arduino IDE and it should compile without issue.

Intended hardware platform is the Arduino Nano or Uno.

## Installing GUI from Source

### Pre-Requisites

- NPM - https://www.npmjs.com/get-npm
- Python
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
