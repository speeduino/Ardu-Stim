/* vim: set syntax=c expandtab sw=2 softtabstop=2 autoindent smartindent smarttab : */                                                                          
/*
 * ardustim
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
 
#include <avr/pgmspace.h>
#include <SerialUI.h>
#include "serialmenu.h"
#include "enums.h"
#include "wheels.h"

 SUI::SerialUI mySUI = SUI::SerialUI(greeting);                                  

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
  mainMenu->addCommand(rpm_key,set_rpm,rpm_help);
  mainMenu->addCommand(sweep_key,sweep_rpm,sweep_help);
}

/* Helper function to spit out amount of ram remainig */
int freeRam () {                                                                
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


/* SerialUI Callbacks */
void show_info()
{
  extern struct wheels Wheels[];
  extern volatile byte selected_wheel;
  extern volatile byte mode;
  extern uint16_t wanted_rpm;
 
  mySUI.println(F("Welcome to ArduStim, written by David J. Andruczyk"));
  mySUI.print(F("Free RAM: "));
  mySUI.print(freeRam());
  mySUI.println(F(" bytes"));
  mySUI.print(F("Currently selected Wheel pattern: "));
  mySUI.print(selected_wheel+1);
  mySUI.print(" ");
  mySUI.println_P(Wheels[selected_wheel].decoder_name);
  if (mode == FIXED_RPM) {
    mySUI.print(F("Fixed RPM mode, Current RPM: "));
    mySUI.println(wanted_rpm);
  } 
  /*else {
    mySUI.print(F("Linear Sweep RPM mode, Low RPM Setpoint: "));
    mySUI.print(rpm_min);    
    mySUI.print(F(" High RPM Setpoint: "));
    mySUI.println(rpm_max);    
    mySUI.print(F("RPM Step: "));
    mySUI.print(rpm_step);    
    mySUI.print(F(" Delay between steps (ms): "));
    mySUI.println(rpm_step_delay);
    mySUI.print(F("Current RPM: "));
    mySUI.println(wanted_rpm);
  }
  */
}

void select_wheel()
{
  extern struct wheels Wheels[];
  extern volatile byte selected_wheel;

  mySUI.showEnterNumericDataPrompt();
  byte newWheel = mySUI.parseInt();
  if ((newWheel < 1) || (newWheel > (MAX_WHEELS+1))) {
    mySUI.returnError("Wheel ID out of range");
  }
  selected_wheel = newWheel - 1; /* use 1-MAX_WHEELS range */
  mySUI.println(F("New Wheel chosen"));
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.println_P(Wheels[selected_wheel].decoder_name);
  mySUI.returnOK();
}

void set_rpm()
{
  extern uint16_t wanted_rpm;
  extern volatile byte mode;
  extern uint16_t wanted_rpm;
  
  mySUI.showEnterNumericDataPrompt();
  uint16_t newRPM = mySUI.parseInt();
  if ((newRPM < 100) || (newRPM > 65535)) {
    mySUI.returnError("Invalid RPM, out of range 100-65535");
  }
  mode = FIXED_RPM;
  wanted_rpm = (unsigned long)newRPM;
  mySUI.print(F("New RPM chosen: "));
  mySUI.println(wanted_rpm);
  mySUI.returnOK();
}

void list_wheels()
{
  extern struct wheels Wheels[];
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
  extern volatile byte selected_wheel;
  extern struct wheels Wheels[];
  if (selected_wheel == (MAX_WHEELS-1))
    selected_wheel = 0;
  else 
    selected_wheel++;

  mySUI.print("New wheel is ");
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.println_P(Wheels[selected_wheel].decoder_name);
  mySUI.returnOK();
}

void select_previous_wheel()
{
  extern volatile byte selected_wheel;
  extern struct wheels Wheels[];
  if (selected_wheel == 0)
    selected_wheel = MAX_WHEELS-1;
  else 
    selected_wheel--;

  mySUI.print(F("New wheel is "));
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.println_P(Wheels[selected_wheel].decoder_name);
  mySUI.returnOK();
}

void sweep_rpm()
{
  extern struct wheels Wheels[];
  volatile struct pattern_set SweepSteps[];
  extern volatile byte selected_wheel;

  byte count = 0;
  uint16_t tmp_min = 0;
  uint16_t tmp_max = 0;
  uint16_t tmp_rpm_per_sec = 0;
  uint16_t tmpi = 0;
  byte last_prescaler = 0;
  long low_tcnt = 0;
  uint16_t low_rpm = 0;
  long high_tcnt = 0;
  uint16_t high_rpm = 0;
  int i = 0;

  char sweep_buffer[20];
  mySUI.showEnterDataPrompt();
  count = mySUI.readBytesToEOL(sweep_buffer,20);
  mySUI.print(F("Read: "));
  mySUI.print(count);
  mySUI.println(F(" characters from the user...")); 
  count = sscanf(sweep_buffer,"%i,%i,%i",&tmp_min,&tmp_max,&tmp_rpm_per_sec);
  mySUI.print(F("Number of successfull matches (should be 3): "));
  mySUI.println(count);
  mySUI.print(F("min: "));
  mySUI.println(tmp_min);
  mySUI.print(F("max: "));
  mySUI.println(tmp_max);
  mySUI.print(F("RPM/sec: "));
  mySUI.println(tmp_rpm_per_sec);
  if ((count == 3) && 
    (tmp_min >= 50) &&
    (tmp_max < 51200) &&
    (tmp_rpm_per_sec > 0) &&
    (tmp_rpm_per_sec < 51200) &&
    (tmp_min < tmp_max))
  {
    tmpi = tmp_min;
    /* Find MSB, which corresponds to the number of times
     	 * RPM doubles from start to end
     	 */
    /*	while (tmpi*=2 < tmp_max)
     {
     doubles++;
     mySUI.println("doubles++");
     }
     */

    //struct pattern_set {
    //  uint16_t beginning_ocr
    //  bool reset_prescale;
    //  byte prescaler_bits;
    //  uint16_t oc_step;
    //  uint16_t steps;
    //}SweepSteps[max_sweep_steps];
    last_prescaler = PRESCALE_1;
    low_tcnt = (long)(8000000.0/(((float)tmp_min)*Wheels[selected_wheel].rpm_scaler));
    low_rpm = tmp_min;
    while((i < 12) && (high_rpm < tmp_max))
    {
      high_tcnt = low_tcnt >> 1; /* divide by two */
      SweepSteps[i].prescaler_bits = check_and_adjust_tcnt_limits(&low_tcnt,&high_tcnt);
      SweepSteps[i].oc_step = (((1.0/low_rpm)*high_tcnt)*(tmp_rpm_per_sec/1000.0));
      SweepSteps[i].steps = (low_tcnt-high_tcnt)/SweepSteps[i].oc_step;
      if (SweepSteps[i].prescaler_bits == 4) {
        SweepSteps[i].oc_step /= 256;  /* Divide by 256 */
        SweepSteps[i].beginning_ocr = low_tcnt/256;  /* Divide by 256 */
        SweepSteps[i].ending_ocr = high_tcnt/256;  /* Divide by 256 */
      } else if (SweepSteps[i].prescaler_bits == 3) {
        SweepSteps[i].oc_step /= 64;  /* Divide by 64 */
        SweepSteps[i].beginning_ocr = low_tcnt/64;  /* Divide by 64 */
        SweepSteps[i].ending_ocr = high_tcnt/64;  /* Divide by 64 */
      } else if (SweepSteps[i].prescaler_bits == 2) {
        SweepSteps[i].oc_step /= 8;  /* Divide by 8 */
        SweepSteps[i].beginning_ocr = low_tcnt/8;  /* Divide by 8 */
        SweepSteps[i].ending_ocr = high_tcnt/8;  /* Divide by 8 */
      } else {
        SweepSteps[i].beginning_ocr = low_tcnt;  /* Divide by 1 */
        SweepSteps[i].ending_ocr = high_tcnt;  /* Divide by 1 */
      }
      
      if (last_prescaler != SweepSteps[i].prescaler_bits)
        SweepSteps[i].reset_prescale = 1;
      else 
        SweepSteps[i].reset_prescale = 0;
        
      mySUI.print(F("sweep step: "));
      mySUI.println(i);
      mySUI.print(F("Beginning tcnt: "));
      mySUI.println(low_tcnt);
      mySUI.print(F("ending tcnt: "));
      mySUI.println(high_tcnt);
      mySUI.print(F("Prescaled beginning tcnt: "));
      mySUI.println(SweepSteps[i].beginning_ocr);
      mySUI.print(F("Prescaled ending tcnt: "));
      mySUI.println(SweepSteps[i].ending_ocr);
      mySUI.print(F("prescaler: "));
      mySUI.println(SweepSteps[i].prescaler_bits);
      mySUI.print(F("steps: "));
      mySUI.println(SweepSteps[i].steps);
      mySUI.print(F("reset_prescale: "));
      mySUI.println(SweepSteps[i].reset_prescale);
      mySUI.print(F("OC_Step: "));
      mySUI.println(SweepSteps[i].oc_step);
      mySUI.print(F("End of step: "));
      mySUI.print(i);
      mySUI.print(F(" High RPM at end: "));
      high_rpm = (8000000/(Wheels[selected_wheel].rpm_scaler*high_tcnt));
      mySUI.println(high_rpm);
      low_tcnt = high_tcnt; // - SweepSteps[i].oc_step;	
      low_rpm = (uint16_t)((float)(8000000.0/low_tcnt)/Wheels[selected_wheel].rpm_scaler);
      last_prescaler = SweepSteps[i].prescaler_bits;
      mySUI.print(F("Low RPM for next step: "));
      mySUI.println(low_rpm);
      i++;
    }
  }
  else {
    mySUI.returnError("Range error !(50-50000)!");
  } 
  mySUI.returnOK();
}

int check_and_adjust_tcnt_limits(long *low_tcnt, long *high_tcnt) 
{

  if ((*low_tcnt >= 16777216) && (*high_tcnt >= 16777216))
  {
    return PRESCALE_256; /* Very low RPM condition wiht low edge pattern */
  }
  else if ((*low_tcnt >= 16777216) && (*high_tcnt >= 524288) && (*high_tcnt < 16777216))
  {
    *high_tcnt = 1677215;
    return PRESCALE_256;
  }
  else if ((*low_tcnt >= 524288) && (*low_tcnt < 16777216) && (*high_tcnt >= 1677216))
  {
    *low_tcnt = 1677215;
    return PRESCALE_256;
  }
  else if ((*low_tcnt >= 524288) && (*low_tcnt < 16777216) && (*high_tcnt >= 524288) && (*high_tcnt < 16777216))
  {
    return PRESCALE_64; 
  }
  else if ((*low_tcnt >= 524288) && (*low_tcnt < 16777216) && (*high_tcnt >= 65536) && (*high_tcnt < 524288))
  {
    *high_tcnt = 524287;
    return PRESCALE_64; 
  }
  else if ((*low_tcnt >= 65536) && (*low_tcnt < 524288) && (*high_tcnt >= 524288) && (*high_tcnt < 1677216))
  {
    *low_tcnt = 524287;
    return PRESCALE_64; 
  }
  else if ((*low_tcnt >= 65536) && (*low_tcnt < 524288) && (*high_tcnt >= 65536) && (*high_tcnt < 524288))
  {
    return PRESCALE_8; 
  }
  else if ((*low_tcnt >= 65536) && (*low_tcnt < 524288) && (*high_tcnt < 65536))
  {
    *high_tcnt = 65535;
    return PRESCALE_8; 
  }
  else if ((*low_tcnt < 65536) && (*high_tcnt >= 65536) && (*high_tcnt < 524288))
  {
    *low_tcnt = 65535;
    return PRESCALE_8; 
  }
  else
    return PRESCALE_1;
  return PRESCALE_1;
}

/* In the passed low/high params, one of them will cause a prescaler overflow
 * so we need to determine a new limit that stays inside the prescaler limits
 */


