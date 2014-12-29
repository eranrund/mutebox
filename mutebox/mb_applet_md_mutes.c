#include "mb_applet_md_mutes.h"
#include "mb_applet_md_mutes_setup.h"
#include "mb_mgr.h"

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
static u8                               g_pattern;

// LCD display stuff
static md_param_e g_lcd_last_changed_param = MD_PARAM_INVALID;
static u32        g_lcd_last_changed_param_at = 0;
static u32        g_lcd_last_changed_param_val = 0;
static u8         g_lcd_last_changed_param_sound = 0;
static u8         g_lcd_force_update = 0;


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

    MIOS32_MIDI_SendProgramChange(g_port, g_base_chan, g_pattern);
}

static void mb_applet_md_mutes_init(void)
{
    static u8 first_time = 1;

    if (first_time)
    {
        g_base_chan = Chn3;
        g_port      = UART0;
        g_mutes     = 0;
        g_patch     = &default_patch;
        g_pattern   = 0;

        mb_applet_md_mutes_sync_hw();

        first_time = 0;
    }

    g_lcd_last_changed_param = MD_PARAM_INVALID;
    g_lcd_last_changed_param_at = 0;
    g_lcd_last_changed_param_val = 0;
    g_lcd_last_changed_param_sound = 0;
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
        
        if (((t.seconds - last_s) > 0) || g_lcd_force_update)
        {
            last_s = t.seconds;
            g_lcd_force_update = 0;

            MIOS32_LCD_CursorSet(0, 0);
            MIOS32_LCD_PrintFormattedString("%c%02d", 'A' + (g_pattern / 16), 1 + (g_pattern % 16));

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
    md_param_e p;

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

static void mb_applet_md_mutes_scs_btn_toggle(u32 btn, u32 val)
{
    MIOS32_MIDI_SendDebugMessage(" >> SCS BTN %d %d\n", btn, val);
    if (val != 1)
    {
        return;
    }

    switch (btn)
    {
        case 0:
            g_pattern = (g_pattern + 1) % 128;
            g_lcd_force_update = 1;
            MIOS32_MIDI_SendProgramChange(g_port, g_base_chan, g_pattern);
            break;

        case 1:
            g_pattern = (g_pattern == 0) ? 127 : (g_pattern - 1);
            g_lcd_force_update = 1;
            MIOS32_MIDI_SendProgramChange(g_port, g_base_chan, g_pattern);
            break;


        case 5:
            mb_mgr_start(&mb_applet_md_mutes_setup);
            break;
    }
}

const mb_applet_t mb_applet_md_mutes =
{
    .name           = "MD MUTES",
    .init           = mb_applet_md_mutes_init,
    .background     = mb_applet_md_mutes_background,
    .ui_btn_toggle  = mb_applet_md_mutes_ui_btn_toggle,
    .ui_pot_change  = mb_applet_md_mutes_ui_pot_change,
    .scs_btn_toggle = mb_applet_md_mutes_scs_btn_toggle,
};
