/* vim: set syntax=c expandtab sw=2 softtabstop=2 autoindent smartindent smarttab : */
/*
 * Arbritrary wheel pattern generator wheel definitions
 *
 * copyright 2014 David J. Andruczyk
 * 
 * Ardu-Stim software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArduStim software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with any ArduStim software.  If not, see http://www.gnu.org/licenses/
 *
 */
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "Arduino.h"
#include "wheel_defs.h"

#define VERSION 2
 
#define TMP_RPM_SHIFT 4 /* x16, 0-16384 RPM via pot */
#define TMP_RPM_CAP 9000 /* MAX RPM via pot control. Adjusted to 9,000rpm max from 16,384rpm to match the GUI */
#define EEPROM_LAST_MODE  100

#define COMPRESSION_TYPE_1CYL_4STROKE 0 //Not initiallity supported
#define COMPRESSION_TYPE_2CYL_4STROKE 1
#define COMPRESSION_TYPE_3CYL_4STROKE 2 //Not initiallity supported
#define COMPRESSION_TYPE_4CYL_4STROKE 3
#define COMPRESSION_TYPE_6CYL_4STROKE 4
#define COMPRESSION_TYPE_8CYL_4STROKE 5

struct configTable 
{
  uint8_t version;                                                   
  uint8_t wheel = FOUR_TWENTY_A;
  uint8_t mode;
  uint16_t fixed_rpm = 2500;
  uint16_t sweep_low_rpm = 250;
  uint16_t sweep_high_rpm = 4000;
  uint16_t sweep_interval = 1000;

  //11
  bool useCompression = false;
  uint8_t compressionType = 0;
  uint16_t compressionRPM = 400;
  uint16_t compressionOffset = 0;
  bool compressionDynamic = false;
} __attribute__ ((packed));
extern struct configTable config;


struct status 
{
  uint16_t base_rpm; //RPM excluding compression modifier
  uint16_t compressionModifier;
  uint16_t rpm; //Final RPM
};
extern struct status currentStatus;

/* Tie things wheel related into one nicer structure ... */
typedef struct _wheels wheels;
struct _wheels {
  const char *decoder_name PROGMEM;
  const unsigned char *edge_states_ptr PROGMEM;
  const float rpm_scaler;
  //const uint16_t rpm_scaler;
  const uint16_t wheel_max_edges;
  const uint16_t wheel_degrees;
};

//A sin wave of amplitude 100 with a complete cycle in 180 degrees (1 entry per degree). 
const uint8_t sin_100_180[] PROGMEM = 
{ 
  0,0,0,0,0,1,1,1,2,2,3,4,4,5,6,7,8,9,10,11,
  12,13,14,15,17,18,19,21,22,24,25,27,28,30,
  31,33,35,36,38,40,41,43,45,47,48,50,52,53,
  55,57,59,60,62,64,65,67,69,70,72,73,75,76,
  78,79,81,82,83,85,86,87,88,89,90,91,92,93,
  94,95,96,96,97,98,98,99,99,99,100,100,100,
  100,100,100,100,100,100,99,99,99,98,98,97,
  96,96,95,94,93,92,91,90,89,88,87,86,85,83,
  82,81,79,78,76,75,73,72,70,69,67,65,64,62,
  60,59,57,55,53,52,50,48,47,45,43,41,40,38,
  36,35,33,31,30,28,27,25,24,22,21,19,18,17,
  15,14,13,12,11,10,9,8,7,6,5,4,4,3,2,2,1,1,
  1,0,0,0,0
};

//A sin wave of amplitude 100 with a complete cycle in 90 degrees (1 entry per degree). 
const uint8_t sin_100_90[] PROGMEM = 
{ 
  0,0,0,1,2,3,4,6,8,10,12,14,17,19,22,25,28,
  31,35,38,41,45,48,52,55,59,62,65,69,72,75,
  78,81,83,86,88,90,92,94,96,97,98,99,100,100,
  100,100,100,99,98,97,96,94,92,90,88,86,83,81,
  78,75,72,69,65,62,59,55,52,48,45,41,38,35,31,
  28,25,22,19,17,14,12,10,8,6,4,3,2,1,0,0
};

//A sin wave of amplitude 100 with a complete cycle in 120 degrees

const uint8_t sin_100_120[] PROGMEM = 
{ 
  0,0,0,1,1,2,2,3,4,5,7,8,10,11,13,15,17,19,
  21,23,25,27,30,32,35,37,40,42,45,47,50,53,
  55,58,60,63,65,68,70,73,75,77,79,81,83,85,
  87,89,90,92,93,95,96,97,98,98,99,99,100,100,
  100,100,100,99,99,98,98,97,96,95,93,92,90,89,
  87,85,83,81,79,77,75,73,70,68,65,63,60,58,55,
  53,50,47,45,42,40,37,35,32,30,27,25,23,21,19,
  17,15,13,11,10,8,7,5,4,3,2,2,1,1,0,0
};

#endif
