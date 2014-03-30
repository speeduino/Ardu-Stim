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
 * along with any FreeEMS software.  If not, see http://www.gnu.org/licenses/
 *
 */
#ifndef __SERIAL_MENU_H__
#define __SERIAL_MENU_H__
 
#include <SerialUI.h>

/* Prototypes */
void serial_setup();
void show_info();
void select_next_wheel();
void select_previous_wheel();
void list_wheels();
void select_wheel();
void set_rpm();
void sweep_rpm();
int check_and_adjust_tcnt_limits(long *, long *) ;  
void reset_new_OCR1A(uint16_t);
/* Prototypes */


SUI_DeclareString(greeting,"+++ Welcome to the ArduStim +++\r\nEnter ? for help.");
SUI_DeclareString(bytes, " bytes.");
SUI_DeclareString(top_menu_title,"ArduStim Main Menu");
SUI_DeclareString(info_key, "information");
SUI_DeclareString(info_help, "Retrieve data and current settings");
SUI_DeclareString(next_key, "next");
SUI_DeclareString(next_help, "Pick the next wheel pattern");
SUI_DeclareString(previous_key, "previous");
SUI_DeclareString(previous_help, "Pick the previous wheel pattern");
SUI_DeclareString(list_key, "list");
SUI_DeclareString(list_help, "List all wheel patterns");
SUI_DeclareString(choose_key, "choose");
SUI_DeclareString(choose_help, "Choose a specific wheel pattern by number");
SUI_DeclareString(wheel_list, "list");
SUI_DeclareString(wheel_list_help, "List all wheel patterns");
SUI_DeclareString(rpm_key, "rpm");
SUI_DeclareString(rpm_help, "Set the RPM");
SUI_DeclareString(sweep_key, "sweep");
SUI_DeclareString(sweep_help, "Sweep the RPM (min,max,rpm/second");
SUI_DeclareString(colon_space, ": ");
SUI_DeclareString(space_RPM, " RPM");
SUI_DeclareString(space_at_space, " at ");
SUI_DeclareString(new_wheel_chosen, "New Wheel chosen: ");
SUI_DeclareString(new_rpm_chosen, "New RPM chosen: ");

/* Info callback */
SUI_DeclareString(info_title, "Welcome to ArduStim, written by David J. Andruczyk");
SUI_DeclareString(free_ram, "Free RAM: ");
SUI_DeclareString(current_pattern, "Currently selected Wheel pattern: ");
SUI_DeclareString(fixed_current_rpm, "Fixed RPM mode, Current RPM: ");
SUI_DeclareString(swept_rpm_from, "Swept RPM mode From: ");
SUI_DeclareString(space_to_colon_space, " to: ");
SUI_DeclareString(space_at_colon_space, " at: ");
SUI_DeclareString(rpm_per_second, " RPM/second");

#endif
