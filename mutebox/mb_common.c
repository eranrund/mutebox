#include <string.h>
#include <stdarg.h>
#include "mb_common.h"

xSemaphoreHandle xMIDIINSemaphore;
xSemaphoreHandle xMIDIOUTSemaphore;

void mb_common_init(void)
{
    xMIDIINSemaphore = xSemaphoreCreateRecursiveMutex();
    xMIDIOUTSemaphore = xSemaphoreCreateRecursiveMutex();
}




u32 mb_common_lcd_stop_flashing_at = 0;

void mb_common_lcd_flash_msg(int msec, const char * fmt, ...)
{
  char buffer[64];
  va_list args;

  va_start(args, fmt);
  vsprintf((char *)buffer, fmt, args);

  mb_common_lcd_stop_flashing_at = MIOS32_TIMESTAMP_Get() + msec;

  MIOS32_LCD_Clear();
  MIOS32_LCD_PrintString(buffer);
}

mb_common_lcd_flash_state_e mb_common_lcd_flash_status(void)
{
    if (0 == mb_common_lcd_stop_flashing_at)
    {
        return MB_LCD_NOT_FLASHING;
    }

    if (MIOS32_TIMESTAMP_Get() > mb_common_lcd_stop_flashing_at)
    {
        mb_common_lcd_stop_flashing_at = 0;
        MIOS32_LCD_Clear();
        return MB_LCD_DONE_FLASHING;
    }

    return MB_LCD_FLASHING;
}
