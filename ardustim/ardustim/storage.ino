#include "storage.h"
#include "EEPROM.h"
#include "wheel_defs.h"
#include "ardustim.h"
#include "enums.h"
#include "globals.h"

void loadConfig()
{
  if(EEPROM.read(EEPROM_VERSION) == 255)
  {
    //New arduino
    config.wheel = 5; //36-1
    config.rpm = 3000;
    config.mode = POT_RPM;

    config.fixed_rpm = 3500;
    config.sweep_high_rpm = 6000;
    config.sweep_low_rpm = 1000;
    config.sweep_interval = 1000;

    saveConfig();
  }
  else
  {
    config.wheel = EEPROM.read(EEPROM_WHEEL);
    config.mode = EEPROM.read(EEPROM_RPM_MODE);

    byte highByte = EEPROM.read(EEPROM_CURRENT_RPM);
    byte lowByte =  EEPROM.read(EEPROM_CURRENT_RPM+1);
    config.rpm = word(highByte, lowByte);

    highByte = EEPROM.read(EEPROM_FIXED_RPM);
    lowByte =  EEPROM.read(EEPROM_FIXED_RPM+1);
    config.fixed_rpm = word(highByte, lowByte);
    config.fixed_rpm = constrain(config.fixed_rpm, 100, TMP_RPM_CAP);

    highByte = EEPROM.read(EEPROM_SWEEP_RPM_MIN);
    lowByte =  EEPROM.read(EEPROM_SWEEP_RPM_MIN+1);
    config.sweep_low_rpm = word(highByte, lowByte);
    config.sweep_low_rpm = constrain(config.sweep_low_rpm, 100, TMP_RPM_CAP);

    highByte = EEPROM.read(EEPROM_SWEEP_RPM_MAX);
    lowByte =  EEPROM.read(EEPROM_SWEEP_RPM_MAX+1);
    config.sweep_high_rpm = word(highByte, lowByte);
    config.sweep_high_rpm = constrain(config.sweep_high_rpm, 100, TMP_RPM_CAP);

    highByte = EEPROM.read(EEPROM_SWEEP_RPM_INT);
    lowByte =  EEPROM.read(EEPROM_SWEEP_RPM_INT+1);
    config.sweep_interval = word(highByte, lowByte);
    config.sweep_interval = constrain(config.sweep_interval, 200, 10000);

    if(config.sweep_low_rpm >= config.sweep_high_rpm) { config.sweep_low_rpm = config.sweep_high_rpm - 100; }

    //Error checking
    if(config.wheel >= MAX_WHEELS) { config.wheel = 5; }
    if(config.mode >= MAX_MODES) { config.mode = FIXED_RPM; }
    if(config.rpm > 15000) { config.rpm = 4000; }
  }
}

void saveConfig()
{
  EEPROM.update(EEPROM_WHEEL, config.wheel);
  EEPROM.update(EEPROM_RPM_MODE, config.mode);
  EEPROM.update(EEPROM_VERSION, EEPROM_CURRENT_VERSION);

  byte highByte = highByte(config.rpm);
  byte lowByte = lowByte(config.rpm);
  EEPROM.update(EEPROM_CURRENT_RPM, highByte);
  EEPROM.update(EEPROM_CURRENT_RPM+1, lowByte);

  highByte = highByte(config.fixed_rpm);
  lowByte = lowByte(config.fixed_rpm);
  EEPROM.update(EEPROM_FIXED_RPM, highByte);
  EEPROM.update(EEPROM_FIXED_RPM+1, lowByte);

  highByte = highByte(config.sweep_low_rpm);
  lowByte = lowByte(config.sweep_low_rpm);
  EEPROM.update(EEPROM_SWEEP_RPM_MIN, highByte);
  EEPROM.update(EEPROM_SWEEP_RPM_MIN+1, lowByte);

  highByte = highByte(config.sweep_high_rpm);
  lowByte = lowByte(config.sweep_high_rpm);
  EEPROM.update(EEPROM_SWEEP_RPM_MAX, highByte);
  EEPROM.update(EEPROM_SWEEP_RPM_MAX+1, lowByte);

  highByte = highByte(config.sweep_interval);
  lowByte = lowByte(config.sweep_interval);
  EEPROM.update(EEPROM_SWEEP_RPM_INT, highByte);
  EEPROM.update(EEPROM_SWEEP_RPM_INT+1, lowByte);

}