#ifndef __MPU6050_H_
#define __MPu6050_H_

#include <stdint.h>
#include <inttypes.h>

uint8_t mpu6050_self_test(void);
void mpu6050_initialize();
void mpu6050_get_motion_6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);

uint8_t mpu6050_who_am_i();
uint8_t mpu6050_test_connection(void);
void mpu6050_run_calibration(uint16_t* values);

#endif
