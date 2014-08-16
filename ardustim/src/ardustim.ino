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

#include "enums.h"
#include "wheel_defs.h"
#include <avr/pgmspace.h>
#include <SerialUI.h>
#include "serialmenu.h"

#define MAX_SWEEP_STEPS 12

/* Sensistive stuff used in ISR's */
volatile uint32_t wanted_rpm = 6000; 
volatile uint16_t sweep_step_counter = 0;
volatile uint8_t selected_wheel = EIGHT_TOOTH_WITH_CAM;
/* Setting rpm to any value over 0 will enabled sweeping by default */
/* Stuff for handling prescaler changes (small tooth wheels are low RPM) */
volatile bool reset_prescaler = false;
volatile bool normal = true;
volatile bool sweep_reset_prescaler = true; /* Force sweep to reset prescaler value */
volatile bool sweep_lock = false;
volatile uint8_t output_invert_mask = 0x00; /* Don't invert anything */
volatile uint8_t sweep_direction = ASCENDING;
volatile byte total_sweep_stages = 0;
volatile int8_t sweep_stage = 0;
volatile uint8_t prescaler_bits = 0;
volatile uint8_t last_prescaler_bits = 0;
volatile uint8_t mode = FIXED_RPM;
volatile uint16_t new_OCR1A = 5000; /* sane default */
volatile uint16_t edge_counter = 0;

/* Less sensitive globals */
uint16_t sweep_low_rpm = 0;
uint16_t sweep_high_rpm = 0;
uint16_t sweep_rate = 0;

SUI::SerialUI mySUI = SUI::SerialUI(greeting);                                  

/* Where to store our sweep pattern sets */
typedef struct _pattern_set pattern_set;
struct _pattern_set {
  uint16_t beginning_ocr;
  uint16_t ending_ocr;
  uint8_t prescaler_bits;
  uint16_t oc_step;
  uint16_t steps;
}SweepSteps[MAX_SWEEP_STEPS];

