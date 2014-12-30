#ifndef __MB_MGR_H__
#define __MB_MGR_H__ 1

#include <mios32.h>
#include "mb_applet.h"

extern const mb_applet_t * mb_mgr_cur_applet;

void mb_mgr_init(void);
void mb_mgr_start(const mb_applet_t * applet);

void mb_mgr_background(void);

void mb_mgr_notify_ui_btn_toggle(u32 btn, u32 val);
void mb_mgr_notify_ui_pot_change(u32 pot, u32 val);
void mb_mgr_notify_scs_btn_toggle(u32 btn, u32 val);

void mb_mgr_notify_seq_stop(void);
void mb_mgr_notify_seq_cont(void);
void mb_mgr_notify_seq_start(void);
void mb_mgr_notify_seq_new_song_pos(u32 new_song_pos);
void mb_mgr_notify_seq_tick(u32 bpm_tick);


#endif

