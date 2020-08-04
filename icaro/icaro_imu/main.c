#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "icarolib/timer/timer.h"
#include "icarolib/twi/i2cdevlib.h"
#include "icarolib/twi/twi.h"
#include "icarolib/icaro_common.h"
#include "icarolib/float.h"

#include "./sensors/mpu6050.h"
#include "./sensors/hcm5883l.h"
#include "./eeprom/eeprom.h"
#include "./mahony.h"

#ifdef DEBUG
#include "icarolib/uart/uart.h"
char BUFFER[150] = {0};
int count = 0;
#endif

#define REGISTER_LENGTH 13

// STATUS[1] ROLL[4] PITCH[4] YAW[4]
uint8_t REGISTER[REGISTER_LENGTH] = {0};
int16_t gx, gy, gz, ax, ay, az, mx, my, mz;

uint8_t twi_request_address = 0;

void setup(void);
void setup_sensors(void);
void calibrate_gyro_accel(void);
void on_receive(int num_bytes);
void on_request();

void on_receive(int num_bytes)
{
    int address = wire_read();   
    for (; wire_available() && address < REGISTER_LENGTH; address++)
    { REGISTER[address] = wire_read(); }
}

void on_request()
{
    if (twi_request_address) { return; }
    int i = twi_request_address;
    for (;wire_get_status() == TWI_STX && i < REGISTER_LENGTH; i++)
    { wire_write(REGISTER[i]); }
}

void calibrate_gyro_accel(void)
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
    
    REGISTER[IMU_STATUS_ADDRESS] = IMU_STATUS_INITIALIZING;
    
    sei();
    
    _delay_ms(100);
    
    init_millis(F_CPU);
    wire_init();
    wire_set_address(IMU_TWI_ADDRESS);
    wire_set_on_receive(on_receive);
    wire_set_on_request(on_request);
    
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
    
    mahony_update(
        gz * 0.001,
        gy * 0.001,
        gz * 0.001,
        ax * 0.001,
        ay * 0.001,
        az * 0.001,
        mx * 0.001,
        my * 0.001,
        mz * 0.001);

    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
     float_to_bytes(getRoll(), &REGISTER[IMU_ROLL_ADDRESS]);
     float_to_bytes(getPitch(), &REGISTER[IMU_PITCH_ADDRESS]);
     float_to_bytes(getYaw(), &REGISTER[IMU_YAW_ADDRESS]);   
    }    
}

int main(void)
{
    REGISTER[IMU_STATUS_ADDRESS] = IMU_STATUS_INITIALIZING;
    
    setup();
    setup_sensors();
    
    REGISTER[IMU_STATUS_ADDRESS] = IMU_STATUS_READY_TO_START;
    
    #ifdef DEBUG
    uart_puts("ready to start\n");
    #endif
    
    long last = 0;
    long delta = 0;
    
    while(1)
    {
        delta = millis() - last;
        
        if (REGISTER[IMU_STATUS_ADDRESS] == IMU_STATUS_RUNNING)
        {
            calculate_roll_pitch_yaw();
        
            if(delta > 1000)
            {
                PORTB ^= (1 << PB5);
        
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
                    bytes_to_float(&REGISTER[IMU_ROLL_ADDRESS]),
                    bytes_to_float(&REGISTER[IMU_PITCH_ADDRESS]),
                    bytes_to_float(&REGISTER[IMU_YAW_ADDRESS]),
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
        else if (REGISTER[IMU_STATUS_CALIBRATING] == IMU_STATUS_RUNNING)
        {
            calibrate_gyro_accel();
        }
    }
}

