
/*
 * Arbritrary wheel pattern generator (60-2 and 36-1 so far... )
 * copyright 2014 David J. Andruczyk
 *
 */
 
/* The "RPM" of the wheel is dependent on the number of edges
 * so for a 60-2 wheel (120 edges), the time between teeth is
 * 8000000/RPM,  but for lesser teeth wheels this will be different
 * Thus we need a corresponding array to fix that, so that the 
 * requested RPM is actually exported as we want
 */
 
#include "wheel_defs.h"
#include <avr/pgmspace.h>

/* Setting RPM_STEP to any value over 0 will enabled sweeping */
#define RPM_STEP 0
#define RPM_MIN 100
#define RPM_MAX 3000
#define RPM_STEP_DELAY 2
 
 unsigned int wanted_rpm = 1000; /* Used ONLY when RPM_STEP is 0 above, otherwise it's the starting point... */
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
 
 /* Wheel types we know about */
 typedef enum { 
   DIZZY_FOUR_CYLINDER,  /* 2 evenly spaced teeth */
   DIZZY_SIX_CYLINDER,   /* 3 evenly spaced teeth */
   DIZZY_EIGHT_CYLINDER, /* 4 evenly spaced teeth */
   SIXTY_MINUS_TWO,      /* 60-2 crank only */
   THIRTY_SIX_MINUS_ONE, /* 36-1 crank only */
   FOUR_MINUS_ONE_WITH_CAM, /* 4-1 crank + cam */
   EIGHT_MINUS_ONE,       /* 8-1 */
   SIX_MINUS_ONE_WITH_CAM,/* 6-1 crank + cam */
   TWELVE_MINUS_ONE_WITH_CAM, /* 12-1 crank + cam */
   FOURTY_MINUS_ONE,      /* Ford V-10 40-1 crank */
   DIZZY_TRIGGER_RETURN,  /* dizzy signal, 40deg on 50 deg off */
   ODDFIRE_VR,            /* Oddfire VR (from jimstim) */
   OPTISPARK_LT1,         /* Optispark 360 and 8 */
   MAX_WHEELS,
 }WheelType;
 
 //volatile byte selected_wheel = SIXTY_MINUS_TWO;
 volatile byte selected_wheel = OPTISPARK_LT1;
 const float rpm_scaler[MAX_WHEELS] = {
   0.03333, /* dizzy 4 */
   0.05, /* dizzy 6 */
   0.06667, /* dizzy 8 */
   1.0, /* 60-2 */
   0.6, /* 36-1  (72 edges/120) */
   0.13333, /* 4-1 with cam */
   0.13333, /* 8-1 */
   0.3,     /* 6-1 with cam */
   1.2,     /* 12-1 with cam */
   0.66667, /* 40-1 */
   0.075,   /* dizzy trigger return */
   0.2,     /* Oddfire VR */
   6.0,     /* Optispark LTA (360 and 8) */ 
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
 };
 
 /* Stick it in flash as we only have 1K of RAM */
 prog_uchar *edge_states_ptr[MAX_WHEELS] = {
   dizzy_four_cylinder, \
   dizzy_six_cylinder, \
   dizzy_eight_cylinder, \
   sixty_minus_two, \
   thirty_six_minus_one, \
   four_minus_one_with_cam, \
   eight_minus_one, \
   six_minus_one_with_cam, \
   twelve_minus_one_with_cam, \
   fourty_minus_one, \
   dizzy_trigger_return, \
   oddfire_vr, \
   optispark_lt1, \
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
   if (edge_counter >= wheel_max_edges[selected_wheel]) {
     edge_counter = 0;
   }
   /* The tables are in flash so we need pgm_read_byte() */
   PORTB = pgm_read_byte(&edge_states_ptr[selected_wheel][edge_counter]);   /* Write it to the port */

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
   tmp=8000000/(wanted_rpm*rpm_scaler[selected_wheel]);
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
