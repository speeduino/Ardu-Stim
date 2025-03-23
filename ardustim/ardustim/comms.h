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
#ifndef __COMMS_H__
#define __COMMS_H__
 
#include <Arduino.h>

// Prototypes for serial command handlers
void commandParser();               // Parses incoming serial commands
void show_info_cb();                // Displays system information (not implemented here)
void select_next_wheel_cb();        // Selects the next wheel pattern
void select_previous_wheel_cb();    // Selects the previous wheel pattern
void toggle_invert_primary_cb();    // Toggles inversion of primary output
void toggle_invert_secondary_cb();  // Toggles inversion of secondary output
void select_wheel_cb();             // Selects a specific wheel (not implemented here)
void set_rpm_cb();                  // Sets RPM (not implemented here)
void sweep_rpm_cb();                // Configures RPM sweep (not implemented here)
void reverse_wheel_direction_cb();  // Reverses wheel direction (not implemented here)

// General serial functions
void serialSetup();                 // Initializes serial communication
void display_new_wheel();           // Updates system for new wheel selection

#endif
