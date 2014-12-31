// $Id: app.c 1920 2014-01-08 19:29:35Z tk $
/*
 * MIOS32 Application Template
 *
 * ==========================================================================
 *
 *  Copyright (C) <year> <your name> (<your email address>)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include <string.h>

#include <ainser.h>
#include <seq_bpm.h>
#include <seq_midi_out.h>
#include <eeprom.h>
#include "app.h"


// include everything FreeRTOS related
#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "mb_common.h"
#include "mb_mgr.h"
#include "mb_applet_md_mutes.h"
#include "mb_seq.h"


#define PRIORITY_TASK_AINSER_SCAN ( tskIDLE_PRIORITY + 3 )
#define PRIORITY_TASK_SEQ		( tskIDLE_PRIORITY + 4 )

static void APP_AINSER_NotifyChange(u32 module, u32 pin, u32 pin_value)
{
    mb_mgr_notify_ui_pot_change(pin, pin_value);
    /*if (pin == 1) {
    MIOS32_MIDI_SendDebugMessage("%d %d\n", pin, pin_value >> 5);
        MIOS32_MIDI_SendCC(UART0, Chn3, 16, pin_value >> 5);
    }*/

}
static void TASK_AINSER_Scan(void *pvParameters)
{
  portTickType xLastExecutionTime;

  // Initialise the xLastExecutionTime variable on task entry
  xLastExecutionTime = xTaskGetTickCount();

  while( 1 ) {
    vTaskDelayUntil(&xLastExecutionTime, 1 / portTICK_RATE_MS);

    // toggle Status LED
    MIOS32_BOARD_LED_Set(1, ~MIOS32_BOARD_LED_Get());

    // scan pins
    AINSER_Handler(APP_AINSER_NotifyChange);

  }
}


static s32 NOTIFY_MIDI_Rx(mios32_midi_port_t port, u8 midi_byte)
{
  // here we could filter a certain port
  // The BPM generator will deliver inaccurate results if MIDI clock 
  // is received from multiple ports
  SEQ_BPM_NotifyMIDIRx(midi_byte);

  return 0; // no error, no filtering
}

static void TASK_SEQ(void *pvParameters)
{
  portTickType xLastExecutionTime;

  // Initialise the xLastExecutionTime variable on task entry
  xLastExecutionTime = xTaskGetTickCount();

  while( 1 ) {
    vTaskDelayUntil(&xLastExecutionTime, 1 / portTICK_RATE_MS);

    // execute sequencer handler
    mb_seq_handler();

    // send timestamped MIDI events
    SEQ_MIDI_OUT_Handler();
  }
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called after startup to initialize the application
/////////////////////////////////////////////////////////////////////////////
void APP_Init(void)
{
    mb_common_init();
    mb_mgr_init();

    MIOS32_BOARD_LED_Init(0xffffffff);
    //EEPROM_Init(0);
    
    AINSER_Init(0);
    AINSER_DeadbandSet(0, 16);
    AINSER_DeadbandSet(1, 16);
    xTaskCreate(TASK_AINSER_Scan, (signed portCHAR *)"AINSER_Scan", configMINIMAL_STACK_SIZE, NULL, PRIORITY_TASK_AINSER_SCAN, NULL);

    SEQ_MIDI_OUT_Init(0);
    mb_seq_init();
    MIOS32_MIDI_DirectRxCallback_Init(NOTIFY_MIDI_Rx);
    xTaskCreate(TASK_SEQ, (signed portCHAR *)"SEQ", configMINIMAL_STACK_SIZE, NULL, PRIORITY_TASK_SEQ, NULL);
//    SEQ_BPM_Start();

    //
    mb_mgr_start(&mb_applet_md_mutes);
}


/////////////////////////////////////////////////////////////////////////////
// This task is running endless in background
/////////////////////////////////////////////////////////////////////////////
void APP_Background(void)
{
    mb_mgr_background();
} 

/*

  // clear LCD
  MIOS32_LCD_Clear();
  u32 last_s = 0;

  // endless loop
  while( 1 ) {
    mios32_sys_time_t t = MIOS32_SYS_TimeGet();

    if (!(t.seconds - last_s)) {
        continue;
    }

    last_s = t.seconds;


    // print system time
    MIOS32_LCD_CursorSet(0, 0); // X, Y
    MIOS32_LCD_PrintFormattedString("System Time");

    MIOS32_LCD_CursorSet(0, 1); // X, Y
    int hours = t.seconds / 3600;
    int minutes = (t.seconds % 3600) / 60;
    int seconds = (t.seconds % 3600) % 60;
    int milliseconds = t.fraction_ms;
    MIOS32_LCD_PrintFormattedString("%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
*/

/////////////////////////////////////////////////////////////////////////////
// This hook is called each mS from the main task which also handles DIN, ENC
// and AIN events. You could add more jobs here, but they shouldn't consume
// more than 300 uS to ensure the responsiveness of buttons, encoders, pots.
// Alternatively you could create a dedicated task for application specific
// jobs as explained in $MIOS32_PATH/apps/tutorials/006_rtos_tasks
/////////////////////////////////////////////////////////////////////////////
void APP_Tick(void)
{
  // PWM modulate the status LED (this is a sign of life)
  u32 timestamp = MIOS32_TIMESTAMP_Get();
  //MIOS32_BOARD_LED_Set(4, (timestamp % 20) <= ((timestamp / 100) % 10));
  MIOS32_BOARD_LED_Set(1, ((timestamp % 20) <= ((timestamp / 100) % 10)) ? 1 : 0);
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called each mS from the MIDI task which checks for incoming
// MIDI events. You could add more MIDI related jobs here, but they shouldn't
// consume more than 300 uS to ensure the responsiveness of incoming MIDI.
/////////////////////////////////////////////////////////////////////////////
void APP_MIDI_Tick(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a MIDI package has been received
/////////////////////////////////////////////////////////////////////////////
void APP_MIDI_NotifyPackage(mios32_midi_port_t port, mios32_midi_package_t midi_package)
{
    if (port == USB3) {
        switch (midi_package.event)
        {
            case CC:
                switch (midi_package.value1)
                {
                    case 0:
                        mb_mgr_notify_scs_btn_toggle(midi_package.value2, 1);
                        break;

                    default:
                        MIOS32_MIDI_SendDebugMessage("SCS ???\n");
                        break;
                }
                break;

            default:
                MIOS32_MIDI_SendDebugMessage("%d %d %d %d %d %d\n",
                    midi_package.type,
                    midi_package.cable,
                    midi_package.chn,
                    midi_package.event,
                    midi_package.value1,
                    midi_package.value2);
                break;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called before the shift register chain is scanned
/////////////////////////////////////////////////////////////////////////////
void APP_SRIO_ServicePrepare(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called after the shift register chain has been scanned
/////////////////////////////////////////////////////////////////////////////
void APP_SRIO_ServiceFinish(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void APP_DIN_NotifyToggle(u32 pin, u32 pin_value)
{
    mb_mgr_notify_ui_btn_toggle(7 - pin, pin_value ? 0 : 1);
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void APP_ENC_NotifyChange(u32 encoder, s32 incrementer)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void APP_AIN_NotifyChange(u32 pin, u32 pin_value)
{
//    MIOS32_MIDI_SendDebugMessage("%d %d\n", pin, pin_value);
}
