#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sensors/mpu6050.h"
#include "sensors/hcm5883l.h"
#include "MahonyAHRS.h"

#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536

#define dt 0.01 // 10 ms sample rate!

short accData[3], gyrData[3];
float pitch, roll;

void calculate_pitch_roll_yaw()
{
  int16_t ax, ay, az, gx, gy, gz, mx, my, mz;
  mpu6050_get_motion_6(&ax, &ay, &az, &gx, &gy, &gz);
  getHeading(&mx, &my, &mz);
  float gyroScale = 3.14159f / 180.0f;
  mahony_update(gx * gyroScale, gy * gyroScale, gz * gyroScale, ax, ay, az, mx, my, mz);

  printf("%f\t%f\t%f\n",
    mahony_get_pitch(),
    mahony_get_roll(),
    mahony_get_yaw()
  );
}

int main(void)
{
  mpu6050_initialize();
  hcm5883l_initialize();
  while (1)
  {
    calculate_pitch_roll_yaw();
  }

  return 0;
}
