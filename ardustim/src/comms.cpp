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

#include "defines.h"
#include "ardustim.h"
#include "enums.h"
#include "comms.h"
#include "structures.h"
#include "storage.h"
#include "wheel_defs.h"
#include <avr/pgmspace.h>
#include <math.h>
#include <util/delay.h>

/* File local variables */
extern uint16_t wanted_rpm;

/* External Globla Variables */
extern sweep_step *SweepSteps;  /* Global pointer for the sweep steps */
extern wheels Wheels[];
extern uint8_t total_sweep_stages;
extern uint16_t sweep_low_rpm;
extern uint16_t sweep_high_rpm;
extern uint16_t sweep_rate;

/* Volatile variables (USED in ISR's) */
extern volatile uint8_t selected_wheel;
extern volatile uint8_t sweep_direction;
extern volatile uint8_t sweep_stage;
extern volatile bool normal;
extern volatile bool sweep_lock;
extern volatile bool sweep_reset_prescaler;
extern volatile uint16_t edge_counter;
extern volatile uint16_t new_OCR1A;
extern volatile uint32_t oc_remainder;

bool cmdPending;
byte currentCommand;
uint16_t numTeeth;

//! Initializes the serial port and sets up the Menu
/*!
 * Sets up the serial port and menu for the serial user interface
 * Sets user input timeout to 20 seconds and overall interactivity timeout at 30
 * at which point it'll disconnect the user
 */
void serialSetup()
{
  Serial.begin(115200);
  cmdPending = false;
}

void commandParser()
{
  char buf[80];
  byte tmp_wheel;
  byte tmp_mode;
  if (cmdPending == false) { currentCommand = Serial.read(); }

  switch (currentCommand)
  {
    case 'a':
      break;

    case 'f': //Set the fixed RPM value
      mode = FIXED_RPM;
      while(Serial.available() < 2) {} //Wait for the new RPM bytes
      wanted_rpm = word(Serial.read(), Serial.read());
      //wanted_rpm = 2000;
      //reset_new_OCR1A(wanted_rpm);
      setRPM(wanted_rpm);
      break;

    case 'c': //Save the current config
      saveConfig();
      break;
      
    case 'L': // send the list of wheel names
      //First byte sent is the number of wheels
      //Serial.println(MAX_WHEELS);
      
      //Wheel names are then sent 1 per line
      for(byte x=0;x<MAX_WHEELS;x++)
      {
        strcpy_P(buf,Wheels[x].decoder_name);
        Serial.println(buf);
      }
      break;

    case 'M': ///Change the RPM mode
      while(Serial.available() < 1) {} //Wait for the new mode byte
      tmp_mode = Serial.read();
      if(tmp_mode <= POT_RPM)
      {
        mode = tmp_mode;
      }
      break;

    case 'n': //Send the number of wheels
      Serial.println(MAX_WHEELS);
      break;

    case 'N': //Send the number of the current wheel
      Serial.println(selected_wheel);
      break;
    
    case 'p': //Send the size of the current wheel
      Serial.println(Wheels[selected_wheel].wheel_max_edges);
      break;

    case 'P': //Send the pattern for the current wheel
      numTeeth = pgm_read_word(Wheels[selected_wheel].wheel_max_edges);
      //PROGMEM_readAnything (&table[i], thisOne);
      for(uint16_t x=0; x<Wheels[selected_wheel].wheel_max_edges; x++)
      {
        if(x != 0) { Serial.print(","); }

        byte tempByte = pgm_read_byte(&Wheels[selected_wheel].edge_states_ptr[x]);
        Serial.print(tempByte);
      }
      Serial.println("");
      //2nd row of data sent is the number of degrees the wheel runs over (360 or 720 typically)
      Serial.println(Wheels[selected_wheel].wheel_degrees);
      break;

    case 'R': //Send the current RPM
      Serial.println(wanted_rpm);
      break;

    case 's': //Set the high and low RPM for sweep mode
      mode = LINEAR_SWEPT_RPM;
      while(Serial.available() < 4) {} //Wait for 4 bytes representing the new low and high RPMs

      sweep_low_rpm = word(Serial.read(), Serial.read());
      sweep_high_rpm = word(Serial.read(), Serial.read());

      sweep_low_rpm = 100;
      sweep_high_rpm = 4000;
      compute_sweep_stages(&sweep_low_rpm, &sweep_high_rpm);
      break;

    case 'S': //Set the current wheel
      while(Serial.available() < 1) {} 
      tmp_wheel = Serial.read();
      if(tmp_wheel < MAX_WHEELS)
      {
        selected_wheel = tmp_wheel;
        display_new_wheel();
      }
      break;

    case 'X': //Just a test method for switching the to the next wheel
      select_next_wheel_cb();
      strcpy_P(buf,Wheels[selected_wheel].decoder_name);
      Serial.println(buf);
      break;

    default:
      break;
  }
  cmdPending = false;
}

