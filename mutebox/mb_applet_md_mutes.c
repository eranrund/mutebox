#include "mb_applet_md_mutes.h"

static void mb_applet_md_mutes_ui_btn_toggle(u32 btn, u32 val)
{
}

static void mb_applet_md_mutes_ui_pot_change(u32 pot, u32 val)
{
}

const mb_applet_t mb_applet_md_mutes =
{
    .name          = "MD MUTES",
    .ui_btn_toggle = mb_applet_md_mutes_ui_btn_toggle,
    .ui_pot_change = mb_applet_md_mutes_ui_pot_change,
};
