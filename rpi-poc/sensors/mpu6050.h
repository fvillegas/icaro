#ifndef __MPU6050_H_
#define __MPu6050_H_

#include <stdlib.h>
#include <stdint.h>

void mpu6050_initialize();
void mpu6050_set_clock_source(uint8_t source);

#endif