/* Helper function to spit out amount of ram remainig */
//! Returns the amount of freeRAM
/*!
 * Figures out the amount of free RAM remaining nad returns it to the caller
 * \return amount of free memory
 */
uint16_t freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

/* SerialUI Callbacks */
//! Inverts the polarity of the primary output signal
void toggle_invert_primary_cb()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x01; /* Flip crank invert mask bit */

}

//! Inverts the polarity of the secondary output signal
void toggle_invert_secondary_cb()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x02; /* Flip cam invert mask bit */
}

void display_new_wheel()
{
  if (mode != LINEAR_SWEPT_RPM)
    reset_new_OCR1A(wanted_rpm);
  else
    compute_sweep_stages(&sweep_low_rpm, &sweep_high_rpm);
  edge_counter = 0; // Reset to beginning of the wheel pattern */
}


//! Selects the next wheel in the list
/*!
 * Selects the next wheel, if at the end, wrap to the beginning of the list,
 * re-calculate the OCR1A value (RPM) and reset, return user information on the
 * selected wheel and current RPM
 */
void select_next_wheel_cb()
{
  if (selected_wheel == (MAX_WHEELS-1))
    selected_wheel = 0;
  else 
    selected_wheel++;
  
  display_new_wheel();
}

//
//! Selects the previous wheel in the list
/*!
 * Selects the nex, if at the beginning, wrap to the end of the list,
 * re-calculate the OCR1A value (RPM) and reset, return user information on the
 * selected wheel and current RPM
 */
void select_previous_wheel_cb()
{
  if (selected_wheel == 0)
    selected_wheel = MAX_WHEELS-1;
  else 
    selected_wheel--;
  
  display_new_wheel();
}


//! Changes the RPM based on user input
/*!
 * Prompts user for new RPM, reads it, validates it's within range, sets lock to
 * prevent a race condition with the sweeper, free's memory of SweepSteps 
 * structure IF allocated, sets the mode to fixed RPM, recalculates the new OCR1A 
 * value based on the user specificaed RPM and sets it and then removes the lock
 */ 
void setRPM(uint32_t newRPM)
{
  if (newRPM < 10)  {
    //mySUI.returnError("Invalid RPM, RPM too low");
    return;
  }
  /* Spinlock */
  while (sweep_lock)
    _delay_us(1);
  sweep_lock = true;
  if (SweepSteps)
    free(SweepSteps);
  //mode = FIXED_RPM;
  reset_new_OCR1A(newRPM);
  wanted_rpm = newRPM;
  sweep_lock = false;
}

//! Toggle the wheel direction, useful for debugging
/*!
 * Reverses the emitting wheel pattern direction.  Used mainly as a debugging aid
 * in case the wheel pattern was coded incorrectly in reverse.
 */
void reverse_wheel_direction_cb()
{
  /*
    mySUI.print_P(wheel_direction_colon_space);
    if (normal)
    {
      normal = false;
      mySUI.println_P(reversed_);
    }
    else
    {
      normal = true;
      mySUI.println_P(normal_);
    }
    */
}


//! Parses input from user and setups up RPM sweep
/*!
 * Provides the user with a prompt and request input then parses a 3 param 
 * comma separate list from the user, validates the input
 * and determins the appropriate Ouput Compare threshold values and
 * prescaler settings as well as the amount to increment with each sweep
 * ISR iteration.  It breaks up the sweep range into octaves and linearily
 * changes OC threshold between those points, mainly due to the fact that the 
 * relationship between RPM and output compare register is an inverse
 * relationship, NOT a linear one, by working in octaves, we end up with
 * a smoother sweep rate, that doesn't accelerate as it approaches the higher
 * RPM threshold.   Since the arduino canot do floating point FAST in an ISR
 * we use this to keep things as quick as possible. This function takes
 * no parameters (it cannot due to SerialUI) and returns void
 */
void sweep_rpm_cb(uint16_t tmp_low_rpm, uint16_t tmp_high_rpm)
{
  
  char sweep_buffer[20] = {0};

  // Validate input ranges
  if (
      (tmp_low_rpm >= 10) &&
      (tmp_high_rpm < 51200) &&
      (sweep_rate >= 1) &&
      (sweep_rate < 51200) &&
      (tmp_low_rpm < tmp_high_rpm))
  {

  compute_sweep_stages(&tmp_low_rpm, &tmp_high_rpm);
  }

}


