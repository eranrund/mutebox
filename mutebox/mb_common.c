#include "mb_common.h"

xSemaphoreHandle xMIDIINSemaphore;
xSemaphoreHandle xMIDIOUTSemaphore;

void mb_common_init(void)
{
    xMIDIINSemaphore = xSemaphoreCreateRecursiveMutex();
    xMIDIOUTSemaphore = xSemaphoreCreateRecursiveMutex();
}

