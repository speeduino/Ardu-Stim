#include "storage.h"
#include "EEPROM.h"
#include "wheel_defs.h"
#include "ardustim.h"
#include "enums.h"
#include "globals.h"

void loadConfig()
{
  config.version = VERSION;
  if(EEPROM.read(EEPROM_VERSION) == 255)
  {
    //New arduino
    config.wheel = 5; //36-1
    currentStatus.rpm = 3000;
    currentStatus.base_rpm = 3000;
    config.mode = POT_RPM;

    config.fixed_rpm = 3500;
    config.sweep_high_rpm = 6000;
    config.sweep_low_rpm = 1000;
    config.sweep_interval = 1000;

    config.useCompression = false;
    config.compressionType = COMPRESSION_TYPE_4CYL_4STROKE;
    config.compressionRPM = 400;
    config.compressionOffset = 0;

    saveConfig();
  }
  else
  {
    config.wheel = EEPROM.read(EEPROM_WHEEL);
    config.mode = EEPROM.read(EEPROM_RPM_MODE);

    byte highByte = EEPROM.read(EEPROM_CURRENT_RPM);
    byte lowByte =  EEPROM.read(EEPROM_CURRENT_RPM+1);
    currentStatus.rpm = word(highByte, lowByte);

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

    config.useCompression = EEPROM.read(EEPROM_USE_COMPRESSION);
    config.compressionType = EEPROM.read(EEPROM_COMPRESSION_TYPE);
    highByte = EEPROM.read(EEPROM_COMPRESSION_RPM);
    lowByte = EEPROM.read(EEPROM_COMPRESSION_RPM+1);
    config.compressionRPM = word(highByte, lowByte);
    highByte = EEPROM.read(EEPROM_COMPRESSION_OFFSET);
    lowByte = EEPROM.read(EEPROM_COMPRESSION_OFFSET+1);
    config.compressionOffset = word(highByte, lowByte);
    //config.compressionType = COMPRESSION_TYPE_6CYL_4STROKE;

    //Error checking
    if(config.wheel >= MAX_WHEELS) { config.wheel = 5; }
    if(config.mode >= MAX_MODES) { config.mode = FIXED_RPM; }
    if(currentStatus.rpm > 15000) { currentStatus.rpm = 4000; }
    if(currentStatus.base_rpm > 15000) { currentStatus.base_rpm = 4000; }
    if(config.compressionType > COMPRESSION_TYPE_8CYL_4STROKE) { config.compressionType = COMPRESSION_TYPE_4CYL_4STROKE; }
    if(config.compressionRPM > 1000) { config.compressionRPM = 400; }
    if(config.compressionOffset > 359) { config.compressionOffset = 0; }
  }
}

void saveConfig()
{
  EEPROM.update(EEPROM_WHEEL, config.wheel);
  EEPROM.update(EEPROM_RPM_MODE, config.mode);
  EEPROM.update(EEPROM_VERSION, VERSION);

  byte highByte = highByte(currentStatus.rpm);
  byte lowByte = lowByte(currentStatus.rpm);
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

  EEPROM.update(EEPROM_USE_COMPRESSION, config.useCompression);
  EEPROM.update(EEPROM_COMPRESSION_TYPE, config.compressionType);
  highByte = highByte(config.compressionRPM);
  lowByte = lowByte(config.compressionRPM);
  EEPROM.update(EEPROM_COMPRESSION_RPM, highByte);
  EEPROM.update(EEPROM_COMPRESSION_RPM+1, lowByte);
  highByte = highByte(config.compressionOffset);
  lowByte = lowByte(config.compressionOffset);
  EEPROM.update(EEPROM_COMPRESSION_OFFSET, highByte);
  EEPROM.update(EEPROM_COMPRESSION_OFFSET+1, lowByte);
}
