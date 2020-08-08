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
#include "user_defaults.h"
#include "wheel_defs.h"
#include <avr/pgmspace.h>
#include <EEPROM.h>

/* Sensistive stuff used in ISR's */
volatile uint8_t fraction = 0;
volatile uint16_t adc0; /* POT RPM */
volatile uint16_t adc1; /* Pot Wheel select */
volatile uint32_t oc_remainder = 0;
/* Setting rpm to any value over 0 will enabled sweeping by default */
/* Stuff for handling prescaler changes (small tooth wheels are low RPM) */
volatile uint8_t analog_port = 0;
volatile bool adc0_read_complete = false;
volatile bool adc1_read_complete = false;
volatile bool reset_prescaler = false;
volatile bool normal = true;
volatile bool sweep_reset_prescaler = true; /* Force sweep to reset prescaler value */
volatile bool sweep_lock = false;
volatile uint8_t output_invert_mask = 0x00; /* Don't invert anything */
volatile uint8_t sweep_direction = ASCENDING;
volatile byte total_sweep_stages = 0;
volatile uint8_t sweep_stage = 0;
volatile uint8_t prescaler_bits = 0;
volatile uint8_t last_prescaler_bits = 0;
volatile uint8_t mode = FIXED_RPM;
volatile uint16_t new_OCR1A = 5000; /* sane default */
volatile uint16_t edge_counter = 0;

/* Less sensitive globals */
uint8_t bitshift = 0;
uint16_t sweep_low_rpm = 250;
uint16_t sweep_high_rpm = 4000;
uint16_t sweep_rate = 1;                               

sweep_step *SweepSteps;  /* Global pointer for the sweep steps */

