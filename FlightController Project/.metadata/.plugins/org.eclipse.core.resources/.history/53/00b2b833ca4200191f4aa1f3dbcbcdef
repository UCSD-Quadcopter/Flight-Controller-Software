/*
                       _oo0oo_
                      o8888888o
                      88" . "88
                      (| -_- |)
                      0\  =  /0
                    ___/`---'\___
                  .' \\|     |// '.
                 / \\|||  :  |||// \
                / _||||| -:- |||||- \
               |   | \\\  -  /// |   |
               | \_|  ''\---/''  |_/ |
               \  .-\__  '-'  ___/-. /
             ___'. .'  /--.--\  `. .'___
          ."" '<  `.___\_<|>_/___.' >' "".
         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
         \  \ `_.   \_ __\ /__ _/   .-` /  /
     =====`-.____`.___ \_____/___.-`___.-'=====
                       `=---='
*/


/*
 * ICM20689.c
 *
 *  Created on: Mar 3, 2019
 *      Author: Hongtao Zhang, Zihao Zhou
 */



#include "ICM20689.h"


//-----------------------------------------------------
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
#define INT_STATUS_REG    0x3A

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
//-----------------------------------------------------
#define ICM_INT16_T_MAX 32768.000f; //32768 = 2^15 = max for signed int16_t


#define ICM_WriteReg_wMsk(regAddr, byte, mskByte) __ICM20689_WriteReg_wMsk(instance, regAddr, byte, mskByte)
#define ICM_WriteRegBit(regAddr, bit, leftShift) __ICM20689_WriteRegBit(instance, regAddr, bit, leftShift)
#define ICM_WriteReg(regAddr, byte) __ICM20689_WriteReg(instance, regAddr, byte)
#define ICM_ReadReg(regAddr) __ICM20689_ReadReg(instance, regAddr)




void __ICM20689_WriteReg(ICM20689* instance, uint8_t regAddr, uint8_t byte) {
    spiBeginDevice(instance->CS);
	spiReadWriteByte(instance->spiBus, SPI_ReadMode(regAddr)); //ICM20689 add 1 to MSB as read,
															   //which is the reverse of the regular standard
															   //Thus use SPI_ReadMode as the write mode
	spiReadWriteByte(instance->spiBus, byte);
    spiEndDevice(instance->CS);
	delay_us(50);
}

uint8_t __ICM20689_ReadReg(ICM20689* instance, uint8_t regAddr) {
    spiBeginDevice(instance->CS);
	spiReadWriteByte(instance->spiBus, SPI_WriteMode(regAddr)); //Same as Write, reversed
	uint8_t rtn = *spiReadWriteByte(instance->spiBus, SPI_Dummy_Byte);
	spiEndDevice(instance->CS);
	return rtn;
}
void __ICM20689_WriteRegBit(ICM20689* instance, uint8_t regAddr, uint8_t bit, uint8_t leftShift) {
	uint8_t byte = ICM_ReadReg(regAddr);
	if(bit == 1) ICM_WriteReg(regAddr, byte | (bit<<leftShift));
	else         ICM_WriteReg(regAddr, byte & (!bit<<leftShift));
}
void __ICM20689_WriteReg_wMsk(ICM20689* instance, uint8_t regAddr, uint8_t byte, uint8_t mskByte) {
	uint8_t regByte = ICM_ReadReg(regAddr);
	uint8_t mskedByte = (regByte & mskByte) | byte;
	ICM_WriteReg(regAddr, mskedByte);
}



ICM20689* newICM20689(ICM20689* instance, SPI* spiBus, GPIO* chip_select) {
	instance->spiBus = spiBus;
	instance->CS = chip_select;
    instance->isDataReady = NotReady;

    //default fsRange
    instance->acc_fsRange  = ICM20689_ACC_4g;
    instance->gyro_fsRange = ICM20689_GYRO_2000dps;

	gpioWrite(chip_select, High);
	return instance;
}


