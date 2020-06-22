#ifndef __MPU6050_H_
#define __MPu6050_H_

#include <stdlib.h>
#include <stdint.h>

void mpu6050_initialize();
void mpu6050_get_motion_6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);

#endif
