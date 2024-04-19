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
 
#define TMP_RPM_SHIFT 4 /* x16, 0-16384 RPM via pot */
#define TMP_RPM_CAP 9000 /* MAX RPM via pot control. Adjusted to 9,000rpm max from 16,384rpm to match the GUI */
#define EEPROM_LAST_MODE  100

struct configTable {
  uint8_t mode;
  uint16_t sweep_low_rpm = 250;
  uint16_t sweep_high_rpm = 4000;
  uint16_t sweep_interval = 1000;
};
extern struct configTable config;

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

#endif
