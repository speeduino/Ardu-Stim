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

 #include "globals.h"
 #include "ardustim.h"
 #include "enums.h"
 #include "comms.h"
 #include "storage.h"
 #include "wheel_defs.h"
 
 // Platform-specific headers
 #if defined(__AVR__)
 #include <avr/pgmspace.h>
 #include <avr/interrupt.h>
 #elif defined(ESP32)
 #include <esp_timer.h>
 #include <driver/gptimer.h>
 #endif
 
 // Pin definitions
 volatile uint8_t analog_port = 0;
 #if defined(__AVR__)
 #define ADC_PIN A0             // Analog input for RPM potentiometer
 #define PRIMARY_OUTPUT_PIN 8   // PB0 - Primary output (crank)
 #define SECONDARY_OUTPUT_PIN 9 // PB1 - Secondary output (cam1)
 #define TERTIARY_OUTPUT_PIN 10 // PB2 - Tertiary output (cam2)
 #define KNOCK_OUTPUT_PIN 11    // PB3 - Knock signal
 #elif defined(ESP32)
 #define ADC_PIN 34             // GPIO 34 for ADC input (RPM pot)
 #define PRIMARY_OUTPUT_PIN 2   // GPIO 2 - Primary output
 #define SECONDARY_OUTPUT_PIN 4 // GPIO 4 - Secondary output
 #define TERTIARY_OUTPUT_PIN 5  // GPIO 5 - Tertiary output
 #define KNOCK_OUTPUT_PIN 18    // GPIO 18 - Knock signal
 #endif

// Global configuration and status structures
struct configTable config;    // Holds settings like wheel type, RPM mode, etc.
struct status currentStatus;  // Tracks current RPM and related values

// Volatile variables used in Interrupt Service Routines (ISRs)
volatile uint16_t adc0;            // ADC reading from potentiometer (RPM control)
volatile uint16_t adc1;            // Reserved for future use (e.g., additional analog input)
volatile bool adc0_read_complete = false; // Flag for completed ADC0 reading
volatile bool adc1_read_complete = false; // Flag for completed ADC1 reading
volatile bool reset_prescaler = false;    // Flag to reset timer prescaler in ISR
volatile uint8_t output_invert_mask = 0x00; // Bitmask to invert output signals (default: no inversion)
volatile uint8_t prescaler_bits = 0;       // Timer prescaler bits for AVR
volatile uint8_t last_prescaler_bits = 0;  // Previous prescaler bits (unused in current code)
volatile uint16_t new_OCR1A = 5000;        // Default timer compare value for AVR (controls interrupt frequency)

#if defined(ESP32)
uint32_t apb_frequency = 80000000UL; // APB clock frequency (typically 80 MHz for ESP32)
volatile uint64_t new_timer_interval_us = 1000; // Default timer ticks for ESP32 (controls interrupt frequency)
gptimer_handle_t timer = NULL;          // Pointer to ESP32 hardware timer instance
#endif

volatile uint16_t edge_counter = 0;      // Counts edges in the wheel pattern
volatile uint32_t cycleStartTime = micros(); // Start time of current cycle (for crank angle calculation)
volatile uint32_t cycleDuration = 0;     // Duration of the last complete cycle

uint32_t sweep_time_counter = 0; // Timer for RPM sweep mode
uint8_t sweep_direction = ASCENDING; // Direction of RPM sweep (ASCENDING or DESCENDING)

