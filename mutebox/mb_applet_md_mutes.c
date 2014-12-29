#include <string.h>
#include <mios32.h>
#include <eeprom.h>

#include "mb_applet_md_mutes.h"
#include "mb_applet_md_mutes_setup.h"
#include "mb_mgr.h"

static const mb_applet_md_mutes_patch_t default_patch =
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

// Global state
mb_applet_md_mutes_state_t mb_applet_md_mutes_state;
#define g_mutes (mb_applet_md_mutes_state.mutes)
#define g_base_chan (mb_applet_md_mutes_state.base_chan)
#define g_port (mb_applet_md_mutes_state.port)
#define g_cur_patch (mb_applet_md_mutes_state.cur_patch)
#define g_pattern (mb_applet_md_mutes_state.pattern)

// LCD display stuff
static md_param_e g_lcd_last_changed_param = MD_PARAM_INVALID;
static u32        g_lcd_last_changed_param_at = 0;
static u32        g_lcd_last_changed_param_val = 0;
static u8         g_lcd_last_changed_param_sound = 0;
static u8         g_lcd_force_update = 0;
static u32        g_lcd_last_updated_at = 0;


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

static void mb_applet_md_mutes_patch_dump(mb_applet_md_mutes_patch_t * patch)
{
    MIOS32_MIDI_SendDebugMessage("flags: 0x%X mutes: 0x%X\n", patch->flags, patch->mutes);

    MIOS32_MIDI_SendDebugMessage(
        "%-6s %-6s %-6s %-6s %-6s %-6s %-6s %-6s\n",
        md_sound_names[0],
        md_sound_names[1],
        md_sound_names[2],
        md_sound_names[3],
        md_sound_names[4],
        md_sound_names[5],
        md_sound_names[6],
        md_sound_names[7]
    );

    MIOS32_MIDI_SendDebugMessage(
        "%-6s %-6s %-6s %-6s %-6s %-6s %-6s %-6s\n",
        md_param_name(patch->pots[0].param1),
        md_param_name(patch->pots[1].param1),
        md_param_name(patch->pots[2].param1),
        md_param_name(patch->pots[3].param1),
        md_param_name(patch->pots[4].param1),
        md_param_name(patch->pots[5].param1),
        md_param_name(patch->pots[6].param1),
        md_param_name(patch->pots[7].param1)
    );
    MIOS32_MIDI_SendDebugMessage(
        "%-6s %-6s %-6s %-6s %-6s %-6s %-6s %-6s\n",
        md_param_name(patch->pots[0].param2),
        md_param_name(patch->pots[1].param2),
        md_param_name(patch->pots[2].param2),
        md_param_name(patch->pots[3].param2),
        md_param_name(patch->pots[4].param2),
        md_param_name(patch->pots[5].param2),
        md_param_name(patch->pots[6].param2),
        md_param_name(patch->pots[7].param2)
    );
}



static void mb_applet_md_mutes_patch_load(u8 patch_num)
{
    mb_applet_md_mutes_patch_t patch;
    s16 *                      buf;
    u32                        i;
    u32                        base;

    // TODO

    buf  = (s16 *) &patch;
    base = patch_num * sizeof(patch);
    return;


    for (i = 0; i < sizeof(patch); i += 2)
    {
        s32 val = EEPROM_Read(base + (i / 2));
        buf[i / 2] = val;
    }

    MIOS32_MIDI_SendDebugMessage("loading patch %d\n", patch_num);
    if ((patch.flags != 0xFFFF) && (patch.flags & MB_APPLET_MD_MUTES_PATCH_FLAG_USED))
    {
        mb_applet_md_mutes_patch_dump(&patch);

        memcpy(&(g_cur_patch.pots), &(patch.pots), sizeof(patch.pots));

        if (patch.flags & MB_APPLET_MD_MUTES_PATCH_FLAG_HAS_MUTES)
        {
            g_mutes = patch.mutes;
            mb_applet_md_mutes_sync_hw();
        }
    }
    else
    {
        MIOS32_MIDI_SendDebugMessage("patch unused\n");
    }
}

static void mb_applet_md_mutes_init(void)
{
    static u8 first_time = 1;

    #define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
    BUILD_BUG_ON(sizeof(mb_applet_md_mutes_patch_t) != MB_APPLET_MD_MUTES_PATCH_SIZE);

    if (first_time)
    {
        g_base_chan = Chn3;
        g_port      = UART0;
        g_mutes     = 0;
        g_pattern   = 0;
        
        memcpy(&g_cur_patch, &default_patch, sizeof(mb_applet_md_mutes_patch_t));

        mb_applet_md_mutes_sync_hw();
//        mb_applet_md_mutes_patch_load(0);

        first_time = 0;
    }

    g_lcd_last_changed_param = MD_PARAM_INVALID;
    g_lcd_last_changed_param_at = 0;
    g_lcd_last_changed_param_val = 0;
    g_lcd_last_changed_param_sound = 0;
    g_lcd_last_updated_at = 0;

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
        
        if (((t.seconds - g_lcd_last_updated_at) > 0) || g_lcd_force_update)
        {
            g_lcd_last_updated_at = t.seconds;
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
        p = g_cur_patch.pots[pot].param1;
    }
    else
    {
        pot -= 8;
        p = g_cur_patch.pots[pot].param2;
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

        case 2:
            mb_applet_md_mutes_patch_dump(&g_cur_patch);
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
