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
#ifndef __ARDUSTIM_H__
#define __ARDUSTIM_H__

#include <stdint.h>
#include <Arduino.h>

/* Prototypes */
void reset_new_OCR1A(uint32_t);
uint8_t get_bitshift_from_prescaler(uint8_t *);
void get_prescaler_bits(uint32_t *, uint8_t *, uint8_t *);
void setRPM(uint16_t);
uint16_t calculateCompressionModifier();
uint16_t calculateCurrentCrankAngle();

/* Prototypes */

#endif
