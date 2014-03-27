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

#include "wheel_defs.h"
#include <avr/pgmspace.h>
#include <SerialUI.h>
#include "serialmenu.h"

int check_and_adjust_tcnt_limits(long *, long *) ;

/* Setting rpm to any value over 0 will enabled sweeping by default */
uint16_t rpm_min = 100;
uint16_t rpm_max = 3000;
uint16_t rpm_step = 5;
uint16_t rpm_step_delay = 2;
const byte max_sweep_steps = 12;
volatile unsigned long wanted_rpm = 4000; /* Used ONLY when RPM_STEP is 0 above, otherwise it's the starting point... */
long previousMillis = 0;

SUI::SerialUI mySUI = SUI::SerialUI(greeting);

volatile uint16_t edge_counter = 0;

/* Stuff for handling prescaler changes (small tooth wheels are low RPM) */
volatile byte reset_prescaler = 0;
volatile byte BIT_CS10 = 0;
volatile byte BIT_CS11 = 0;
volatile byte BIT_CS12 = 0;

enum { 
  PRESCALE_1 = 1, 
  PRESCALE_8 = 2,
  PRESCALE_64 = 3, 
  PRESCALE_256 = 4,
  RECALC
};

byte last_prescale = PRESCALE_1;
byte new_prescale = PRESCALE_1;

enum { 
  DESCENDING, 
  ASCENDING 
};

enum {
  SWEEPING_RPM,
  FIXED_RPM,
};

/* Prototypes */
void run_ardustim(void);

byte mode = FIXED_RPM;

byte sweep_state = ASCENDING;
volatile uint16_t new_OCR1A = 5000; /* sane default */
volatile byte selected_wheel = TWENTY_FOUR_MINUS_TWO_WITH_SECOND_TRIGGER;
volatile uint16_t total_sweep_steps = 0;

struct pattern_set {
  uint16_t beginning_ocr;
  bool reset_prescale;
  byte prescaler_bits;
  uint16_t oc_step;
  uint16_t steps;
}
SweepSteps[max_sweep_steps];

/* Tie things into one nicer structure ... */
struct wheels {
  prog_char *decoder_name;
  prog_uchar *edge_states_ptr;
  const float rpm_scaler;
  const uint16_t wheel_max_edges;
} 
Wheels[MAX_WHEELS] = {

  /* Pointer to friendly name string, pointer to edge array, RPM Scaler, Number of edges in the array */
  { 
    dizzy_four_cylinder_friendly_name, dizzy_four_cylinder, 0.03333, 4         }
  ,
  { 
    dizzy_six_cylinder_friendly_name, dizzy_six_cylinder, 0.05, 6         }
  ,
  { 
    dizzy_eight_cylinder_friendly_name, dizzy_eight_cylinder, 0.06667, 8         }
  ,
  { 
    sixty_minus_two_friendly_name, sixty_minus_two, 1.0, 120         }
  ,
  { 
    sixty_minus_two_with_cam_friendly_name, sixty_minus_two_with_cam, 1.0, 240         }
  ,
  { 
    thirty_six_minus_one_friendly_name, thirty_six_minus_one, 0.6, 72         }
  ,
  { 
    four_minus_one_with_cam_friendly_name, four_minus_one_with_cam, 0.06667, 16         }
  ,
  { 
    eight_minus_one_friendly_name, eight_minus_one, 0.13333, 16         }
  ,
  { 
    six_minus_one_with_cam_friendly_name, six_minus_one_with_cam, 0.15, 36         }
  ,
  { 
    twelve_minus_one_with_cam_friendly_name, twelve_minus_one_with_cam, 0.6, 144         }
  ,
  { 
    fourty_minus_one_friendly_name, fourty_minus_one, 0.66667, 80         }
  ,
  { 
    dizzy_trigger_return_friendly_name, dizzy_trigger_return, 0.075, 9         }
  ,
  { 
    oddfire_vr_friendly_name, oddfire_vr, 0.2, 24         }
  ,
  { 
    optispark_lt1_friendly_name, optispark_lt1, 3.0, 720         }
  ,
  { 
    twelve_minus_three_friendly_name, twelve_minus_three, 0.4, 48         }
  ,
  { 
    thirty_six_minus_two_two_two_friendly_name, thirty_six_minus_two_two_two, 0.6, 72         }
  ,
  { 
    thirty_six_minus_two_two_two_with_cam_friendly_name, thirty_six_minus_two_two_two_with_cam, 0.15, 144         }
  ,
  { 
    fourty_two_hundred_wheel_friendly_name, fourty_two_hundred_wheel, 0.6, 72         }
  ,
  { 
    thirty_six_minus_one_with_cam_fe3_friendly_name, thirty_six_minus_one_with_cam_fe3, 0.6, 144         }
  ,
  { 
    six_g_seventy_two_with_cam_friendly_name, six_g_seventy_two_with_cam, 0.6, 144         }
  ,
  { 
    buell_oddfire_cam_friendly_name, buell_oddfire_cam, 0.33333, 80         }
  ,
  { 
    gm_ls1_crank_and_cam_friendly_name, gm_ls1_crank_and_cam, 6.0, 720         }
  ,
  { 
    lotus_thirty_six_minus_one_one_one_one_friendly_name, lotus_thirty_six_minus_one_one_one_one, 0.6, 72         }
  ,
  { 
    honda_rc51_with_cam_friendly_name, honda_rc51_with_cam, 0.2, 48         }
  ,
  { 
    thirty_six_minus_one_with_second_trigger_friendly_name, thirty_six_minus_one_with_second_trigger, 0.6, 144         }
  ,
  { 
    thirty_six_minus_one_plus_one_with_cam_ngc4_friendly_name, thirty_six_minus_one_plus_one_with_cam_ngc4, 3.0, 720         }
  ,
  { 
    weber_iaw_with_cam_friendly_name, weber_iaw_with_cam, 0.6, 144         }
  ,
  { 
    fiat_one_point_eight_sixteen_valve_with_cam_friendly_name, fiat_one_point_eight_sixteen_valve_with_cam, 3.0, 720         }
  ,
  { 
    three_sixty_nissan_cas_friendly_name, three_sixty_nissan_cas, 3.0, 720         }
  ,
  { 
    twenty_four_minus_two_with_second_trigger_friendly_name, twenty_four_minus_two_with_second_trigger, 0.3, 72         }
  ,
};

