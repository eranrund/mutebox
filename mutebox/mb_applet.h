#ifndef __MB_APPLET_H__
#define __MB_APPLET_H__ 1

#include <mios32.h>


typedef struct {
    const char * name;
    void (*init)(void);
    void (*background)(void);

    void (*ui_btn_toggle)(u32 btn, u32 val);
    void (*ui_pot_change)(u32 pot, u32 val);
    void (*scs_btn_toggle)(u32 btn, u32 val);

    void (*seq_stop)(void);
    void (*seq_cont)(void);
    void (*seq_start)(void);
    void (*seq_new_song_pos)(u32 new_song_pos);
    void (*seq_tick)(u32 bpm_tick);

} mb_applet_t;


#endif

