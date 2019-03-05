#ifndef __THL_Utility_H
#define __THL_Utility_H
#include "THL_Portability.h"

/*=============================COMMON STATES===============================*/
#define True 1
#define False 0

#define Succeeded 1
#define Failed 0

#define High 1
#define Low 0

#define Enabled 1
#define Disabled 0

#define On 1
#define Off 0

typedef enum
{
	NotReady = 0,
	Ready = 1,
	InProcess = 2,
	Completed = 3,
	TimeOut = 4,
	Error = 5
}CommStatus;



/*=========================================================================*/

/*=============================FORMAT STRINGS==============================*/
#define Format_Param const char* format, ...	
#define formatStrings(bufferPointer) do{ \
	va_list args; \
    va_start(args, format); \
    vsprintf(bufferPointer, format, args);\
	va_end(args); \
}while(0)


/*=========================================================================*/


__weak void Exception_Handler(const char* str);
void throwException(const char* str);

#endif
