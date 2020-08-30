#include "storage.h"
#include "EEPROM.h"
#include "wheel_defs.h"
#include "user_defaults.h"
#include "ardustim.h"
#include "enums.h"

void loadConfig()
{
  if(EEPROM.read(EEPROM_VERSION) == 255)
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
}