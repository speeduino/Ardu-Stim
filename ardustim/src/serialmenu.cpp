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

#include <avr/pgmspace.h>
#include <SerialUI.h>
#include "ardustim.h"
#include "defines.h"
#include "enums.h"
#include "serialmenu.h"
#include "structures.h"
#include "wheel_defs.h"

extern SUI::SerialUI mySUI;
extern sweep_step *SweepSteps;  /* Global pointer for the sweep steps */
extern wheels Wheels[];
extern uint8_t mode;
extern uint8_t total_sweep_stages;
extern uint16_t wanted_rpm;
extern uint16_t sweep_low_rpm;
extern uint16_t sweep_high_rpm;
extern uint16_t sweep_rate;

extern volatile uint8_t selected_wheel;
extern volatile uint8_t sweep_direction;
extern volatile uint8_t sweep_stage;
extern volatile bool normal;
extern volatile bool sweep_lock;
extern volatile bool sweep_reset_prescaler;
extern volatile uint16_t edge_counter;
extern volatile uint16_t new_OCR1A;
extern volatile uint32_t oc_remainder;

/* 
 * Setup the initial menu structure and callbacks
 */
void serial_setup()
{
  mySUI.begin(9600);
  mySUI.setTimeout(20000);
  mySUI.setMaxIdleMs(30000);
  SUI::Menu *mainMenu = mySUI.topLevelMenu();
  /* Simple all on one menu... */
  mainMenu->setName(top_menu_title);
  mainMenu->addCommand(info_key,show_info,info_help);
  mainMenu->addCommand(next_key,select_next_wheel,next_help);
  mainMenu->addCommand(previous_key,select_previous_wheel,previous_help);
  mainMenu->addCommand(list_key,list_wheels,list_help);
  mainMenu->addCommand(choose_key,select_wheel,choose_help);
  mainMenu->addCommand(reverse_key,reverse_wheel_direction,reverse_help);
  mainMenu->addCommand(rpm_key,set_rpm,rpm_help);
  mainMenu->addCommand(sweep_key,sweep_rpm,sweep_help);
  mainMenu->addCommand(pri_invert_key,toggle_invert_primary,pri_invert_help);
  mainMenu->addCommand(sec_invert_key,toggle_invert_secondary,sec_invert_help);
}

/* Helper function to spit out amount of ram remainig */
int freeRam () {                                                                
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


/* SerialUI Callbacks */
void toggle_invert_primary()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x01; /* Flip crank invert mask bit */
  mySUI.print_P(primary);
  mySUI.print_P(space_signal);
  if (output_invert_mask & 0x01)
    mySUI.println_P(space_inverted);
  else
    mySUI.println_P(space_normal);
}

void toggle_invert_secondary()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x02; /* Flip cam invert mask bit */
  mySUI.print_P(secondary);
  mySUI.print_P(space_signal);
  if (output_invert_mask & 0x02)
    mySUI.println_P(space_inverted);
  else
    mySUI.println_P(space_normal);
}

void show_info()
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
    mySUI.print_P(space_to_colon_space);
    mySUI.print(sweep_high_rpm);
    mySUI.print_P(space_at_colon_space);
    mySUI.print(sweep_rate);
    mySUI.println_P(rpm_per_second);
  }
}

void select_wheel()
{
  mySUI.showEnterNumericDataPrompt();
  byte newWheel = mySUI.parseInt();
  if ((newWheel < 1) || (newWheel > (MAX_WHEELS+1))) {
    mySUI.returnError("Wheel ID out of range");
  }
  selected_wheel = newWheel - 1; /* use 1-MAX_WHEELS range */
  reset_new_OCR1A(wanted_rpm);

  mySUI.println_P(new_wheel_chosen);
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.print_P(Wheels[selected_wheel].decoder_name);
  mySUI.print_P(space_at_space);
  mySUI.print(wanted_rpm);
  mySUI.println_P(space_RPM);
  mySUI.returnOK();
  edge_counter = 0;
}

void set_rpm()
{
  mySUI.showEnterNumericDataPrompt();
  uint32_t newRPM = mySUI.parseULong();
  if (newRPM < 10)  {
    mySUI.returnError("Invalid RPM, RPM too low");
    return;
  }
  if (SweepSteps)
    free(SweepSteps);
  mode = FIXED_RPM;
  reset_new_OCR1A(newRPM);
  wanted_rpm = newRPM;

  mySUI.print_P(new_rpm_chosen);
  mySUI.println(wanted_rpm);
  mySUI.returnOK();
}

void list_wheels()
{
  byte i = 0;
  for (i=0;i<MAX_WHEELS;i++)
  {
    mySUI.print(i+1);
    mySUI.print_P(colon_space);
    mySUI.println_P((Wheels[i].decoder_name));
  }
  mySUI.returnOK();
}

void select_next_wheel()
{
  if (selected_wheel == (MAX_WHEELS-1))
    selected_wheel = 0;
  else 
    selected_wheel++;
  edge_counter = 0;
  reset_new_OCR1A(wanted_rpm);
  
  mySUI.println_P(new_wheel_chosen);
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.print_P(Wheels[selected_wheel].decoder_name);
  mySUI.print_P(space_at_space);
  mySUI.print(wanted_rpm);
  mySUI.println_P(space_RPM);
  mySUI.returnOK();
}