// Array of wheel definitions for various crankshaft/camshaft patterns
wheels Wheels[MAX_WHEELS] = {
   /* Pointer to friendly name string, pointer to edge array, RPM Scaler, Number of edges in the array, whether the number of edges covers 360 or 720 degrees */
  { dizzy_four_cylinder_friendly_name, dizzy_four_cylinder, 0.03333, 4, 360 },
  { dizzy_six_cylinder_friendly_name, dizzy_six_cylinder, 0.05, 6, 360 },
  { dizzy_eight_cylinder_friendly_name, dizzy_eight_cylinder, 0.06667, 8, 360 },
  { sixty_minus_two_friendly_name, sixty_minus_two, 1.0, 120, 360 },
  { sixty_minus_two_with_cam_friendly_name, sixty_minus_two_with_cam, 1.0, 240, 720 },
  { sixty_minus_two_with_halfmoon_cam_friendly_name, sixty_minus_two_with_halfmoon_cam, 1.0, 240, 720 },
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
  { thirty_six_minus_two_two_two_h6_friendly_name, thirty_six_minus_two_two_two_h6, 0.6, 72, 360 },
  { thirty_six_minus_two_two_two_with_cam_friendly_name, thirty_six_minus_two_two_two_with_cam, 0.6, 144, 720 },
  { fourty_two_hundred_wheel_friendly_name, fourty_two_hundred_wheel, 0.6, 72, 360 },
  { thirty_six_minus_one_with_cam_fe3_friendly_name, thirty_six_minus_one_with_cam_fe3, 0.6, 144, 720 },
  { six_g_seventy_two_with_cam_friendly_name, six_g_seventy_two_with_cam, 0.6, 144, 720 },
  { buell_oddfire_cam_friendly_name, buell_oddfire_cam, 0.33333, 80, 720 },
  { gm_ls1_crank_and_cam_friendly_name, gm_ls1_crank_and_cam, 6.0, 720, 720 },
  { gm_ls_58X_crank_and_4x_cam_friendly_name, GM_LS_58X_crank_and_4x_cam, 1.0, 240, 720},
  { lotus_thirty_six_minus_one_one_one_one_friendly_name, lotus_thirty_six_minus_one_one_one_one, 0.6, 72, 360 },
  { honda_rc51_with_cam_friendly_name, honda_rc51_with_cam, 0.2, 48, 720 },
  { thirty_six_minus_one_with_second_trigger_friendly_name, thirty_six_minus_one_with_second_trigger, 0.6, 144, 720 },
  { chrysler_ngc_thirty_six_plus_two_minus_two_with_ngc4_cam_friendly_name, chrysler_ngc_thirty_six_plus_two_minus_two_with_ngc4_cam, 3.0, 720, 720 },
  { chrysler_ngc_thirty_six_minus_two_plus_two_with_ngc6_cam_friendly_name, chrysler_ngc_thirty_six_minus_two_plus_two_with_ngc6_cam, 3.0, 720, 720 },
  { chrysler_ngc_thirty_six_minus_two_plus_two_with_ngc8_cam_friendly_name, chrysler_ngc_thirty_six_minus_two_plus_two_with_ngc8_cam, 3.0, 720, 720 },
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
  { twenty_four_with_cam_friendly_name, twenty_four_with_cam, 0.6, 144, 720 },
  { subaru_six_seven_name_friendly_name, subaru_six_seven, 3.0, 720, 720 },
  { gm_seven_x_friendly_name, gm_seven_x, 1.502, 180, 720 },
  { four_twenty_a_friendly_name, four_twenty_a, 0.6, 144, 720 },
  { ford_st170_friendly_name, ford_st170, 3.0, 720, 720 },
  { mitsubishi_3A92_friendly_name, mitsubishi_3A92, 0.6, 144, 720 },
  { Toyota_4AGE_CAS_friendly_name, toyota_4AGE_CAS, 0.333, 144, 720 },
  { Toyota_4AGZE_friendly_name, toyota_4AGZE, 0.333, 144, 720 },
  { Suzuki_DRZ400_friendly_name, suzuki_DRZ400,0.6, 72, 360},
  { Jeep_2000_4cyl_friendly_name, jeep_2000_4cyl, 1.5, 360, 720},
  { Jeep_2000_6cyl_friendly_name, jeep_2000_6cyl, 1.5, 360, 720 },
  { BMW_N20_friendly_name, bmw_n20, 1.0, 240, 720},
  { VIPER9602_friendly_name, viper9602wheel, 1.0, 240, 720},
  { thirty_six_minus_two_with_second_trigger_friendly_name, thirty_six_minus_two_with_second_trigger, 0.6, 144, 720 },
  { GM_40_Tooth_Trans_OSS_friendly_name, GM40toothOSS, 1.0, 80, 360 },
};

/**
 * @brief AVR ADC Interrupt Service Routine
 * 
 * Reads the ADC value from the potentiometer (ADC0) when conversion is complete.
 */
#if defined(__AVR__)
ISR(ADC_vect) {
    if (analog_port == 0) { // Only reading ADC0 currently
        adc0 = ADCL | (ADCH << 8); // Combine low and high bytes
        adc0_read_complete = true; // Signal completion
    }
}

/**
 * @brief AVR Timer1 Interrupt Service Routine
 * 
 * Generates the wheel pattern by setting PORTB pins based on the current edge state.
 */
