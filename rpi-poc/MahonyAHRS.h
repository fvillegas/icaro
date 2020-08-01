/**
 * clone from https://github.com/PaulStoffregen/MahonyAHRS/blob/master/src/MahonyAHRS.h
 * migrated to C
 */
//=============================================================================================
// MahonyAHRS.h
//=============================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=============================================================================================
#ifndef MahonyAHRS_h
#define MahonyAHRS_h
#include <math.h>

//--------------------------------------------------------------------------------------------

void mahony_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void mahony_update_imu(float gx, float gy, float gz, float ax, float ay, float az);
float mahony_get_roll();
float mahony_get_pitch();
float mahony_get_yaw();
float mahony_get_roll_radians();
float mahony_get_pitch_radians();
float mahony_get_yaw_radians();

#endif
