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
#include "serialmenu.h"
#include "structures.h"
#include "wheel_defs.h"
#include <avr/pgmspace.h>
#include <math.h>
#include <util/delay.h>
#include <SerialUI.h>

/* File local variables */
extern uint16_t wanted_rpm;

/* External Globla Variables */
extern SUI::SerialUI mySUI;
extern sweep_step *SweepSteps;  /* Global pointer for the sweep steps */
extern wheels Wheels[];
extern uint8_t mode;
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

//! Initializes the serial port and sets up the Menu
/*!
 * Sets up the serial port and menu for the serial user interface
 * Sets user input timeout to 20 seconds and overall interactivity timeout at 30
 * at which point it'll disconnect the user
 */
void serial_setup()
{
  mySUI.begin(9600);
  mySUI.setTimeout(20000);
  mySUI.setMaxIdleMs(30000);
  SUI::Menu *mainMenu = mySUI.topLevelMenu();
  SUI::Menu *wheelMenu;
  SUI::Menu *advMenu;
  /* Simple all on one menu... */
  mainMenu->setName(top_menu_title);
  mainMenu->addCommand(info_key,show_info_cb,info_help);
  mainMenu->addCommand(rpm_key,set_rpm_cb,rpm_key);
  mainMenu->addCommand(sweep_key,sweep_rpm_cb,sweep_help);
  wheelMenu = mainMenu->subMenu(wheel_menu_key,wheel_menu_help);
  wheelMenu->addCommand(next_key,select_next_wheel_cb,next_help);
  wheelMenu->addCommand(previous_key,select_previous_wheel_cb,previous_help);
  wheelMenu->addCommand(list_key,list_wheels_cb,list_help);
  wheelMenu->addCommand(choose_key,select_wheel_cb,choose_help);
  advMenu = mainMenu->subMenu(advanced_menu_key,advanced_menu_help);
  advMenu->addCommand(reverse_key,reverse_wheel_direction_cb,reverse_help);
  advMenu->addCommand(pri_invert_key,toggle_invert_primary_cb,pri_invert_help);
  advMenu->addCommand(sec_invert_key,toggle_invert_secondary_cb,sec_invert_help);
  //advMenu->addCommand(pri_glitch_key,primary_glitch_cb,pri_glitch_help);
  //advMenu->addCommand(sec_glitch_key,secondary_glitch_cb,sec_glitch_help);
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
  mySUI.print_P(primary_space_signal_colon_space);
  if (output_invert_mask & 0x01)
    mySUI.println_P(inverted_);
  else
    mySUI.println_P(normal_);
}

//! Inverts the polarity of the secondary output signal
void toggle_invert_secondary_cb()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x02; /* Flip cam invert mask bit */
  mySUI.print_P(secondary_space_signal_colon_space);
  if (output_invert_mask & 0x02)
    mySUI.println_P(inverted_);
  else
    mySUI.println_P(normal_);
}


//! Returns info about status, mode and free RAM
void show_info_cb()
{
  mySUI.println_P(info_title);
  mySUI.print_P(free_ram);
  mySUI.print(freeRam());
  mySUI.println_P(bytes);
  mySUI.print_P(current_pattern);
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.println_P(Wheels[selected_wheel].decoder_name);
  if (mode == FIXED_RPM) {
    mySUI.print_P(fixed_current_rpm);
    mySUI.println(wanted_rpm);
  } 
  if (mode == LINEAR_SWEPT_RPM) {
    mySUI.print_P(swept_rpm_from);
    mySUI.print(sweep_low_rpm);
    mySUI.print_P(arrows);
    mySUI.print(sweep_high_rpm);
    mySUI.print_P(space_at_colon_space);
    mySUI.print(sweep_rate);
    mySUI.println_P(space_rpm_per_sec);
  }
}


//! Display newly selected wheel information
/*!
 * Resets the output compare register for the newly changed wheel, then
 * resets edge_counter (wheel array index) to 0 and displays the new
 * wheel information to the end user
 */