void setup() {
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

  cli(); // stop interrupts

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

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  sei(); // Enable interrupts
} // End setup

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

  if (mySUI.checkForUserOnce())
  {
    // Someone connected!
    mySUI.enter();
    while (mySUI.userPresent()) 
    {
      mySUI.handleRequests();

      check_to_run_ardustim();
    }
    check_to_run_ardustim();
  }
  check_to_run_ardustim();
}

void check_to_run_ardustim()
{
  unsigned long currentMillis = millis();
  if (mode == SWEEPING_RPM)
  {
    if (currentMillis - previousMillis > rpm_step_delay)
    {
      previousMillis = currentMillis;
      run_ardustim();
    }
  }
  else
    run_ardustim();
}


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
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

/* SerialUI Callbacks */
void show_info()
{
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
  else {
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
}

void select_wheel()
{
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

  mySUI.print("New wheel is ");
  mySUI.print(selected_wheel+1);
  mySUI.print_P(colon_space);
  mySUI.println_P(Wheels[selected_wheel].decoder_name);
  mySUI.returnOK();
}

void select_previous_wheel()
{
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
      } else if (SweepSteps[i].prescaler_bits == 3) {
        SweepSteps[i].oc_step /= 64;  /* Divide by 64 */
        SweepSteps[i].beginning_ocr = low_tcnt/64;  /* Divide by 64 */
      } else if (SweepSteps[i].prescaler_bits == 2) {
        SweepSteps[i].oc_step /= 8;  /* Divide by 8 */
        SweepSteps[i].beginning_ocr = low_tcnt/8;  /* Divide by 8 */
      } else {
        SweepSteps[i].beginning_ocr = low_tcnt;  /* Divide by 1 */
      }
      
      if (last_prescaler != SweepSteps[i].prescaler_bits)
        SweepSteps[i].reset_prescale = 1;
      else 
        SweepSteps[i].reset_prescale = 0;
        
      mySUI.print(F("sweep step: "));
      mySUI.println(i);
      mySUI.print(F("Beginning tcnt: "));
      mySUI.println(low_tcnt);
      mySUI.print(F("Prescaled tcnt: "));
      mySUI.println(SweepSteps[i].beginning_ocr);
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
    mySUI.returnError("Parameter error!");
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


