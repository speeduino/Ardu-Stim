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

#include "ardustim.h"
#include "sweep.h"


//! Builds the SweepSteps[] structure
/*!
 * For sweeping we cannot just pick the TCNT value at the beginning and ending
 * and sweep linearily between them as it'll result in a VERY slow RPM change
 * at the low end and a VERY FAST change at the high end due to the inverse
 * relationship between RPM and TCNT. So we compromise and break up the RPM
 * range into octaves (doubles of RPM), and use a linear TCNT change between
 * those two points. It's not perfect, but computationally easy
 *
 * \param low_rpm_tcnt pointer to low rpm OC value, (not prescaled!)
 * \param high_rpm_tcnt pointer to low rpm OC value, (not prescaled!)
 * \param total_stages pointer to tell the number of structs to allocate
 * \returns pointer to array of structures for each sweep stage.
 */
sweep_step *build_sweep_steps(uint32_t *low_rpm_tcnt, uint32_t *high_rpm_tcnt, uint8_t *total_stages)
{
  sweep_step *steps;
  uint8_t prescaler_bits;
  uint8_t bitshift;
  uint32_t tmp = *low_rpm_tcnt;
  /* DEBUG
  mySUI.print(*low_rpm_tcnt);
  mySUI.print(F("<->"));
  mySUI.println(*high_rpm_tcnt);
   */

  steps = (sweep_step *)malloc(sizeof(sweep_step)*(*total_stages));

#ifdef MORE_LINEAR_SWEEP
  for (uint8_t i = 0; i < (*total_stages); i+=2)
#else
  for (uint8_t i = 0; i < (*total_stages); i++)
#endif
  {
    /* The low rpm value will ALWAYS have the highed TCNT value so use that
    to determine the prescaler value
    */
    get_prescaler_bits(&tmp, &steps[i].prescaler_bits, &bitshift);
    
    steps[i].beginning_ocr = (uint16_t)(tmp >> bitshift);
    if ((tmp >> 1) < (*high_rpm_tcnt))
      steps[i].ending_ocr = (uint16_t)((*high_rpm_tcnt) >> bitshift);
    else
      steps[i].ending_ocr = (uint16_t)(tmp >> (bitshift + 1)); // Half the begin value
    tmp = tmp >> 1; /* Divide by 2 */
    /* DEBUG
    mySUI.print(steps[i].beginning_ocr);
    mySUI.print(F("<->"));
    mySUI.println(steps[i].ending_ocr);
    */
  }
  return steps;
}