ISR(TIMER1_COMPA_vect) {
    // Set PORTB pins based on edge state (inverted if mask is set)
    PORTB = output_invert_mask ^ pgm_read_byte(&Wheels[config.wheel].edge_states_ptr[edge_counter]);
    edge_counter++; // Advance to next edge
    if (edge_counter == Wheels[config.wheel].wheel_max_edges) {
        edge_counter = 0; // Reset to start of pattern
        cycleDuration = micros() - cycleStartTime; // Update cycle duration
        cycleStartTime = micros(); // Restart cycle timer
    }
    if (reset_prescaler) { // Adjust prescaler if flagged
        TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); // Clear prescaler bits
        TCCR1B |= prescaler_bits; // Set new prescaler
        reset_prescaler = false;
    }
    OCR1A = new_OCR1A; // Update timer compare value
}
#elif defined(ESP32)
/**
 * @brief ESP32 Timer Interrupt Service Routine (ISR)
 * 
 * Generates the wheel pattern by setting output pins based on the current edge state.
 * Runs on a hardware timer interrupt, updating pins every `new_timer_ticks` microseconds.
 */
bool __attribute__((section(".iram1.0"))) timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    // Copy the body of your existing onTimer() function here
    uint8_t state = pgm_read_byte(&Wheels[config.wheel].edge_states_ptr[edge_counter]) ^ output_invert_mask;
    digitalWrite(PRIMARY_OUTPUT_PIN, (state & 1) ? HIGH : LOW);
    digitalWrite(SECONDARY_OUTPUT_PIN, (state & 2) ? HIGH : LOW);
    digitalWrite(TERTIARY_OUTPUT_PIN, (state & 4) ? HIGH : LOW);
    digitalWrite(KNOCK_OUTPUT_PIN, (state & 8) ? HIGH : LOW);
    edge_counter = edge_counter + 1;
    if (edge_counter == Wheels[config.wheel].wheel_max_edges) {
      edge_counter = 0;
      cycleDuration = micros() - cycleStartTime;
      cycleStartTime = micros();
    }
    
    // Update timer interval (equivalent to timerAlarmWrite in v2.x)
    if (new_timer_interval_us != edata->alarm_value) {
      gptimer_alarm_config_t alarm_config;
      memset(&alarm_config, 0, sizeof(alarm_config));
      alarm_config.alarm_count = new_timer_interval_us;
      alarm_config.reload_count = 0; // one-shot mode
      alarm_config.flags.auto_reload_on_alarm = true;
      gptimer_set_alarm_action(timer, &alarm_config);
    }
    
    return true; // Return true to keep timer running
  }
#endif

/**
 * @brief Setup function to initialize the system
 * 
 * Configures serial communication, pins, timers, and interrupts based on the platform.
 */
void setup() {
  loadConfig();  // Load configuration from storage
  serialSetup(); // Initialize serial communication

  cli(); // Disable interrupts during setup to prevent interference

  // Configure output pins
  pinMode(PRIMARY_OUTPUT_PIN, OUTPUT);
  pinMode(SECONDARY_OUTPUT_PIN, OUTPUT);
  pinMode(TERTIARY_OUTPUT_PIN, OUTPUT);
  pinMode(KNOCK_OUTPUT_PIN, OUTPUT);

  // Platform-specific timer and ADC setup
  #if defined(__AVR__)
  // AVR Timer1 setup for pattern generation
  TCCR1A = 0;              // Clear Timer1 control register A
  TCCR1B = 0;              // Clear Timer1 control register B
  TCNT1 = 0;               // Reset Timer1 counter
  OCR1A = 1000;            // Initial compare value (approx. 8000 RPM for 60-2 wheel)
  TCCR1B |= (1 << WGM12);  // CTC mode (Clear Timer on Compare)
  TCCR1B |= (1 << CS10);   // Prescaler 1 (no prescaling)
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt

  // AVR ADC setup (interrupt-driven for potentiometer reading)
  ADMUX &= B11011111;  // Right-adjust result
  ADMUX |= B01000000;  // Use AVcc as reference voltage
  ADMUX &= B11110000;  // Select ADC0 (A0 pin)
  ADCSRA |= B10000000; // Enable ADC
  ADCSRA |= B00100000; // Enable auto-trigger
  ADCSRB &= B11111000; // Free-running mode
  ADCSRA |= B00000111; // Prescaler 128
  ADCSRA |= B00001000; // Enable ADC interrupt
  ADCSRA |= B01000000; // Start first conversion
  #elif defined(ESP32)
  // ESP32 timer setup
  apb_frequency = getApbFrequency(); // Get actual APB frequency (typically 80 MHz)
  gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000, // 1 MHz (1 tick = 1 microsecond)
  };
  ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer));
  
  // Set up callback
  gptimer_event_callbacks_t timer_callbacks = {
    .on_alarm = timer_callback,
  };
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &timer_callbacks, NULL));
  
  // Set up initial alarm
  gptimer_alarm_config_t alarm_config = {
    .alarm_count = 1000, // 1000 microseconds = 1 ms
    .reload_count = 0,
  };
  alarm_config.flags.auto_reload_on_alarm = true;
  ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &alarm_config));
  
  // Enable timer
  ESP_ERROR_CHECK(gptimer_enable(timer));
  ESP_ERROR_CHECK(gptimer_start(timer));
  #endif

  sei(); // Enable interrupts after setup
  reset_new_OCR1A(currentStatus.rpm); // Set initial RPM based on loaded config
}

