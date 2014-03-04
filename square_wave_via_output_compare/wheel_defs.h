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
  
  /* Wheel types we know about...
   * This enumerations is the INDEX into the Wheels[] array of structures
   * defined in main file. That struct contains pointers to the following:
   * wheel name in a user friendly string
   * pointer to the wheel edge array used by the ISR
   * RPM scaling factor (num_edges/120 for crank wheels)
   * Number of edges in the edge array above, needed by the ISR 
   */
 typedef enum { 
   DIZZY_FOUR_CYLINDER,  /* 2 evenly spaced teeth */
   DIZZY_SIX_CYLINDER,   /* 3 evenly spaced teeth */
   DIZZY_EIGHT_CYLINDER, /* 4 evenly spaced teeth */
   SIXTY_MINUS_TWO,      /* 60-2 crank only */
   SIXTY_MINUS_TWO_WITH_CAM, /* 60-2 with 2nd trigger on cam */
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
   SIX_G_SEVENTY_TWO_WITH_CAM, /* Mitsubishi DOHC CAS and TCDS 6G72 */
   BUELL_ODDFIRE_CAM,     /* Buell 45 deg cam wheel */
   GM_LS1_CRANK_AND_CAM,  /* GM LS1 24 tooth with cam */
   LOTUS_THIRTY_SIX_MINUS_ONE_ONE_ONE_ONE, /* Lotus crank wheel 36-1-1-1-1 */
   HONDA_RC51_WITH_CAM,   /* Honda oddfire 90 deg V-twin */
   THIRTY_SIX_MINUS_ONE_WITH_SECOND_TRIGGER, /* From jimstim */
   MAX_WHEELS,
 }WheelType;

