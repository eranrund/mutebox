#include "mb_applet_md_mutes.h"

const char * const md_sound_names[] = {
    "BD", "SD", "HT", "MT", "LT", "CP", "RS", "CB",
    "CH", "OH", "RC", "CC", "M1", "M2", "M3", "M4", 
};

static const mb_applet_md_mutes_patch default_patch =
{
    .pots = {
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
        {.param1 = MD_PARAM_MACHINE_PARAM1, .param2 = MD_PARAM_MACHINE_PARAM2},
    }
};

static u16                              g_mutes;
static mios32_midi_chn_t                g_base_chan;
static mios32_midi_port_t               g_port;
static const mb_applet_md_mutes_patch * g_patch;

// LCD ddisplay stuff
static mb_applet_md_mutes_param_e g_lcd_last_changed_param = MD_PARAM_INVALID;
static u32                        g_lcd_last_changed_param_at = 0;
static u32                        g_lcd_last_changed_param_val = 0;
static u8                         g_lcd_last_changed_param_sound = 0;


static const char * md_param_name(mb_applet_md_mutes_param_e param)
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

static void mb_applet_md_mutes_send_cc(u8 sound, u8 param, u8 val)
{
    MIOS32_MIDI_SendDebugMessage("MD CC S%d %s: %d\n", sound, md_param_name(param), val);
                
    switch (param) {
        case MD_PARAM_LEVEL:
        case MD_PARAM_MUTE:
            MIOS32_MIDI_SendCC(g_port, g_base_chan + (sound / 4), param + (sound % 4), val);
            break;

        default:
            MIOS32_MIDI_SendCC(g_port, g_base_chan + (sound / 4), param + ((sound % 4)* 24), val);
            break;
    }
}

static void mb_applet_md_mutes_sync_hw(void)
{
    int i;

    for (i = 0; i < 16; ++i)
    {
        mb_applet_md_mutes_send_cc(i, MD_PARAM_MUTE, (g_mutes & (1 << i)) ? 1 : 0);
        MIOS32_DOUT_PinSet(i, (g_mutes & (1 << i)) ? 0 : 1);
    }
}

static void mb_applet_md_mutes_init(void)
{
    g_base_chan = Chn3;
    g_port      = UART0;
    g_mutes     = 0;
    g_patch     = &default_patch;

    mb_applet_md_mutes_sync_hw();
}

static void mb_applet_md_mutes_background(void)
{
    u32 t;
    
    t = MIOS32_TIMESTAMP_Get();

    if (g_lcd_last_changed_param != MD_PARAM_INVALID)
    {
        if (0 == g_lcd_last_changed_param_at)
        {
            g_lcd_last_changed_param_at = t;
            MIOS32_LCD_Clear();
            MIOS32_LCD_PrintFormattedString("%s", md_sound_names[g_lcd_last_changed_param_sound]);
            MIOS32_LCD_CursorSet(0,1);
            MIOS32_LCD_PrintFormattedString("%s %03d", md_param_name(g_lcd_last_changed_param), g_lcd_last_changed_param_val);
        }
        else if (MIOS32_TIMESTAMP_GetDelay(g_lcd_last_changed_param_at ) >= 500)
        {
            MIOS32_LCD_Clear();
            g_lcd_last_changed_param = MD_PARAM_INVALID;
            g_lcd_last_changed_param_at = 0;
        }

        return;
    }

    {
        mios32_sys_time_t t = MIOS32_SYS_TimeGet();
        static u32 last_s = 0;
        
        if ((t.seconds - last_s) > 0)
        {
            last_s = t.seconds;

            MIOS32_LCD_CursorSet(0, 1); // X, Y
            int hours = t.seconds / 3600;
            int minutes = (t.seconds % 3600) / 60;
            int seconds = (t.seconds % 3600) % 60;
            int milliseconds = t.fraction_ms;
            MIOS32_LCD_PrintFormattedString("%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
        }
    }
}

static void mb_applet_md_mutes_ui_btn_toggle(u32 btn, u32 val)
{
    // If pressed, toggle mutes
    if (val == 1)
    {
        g_mutes ^= (1 << btn);
        mb_applet_md_mutes_send_cc(btn, MD_PARAM_MUTE, (g_mutes & (1 << btn)) ? 1 : 0);
        MIOS32_DOUT_PinSet(btn, (g_mutes & (1 << btn)) ? 0 : 1);

        g_lcd_last_changed_param       = MD_PARAM_MUTE;
        g_lcd_last_changed_param_at    = 0;
        g_lcd_last_changed_param_val   = (g_mutes & (1 << btn)) ? 0 : 1;
        g_lcd_last_changed_param_sound = btn;

    }
}

static void mb_applet_md_mutes_ui_pot_change(u32 pot, u32 val)
{
    mb_applet_md_mutes_param_e p;

    val >>= 5;
    val = 127 - val;
    pot -= 1;

    if (pot < 8)
    {
        p = g_patch->pots[pot].param1;
    }
    else
    {
        pot -= 8;
        p = g_patch->pots[pot].param2;
    }

    mb_applet_md_mutes_send_cc(pot, p, val);
    
    g_lcd_last_changed_param       = p;
    g_lcd_last_changed_param_at    = 0;
    g_lcd_last_changed_param_val   = val;
    g_lcd_last_changed_param_sound = pot;
}

const mb_applet_t mb_applet_md_mutes =
{
    .name          = "MD MUTES",
    .init          = mb_applet_md_mutes_init,
    .background    = mb_applet_md_mutes_background,
    .ui_btn_toggle = mb_applet_md_mutes_ui_btn_toggle,
    .ui_pot_change = mb_applet_md_mutes_ui_pot_change,
};
