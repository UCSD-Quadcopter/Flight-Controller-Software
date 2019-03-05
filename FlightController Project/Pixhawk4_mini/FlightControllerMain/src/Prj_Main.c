#include "Prj_Main.h"
#include "THL_Library_Basic.h"

#include "ICM20689.h"

extern UART_HandleTypeDef huart7;

GPIO  blue_led_mem; //instance memory allocated as static
GPIO* blue_led;

extern SPI_HandleTypeDef hspi1;
SPI spi1_mem;
SPI* spi1;
ICM20689 IMU_mem;
ICM20689* IMU;
GPIO IMU_CS_mem;
GPIO* IMU_CS;

void prj_main(void) {
	blue_led = newGPIO(&blue_led_mem, BLUE_LED_GPIO_Port, BLUE_LED_Pin);
	newMainUSART(&huart7);

	//Shut the other devices on the same SPI bus
	HAL_GPIO_WritePin(ICM20602_CS_GPIO_Port, ICM20602_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BMI055_Acc_CS_GPIO_Port, BMI055_Acc_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BMI055_Gyro_CS_GPIO_Port, BMI055_Gyro_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Aux_Mem_CS_GPIO_Port, Aux_Mem_CS_Pin, GPIO_PIN_SET);


	IMU_CS = newGPIO(&IMU_CS_mem, ICM20689_CS_GPIO_Port, ICM20689_CS_Pin);
	spi1 = newSPI(&spi1_mem, &hspi1);
	IMU = newICM20689(&IMU_mem, spi1, IMU_CS);

	gpioWrite(blue_led, Low);
	printf_u("\rPixhawk4 mini Flight Controller from Team TritonWings at UCSD\r\n");

	gpioWrite(blue_led, High);
	for(int i = 0; i < 5; i++) {
		toggle(blue_led);
		delay(100);
		toggle(blue_led);
		delay(100);
	}
	printf_u("\r WhoAmI = %x\r\n", ICM20689_getWhoAmI(IMU));
}
