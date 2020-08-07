#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "icarolib/timer/timer.h"
#include "icarolib/twi/i2cdevlib.h"
#include "icarolib/uart/uart.h"
#include "icarolib/icaro_common.h"
#include "icarolib/float.h"

#ifdef DEBUG
char DEBUG_BUFFER[150] = {0};
#endif

union Float f;
uint8_t BUFFER[10] = {0};

void setup()
{
    DDRB |= (1 << PB5);
    init_millis(F_CPU);
    wire_init();
    
    #ifdef DEBUG
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    #endif
    
    sei();
    
    _delay_ms(1000);
    
    i2c_write_byte(IMU_TWI_ADDRESS, IMU_STATUS_ADDRESS, IMU_STATUS_CALIBRATING);
    uint8_t result[1];
    do 
    {
        i2c_read_byte(IMU_TWI_ADDRESS, IMU_STATUS_ADDRESS, result, I2CDEV_DEFAULT_READ_TIMEOUT);
        _delay_ms(1000);
    } while (result[0] != IMU_STATUS_READY_TO_START);
    i2c_write_byte(IMU_TWI_ADDRESS, IMU_STATUS_ADDRESS, IMU_STATUS_RUNNING);
}

long last_time = 0;
long last_imu = 0;
float roll, pitch, yaw;
uint8_t imu_data[12] = {0};

int main(void)
{
    setup();
    
    while(1)
    {
        long delta = millis() - last_time;
        if(delta > 1000)
        {
            PORTB ^= (1 << PB5);
            
            #ifdef DEBUG
            sprintf(DEBUG_BUFFER, "rpy\t%f\t%f\t%f\n", roll, pitch, yaw);
            uart_puts(DEBUG_BUFFER);
            #endif
            
            last_time = millis();
        }
        
        delta = millis() - last_imu;
        if (delta > 1000)
        {
            for(int i = 0; i < 12; i++)
            { i2c_read_bytes(IMU_TWI_ADDRESS, IMU_ROLL_ADDRESS + i, 1, &imu_data[i], I2CDEV_DEFAULT_READ_TIMEOUT); }
            
            
            f.m_float = 0;
            memcpy(f.m_bytes, &imu_data[0], sizeof(float));
            roll = f.m_float;
                
            f.m_float = 0;
            memcpy(f.m_bytes, &imu_data[4], sizeof(float));
            pitch = f.m_float;
                
            f.m_float = 0;
            memcpy(f.m_bytes, &imu_data[8], sizeof(float));
            yaw = f.m_float;
            
            last_imu = millis();
        }
    }
    
    return 0;
}