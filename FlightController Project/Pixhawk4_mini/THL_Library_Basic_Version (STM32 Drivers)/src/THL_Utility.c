#include "THL_Utility.h"

/*==========================Exception Handling==============================*/
__weak void Exception_Handler(const char* str) {
	UNUSED(str);
}
void throwException(const char* str) {
	Exception_Handler(str);
}
/*==========================================================================*/
