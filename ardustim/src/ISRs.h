/* vim: set syntax=c expandtab sw=2 softtabstop=2 autoindent smartindent smarttab : */                                      
/*
 * Arbritrary wheel pattern generator
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

#ifndef __ISRS_H__
#define __ISRS_H__

#include "ardustim.h"

ISR(ADC_vect);         /* Analog pot for analog RPM control with no UI */
ISR(TIMER1_COMPA_vect) /* High speed pattern output */
ISR(TIMER2_COMPA_vect) /* Sweeper */


#endif