void compute_sweep_stages(uint16_t *tmp_low_rpm, uint16_t *tmp_high_rpm)
{
  uint8_t j;
  uint8_t total_stages;
  uint16_t end_tcnt;
  uint32_t low_rpm_tcnt;
  uint32_t high_rpm_tcnt;
  uint16_t this_step_low_rpm;
  uint16_t this_step_high_rpm;
  uint16_t divisor;
  uint16_t steps;
  uint32_t scaled_remainder;
  uint16_t rpm_span_this_stage;
  float per_isr_tcnt_change;
  float rpm_per_isr;

  /* Spin until unlocked, then lock */
  while (sweep_lock)
    _delay_us(1);
  sweep_lock = true;

  // Get OC Register values for begin/end points
  low_rpm_tcnt = (uint32_t)(8000000.0/(((float)(*tmp_low_rpm))*Wheels[selected_wheel].rpm_scaler));
  high_rpm_tcnt = (uint32_t)(8000000.0/(((float)(*tmp_high_rpm))*Wheels[selected_wheel].rpm_scaler));

  // Get number of frequency doublings, rounding 
#ifdef MORE_LINEAR_SWEEP
  total_stages = (uint8_t)ceil(log((float)(*tmp_high_rpm)/(float)(*tmp_low_rpm))/LOG_2);
  //mySUI.print(F("MLS total stages: "));
#else
  total_stages = (uint8_t)ceil(log((float)(*tmp_high_rpm)/(float)(*tmp_low_rpm))/(2*LOG_2));
  //mySUI.print(F("total stages: "));
#endif
  //mySUI.println(total_stages);
  if (SweepSteps)
    free(SweepSteps);
  j = 0;
  SweepSteps = build_sweep_steps(&low_rpm_tcnt,&high_rpm_tcnt,&total_stages); 

#ifdef MORE_LINEAR_SWEEP
  for (uint8_t i = 0 ; i < total_stages; i+=2)
  {
    SweepSteps[i+1].prescaler_bits = SweepSteps[i].prescaler_bits;
    SweepSteps[i+1].ending_ocr = SweepSteps[i].ending_ocr;
    SweepSteps[i].ending_ocr =  (0.38 * (float)(SweepSteps[i].beginning_ocr - SweepSteps[i].ending_ocr)) + SweepSteps[i].ending_ocr;
    SweepSteps[i+1].beginning_ocr = SweepSteps[i].ending_ocr;

    for (j = 0; j < 2 ; j++)
    {
#else
      for (uint8_t i = 0 ; i < total_stages; i++)
      {
#endif
        this_step_low_rpm = get_rpm_from_tcnt(&SweepSteps[i+j].beginning_ocr, &SweepSteps[i+j].prescaler_bits);
        this_step_high_rpm = get_rpm_from_tcnt(&SweepSteps[i+j].ending_ocr, &SweepSteps[i+j].prescaler_bits);
        /* How much RPM changes this stage */
        rpm_span_this_stage = this_step_high_rpm - this_step_low_rpm;
        /* How much TCNT changes this stage */
        steps = (uint16_t)(1000*(float)rpm_span_this_stage / (float)sweep_rate);
        per_isr_tcnt_change = (float)(SweepSteps[i+j].beginning_ocr - SweepSteps[i+j].ending_ocr)/steps;
        scaled_remainder = (uint32_t)(FACTOR_THRESHOLD*(per_isr_tcnt_change - (uint16_t)per_isr_tcnt_change));
        SweepSteps[i+j].tcnt_per_isr = (uint16_t)per_isr_tcnt_change;
        SweepSteps[i+j].remainder_per_isr = scaled_remainder;

        /* Debugging
           mySUI.print(F("sweep step: "));
           mySUI.println(i+j);
           mySUI.print(F("steps: "));
           mySUI.println(steps);
           mySUI.print(F("Beginning tcnt: "));
           mySUI.print(SweepSteps[i+j].beginning_ocr);
           mySUI.print(F(" for RPM: "));
           mySUI.println(this_step_low_rpm);
           mySUI.print(F("ending tcnt: "));
           mySUI.print(SweepSteps[i+j].ending_ocr);
           mySUI.print(F(" for RPM: "));
           mySUI.println(this_step_high_rpm);
           mySUI.print(F("prescaler bits: "));
           mySUI.println(SweepSteps[i+j].prescaler_bits);
           mySUI.print(F("tcnt_per_isr: "));
           mySUI.println(SweepSteps[i+j].tcnt_per_isr);
           mySUI.print(F("scaled remainder_per_isr: "));
           mySUI.println(SweepSteps[i+j].remainder_per_isr);
           mySUI.print(F("FP TCNT per ISR: "));
           mySUI.println(per_isr_tcnt_change,6);
           mySUI.print(F("End of step: "));
           mySUI.println(i+j);
           */
#ifndef MORE_LINEAR_SWEEP
      }
#else
    }
  }
#endif
  total_sweep_stages = total_stages;
  /*
  mySUI.print(F("Total sweep stages: "));
  mySUI.println(total_sweep_stages);
  */
  /* Reset params for Timer2 ISR */
  sweep_stage = 0;
  sweep_direction = ASCENDING;
  sweep_reset_prescaler = true;
  new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;  
  oc_remainder = 0;
  mode = LINEAR_SWEPT_RPM;
  sweep_high_rpm = *tmp_high_rpm;
  sweep_low_rpm = *tmp_low_rpm;
  sweep_lock = false;
}

