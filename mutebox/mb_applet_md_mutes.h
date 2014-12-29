#ifndef __mb_applet_md_mutes__
#define __mb_applet_md_mutes__

#include "mb_applet.h"

typedef enum {
    MD_PARAM_INVALID = 0,
    MD_PARAM_LEVEL = 8,
    MD_PARAM_MUTE = 12,
    MD_PARAM_MACHINE_PARAM1 = 16,
    MD_PARAM_MACHINE_PARAM2 = 17,
    MD_PARAM_MACHINE_PARAM3 = 18,
    MD_PARAM_MACHINE_PARAM4 = 19,
    MD_PARAM_MACHINE_PARAM5 = 20,
    MD_PARAM_MACHINE_PARAM6 = 21,
    MD_PARAM_MACHINE_PARAM7 = 22,
    MD_PARAM_MACHINE_PARAM8 = 23,
    MD_PARAM_AM_DEPTH = 24,
    MD_PARAM_AM_RATE = 25,
    MD_PARAM_EQ_FREQ = 26,
    MD_PARAM_EQ_GAIN = 27,
    MD_PARAM_FILTER_BASE_FREQ = 28,
    MD_PARAM_FILTER_WIDTH = 29,
    MD_PARAM_FILTER_Q = 30,
    MD_PARAM_SR = 31,
    MD_PARAM_DIST = 32,
    MD_PARAM_VOL = 33,
    MD_PARAM_PAN = 34,
    MD_PARAM_DELAY_SEND = 35,
    MD_PARAM_REVERB_SEND = 36,
    MD_PARAM_LFO_SPEED = 37,
    MD_PARAM_LFO_AMOUNT = 38,
    MD_PARAM_LFO_SHAPE = 39,
} mb_applet_md_mutes_param_e;


typedef struct {
    struct {
        mb_applet_md_mutes_param_e param1;
        mb_applet_md_mutes_param_e param2;
    } pots[16];
} mb_applet_md_mutes_patch;

extern const mb_applet_t mb_applet_md_mutes;

#endif
