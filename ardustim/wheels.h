
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
 #ifndef __WHEELS_H__
 #define __WHEELS_H__
 
 #include <avr/pgmspace.h>
 #include "wheel_defs.h"
 
 #define MAX_SWEEP_STEPS 12

 
volatile struct pattern_set {
  uint16_t beginning_ocr;
  uint16_t ending_ocr;
  bool reset_prescale;
  byte prescaler_bits;
  uint16_t oc_step;
  uint16_t steps;
}
SweepSteps[MAX_SWEEP_STEPS];

/* Tie things into one nicer structure ... */
const struct wheels {
  prog_char *decoder_name;
  prog_uchar *edge_states_ptr;
  const float rpm_scaler;
  const uint16_t wheel_max_edges;
} 
Wheels[MAX_WHEELS] = {

  /* Pointer to friendly name string, pointer to edge array, RPM Scaler, Number of edges in the array */
  { 
    dizzy_four_cylinder_friendly_name, dizzy_four_cylinder, 0.03333, 4 }
  ,
  { 
    dizzy_six_cylinder_friendly_name, dizzy_six_cylinder, 0.05, 6 }
  ,
  { 
    dizzy_eight_cylinder_friendly_name, dizzy_eight_cylinder, 0.06667, 8 }
  ,
  { 
    sixty_minus_two_friendly_name, sixty_minus_two, 1.0, 120 }
  ,
  { 
    sixty_minus_two_with_cam_friendly_name, sixty_minus_two_with_cam, 1.0, 240 }
  ,
  { 
    thirty_six_minus_one_friendly_name, thirty_six_minus_one, 0.6, 72         }
  ,
  { 
    four_minus_one_with_cam_friendly_name, four_minus_one_with_cam, 0.06667, 16         }
  ,
  { 
    eight_minus_one_friendly_name, eight_minus_one, 0.13333, 16         }
  ,
  { 
    six_minus_one_with_cam_friendly_name, six_minus_one_with_cam, 0.15, 36         }
  ,
  { 
    twelve_minus_one_with_cam_friendly_name, twelve_minus_one_with_cam, 0.6, 144         }
  ,
  { 
    fourty_minus_one_friendly_name, fourty_minus_one, 0.66667, 80         }
  ,
  { 
    dizzy_trigger_return_friendly_name, dizzy_trigger_return, 0.075, 9         }
  ,
  { 
    oddfire_vr_friendly_name, oddfire_vr, 0.2, 24         }
  ,
  { 
    optispark_lt1_friendly_name, optispark_lt1, 3.0, 720         }
  ,
  { 
    twelve_minus_three_friendly_name, twelve_minus_three, 0.4, 48         }
  ,
  { 
    thirty_six_minus_two_two_two_friendly_name, thirty_six_minus_two_two_two, 0.6, 72         }
  ,
  { 
    thirty_six_minus_two_two_two_with_cam_friendly_name, thirty_six_minus_two_two_two_with_cam, 0.15, 144         }
  ,
  { 
    fourty_two_hundred_wheel_friendly_name, fourty_two_hundred_wheel, 0.6, 72         }
  ,
  { 
    thirty_six_minus_one_with_cam_fe3_friendly_name, thirty_six_minus_one_with_cam_fe3, 0.6, 144         }
  ,
  { 
    six_g_seventy_two_with_cam_friendly_name, six_g_seventy_two_with_cam, 0.6, 144         }
  ,
  { 
    buell_oddfire_cam_friendly_name, buell_oddfire_cam, 0.33333, 80         }
  ,
  { 
    gm_ls1_crank_and_cam_friendly_name, gm_ls1_crank_and_cam, 6.0, 720         }
  ,
  { 
    lotus_thirty_six_minus_one_one_one_one_friendly_name, lotus_thirty_six_minus_one_one_one_one, 0.6, 72         }
  ,
  { 
    honda_rc51_with_cam_friendly_name, honda_rc51_with_cam, 0.2, 48         }
  ,
  { 
    thirty_six_minus_one_with_second_trigger_friendly_name, thirty_six_minus_one_with_second_trigger, 0.6, 144         }
  ,
  { 
    thirty_six_minus_one_plus_one_with_cam_ngc4_friendly_name, thirty_six_minus_one_plus_one_with_cam_ngc4, 3.0, 720         }
  ,
  { 
    weber_iaw_with_cam_friendly_name, weber_iaw_with_cam, 0.6, 144         }
  ,
  { 
    fiat_one_point_eight_sixteen_valve_with_cam_friendly_name, fiat_one_point_eight_sixteen_valve_with_cam, 3.0, 720         }
  ,
  { 
    three_sixty_nissan_cas_friendly_name, three_sixty_nissan_cas, 3.0, 720         }
  ,
  { 
    twenty_four_minus_two_with_second_trigger_friendly_name, twenty_four_minus_two_with_second_trigger, 0.3, 72         }
  ,
};

#endif