wheels Wheels[MAX_WHEELS] = {
  /* Pointer to friendly name string, pointer to edge array, RPM Scaler, Number of edges in the array */
  { dizzy_four_cylinder_friendly_name, dizzy_four_cylinder, 0.003333, 4, 360 },
  { dizzy_six_cylinder_friendly_name, dizzy_six_cylinder, 0.05, 6, 360 },
  { dizzy_eight_cylinder_friendly_name, dizzy_eight_cylinder, 0.06667, 8, 360 },
  { sixty_minus_two_friendly_name, sixty_minus_two, 1.0, 120, 360 },
  { sixty_minus_two_with_cam_friendly_name, sixty_minus_two_with_cam, 1.0, 240, 720 },
  { thirty_six_minus_one_friendly_name, thirty_six_minus_one, 0.6, 72, 360 },
  { twenty_four_minus_one_friendly_name, twenty_four_minus_one, 0.5, 48, 360 },
  { four_minus_one_with_cam_friendly_name, four_minus_one_with_cam, 0.06667, 16, 720 },
  { eight_minus_one_friendly_name, eight_minus_one, 0.13333, 16, 360 },
  { six_minus_one_with_cam_friendly_name, six_minus_one_with_cam, 0.15, 36, 720 },
  { twelve_minus_one_with_cam_friendly_name, twelve_minus_one_with_cam, 0.6, 144, 720 },
  { fourty_minus_one_friendly_name, fourty_minus_one, 0.66667, 80, 360 },
  { dizzy_four_trigger_return_friendly_name, dizzy_four_trigger_return, 0.15, 9, 720 },
  { oddfire_vr_friendly_name, oddfire_vr, 0.2, 24, 360 },
  { optispark_lt1_friendly_name, optispark_lt1, 3.0, 720, 720 },
  { twelve_minus_three_friendly_name, twelve_minus_three, 0.4, 48, 360 },
  { thirty_six_minus_two_two_two_friendly_name, thirty_six_minus_two_two_two, 0.6, 72, 360 },
  { thirty_six_minus_two_two_two_with_cam_friendly_name, thirty_six_minus_two_two_two_with_cam, 0.6, 144, 720 },
  { fourty_two_hundred_wheel_friendly_name, fourty_two_hundred_wheel, 0.6, 72, 360 },
  { thirty_six_minus_one_with_cam_fe3_friendly_name, thirty_six_minus_one_with_cam_fe3, 0.6, 144, 720 },
  { six_g_seventy_two_with_cam_friendly_name, six_g_seventy_two_with_cam, 0.6, 144, 720 },
  { buell_oddfire_cam_friendly_name, buell_oddfire_cam, 0.33333, 80, 720 },
  { gm_ls1_crank_and_cam_friendly_name, gm_ls1_crank_and_cam, 6.0, 720, 720 },
  { lotus_thirty_six_minus_one_one_one_one_friendly_name, lotus_thirty_six_minus_one_one_one_one, 0.6, 72, 360 },
  { honda_rc51_with_cam_friendly_name, honda_rc51_with_cam, 0.2, 48, 720 },
  { thirty_six_minus_one_with_second_trigger_friendly_name, thirty_six_minus_one_with_second_trigger, 0.6, 144, 720 },
  { thirty_six_minus_one_plus_one_with_cam_ngc4_friendly_name, thirty_six_minus_one_plus_one_with_cam_ngc4, 3.0, 720, 720 },
  { weber_iaw_with_cam_friendly_name, weber_iaw_with_cam, 1.2, 144, 720 },
  { fiat_one_point_eight_sixteen_valve_with_cam_friendly_name, fiat_one_point_eight_sixteen_valve_with_cam, 3.0, 720, 720 },
  { three_sixty_nissan_cas_friendly_name, three_sixty_nissan_cas, 3.0, 720, 720 },
  { twenty_four_minus_two_with_second_trigger_friendly_name, twenty_four_minus_two_with_second_trigger, 0.3, 72, 720 },
  { yamaha_eight_tooth_with_cam_friendly_name, yamaha_eight_tooth_with_cam, 0.26667, 64, 720 },
  { gm_four_tooth_with_cam_friendly_name, gm_four_tooth_with_cam, 0.06666, 8, 720 },
  { gm_six_tooth_with_cam_friendly_name, gm_six_tooth_with_cam, 0.1, 12, 720 },
  { gm_eight_tooth_with_cam_friendly_name, gm_eight_tooth_with_cam, 0.13333, 16, 720 },
  { volvo_d12acd_with_cam_friendly_name, volvo_d12acd_with_cam, 4.0, 480, 720 },
  { mazda_thirty_six_minus_two_two_two_with_six_tooth_cam_friendly_name, mazda_thirty_six_minus_two_two_two_with_six_tooth_cam, 1.5, 360, 720 },
  { mitsubishi_4g63_4_2_friendly_name, mitsubishi_4g63_4_2, 0.6, 144, 720 },
  { audi_135_with_cam_friendly_name, audi_135_with_cam, 1.5, 1080, 720 },
  { honda_d17_no_cam_friendly_name, honda_d17_no_cam, 0.6, 144, 720 },
  { mazda_323_au_friendly_name, mazda_323_au, 1, 30, 720 },
  { daihatsu_3cyl_friendly_name, daihatsu_3cyl, 0.8, 144, 360 },
  { miata_9905_friendly_name, miata_9905, 0.6, 144, 720 },
  { twelve_with_cam_friendly_name, twelve_with_cam, 0.6, 144, 720 },
  { twenty_four_with_cam_friendly_name, twelve_with_cam, 0.6, 144, 720 },
  { subaru_six_seven_name_friendly_name, subaru_six_seven, 3.0, 720, 720 },
  { gm_seven_x_friendly_name, gm_seven_x, 1.502, 180, 720 },
  { four_twenty_a_friendly_name, four_twenty_a, 0.6, 144, 720 },
  { ford_st170_friendly_name, ford_st170, 0.6, 720, 720 },
  { mitsubishi_3A92_friendly_name, mitsubishi_3A92, 0.6, 144, 720 },
  { spark_dizzy_4cyl_a_friendly_name, spark_dizzy_4cyl, 0.6, 144, 720 } 
};

