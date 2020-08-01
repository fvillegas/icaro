/*
 * readeeprom.c
 *
 * Created: 7/24/2020 11:28:50 AM
 * Author : fcvil
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>

void read(uint16_t* values)
{
    values[0] = eeprom_read_word(0);    
}

void write(uint16_t address, uint16_t value)
{
    eeprom_write_word(address, value);   
}

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
        int16_t values[6] = {0};
        read(values);
        write(0, 2);
        read(values);
        
        continue;
        
    }
}

