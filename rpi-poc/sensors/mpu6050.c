/**
 * Based on https://www.i2cdevlib.com/
 * 
 */

#include <stdbool.h>

#include "mpu6050.h"
#include "mpu6050_registers.h"

/**
 * Set clock source setting.
 * 
 * An internal 8MHz oscillator, gyroscope based clock, or external sources can
 * be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
 * or an external source is chosen as the clock source, the MPU-60X0 can operate
 * in low power modes with the gyroscopes disabled.
 *
 * Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
 * However, it is highly recommended that the device be configured to use one of
 * the gyroscopes (or an external clock source) as the clock reference for
 * improved stability. The clock source can be selected according to the following table:
 *
 * <pre>
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
 * </pre>
 *
 * @param source New clock source setting
 * @see getClockSource()
 * @see MPU6050_PWR_MGMT_1
 * @see MPU6050_PWR_MGMT_1_CLK_SEL_BIT
 * @see MPU6050_PWR_MGMT_1_CLK_SEL_LENGTH
 */
void mpu6050_set_clock_source(uint8_t source)
{
    write_bits(
        MPU6050_ADDRESS,
        MPU6050_PWR_MGMT_1,
        MPU6050_PWR_MGMT_1_CLK_SEL_BIT,
        MPU6050_PWR_MGMT_1_CLK_SEL_LENGTH, 
        source
    );
}

/**
 * Set full-scale gyroscope range.
 * 
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_GYRO_CONFIG
 * @see MPU6050_GYRO_FS_SEL_BIT
 * @see MPU6050_GYRO_FS_SEL_LENGTH
 */
void mpu6050_set_full_scale_gyro_range(uint8_t range)
{
    write_bits(
        MPU6050_ADDRESS,
        MPU6050_GYRO_CONFIG,
        MPU6050_GYRO_FS_SEL_BIT,
        MPU6050_GYRO_FS_SEL_LENGTH,
        range
    );
}

/**
 * Set full-scale accelerometer range.
 * 
 * @param range New full-scale accelerometer range setting
 * @see getFullScaleAccelRange()
 */
void mpu6050_set_full_scale_accel_range(uint8_t range)
{
    write_bits(
        MPU6050_ADDRESS,
        MPU6050_ACCEL_CONFIG,
        MPU6050_ACCEL_CONFIG_AFS_SEL_BIT,
        MPU6050_ACCEL_CONFIG_AFS_SEL_LENGTH,
        range
    );
}

/**
 * Set sleep mode status.
 * 
 * @param enabled New sleep mode enabled status
 * @see getSleepEnabled()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
void mpu6050_set_sleep_enabled(bool enabled)
{
    write_bit(
        MPU6050_ADDRESS,
        MPU6050_PWR_MGMT_1,
        MPU6050_PWR_MGMT_1_SLEEP_BIT,
        enabled
    );
}

/**
 * Set I2C Master Mode enabled status.
 * 
 * @param enabled New I2C Master Mode enabled status
 * 
 * @see MPU6050_USER_CTRL
 * @see MPU6050_USER_CTRL_I2C_MST_EN_BIT
 */
void mput6050_set_I2C_master_mode_enabled(bool enabled) {
    write_bit(
        MPU6050_ADDRESS,
        MPU6050_USER_CTRL,
        MPU6050_USER_CTRL_I2C_MST_EN_BIT,
        enabled
    );
}

/**
 * Set I2C bypass enabled status.
 * 
 * When this bit is equal to 1 and I2C_MST_EN (Register 106 bit[5]) is equal to
 * 0, the host application processor will be able to directly access the
 * auxiliary I2C bus of the MPU-60X0. When this bit is equal to 0, the host
 * application processor will not be able to directly access the auxiliary I2C
 * bus of the MPU-60X0 regardless of the state of I2C_MST_EN (Register 106
 * bit[5]).
 * 
 * @param enabled New I2C bypass enabled status
 * 
 * @see MPU6050_INT_PIN_CFG
 * @see MPU6050_INT_PIN_CFG_I2C_BYPASS_EN_BIT
 */
void mpu6050_set_I2C_bypass_enabled(bool enabled) {
    write_bit(
        MPU6050_ADDRESS,
        MPU6050_INT_PIN_CFG,
        MPU6050_INT_PIN_CFG_I2C_BYPASS_EN_BIT,
        enabled
    );
}

/**
 * Power on and prepare for general usage.
 * 
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void mpu6050_initialize()
{
    mput6050_set_I2C_master_mode_enabled(false);
    mpu6050_set_I2C_bypass_enabled(true);
    mpu6050_set_clock_source(MPU6050_CLOCK_PLL_XGYRO);
    mpu6050_set_full_scale_gyro_range(MPU6050_GYRO_FS_250);
    mpu6050_set_full_scale_accel_range(MPU6050_ACCEL_FS_2);
    mpu6050_set_sleep_enabled(false);
}

/**
 * Get raw 6-axis motion sensor readings (accel/gyro).
 * 
 * Retrieves all currently available motion sensor values.
 * @param ax 16-bit signed integer container for accelerometer X-axis value
 * @param ay 16-bit signed integer container for accelerometer Y-axis value
 * @param az 16-bit signed integer container for accelerometer Z-axis value
 * @param gx 16-bit signed integer container for gyroscope X-axis value
 * @param gy 16-bit signed integer container for gyroscope Y-axis value
 * @param gz 16-bit signed integer container for gyroscope Z-axis value
 * @see getAcceleration()
 * @see getRotation()
 * @see MPU6050_ACCEL_XOUT_H
 */
void mpu6050_get_motion_6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    uint8_t buffer[14];

    read_bytes(
        MPU6050_ADDRESS,
        MPU6050_ACCEL_XOUT_H,
        14,
        buffer
    );

    *ax = (((int16_t) buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t) buffer[2]) << 8) | buffer[3];
    *az = (((int16_t) buffer[4]) << 8) | buffer[5];
    *gx = (((int16_t) buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t) buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t) buffer[12]) << 8) | buffer[13];
}