void select_previous_wheel()
{
  if (selected_wheel == 0)
    selected_wheel = MAX_WHEELS-1;
  else 
    selected_wheel--;
  edge_counter = 0;
  reset_new_OCR1A(wanted_rpm);
  
  mySUI.println_P(new_wheel_chosen);
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.print_P(Wheels[selected_wheel].decoder_name);
  mySUI.print_P(space_at_space);
  mySUI.print(wanted_rpm);
  mySUI.println_P(space_RPM);
  mySUI.returnOK();
}

void reverse_wheel_direction()
{
    if (normal)
    {
      normal = false;
      mySUI.println_P(wheel_reverse);
    }
    else
    {
      normal = true;
      mySUI.println_P(wheel_forward);
    }
    mySUI.returnOK();
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
void sweep_rpm()
{
  byte count;
  uint8_t total_stages;
  uint16_t tmp_low_rpm;
  uint16_t tmp_high_rpm;
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

  char sweep_buffer[20] = {0};
  mySUI.showEnterDataPrompt();
  count = mySUI.readBytesToEOL(sweep_buffer,20);
  mySUI.print(F("Read: "));
  mySUI.print(count);
  mySUI.println(F(" characters from the user...")); 
  count = sscanf(sweep_buffer,"%i,%i,%i",&tmp_low_rpm,&tmp_high_rpm,&sweep_rate);
  mySUI.print(F("Number of successfull matches (should be 3): "));
  mySUI.println(count);
  mySUI.print(F("low RPM: "));
  mySUI.println(tmp_low_rpm);
  mySUI.print(F("high RPM: "));
  mySUI.println(tmp_high_rpm);
  mySUI.print(F("RPM/sec: "));
  mySUI.println(sweep_rate);
  // Validate input ranges
  if ((count == 3) && 
      (tmp_low_rpm >= 25) &&
      (tmp_high_rpm < 51200) &&
      (sweep_rate > 10) &&
      (sweep_rate < 51200) &&
      (tmp_low_rpm < tmp_high_rpm))
  {
    // Get OC Register values for begin/end points
    low_rpm_tcnt = (uint32_t)(8000000.0/(((float)tmp_low_rpm)*Wheels[selected_wheel].rpm_scaler));
    high_rpm_tcnt = (uint32_t)(8000000.0/(((float)tmp_high_rpm)*Wheels[selected_wheel].rpm_scaler));

    // Get number of frequency doublings, rounding 
    total_stages = (uint8_t)ceil(logf((float)tmp_high_rpm/(float)tmp_low_rpm)/logf(2));
    if (SweepSteps)
      free(SweepSteps);
    SweepSteps = build_sweep_steps(&low_rpm_tcnt,&high_rpm_tcnt,&total_stages); 

    for (uint8_t i = 0 ; i< total_stages; i++)
    {
      this_step_low_rpm = get_rpm_from_tcnt(&SweepSteps[i].beginning_ocr, &SweepSteps[i].prescaler_bits);
      this_step_high_rpm = get_rpm_from_tcnt(&SweepSteps[i].ending_ocr, &SweepSteps[i].prescaler_bits);
      /* How much RPM changes this stage */
      rpm_span_this_stage = this_step_high_rpm - this_step_low_rpm;
      /* How many TCNT changes this stage */
      steps = (uint16_t)(1000*(float)rpm_span_this_stage / (float)sweep_rate);
      per_isr_tcnt_change = (float)(SweepSteps[i].beginning_ocr - SweepSteps[i].ending_ocr)/steps;
      scaled_remainder = (uint32_t)(FACTOR_THRESHOLD*(per_isr_tcnt_change - (uint16_t)per_isr_tcnt_change));
      SweepSteps[i].tcnt_per_isr = (uint16_t)per_isr_tcnt_change;
      SweepSteps[i].remainder_per_isr = scaled_remainder;

/*
      mySUI.print(F("sweep step: "));
      mySUI.println(i);
      mySUI.print(F("steps: "));
      mySUI.println(steps);
      mySUI.print(F("Beginning tcnt: "));
      mySUI.print(SweepSteps[i].beginning_ocr);
      mySUI.print(F(" for RPM: "));
      mySUI.println(this_step_low_rpm);
      mySUI.print(F("ending tcnt: "));
      mySUI.print(SweepSteps[i].ending_ocr);
      mySUI.print(F(" for RPM: "));
      mySUI.println(this_step_high_rpm);
      mySUI.print(F("prescaler bits: "));
      mySUI.println(SweepSteps[i].prescaler_bits);
      mySUI.print(F("tcnt_per_isr: "));
      mySUI.println(SweepSteps[i].tcnt_per_isr);
      mySUI.print(F("scaled remainder_per_isr: "));
      mySUI.println(SweepSteps[i].remainder_per_isr);
      mySUI.print(F("FP TCNT per ISR: "));
      mySUI.println(per_isr_tcnt_change,6);
      mySUI.print(F("End of step: "));
      mySUI.println(i);
      */
    }
    total_sweep_stages = total_stages;
    //mySUI.print(F("Total sweep stages: "));
    //mySUI.println(total_sweep_stages);
  }
  else {
    mySUI.returnError("Range error !(100-50000,100-50000,10-50000)!");
  } 
  mySUI.returnOK();
  /* Reset params for Timer2 ISR */
  sweep_stage = 0;
  sweep_direction = ASCENDING;
  sweep_reset_prescaler = true;
  new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;  
  oc_remainder = 0;
  mode = LINEAR_SWEPT_RPM;
  sweep_high_rpm = tmp_high_rpm;
  sweep_low_rpm = tmp_low_rpm;
  sweep_lock = false;
}