/* Tie things wheel related into one nicer structure ... */
typedef struct _wheels wheels;
struct _wheels {
  const char *decoder_name PROGMEM;
  const unsigned char *edge_states_ptr PROGMEM;
  const float rpm_scaler;
  const uint16_t wheel_max_edges;
}Wheels[MAX_WHEELS] = {
  /* Pointer to friendly name string, pointer to edge array, RPM Scaler, Number of edges in the array */
  { dizzy_four_cylinder_friendly_name, dizzy_four_cylinder, 0.03333, 4 },
  { dizzy_six_cylinder_friendly_name, dizzy_six_cylinder, 0.05, 6 },
  { dizzy_eight_cylinder_friendly_name, dizzy_eight_cylinder, 0.06667, 8 },
  { sixty_minus_two_friendly_name, sixty_minus_two, 1.0, 120 },
  { sixty_minus_two_with_cam_friendly_name, sixty_minus_two_with_cam, 1.0, 240 },
  { thirty_six_minus_one_friendly_name, thirty_six_minus_one, 0.6, 72 },
  { four_minus_one_with_cam_friendly_name, four_minus_one_with_cam, 0.06667, 16 },
  { eight_minus_one_friendly_name, eight_minus_one, 0.13333, 16 },
  { six_minus_one_with_cam_friendly_name, six_minus_one_with_cam, 0.15, 36 },
  { twelve_minus_one_with_cam_friendly_name, twelve_minus_one_with_cam, 0.6, 144 },
  { fourty_minus_one_friendly_name, fourty_minus_one, 0.66667, 80 },
  { dizzy_four_trigger_return_friendly_name, dizzy_four_trigger_return, 0.15, 9 },
  { oddfire_vr_friendly_name, oddfire_vr, 0.2, 24 },
  { optispark_lt1_friendly_name, optispark_lt1, 3.0, 720 },
  { twelve_minus_three_friendly_name, twelve_minus_three, 0.4, 48 },
  { thirty_six_minus_two_two_two_friendly_name, thirty_six_minus_two_two_two, 0.6, 72 },
  { thirty_six_minus_two_two_two_with_cam_friendly_name, thirty_six_minus_two_two_two_with_cam, 0.6, 144 },
  { fourty_two_hundred_wheel_friendly_name, fourty_two_hundred_wheel, 0.6, 72 },
  { thirty_six_minus_one_with_cam_fe3_friendly_name, thirty_six_minus_one_with_cam_fe3, 0.6, 144 },
  { six_g_seventy_two_with_cam_friendly_name, six_g_seventy_two_with_cam, 0.6, 144 },
  { buell_oddfire_cam_friendly_name, buell_oddfire_cam, 0.33333, 80 },
  { gm_ls1_crank_and_cam_friendly_name, gm_ls1_crank_and_cam, 6.0, 720 },
  { lotus_thirty_six_minus_one_one_one_one_friendly_name, lotus_thirty_six_minus_one_one_one_one, 0.6, 72 },
  { honda_rc51_with_cam_friendly_name, honda_rc51_with_cam, 0.2, 48 },
  { thirty_six_minus_one_with_second_trigger_friendly_name, thirty_six_minus_one_with_second_trigger, 0.6, 144 },
  { thirty_six_minus_one_plus_one_with_cam_ngc4_friendly_name, thirty_six_minus_one_plus_one_with_cam_ngc4, 3.0, 720 },
  { weber_iaw_with_cam_friendly_name, weber_iaw_with_cam, 1.2, 144 },
  { fiat_one_point_eight_sixteen_valve_with_cam_friendly_name, fiat_one_point_eight_sixteen_valve_with_cam, 3.0, 720 },
  { three_sixty_nissan_cas_friendly_name, three_sixty_nissan_cas, 3.0, 720 },
  { twenty_four_minus_two_with_second_trigger_friendly_name, twenty_four_minus_two_with_second_trigger, 0.3, 72 },
  { eight_tooth_with_cam_friendly_name, eight_tooth_with_cam, 0.26667, 64 },
};

/* Initialization */
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
  // Enable output compare interrupt ffor timer channel 1 (16 bit)
  TIMSK1 |= (1 << OCIE1A);

  // Set timer2 to run sweeper routine
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // Set compare register to sane default
  OCR2A = 249;  /* With prescale of x64 gives 1ms tick */

  // Turn on CTC mode
  TCCR2A |= (1 << WGM21); // Normal mode (not PWM)
  // Set prescaler to x64
  TCCR2B |= (1 << CS22); /* Prescaler of 64 */
  // Enable output compare interrupt for timer channel 2
  TIMSK2 |= (1 << OCIE2A);


  pinMode(7, OUTPUT); /* Debug pin for Saleae to track sweep ISR execution speed */
  pinMode(8, OUTPUT); /* Primary (crank usually) output */
  pinMode(9, OUTPUT); /* Secondary (cam usually) output */
  pinMode(10, OUTPUT); /* Knock signal for seank, ony on LS1 pattern, NOT IMPL YET */

  sei(); // Enable interrupts
} // End setup