Bool initICM20689(ICM20689* instance, uint8_t gyro_fsRange, uint8_t acc_fsRange) {
	instance->gyro_fsRange = gyro_fsRange;
	instance->acc_fsRange = acc_fsRange;

	//Wait 1 sec b4 setting up
	delay(1000);

	if(ICM_ReadReg(WHO_AM_I_REG) != 0x98) {
		return Failed;
	}

	//Reset and restore to DEFAULT setting, MSB auto set back to 0 right after reset
	ICM_WriteReg(PWR_MGMT1_REG, 0X80); // 0x080 = [1000,0000]
	//Allow startup time
	delay(200);
	if((ICM_ReadReg(PWR_MGMT1_REG) >> 7) != 0) return Failed; // Fail to reset in 200 ms
	//Reset signal path
	ICM_WriteReg(SIGPATH_RST_REG, 0x03);
	delay(100);
	//Wake up IMU (DEFAULT setting is sleep mode in which bit[6] is 1)
	ICM_WriteReg(PWR_MGMT1_REG, 0X00);

	// Config Gyro fsRange and sample rate
	ICM_WriteReg_wMsk(GYRO_CFG_REG, instance->gyro_fsRange | 0x02, 0x04); // 0x04 = [0000,0100]b
														  // 0x02 = [0000,0010], bypass DLPF(digital low pass filter)
														  // Check out F_CHOICE_b. 8173Hz(-3dB) ~ 8595Hz(Noise BW)
														  /* 32kHz sample rate */

	// Config Accel fsRange and sample rate
	ICM_WriteReg_wMsk(ACCEL_CFG1_REG, instance->acc_fsRange, 0x07); // 0x07 = [000,0111]b
	ICM_WriteReg_wMsk(ACCEL_CFG2_REG, 0x08, 0xF7); //0xF7 = [1111,0111]b
	 	 	 	 	 	 	 	   // 0x08 = [0000,1000]b, bypass DLPF
								   // Check out F_CHOICE_b. 1046Hz(-3dB) ~ 1100Hz(Noise BW)
								   /* 4kHz sample rate */


	//Disable DMP related functionality
	ICM_WriteReg_wMsk(FIFO_EN_REG, 0x00, 0x07);  // Disable FIFO
	ICM_WriteReg(CFG_REG, 0x00); //Disable FSYNC

/*
	//Enable data ready interrupt
	ICM_WriteReg_wMsk(INT_PIN_CFG_REG, 0x10, 0x02); // 0x30 = [0011,0000]b, Latch: pin level held until interrupt status is cleared
	delay(100);
	ICM_WriteReg_wMsk(INT_EN_REG, 0x01, 0x08);
	delay(100);
*/


	//Achieve full gyro performance
	ICM_WriteReg(PWR_MGMT1_REG, 0x01);	  // Set up x-axis PLL if ready

	//Turn on gyro and accel
	ICM_WriteReg(PWR_MGMT2_REG, 0x00);

	delay(100);

	debugString("\r[GYRO_CFG_REG] = 0x%02hX \n", ICM_ReadReg(GYRO_CFG_REG));
	debugString("\r[ACCEL_CFG1_REG] = 0x%02hX \n", ICM_ReadReg(ACCEL_CFG1_REG));
	debugString("\r[ACCEL_CFG2_REG] = 0x%02hX \n", ICM_ReadReg(ACCEL_CFG2_REG));
	debugString("\r[FIFO_EN_REG] = 0x%02hX \n", ICM_ReadReg(FIFO_EN_REG));
	debugString("\r[CFG_REG] = 0x%02hX \n", ICM_ReadReg(CFG_REG));
	debugString("\r[INT_PIN_CFG_REG] = 0x%02hX \n", ICM_ReadReg(INT_PIN_CFG_REG));
	debugString("\r[INT_EN_REG] = 0x%02hX \n", ICM_ReadReg(INT_EN_REG));
	debugString("\r[PWR_MGMT1_REG] = 0x%02hX \n", ICM_ReadReg(PWR_MGMT1_REG));
	debugString("\r[PWR_MGMT2_REG] = 0x%02hX \n", ICM_ReadReg(PWR_MGMT2_REG));

	return Succeeded;
}


