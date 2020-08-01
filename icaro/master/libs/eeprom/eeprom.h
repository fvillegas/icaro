#ifndef EEPROM_H_
#define EEPROM_H_

#define AX_OFFSET_ADDRESS 0
#define AY_OFFSET_ADDRESS 2

void fetch_calibration(uint16_t* values);
void save_calibration(uint16_t* values);

#endif /* EEPROM_H_ */