void display_new_wheel()
{
  mySUI.println_P(new_wheel_chosen);
  mySUI.print(selected_wheel + 1);
  mySUI.print_P(colon_space);
  mySUI.print_P(Wheels[selected_wheel].decoder_name);
  mySUI.print_P(space_at_colon_space);
  mySUI.print(wanted_rpm);
  mySUI.print_P(space);
  mySUI.println_P(RPM);
  if (mode != LINEAR_SWEPT_RPM)
    reset_new_OCR1A(wanted_rpm);
  else
    compute_sweep_stages(&sweep_low_rpm, &sweep_high_rpm);
  edge_counter = 0; // Reset to beginning of the wheel pattern */
}


//! Prompts user for new wheel ID
/*!
 * Presents user with a numeric data prompt, waits for input, then verifies
 * they inputted a valid choice, then changes the running wheel pattern to the
 * user selected one and reruns the RPM calc (As oit's pattern specific) and then
 * resets the wheel position to 0 to avoid starting in a position off the end of 
 * the wheel pattern array
 */
void select_wheel_cb()
{
  mySUI.showEnterNumericDataPrompt();
  byte newWheel = mySUI.parseInt();
  if ((newWheel < 1) || (newWheel > (MAX_WHEELS+1))) {
    mySUI.returnError("Wheel ID out of range");
    return;
  }
  selected_wheel = newWheel - 1; /* use 1-MAX_WHEELS range */
  display_new_wheel();
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
void set_rpm_cb()
{
  mySUI.showEnterNumericDataPrompt();
  uint32_t newRPM = mySUI.parseULong();
  if (newRPM < 10)  {
    mySUI.returnError("Invalid RPM, RPM too low");
    return;
  }
  /* Spinlock */
  while (sweep_lock)
    _delay_us(1);
  sweep_lock = true;
  if (SweepSteps)
    free(SweepSteps);
  mode = FIXED_RPM;
  reset_new_OCR1A(newRPM);
  wanted_rpm = newRPM;

  mySUI.print_P(new_rpm_chosen);
  mySUI.println(wanted_rpm);
  sweep_lock = false;
}


//! Returns a list of user selectable wheel patterns
/*!
 * Iterates through the list of wheel patterns and prints them back to the user
 */
void list_wheels_cb()
{
  byte i = 0;
  for (i=0;i<MAX_WHEELS;i++)
  {
    mySUI.print(i+1);
    mySUI.print_P(colon_space);
    mySUI.println_P((Wheels[i].decoder_name));
  }
}


//! Toggle the wheel direction, useful for debugging
/*!
 * Reverses the emitting wheel pattern direction.  Used mainly as a debugging aid
 * in case the wheel pattern was coded incorrectly in reverse.
 */
void reverse_wheel_direction_cb()
{
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
void sweep_rpm_cb()
{
  uint16_t tmp_low_rpm;
  uint16_t tmp_high_rpm;
  uint8_t j;
  char sweep_buffer[20] = {0};

  mySUI.showEnterDataPrompt();
  j = mySUI.readBytesToEOL(sweep_buffer,20);
  /* Debugging
  mySUI.print_P(read_colon_space);
  mySUI.print(j);
  mySUI.println_P(space_chars_from_user); 
  */
  j = sscanf(sweep_buffer,"%i,%i,%i",&tmp_low_rpm,&tmp_high_rpm,&sweep_rate);
  // Validate input ranges
  if ((j == 3) && 
      (tmp_low_rpm >= 10) &&
      (tmp_high_rpm < 51200) &&
      (sweep_rate >= 1) &&
      (sweep_rate < 51200) &&
      (tmp_low_rpm < tmp_high_rpm))
  {
  mySUI.print_P(sweeping_from_colon_space);
  mySUI.print(tmp_low_rpm);
  mySUI.print_P(arrows);
  mySUI.print(tmp_high_rpm);
  mySUI.print_P(space_at_colon_space);
  mySUI.print(sweep_rate);
  mySUI.println_P(space_rpm_per_sec);

  compute_sweep_stages(&tmp_low_rpm, &tmp_high_rpm);
  }
  else {
    mySUI.returnError(range_error);
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

