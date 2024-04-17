#include "storage.h"
#include "EEPROM.h"
#include "wheel_defs.h"
#include "user_defaults.h"
#include "ardustim.h"
#include "enums.h"
#include "defines.h"

void loadConfig()
{
  if(EEPROM.read(EEPROM_VERSION) == 255)
  //if(true)
  {
    //New arduino
    selected_wheel = 5; //36-1
    wanted_rpm = 3000;
    mode = POT_RPM;
  }
  else
  {
    selected_wheel = EEPROM.read(EEPROM_CURRRENT_WHEEL);
    mode = EEPROM.read(EEPROM_CURRENT_RPM_MODE);

    byte highByte = EEPROM.read(EEPROM_CURRENT_RPM);
    byte lowByte =  EEPROM.read(EEPROM_CURRENT_RPM+1);
    wanted_rpm = word(highByte, lowByte);

    highByte = EEPROM.read(EEPROM_SWEEP_RPM_MIN);
    lowByte =  EEPROM.read(EEPROM_SWEEP_RPM_MIN+1);
    sweep_low_rpm = word(highByte, lowByte);
    sweep_low_rpm = constrain(sweep_low_rpm, 100, TMP_RPM_CAP);

    highByte = EEPROM.read(EEPROM_SWEEP_RPM_MAX);
    lowByte =  EEPROM.read(EEPROM_SWEEP_RPM_MAX+1);
    sweep_high_rpm = word(highByte, lowByte);
    sweep_high_rpm = constrain(sweep_high_rpm, 100, TMP_RPM_CAP);

    if(sweep_low_rpm >= sweep_high_rpm) { sweep_low_rpm = sweep_high_rpm - 100; }

    //Error checking
    if(selected_wheel >= MAX_WHEELS) { selected_wheel = 5; }
    if(mode >= MAX_MODES) { mode = FIXED_RPM; }
    if(wanted_rpm > 15000) { wanted_rpm = 4000; }
  }
}

void saveConfig()
{
  EEPROM.update(EEPROM_CURRRENT_WHEEL, selected_wheel);
  EEPROM.update(EEPROM_CURRENT_RPM_MODE, mode);
  EEPROM.update(EEPROM_VERSION, EEPROM_CURRENT_VERSION);

  byte highByte = highByte(wanted_rpm);
  byte lowByte = lowByte(wanted_rpm);
  EEPROM.update(EEPROM_CURRENT_RPM, highByte);
  EEPROM.update(EEPROM_CURRENT_RPM+1, lowByte);

  highByte = highByte(sweep_low_rpm);
  lowByte = lowByte(sweep_low_rpm);
  EEPROM.update(EEPROM_SWEEP_RPM_MIN, highByte);
  EEPROM.update(EEPROM_SWEEP_RPM_MIN+1, lowByte);

  highByte = highByte(sweep_high_rpm);
  lowByte = lowByte(sweep_high_rpm);
  EEPROM.update(EEPROM_SWEEP_RPM_MAX, highByte);
  EEPROM.update(EEPROM_SWEEP_RPM_MAX+1, lowByte);

}