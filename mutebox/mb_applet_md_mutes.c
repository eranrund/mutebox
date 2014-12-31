#include <string.h>
#include <mios32.h>
#include <eeprom.h>

#include "mb_common.h"
#include "mb_applet_md_mutes.h"
#include "mb_applet_md_mutes_setup.h"
#include "mb_mgr.h"
#include "mb_seq.h"

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
    },
    .measures = 4,
};

// Global state
mb_applet_md_mutes_state_t mb_applet_md_mutes_state;
#define g_mutes (mb_applet_md_mutes_state.mutes)
#define g_base_chan (mb_applet_md_mutes_state.base_chan)
#define g_port (mb_applet_md_mutes_state.port)
#define g_cur_patch (mb_applet_md_mutes_state.cur_patch)
#define g_pattern (mb_applet_md_mutes_state.pattern)
#define g_next_pattern (mb_applet_md_mutes_state.next_pattern)
#define g_need_next_pattern (mb_applet_md_mutes_state.need_next_pattern)

// LCD display stuff
static md_param_e g_lcd_last_changed_param = MD_PARAM_INVALID;
static u32        g_lcd_last_changed_param_at = 0;
static u32        g_lcd_last_changed_param_val = 0;
static u8         g_lcd_last_changed_param_sound = 0;
static u8         g_lcd_force_update = 0;
static u32        g_lcd_last_updated_at = 0;

static u8         g_lcd_blink_state = 1;


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

static s32 mb_applet_md_mutes_patch_load(u8 num, mb_applet_md_mutes_patch_t * patch)
{
    s16 * buf;
    u32   i;
    u32   base;

    buf  = (s16 *) patch;
    base = num * sizeof(*patch);

    for (i = 0; i < sizeof(*patch); i += 2)
    {
        s32 val = EEPROM_Read(base + (i / 2));
        if (val < 0) {
            val = 0;
        }

        buf[i / 2] = val;
    }

    MIOS32_MIDI_SendDebugMessage("loading patch %d: %x\n", num, patch->flags);
    if ((patch->flags != 0xFFFF) && (patch->flags & MB_APPLET_MD_MUTES_PATCH_FLAG_USED))
    {
        mb_applet_md_mutes_patch_dump(patch);
        return 0;
    }

    return -1;
}

static s32 mb_applet_md_mutes_patch_store(u8 num, const mb_applet_md_mutes_patch_t * patch)
{
    s32 ret;
    int i;
    int base;
    const u16 * buf;

    if ((0 == patch->flags) ||
        (0xFFFF == patch->flags) ||
        !(patch->flags & MB_APPLET_MD_MUTES_PATCH_FLAG_USED))
    {
        return -1;
    }

    buf  = (const u16 *) patch;
    base = num * sizeof(mb_applet_md_mutes_patch_t);

    for (i = 0; i < sizeof(mb_applet_md_mutes_patch_t) / 2; i++)
    {
        ret = EEPROM_Write(base + i, buf[i]);
        if (0 > ret)
        {
            return ret;
        }
    }

    return 0;
}

static void mb_applet_md_mutes_queue_next_pattern(u8 pattern)
{
    g_next_pattern = pattern;
    g_lcd_force_update = 1;
    g_lcd_blink_state = 1;
    if (MB_SEQ_STOPPED == mb_seq_get_state())
    {
        g_need_next_pattern = 1;
    }
    MIOS32_MIDI_SendProgramChange(g_port, g_base_chan, g_next_pattern);
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
        g_next_pattern = 0;
        g_need_next_pattern = 1;
        
        memcpy(&g_cur_patch, &default_patch, sizeof(mb_applet_md_mutes_patch_t));


//        mb_applet_md_mutes_sync_hw();
//        mb_applet_md_mutes_patch_load(0);

        first_time = 0;
    }

    g_lcd_last_changed_param = MD_PARAM_INVALID;
    g_lcd_last_changed_param_at = 0;
    g_lcd_last_changed_param_val = 0;
    g_lcd_last_changed_param_sound = 0;
    g_lcd_last_updated_at = 0;
    g_lcd_force_update = 1;

    mb_applet_md_mutes_sync_hw();
}

