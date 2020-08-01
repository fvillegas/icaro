/*
* blink.c
*
* Created: 7/18/2020 6:23:01 PM
* Author : fcvil
*/

#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
    DDRB |= (1 << PB5);
    while (1)
    {
        PORTB ^= (1 << PB5);
        _delay_ms(1000);
    }
}

