#ifndef __MB_APPLET_H__
#define __MB_APPLET_H__ 1

#include <mios32.h>


typedef struct {
    const char * name;
    void (*ui_btn_toggle)(u32 btn, u32 val);
    void (*ui_pot_change)(u32 pot, u32 val);

} mb_applet_t;


#endif

