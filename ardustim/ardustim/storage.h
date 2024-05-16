#define EEPROM_VERSION          1
#define EEPROM_WHEEL            2
#define EEPROM_RPM_MODE         3
#define EEPROM_CURRENT_RPM      4 //Note this is 2 bytes
#define EEPROM_SWEEP_RPM_MIN    6 //Note this is 2 bytes
#define EEPROM_SWEEP_RPM_MAX    8 //Note this is 2 bytes
#define EEPROM_SWEEP_RPM_INT    10 //Note this is 2 bytes
#define EEPROM_FIXED_RPM        12 //Note this is 2 bytes
#define EEPROM_USE_COMPRESSION  14
#define EEPROM_COMPRESSION_TYPE 15
#define EEPROM_COMPRESSION_RPM  16 //Note this is 2 bytes
#define EEPROM_COMPRESSION_OFFSET 18 //Note this is 2 bytes

void loadConfig();
void saveConfig();
