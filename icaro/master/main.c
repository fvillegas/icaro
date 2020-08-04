#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "icarolib/timer/timer.h"
#include "icarolib/twi/i2cdevlib.h"
#include "icarolib/uart/uart.h"
#include "icarolib/icaro_common.h"
#include "icarolib/float.h"

uint8_t BUFFER[10] = {0};
    
void setup()
{
    DDRB |= (1 << PB5);
    init_millis(F_CPU);
    wire_init();
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    sei();
    
    i2c_read_byte(IMU_TWI_ADDRESS, IMU_STATUS_ADDRESS, values, I2CDEV_DEFAULT_READ_TIMEOUT);
    
    i2c_write_byte(IMU_TWI_ADDRESS, IMU_STATUS_ADDRESS, 10);
    i2c_read_byte(IMU_TWI_ADDRESS, IMU_STATUS_ADDRESS, values, I2CDEV_DEFAULT_READ_TIMEOUT);
}

long last_time = 0;

int main(void)
{
    setup();   
    while(1)
    {
        long delta = millis() - last_time;
        if(delta > 1000)
        {
         PORTB ^= (1 << PB5);
         last_time = millis();
        }
    }
    
    return 0;
}