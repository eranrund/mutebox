#include "mb_applet_md_mutes_setup.h"
#include "mb_applet_md_mutes.h"
#include "mb_mgr.h"

static u8 g_selected_sound;
static u8 g_selected_pot;
static u32 g_last_blinked_at;

static void mb_applet_md_mutes_setup_update_ui(void)
{
    u8 pattern;
    int i;
    pattern = mb_applet_md_mutes_state.pattern;

    MIOS32_LCD_Clear();
    MIOS32_LCD_PrintFormattedString("%c%02d            SETUP", 'A' + (pattern / 16), 1 + (pattern % 16));
    MIOS32_LCD_CursorSet(0,1);
    MIOS32_LCD_PrintFormattedString(
        "%s %s  %s",
        md_sound_names[g_selected_sound],
        g_selected_pot ? "TOP" : "BOT",
        md_param_name(g_selected_pot ? 
            mb_applet_md_mutes_state.cur_patch.pots[g_selected_sound].param1 :
            mb_applet_md_mutes_state.cur_patch.pots[g_selected_sound].param2
        )
    );

    for (i = 0; i < 16; ++i)
    {
       MIOS32_DOUT_PinSet(i, 0);
    }

    MIOS32_DOUT_PinSet(g_selected_sound, 1);
    g_last_blinked_at = MIOS32_TIMESTAMP_Get();
}

static void mb_applet_md_mutes_setup_init(void)
{    
    g_selected_sound = 0;
    g_selected_pot = 1;

    mb_applet_md_mutes_setup_update_ui();
}

static void mb_applet_md_mutes_setup_background(void)
{
    if (MIOS32_TIMESTAMP_GetDelay(g_last_blinked_at) >= 500)
    {
        g_last_blinked_at = MIOS32_TIMESTAMP_Get();
        MIOS32_DOUT_PinSet(g_selected_sound, !MIOS32_DOUT_PinGet(g_selected_sound));
    }
}

static void mb_applet_md_mutes_setup_ui_btn_toggle(u32 btn, u32 val)
{
    if (val == 1)
    {
        if (g_selected_sound == btn)
        {
            g_selected_pot = g_selected_pot ? 0 : 1;
        }
        else
        {
            g_selected_sound = btn;
            g_selected_pot = 1;
        }
        mb_applet_md_mutes_setup_update_ui();
    }
}

static void mb_applet_md_mutes_setup_ui_pot_change(u32 pot, u32 val)
{
    // TODO
    /*
    if ((pot != g_selected_sound) || (g_selected_pot != 1))
    {
        g_selected_sound = pot;
        g_selected_pot = 1;
        mb_applet_md_mutes_setup_update_ui();
    }
    */
}

static void mb_applet_md_mutes_setup_scs_btn_toggle(u32 btn, u32 val)
{
    u8 * p;

    if (val != 1)
    {
        return;
    }

    switch (btn)
    {
        case 0:
            p = g_selected_pot ? 
                &(mb_applet_md_mutes_state.cur_patch.pots[g_selected_sound].param1) :
                &(mb_applet_md_mutes_state.cur_patch.pots[g_selected_sound].param2);
            *p = md_get_next_param(*p);
            mb_applet_md_mutes_setup_update_ui();
        break;

        case 1:
            p = g_selected_pot ? 
                &(mb_applet_md_mutes_state.cur_patch.pots[g_selected_sound].param1) :
                &(mb_applet_md_mutes_state.cur_patch.pots[g_selected_sound].param2);
            *p = md_get_prev_param(*p);
            mb_applet_md_mutes_setup_update_ui();
        break;

        case 5:
            mb_mgr_start(&mb_applet_md_mutes);
    }
}

const mb_applet_t mb_applet_md_mutes_setup =
{
    .name           = "MD MUTES SETUP",
    .init           = mb_applet_md_mutes_setup_init,
    .background     = mb_applet_md_mutes_setup_background,
    .ui_btn_toggle  = mb_applet_md_mutes_setup_ui_btn_toggle,
    .ui_pot_change  = mb_applet_md_mutes_setup_ui_pot_change,
    .scs_btn_toggle = mb_applet_md_mutes_setup_scs_btn_toggle,
};
