#include <unistd.h>
#include <stdint.h>

#include "icarolib/twi/i2cdevlib.h"

#include "./hcm5883l.h"
#include "./hcm5883l_registers.h"

uint8_t mode;
uint8_t mag_buffer[6];

/** Set magnetic field gain value.
* @param gain New magnetic field gain value
* @see getGain()
* @see HMC5883L_RA_CONFIG_B
* @see HMC5883L_CRB_GAIN_BIT
* @see HMC5883L_CRB_GAIN_LENGTH
*/
void setGain(uint8_t gain)
{
	// use this method to guarantee that bits 4-0 are set to zero, which is a
	// requirement specified in the datasheet; it's actually more efficient than
	// using the I2Cdev.writeBits method
	i2c_write_byte(HMC5883L_ADDRESS, HMC5883L_CONFIG_B, gain << (HMC5883L_CRB_GAIN_BIT - HMC5883L_CRB_GAIN_LENGTH + 1));
}

/** Set measurement mode.
* @param newMode New measurement mode
* @see getMode()
* @see HMC5883L_MODE_CONTINUOUS
* @see HMC5883L_MODE_SINGLE
* @see HMC5883L_MODE_IDLE
* @see HMC5883L_RA_MODE
* @see HMC5883L_MODEREG_BIT
* @see HMC5883L_MODEREG_LENGTH
*/
void setMode(uint8_t newMode)
{
	// use this method to guarantee that bits 7-2 are set to zero, which is a
	// requirement specified in the datasheet; it's actually more efficient than
	// using the I2Cdev.writeBits method
	i2c_write_byte(HMC5883L_ADDRESS, HMC5883L_MODE, newMode << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
	mode = newMode; // track to tell if we have to clear bit 7 after a read
}

void hcm5883l_initialize()
{
	// write CONFIG_A register
	i2c_write_byte(HMC5883L_ADDRESS, HMC5883L_CONFIG_A,
	(HMC5883L_AVERAGING_8 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
	(HMC5883L_RATE_15 << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
	(HMC5883L_BIAS_NORMAL << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1)));

	// write CONFIG_B register
	setGain(HMC5883L_GAIN_1090);

	// write MODE register
	setMode(HMC5883L_MODE_SINGLE);
}

/** Get 3-axis heading measurements.
* In the event the ADC reading overflows or underflows for the given channel,
* or if there is a math overflow during the bias measurement, this data
* register will contain the value -4096. This register value will clear when
* after the next valid measurement is made. Note that this method automatically
* clears the appropriate bit in the MODE register if Single mode is active.
* @param x 16-bit signed integer container for X-axis heading
* @param y 16-bit signed integer container for Y-axis heading
* @param z 16-bit signed integer container for Z-axis heading
* @see HMC5883L_RA_DATAX_H
*/
void hcm5883l_get_heading(int16_t *x, int16_t *y, int16_t *z)
{
	i2c_read_bytes(
        HMC5883L_ADDRESS,
        HMC5883L_DATAX_H,
        6,
        mag_buffer,
        I2CDEV_DEFAULT_READ_TIMEOUT
    );
	if (mode == HMC5883L_MODE_SINGLE)
	{
		i2c_write_byte(HMC5883L_ADDRESS,
        HMC5883L_MODE,
        HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
	}
	*x = (((int16_t)mag_buffer[0]) << 8) | mag_buffer[1];
	*y = (((int16_t)mag_buffer[4]) << 8) | mag_buffer[5];
	*z = (((int16_t)mag_buffer[2]) << 8) | mag_buffer[3];
}