/**
 * @brief Main loop function
 * 
 * Handles serial commands, updates RPM based on mode, and applies compression modifier.
 */
void loop() {
  uint16_t tmp_rpm = currentStatus.base_rpm; // Temporary RPM value

  if (Serial.available() > 0) {
      commandParser(); // Process incoming serial commands
  }

  // Update RPM based on selected mode
  if (config.mode == POT_RPM) {
      // Potentiometer-controlled RPM
      #if defined(__AVR__)
      if (adc0_read_complete) {
          adc0_read_complete = false;
          tmp_rpm = adc0 << TMP_RPM_SHIFT; // Scale ADC value (0-1023) to RPM (0-16384)
          if (tmp_rpm > TMP_RPM_CAP) tmp_rpm = TMP_RPM_CAP; // Cap the maximum RPM
      }
      #elif defined(ESP32)
      adc0 = analogRead(ADC_PIN); // Polled ADC reading (0-4095)
      tmp_rpm = adc0 << TMP_RPM_SHIFT; // Scale to RPM
      if (tmp_rpm > TMP_RPM_CAP) tmp_rpm = TMP_RPM_CAP; // Cap the maximum RPM
      #endif
  } else if (config.mode == LINEAR_SWEPT_RPM) {
      // Linear sweep between low and high RPM
      if (micros() > (sweep_time_counter + config.sweep_interval)) {
          sweep_time_counter = micros();
          if (sweep_direction == ASCENDING) {
              tmp_rpm = currentStatus.base_rpm + 1;
              if (tmp_rpm >= config.sweep_high_rpm) sweep_direction = DESCENDING;
          } else {
              tmp_rpm = currentStatus.base_rpm - 1;
              if (tmp_rpm <= config.sweep_low_rpm) sweep_direction = ASCENDING;
          }
      }
  } else if (config.mode == FIXED_RPM) {
      // Fixed RPM mode
      tmp_rpm = config.fixed_rpm;
  }

  currentStatus.base_rpm = tmp_rpm; // Update base RPM
  currentStatus.compressionModifier = calculateCompressionModifier(); // Calculate compression effect
  if (currentStatus.compressionModifier >= currentStatus.base_rpm) {
      currentStatus.compressionModifier = 0; // Prevent negative RPM
  }
  setRPM(currentStatus.base_rpm - currentStatus.compressionModifier); // Apply final RPM
}

/**
 * @brief Calculates the compression modifier based on crank angle
 * 
 * Simulates engine compression effects by reducing RPM based on a sine wave pattern.
 * @return uint16_t The compression modifier value (0 if disabled or above threshold)
 */
uint16_t calculateCompressionModifier() {
  if (!config.useCompression || currentStatus.base_rpm > config.compressionRPM) {
      return 0; // No modifier if disabled or RPM exceeds threshold
  }

  uint16_t crankAngle = calculateCurrentCrankAngle(); // Get current crank angle
  uint16_t modAngle = crankAngle;
  uint16_t compressionModifier = 0;

  // Adjust modifier based on engine type
  switch (config.compressionType) {
      case COMPRESSION_TYPE_2CYL_4STROKE:
          modAngle = crankAngle / 2;
          compressionModifier = pgm_read_byte(&sin_100_180[modAngle]);
          break;
      case COMPRESSION_TYPE_4CYL_4STROKE:
          modAngle = crankAngle % 180;
          compressionModifier = pgm_read_byte(&sin_100_180[modAngle]);
          break;
      case COMPRESSION_TYPE_6CYL_4STROKE:
          modAngle = crankAngle % 120;
          compressionModifier = pgm_read_byte(&sin_100_120[modAngle]);
          break;
      case COMPRESSION_TYPE_8CYL_4STROKE:
          modAngle = crankAngle % 90;
          compressionModifier = pgm_read_byte(&sin_100_90[modAngle]);
          break;
      default:
          modAngle = crankAngle % 180;
          compressionModifier = pgm_read_byte(&sin_100_180[modAngle]);
          break;
  }

  // Scale modifier dynamically based on RPM if enabled
  if (config.compressionDynamic && currentStatus.base_rpm < 655U) {
      compressionModifier = (compressionModifier * currentStatus.base_rpm) / config.compressionRPM;
  }
  return compressionModifier;
}

