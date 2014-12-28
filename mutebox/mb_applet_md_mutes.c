#include "mb_applet_md_mutes.h"

static u16                g_mutes;
static mios32_midi_chn_t  g_base_chan;
static mios32_midi_port_t g_port;


static void mb_applet_md_mutes_sync_hw(void)
{
    int i;

    for (i = 0; i < 16; ++i)
    {
        MIOS32_MIDI_SendCC(g_port, g_base_chan + (i / 4), 12 + (i % 4), (g_mutes & (1 << i)) ? 1 : 0);
        MIOS32_DOUT_PinSet(i, (g_mutes & (1 << i)) ? 0 : 1);
    }
}

static void mb_applet_md_mutes_init(void)
{
    g_base_chan = Chn3;
    g_port      = UART0;
    g_mutes     = 0;

    mb_applet_md_mutes_sync_hw();
}

static void mb_applet_md_mutes_ui_btn_toggle(u32 btn, u32 val)
{
    // If pressed, toggle mutes
    if (val == 1)
    {
        g_mutes ^= (1 << btn);
        MIOS32_MIDI_SendCC(g_port, g_base_chan + (btn / 4), 12 + (btn % 4), (g_mutes & (1 << btn)) ? 1 : 0);
        MIOS32_DOUT_PinSet(btn, (g_mutes & (1 << btn)) ? 0 : 1);
    }
}

static void mb_applet_md_mutes_ui_pot_change(u32 pot, u32 val)
{
}

const mb_applet_t mb_applet_md_mutes =
{
    .name          = "MD MUTES",
    .init          = mb_applet_md_mutes_init,
    .ui_btn_toggle = mb_applet_md_mutes_ui_btn_toggle,
    .ui_pot_change = mb_applet_md_mutes_ui_pot_change,
};
