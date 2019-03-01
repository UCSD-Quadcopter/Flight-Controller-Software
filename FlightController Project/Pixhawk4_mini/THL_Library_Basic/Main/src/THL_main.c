#include "THL_main.h"
#include "THL_Usart.h"
#include "stm32f7xx.h"

extern UART_HandleTypeDef huart7;
GPIO  blue_led_mem; //instance memory allocated as static
GPIO* blue_led;

void THL_main(void) {
	newGPIO(blue_led, BLUE_LED_GPIO_Port, BLUE_LED_Pin);
	newMainUSART(&huart7);

	gpioWrite(blue_led, Low);
	printf_u("\rPixhawk4 mini Flight Controller from Team TritonWings at UCSD\r\n");

	delay(1000);
	gpioWrite(blue_led, High);

	for(int i = 0; i < 5; i++) {
		toggle(blue_led);
		delay(1000);
		toggle(blue_led);
		delay(1000);
	}
}


