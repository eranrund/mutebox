#ifndef __MB_SEQ_H__
#define __MB_SEQ_H__

#include <mios32.h>

typedef enum {
    MB_SEQ_STOPPED = 0,
    MB_SEQ_RUNNING,
} mb_seq_state_e;

void mb_seq_init(void);
void mb_seq_handler(void);

void mb_seq_start(void);
void mb_seq_stop(void);
u8 mb_seq_get_step_pos(void);
u8 mb_seq_get_step_measure(void);
mb_seq_state_e mb_seq_get_state(void);
void mb_seq_send_midi_clk_event(u8 evnt0, u32 bpm_tick);

#endif
