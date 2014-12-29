#ifndef __mb_applet_md_mutes__
#define __mb_applet_md_mutes__

#include "mb_applet.h"
#include "md.h"

#define MB_APPLET_MD_MUTES_PATCH_SIZE 64

#define MB_APPLET_MD_MUTES_PATCH_FLAG_USED      1
#define MB_APPLET_MD_MUTES_PATCH_FLAG_HAS_MUTES 2

#pragma pack(1)
typedef struct {
    struct {
        /*md_param_e*/ u8 param1;
        /*md_param_e*/ u8 param2;
    } pots[16];

    u16 mutes;
    u16 flags;

    u8 reserved[MB_APPLET_MD_MUTES_PATCH_SIZE - 36];
} mb_applet_md_mutes_patch_t;

#pragma pack()

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
