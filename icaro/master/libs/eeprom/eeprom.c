#include <avr/eeprom.h>

#include "eeprom.h"

void fetch_calibration(uint16_t* values)
{       
    values[0] = eeprom_read_word(AX_OFFSET_ADDRESS);   
}

void save_calibration(uint16_t* values)
{
    eeprom_write_word(AX_OFFSET_ADDRESS, values[0]);
    eeprom_write_word(AY_OFFSET_ADDRESS, values[1]);
}