#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

// 0 - STATUS[1] 1- ROLL[4] 5- PITCH[4] 9- YAW[4]
uint8_t volatile REGISTER[REGISTER_LENGTH] = {0};
int16_t gx, gy, gz, ax, ay, az, mx, my, mz;

uint8_t twi_request_address = 0;

union Float f;

void setup(void);
void setup_sensors(void);
void calibrate_gyro_accel(void);
void on_receive(int num_bytes);
void on_request();

void on_receive(int num_bytes)
{
    twi_request_address = wire_read();
    for (; wire_available() && twi_request_address < REGISTER_LENGTH; twi_request_address++)
    { REGISTER[twi_request_address] = wire_read(); }
}

void on_request()
{
    for (;wire_get_status() == TWI_STX && twi_request_address < REGISTER_LENGTH; twi_request_address++)
    { wire_write(REGISTER[twi_request_address]); }
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

    f.m_float = getRoll();
    REGISTER[IMU_ROLL_ADDRESS] = f.m_bytes[0];
    REGISTER[IMU_ROLL_ADDRESS + 1] = f.m_bytes[1];
    REGISTER[IMU_ROLL_ADDRESS + 2] = f.m_bytes[2];
    REGISTER[IMU_ROLL_ADDRESS + 3] = f.m_bytes[3];
    
    f.m_float = getPitch();
    REGISTER[IMU_PITCH_ADDRESS] = f.m_bytes[0];
    REGISTER[IMU_PITCH_ADDRESS + 1] = f.m_bytes[1];
    REGISTER[IMU_PITCH_ADDRESS + 2] = f.m_bytes[2];
    REGISTER[IMU_PITCH_ADDRESS + 3] = f.m_bytes[3];
    
    f.m_float = getYaw();
    REGISTER[IMU_YAW_ADDRESS] = f.m_bytes[0];
    REGISTER[IMU_YAW_ADDRESS + 1] = f.m_bytes[1];
    REGISTER[IMU_YAW_ADDRESS + 2] = f.m_bytes[2];
    REGISTER[IMU_YAW_ADDRESS + 3] = f.m_bytes[3];
    f.m_float = 0;
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
    
    long last_debug = 0;
    long last_imu = 0;
    long delta = 0;
    
    while(1)
    {
        if (REGISTER[IMU_STATUS_ADDRESS] == IMU_STATUS_RUNNING)
        {
            delta = millis() - last_imu;
            if (delta > 14)
            {
                calculate_roll_pitch_yaw();
                #if DEBUG
                count++;
                #endif
                last_imu = millis();
            }
            delta = millis() - last_debug;
            if (delta > 1000)
            {
                PORTB ^= (1 << PB5);
                last_debug = millis();
            }
        }
        else if (REGISTER[IMU_STATUS_ADDRESS] == IMU_STATUS_CALIBRATING)
        {
            #ifdef DEBUG
            uart_puts("running calibration\n");
            #endif
            PORTB ^= (1 << PB5);
            calibrate_gyro_accel();
            REGISTER[IMU_STATUS_ADDRESS] = IMU_STATUS_READY_TO_START;
            PORTB ^= (1 << PB5);
            #ifdef DEBUG
            uart_puts("calibration done\n");
            #endif
        }
    }
}

