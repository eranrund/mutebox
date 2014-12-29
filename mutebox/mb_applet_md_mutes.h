#ifndef __mb_applet_md_mutes__
#define __mb_applet_md_mutes__

#include "mb_applet.h"
#include "md.h"

typedef struct {
    struct {
        md_param_e param1;
        md_param_e param2;
    } pots[16];
} mb_applet_md_mutes_patch;

extern const mb_applet_t mb_applet_md_mutes;

#endif
