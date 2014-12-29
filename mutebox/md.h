#ifndef __MD__
#define __MD__

#include <mios32.h>

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
} md_param_e;

extern const char * const md_sound_names[];
const char * md_param_name(md_param_e param);
md_param_e md_get_next_param(md_param_e p);
md_param_e md_get_prev_param(md_param_e p);

#endif
