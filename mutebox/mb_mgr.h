#ifndef __MB_MGR_H__
#define __MB_MGR_H__ 1

#include <mios32.h>
#include "mb_applet.h"

void mb_mgr_init(void);
void mb_mgr_start(const mb_applet_t * applet);
void mb_mgr_notify_ui_btn_toggle(u32 btn, u32 val);
void mb_mgr_notify_ui_pot_change(u32 pot, u32 val);
void mb_mgr_background(void);

#endif