static void mb_applet_md_mutes_uninit(void)
{
    mb_applet_md_mutes_queue_next_pattern(g_pattern);
}

static void mb_applet_md_seq_to_lcd(void)
{
    u8 step_pos = mb_seq_get_step_pos();
    char buf[] = "....";

    MIOS32_LCD_CursorSet(16, 0);
    if (mb_seq_get_state() == MB_SEQ_STOPPED)
    {
        MIOS32_LCD_PrintString("STOP");
    }
    else
    {
        buf[mb_seq_get_step_measure() % g_cur_patch.measures] = '1' + (step_pos / 4);
        MIOS32_LCD_PrintString(buf);
    }
}

static void mb_applet_md_mutes_background(void)
{
    u32 t = MIOS32_TIMESTAMP_Get();
    mb_applet_md_mutes_patch_t patch;

    /////////////////////////////// Next pattern
    if (g_need_next_pattern)
    {
        g_need_next_pattern = 0;
        g_pattern = g_next_pattern;

        if (0 == mb_applet_md_mutes_patch_load(g_pattern, &patch))
        {
            MIOS32_MIDI_SendDebugMessage("PATCH LOAD %d OK", g_pattern);
            memcpy(&g_cur_patch, &patch, sizeof(patch));

            if (g_cur_patch.flags & MB_APPLET_MD_MUTES_PATCH_FLAG_HAS_MUTES)
            {
                g_mutes = g_cur_patch.mutes;                
                mb_applet_md_mutes_sync_hw();
            }
        }
        else
        {
            MIOS32_MIDI_SendDebugMessage("PATCH LOAD %d ERR", g_pattern);
            g_cur_patch.flags &= ~(MB_APPLET_MD_MUTES_PATCH_FLAG_USED | MB_APPLET_MD_MUTES_PATCH_FLAG_HAS_MUTES);
        }
    }

    /////////////////////////////// LCD hell
    switch (mb_common_lcd_flash_status()) {
        case MB_LCD_NOT_FLASHING:
            break;
        case MB_LCD_FLASHING:
            return;
        case MB_LCD_DONE_FLASHING:
            g_lcd_force_update = 1;
            break;
    }

    /* if a param has been changed recently we will be displaying it's value */
    if (g_lcd_last_changed_param != MD_PARAM_INVALID)
    {
        if (0 == g_lcd_last_changed_param_at)
        {
            g_lcd_last_changed_param_at = t;
            MIOS32_LCD_Clear();
            mb_applet_md_seq_to_lcd();
            MIOS32_LCD_CursorSet(0,0);
            MIOS32_LCD_PrintFormattedString("%s", md_sound_names[g_lcd_last_changed_param_sound]);
            MIOS32_LCD_CursorSet(0,1);
            MIOS32_LCD_PrintFormattedString("%s %03d", md_param_name(g_lcd_last_changed_param), g_lcd_last_changed_param_val);
        }
        else if (MIOS32_TIMESTAMP_GetDelay(g_lcd_last_changed_param_at ) >= 500)
        {
            MIOS32_LCD_Clear();
            mb_applet_md_seq_to_lcd();
            g_lcd_last_changed_param = MD_PARAM_INVALID;
            g_lcd_last_changed_param_at = 0;
        }

        if (g_lcd_last_changed_param != MD_PARAM_INVALID) {
            return;
        }
    }

    /* regular status display */
    {
        u32 periodic_redraw = MIOS32_TIMESTAMP_GetDelay(g_lcd_last_updated_at) >= 500;
        if (periodic_redraw || g_lcd_force_update)
        {
            g_lcd_force_update = 0;
            if (periodic_redraw) {
                g_lcd_last_updated_at = t;
                g_lcd_blink_state = !g_lcd_blink_state;
            }

            MIOS32_LCD_CursorSet(0, 0);
            if (g_pattern == g_next_pattern)
            {
                /* no pattern change is queued */
                MIOS32_LCD_PrintFormattedString(
                    "%c%02d %c%c",
                    'A' + (g_pattern / 16), 1 + (g_pattern % 16),
                    (g_cur_patch.flags & MB_APPLET_MD_MUTES_PATCH_FLAG_USED) ? '+' : ' ',
                    (g_cur_patch.flags & MB_APPLET_MD_MUTES_PATCH_FLAG_HAS_MUTES) ? 'M' : ' '
                );
            }
            else
            {
                /* pattern change queud */
                if (g_lcd_blink_state)
                {
                    MIOS32_LCD_PrintFormattedString("%c%02d", 'A' + (g_next_pattern / 16), 1 + (g_next_pattern % 16));
                }
                else
                {
                    MIOS32_LCD_PrintString("   ");
                }
            }

            mb_applet_md_seq_to_lcd();

            MIOS32_LCD_CursorSet(0, 1); // X, Y
            mios32_sys_time_t tt = MIOS32_SYS_TimeGet();
            int hours = tt.seconds / 3600;
            int minutes = (tt.seconds % 3600) / 60;
            int seconds = (tt.seconds % 3600) % 60;
            int milliseconds = tt.fraction_ms;
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
    int r;

    //MIOS32_MIDI_SendDebugMessage(" >> SCS BTN %d %d\n", btn, val);
    if (val != 1)
    {
        return;
    }

    switch (btn)
    {
        case 0:
            mb_applet_md_mutes_queue_next_pattern((g_next_pattern + 1) & 0x7F);
            break;

        case 1:
            mb_applet_md_mutes_queue_next_pattern((g_next_pattern - 1) & 0x7F);
           break;

        case 2:
            mb_applet_md_mutes_patch_dump(&g_cur_patch);
            break;

        /* PLAY/STOP */
        case 3:
            switch (mb_seq_get_state()) {
                case MB_SEQ_STOPPED:
                    mb_seq_start();
                    break;

                case MB_SEQ_RUNNING:
                    mb_seq_stop();
                    break;
            }
            break;

        case 4:
            g_cur_patch.mutes = g_mutes;
            g_cur_patch.flags |= MB_APPLET_MD_MUTES_PATCH_FLAG_USED | MB_APPLET_MD_MUTES_PATCH_FLAG_HAS_MUTES;

            r = mb_applet_md_mutes_patch_store(g_pattern, &g_cur_patch);

            if (r == 0)
            {
                mb_common_lcd_flash_msg(1000, "%c%02d   SAVED", 'A' + (g_pattern / 16), 1 + (g_pattern % 16));
            }
            else
            {
                mb_common_lcd_flash_msg(1000, "%c%02d  ERR! %d", 'A' + (g_pattern / 16), 1 + (g_pattern % 16), r);
            }

            break;

        /* SETUP */
        case 5:
            mb_mgr_start(&mb_applet_md_mutes_setup);
            break;
    }
}

static void mb_applet_md_mutes_seq_tick(u32 bpm_tick)
{
    u8 step_pos = mb_seq_get_step_pos();

    if ((step_pos % 4) == 0)
    {
        g_lcd_force_update = 1;
    }

    if ((0 == step_pos) &&
        (0 == (mb_seq_get_step_measure() % g_cur_patch.measures)) &&
        (g_pattern != g_next_pattern))
    {
        g_lcd_force_update = 1;
        g_need_next_pattern = 1;
    }
}

static void mb_applet_md_mutes_seq_stop(void)
{
    mb_applet_md_mutes_queue_next_pattern(g_pattern);
}

const mb_applet_t mb_applet_md_mutes =
{
    .name           = "MD MUTES",
    .init           = mb_applet_md_mutes_init,
    .uninit         = mb_applet_md_mutes_uninit,
    .background     = mb_applet_md_mutes_background,

    .ui_btn_toggle  = mb_applet_md_mutes_ui_btn_toggle,
    .ui_pot_change  = mb_applet_md_mutes_ui_pot_change,
    .scs_btn_toggle = mb_applet_md_mutes_scs_btn_toggle,

    .seq_tick = mb_applet_md_mutes_seq_tick,
    .seq_stop = mb_applet_md_mutes_seq_stop,
};
