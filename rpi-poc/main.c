
#include "sensors/mpu6050.h"

int main(void)
{
    int16_t ax, ay, az, gx, gy, gz;
    mpu6050_initialize();

    while(1)
    {
      mpu6050_get_motion_6(&ax, &ay, &az, &gx, &gy, &gz);
      printf("%d,%d,%d,%d,%d,%d\n", ax, ay, az, gx, gy, gz);
    }

    return 0;
}
