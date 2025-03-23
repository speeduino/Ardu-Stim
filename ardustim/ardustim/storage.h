#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "Arduino.h"
#if defined(__AVR__)
#include <EEPROM.h> // EEPROM library for AVR
#elif defined(ESP32)
#include <Preferences.h> // Preferences library for ESP32
extern Preferences preferences;
#endif

// EEPROM address definitions for AVR
#define EEPROM_VERSION          1
#define EEPROM_WHEEL            2
#define EEPROM_RPM_MODE         3
#define EEPROM_CURRENT_RPM      4
#define EEPROM_SWEEP_RPM_MIN    6
#define EEPROM_SWEEP_RPM_MAX    8
#define EEPROM_SWEEP_RPM_INT    10
#define EEPROM_FIXED_RPM        12
#define EEPROM_USE_COMPRESSION  14
#define EEPROM_COMPRESSION_TYPE 15
#define EEPROM_COMPRESSION_RPM  16
#define EEPROM_COMPRESSION_OFFSET 18

void loadConfig(); // Loads configuration from storage
void saveConfig(); // Saves configuration to storage

#endif