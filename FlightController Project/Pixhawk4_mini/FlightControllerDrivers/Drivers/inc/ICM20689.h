/*
 * ICM20689.h
 *
 *  Created on: Mar 3, 2019
 *      Author: zhang
 */

#ifndef INC_ICM20689_H_
#define INC_ICM20689_H_

#include "THL_Library_Basic.h"

#define ICM_ADDR 0x68    // AD0 should be low

#define SELF_TEST_X_GYRO_REG    0x00
#define SELF_TEST_Y_GYRO_RE     0x01
#define SELF_TEST_Z_GYRO_REG    0x02

#define SELF_TEST_X_ACCEL_REG   0x0D
#define SELF_TEST_Y_ACCEL_REG   0x0E
#define SELF_TEST_Z_ACCEL_REG   0x0F

#define SMPLRT_REG        0x19
#define CFG_REG           0x1A
#define GYRO_CFG_REG      0x1B
#define ACCEL_CFG1_REG    0x1C
#define ACCEL_CFG2_REG    0x1D

#define FIFO_EN_REG       0x23
#define FSYNC_INT_REG     0x36

#define INT_PIN_CFG_REG   0x37
#define INT_EN_REG        0x38
#define INT_STA_REG       0x3A

#define ACCEL_XOUTH_REG   0x3B
#define ACCEL_XOUTL_REG   0x3C
#define ACCEL_YOUTH_REG   0x3D
#define ACCEL_YOUTL_REG   0x3E
#define ACCEL_ZOUTH_REG   0x3F
#define ACCEL_ZOUTL_REG   0x40

#define TEMP_OUTH_REG     0x41
#define TEMP_OUTL_REG     0x42

#define GYRO_XOUTH_REG    0x43
#define GYRO_XOUTL_REG    0x44
#define GYRO_YOUTH_REG    0x45
#define GYRO_YOUTL_REG    0x46
#define GYRO_ZOUTH_REG    0x47
#define GYRO_ZOUTL_REG    0x48

#define SIGPATH_RST_REG   0x68
#define USER_CTRL_REG     0x6A
#define PWR_MGMT1_REG     0x6B
#define PWR_MGMT2_REG     0x6C
#define FIFO_CNTH_REG     0x72
#define FIFO_CNTL_REG     0x73
#define FIFO_RW_REG       0x74
#define WHO_AM_I_REG      0x75

#define ICM20689_WriteReg(regAddr, byte) __ICM20689_WriteReg(instance, regAddr, byte)
#define ICM20689_ReadReg(regAddr) __ICM20689_ReadReg(instance, regAddr)


typedef struct{
	SPI* spiBus;
	GPIO* CS;
	volatile uint8_t Status;
}ICM20689;


void __ICM20689_WriteReg(ICM20689* instance, uint8_t regAddr, uint8_t byte);
uint8_t __ICM20689_ReadReg(ICM20689* instance, uint8_t regAddr);


ICM20689* newICM20689(ICM20689* instance, SPI* spiBus, GPIO* chip_select);

uint8_t initICM20689(ICM20689* instance);

uint8_t ICM20689_getWhoAmI(ICM20689* instance);


#endif /* INC_ICM20689_H_ */