ISR(TIMER2_COMPA_vect) {
  PORTD = (1 << 7);
  if ( mode != LINEAR_SWEPT_RPM)
  {
    PORTD = (0 << 7);
    return;
  }
  if (sweep_lock)  // semaphore to protect around changes/critical sections
  {  
    PORTD = (0 << 7);
    return;
  }
  sweep_lock = true;
  if (sweep_reset_prescaler)
  {
    reset_prescaler = true;
    sweep_reset_prescaler = false;
    prescaler_bits = SweepSteps[sweep_stage].prescaler_bits;
    last_prescaler_bits = prescaler_bits;  
    new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;  
  }
  /* Sweep code */
  /*
  struct _pattern_set {
    uint16_t beginning_ocr;
    uint16_t ending_ocr;
    uint8_t prescaler_bits;
    uint16_t oc_step;
    uint16_t steps;
  }SweepSteps[MAX_SWEEP_STEPS];
  */
  if (sweep_direction == ASCENDING)
  {
//    PORTD |= 1 << 7;  /* Debugginga, ascending */
    if (sweep_step_counter < SweepSteps[sweep_stage].steps)
    {
      new_OCR1A -= SweepSteps[sweep_stage].oc_step;
      sweep_step_counter++;
    }
    else /* END of the stage, find out where we are */
    {
      sweep_stage++;
      if (sweep_stage < total_sweep_stages)
      {
        new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;
        sweep_step_counter = 0; /* we got the "0'th value in the previous line */
        if (SweepSteps[sweep_stage].prescaler_bits != last_prescaler_bits)
        {
          reset_prescaler = true;
          prescaler_bits = SweepSteps[sweep_stage].prescaler_bits;
          last_prescaler_bits = prescaler_bits;
        }
      }
      else /* END of line, time to reverse direction */
      {
        sweep_stage--; /*Bring back within limits */
        sweep_direction = DESCENDING;
        new_OCR1A = SweepSteps[sweep_stage].ending_ocr;
        sweep_step_counter = SweepSteps[sweep_stage].steps; /* we got the last value in hte previous line */
      }
    }
  }
  else /* Descending */
  {
//      PORTD &= ~(1<<7);  /*Descending  turn pin off */
    if (sweep_step_counter > 0)
    {
      new_OCR1A += SweepSteps[sweep_stage].oc_step;
      sweep_step_counter--;
    }
    else /* End of stage */
    {
      sweep_stage--;
      if (sweep_stage >= 0)
      {
        new_OCR1A = SweepSteps[sweep_stage].ending_ocr;
        sweep_step_counter = SweepSteps[sweep_stage].steps;
        if (SweepSteps[sweep_stage].prescaler_bits != last_prescaler_bits)
        {
          reset_prescaler = true;
          prescaler_bits = SweepSteps[sweep_stage].prescaler_bits;
          last_prescaler_bits = prescaler_bits;
        }
      }
      else /*End of the line */
      {
        sweep_stage++; /*Bring back within limits */
        sweep_direction = ASCENDING;
        new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;
        sweep_step_counter = 0; /* we got the last value in hte previous line */
      }
    }
  }
  sweep_lock = false;
  PORTD = (0 << 7);
}

/* Pumps the pattern out of flash to the port 
 * The rate at which this runs is dependent on what OCR1A is set to
 * the sweeper in timer2 alters this on the fly to alow changing of RPM
 * in a very nice way
 */
ISR(TIMER1_COMPA_vect) {
  /* This is VERY simple, just walk the array and wrap when we hit the limit */
  PORTB = output_invert_mask ^ pgm_read_byte(&Wheels[selected_wheel].edge_states_ptr[edge_counter]);   /* Write it to the port */
  /* Normal direction  overflow handling */
  if (normal)
  {
    edge_counter++;
    if (edge_counter == Wheels[selected_wheel].wheel_max_edges) {
      edge_counter = 0;
    }
  }
  else
  {
    if (edge_counter == 0)
      edge_counter = Wheels[selected_wheel].wheel_max_edges;
    edge_counter--;
  }
  /* The tables are in flash so we need pgm_read_byte() */

  /* Reset Prescaler only if flag is set */
  if (reset_prescaler)
  {
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); /* Clear CS10, CS11 and CS12 */
    TCCR1B |= prescaler_bits;
    reset_prescaler = false;
  }
  /* Reset next compare value for RPM changes */
  OCR1A = new_OCR1A;  /* Apply new "RPM" from Timer2 ISR, i.e. speed up/down the virtual "wheel" */
}

