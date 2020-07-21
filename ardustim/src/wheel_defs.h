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
   TWENTY_FOUR_MINUS_ONE,
   FOUR_MINUS_ONE_WITH_CAM, /* 4-1 crank + cam */
   EIGHT_MINUS_ONE,       /* 8-1 crank only */
   SIX_MINUS_ONE_WITH_CAM,/* 6-1 crank + cam */
   TWELVE_MINUS_ONE_WITH_CAM, /* 12-1 crank + cam */
   FOURTY_MINUS_ONE,      /* Ford V-10 40-1 crank only */
   DIZZY_FOUR_TRIGGER_RETURN,  /* dizzy 4 cylinder signal, 40deg on 50 deg off */
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
   THIRTY_SIX_MINUS_ONE_PLUS_ONE_WITH_CAM_NGC4, /* From jimstim 36-1+1 wheel #5 4 cyl chrysler? */
   WEBER_IAW_WITH_CAM, /* From jimstim IAW weber-marelli */
   FIAT_ONE_POINT_EIGHT_SIXTEEN_VALVE_WITH_CAM, /* Fiat 1.8 16V from jimstim */
   THREE_SIXTY_NISSAN_CAS, /*from jimstim 360 tooth cas with 6 slots */
   TWENTY_FOUR_MINUS_TWO_WITH_SECOND_TRIGGER, /* Mazda CAS 24-1 inner ring single pulse outer ring */
   YAMAHA_EIGHT_TOOTH_WITH_CAM, /* 02-03 Yamaha R1, seank */
   GM_FOUR_TOOTH_WITH_CAM, /* GM 4 even crank with half moon cam */
   GM_SIX_TOOTH_WITH_CAM, /* GM 4 even crank with half moon cam */
   GM_EIGHT_TOOTH_WITH_CAM, /* GM 4 even crank with half moon cam */
   VOLVO_D12ACD_WITH_CAM, /* Volvo Diesel d12[acd] with cam (alex32 on forums.libreems.org */
   MAZDA_THIRTY_SIX_MINUS_TWO_TWO_TWO_WITH_SIX_TOOTH_CAM,
   MITSUBISH_4g63_4_2,
   AUDI_135_WITH_CAM,
   HONDA_D17_NO_CAM,
   MAZDA_323_AU,
   DAIHATSU_3CYL,
   MIATA_9905,
   TWELVE_WITH_CAM, //12 evenly spaced crank teeth and a single cam tooth
   TWENTY_FOUR_WITH_CAM, //24 evenly spaced crank teeth and a single cam tooth
   SUBARU_SIX_SEVEN,      /* Subaru 6 crank, 7 cam */
   GM_7X,                 /* GM 7X pattern. 6 even teeth with 1 extra uneven tooth */
   FOUR_TWENTY_A,         /* DSM 420a */
   FORD_ST170,            /* Ford ST170 */
   MITSUBISHI_3A92,        /* Mitsubishi 3cylinder 3A92 */
   MAX_WHEELS,
 }WheelType;

