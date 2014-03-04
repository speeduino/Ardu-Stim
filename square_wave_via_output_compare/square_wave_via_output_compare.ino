
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
 * along with any FreeEMS software.  If not, see http://www.gnu.org/licenses/
 *
 */
 
/* The "RPM" of the wheel is dependent on the number of edges
 * so for a 60-2 wheel (120 edges), the time between teeth is
 * 8000000/RPM,  but for lesser teeth wheels this will be different
 * Thus we need a corresponding array to fix that, so that the 
 * requested RPM is actually synthesized as we want
 */
 
#include "wheel_defs.h"
#include <avr/pgmspace.h>

/* Setting RPM_STEP to any value over 0 will enabled sweeping */
#define RPM_STEP 0
#define RPM_MIN 100
#define RPM_MAX 3000
#define RPM_STEP_DELAY 2
 
 unsigned int wanted_rpm = 2000; /* Used ONLY when RPM_STEP is 0 above, otherwise it's the starting point... */
 volatile uint16_t edge_counter = 0;
 
 /* Stuff for handling prescaler changes (small tooth wheels are low RPM) */
 volatile byte reset_prescaler = 0;
 volatile byte BIT_CS10 = 0;
 volatile byte BIT_CS11 = 0;
 volatile byte BIT_CS12 = 0;
 
 enum { 
   PRESCALE_1, 
   PRESCALE_8,
   PRESCALE_64, 
   PRESCALE_256 
 };
 byte last_prescale = PRESCALE_1;
 byte new_prescale = PRESCALE_1;
 
 enum { 
   DESCENDING, 
   ASCENDING 
 };
 byte sweep_state = ASCENDING;
 volatile uint16_t new_OCR1A = 5000; /* sane default */
 volatile byte selected_wheel = HONDA_RC51_WITH_CAM;

  /* Tie things into one nicer structure ... */
 struct wheels {
   prog_char *decoder_name;
   prog_uchar *edge_states_ptr;
   const float rpm_scaler;
   const uint16_t wheel_max_edges;
 } Wheels[MAX_WHEELS] = {
   /* Pointer to friendly name string, pointer to edge array, RPM Scaler, Number of edges in the array */
   { dizzy_four_cylinder_friendly_name, dizzy_four_cylinder, 0.03333, 4 },
   { dizzy_six_cylinder_friendly_name, dizzy_six_cylinder, 0.05, 6 },
   { dizzy_eight_cylinder_friendly_name, dizzy_eight_cylinder, 0.06667, 8 },
   { sixty_minus_two_friendly_name, sixty_minus_two, 1.0, 120 },
   { sixty_minus_two_with_cam_friendly_name, sixty_minus_two_with_cam, 2.0, 240 },
   { thirty_six_minus_one_friendly_name, thirty_six_minus_one, 0.6, 72 },
   { four_minus_one_with_cam_friendly_name, four_minus_one_with_cam, 0.13333, 16 },
   { eight_minus_one_friendly_name, eight_minus_one, 0.13333, 16 },
   { six_minus_one_with_cam_friendly_name, six_minus_one_with_cam, 0.3, 36 },
   { twelve_minus_one_with_cam_friendly_name, twelve_minus_one_with_cam, 1.2, 144 },
   { fourty_minus_one_friendly_name, fourty_minus_one, 0.66667, 80 },
   { dizzy_trigger_return_friendly_name, dizzy_trigger_return, 0.075, 9 },
   { oddfire_vr_friendly_name, oddfire_vr, 0.2, 24 },
   { optispark_lt1_friendly_name, optispark_lt1, 6.0, 720 },
   { twelve_minus_three_friendly_name, twelve_minus_three, 0.4, 48 },
   { thirty_six_minus_two_two_two_friendly_name, thirty_six_minus_two_two_two, 0.6, 72},
   { thirty_six_minus_two_two_two_with_cam_friendly_name, thirty_six_minus_two_two_two_with_cam, 0.6, 72 },
   { fourty_two_hundred_wheel_friendly_name, fourty_two_hundred_wheel, 0.6, 72 },
   { thirty_six_minus_one_with_cam_fe3_friendly_name, thirty_six_minus_one_with_cam_fe3, 1.2, 144 },
   { six_g_seventy_two_with_cam_friendly_name, six_g_seventy_two_with_cam, 1.2, 144 },
   { buell_oddfire_cam_friendly_name, buell_oddfire_cam, 0.33333, 80 },
   { gm_ls1_crank_and_cam_friendly_name, gm_ls1_crank_and_cam, 6.0, 720 },
   { lotus_thirty_six_minus_one_one_one_one_friendly_name, lotus_thirty_six_minus_one_one_one_one, 0.6, 72 },
   { honda_rc51_with_cam_friendly_name, honda_rc51_with_cam, 0.4, 48 },
   { thirty_six_minus_one_with_second_trigger_friendly_name, thirty_six_minus_one_with_second_trigger, 1.2, 144 },
   { thirty_six_minus_one_plus_one_with_cam_ngc4_friendly_name, thirty_six_minus_one_plus_one_with_cam_ngc4, 6.0, 720},
};

 void setup() {
   Serial.begin(9600);
   cli(); // stop interrupts
   
   // Set timer1 to generate pulses
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;
   // Set compare registers 
   // OCR1A = 8000;  /* 1000 RPM */
   OCR1A = 4000;  /* 2000  RPM */ 
   OCR1A = 2000;  /* 4000  RPM */
   OCR1A = 1000;  /* 8000  RPM */
   //OCR1A = 500;   /* 16000 RPM */
   //OCR1A = 250;   /* 32000 RPM */

   // Turn on CTC mode
   TCCR1B |= (1 << WGM12); // Normal mode (not PWM)
   // Set prescaler to 1
   TCCR1B |= (1 << CS10); /* Prescaler of 1 */
   // Enable output compare interrupt
   TIMSK1 |= (1 << OCIE1A);
   
   sei(); // Enable interrupts
   DDRB = B00000011; /* Set pin 8 and 9 as output (crank and cam respectively) */
   //pinMode(8, OUTPUT);
 } // End setup
 
 ISR(TIMER1_COMPA_vect) {
   /* This is VERY simple, just walk the array and wrap when we hit the limit */
   edge_counter++;
   if (edge_counter >= Wheels[selected_wheel].wheel_max_edges) {
     edge_counter = 0;
   }
   /* The tables are in flash so we need pgm_read_byte() */
   PORTB = pgm_read_byte(&Wheels[selected_wheel].edge_states_ptr[edge_counter]);   /* Write it to the port */

   /* Reset Prescaler, this is INEFFICIENT to do this on every loop through 
    * Find a way to ONLY reset it when necessary */
   if (reset_prescaler)
   {
     TCCR1B &= ~((1 << BIT_CS10) | (1 << BIT_CS11) | (1 << BIT_CS12)); /* Clear CS10, CS11 and CS12 */
     TCCR1B |= (BIT_CS10 << CS10) | (BIT_CS11 << CS11) | (BIT_CS12 << CS12);
     reset_prescaler = 0;
   }
   /* Reset next compare value for RPM changes */
   OCR1A = new_OCR1A;  /* Apply new "RPM" from main loop, i.e. speed up/down the virtual "wheel" */
 }
 
 void loop() {
   uint32_t tmp = 0;

/* We could do one of the following:
 * programmatically screw with the OCR1A register to adjust the RPM (i.e. auto-sweep)
 * read a pot and modify it
 * read the serial port and modify it
 * read other inputs to switch wheel modes
 */
   
   switch (sweep_state) {
     case DESCENDING:
     wanted_rpm -= RPM_STEP;
     if (wanted_rpm <= RPM_MIN) {
       sweep_state = ASCENDING;
     }
     //Serial.print("Descending, wanted_rpm is: ");
     //Serial.println(wanted_rpm);
     break;
     case ASCENDING:
     wanted_rpm += RPM_STEP;
     if (wanted_rpm >= RPM_MAX) {
       sweep_state = DESCENDING;
     }
     //Serial.print("Ascending, wanted_rpm is: ");
     //Serial.println(wanted_rpm);    break;   
   }
   tmp=8000000/(wanted_rpm*Wheels[selected_wheel].rpm_scaler);
   BIT_CS10 = 1;
   BIT_CS11 = 0;
   BIT_CS12 = 0;
   if (tmp > 16776960) {
      /* Need to set prescaler to x256 */
      BIT_CS12 = 1;
      new_OCR1A = tmp/256;
      new_prescale = PRESCALE_256; 
   } 
   else if (tmp > 524288 ) {
      /* Need to reset prescaler to 64 to prevent overflow */
      BIT_CS11=1;
      new_OCR1A = tmp/64;
      new_prescale = PRESCALE_64;
   } 
   else if (tmp > 65536) {
      BIT_CS10=0;
      BIT_CS11=1;
      new_OCR1A = tmp/8;
      new_prescale = PRESCALE_8;
   }
   else {
     new_OCR1A = (uint16_t)tmp;
     new_prescale = PRESCALE_1;
   }
   if (new_prescale != last_prescale) {
     reset_prescaler = 1;
   }
   last_prescale = new_prescale;
   //Serial.print("new_OCR1A var is: ");
   //Serial.println(new_OCR1A);
   delay(RPM_STEP_DELAY);


 }
