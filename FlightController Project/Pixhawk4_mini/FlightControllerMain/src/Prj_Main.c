/*
 * Prj_Main.c
 *
 *      Author: Hongtao Zhang
 */

#include <string.h>

#include "Prj_Main.h"
#include "THL_Library_Basic.h"
#include "SensorsDrivers.h"
#include "FC_IRS.h"


char debugStr[200];
void flushDebugStr(char* str);


extern UART_HandleTypeDef huart7;

GPIO  blue_led_mem; //instance memory allocated as static
GPIO* blue_led;

GPIO  red_led_mem; //instance memory allocated as static
GPIO* red_led;

GPIO  green_led_mem; //instance memory allocated as static
GPIO* green_led;

USART* console;


void prj_main(void) {
	blue_led = newGPIO(&blue_led_mem, BLUE_LED_GPIO_Port, BLUE_LED_Pin);
	red_led = newGPIO(&red_led_mem, RED_LED_GPIO_Port, RED_LED_Pin);
	green_led = newGPIO(&green_led_mem, GREEN_LED_GPIO_Port, GREEN_LED_Pin);

	console = newMainUSART(&huart7);

	HAL_GPIO_WritePin(ICM20602_CS_GPIO_Port, ICM20602_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BMI055_Acc_CS_GPIO_Port, BMI055_Acc_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BMI055_Gyro_CS_GPIO_Port, BMI055_Gyro_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Aux_Mem_CS_GPIO_Port, Aux_Mem_CS_Pin, GPIO_PIN_SET);


	printf_u("\rPixhawk4 mini Flight Controller from Team TritonWings at UCSD\r\n");

	/*gpioWrite(blue_led, High);
	for(int i = 0; i < 5; i++) {
		toggle(blue_led);
		delay(100);
		toggle(blue_led);
		delay(100);
	}*/
	gpioWrite(blue_led, Low);
	gpioWrite(red_led, Low);
	gpioWrite(green_led, Low);


	initSensors();

	printf_u("\r%s", debugStr);
	flushDebugStr(debugStr);


#define SAMPLE_SIZE 100

	ICM20689* IMU = getIMU1();

	//double acc_X[SAMPLE_SIZE], acc_Y[SAMPLE_SIZE], acc_Z[SAMPLE_SIZE];
	//double gyro_X[SAMPLE_SIZE], gyro_Y[SAMPLE_SIZE], gyro_Z[SAMPLE_SIZE];
	//uint32_t timeStamps[SAMPLE_SIZE];
	int dataIdx = 0;
	int prtIdx = 0;

	uint32_t t1;
	uint32_t t0 = micros();

	while(1) {
		IMU = getIMU1();
		t0 = micros();

		while(console->TxStatus != Completed);
		printf_DMA("\r%lf %lf %lf %lf %lf %lf %d\n", IMU->acc_X,IMU->acc_Y,IMU->acc_Z,IMU->gyro_X,IMU->gyro_Y,IMU->gyro_Z, t0);
		t1 = micros() - t0;


		//while(console->TxStatus != Completed);
		//printf_DMA("\rTime took = %d us\n", t1);
	}



/*
	while(1) {
		if(dataIdx > SAMPLE_SIZE) {

			for(dataIdx = 1; dataIdx < SAMPLE_SIZE; dataIdx++) {
				printf_u("\rAcc| x[%10.6lf] y[%10.6lf] z[%10.6lf] | ***",
						acc_X[dataIdx], acc_Y[dataIdx], acc_Z[dataIdx]);
				printf_u("*** Gyro| x[%15.6lf] y[%15.6lf] z[%15.6lf] |",
						gyro_X[dataIdx], gyro_Y[dataIdx], gyro_Z[dataIdx]);
				printf_u(" Time Stamp[%12d us]\r\n", timeStamps[dataIdx] - t0);
			}
			printf_u("\r\n");




			dataIdx = 0;
		} else {
			//t0 = micros();
			IMU = getIMU1();
			//t1 = micros() - t0;
			//printf_u("\r%d\n", t1);
			timeStamps[dataIdx] = micros();
			acc_X[dataIdx] = IMU->acc_X;
			acc_Y[dataIdx] = IMU->acc_Y;
			acc_Z[dataIdx] = IMU->acc_Z;

			gyro_X[dataIdx] = IMU->gyro_X;
			gyro_Y[dataIdx] = IMU->gyro_Y;
			gyro_Z[dataIdx] = IMU->gyro_Z;

		    dataIdx++;

			gpioWrite(blue_led, Low);
		}

	}
	*/
}


void Exception_Handler(const char* str) {
	printf_u("\r%s\r\n",str);
}


void DebugStr_Handler(const char* str) {
	static uint8_t isFirstCall = 0;
	if(isFirstCall == 0) strcpy(debugStr, str);
	else strcat(debugStr, str);
	isFirstCall++;
}
void flushDebugStr(char* str) {
	memset(str, 0, strlen(str));
}


