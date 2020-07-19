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
/* Structures */

/* Prototypes */
void commandParser();
void show_info_cb();
void select_next_wheel_cb();
void select_previous_wheel_cb();
void toggle_invert_primary_cb();
void toggle_invert_secondary_cb();
void select_wheel_cb();
void set_rpm_cb();
void sweep_rpm_cb();
void reverse_wheel_direction_cb();
void setRPM(uint32_t);

/* General functions */
void serialSetup();
void display_new_wheel();
void compute_sweep_stages(uint16_t *, uint16_t *);
/* General functions */

#endif
