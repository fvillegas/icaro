#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sensors/mpu6050.h"
#include "MahonyAHRS.h"

#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536

#define dt 0.01 // 10 ms sample rate!

void complementary_filter(short accData[3], short gyrData[3], float *pitch, float *roll)
{
  float pitchAcc, rollAcc;

  // Integrate the gyroscope data -> int(angularSpeed) = angle
  *pitch += ((float)gyrData[0] / GYROSCOPE_SENSITIVITY) * dt; // Angle around the X-axis
  *roll -= ((float)gyrData[1] / GYROSCOPE_SENSITIVITY) * dt;  // Angle around the Y-axis

  // Compensate for drift with accelerometer data if !bullshit
  // Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
  int forceMagnitudeApprox = abs(accData[0]) + abs(accData[1]) + abs(accData[2]);
  if (forceMagnitudeApprox > 8192 && forceMagnitudeApprox < 32768)
  {
    // Turning around the X axis results in a vector on the Y-axis
    pitchAcc = atan2f((float)accData[1], (float)accData[2]) * 180 / M_PI;
    *pitch = *pitch * 0.98 + pitchAcc * 0.02;

    // Turning around the Y axis results in a vector on the X-axis
    rollAcc = atan2f((float)accData[0], (float)accData[2]) * 180 / M_PI;
    *roll = *roll * 0.98 + rollAcc * 0.02;
  }
}

short accData[3], gyrData[3];
float pitch, roll;

Mahony filter;

void calculate_pitch_roll_yaw()
{
  int16_t ax, ay, az, gx, gy, gz;
  
  mpu6050_get_motion_6(&ax, &ay, &az, &gx, &gy, &gz);
  
//  accData[0] = ax;
//  accData[1] = ay;
//  accData[2] = az;
//
//  gyrData[0] = gx;
//  gyrData[1] = gy;
//  gyrData[2] = gz;
//
//  complementary_filter(accData, gyrData, &pitch, &roll);
// Update the Mahony filter, with scaled gyroscope
  float gyroScale = 0.001;  // TODO: the filter updates too fast
  filter.updateIMU(gx * gyroScale, gy * gyroScale, gz * gyroScale, ax, ay, az);

  printf("%f\t%f\t%f\n", filter.getPitch(), filter.getRoll(), filter.getYaw());
}

int main(void)
{
  mpu6050_initialize();

  while (1)
  {
    calculate_pitch_roll_yaw();
  }

  return 0;
}