ICM20689* ICM20689_getAccel(ICM20689* instance) {
	uint8_t buf[6];

	buf[0] = ICM_ReadReg(ACCEL_XOUTH_REG);
	buf[1] = ICM_ReadReg(ACCEL_XOUTL_REG);
	buf[2] = ICM_ReadReg(ACCEL_YOUTH_REG);
	buf[3] = ICM_ReadReg(ACCEL_YOUTL_REG);
	buf[4] = ICM_ReadReg(ACCEL_ZOUTH_REG);
	buf[5] = ICM_ReadReg(ACCEL_ZOUTL_REG);

	int16_t acc_X_raw = (int16_t)(buf[0] << 8) | buf[1];
	int16_t acc_Y_raw = (int16_t)(buf[2] << 8) | buf[3];
	int16_t acc_Z_raw = (int16_t)(buf[4] << 8) | buf[5];

	double acc_fsRange;
	switch(instance->acc_fsRange) {
		case ICM20689_ACC_2g : acc_fsRange =  2.000f;  break;
		case ICM20689_ACC_4g : acc_fsRange =  4.000f;  break;
		case ICM20689_ACC_8g : acc_fsRange =  8.000f;  break;
		case ICM20689_ACC_16g : acc_fsRange = 16.000f; break;
		default: throwException("ICM20689_getAccel");
				 acc_fsRange = 4.000f;
		break;
	}

	instance->acc_X = acc_fsRange * (double)acc_X_raw / ICM_INT16_T_MAX;
	instance->acc_Y = acc_fsRange * (double)acc_Y_raw / ICM_INT16_T_MAX;
	instance->acc_Z = acc_fsRange * (double)acc_Z_raw / ICM_INT16_T_MAX;
	return instance;
}

ICM20689* ICM20689_getGyro(ICM20689* instance) {
	uint8_t buf[6];

	buf[0] = ICM_ReadReg(GYRO_XOUTH_REG);
	buf[1] = ICM_ReadReg(GYRO_XOUTL_REG);
	buf[2] = ICM_ReadReg(GYRO_YOUTH_REG);
	buf[3] = ICM_ReadReg(GYRO_YOUTL_REG);
	buf[4] = ICM_ReadReg(GYRO_ZOUTH_REG);
	buf[5] = ICM_ReadReg(GYRO_ZOUTL_REG);

	int16_t gyro_X_raw = (int16_t)(buf[0] << 8) | buf[1];
	int16_t gyro_Y_raw = (int16_t)(buf[2] << 8) | buf[3];
	int16_t gyro_Z_raw = (int16_t)(buf[4] << 8) | buf[5];

	double gyro_fsRange;
	switch(instance->gyro_fsRange) {
		case ICM20689_GYRO_250dps : gyro_fsRange =  250.000f;  break;
		case ICM20689_GYRO_500dps : gyro_fsRange =  500.000f;  break;
		case ICM20689_GYRO_1000dps : gyro_fsRange = 1000.000f; break;
		case ICM20689_GYRO_2000dps : gyro_fsRange = 2000.000f; break;
		default: throwException("ICM20689_getGyro");
				 gyro_fsRange = 2000.000f;
		break;
	}

	instance->gyro_X = gyro_fsRange * (double)gyro_X_raw / ICM_INT16_T_MAX;
	instance->gyro_Y = gyro_fsRange * (double)gyro_Y_raw / ICM_INT16_T_MAX;
	instance->gyro_Z = gyro_fsRange * (double)gyro_Z_raw / ICM_INT16_T_MAX;

	return instance;
}

Bool ICM20689_clearINT_STATUS(ICM20689* instance) {
	ICM_ReadReg(INT_STATUS_REG);
	return True;
}
