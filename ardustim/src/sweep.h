/* ex: filetype=c expandtab shiftwidth=2 tabstop=2 softtabstop=2: */                                                        
/*
 * Arbritrary crank/cam wheel pattern generator
 *
 * copyright 2014-2016 David J. Andruczyk
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

#ifndef __SWEEP_H__
#define __SWEEP_H__

#include "structures.h"

sweep_step * build_sweep_steps(uint32_t *, uint32_t *, uint8_t *);              
void get_prescaler_bits(uint32_t *, uint8_t *, uint8_t *);
void reset_new_OCR1A(uint32_t);

#endif