/* Initialization */
void setup() {
  serialSetup();

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
  // Enable output compare interrupt for timer channel 1 (16 bit)
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


  /* Configure ADC as per http://www.glennsweeney.com/tutorials/interrupt-driven-analog-conversion-with-an-atmega328p */
  // clear ADLAR in ADMUX (0x7C) to right-adjust the result
  // ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
  ADMUX &= B11011111;
  
  // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  // proper source (01)
  ADMUX |= B01000000;
  
  // Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog
  // input
  ADMUX &= B11110000;
  
  // Set MUX3..0 in ADMUX (0x7C) to read from AD8 (Internal temp)
  // Do not set above 15! You will overrun other parts of ADMUX. A full
  // list of possible inputs is available in Table 24-4 of the ATMega328
  // datasheet
  // ADMUX |= 8;
  // ADMUX |= B00001000; // Binary equivalent
  
  // Set ADEN in ADCSRA (0x7A) to enable the ADC.
  // Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;
  
  // Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
  ADCSRA |= B00100000;
  
  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  ADCSRB &= B11111000;
  
  // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
  // Above 200KHz 10-bit results are not reliable.
  ADCSRA |= B00000111;
  
  // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  // Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;

//  pinMode(7, OUTPUT); /* Debug pin for Saleae to track sweep ISR execution speed */
  pinMode(8, OUTPUT); /* Primary (crank usually) output */
  pinMode(9, OUTPUT); /* Secondary (cam usually) output */
  pinMode(10, OUTPUT); /* Knock signal for seank, ony on LS1 pattern, NOT IMPL YET */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  pinMode(53, OUTPUT); 
  pinMode(52, OUTPUT); 
#endif

  sei(); // Enable interrupts
  // Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  ADCSRA |= B01000000;
  /* Make sure we are using the DEFAULT RPM on startup */
  reset_new_OCR1A(wanted_rpm); 

  //Set RPM mode
  mode = EEPROM.read(EEPROM_LAST_MODE);
  if(mode > POT_RPM) { mode = POT_RPM; }


} // End setup


//! ADC ISR for alternating between ADC pins 0 and 1
/*!
 * Reads ADC ports 0 and 1 alternately. Port 0 is RPM, Port 1 is for
 * future fun (possible wheel selection)
 */
ISR(ADC_vect){
  if (analog_port == 0)
  {
    adc0 = ADCL | (ADCH << 8);
	  adc0_read_complete = true;
    /* Flip to channel 1 */
    //ADMUX |= B00000001;
    //analog_port = 1;
    /* Trigger another conversion */
    //ADCSRA |= B01000000;
    return;
  } 
//  if (analog_port == 1)
//  {
//    adc1 = ADCL | (ADCH << 8);
//	adc1_read_complete = true;
//    /* Flip to channel 0 */
//    /* Tell it to read ADC0, clear MUX0..3 */
//    ADMUX &= B11110000;
//    analog_port = 0;
//    /* Trigger another conversion */
//    ADCSRA |= B01000000;
//    return;
//  }
}


/* This is the "low speed" 1000x/second sweeper interrupt routine
 * who's sole purpose in life is to reset the output compare value
 * for timer zero to change the output RPM.  In cases where the RPM
 * change per ISR is LESS than one LSB of the counter a set of modulus
 * variabels are used to handle fractional values.
 */
