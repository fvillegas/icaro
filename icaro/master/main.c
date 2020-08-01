#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/power.h>
#include "timer/timer.h"
//#include "twi/twi.h"
//#include "twi/i2cdevlib.h"
//#define UART_BAUD_RATE 57600
//#include "uart/uart.h"
//#include "sensors/mpu6050.h"
//#include "sensors/hcm5883l.h"
//#include "eeprom/eeprom.h"
//#include "mahony.h"

// char buffer[150] = {0};
// long last = 0;
// long gyro_x_cal, gyro_y_cal, gyro_z_cal;
// 
// void setup(void)
// {
//     DDRB |= (1 << PB5);
//     sei();
//     _delay_ms(10);
//     init_millis(F_CPU);
//     uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
//     twi_init();
//     _delay_ms(100);
// }
// 
// void setup_sensors(void)
// {
//     mpu6050_initialize();
//     //mpu6050_run_calibration(values);
//     hcm5883l_initialize();
//     mahony_init();
//     _delay_ms(100);
// }
// 
// void calculate_roll_pitch_yaw(void)
// {
//     int16_t gx, gy, gz, ax, ay, az, mx, my, mz;
//     mpu6050_get_motion_6(&ax, &ay, &az, &gx, &gy, &gz);
//     hcm5883l_get_heading(&mx, &my, &mz);
// 
//     mahony_update(gz * 0.001, gy * 0.001, gz * 0.001, ax * 0.001, ay * 0.001, az * 0.001, mx * 0.001, my * 0.001, mz * 0.001);
// 
//     float roll_pitch_yaw[3] = {0};
// 
//     roll_pitch_yaw[0] = getRoll();
//     roll_pitch_yaw[1] = getPitch();
//     roll_pitch_yaw[2] = getYaw();
// 
//     long delta = millis() - last;
//     if(delta > 100)
//     {
//         sprintf(buffer, "rpy \t%f \t%f \t%f \n",
//         roll_pitch_yaw[0],
//         roll_pitch_yaw[1],
//         roll_pitch_yaw[2]
//         );
//         uart_puts(buffer);
//         last = millis();
//     }
// }

void pwm_init()
{
    DDRB |= (1<<DDB1);
}



void main(void)
{
    cli();
    pwm_init(100, 10);
    sei();
}