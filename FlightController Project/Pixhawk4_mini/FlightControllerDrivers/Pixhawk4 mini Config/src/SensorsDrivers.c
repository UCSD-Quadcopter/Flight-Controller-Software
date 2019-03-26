/*
 * SensorsDrivers.c
 *
 *  Created on: Mar 4, 2019
 *      Author: zhang
 */

#include "SensorsDrivers.h"


extern SPI_HandleTypeDef hspi1;
SPI spi1_mem;
SPI* spi1;

ICM20689  IMU1_mem;
ICM20689* IMU1;
GPIO IMU1_CS_mem;
GPIO* IMU1_CS;
volatile uint8_t IMU1_INT_EN = False;



/*==============================ICM20689===============================*/

volatile uint8_t isIMU1_DataReady(void) {
	return IMU1->isDataReady;
}
ICM20689* getIMU1(void) {
	//ICM20689_getAccel(IMU1);
	ICM20689_getGyro(IMU1);
	IMU1->isDataReady = NotReady;
	return IMU1;
}

void IMU1_ISR(void) {
	if(IMU1_INT_EN == False) return;
	if(ICM20689_clearINT_STATUS(IMU1)) {
		IMU1->timeStamp = micros();
		IMU1->isDataReady = Ready;
	}
}

/*=====================================================================*/



void initSensors() {
	uint8_t init_status;
	spi1 = newSPI(&spi1_mem, &hspi1);

	IMU1_CS = newGPIO(&IMU1_CS_mem, ICM20689_CS_GPIO_Port, ICM20689_CS_Pin);
	IMU1 = newICM20689(&IMU1_mem, spi1, IMU1_CS);
	init_status = initICM20689(IMU1, ICM20689_GYRO_1000dps, ICM20689_ACC_4g);
	printf_u("\rinit_status = %s\r\n", init_status == Succeeded ? "Succeeded" : "Failed");
	//IMU1_INT_EN = True;

}
