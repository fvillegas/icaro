#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "icaro/timer/timer.h"
#include "icaro/twi/twi.h"
#include "icaro/twi/i2cdevlib.h"
#include "icaro/uart/uart.h"

#include "sensors/mpu6050_registers.h"

uint8_t DEBUG_BUFFER[150] = {0};

void setup(void)
{
    init_millis(F_CPU);
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    twi_init();
    sei();
    uart_puts("setup finish");
}

int main(void)
{
    setup();
    uint8_t buffer[32] = {0};
    i2c_read_bits(
        MPU6050_ADDRESS,
        MPU6050_RA_WHO_AM_I,
        MPU6050_WHO_AM_I_BIT,
        MPU6050_WHO_AM_I_LENGTH,
        buffer,
        I2CDEV_DEFAULT_READ_TIMEOUT
    );
    
    sprintf(DEBUG_BUFFER, "mpu6050 address %d", buffer[0]);
    uart_puts(DEBUG_BUFFER);
    
    while (1)
    {
    }
}

