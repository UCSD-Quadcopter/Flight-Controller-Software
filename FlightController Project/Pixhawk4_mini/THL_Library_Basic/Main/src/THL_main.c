#include "THL_main.h"
#include "THL_Usart.h"

extern UART_HandleTypeDef huart7;
void THL_main(void) {
	USART* serial_console = newMainUSART(&huart7);
	while(1) print(serial_console, "\rHellow World! \r\n");
	delay(1000);
}


