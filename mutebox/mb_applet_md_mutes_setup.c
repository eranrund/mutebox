#include "mb_applet_md_mutes_setup.h"
#include "mb_applet_md_mutes.h"
#include "mb_mgr.h"

static void mb_applet_md_mutes_setup_init(void)
{
    int i;

    MIOS32_LCD_PrintFormattedString(mb_mgr_cur_applet->name);

    for (i = 0; i < 16; ++i)
    {
       MIOS32_DOUT_PinSet(i, 0);
    }
}

static void mb_applet_md_mutes_setup_background(void)
{

}

static void mb_applet_md_mutes_setup_ui_btn_toggle(u32 btn, u32 val)
{

}

static void mb_applet_md_mutes_setup_ui_pot_change(u32 pot, u32 val)
{

}

static void mb_applet_md_mutes_setup_scs_btn_toggle(u32 btn, u32 val)
{
    if (val != 1)
    {
        return;
    }

    switch (btn)
    {
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
