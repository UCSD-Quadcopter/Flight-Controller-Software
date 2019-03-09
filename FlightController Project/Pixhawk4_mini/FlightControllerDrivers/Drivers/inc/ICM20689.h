/*
 * ICM20689.h
 *
 *  Created on: Mar 3, 2019
 *      Author: Hongtao Zhang, Zihao Zhou
 */

#ifndef INC_ICM20689_H_
#define INC_ICM20689_H_

#include "THL_Library_Basic.h"

#define ICM20689_GYRO_250dps   0x00 // Gyro Full Scale -> ±250dps.  0x00 = [0000,0000]b
#define ICM20689_GYRO_500dps   0x08 // Gyro Full Scale -> ±500dps.  0x08 = [0000,1000]b
#define ICM20689_GYRO_1000dps  0x10 // Gyro Full Scale -> ±1000dps. 0x10 = [0001,0000]b
#define ICM20689_GYRO_2000dps  0x18 // Gyro Full Scale -> ±2000dps. 0x18 = [0001,1000]b

#define ICM20689_ACC_2g   0x00 // Accel Full Scale -> ±2g.   0x00 = [0000,0000]b
#define ICM20689_ACC_4g   0x08 // Accel Full Scale -> ±4g.   0x00 = [0000,1000]b
#define ICM20689_ACC_8g   0x10 // Accel Full Scale -> ±8g.   0x00 = [0001,0000]b
#define ICM20689_ACC_16g  0x18 // Accel Full Scale -> ±16g.  0x00 = [0001,1000]b


typedef struct{
	//Communication bus
	SPI* spiBus;
	GPIO* CS;
	GPIO* INT;

	//Internal setting
	volatile uint8_t acc_fsRange;
	volatile uint8_t gyro_fsRange;

	//Data
	volatile double acc_X;
	volatile double acc_Y;
	volatile double acc_Z;
	volatile double gyro_X;
	volatile double gyro_Y;
	volatile double gyro_Z;

	//Timing and Sampling status
	volatile uint32_t timeStamp;
	volatile uint8_t isDataReady;
}ICM20689;




void __ICM20689_WriteReg(ICM20689* instance, uint8_t regAddr, uint8_t byte);
uint8_t __ICM20689_ReadReg(ICM20689* instance, uint8_t regAddr);


ICM20689* newICM20689(ICM20689* instance, SPI* spiBus, GPIO* chip_select);

Bool initICM20689(ICM20689* instance, uint8_t gyro_fsRange, uint8_t acc_fsRange);

ICM20689* ICM20689_getAccel(ICM20689* instance);
ICM20689* ICM20689_getGyro(ICM20689* instance);

Bool ICM20689_clearINT_STATUS(ICM20689* instance);


#endif /* INC_ICM20689_H_ */