/* Name strings for EACH wheel type, for serial UI */
 PROGMEM prog_char dizzy_four_cylinder_friendly_name[] = "4 cylinder dizzy";
 PROGMEM prog_char dizzy_six_cylinder_friendly_name[] = "6 cylinder dizzy";
 PROGMEM prog_char dizzy_eight_cylinder_friendly_name[] = "8 cylinder dizzy";
 PROGMEM prog_char sixty_minus_two_friendly_name[] = "60-2 crank only";
 PROGMEM prog_char sixty_minus_two_with_cam_friendly_name[] = "60-2 crank and cam";
 PROGMEM prog_char thirty_six_minus_one_friendly_name[] = "36-1 crank only";
 PROGMEM prog_char four_minus_one_with_cam_friendly_name[] = "4-1 crank wheel with cam";
 PROGMEM prog_char eight_minus_one_friendly_name[] = "8-1 crank only (R6)";
 PROGMEM prog_char six_minus_one_with_cam_friendly_name[] = "6-1 crank with cam";
 PROGMEM prog_char twelve_minus_one_with_cam_friendly_name[] = "12-1 crank with cam";
 PROGMEM prog_char fourty_minus_one_friendly_name[] = "40-1 crank only (Ford V10)";
 PROGMEM prog_char dizzy_trigger_return_friendly_name[] = "Distributor style 50deg off, 40 deg on";
 PROGMEM prog_char oddfire_vr_friendly_name[] = "odd fire 90 deg pattern 0 and 135 pulses";
 PROGMEM prog_char optispark_lt1_friendly_name[] = "GM OptiSpark LT1 360 and 8";
 PROGMEM prog_char twelve_minus_three_friendly_name[] = "12-3 oddball";
 PROGMEM prog_char thirty_six_minus_two_two_two_friendly_name[] = "36-2-2-2 Crank only";
 PROGMEM prog_char thirty_six_minus_two_two_two_with_cam_friendly_name[] = "36-2-2-2 Crank and cam";
 PROGMEM prog_char fourty_two_hundred_wheel_friendly_name[] = "GM 4200 crank wheel";
 PROGMEM prog_char thirty_six_minus_one_with_cam_fe3_friendly_name[] = "Mazda FE3 36-1 with cam";
 PROGMEM prog_char six_g_seventy_two_with_cam_friendly_name[] = "Mitsubishi 6g72 with cam";
 PROGMEM prog_char buell_oddfire_cam_friendly_name[] = "Buell Oddfire CAM wheel";
 PROGMEM prog_char gm_ls1_crank_and_cam_friendly_name[] = "GM LS1 crank and cam";
 PROGMEM prog_char lotus_thirty_six_minus_one_one_one_one_friendly_name[] = "Odd Lotus 36-1-1-1-1 flywheel";
 PROGMEM prog_char honda_rc51_with_cam_friendly_name[] = "Honda RC51 with cam";
 PROGMEM prog_char thirty_six_minus_one_with_second_trigger_friendly_name[] = "36-1 crank with 2nd trigger on teeth 33-34";

 /* Very simple 50% duty cycle */
 PROGMEM prog_uchar dizzy_four_cylinder[] = 
   { /* dizzy 4 cylinder */
     1,0,1,0 /* two pulses per crank revolution (one per cylinder) */
   };
   
 /* Very simple 50% duty cycle */
 PROGMEM prog_uchar dizzy_six_cylinder[] = 
   { /* dizzy 6 cylinder */
     1,0,1,0,1,0 /* three pulses per crank revolution (one per cylinder) */
   };
   
 /* Very simple 50% duty cycle */
 PROGMEM prog_uchar dizzy_eight_cylinder[] = 
   { /* dizzy 8 cyl */
     1,0,1,0,1,0,1,0 /* four pulses per crank revolution (one per cylinder) */
   };
   
 /* Standard bosch 60-2 pattern, 50% duty cyctle during normal teeth */
 PROGMEM prog_uchar sixty_minus_two[] = 
   { /* 60-2 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 1-5 */ 
     1,0,1,0,1,0,1,0,1,0,  /* teeth 6-10 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 26-30 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 31-35 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 36-40 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 41-45 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 46-50 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 51-55 */
     1,0,1,0,1,0,0,0,0,0   /* teeth 56-58 and 59-60 MISSING */
   };
 
 /* Bosch 60-2 pattern with 2nd trigger on rotation 2, 
  * 50% duty cyctle during normal teeth */
 PROGMEM prog_uchar sixty_minus_two_with_cam[] = 
   { /* 60-2 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 1-5 */ 
     1,0,1,0,1,0,1,0,1,0,  /* teeth 6-10 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 26-30 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 31-35 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 36-40 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 41-45 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 46-50 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 51-55 */
     1,0,1,0,1,0,0,0,0,0,  /* teeth 56-58 and 59-60 MISSING */
     1,0,1,0,1,0,1,0,1,0,  /* Second revolution teeth 1-5 */ 
     1,0,1,0,1,0,1,0,1,0,  /* teeth 6-10 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 26-30 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 31-35 */
     1,2,1,0,1,0,1,0,1,0,  /* teeth 36-40, Cam trigger on latter half of 36th */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 41-45 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 46-50 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 51-55 */
     1,0,1,0,1,0,0,0,0,0   /* teeth 56-58 and 59-60 MISSING */
   };
 
 /* Standard ford/mazda and aftermarket 36-1 pattern, 50% duty cyctle during normal teeth */  
 PROGMEM prog_uchar thirty_six_minus_one[] = 
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 1-5 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 6-10 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 26-30 */
     1,0,1,0,1,0,1,0,1,0,  /* teeth 31-35 */
     0,0                   /* MISSING 36th tooth  */
   }; 
   
 /* 4-1 crank signal 50% duty cycle with Cam tooth enabled during the second rotation prior to tooth 2 */
 PROGMEM prog_uchar four_minus_one_with_cam[] = 
   { /* 4-1 with cam */
     0,1,0,1,0,1,0,0,  /* Teeth 1-3, then MISSING */
     0,1,2,1,0,1,0,0   /* Tooth 5, 2nd trigger on cam between 5 and 6m then 6 and 7 and MISSING 8th */
   };
   
 /* Yamaha R6 crank trigger 8 teeth missing one, (22.5deg low, 22.5deg high) 50% duty cycle during normal teeth */
 PROGMEM prog_uchar eight_minus_one[] = 
   { /* 8-1 */
     0,1,0,1,0,1,0,1,  /* Teeth 1-4 */
     0,1,0,1,0,1,0,0   /* Teeth 5-7, then MISSING */
   };
  
  /* 40deg low, 20 deg high per tooth, cam signal on second rotation during 40deg low portion of 3rd tooth */
 PROGMEM prog_uchar six_minus_one_with_cam[] = 
   { /* 6-1 with cam */
     0,0,1,0,0,1,0,0,1,  /* Teeth 1-3 */
     0,0,1,0,0,1,0,0,0,  /* Teeth 4 and 5 and MISSING 6th */
     0,0,1,0,0,1,2,2,1,  /* 2nd rev teeth 7 and 8, then 2nd trigger on cam between 8 and 9 */
     0,0,1,0,0,1,0,0,0   /* teeth 10 and 11 then missing 12th */
   };
  
  /* 25 deg low, 5 deg high, #12 is missing,  cam is high for 25 deg on second crank rotation just after tooth 21 (9) */
 PROGMEM prog_uchar twelve_minus_one_with_cam[] = 
   { /* 12-1 with cam */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 1 and 2 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 3 and 4 */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 5 and 6 */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 7 and 8 */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 9 and 10 */
	 0,0,0,0,0,1,0,0,0,0,0,0, /* Tooth 11 and MISSING twelth */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* 2nd rotation: Teeth 13 and 14 */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 15 and 16 */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 17 and 18 */
	 0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 19 and 20 */
	 0,0,0,0,0,1,2,2,2,2,2,1, /* Tooth 21 and 22,  2nd trigger on cam between teeth 21 and 22 for 25 deg */
	 0,0,0,0,0,1,0,0,0,0,0,0  /* Totth 23 and MISSING 24th */
   };
   
  /* Ford V10 version of EDIS with 40 teeth instead of 36, 50% duty cycle during normal teeth.. */
 PROGMEM prog_uchar fourty_minus_one[] = 
   { /* 40-1 */
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 1-5 */
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 6-10 */ 
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 11-15 */ 
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 16-20 */ 
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 21-25 */ 
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 26-30 */ 
     0,1,0,1,0,1,0,1,0,1,  /* Teeth 31-35 */ 
     0,1,0,1,0,1,0,1,0,0   /* Teeth 36-39 and MISSING 40th tooth */ 
   };
  
  /* 50deg off, 40 deg on dissy style signal */
  PROGMEM prog_uchar dizzy_trigger_return[] = 
    { /* dizzy trigger return */
      0,0,0,0,0,1,1,1,1l  /* Simple off/on signal, 50deg off, 40 deg on */
    };
   
  /* Oddfire V twin  135/225 split */
  PROGMEM prog_uchar oddfire_vr[] = 
    { /* Oddfire VR */
      1,0,0,0,0,0,0,0,0,1,0,0  /* Tooth 1 and 2 at 0 deg and 135 deg, then 225 off */
      0,0,0,0,0,0,0,0,0,0,0,0 
    };
  
  /* GM LT1 360 and 8 wheel, see http://powerefi.com/files/opti-LT1-count.JPG */
  PROGMEM prog_uchar optispark_lt1[] = 
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
  
  PROGMEM prog_uchar twelve_minus_three[] = 
    { /* 12-3, http://www.msextra.com/doc/triggers/12_3_wheel_133.jpg */
      1,0,0,0,1,0,0,0,  /* Teeth 1-2 */
      1,0,0,0,1,0,0,0,  /* Teeth 3-4 */
      1,0,0,0,1,0,0,0,  /* Teeth 5-6 */
      1,0,0,0,1,0,0,0,  /* Teeth 7-8 */
      1,0,0,0,0,0,0,0,  /* Tooth 9 and MISSING 10th */
      0,0,0,0,0,0,0,0   /* MISSING Teeth 11-12 */
    };
  
  PROGMEM prog_uchar thirty_six_minus_two_two_two[] = 
    { /* 36-2-2-2  */
      1,0,0,0,0,0,1,0,0,0, /* Tooth 1, then MISSING 2 and 3, then 4 and MISSING 5 */
      0,0,1,0,1,0,1,0,1,0, /* Missing Tooth 6, then 7-10 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */ 
      1,0,1,0,0,0,0,0,1,0, /* Teeth 21-25 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 31-35 */
      1,0                  /* 36th Tooth
    };
  
  PROGMEM prog_uchar thirty_six_minus_two_two_two_with_cam[] = 
    { /* 36-2-2-2 with cam  */
      1,2,2,2,2,2,1,0,0,0, /* Tooth one, missing teeth 2,3 and 5, 2nd trigger during teeth 2 and 3 */
      0,0,1,0,1,0,1,0,1,0, /* Missing tooth 6, then 7-10 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */ 
      1,0,1,2,2,2,2,2,1,0, /* Teeth 21-2sing 23-24, then 25, 2nd trigger during teeth 23 and 24 */ 
      1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */ 
      1,0,1,0,1,0,1,0,1,0, /* Teeth 31-35 */ 
      1,0                  /* 36th Tooth */
    };
  
  PROGMEM prog_uchar fourty_two_hundred_wheel[] = 
    { /* 4200 wheel http://msextra.com/doc/triggers/4200_timing.pdf */
		/* 55 deg high, 
		 * 5 deg low, 
		 * 55 deg high, 
		 * 5 deg low, 
		 * 5 deg high, 
		 * 5 deg low,
		 * 45 deg high,
		 * 5 deg low,
		 * 55 deg high,
		 * 5 deg low,
		 * 65 deg high,
		 * 5 deg low,
		 * 45 deg high
		 * 5 deg low, (360 degreees ) */
      1,1,1,1,1,1,1,1,1,1, 
	  1,0,1,1,1,1,1,1,1,1, 
	  1,1,1,0,1,0,1,1,1,1, 
	  1,1,1,1,1,0,1,1,1,1,
	  1,1,1,1,1,1,1,0,1,1,
	  1,1,1,1,1,1,1,1,1,1,
	  1,0,1,1,1,1,1,1,1,1,
	  1,0
  };

 /* Mazda F3 36-1 with cam */
 PROGMEM prog_uchar thirty_six_minus_one_with_cam_fe3[] = 
   { /* 36-1 with cam, 3 cam teeth, 2 180deg from each other */
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
     1,0,1,0,1,0,1,0,1,0,0,0  /* 660-715 deg Crank missing tooth at end */
   }; 
  
  /* Mitsubishi 6g72 crank/cam */
  PROGMEM prog_uchar six_g_seventy_two_with_cam[] = 
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
   
  PROGMEM prog_uchar buell_oddfire_cam[] = 
    { /* Buell oddfire cam wheel */
      0,0,0,0,0,0,0,0,0,0,
      0,1,1,1,1,0,0,0,0,0,
      0,1,1,1,1,0,0,0,0,0,
      0,1,1,1,1,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,1,1,1,1,
      0,0,0,0,0,0,1,1,1,1,
      0,0,0,0,0,0,1,1,1,1
    };
  
  PROGMEM prog_uchar gm_ls1_crank_and_cam[] = 
    { /* GM LS1 24 tooth crank snd 1 tooth cam */
      0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,
      1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,
      1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
      0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
      1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
      0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
      0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
      1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
      0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
      0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
      1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
      0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
      0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,
      1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
      0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
      0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,
      0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,
      0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
      2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,2,2,2,3,3, /* Second rotation with */
      3,3,3,3,3,3,3,3,3,3,2,2,2,3,3,3,3,3,3,3, /* Cam tooth enabled */
      3,3,3,3,3,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,
      2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,3,3,
      3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,
      2,2,3,3,3,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,
      2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,3,3,
      3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,
      2,2,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,
      2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,3,3,
      3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,
      2,2,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,
      2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,2,2,2,3,3,
      3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,
      2,2,3,3,3,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,
      2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,2,2,2,2,2,
      2,2,2,2,2,2,2,3,3,3,2,2,2,2,2,2,2,2,2,2,
      2,2,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,
    };
  
 /* Lotus 36-1-1-1-1 wheel, missing teeth at
  * 36,32,17 and 14
  */
 PROGMEM prog_uchar lotus_thirty_six_minus_one_one_one_one[] = 
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0, 
     1,0,1,0,1,0,1,0,1,0, 
     1,0,1,0,1,0,0,0,1,0, 
     1,0,0,0,1,0,1,0,1,0, 
     1,0,1,0,1,0,1,0,1,0, 
     1,0,1,0,1,0,1,0,1,0, 
     1,0,0,0,1,0,1,0,1,0, 
     0,0
   }; 
 PROGMEM prog_uchar honda_rc51_with_cam[] = 
   { /* Honda RC51 oddfire 90deg Vtwin with cam */
      0,1,0,1,0,1,0,1,0,3, 
      0,1,0,1,0,1,0,1,0,1, 
      0,1,0,1,0,3,0,1,0,3, 
      0,1,0,1,0,1,0,1,0,1, 
      0,1,0,1,0,1,0,1
   };

 /* 36-1 with second trigger pulse across teeth 33-34 on first rotation */
 PROGMEM prog_uchar thirty_six_minus_one_with_second_trigger[] = 
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 1-5 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 6-10  */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */
     1,0,1,0,3,2,3,2,1,0, /* Teeth 31-35, second trigger during 33-34 */
     0,0,                 /* 36th tooth MISSING */
     1,0,1,0,1,0,1,0,1,0, /* Revolution 2  Teeth 1-5 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 6-10 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */
     1,0,1,0,1,0,1,0,1,0, /* Teeth 31-35 */
     0,0                  /* 36th MISSING tooth
   }; 
   
  #endif
