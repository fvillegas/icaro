/*
 * twi-receiver.c
 *
 * Created: 7/28/2020 11:12:35 PM
 * Author : fcvil
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "icarolib/twi/twi.h"
#include "icarolib/twi/i2cdevlib.h"
#include "icarolib/uart/uart.h"

void receive_event(uint8_t* data, int how_many)
{
    while(*data) {
        uart_putc(*data++);
    }
    uart_puts("\n message received \n");
}

void setup(void)
{
    twi_init();
    twi_set_address(4);
    twi_attach_slave_rx_event(receive_event);
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    
    sei();
}

int main(void)
{
    setup();
    
    uart_puts("hello from receiver \n");
    
    while (1)
    {
    }
}

