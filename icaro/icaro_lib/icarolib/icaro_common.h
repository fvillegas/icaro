/*
 * common.h
 *
 * Created: 8/1/2020 11:22:19 AM
 *  Author: fcvil
 */ 


#ifndef __ICARO_COMMON_H_
#define __ICARO_COMMON_H_

#define IMU_TWI_ADDRESS 1
#define IMU_STATUS_ADDRESS 0
#define IMU_STATUS_INITIALIZING  1
#define IMU_STATUS_CALIBRATING  2
#define IMU_STATUS_READY_TO_START 3
#define IMU_STATUS_RUNNING 10

#define  IMU_ROLL_ADDRESS 1
#define  IMU_PITCH_ADDRESS 5
#define  IMU_YAW_ADDRESS 9

#define IMU_START 0
#define IMU_STOP 1

#endif