/**
* @brief Calculates the current crank angle
* 
* Uses cycle timing to determine the current position in the wheel pattern.
* @return uint16_t Crank angle in degrees (0-360)
*/
uint16_t calculateCurrentCrankAngle() {
  if (cycleDuration == 0) return 0; // No valid cycle yet

  uint32_t cycleTime = micros() - cycleStartTime;
  if (Wheels[config.wheel].wheel_degrees == 720) cycleTime *= 2; // Adjust for 720-degree patterns
  
  uint16_t tmpCrankAngle = ((cycleTime * 360U) / cycleDuration); // Calculate angle
  tmpCrankAngle += config.compressionOffset; // Apply offset
  while (tmpCrankAngle > 360) tmpCrankAngle -= 360; // Normalize to 0-360
  return tmpCrankAngle;
}

/**
* @brief Sets the RPM and adjusts the timer
* 
* Updates the timer interrupt frequency based on the desired RPM.
* @param newRPM The target RPM value
*/
void setRPM(uint16_t newRPM) {
  if (newRPM < 10) return; // Minimum RPM threshold
  if (currentStatus.rpm != newRPM) reset_new_OCR1A(newRPM); // Update timer if RPM changes
  currentStatus.rpm = newRPM; // Store final RPM
}

/**
* @brief Calculates the timer compare value based on RPM
* 
* Adjusts the timer interrupt frequency to match the desired RPM.
* @param new_rpm The target RPM
*/
void reset_new_OCR1A(uint32_t new_rpm) {
  if (new_rpm < 10) return; // Minimum RPM threshold

  #if defined(__AVR__)
  // AVR: Calculate timer ticks based on 8 MHz clock
  uint32_t tmp = (uint32_t)(8000000.0 / (Wheels[config.wheel].rpm_scaler * (float)new_rpm));
  uint8_t tmp_prescaler_bits;
  uint8_t bitshift;
  get_prescaler_bits(&tmp, &tmp_prescaler_bits, &bitshift); // Determine prescaler
  new_OCR1A = (uint16_t)(tmp >> bitshift); // Adjust for prescaler
  prescaler_bits = tmp_prescaler_bits;
  reset_prescaler = true; // Flag ISR to update prescaler
  #elif defined(ESP32)
  // ESP32: Calculate timer ticks based on APB frequency
  uint64_t f_interrupt = ((uint64_t)new_rpm * Wheels[config.wheel].wheel_max_edges) / 60ULL;
  if (f_interrupt == 0) f_interrupt = 1;
  new_timer_interval_us = 1000000ULL / f_interrupt; // Convert frequency to period in microseconds
  if (new_timer_interval_us < 1) new_timer_interval_us = 1;
  #endif
}

/**
* @brief Gets the bit shift value for a given prescaler
* 
* @param prescaler_bits Pointer to the prescaler enum value
* @return uint8_t Number of bits to shift
*/
uint8_t get_bitshift_from_prescaler(uint8_t *prescaler_bits) {
  switch (*prescaler_bits) {
      case PRESCALE_1024: return 10;
      case PRESCALE_256:  return 8;
      case PRESCALE_64:   return 6;
      case PRESCALE_8:    return 3;
      case PRESCALE_1:    return 0;
      default:            return 0;
  }
}

/**
* @brief Determines the prescaler and bit shift for the timer
* 
* Adjusts the timer prescaler to keep the compare value within 16-bit range.
* @param potential_oc_value Pointer to the calculated timer value
* @param prescaler Pointer to store the prescaler enum value
* @param bitshift Pointer to store the bit shift value
*/
void get_prescaler_bits(uint32_t *potential_oc_value, uint8_t *prescaler, uint8_t *bitshift) {
  if (*potential_oc_value >= 16777216) {
      *prescaler = PRESCALE_1024;
      *bitshift = 10;
  } else if (*potential_oc_value >= 4194304) {
      *prescaler = PRESCALE_256;
      *bitshift = 8;
  } else if (*potential_oc_value >= 524288) {
      *prescaler = PRESCALE_64;
      *bitshift = 6;
  } else if (*potential_oc_value >= 65536) {
      *prescaler = PRESCALE_8;
      *bitshift = 3;
  } else {
      *prescaler = PRESCALE_1;
      *bitshift = 0;
  }
}