void loop() {
  /* Just handle the Serial UI, everything else is in 
   * interrupt handlers or callbacks from SerialUI.
   */

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

void reset_new_OCR1A(uint32_t new_rpm)
{
  long tmpl = 0;
  long tmp2 = 0;
  tmpl = (long)(8000000.0/(Wheels[selected_wheel].rpm_scaler * (float)new_rpm));
/*  mySUI.print(F("new_OCR1a: "));
  mySUI.println(tmpl);
  */
  tmp2 = (long)check_and_adjust_tcnt_limits(&prescaler_bits, &tmpl, &tmpl);
  /*
  mySUI.print(F("new_OCR1a: "));
  mySUI.println(tmp2);
  */
  new_OCR1A = (uint16_t)tmp2; 
  reset_prescaler = true; 
}


void sweep_rpm()
{
  byte count = 0;
  uint16_t tmp_min = 0;
  uint16_t tmp_max = 0;
  uint16_t tmp_rpm_per_sec = 0;
  uint16_t end_tcnt = 0;
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
    (tmp_min >= 10) &&
    (tmp_max < 51200) &&
    (tmp_rpm_per_sec > 0) &&
    (tmp_rpm_per_sec < 51200) &&
    (tmp_min < tmp_max))
  {
    sweep_low_rpm = tmp_min;
    sweep_high_rpm = tmp_max;
    sweep_rate = tmp_rpm_per_sec;
    //struct pattern_set {
    //  uint16_t beginning_ocr
    //  bool reset_prescale;
    //  byte prescaler_bits;
    //  uint16_t oc_step;
    //  uint16_t steps;
    //}SweepSteps[max_sweep_steps];
    sweep_lock = true;
    low_tcnt = (long)(8000000.0/(((float)tmp_min)*Wheels[selected_wheel].rpm_scaler));
    high_tcnt = low_tcnt >> 1; /* divide by two */
    low_rpm = tmp_min;
    end_tcnt = 8000000/(tmp_max*Wheels[selected_wheel].rpm_scaler);

    while((i < MAX_SWEEP_STEPS) && (high_rpm < tmp_max))
    {
      check_and_adjust_tcnt_limits(&SweepSteps[i].prescaler_bits, &low_tcnt,&high_tcnt);
      if (high_tcnt < end_tcnt) /* Prevent overshoot */
        high_tcnt = end_tcnt;
      SweepSteps[i].oc_step = (((1.0/low_rpm)*high_tcnt)*(tmp_rpm_per_sec/1000.0));
      if (SweepSteps[i].oc_step == 0)
        SweepSteps[i].oc_step = 1;
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
      mySUI.print(F("OC_Step: "));
      mySUI.println(SweepSteps[i].oc_step);
      mySUI.print(F("End of step: "));
      mySUI.print(i);
      mySUI.print(F(" High RPM at end: "));
      high_rpm = (8000000/(Wheels[selected_wheel].rpm_scaler*high_tcnt));
      mySUI.println(high_rpm);
      high_tcnt >>= 1; //  SweepSteps[i].oc_step;eset for next round.

      low_tcnt = 8000000/((high_rpm + (tmp_rpm_per_sec/1000))*Wheels[selected_wheel].rpm_scaler);
      low_rpm =  (uint16_t)((float)(8000000.0/low_tcnt)/Wheels[selected_wheel].rpm_scaler);
      mySUI.print(F("Low RPM for next step: "));
      mySUI.println(low_rpm);
      i++;
    }
    total_sweep_stages = i;
    mySUI.print(F("Total sweep stages: "));
    mySUI.println(total_sweep_stages);
      i++;
  }
  else {
    mySUI.returnError("Range error !(50-50000)!");
  } 
  mySUI.returnOK();
  /* Reset params for Timer2 ISR */
  sweep_stage = 0;
  sweep_step_counter = 0;
  sweep_direction = ASCENDING;
  sweep_reset_prescaler = true;
  mode = LINEAR_SWEPT_RPM;
  sweep_lock = false;
}