ISR(TIMER2_COMPA_vect) {
//  PORTD = (1 << 7);
  if ( mode != LINEAR_SWEPT_RPM)
  {
//    PORTD = (0 << 7);
    return;
  }
  if (sweep_lock)  // semaphore to protect around changes/critical sections
  {  
 //   PORTD = (0 << 7);
    return;
  }
  sweep_lock = true;
  if (sweep_reset_prescaler)
  {
    sweep_reset_prescaler = false;
    reset_prescaler = true;
    prescaler_bits = SweepSteps[sweep_stage].prescaler_bits;
    last_prescaler_bits = prescaler_bits;  
  }
  /* Sweep code */
  if (sweep_direction == ASCENDING)
  {
    oc_remainder += SweepSteps[sweep_stage].remainder_per_isr;
    /* IF the total is over the threshold we increment the TCNT factor
     * for each multiple it is over by
     */
    while (oc_remainder > FACTOR_THRESHOLD)
    {
      fraction++;
      oc_remainder -= FACTOR_THRESHOLD;
    }
    if (new_OCR1A > SweepSteps[sweep_stage].ending_ocr)
    {
      new_OCR1A -= (SweepSteps[sweep_stage].tcnt_per_isr + fraction);
      fraction = 0;
    }
    else /* END of the stage, find out where we are */
    {
      sweep_stage++;
      oc_remainder = 0;
      if (sweep_stage < total_sweep_stages)
      {
        /* Toggle  when changing stages */
        //PORTD &= ~(1<<7); /* turn DBG pin off */
        //PORTD |= (1<<7);  /* Turn DBG pin on */
        new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;
        if (SweepSteps[sweep_stage].prescaler_bits != last_prescaler_bits)
          sweep_reset_prescaler = true;
      }
      else /* END of line, time to reverse direction */
      {
        sweep_stage--; /*Bring back within limits */
        sweep_direction = DESCENDING;
        new_OCR1A = SweepSteps[sweep_stage].ending_ocr;
        if (SweepSteps[sweep_stage].prescaler_bits != last_prescaler_bits)
          sweep_reset_prescaler = true;
        PORTD |= 1 << 7;  /* Debugginga, ascending */
      }
      /* Reset fractionals or next round */
    }
  }
  else /* Descending */
  {
    oc_remainder += SweepSteps[sweep_stage].remainder_per_isr;
    while (oc_remainder > FACTOR_THRESHOLD)
    {
      fraction++;
      oc_remainder -= FACTOR_THRESHOLD;
    }
    if (new_OCR1A < SweepSteps[sweep_stage].beginning_ocr)
    {
      new_OCR1A += (SweepSteps[sweep_stage].tcnt_per_isr + fraction);
      fraction = 0;
    }
    else /* End of stage */
    {
      sweep_stage--;
      oc_remainder = 0;
      if (sweep_stage >= 0)
      {
        new_OCR1A = SweepSteps[sweep_stage].ending_ocr;
        if (SweepSteps[sweep_stage].prescaler_bits != last_prescaler_bits)
          sweep_reset_prescaler = true;
      }
      else /*End of the line */
      {
        sweep_stage++; /*Bring back within limits */
        sweep_direction = ASCENDING;
        new_OCR1A = SweepSteps[sweep_stage].beginning_ocr;
        if (SweepSteps[sweep_stage].prescaler_bits != last_prescaler_bits)
          sweep_reset_prescaler = true;
        PORTD &= ~(1<<7);  /*Descending  turn pin off */
      }
    }
  }
  sweep_lock = false;
  //wanted_rpm = get_rpm_from_tcnt(&SweepSteps[sweep_stage].beginning_ocr, &SweepSteps[sweep_stage].prescaler_bits);
//  PORTD = (0 << 7);
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

void loop() 
{
  uint16_t tmp_rpm = 0;
  /* Just handle the Serial UI, everything else is in 
   * interrupt handlers or callbacks from SerialUI.
   */


  if(Serial.available() > 0) { commandParser(); }

  if(mode == POT_RPM)
  {
    if (adc0_read_complete == true)
    {
      adc0_read_complete = false;
      tmp_rpm = adc0 << TMP_RPM_SHIFT;
      if (tmp_rpm > TMP_RPM_CAP) { tmp_rpm = TMP_RPM_CAP; }
      wanted_rpm = tmp_rpm;
      reset_new_OCR1A(tmp_rpm);
    }
  }

}


void reset_new_OCR1A(uint32_t new_rpm)
{
  uint32_t tmp;
  uint8_t bitshift;
  uint8_t tmp_prescaler_bits;
  tmp = (uint32_t)(8000000.0/(Wheels[selected_wheel].rpm_scaler * (float)(new_rpm < 10 ? 10:new_rpm)));
/*  mySUI.print(F("new_OCR1a: "));
  mySUI.println(tmpl);
  */
  get_prescaler_bits(&tmp,&tmp_prescaler_bits,&bitshift);
  /*
  mySUI.print(F("new_OCR1a: "));
  mySUI.println(tmp2);
  */
  new_OCR1A = (uint16_t)(tmp >> bitshift); 
  prescaler_bits = tmp_prescaler_bits;
  reset_prescaler = true; 
}


uint8_t get_bitshift_from_prescaler(uint8_t *prescaler_bits)
{
  switch (*prescaler_bits)
  {
    case PRESCALE_1024:
    return 10;
    case PRESCALE_256:
    return 8;
    case PRESCALE_64:
    return 6;
    case PRESCALE_8:
    return 3;
    case PRESCALE_1:
    return 0;
  }
  return 0;
}


//! Gets RPM from the TCNT value
/*!
 * Gets the RPM value based on the passed TCNT and prescaler
 * \param tcnt pointer to Output Compare register value
 * \param prescaler_bits point to prescaler bits enum
 */
uint16_t get_rpm_from_tcnt(uint16_t *tcnt, uint8_t *prescaler_bits)
{
  bitshift = get_bitshift_from_prescaler(prescaler_bits);
  return (uint16_t)((float)(8000000 >> bitshift)/(Wheels[selected_wheel].rpm_scaler*(*tcnt)));
}


//! Gets prescaler enum and bitshift based on OC value
void get_prescaler_bits(uint32_t *potential_oc_value, uint8_t *prescaler, uint8_t *bitshift)
{
  if (*potential_oc_value >= 16777216)
  {
    *prescaler = PRESCALE_1024;
    *bitshift = 10;
  }
  else if (*potential_oc_value >= 4194304)
  {
    *prescaler = PRESCALE_256;
    *bitshift = 8;
  }
  else if (*potential_oc_value >= 524288)
  {
    *prescaler = PRESCALE_64;
    *bitshift = 6;
  }
  else if (*potential_oc_value >= 65536)
  {
    *prescaler = PRESCALE_8;
    *bitshift = 3;
  }
  else
  {
    *prescaler = PRESCALE_1;
    *bitshift = 0;
  }
}


//! Builds the SweepSteps[] structure
/*!
 * For sweeping we cannot just pick the TCNT value at the beginning and ending
 * and sweep linearily between them as it'll result in a VERY slow RPM change
 * at the low end and a VERY FAST change at the high end due to the inverse
 * relationship between RPM and TCNT. So we compromise and break up the RPM
 * range into octaves (doubles of RPM), and use a linear TCNT change between
 * those two points. It's not perfect, but computationally easy
 *
 * \param low_rpm_tcnt pointer to low rpm OC value, (not prescaled!)
 * \param high_rpm_tcnt pointer to low rpm OC value, (not prescaled!)
 * \param total_stages pointer to tell the number of structs to allocate
 * \returns pointer to array of structures for each sweep stage.
 */
sweep_step *build_sweep_steps(uint32_t *low_rpm_tcnt, uint32_t *high_rpm_tcnt, uint8_t *total_stages)
{
  sweep_step *steps;
  uint8_t prescaler_bits;
  uint8_t bitshift;
  uint32_t tmp = *low_rpm_tcnt;
  /* DEBUG
  mySUI.print(*low_rpm_tcnt);
  mySUI.print(F("<->"));
  mySUI.println(*high_rpm_tcnt);
   */

  steps = (sweep_step *)malloc(sizeof(sweep_step)*(*total_stages));

#ifdef MORE_LINEAR_SWEEP
  for (uint8_t i = 0; i < (*total_stages); i+=2)
#else
  for (uint8_t i = 0; i < (*total_stages); i++)
#endif
  {
    /* The low rpm value will ALWAYS have the highed TCNT value so use that
    to determine the prescaler value
    */
    get_prescaler_bits(&tmp, &steps[i].prescaler_bits, &bitshift);
    
    steps[i].beginning_ocr = (uint16_t)(tmp >> bitshift);
    if ((tmp >> 1) < (*high_rpm_tcnt))
      steps[i].ending_ocr = (uint16_t)((*high_rpm_tcnt) >> bitshift);
    else
      steps[i].ending_ocr = (uint16_t)(tmp >> (bitshift + 1)); // Half the begin value
    tmp = tmp >> 1; /* Divide by 2 */
    /* DEBUG
    mySUI.print(steps[i].beginning_ocr);
    mySUI.print(F("<->"));
    mySUI.println(steps[i].ending_ocr);
    */
  }
  return steps;
}
