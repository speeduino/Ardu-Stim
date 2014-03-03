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
 #ifndef __WHEEL_DEFS_H__
 #define __WHEEL_DEFS_H__
 
 #include <avr/pgmspace.h>
 
 /* Wheel patterns! 
  *
  * Wheel patterns define the pin states and specific times. The ISR runs 
  * at a constant speed related to the requested RPM. The request RPM is 
  * scaled based on the LENGTH of each wheel's array.  The reference is 
  * the 60-2 which was the first decoder designed which has 120 "edges" 
  * (transitions" for each revolution of the wheel. Any other wheel that 
  * also has 120 edges has and RPM scaling factor of 1.0. IF a wheel has 
  * less edges needed to "describe" it, it's number of edges are divided by 120 to
  * get the scaling factor which is applied to the RPM calculation.
  * There is an enumeration (below) that lists the defined wheel types, 
  * as well as an array listing the rpm_scaling factors with regards to 
  * each pattern.
  * 
  * NOTE: There is MORE THAN ONE WAY to define a wheel pattern.  You can 
  * use more edges to get to 1 deg accuracy but the side effect is that 
  * your maximum RPM is capped because of that. Currently 60-2 can run 
  * up to about 60,000 RPM, 360and8 can only do about 10,000 RPM becasue 
  * it has 6x the number of edges...  The less edges, the faster it can go... :)
  * 
  * Using more edges allows you to do things like vary the dutycycle,  
  * i.e. a simple non-missing tooth 50% duty cycle wheel can be defined 
  * with only 2 entries if you really want, but I didn't do it that way 
  * for some of the simple ones as it made it seem somewhat confusing
  * to look at as it required you to keep the rpm_scaler factor in mind.  
  * Most/all patterns show the pulses you're receive for one revolution
  * of a REAL wheel on a real engine.
  */
  
  /* Wheel types we know about */
 typedef enum { 
   DIZZY_FOUR_CYLINDER,  /* 2 evenly spaced teeth */
   DIZZY_SIX_CYLINDER,   /* 3 evenly spaced teeth */
   DIZZY_EIGHT_CYLINDER, /* 4 evenly spaced teeth */
   SIXTY_MINUS_TWO,      /* 60-2 crank only */
   THIRTY_SIX_MINUS_ONE, /* 36-1 crank only */
   FOUR_MINUS_ONE_WITH_CAM, /* 4-1 crank + cam */
   EIGHT_MINUS_ONE,       /* 8-1 crank only */
   SIX_MINUS_ONE_WITH_CAM,/* 6-1 crank + cam */
   TWELVE_MINUS_ONE_WITH_CAM, /* 12-1 crank + cam */
   FOURTY_MINUS_ONE,      /* Ford V-10 40-1 crank only */
   DIZZY_TRIGGER_RETURN,  /* dizzy signal, 40deg on 50 deg off */
   ODDFIRE_VR,            /* Oddfire V-twin */
   OPTISPARK_LT1,         /* Optispark 360 and 8 */
   TWELVE_MINUS_THREE,    /* 12-3 */
   THIRTY_SIX_MINUS_TWO_TWO_TWO, /* 36-2-2-2 crank only */
   THIRTY_SIX_MINUS_TWO_TWO_TWO_WITH_CAM, /* 36-2-2-2 crank and cam */
   FOURTY_TWO_HUNDRED_WHEEL, /* 4200 wheel */
   THIRTY_SIX_MINUS_ONE_WITH_CAM_FE3, /* Mazda F3 36-1 crank and cam */
   SIX_G_SEVENTY_TWO,     /* Mitsubishi DOHC CAS and TCDS 6G72 */
   MAX_WHEELS,
 }WheelType;
 
 
 const float rpm_scaler[MAX_WHEELS] = {
   0.03333, /* dizzy 4 */
   0.05, /* dizzy 6 */
   0.06667, /* dizzy 8 */
   1.0, /* 60-2 */
   0.6, /* 36-1 */
   0.13333, /* 4-1 with cam */
   0.13333, /* 8-1 */
   0.3,     /* 6-1 with cam */
   1.2,     /* 12-1 with cam */
   0.66667, /* 40-1 */
   0.075,   /* dizzy trigger return */
   0.2,     /* Oddfire VR */
   6.0,     /* Optispark LTA (360 and 8) */ 
   0.4,     /* 12-3 */
   0.6,     /* 36-2-2-2  crank only */
   0.6,     /* 36-2-2-2  crank and cam */
   3.0,     /* 4200 crank wheel */
   1.2,     /* 36-1 with cam, mazda f3 */
   0.6,     /* 6G72 mitsubishi */
 }; 
  
 const uint16_t wheel_max_edges[MAX_WHEELS] = {
   4,   /* dizzy 4 */
   6,   /* dizzy 6 */
   6,   /* dizzy 8 */
   120, /* 60-2 */
   72,  /* 36 -1 */
   16,  /* 4-1 with cam */
   16,  /* 8-1 */
   36,  /* 6-1 with cam */
   144, /* 12-1 with cam */
   80,  /* 40-1 */
   9,   /* dizzy trigger return */
   24,  /* Oddfire VR */
   720, /* Optispark LT1 (360 and 8) */
   48,  /* 12-3 */
   72,  /* 36-2-2-2 crank only */
   72,  /* 36-2-2-2 crank and cam */
   360, /* 4200 wheel */
   144, /* 36-1 with cam, mazda fe3 */
   72,  /* 6G72 mitsubishi */
 };
 
 /* Very simple 50% duty cycle */
 PROGMEM prog_uchar dizzy_four_cylinder[] = \
   { /* dizzy 4 cylinder */
     1,0,1,0
   };
   
 /* Very simple 50% duty cycle */
 PROGMEM prog_uchar dizzy_six_cylinder[] = \
   { /* dizzy 6 cylinder */
     1,0,1,0,1,0
   };
   
 /* Very simple 50% duty cycle */
 PROGMEM prog_uchar dizzy_eight_cylinder[] = \
   { /* dizzy 8 cyl */
     1,0,1,0,1,0,1,0
   };
   
 /* Standard bosch 60-2 pattern, 50% duty cyctle during normal teeth */
 PROGMEM prog_uchar sixty_minus_two[] = \
   { /* 60-2 */
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,0,0,0,0
   };
 
 /* Standard ford/mazda and aftermarket 36-1 pattern, 50% duty cyctle during normal teeth */  
 PROGMEM prog_uchar thirty_six_minus_one[] = \
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     0,0
   }; 
   
 /* 4-1 crank signal 50% duty cycle with Cam tooth enabled during the second rotation prior to tooth 2 */
 PROGMEM prog_uchar four_minus_one_with_cam[] = \
   { /* 4-1 with cam */
     0,1,0,1,0,1,0,0,0,1, \
     2,1,0,1,0,0
   };
   
 /* Yamaha R6 crank trigger 8 teeth missing one, (22.5deg low, 22.5deg high) 50% duty cycle during normal teeth */
 PROGMEM prog_uchar eight_minus_one[] = \
   { /* 8-1 */
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,0
   };
  
  /* 40deg low, 20 deg high per tooth, cam signal on second rotation during 40deg low portion of 3rd tooth */
  PROGMEM prog_uchar six_minus_one_with_cam[] = \
   { /* 6-1 with cam */
     0,0,1,0,0,1,0,0,1,0, \
     0,1,0,0,1,0,0,0,0,0, \
     1,0,0,1,2,2,1,0,0,1, \
     0,0,1,0,0,0
   };
  
  /* 25 deg low, 5 deg high, #12 is missing,  cam is high for 25 deg on second crank rotation just after tooth 9 */
  PROGMEM prog_uchar twelve_minus_one_with_cam[] = \
   { /* 12-1 with cam */
     0,0,0,0,0,1,0,0,0,0, \
     0,1,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,0,0,0,0, \
     0,1,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,0,0,0,0, \
     0,0,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,0,0,0,0, \
     0,1,0,0,0,0,0,1,0,0, \
     0,0,0,1,0,0,0,0,0,1, \
     0,0,0,0,0,1,2,2,2,2, \
     2,1,0,0,0,0,0,1,0,0, \
     0,0,0,0
   };
   
  /* Ford V10 version of EDIS with 40 teeth instead of 36, 50% duty cycle during normal teeth.. */
  PROGMEM prog_uchar fourty_minus_one[] = \
   { /* 40-1 */
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,1, \
     0,1,0,1,0,1,0,1,0,0 
   };
  
  /* 50deg off, 40 deg on dissy style signal */
  PROGMEM prog_uchar dizzy_trigger_return[] = \
   { /* dizzy trigger return */
     0,0,0,0,0,1,1,1,1
   };
   
  /* Oddfire V twin, 15 deg on, 120deg off, 15 deg on, 210 deg off */
  PROGMEM prog_uchar oddfire_vr[] = \
   { /* Oddfire VR */
     1,0,0,0,0,0,0,0,0,1, \
     0,0,0,0,0,0,0,0,0,0, \
     0,0,0,0
   };
  
  /* GM LT1 360 and 8 wheel, see http://powerefi.com/files/opti-LT1-count.JPG */
  PROGMEM prog_uchar optispark_lt1[] = \
  { /* Optispark 360 outside teeth, 8 varying inside teeth */
  /* 1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30 */
    3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2, /* 0-30 */
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 31-60 */
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 61-90 */
    3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 91-120 */
    3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 121-150 */
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 151-180 */
    3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 181-210 */
    1,0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 211-240 */
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 241-270 */
    3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 271-300 */
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 301-330 */
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, /* 331-360 */
  };
  
  PROGMEM prog_uchar twelve_minus_three[] = \
  { /* 12-3, http://www.msextra.com/doc/triggers/12_3_wheel_133.jpg */
    1,0,0,0,1,0,0,0,  /* 1,2 */
    1,0,0,0,1,0,0,0,  /* 2,4 */
    1,0,0,0,1,0,0,0,  /* 5,6 */
    1,0,0,0,1,0,0,0,  /* 7,8 */
    1,0,0,0,0,0,0,0,  /* 9,10M */
    0,0,0,0,0,0,0,0    /* 11M,12M */
  };
  
  PROGMEM prog_uchar thirty_six_minus_two_two_two[] = \
  { /* 36-2-2-2  */
    1,0,0,0,0,0,1,0,0,0, \
    0,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,0,0,0,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, 
    1,0
  };
  
  PROGMEM prog_uchar thirty_six_minus_two_two_two_with_cam[] = \
  { /* 36-2-2-2 with cam  */
    1,2,2,2,2,2,1,0,0,0, \
    0,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, \
    1,0,1,2,2,2,2,2,1,0, \
    1,0,1,0,1,0,1,0,1,0, \
    1,0,1,0,1,0,1,0,1,0, 
    1,0
  };
  
  PROGMEM prog_uchar fourty_two_hundred_wheel[] = \
  { /* 4200 wheel http://msextra.com/doc/triggers/4200_timing.pdf */
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1, \
   1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1
   
  };

 /* Mazda F3 36-1 with cam */
 PROGMEM prog_uchar thirty_six_minus_one_with_cam_fe3[] = \
 { /* 36-1 */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 0-55 deg */
   1,0,1,0,1,0,3,2,3,0,1,0, /* 60-115 deg  cam tooth at 90 deg crank for 15 crank degrees */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 120-175 deg */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 180-235 deg */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 240-295 deg */
   1,0,1,0,1,0,1,0,1,0,0,0, /* 300-355 deg Crank missing tooth at end */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 360-415 deg */
   3,2,3,0,1,0,3,2,3,0,1,0, /* 420-475 deg 2 more cam teeth at 420 and 450 deg, 15 crank degrees each */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 480-535 deg */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 540-595 deg */
   1,0,1,0,1,0,1,0,1,0,1,0, /* 600-655 deg */
   1,0,1,0,1,0,1,0,1,0,1,0  /* 660-715 deg */
  }; 
  
  /* Mitsubishi 6g72 crank/cam */
  PROGMEM prog_uchar six_g_seventy_two_with_cam[] = \
  { /* Mitsubishi 6g72 */
    3,3,3,3,3,3,3,3,3,2,
    2,2,2,2,0,0,0,0,0,0,
    0,0,0,1,1,1,1,1,1,1,
    3,3,3,2,2,2,2,2,0,0,
    0,0,0,0,0,0,0,1,1,1,
    1,1,1,1,1,1,1,0,0,0,
    0,0,0,0,0,0,0,0,2,2,
    2,3,3,3,3,3,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,
    1,1,3,3,3,2,2,2,2,2,
    0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,0,0,
    0,2,2,3
  };
   
  #endif
