#ifndef __mb_applet_md_mutes__
#define __mb_applet_md_mutes__

#include "mb_applet.h"
#include "md.h"

typedef struct {
    struct {
        md_param_e param1;
        md_param_e param2;
    } pots[16];
} mb_applet_md_mutes_patch_t;

typedef struct {
    u16                        mutes;
    mios32_midi_chn_t          base_chan;
    mios32_midi_port_t         port;
    mb_applet_md_mutes_patch_t cur_patch;
    u8                         pattern;
} mb_applet_md_mutes_state_t;


extern const mb_applet_t mb_applet_md_mutes;
extern mb_applet_md_mutes_state_t mb_applet_md_mutes_state;

#endif
