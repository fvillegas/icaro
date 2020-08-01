#include <avr/eeprom.h>

#include "eeprom.h"

void fetch_mpu6050_calibration(int16_t* values)
{       
    values[0] = eeprom_read_word((uint16_t*)AX_OFFSET_ADDRESS);
    values[1] = eeprom_read_word((uint16_t*)AY_OFFSET_ADDRESS);
    values[2] = eeprom_read_word((uint16_t*)AZ_OFFSET_ADDRESS);
    
    values[3] = eeprom_read_word((uint16_t*)GX_OFFSET_ADDRESS);
    values[4] = eeprom_read_word((uint16_t*)GY_OFFSET_ADDRESS);
    values[5] = eeprom_read_word((uint16_t*)GZ_OFFSET_ADDRESS);
}

void save_mpu6050_calibration(int16_t* values)
{
    eeprom_write_word((uint16_t*)AX_OFFSET_ADDRESS, values[0]);
    eeprom_write_word((uint16_t*)AY_OFFSET_ADDRESS, values[1]);
    eeprom_write_word((uint16_t*)AZ_OFFSET_ADDRESS, values[2]);
    
    eeprom_write_word((uint16_t*)GY_OFFSET_ADDRESS, values[3]);
    eeprom_write_word((uint16_t*)GX_OFFSET_ADDRESS, values[4]);
    eeprom_write_word((uint16_t*)GZ_OFFSET_ADDRESS, values[5]);
}