/*
 * SensorsDrivers.h
 *
 *  Created on: Mar 4, 2019
 *      Author: zhang
 */

#ifndef INC_SENSORSDRIVERS_H_
#define INC_SENSORSDRIVERS_H_

#include "main.h"
#include "THL_Library_Basic.h"
#include "ICM20689.h"

/*Sensor data are updated realtime either
 *by DRDY interrupt service routine/Timer periodic interrupt
 *or an independent RTOS task.
 */


volatile uint8_t isIMU1_DataReady(void);
void IMU1_ISR(void);
ICM20689* getIMU1(void);

void initSensors(void);




#endif /* INC_SENSORSDRIVERS_H_ */
