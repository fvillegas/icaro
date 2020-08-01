/*
* pwm.c
*
* Created: 7/25/2020 7:51:48 PM
* Author : fcvil
*/

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "icarolib/timer/timer.h"
#include "icarolib/uart/uart.h"

char BUFFER[150];
volatile long last = 0;

volatile long last_d8 = 0;
uint8_t last_channel_1;
uint16_t timer_1;
uint16_t channel_1_value;
uint16_t current_time;
int receiver_input[5];

void setup(void)
{
    init_millis(F_CPU);
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    sei();
    
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);
    
    uart_puts("setup finish\n");
}

int main(void)
{
    DDRB |= (1 << PB5);
    
    // DDRD |= (1 << DDD6);
    // // PD6 is now an output
    // OCR0A = 128;
    // // set PWM for 50% duty cycle
    // TCCR0A |= (1 << COM0A1);
    // // set none-inverting mode
    // TCCR0A |= (1 << WGM01) | (1 << WGM00);
    // // set fast PWM Mode
    // TCCR0B |= (1 << CS02) | (1 << CS00); //(1 << CS01) | (1 << CS00);
    // // set prescaler to 1024 and starts PWM
    
    setup();
    
    PORTB ^= (1 << PB5);
    
    while (1)
    {
        _delay_ms(100);
        sprintf(BUFFER, "> %d \n", receiver_input[1]);
        uart_puts(BUFFER);
    }
}

ISR(PCINT0_vect)
{
    current_time = millis();
    if(PINB & 0b00000001)
    {                                        //Is input 8 high?
        if(last_channel_1 == 0)
        {                                   //Input 8 changed from 0 to 1.
            last_channel_1 = 1;                                      //Remember current input state.
            timer_1 = current_time;                                  //Set timer_1 to current_time.
        }
    }
    else if(last_channel_1 == 1)
    {                                //Input 8 is not high and changed from 1 to 0.
        last_channel_1 = 0;                                        //Remember current input state.
        receiver_input[1] = current_time - timer_1;                 //Channel 1 is current_time - timer_1.
    }
}


