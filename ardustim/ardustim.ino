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
 * along with any FreeEMS software.  If not, see http://www.gnu.org/licenses/
 *
 */

/* The "RPM" of the wheel is dependent on the number of edges
 * so for a 60-2 wheel (120 edges), the time between teeth is
 * 8000000/RPM,  but for lesser teeth wheels this will be different
 * Thus we need a corresponding array to fix that, so that the 
 * requested RPM is actually synthesized as we want
 */

#include "enums.h"
#include "wheel_defs.h"
#include <avr/pgmspace.h>
#include <SerialUI.h>
#include "serialmenu.h"


volatile byte selected_wheel = TWENTY_FOUR_MINUS_TWO_WITH_SECOND_TRIGGER;
/* Setting rpm to any value over 0 will enabled sweeping by default */
volatile unsigned long wanted_rpm = 4000; /* Used ONLY when RPM_STEP is 0 above, otherwise it's the starting point... */

/* Stuff for handling prescaler changes (small tooth wheels are low RPM) */
volatile byte reset_prescaler = 0;
volatile byte BIT_CS10 = 0;
volatile byte BIT_CS11 = 0;
volatile byte BIT_CS12 = 0;
volatile uint16_t new_OCR1A = 5000; /* sane default */
volatile uint16_t edge_counter = 0;

byte mode = FIXED_RPM;
byte sweep_state = ASCENDING;





void setup() {
  serial_setup();

  cli(); // stop interrupts

  /* Configuring TIMER1 (pattern generator) */

  // Set timer1 to generate pulses
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Set compare register to sane default
  OCR1A = 1000;  /* 8000 RPM (60-2) */

  // Turn on CTC mode
  TCCR1B |= (1 << WGM12); // Normal mode (not PWM)
  // Set prescaler to 1
  TCCR1B |= (1 << CS10); /* Prescaler of 1 */
  // Enable output compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  // Set timer2 to run sweeper
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // Set compare register to sane default
  OCR2A = 250;  /* 8000 RPM (60-2) */

  // Turn on CTC mode
  TCCR2B |= (1 << WGM12); // Normal mode (not PWM)
  // Set prescaler to 1
  TCCR2B |= (1 << CS22); /* Prescaler of 1 */
  // Enable output compare interrupt
  TIMSK1 |= (1 << OCIE2A);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  sei(); // Enable interrupts
} // End setup


ISR(TIMER2_COMPA_vect) {
  /* Don't do anything yet */
}

ISR(TIMER1_COMPA_vect) {
  /* This is VERY simple, just walk the array and wrap when we hit the limit */
  edge_counter++;
  if (edge_counter >= Wheels[selected_wheel].wheel_max_edges) {
    edge_counter = 0;
  }
  /* The tables are in flash so we need pgm_read_byte() */
  PORTB = pgm_read_byte(&Wheels[selected_wheel].edge_states_ptr[edge_counter]);   /* Write it to the port */

  /* Reset Prescaler only if flag is set */
  if (reset_prescaler)
  {
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); /* Clear CS10, CS11 and CS12 */
    TCCR1B |= (BIT_CS10 << CS10) | (BIT_CS11 << CS11) | (BIT_CS12 << CS12);
    reset_prescaler = 0;
  }
  /* Reset next compare value for RPM changes */
  OCR1A = new_OCR1A;  /* Apply new "RPM" from main loop, i.e. speed up/down the virtual "wheel" */
}

void loop() {
  /* We could do one of the following:
   * programmatically screw with the OCR1A register to adjust the RPM (i.e. auto-sweep)
   * read a pot and modify it
   * read the serial port and modify it
   * read other inputs to switch wheel modes
   */
  extern SUI::SerialUI mySUI;

  if (mySUI.checkForUserOnce())
  {
    // Someone connected!
    mySUI.enter();
    while (mySUI.userPresent()) 
    {
      mySUI.handleRequests();
    }
  }
}


/*
void run_ardustim()
{
  uint32_t tmp = 0;
  switch (mode) {
  case SWEEPING_RPM: 
    switch (sweep_state) {
    case DESCENDING:
      wanted_rpm -= rpm_step;
      if (wanted_rpm <= rpm_min) {
        sweep_state = ASCENDING;
      }
      break;
    case ASCENDING:
      wanted_rpm += rpm_step;
      if (wanted_rpm >= rpm_max) {
        sweep_state = DESCENDING;
      }
      break;
    }
  case FIXED_RPM:
    break;
  }
  tmp=8000000/(wanted_rpm*Wheels[selected_wheel].rpm_scaler);
  BIT_CS10 = 1;
  BIT_CS11 = 0;
  BIT_CS12 = 0;
  if (tmp > 16776960) {
    // Need to set prescaler to x256 
    BIT_CS12 = 1;
    new_OCR1A = tmp/256;
    new_prescale = PRESCALE_256; 
  } 
  else if (tmp > 524288 ) {
    // Need to reset prescaler to 64 to prevent overflow 
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
}
*/

