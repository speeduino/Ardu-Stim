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
#if defined(__AVR__)
#include <avr/pgmspace.h>
#include <util/delay.h>
#endif
#if defined(ESP32)
#include <esp_heap_caps.h>
#endif
#include <math.h>

/* External Global Variables */
extern wheels Wheels[];

/* Volatile variables (USED in ISR's) */
extern volatile bool normal;
extern volatile uint16_t edge_counter;
extern volatile uint16_t new_OCR1A;

// Command processing variables
bool cmdPending;
byte currentCommand;

// Initializes the serial port and sets up the Menu
/* @brief Initializes serial communication
 * Sets up the serial port and menu for the serial user interface
 * Sets user input timeout to 20 seconds and overall interactivity timeout at 30
 * at which point it'll disconnect the user
 */
void serialSetup()
{
  Serial.begin(115200);
  cmdPending = false;
}

/**
 * @brief Parses incoming serial commands
 */
void commandParser()
{
  char buf[80];
  byte tmp_wheel;
  void* pnt_Config = &config;
  if (cmdPending == false) { currentCommand = Serial.read(); }

  switch (currentCommand)
  {
    case 'a':
      break;

    case 'b': // Report board type
      #if defined(__AVR__)
      Serial.println("AVR");
      #elif defined(ESP8266)
      Serial.println("ESP8266");
      #elif defined(ESP32)
      Serial.println("ESP32");
      #else
      Serial.println("Unknown");
      #endif
      break;

    case 'c':// Receive full config
      while(Serial.available() < static_cast<int>(sizeof(struct configTable) - 1)) {} //Wait for all bytes
      for(uint8_t x=1; x<(sizeof(struct configTable)); x++)
      {
        *((uint8_t *)pnt_Config + x) = Serial.read(); //Read each byte into the config table
      }
      break;

    case 'C': // Send current config
      for(uint8_t x=0; x<sizeof(struct configTable); x++)
      {
        Serial.write(*((uint8_t *)pnt_Config + x)); //Each byte is simply the location in memory of the config Page + the offset
      }
      break;
      
    case 'L': // Send the list of wheel names      
      //Wheel names are then sent 1 per line
      for(byte x=0;x<MAX_WHEELS;x++)
      {
        strcpy_P(buf,Wheels[x].decoder_name);
        Serial.println(buf);
      }
      break;

    case 'n': // Send number of wheels
      Serial.println(MAX_WHEELS);
      break;

    case 'N': // Send current wheel index
      Serial.println(config.wheel);
      break;
    
    case 'p': // Send current wheel size (edges)
      Serial.println(Wheels[config.wheel].wheel_max_edges);
      break;

    case 'P': // Send current wheel pattern
      for(uint16_t x=0; x<Wheels[config.wheel].wheel_max_edges; x++)
      {
        if(x != 0) { Serial.print(","); }

        byte tempByte = pgm_read_byte(&Wheels[config.wheel].edge_states_ptr[x]);
        Serial.print(tempByte);
      }
      Serial.println("");
      //2nd row of data sent is the number of degrees the wheel runs over (360 or 720 typically)
      Serial.println(Wheels[config.wheel].wheel_degrees);
      break;

    case 'R': // Send current RPM
      Serial.println(currentStatus.rpm);
      break;

    case 'r': // Set sweep mode parameters
      config.mode = LINEAR_SWEPT_RPM;
      while(Serial.available() < 6) {} //Wait for 4 bytes representing the new low and high RPMs

      config.sweep_low_rpm = word(Serial.read(), Serial.read());
      config.sweep_high_rpm = word(Serial.read(), Serial.read());
      config.sweep_interval = word(Serial.read(), Serial.read());
      break;

    case 's': // Save config to storage
      saveConfig();
      break;

    case 'S': // Set current wheel
      while(Serial.available() < 1) {} 
      tmp_wheel = Serial.read();
      if(tmp_wheel < MAX_WHEELS)
      {
        config.wheel = tmp_wheel;
        display_new_wheel();
      }
      break;

    case 'X': // Test: Switch to next wheel
      select_next_wheel_cb();
      strcpy_P(buf,Wheels[config.wheel].decoder_name);
      Serial.println(buf);
      break;

    default:
      break;
  }
  cmdPending = false;
}

/**
 * @brief Returns the amount of free RAM available
 * 
 * For AVR, calculates free RAM using stack and heap pointers.
 * For ESP32, uses ESP.getFreeHeap() to get free heap size.
 * 
 * @return uint32_t Free RAM in bytes
 */
uint32_t freeRam() {
  #if defined(__AVR__)
      extern int __heap_start, *__brkval;
      int v;
      int free_memory;
      if (__brkval == 0) {
          free_memory = (int) &v - (int) &__heap_start;
      } else {
          free_memory = (int) &v - (int) __brkval;
      }
      return (uint32_t) free_memory;
  #elif defined(ESP32)
      return heap_caps_get_free_size(MALLOC_CAP_8BIT);
  #else
      return 0;  // Unsupported platform
  #endif
  }

/* SerialUI Callbacks */
/**
 * @brief Toggles inversion of the primary output
 */
void toggle_invert_primary_cb()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x01; /* Flip crank invert mask bit */

}

/**
 * @brief Toggles inversion of the secondary output
 */
void toggle_invert_secondary_cb()
{
  extern uint8_t output_invert_mask;
  output_invert_mask ^= 0x02; /* Flip cam invert mask bit */
}

/**
 * @brief Updates system for a new wheel selection
 */
void display_new_wheel()
{
  reset_new_OCR1A(currentStatus.rpm);
  edge_counter = 0; // Reset to beginning of the wheel pattern */
}


/**
 * @brief Selects the next wheel in the list
 */
/*!
 * Selects the next wheel, if at the end, wrap to the beginning of the list,
 * re-calculate the OCR1A value (RPM) and reset, return user information on the
 * selected wheel and current RPM
 */
void select_next_wheel_cb()
{
  if (config.wheel == (MAX_WHEELS-1))
    config.wheel = 0;
  else 
    config.wheel++;
  
  display_new_wheel();
}

/**
 * @brief Selects the previous wheel in the list
 */
/*!
 * Selects the nex, if at the beginning, wrap to the end of the list,
 * re-calculate the OCR1A value (RPM) and reset, return user information on the
 * selected wheel and current RPM
 */
void select_previous_wheel_cb()
{
  if (config.wheel == 0)
    config.wheel = MAX_WHEELS-1;
  else 
    config.wheel--;
  
  display_new_wheel();
}