/* Name strings for EACH wheel type, for serial UI */
 const char dizzy_four_cylinder_friendly_name[] PROGMEM = "4 cylinder dizzy";
 const char dizzy_six_cylinder_friendly_name[] PROGMEM = "6 cylinder dizzy";
 const char dizzy_eight_cylinder_friendly_name[] PROGMEM = "8 cylinder dizzy";
 const char sixty_minus_two_friendly_name[] PROGMEM = "60-2 crank only";
 const char sixty_minus_two_with_cam_friendly_name[] PROGMEM = "60-2 crank and cam";
 const char thirty_six_minus_one_friendly_name[] PROGMEM = "36-1 crank only";
 const char twenty_four_minus_one_friendly_name[] PROGMEM = "24-1 crank only";
 const char four_minus_one_with_cam_friendly_name[] PROGMEM = "4-1 crank wheel with cam";
 const char eight_minus_one_friendly_name[] PROGMEM = "8-1 crank only (R6)";
 const char six_minus_one_with_cam_friendly_name[] PROGMEM = "6-1 crank with cam";
 const char twelve_minus_one_with_cam_friendly_name[] PROGMEM = "12-1 crank with cam";
 const char fourty_minus_one_friendly_name[] PROGMEM = "40-1 crank only (Ford V10)";
 const char dizzy_four_trigger_return_friendly_name[] PROGMEM = "Distributor style 4 cyl 50deg off, 40 deg on";
 const char oddfire_vr_friendly_name[] PROGMEM = "odd fire 90 deg pattern 0 and 135 pulses";
 const char optispark_lt1_friendly_name[] PROGMEM = "GM OptiSpark LT1 360 and 8";
 const char twelve_minus_three_friendly_name[] PROGMEM = "12-3 oddball";
 const char thirty_six_minus_two_two_two_friendly_name[] PROGMEM = "36-2-2-2 Crank only";
 const char thirty_six_minus_two_two_two_with_cam_friendly_name[] PROGMEM = "36-2-2-2 Crank and cam";
 const char fourty_two_hundred_wheel_friendly_name[] PROGMEM = "GM 4200 crank wheel";
 const char thirty_six_minus_one_with_cam_fe3_friendly_name[] PROGMEM = "Mazda FE3 36-1 with cam";
 const char six_g_seventy_two_with_cam_friendly_name[] PROGMEM = "Mitsubishi 6g72 with cam";
 const char buell_oddfire_cam_friendly_name[] PROGMEM = "Buell Oddfire CAM wheel";
 const char gm_ls1_crank_and_cam_friendly_name[] PROGMEM = "GM LS1 crank and cam";
 const char lotus_thirty_six_minus_one_one_one_one_friendly_name[] PROGMEM = "Odd Lotus 36-1-1-1-1 flywheel";
 const char honda_rc51_with_cam_friendly_name[] PROGMEM = "Honda RC51 with cam";
 const char thirty_six_minus_one_with_second_trigger_friendly_name[] PROGMEM = "36-1 crank with 2nd trigger on teeth 33-34";
 const char thirty_six_minus_one_plus_one_with_cam_ngc4_friendly_name[] PROGMEM = "36-1+1 crank with cam pattern NGC4";
 const char weber_iaw_with_cam_friendly_name[] PROGMEM = "Weber-Marelli 8 crank+2 cam pattern";
 const char fiat_one_point_eight_sixteen_valve_with_cam_friendly_name[] PROGMEM = "Fiat 1.8 16V crank and cam";
 const char three_sixty_nissan_cas_friendly_name[] PROGMEM = "Nissan 360 CAS with 6 slots";
 const char twenty_four_minus_two_with_second_trigger_friendly_name[] PROGMEM = "Mazda CAS 24-2 with single pulse outer ring";
 const char yamaha_eight_tooth_with_cam_friendly_name[] PROGMEM = "Yamaha 2002-03 R1 8 even-tooth crank with 1 tooth cam";
 const char gm_four_tooth_with_cam_friendly_name[] PROGMEM = "GM 4 even-tooth crank with 1 tooth cam";
 const char gm_six_tooth_with_cam_friendly_name[] PROGMEM = "GM 6 even-tooth crank with 1 tooth cam";
 const char gm_eight_tooth_with_cam_friendly_name[] PROGMEM = "GM 8 even-tooth crank with 1 tooth cam";
 const char volvo_d12acd_with_cam_friendly_name[] PROGMEM = "Volvo d12[acd] crank with 7 tooth cam";
 const char mazda_thirty_six_minus_two_two_two_with_six_tooth_cam_friendly_name[] PROGMEM = "Mazda 36-2-2-2 with 6 tooht cam";
 const char mitsubishi_4g63_4_2_friendly_name[] PROGMEM = "Mitsubishi 4g63 aka 4/2 crank and cam";
 const char audi_135_with_cam_friendly_name[] PROGMEM = "Audi 135 tooth crank and cam";
 const char honda_d17_no_cam_friendly_name[] PROGMEM = "Honda D17 Crank (12+1)";
 const char mazda_323_au_friendly_name[] PROGMEM = "Mazda 323 AU version";
 const char daihatsu_3cyl_friendly_name[] PROGMEM = "Daihatsu 3+1 distributor (3 cylinders)";
 const char miata_9905_friendly_name[] PROGMEM = "Miata 99-05";
 const char twelve_with_cam_friendly_name[] PROGMEM = "12/1 (12 crank with cam)";
 const char twenty_four_with_cam_friendly_name[] PROGMEM = "24/1 (24 crank with cam)";
 const char subaru_six_seven_name_friendly_name[] PROGMEM = "Subaru 6/7 crank and cam";
 const char gm_seven_x_friendly_name[] PROGMEM = "GM 7X";
 const char four_twenty_a_friendly_name[] PROGMEM = "DSM 420a";
 const char ford_st170_friendly_name[] PROGMEM = "Ford ST170";
 const char mitsubishi_3A92_friendly_name[] PROGMEM = "Mitsubishi 3A92";
 

 /* Very simple 50% duty cycle */
 const unsigned char dizzy_four_cylinder[] PROGMEM = 
   { /* dizzy 4 cylinder */
     1,0,1,0 /* two pulses per crank revolution (one per cylinder) */
   };
   
 /* Very simple 50% duty cycle */
 const unsigned char dizzy_six_cylinder[] PROGMEM = 
   { /* dizzy 6 cylinder */
     1,0,1,0,1,0 /* three pulses per crank revolution (one per cylinder) */
   };
   
 /* Very simple 50% duty cycle */
 const unsigned char dizzy_eight_cylinder[] PROGMEM = 
   { /* dizzy 8 cyl */
     1,0,1,0,1,0,1,0 /* four pulses per crank revolution (one per cylinder) */
   };
   
 /* Standard bosch 60-2 pattern, 50% duty cyctle during normal teeth */
 const unsigned char sixty_minus_two[] PROGMEM = 
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
 const unsigned char sixty_minus_two_with_cam[] PROGMEM = 
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
 const unsigned char thirty_six_minus_one[] PROGMEM = 
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
   
  /* Standard ford/mazda and aftermarket 36-1 pattern, 50% duty cyctle during normal teeth */  
 const unsigned char twenty_four_minus_one[] PROGMEM = 
   { /* 36-1 */
     1,0,1,0,1,0,  /* teeth 1-3 */
     1,0,1,0,1,0,  /* teeth 4-6 */
     1,0,1,0,1,0,  /* teeth 7-9 */
     1,0,1,0,1,0,  /* teeth 10-12 */
     1,0,1,0,1,0,  /* teeth 13-15 */
     1,0,1,0,1,0,  /* teeth 16-18 */
     1,0,1,0,1,0,  /* teeth 19-21 */
     1,0,1,0,0,0,  /* teeth 22-21 */
   }; 
   
 /* 4-1 crank signal 50% duty cycle with Cam tooth enabled during the second rotation prior to tooth 2 */
 const unsigned char four_minus_one_with_cam[] PROGMEM = 
   { /* 4-1 with cam */
     0,1,0,1,0,1,0,0,  /* Teeth 1-3, then MISSING */
     0,1,2,1,0,1,0,0   /* Tooth 5, 2nd trigger on cam between 5 and 6 
						  then 6 and 7 and MISSING 8th */
   };
   
 /* Yamaha R6 crank trigger 8 teeth missing one, (22.5deg low, 22.5deg high) 50% duty cycle during normal teeth */
 const unsigned char eight_minus_one[] PROGMEM = 
   { /* 8-1 */
     0,1,0,1,0,1,0,1,  /* Teeth 1-4 */
     0,1,0,1,0,1,0,0   /* Teeth 5-7, then MISSING */
   };
  
  /* 40deg low, 20 deg high per tooth, cam signal on second rotation during 40deg low portion of 3rd tooth */
 const unsigned char six_minus_one_with_cam[] PROGMEM = 
   { /* 6-1 with cam */
     0,0,1,0,0,1,0,0,1,  /* Teeth 1-3 */
     0,0,1,0,0,1,0,0,0,  /* Teeth 4 and 5 and MISSING 6th */
     0,0,1,0,0,1,2,2,1,  /* 2nd rev teeth 7 and 8, then 2nd trigger on cam between 8 and 9 */
     0,0,1,0,0,1,0,0,0   /* teeth 10 and 11 then missing 12th */
   };
  
  /* 25 deg low, 5 deg high, #12 is missing,  cam is high for 25 deg on second crank rotation just after tooth 21 (9) */
 const unsigned char twelve_minus_one_with_cam[] PROGMEM = 
   { /* 12-1 with cam */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 1 and 2 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 3 and 4 */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 5 and 6 */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 7 and 8 */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 9 and 10 */
	   0,0,0,0,0,1,0,0,0,0,0,0, /* Tooth 11 and MISSING twelth */
     //0,0,0,0,0,1,0,0,0,0,0,1, /* Tooth 11 and WITHOUT MISSING twelth */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* 2nd rotation: Teeth 13 and 14 */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 15 and 16 */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 17 and 18 */
	   0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 19 and 20 */
	   0,0,0,0,0,1,2,2,2,2,2,1, /* Tooth 21 and 22,  2nd trigger on cam between teeth 21 and 22 for 25 deg */
	   0,0,0,0,0,1,0,0,0,0,0,0  /* Totth 23 and MISSING 24th */
     //0,0,0,0,0,1,0,0,0,0,0,1  /* Totth 23 and WITHOUT MISSING 24th */
   };
   
  /* Ford V10 version of EDIS with 40 teeth instead of 36, 50% duty cycle during normal teeth.. */
 const unsigned char fourty_minus_one[] PROGMEM = 
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
  const unsigned char dizzy_four_trigger_return[] PROGMEM = 
    { /* dizzy trigger return */
      0,0,0,0,0,1,1,1,1l  /* Simple off/on signal, 50deg off, 40 deg on */
    };
   
  /* Oddfire V twin  135/225 split */
  const unsigned char oddfire_vr[] PROGMEM = 
    { /* Oddfire VR */
      1,0,0,0,0,0,0,0,0,1,0,0, /* Tooth 1 and 2 at 0 deg and 135 deg, then 225 off */
      0,0,0,0,0,0,0,0,0,0,0,0 
    };
  
  /* GM LT1 360 and 8 wheel, see http://powerefi.com/files/opti-LT1-count.JPG */
  const unsigned char optispark_lt1[] PROGMEM = 
    { /* Optispark 360 outside teeth, 8 varying inside teeth */
    /* 1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 0-30 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 31-60 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,2,3,2,3, /* 61-90 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 91-120 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
      2,3,2,3,2,3,2,3,2,3,0,1,0,1,0,1,0,1,0,1, /* 121-150 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,2,3,2,3, /* 151-180 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 181-210 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, 
      0,1,0,1,0,1,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
      2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 211-240 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,2,3,2,3, /* 241-270 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 271-300 */
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
      2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 301-330 */
      2,3,2,3,2,3,2,3,2,3,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
      0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,2,3,2,3  /* 331-360 */
  };
  
  const unsigned char twelve_minus_three[] PROGMEM = 
    { /* 12-3, http://www.msextra.com/doc/triggers/12_3_wheel_133.jpg */
      1,0,0,0,1,0,0,0,  /* Teeth 1-2 */
      1,0,0,0,1,0,0,0,  /* Teeth 3-4 */
      1,0,0,0,1,0,0,0,  /* Teeth 5-6 */
      1,0,0,0,1,0,0,0,  /* Teeth 7-8 */
      1,0,0,0,0,0,0,0,  /* Tooth 9 and MISSING 10th */
      0,0,0,0,0,0,0,0   /* MISSING Teeth 11-12 */
    };
  
  const unsigned char thirty_six_minus_two_two_two[] PROGMEM = 
    {
      1,0,1,0,1,0,1,0,1,0,
      1,0,1,0,1,0,1,0,1,0,
      1,0,1,0,1,0,0,0,0,0,
      1,0,0,0,0,0,1,0,1,0,
      1,0,1,0,1,0,1,0,1,0,
      1,0,1,0,1,0,1,0,1,0,
      1,0,0,0,0,0,1,0,1,0,
      1,0 
    };
  
  const unsigned char thirty_six_minus_two_two_two_with_cam[] PROGMEM = 
    { /* 36-2-2-2 with cam  */
      1,0,0,2,0,0,1,0,0,0, /* Tooth one, missing teeth 2,3 and 5, 2nd trigger during teeth 2 and 3 */
      0,0,1,0,1,0,1,0,1,0, /* Missing tooth 6, then 7-10 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */ 
      1,0,1,0,0,2,0,0,1,0, /* Teeth 21-2sing 23-24, then 25, 2nd trigger during teeth 23 and 24 */ 
      1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */ 
      1,0,1,0,1,0,1,0,1,0, /* Teeth 31-35 */ 
      1,0,                  /* 36th Tooth */
      1,0,0,0,0,0,1,0,0,0, /* Tooth one, missing teeth 2,3 and 5 */
      0,0,1,0,1,2,1,0,1,0, /* Missing tooth 6, then 7-10 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 11-15 */
      1,0,1,0,1,0,1,0,1,0, /* Teeth 16-20 */ 
      1,0,1,0,0,0,0,0,1,0, /* Teeth 21-22 missing 23-24, then 25 */ 
      1,0,1,0,1,0,1,0,1,0, /* Teeth 26-30 */ 
      1,0,1,0,1,0,1,0,1,0, /* Teeth 31-35 */ 
      1,0                  /* 36th Tooth */
    };
   
  
  const unsigned char fourty_two_hundred_wheel[] PROGMEM = 
    { /* 4200 wheel http://msextra.com/doc/triggers/4200_timing.pdf */
		/* 55 deg high, 5 deg low, 55 deg high, 5 deg low,
		 * 5 deg high, 5 deg low, 45 deg high, 5 deg low,
		 * 55 deg high, 5 deg low, 65 deg high, 5 deg low,
		 * 45 deg high, 5 deg low, (360 degreees ) */
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
 const unsigned char thirty_six_minus_one_with_cam_fe3[] PROGMEM = 
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
  const unsigned char six_g_seventy_two_with_cam[] PROGMEM = 
    { /* Mitsubishi 6g72 */
	  /* Crank signal's are 50 deg wide, and one per cylinder
	   * Cam signals have 3 40 deg wide teeh and one 85 deg wide tooth
	   * Counting both From TDC#1
	   * Crank: 40 deg high, 70 deg low (repeats whole cycle)
	   * Cam: 70 deg high, 80 deg low, 40 deg high, 150 deg low,
	   * 40 deg high, 130 deg low, 40 deg high, 155 deg low 
	   */
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
   
  const unsigned char buell_oddfire_cam[] PROGMEM = 
    { /* Buell oddfire cam wheel */
	  /* Wheel is a cam wheel (degress are in crank degrees 
	   * 36 deg high, 54 deg low,
	   * 36 deg high, 54 deg low,
	   * (Rear at TDC) 36 deg high,
	   * 1889 deg low, 36 deg high
	   * 54 deg low, 36 deg high,
	   * 54 deg low, (Front at TDC),
	   * 36 deg high, 99 deg low
	   */
	  1,1,1,1,0,0,0,0,0,0, /* Tooth 1 (36 deg) and 54 deg space */
	  1,1,1,1,0,0,0,0,0,0, /* Tooth 2 (36 deg) and 54 deg space */
	  1,1,1,1,0,0,0,0,0,0, /* Tooth 3 (36 deg) and begin of 189 deg space */
	  0,0,0,0,0,0,0,0,0,0, /* Part of 189 deg space */
	  0,0,0,0,0,1,1,1,1,0, /* Tail of 189 deg space, 36 deg tooth, begin of 54 deg space */
	  0,0,0,0,0,1,1,1,1,0, /* Tail of 54 deg space, 36 deg tooth, begin of 54 deg space */
	  0,0,0,0,0,1,1,1,1,0, /* Tail of 54 deg space, last 36 deg tooth, begin of 99 deg space */
	  0,0,0,0,0,0,0,0,0,0  /* Tail of 99 deg space */
    };
  
  const unsigned char gm_ls1_crank_and_cam[] PROGMEM = 
    { /* GM LS1 24 tooth crank snd 1 tooth cam */
	  /* 12 deg low, 3 deg high, 3 deg low,
	   * 12 deg high, 3deg low, 12 deg high,
	   * 3 deg low, 12 deg high, 3 deg low,
	   * 12 deg high, 3 deg low, 12 deg high,
	   * 12 deg low, 3 deg high, 3 deg low,
	   * 12 deg high, 3 deg low, 12 deg high,
	   * 3 deg low, 12 deg high, 12 deg low,
	   * 3 deg high, 12 deg low, 3 deg high,
	   * 3 deg low, 12 deg high, 3 deg low,
	   * 12 deg high, 12 deg low, 3 deg high,
	   * 12 deg low, 3 deg high, 12 deg low,
	   * 3 deg high, 12 deg low, 3 deg high,
	   * 3 deg low, 12 deg high, 12 deg low,
	   * 3 deg high, 3 deg low, 12 deg high,
	   * 12 deg low, 3 deg high, 12 deg low,
	   * 3 deg high, 12 deg low, 3 deg high
	   * Second rotation is the SAME pattern
	   * with cam signal held high for 360 
	   * crank degrees */
      4,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,
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
 const unsigned char lotus_thirty_six_minus_one_one_one_one[] PROGMEM = 
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0, /* teeth 1-5 */
     1,0,1,0,1,0,1,0,1,0, /* teeth 6-10 */
     1,0,1,0,1,0,0,0,1,0, /* teeth 11-15 */
     1,0,0,0,1,0,1,0,1,0, /* teeth 16-20 */
     1,0,1,0,1,0,1,0,1,0, /* teeth 21-25 */
     1,0,1,0,1,0,1,0,1,0, /* teeth 26-30 */
     1,0,0,0,1,0,1,0,1,0, /* teeth 31-35 */
     0,0                  /* MISSING 36th tooth */
   }; 
 const unsigned char honda_rc51_with_cam[] PROGMEM = 
   { /* Honda RC51 oddfire 90deg Vtwin with cam */
      0,1,0,1,0,1,0,1,0,1, /* teeth 1-5 */
      0,3,0,1,0,1,0,1,0,1, /* teeth 6-10, cam triggers on tooth 6 */
      0,1,0,1,             /* Teeth 11-12 */
      0,1,0,1,0,1,0,3,0,1, /* 2nd rotation, teeth 1-5 (13-17), cam trigger on tooth 4(16)*/
      0,3,0,1,0,1,0,1,0,1, /* teeth 6-10 (18-22), cam trigger on tooth 18 */
      0,1,0,1              /* teeth 11-12, (23-24) */
   };

 /* 36-1 with second trigger pulse across teeth 33-34 on first rotation */
 const unsigned char thirty_six_minus_one_with_second_trigger[] PROGMEM = 
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
     1,0,1,0,1,0,1,0,0,0, /* Teeth 31-35 */
     0,0                  /* 36th MISSING tooth */
   }; 
   
 const unsigned char thirty_six_minus_one_plus_one_with_cam_ngc4[] PROGMEM = 
   { /* 36-1+1 NGC-4 needs 1 deg resolution, chrysler 2.0/2.4 engines
      * cam edges are at 26,62,98,134,170,314,350,368,422,458,494,530,674 and 710 dev
	  * crank is 36-1 with alarge window at one end 2.5 teeth cycles (25 deg) wide and alosed
	  * at 180deg apart for 2 teeth cycles (20 deg)
	  */
	  /* Crankshaft degrees
     1   3   5   7   9  11  13  15  17  19  21  23  25  27  29  31  33  35  37  39  41 */
	 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3, /* degrees */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 41-80 */
	 0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3, /* 81-120 */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 121-160 */
	 0,0,0,0,0,1,1,1,1,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 161-200 */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3, /* 201-240 */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3, /* 241-280 */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,0,0,1,1,1,1,1, /* 281-320 */
     0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,3,2,2,2,2,2,3,3,3,3,3, /* 321-360 */
	 2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 361-400 */
     0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3, /* 401-440 */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 441-480 */
	 0,0,0,0,0,1,1,1,1,1,0,0,0,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3, /* 481-520 */
	 2,2,2,2,2,3,3,3,3,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 521-560 */
	 0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 561-600 */
	 0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 601-640 */
	 0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,2,2,3,3,3,3,3, /* 641-680 */
	 2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,1,0,0,0,0,0,1,1,1,1,1  /* 681-720 */
   };


 const unsigned char weber_iaw_with_cam[] PROGMEM =
   { /*Weber marelli (Cosworth/Lancia) from jimstim
	   80 deg low, 10 deg high, Tooth 1
	   20 deg low, 45 deg cam pulse, 15 deg low, 10 deg high, Cam tooth 1 and crank tooth 2
	   80 deg low, 10 deg high, Crank tooth 3
	   20 deg low, 45 deg cam pulse, 15 deg low, 10 deg high, Cam tooth2 and crank tooth 4
	   80 deg low, 10 deg high, Crank tooth 5
	   80 deg low, 10 deg high  Crank tooth 6
	   80 deg low, 10 deg high, Crank tooth 7
	   80 deg low, 10 deg high  Crank tooth 8 */
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,1,1, /* Teeth 1 and 2 & cam1 */
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,1,1, /* Teeth 3 and 4 & cam2 */
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1, /* Teeth 5 and 6 */
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1  /* Teeth 7 and 8 */
   };

 const unsigned char fiat_one_point_eight_sixteen_valve_with_cam[] PROGMEM =
   {
     /* Starting from TDC #1 
      * Cam is high for 40 deg, low for 20, high 170deg, low for 170, high for 20, 
      * low for 170, then high for 130
      * Crank is low for 97, high for 5, low for 27, high for 5, low for 50, high for 5,
      * low for 102, high for 5, low for 27, high for 5, low for 50, high for 5,
      * low for 102, high for 5, low for 27, high for 5, low for 50, high for 5,
      * low for 102, high for 5, low for 27, high for 5, low for 50, high for 5, low for 5
      * http://msextra.com/doc/triggers/fiat1.8-16v.jpg
      */
      /* Crankshaft degrees
     1   3   5   7   9  11  13  15  17  19  21  23  25  27  29  31  33  35  37  39  */
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
     2,2,2,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
     2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
     2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2
   };

 const unsigned char three_sixty_nissan_cas[] PROGMEM = 
 /* This version has the 360 teeth on the cam
   {
     1,2,0,2,0,2,0,2,0,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 1-40 deg
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 41-80 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 81-120 deg 
     1,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 121-160 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 161-200 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 201-240 deg 
     1,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 241-280 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 281-320 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 321-360 deg 
     1,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,1,3,1,3,1,3, // 361-400 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 401-340 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 441-480 deg 
     1,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2, // 481-520 deg 
     0,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 521-560 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 561-600 deg 
     1,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2, // 601-640 deg 
     0,2,0,2,0,2,0,2,0,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 641-680 deg 
     1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3, // 681-720 deg 
   };*/
   {
     /* Home teeth every 120 degrees in increasing widths (8,16,24,32,40,48) */
     /* Crankshaft degrees
     1   3   5   7   9  11  13  15  17  19  21  23  25  27  29  31  33  35  37  39  */
     2,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 1-40 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 41-80 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 81-120 deg */
     2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 121-160 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 161-200 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 201-240 deg */
     2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 241-280 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 281-320 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 321-360 deg */
     2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3, /* 361-400 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 401-340 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 441-480 deg */
     2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 481-520 deg */
     0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 521-560 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 561-600 deg */
     2,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1, /* 601-640 deg */
     0,1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 641-680 deg */
     2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3, /* 681-720 deg */
   };

 const unsigned char twenty_four_minus_two_with_second_trigger[] PROGMEM = 
   {
	 /* See http://postimg.org/image/pcwkrxktx/, 24-2 inner ring, single outer pulse */
	 1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,3,2,2,2,2,2, /* 11 teeth then outer and missing */
	 3,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0, /* 11 more teeth then missing */
   };

 /* eight tooth with 1 tooth cam */
 const unsigned char yamaha_eight_tooth_with_cam[] PROGMEM = 
   { /* Yamaha R1 (02-03) 8 tooth crank with 1 tooth cam */
     0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1, /* Teeth 1-4, 11.25 deg per step */
     0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1, /* teeth 5-8 */
     0,2,2,3,2,0,0,1,0,0,0,1,0,0,0,1, /* Cam tooth on 9 */
     0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1  /* Teeth 13-16 */
   };

 /* 50% dutycle, 4 tooth + 1 cam */
 const unsigned char gm_four_tooth_with_cam[] PROGMEM = 
   { /* 4 cylinder with 1 cam pulse for 360 crank degrees */
     1,0,1,0,3,2,3,2 /* two pulses per crank revolution (one per cylinder) */
   };
   
 /* 50% dutycle, 6 tooth + 1 cam */
 const unsigned char gm_six_tooth_with_cam[] PROGMEM = 
   { /* 6 cylinder with 1 cam pulse for 360 crank degrees */
     1,0,1,0,1,0,3,2,3,2,3,2 /* three pulses per crank revolution (one per cylinder) */
   };
   
 /*  50% dutycle, 8 tooth + 1 cam */
 const unsigned char gm_eight_tooth_with_cam[] PROGMEM = 
   { /* 8 cylinder with 1 cam pulse for 360 crank degrees  */
     1,0,1,0,1,0,1,0,3,2,3,2,3,2,3,2 /* four pulses per crank revolution (one per cylinder) */
   };
   
 const unsigned char volvo_d12acd_with_cam[] PROGMEM = 
   { /* Volvo 6 cylinder dieslet  17-1-17-1-17-1 (60 overall teeth) */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 1-4 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 5-8 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 9-12 */
	 2,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 13-16 */
	 0,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1, /* Teeth 17-20 1 normal, 1 long (3 teeth wide) */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 21-24 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 25-28 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 29-32 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 33-36 */
	 0,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1, /* Teeth 37-40 1 normal, 1 long (3 teeth wide) */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 41-44 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 45-48 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 49-52 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 53-56 */
	 0,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1, /* Teeth 57-60 1 normal, 1 long (3 teeth wide) */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 1-4 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 5-8 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 9-12 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 13-16 */
	 0,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1, /* Teeth 17-20 1 normal, 1 long (3 teeth wide) */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 21-24 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 25-28 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 29-32 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 33-36 */
	 0,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1, /* Teeth 37-40 1 normal, 1 long (3 teeth wide) */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 41-44 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 45-48 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 49-52 */
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1, /* Teeth 53-56 */
	 0,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1, /* Teeth 57-60 1 normal, 1 long (3 teeth wide) */
   };
 const unsigned char mazda_thirty_six_minus_two_two_two_with_six_tooth_cam[] PROGMEM = 
   { /* Mazda 36-2-2-2 with 6 tooth cam */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 1-3*/
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 4-6*/
     1,1,0,0,0, 1,3,2,2,2, 3,3,2,2,2, /* Teeth 7-9 , second trigger on tooth 9 */ 
     3,1,0,0,0, 1,1,0,0,0, 0,0,0,0,0, /* Teeth 10,11, missing 12 */
     0,0,0,0,0, 1,3,2,2,2, 2,2,2,2,2, /* Missing 13, 14, Missing 15 , second trigger on missing tooth 15 */
     2,0,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Missing 16, 17-18m 2nd trigger ends on tooth 16 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 19-21 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 22-24 */
     1,1,0,0,0, 1,3,2,2,2, 3,3,2,2,2, /* Teeth 25-27, second trigger on tooth 26-27 */
     3,1,0,0,0, 1,1,0,0,0, 0,0,0,0,0, /* Teeth 28-29, missing 30 */
     0,0,0,0,0, 1,3,2,2,2, 3,3,2,2,2, /* Missing 31, Tooth 32, 33, 2nd trigger within tooth 32 */
     3,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 34-36, 2nd trigger ends just after tooth 32 starts*/
     /* SECOND ROTATION */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 1-3 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 4-6 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 7-9 */
     1,1,0,0,0, 1,1,0,0,0, 0,0,0,0,0, /* Teeth 10,11, missing 12 */
     0,0,0,0,0, 1,3,2,2,2, 2,2,2,2,2, /* Missing 13, 14, Missing 15 , second trigger on 14-15 */
     2,0,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Missing 16, 17-18 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 19-21 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 22-24 */
     1,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 25-27 */
     1,1,0,0,0, 1,1,0,0,0, 0,0,0,0,0, /* Teeth 28-29, missing 30 */
     0,0,0,0,0, 1,3,2,2,2, 3,3,2,2,2, /* Missing 31,Tooth 32-33, 2nd trigger  on 32-33*/
     3,1,0,0,0, 1,1,0,0,0, 1,1,0,0,0, /* Teeth 34-36, 2nd trigger ends jsut after tooth 34 starts */
};

 /* Mitsubish 4g63 aka 4/2 crank and cam */
 const unsigned char mitsubishi_4g63_4_2[] PROGMEM = 
   { //Split into 5 degree blocks (12 per line)
      2,2,2,2,2,2,2,2,2,2,2,0,
      0,0,0,0,0,0,0,0,0,1,1,1, //Start edge 6
      1,1,1,1,1,1,1,1,1,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,2,2,2,3,3,3,
      3,3,3,3,3,3,3,3,1,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,2,3,3,3,
      3,3,3,3,3,3,3,3,3,3,3,2
   };
   
 /* Mitsubish 4g63 aka 4/2 crank and cam */
 const unsigned char audi_135_with_cam[] PROGMEM = 
   { //0 - 180 degrees
     3,3,2,2,3,3,2,2,3,3,2,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,

     //181 - 360 degrees
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     
     //360 - 480
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,

     //481-720
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
     1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
     0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,2,
     
   }; 

  /* Honda D17 12+1. 5 degree per entry*/
 const unsigned char honda_d17_no_cam[] PROGMEM = 
   { //0 - 360 degrees
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,1,0,0,0,
     
     //360 - 720
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,0,0,0,0,
     1,0,0,0,0,0,1,0,1,0,0,0
     
   }; 

    /*
     * http://imgur.com/a/ynLWp
     */
   const unsigned char mazda_323_au[] PROGMEM = 
   {
     0,0,0,0,0,2,0,0,1,0,0,0,
     0,0,1,0,0,0,0,0,2,0,2,1, 
     0,0,0,0,0,1
   };

   /*
     * http://www.msextra.com/doc/triggers/daihatsu-trigs.txt
     * http://www.msextra.com/doc/triggers/daihatsu3cyla.jpg
     * http://jbperf.com/JimStim/wheels_default.jsw
     * 5 degree per entry
     * 
     */
   const unsigned char daihatsu_3cyl[] PROGMEM = 
   { //0 - 360 degrees
     1,1,0,0,0,0,1,1,0,0,0,0, //0-60. Primary pulse plus the additional 1 at 15 crank degrees / 30 cam degrees
     0,0,0,0,0,0,0,0,0,0,0,0, //60-120
     0,0,0,0,0,0,0,0,0,0,0,0, //120-180
     0,0,0,0,0,0,0,0,0,0,0,0, //180-240
     1,1,0,0,0,0,0,0,0,0,0,0, //240-300
     0,0,0,0,0,0,0,0,0,0,0,0, //300-360
     
     //360 - 720
     0,0,0,0,0,0,0,0,0,0,0,0, //360-420
     0,0,0,0,0,0,0,0,0,0,0,0, //420-480
     1,1,0,0,0,0,0,0,0,0,0,0, //480-540
     0,0,0,0,0,0,0,0,0,0,0,0, //540-600
     0,0,0,0,0,0,0,0,0,0,0,0, //600-660
     0,0,0,0,0,0,0,0,0,0,0,0  //660-720
   };

    /* Mitsubish 4g63 aka 4/2 crank and cam */
 const unsigned char miata_9905[] PROGMEM = 
   { //Split into 5 degree blocks (12 per line)
      0,0,0,0,0,0,2,2,0,0,0,0, //Single cam tooth
      0,0,0,0,0,0,0,1,1,0,0,0, //Pulse at 100*
      0,0,0,0,0,0,0,0,0,1,1,0, //Pulse at 170*
      0,0,0,0,0,0,0,0,0,0,0,0, //No pulse
      0,0,0,0,0,0,0,1,1,0,0,0, //Pulse at 280*
      0,0,0,0,0,0,0,0,0,1,1,0, //Pulse at 350*
      0,0,2,2,0,0,2,2,0,0,0,0, //Double cam pulses
      0,0,0,0,0,0,0,1,1,0,0,0, //Pulse at 460*
      0,0,0,0,0,0,0,0,0,1,1,0, //Pulse at 530*
      0,0,0,0,0,0,0,0,0,0,0,0, //No pulse
      0,0,0,0,0,0,0,1,1,0,0,0, //Pulse at 640
      0,0,0,0,0,0,0,0,0,1,1,0  //Pulse at 710
   };

   /* 25 deg low, 5 deg high, #12 is missing,  cam is high for 25 deg on second crank rotation just after tooth 21 (9) */
 const unsigned char twelve_with_cam[] PROGMEM = 
   { /* 12-1 with cam */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 1 and 2 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 3 and 4 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 5 and 6 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 7 and 8 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 9 and 10 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Tooth 11 and 12 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* 2nd rotation: Teeth 13 and 14 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 15 and 16 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 17 and 18 */
     0,0,0,0,0,1,0,0,0,0,0,1, /* Teeth 19 and 20 */
     0,0,0,0,0,1,2,2,2,2,2,1, /* Tooth 21 and 22,  2nd trigger on cam between teeth 21 and 22 for 25 deg */
     0,0,0,0,0,1,0,0,0,0,0,1  /* Totth 23 and 24th */
   };

 /* 25 deg low, 5 deg high, #12 is missing,  cam is high for 25 deg on second crank rotation just after tooth 21 (9) */
 const unsigned char twenty_four_with_cam[] PROGMEM = 
   { /* 24/1 with cam */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 1 and 2 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 3 and 4 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 5 and 6 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 7 and 8 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 9 and 10 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Tooth 11 and 12 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* 2nd rotation: Teeth 13 and 14 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 15 and 16 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 17 and 18 */
     0,0,1,0,0,1,0,0,1,0,0,1, /* Teeth 19 and 20 */
     0,0,1,0,0,1,2,2,3,2,2,1, /* Tooth 21 and 22,  2nd trigger on cam between teeth 21 and 22 for 25 deg */
     0,0,1,0,0,1,0,0,1,0,0,1  /* Totth 23 and 24th */
   };

  const unsigned char subaru_six_seven[] PROGMEM =
   { /* 6/7 */
    /* Cyl 1 TDC */
      0,0,0,0,0,2,2,2,0,0, 0,2,2,2,0,0,0,2,2,2, /* 00-19 degrees - cam 1-2-3: 5* ATDC */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 20-39 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 40-59 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 60-20 degrees */
      0,0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 80-99 degrees - crank 1: 83* ATDC */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,1,0,0, /* 100 degrees - crank 2: 115* ATDC */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 120 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 140 degrees */
      0,0,0,0,0,0,0,0,0,0, 1,1,1,0,0,0,0,0,0,0, /* 160 degrees - crank 3: 170* ATDC */

    /* Cyl 2 TDC */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 180 degrees */
      2,2,2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 200 degrees - cam 4: 20* ATDC (200*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 220 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 240 degrees */
      0,0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 260 degrees - crank 4: 83* ATDC (263*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,1,0,0, /* 280 degrees - crank 5: 115* ATDC (295*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 300 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 320 degrees */
      0,0,0,0,0,0,0,0,0,0, 1,1,1,0,0,0,0,0,0,0, /* 340 degrees - crank 6: 170* ATDC (350*) */

    /* Cyl 3 TDC */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 360 degrees */
      2,2,2,0,0,0,2,2,2,0, 0,0,0,0,0,0,0,0,0,0, /* 380 degrees - cam 5-6: 20* ATDC (380*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 400 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 420 degrees */
      0,0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 440 degrees - crank 1: 83* ATDC (443*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,1,0,0, /* 460 degrees - crank 2: 115* ATDC (475*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 480 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 500 degrees */
      0,0,0,0,0,0,0,0,0,0, 1,1,1,0,0,0,0,0,0,0, /* 520 degrees - crank 3: 170* ATDC */

    /* Cyl 4 TDC */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 540 degrees */
      2,2,2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 560 degrees - cam 7: 20* ATDC (560*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 580 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 600 degrees */
      0,0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 620 degrees - crank 4: 83* ATDC (623*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,1,0,0, /* 640 degrees - crank 5: 115* ATDC (655*) */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 660 degrees */
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, /* 680 degrees */
      0,0,0,0,0,0,0,0,0,0, 1,1,1,0,0,0,0,0,0,0  /* 700 degrees - crank 6: 170* ATDC (710*) */
    };
    
 /* GM 7X for 6 cylinder engines */  
 /* https://speeduino.com/forum/download/file.php?id=4743 */
 const unsigned char gm_seven_x[] PROGMEM = 
   { /* Every number represents 2 degrees */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 0-20 */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 20-40 */
     0,1,1,0,0,0,0,0,0,0,  /* Degrees 40-60. Tooth #1 at 42* for 4* duration. These are actually 5* duration, but close enough */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 60-80 */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 80-100 */
     0,1,1,0,0,0,1,1,0,0,  /* Degrees 100-120. Tooth #2 at 102* for 4* duration. "Extra" tooth at 112* */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 120-140 */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 140-160 */
     0,1,1,0,0,0,0,0,0,0,  /* Degrees 160-180. Tooth #3 at 162* for 4* duration */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 180-200 */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 200-220 */
     0,1,1,0,0,0,0,0,0,0,  /* Degrees 220-240. Tooth #4 at 222* for 4* duration */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 240-260 */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 260-280 */
     0,1,1,0,0,0,0,0,0,0,  /* Degrees 280-300. Tooth #5 at 282* for 4* duration */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 300-320 */
     0,0,0,0,0,0,0,0,0,0,  /* Degrees 320-340 */
     0,1,1,0,0,0,0,0,0,0  /* Degrees 340-360. Tooth #6 at 342* for 4* duration */
   }; 

  /* DSM 420a Eclipse */
  /* https://github.com/noisymime/speeduino/issues/133 */
  const unsigned char four_twenty_a[] PROGMEM = 
   { /* Every number represents 5 degrees */
      0,0,0,0,0,0,0,0,0,0,0,2,
      2,2,2,2,2,2,2,2,2,3,3,2,
      2,3,3,2,2,3,3,2,2,3,3,2,
      2,2,2,2,2,0,0,0,0,0,0,1,
      1,1,1,1,1,1,1,1,1,1,1,0,
      0,1,1,0,0,1,1,0,0,1,1,0,
      0,0,0,0,0,0,0,0,0,0,0,2,
      2,2,2,2,2,2,2,2,2,3,3,2,
      2,3,3,2,2,3,3,2,2,3,3,2,
      2,2,2,2,2,2,2,2,2,2,2,3,
      3,3,3,3,3,1,1,1,1,1,1,0,
      0,1,1,0,0,1,1,0,0,1,1,0
   }; 

   const unsigned char ford_st170[] PROGMEM = 
   {
      0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
      1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,3,3,3,3,3,2,2,2,2,2,3,
      3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,
      1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,
      1,1,1,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,1,1,0,0,0,0,0,1,
      1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,
      1,1,1,1,0,0,0,0,0,1,1,1,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,
      3,3,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
      1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
      1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
      1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,3,3,3,3,3,2,2,2,2,2,
      3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
      1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
      0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
      0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
      0,1,1,1,1,1,0,0,0,0,0,1,3,3,3,3,2,2,2,2,2,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,3,2,2,2,
      2,3,3,3,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,
      0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0
   };

   const unsigned char mitsubishi_3A92[] PROGMEM = 
   {
      3,2,3,2,2,2,3,2,1,0,1,0,
      1,0,3,2,1,0,3,2,3,2,3,2,
      3,2,3,2,2,2,3,2,3,2,3,2,
      3,2,3,2,3,2,3,2,3,2,3,2,
      3,2,2,2,2,2,3,2,3,2,1,0,
      1,0,1,0,3,2,3,2,3,2,3,2,
      3,2,3,2,2,2,3,2,3,2,3,2,
      3,2,3,2,3,2,3,2,3,2,3,2,
      3,2,3,2,2,2,3,2,1,0,1,0,
      1,0,3,2,3,2,3,2,3,2,3,2,
      3,2,2,2,2,2,3,2,3,2,3,2,
      3,2,3,2,3,2,3,2,3,2,3,2
   };

  #endif
