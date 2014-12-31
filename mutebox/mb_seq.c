#include <mios32.h>
#include <seq_bpm.h>
#include <seq_midi_out.h>
#include "mb_common.h"
#include "mb_seq.h"
#include "mb_mgr.h"


static u8 seq_step_pos;
static u8 seq_step_measure;
static u8 seq_pause = 0;


void mb_seq_play_off_events(void)
{
    SEQ_MIDI_OUT_FlushQueue();
}

void mb_seq_reset(void)
{
  // since timebase has been changed, ensure that Off-Events are played 
  // (otherwise they will be played much later...)
  mb_seq_play_off_events();

  // release pause mode
  seq_pause = 0;

  // reset BPM tick
  SEQ_BPM_TickSet(0);
}

void mb_seq_init(void)
{
    mb_seq_reset();
    SEQ_BPM_Init(0);
    SEQ_BPM_PPQN_Set(384);
    SEQ_BPM_Set(142.0);
}

void mb_seq_start(void)
{
    SEQ_BPM_Start();
}

void mb_seq_stop(void)
{
    SEQ_BPM_Stop();
}

u8 mb_seq_get_step_pos(void)
{
    return seq_step_pos;
}

u8 mb_seq_get_step_measure(void)
{
    return seq_step_measure;
}

mb_seq_state_e mb_seq_get_state(void)
{
    if (SEQ_BPM_IsRunning())
    {
        return MB_SEQ_RUNNING;
    }

    return MB_SEQ_STOPPED;
}

void mb_seq_send_midi_clk_event(u8 evnt0, u32 bpm_tick)
{
    int i;
    mios32_midi_package_t p;
    mios32_midi_port_t port;

    p.ALL = 0;
    p.type = 0x5; // Single-byte system common message
    p.evnt0 = evnt0;

    port = UART0;
    if (MIOS32_MIDI_CheckAvailable(port))
    {
        if (bpm_tick)
        {
            SEQ_MIDI_OUT_Send(port, p, SEQ_MIDI_OUT_ClkEvent, bpm_tick, 0);
        }
        else
        {
            MUTEX_MIDIOUT_TAKE;
            MIOS32_MIDI_SendPackage(port, p);
            MUTEX_MIDIOUT_GIVE;
        }
    }
     
#if 0
    u32 port_mask = 0x00000001;
    for(i=0; i<32; ++i, port_mask<<=1) {
    if( seq_midi_router_mclk_out & port_mask & 0xffffff0f ) { // filter USB5..USB8 to avoid unwanted clock events to non-existent ports
      // coding: USB0..7, UART0..7, IIC0..7, OSC0..7
      mios32_midi_port_t port = (USB0 + ((i&0x18) << 1)) | (i&7);

      // TODO: special check for OSC, since MIOS32_MIDI_CheckAvailable() won't work here
      if( MIOS32_MIDI_CheckAvailable(port) ) {
    if( bpm_tick )
      SEQ_MIDI_OUT_Send(port, p, SEQ_MIDI_OUT_ClkEvent, bpm_tick, 0);
    else {
      MUTEX_MIDIOUT_TAKE;
      MIOS32_MIDI_SendPackage(port, p);
      MUTEX_MIDIOUT_GIVE;
    }
      }
    }
    }
#endif
}


static void mb_seq_tick(u32 bpm_tick)
{
    if ((bpm_tick % 16) == 0)
    {
        mb_seq_send_midi_clk_event(0xf8, bpm_tick);
    }

    // whenever we reach a new 16th note (96 ticks @384 ppqn):
    if ((bpm_tick % (SEQ_BPM_PPQN_Get()/4)) == 0)
    {
        // ensure that step number will be reseted on first bpm_tick
        if (bpm_tick == 0)
        {
            seq_step_pos = 0;
            seq_step_measure = 0;
        }
        else
        {
            // increment step number until it reaches 16
            if( ++seq_step_pos >= 16 )
            {
                seq_step_pos = 0;
                seq_step_measure++;
            }
        }

        /*if ((seq_step_pos % 4) == 0) {
            MIOS32_MIDI_SendDebugMessage("%d", seq_step_pos);
        }*/
    }
}


void mb_seq_handler(void)
{
    u8 num_loops = 0;
    u8 again = 0;
    do {
        ++num_loops;

        if( SEQ_BPM_ChkReqStop() ) {
            mb_seq_send_midi_clk_event(0xfc, 0); 
            mb_seq_play_off_events();
            mb_mgr_notify_seq_stop();
        }

        if( SEQ_BPM_ChkReqCont() ) {
            mb_seq_send_midi_clk_event(0xfb, 0); 
            seq_pause = 0;
            mb_mgr_notify_seq_cont();
        }

        if( SEQ_BPM_ChkReqStart() ) {
            mb_seq_send_midi_clk_event(0xfa, 0); 
            mb_seq_reset();
            mb_mgr_notify_seq_start();
        }

        u16 new_song_pos;
        if( SEQ_BPM_ChkReqSongPos(&new_song_pos) ) {
            mb_seq_play_off_events();
            mb_mgr_notify_seq_new_song_pos(new_song_pos);
        }

        u32 bpm_tick;
        if( SEQ_BPM_ChkReqClk(&bpm_tick) > 0 ) {
            again = 1;
            mb_seq_tick(bpm_tick);
            mb_mgr_notify_seq_tick(bpm_tick);
        }
    } while( again && num_loops < 10 );
}

