#include "md.h"

const char * const md_sound_names[] = {
    "BD", "SD", "HT", "MT", "LT", "CP", "RS", "CB",
    "CH", "OH", "RC", "CC", "M1", "M2", "M3", "M4", 
};

const char * md_param_name(md_param_e param)
{
    switch (param)
    {
        case MD_PARAM_LEVEL:
            return "LVL";

        case MD_PARAM_MUTE:
            return "MUTE";

        case MD_PARAM_MACHINE_PARAM1:
            return "P1";

        case MD_PARAM_MACHINE_PARAM2:
            return "P2";
            
        case MD_PARAM_MACHINE_PARAM3:
            return "P3";

        case MD_PARAM_MACHINE_PARAM4:
            return "P4";

        case MD_PARAM_MACHINE_PARAM5:
            return "P5";

        case MD_PARAM_MACHINE_PARAM6:
            return "P6";

        case MD_PARAM_MACHINE_PARAM7:
            return "P7";

        case MD_PARAM_MACHINE_PARAM8:
            return "P8";

        case MD_PARAM_AM_DEPTH:
            return "AMD";

        case MD_PARAM_AM_RATE:
            return "AMR";

        case MD_PARAM_EQ_FREQ:
            return "EQF";

        case MD_PARAM_EQ_GAIN:
            return "EQG";

        case MD_PARAM_FILTER_BASE_FREQ:
            return "FBF";

        case MD_PARAM_FILTER_WIDTH:
            return "FW";

        case MD_PARAM_FILTER_Q:
            return "FQ";

        case MD_PARAM_SR:
            return "SR";

        case MD_PARAM_DIST:
            return "DIST";

        case MD_PARAM_VOL:
            return "VOL";

        case MD_PARAM_PAN:
            return "PAN";

        case MD_PARAM_DELAY_SEND:
            return "DLY";

        case MD_PARAM_REVERB_SEND:
            return "RVR";

        case MD_PARAM_LFO_SPEED:
            return "LFOS";

        case MD_PARAM_LFO_AMOUNT:
            return "LFOA";

        case MD_PARAM_LFO_SHAPE:
            return "LFOH";

        default:
            return "???";

    }
}


md_param_e md_get_next_param(md_param_e p)
{
    switch (p) {
        case MD_PARAM_INVALID:
            return MD_PARAM_LEVEL;

        case MD_PARAM_LEVEL:
            return MD_PARAM_MUTE;

        case MD_PARAM_MUTE:
            return MD_PARAM_MACHINE_PARAM1;

        case MD_PARAM_LFO_SHAPE:
            return MD_PARAM_LEVEL;

        default:
            return p + 1;
    }
}
md_param_e md_get_prev_param(md_param_e p)
{
    switch (p) {
        case MD_PARAM_INVALID:
            return MD_PARAM_LFO_SHAPE;

        case MD_PARAM_LEVEL:
            return MD_PARAM_LFO_SHAPE;

        case MD_PARAM_MUTE:
            return MD_PARAM_LEVEL;

        case MD_PARAM_MACHINE_PARAM1:
            return MD_PARAM_MUTE;

        default:
            return p - 1;
    }
}

