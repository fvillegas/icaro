#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "icarolib/timer/timer.h"
#include "icarolib/twi/twi.h"
#include "icarolib/icaro_common.h"

#include "./sensors/mpu6050.h"
#include "./sensors/hcm5883l.h"
#include "./eeprom/eeprom.h"
#include "./mahony.h"

#ifdef DEBUG
#include "icarolib/uart/uart.h"
char BUFFER[150] = {0};
int count = 0;
#endif

int8_t ENV[10] = {0};
int16_t gx, gy, gz, ax, ay, az, mx, my, mz;
float roll_pitch_yaw[3] = {0};

void setup(void);
void setup_sensors(void);
void calibrate_gyro_accel();

void twi_imu_tx_event(uint8_t bytes);

void twi_imu_tx_event()
{
}

void calibrate_gyro_accel()
{
    int16_t values[6] = {0};
    
    #if DEBUG
    uart_puts("mpu6050 running calibration\n");
    #endif
    
    mpu6050_run_calibration(values);
    save_mpu6050_calibration(values);
    
    #if DEBUG
    uart_puts("mpu6050 calibration done\n");
    #endif
    
    fetch_mpu6050_calibration(values);
    mpu6050_set_x_accel_offset(values[0]);
    mpu6050_set_y_accel_offset(values[1]);
    mpu6050_set_z_accel_offset(values[2]);
    
    mpu6050_set_x_gyro_offset(values[3]);
    mpu6050_set_y_gyro_offset(values[4]);
    mpu6050_set_z_gyro_offset(values[5]);    
}

void setup(void)
{
    DDRB |= (1 << PB5);
 
    ENV[IMU_STATUS_ADDRESS] = IMU_STATUS_INITIALIZING;
    
    sei();
    
    _delay_ms(100);
    
    init_millis(F_CPU);
    twi_init();
    
    twi_attach_slave_tx_event(twi_imu_tx_event);
    
    #ifdef DEBUG
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    #endif
    
    _delay_ms(100);
    
    #ifdef DEBUG
    int16_t mpu6050_offset_values[6] = {0};
    fetch_mpu6050_calibration(mpu6050_offset_values);
    sprintf(
        BUFFER, 
        "mpu6050 calibration values %d %d %d %d %d %d\n",
        mpu6050_offset_values[0],
        mpu6050_offset_values[1],
        mpu6050_offset_values[2],
        mpu6050_offset_values[3],
        mpu6050_offset_values[4],
        mpu6050_offset_values[5]
    );
    uart_puts(BUFFER);
    uart_puts("setup done\n");
    #endif
    
    setup_sensors();
    
    ENV[IMU_STATUS_ADDRESS] = IMU_STATUS_READY_TO_START;
}

void setup_sensors(void)
{
    mpu6050_initialize();
    int16_t values[6] = {0};
    
    fetch_mpu6050_calibration(values);
    mpu6050_set_x_accel_offset(values[0]);
    mpu6050_set_y_accel_offset(values[1]);
    mpu6050_set_z_accel_offset(values[2]);
    
    mpu6050_set_x_gyro_offset(values[3]);
    mpu6050_set_y_gyro_offset(values[4]);
    mpu6050_set_z_gyro_offset(values[5]);
    
    #if DEBUG
    sprintf(BUFFER, "current millis %ld\n", millis());
    uart_puts(BUFFER);
    #endif

    hcm5883l_initialize();

    mahony_init();

    _delay_ms(100);
}

void calculate_roll_pitch_yaw()
{
    mpu6050_get_motion_6(&ax, &ay, &az, &gx, &gy, &gz);
    hcm5883l_get_heading(&mx, &my, &mz);
    
    mahony_update(gz * 0.001, gy * 0.001, gz * 0.001, ax * 0.001, ay * 0.001, az * 0.001, mx * 0.001, my * 0.001, mz * 0.001);
    
    roll_pitch_yaw[0] = getRoll();
    roll_pitch_yaw[1] = getPitch();
    roll_pitch_yaw[2] = getYaw();
}

int main(void)
{
    setup();
    
    long last = 0;
    long delta = 0;
    
    while (ENV[IMU_STATUS_ADDRESS] == IMU_STATUS_RUNNING)
    {     
        delta = millis() - last;
        
        if(delta > 1000)
        {
            PORTB ^= (1 << PB5);   
         
            calculate_roll_pitch_yaw();
            
            #if DEBUG
            #if SENSOR_RAW_VALUES
            sprintf(BUFFER,
                "raw\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                gx, gy, gz, ax, ay, az, mx, my, mz);
                uart_puts(BUFFER);
            #endif
            
            count++;
            sprintf(
                BUFFER,
                "rpy\t%f\t%f\t%f\treadings\t%d\n",
                roll_pitch_yaw[0],
                roll_pitch_yaw[1],
                roll_pitch_yaw[2],
                count
                );
            uart_puts(BUFFER);
            count = 0;
            #endif
            
            last = millis();
        }
        
        #if DEBUG
        count++;
        #endif
    }
}

