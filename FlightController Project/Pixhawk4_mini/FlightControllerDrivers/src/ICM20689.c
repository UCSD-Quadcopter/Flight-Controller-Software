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
 *      Author: zhang
 */



#include "ICM20689.h"



void __ICM20689_WriteReg(ICM20689* instance, uint8_t regAddr, uint8_t byte) {
    spiBeginDevice(instance->CS);
	spiReadWriteByte(instance->spiBus, SPI_ReadMode(regAddr)); //ICM20689 add 1 to MSB as read,
															   //which is the reverse of the regular standard
															   //Thus use SPI_ReadMode as the write mode
	spiReadWriteByte(instance->spiBus, byte);
    spiEndDevice(instance->CS);
}

uint8_t __ICM20689_ReadReg(ICM20689* instance, uint8_t regAddr) {
    spiBeginDevice(instance->CS);
	spiReadWriteByte(instance->spiBus, SPI_WriteMode(regAddr)); //Same as Write, reversed
	uint8_t rtn = *spiReadWriteByte(instance->spiBus, SPI_Dummy_Byte);
	spiEndDevice(instance->CS);
	return rtn;
}


ICM20689* newICM20689(ICM20689* instance, SPI* spiBus, GPIO* chip_select) {
	instance->spiBus = spiBus;
	instance->CS = chip_select;
	instance->Status = Ready;
	gpioWrite(chip_select, High);
	return instance;
}


uint8_t initICM20689(ICM20689* instance) {

	if(ICM20689_ReadReg(WHO_AM_I_REG) != 0x98) {
		return Failed;
	}

    ICM20689_WriteReg(PWR_MGMT1_REG, 0X80); // Reset
	delay(100);

    if((ICM20689_ReadReg(PWR_MGMT1_REG) >> 7) != 0) {
        return Failed; // Fail to reset in 100 ms
    } else {
    	ICM20689_WriteReg(PWR_MGMT1_REG, 0X00);
    }

    ICM20689_WriteReg(GYRO_CFG_REG, 3<<3);    // Gyro Full Scale -> ±2000dps
    ICM20689_WriteReg(ACCEL_CFG1_REG, 0<<3);  // Accel Full Scale -> ±2g
	ICM20689_WriteReg(ACCEL_CFG2_REG, 0x07);  // Disable DLPF, sample rate = 1k

	ICM20689_WriteReg(SMPLRT_REG, 0x00);
	//ICM20689_WriteReg(CFG_REG, 0x07); // Disable DLPF, sample rate = 8k
	ICM20689_WriteReg(CFG_REG, 0x07);

	ICM20689_WriteReg(INT_EN_REG, 0x00);	// Disable Interrupt
	ICM20689_WriteReg(FIFO_EN_REG, 0x00);  // Disable FIFO

	ICM20689_WriteReg(PWR_MGMT1_REG, 0x01);	  // Set up x-axis PLL if ready
	ICM20689_WriteReg(PWR_MGMT2_REG, 0x00);   // Turn on gyro and acce

	return Succeeded;
}

uint8_t ICM20689_getWhoAmI(ICM20689* instance) {
	return ICM20689_ReadReg(WHO_AM_I_REG);
}
