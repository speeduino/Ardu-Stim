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
#ifndef __SERIAL_MENU_H__
#define __SERIAL_MENU_H__
 
#include <SerialUI.h>
//#include "structures.h"

/* Structures */

/* Prototypes */
/* Callbacks */
void show_info_cb(void);
void select_next_wheel_cb(void);
void select_previous_wheel_cb(void);
void toggle_invert_primary_cb(void);
void toggle_invert_secondary_cb(void);
void list_wheels_cb(void);
void select_wheel_cb(void);
void set_rpm_cb(void);
void sweep_rpm_cb(void);
void reverse_wheel_direction_cb(void);
void do_exit(void);
/* Callbacks */

/* General functions */
void serial_setup(void);
void display_new_wheel(void);
void compute_sweep_stages(uint16_t *, uint16_t *);
uint16_t get_rpm_from_tcnt(uint16_t *, uint8_t *);
uint8_t get_bitshift_from_prescaler(uint8_t *);

/* General functions */

/* Prototypes */

SUI_DeclareString(greeting,"+++ Welcome to the ArduStim +++\r\nEnter ? for help");
SUI_DeclareString(bytes, " bytes.");
SUI_DeclareString(top_menu_title,"ArduStim Main Menu");
SUI_DeclareString(info_key, "Information");
SUI_DeclareString(info_help, "Retrieve data and current settings");
SUI_DeclareString(rpm_key, "Set Fixed RPM");
SUI_DeclareString(sweep_key, "Set Swept RPM");
SUI_DeclareString(sweep_help, "Sweep the RPM (min,max,rate(rpm/sec))");
SUI_DeclareString(exit_key, "Exit");
SUI_DeclareString(exit_help, "Exit (and terminate Druid)");  
SUI_DeclareString(wheel_menu_key, "Wheel Options");
SUI_DeclareString(wheel_menu_help, "Wheel Options, (list,choose,select)");
SUI_DeclareString(next_key, "Next wheel");
SUI_DeclareString(next_help, "Pick the next wheel pattern");
SUI_DeclareString(previous_key, "Previous wheel");
SUI_DeclareString(previous_help, "Pick the previous wheel pattern");
SUI_DeclareString(list_key, "List wheels");
SUI_DeclareString(list_help, "List all wheel patterns");
SUI_DeclareString(choose_key, "Choose wheel");
SUI_DeclareString(choose_help, "Choose a specific wheel pattern by number");
SUI_DeclareString(reverse_key, "Reverse Wheel Direction");
SUI_DeclareString(reverse_help, "Reverse the wheel's direction of rotation");
SUI_DeclareString(advanced_menu_key, "Advanced Options");
SUI_DeclareString(advanced_menu_help, "Advanced Options (polarity,glitch)");
SUI_DeclareString(pri_invert_key, "Invert Primary");
SUI_DeclareString(pri_invert_help, "Invert Primary (crank) signal polarity");
SUI_DeclareString(sec_invert_key, "Invert Secondary");
SUI_DeclareString(sec_invert_help, "Invert Secondary (cam) signal polarity");
SUI_DeclareString(RPM_label, "RPM:");

/* Info callback */
SUI_DeclareString(info_title, "Welcome to ArduStim, written by David J. Andruczyk");
SUI_DeclareString(free_ram, "Free RAM: ");
SUI_DeclareString(colon_space, ": ");
SUI_DeclareString(space, " ");
SUI_DeclareString(RPM, "RPM");
SUI_DeclareString(arrows, "<->");
SUI_DeclareString(read_colon_space, "Read: ");
SUI_DeclareString(space_to_colon_space, " to: ");
SUI_DeclareString(space_at_colon_space, " at: ");
SUI_DeclareString(space_chars_from_user, " charactors from the user");
SUI_DeclareString(new_wheel_chosen, "New Wheel chosen: ");
SUI_DeclareString(new_rpm_chosen, "New RPM chosen: ");
SUI_DeclareString(current_pattern, "Currently selected Wheel pattern: ");
SUI_DeclareString(fixed_current_rpm, "Fixed RPM mode, Current RPM: ");
SUI_DeclareString(swept_rpm_from, "Swept RPM mode From: ");
SUI_DeclareString(wheel_direction_colon_space, "Wheel Direction: ");
SUI_DeclareString(signal, "Signal");
SUI_DeclareString(rpm_per_sec_colon_space, "RPM/sec: ");
SUI_DeclareString(space_rpm_per_sec, " RPM/sec");
SUI_DeclareString(normal_, "Normal");
SUI_DeclareString(inverted_, "Inverted");
SUI_DeclareString(reversed_, "Reversed");
SUI_DeclareString(primary_space_signal_colon_space, "Primary Signal: ");
SUI_DeclareString(secondary_space_signal_colon_space, "Secondary Signal: ");
SUI_DeclareString(low_RPM_colon_space, "Low RPM: ");
SUI_DeclareString(high_RPM_colon_space, "High RPM: ");
SUI_DeclareString(range_error,"Range error !(10-50000,10-50000,1-50000)!");
SUI_DeclareString(number_of_matches,"Number of successful matches (should be 3): ");
SUI_DeclareString(sweeping_from_colon_space,"Sweeping from: ");

#endif