uint16_t check_and_adjust_tcnt_limits(volatile byte *prescale_bits, long *low_tcnt, long *high_tcnt) 
{
  /* Really Really LOW RPM */
  if ((*low_tcnt >= 16777216) && (*high_tcnt >= 16777216))
  {
     *prescale_bits = PRESCALE_1024; /* Very low RPM condition with low edge pattern */
    return (uint16_t)(*low_tcnt/1024);
  }
  else if ((*low_tcnt >= 16777216) && (*high_tcnt >= 4194304) && (*high_tcnt < 16777216))
  {
    *high_tcnt = 16777215;
    *prescale_bits = PRESCALE_1024;
    return (uint16_t)(*low_tcnt/1024);
  }
  else if ((*low_tcnt >= 4194304) && (*low_tcnt < 16777216) && (*high_tcnt >= 16777216))
  {
    *low_tcnt = 16777215;
    *prescale_bits = PRESCALE_1024;
    return (uint16_t)(*low_tcnt/1024);
  }
  else if ((*low_tcnt >= 4194304) && (*low_tcnt < 16777216) && (*high_tcnt >= 4194304) && (*high_tcnt < 16777216))
  {
    *prescale_bits = PRESCALE_256; 
    return (uint16_t)(*low_tcnt/256);
  }
  else if ((*low_tcnt >= 4194304) && (*low_tcnt < 16777216) && (*high_tcnt >= 524288) && (*high_tcnt < 4194304))
  {
    *high_tcnt = 524287;
    *prescale_bits = PRESCALE_256; 
    return (uint16_t)(*low_tcnt/256);
  }
  else if ((*low_tcnt >= 524288) && (*low_tcnt < 4194304) && (*high_tcnt >= 4194304) && (*high_tcnt < 16777216))
  {
    *low_tcnt = 524287;
    *prescale_bits = PRESCALE_256; 
    return (uint16_t)(*low_tcnt/256);
  }
  else if ((*low_tcnt >= 524288) && (*low_tcnt < 4194304) && (*high_tcnt >= 524288) && (*high_tcnt < 4194304))
  {
    *prescale_bits = PRESCALE_64; 
    return (uint16_t)(*low_tcnt/64);
  }
  else if ((*low_tcnt >= 524288) && (*low_tcnt < 4194304) && (*high_tcnt >= 65536) && (*high_tcnt < 524288))
  {
    *high_tcnt = 524287;
    *prescale_bits = PRESCALE_64; 
    return (uint16_t)(*low_tcnt/64);
  }
  else if ((*low_tcnt >= 65536) && (*low_tcnt < 524288) && (*high_tcnt >= 524288) && (*high_tcnt < 4194304))
  {
    *low_tcnt = 524287;
    *prescale_bits = PRESCALE_64; 
    return (uint16_t)(*low_tcnt/64);
  }
  else if ((*low_tcnt >= 65536) && (*low_tcnt < 524288) && (*high_tcnt >= 65536) && (*high_tcnt < 524288))
  {
    *prescale_bits = PRESCALE_8; 
    return (uint16_t)(*low_tcnt/8);
  }
  else if ((*low_tcnt >= 65536) && (*low_tcnt < 524288) && (*high_tcnt < 65536))
  {
    *high_tcnt = 65535;
    *prescale_bits = PRESCALE_8; 
    return (uint16_t)(*low_tcnt/8);
  }
  else if ((*low_tcnt < 65536) && (*high_tcnt >= 65536) && (*high_tcnt < 524288))
  {
    *low_tcnt = 65535;
    *prescale_bits = PRESCALE_8; 
    return (uint16_t)(*low_tcnt/8);
  }
  else
    *prescale_bits = PRESCALE_1;
  *prescale_bits = PRESCALE_1;
  return (uint16_t)*low_tcnt;
}

/* In the passed low/high params, one of them will cause a prescaler overflow
 * so we need to determine a new limit that stays inside the prescaler limits
 */


