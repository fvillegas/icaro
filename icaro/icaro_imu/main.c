#include <avr/io.h>

#include "icaro/twi.h"
#include "icaro/imu.h"

void setup(void)
{
    twi_init();
    twi_set_address(IMU_ADDRESS);    
}

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

