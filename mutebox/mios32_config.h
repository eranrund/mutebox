// $Id: mios32_config.h 502 2009-05-09 14:20:30Z tk $
/*
 * Local MIOS32 configuration file
 *
 * this file allows to disable (or re-configure) default functions of MIOS32
 * available switches are listed in $MIOS32_PATH/modules/mios32/MIOS32_CONFIG.txt
 *
 */

#ifndef _MIOS32_CONFIG_H
#define _MIOS32_CONFIG_H

// The boot message which is print during startup and returned on a SysEx query
#define MIOS32_LCD_BOOT_MSG_LINE1 "MuteBox"
#define MIOS32_LCD_BOOT_MSG_LINE2 "(C) 2014 <Eran Rundstein>"


#define MIOS32_SRIO_NUM_SR 1
#define AINSER_NUM_MODULES 1


#endif /* _MIOS32_CONFIG_H */
