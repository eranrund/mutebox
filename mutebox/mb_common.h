#ifndef __MB_COMMON_H__
#define __MB_COMMON_H__

#include <mios32.h>
#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

extern xSemaphoreHandle xMIDIOUTSemaphore;
#define MUTEX_MIDIOUT_TAKE { if( xMIDIOUTSemaphore ) while( xSemaphoreTakeRecursive(xMIDIOUTSemaphore, (portTickType)1) != pdTRUE ); }
#define MUTEX_MIDIOUT_GIVE { if( xMIDIOUTSemaphore ) xSemaphoreGiveRecursive(xMIDIOUTSemaphore); }

void mb_common_init(void);

typedef enum {
    MB_LCD_NOT_FLASHING,
    MB_LCD_FLASHING,
    MB_LCD_DONE_FLASHING,
} mb_common_lcd_flash_state_e;
void mb_common_lcd_flash_msg(int msec, const char * fmt, ...);
mb_common_lcd_flash_state_e mb_common_lcd_flash_status(void);


#endif

