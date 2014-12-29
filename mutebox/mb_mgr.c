#include "mb_mgr.h"
#include "mb_applet.h"

const mb_applet_t * mb_mgr_cur_applet;

void mb_mgr_init(void)
{
}

void mb_mgr_start(const mb_applet_t * applet)
{
    mb_mgr_cur_applet = applet;
    MIOS32_MIDI_SendDebugMessage("APPLET START %s\n", applet->name);

    MIOS32_LCD_Clear();
    MIOS32_LCD_CursorSet(0, 0);

    if (mb_mgr_cur_applet->init)
    {
        mb_mgr_cur_applet->init();
    }
}

void mb_mgr_notify_ui_btn_toggle(u32 btn, u32 val)
{
    MIOS32_MIDI_SendDebugMessage("UI BTN %d %d\n", btn, val);
    if (mb_mgr_cur_applet->ui_btn_toggle)
    {
        mb_mgr_cur_applet->ui_btn_toggle(btn, val);
    }
}

void mb_mgr_notify_ui_pot_change(u32 pot, u32 val)
{
    if (pot != 1) return; // TODO 
    MIOS32_MIDI_SendDebugMessage("UI POT %d %d\n", pot, val);
    if (mb_mgr_cur_applet->ui_pot_change)
    {
        mb_mgr_cur_applet->ui_pot_change(pot, val);
    }
}

void mb_mgr_background(void)
{
    if (mb_mgr_cur_applet->background)
    {
        mb_mgr_cur_applet->background();
    }
}

void mb_mgr_notify_scs_btn_toggle(u32 btn, u32 val)
{
    MIOS32_MIDI_SendDebugMessage("SCS BTN %d %d\n", btn, val);
    if (mb_mgr_cur_applet->scs_btn_toggle)
    {
        mb_mgr_cur_applet->scs_btn_toggle(btn, val);
    }
}

