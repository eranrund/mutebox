# $Id: Makefile 346 2009-02-10 22:10:39Z tk $

################################################################################
# following setup taken from environment variables
################################################################################

PROCESSOR =	$(MIOS32_PROCESSOR)
FAMILY    = 	$(MIOS32_FAMILY)
BOARD	  = 	$(MIOS32_BOARD)

#explicitely select PCD8544 display
LCD       =     $(MIOS32_LCD)


################################################################################
# Source Files, include paths and libraries
################################################################################

THUMB_SOURCE    = app.c \
		  osc_server.c \
		  osc_client.c \
		  fs.c xml.c \
		  uip_task.c \
		  telnetd.c \
		  shell.c \
		  dhcpc.c 

# (following source stubs not relevant for Cortex M3 derivatives)
THUMB_AS_SOURCE =
ARM_SOURCE      =
ARM_AS_SOURCE   =

C_INCLUDE = 	-I .
A_INCLUDE = 	-I .

LIBS = 		


################################################################################
# Remaining variables
################################################################################

LD_FILE   = 	$(MIOS32_PATH)/etc/ld/$(FAMILY)/$(PROCESSOR).ld
PROJECT   = 	project

DEBUG     =	-g -Werror
OPTIMIZE  =	-Os -fno-strict-aliasing

CFLAGS =	$(DEBUG) $(OPTIMIZE)


################################################################################
# Include source modules via additional makefiles
################################################################################

# sources of programming model
include $(MIOS32_PATH)/programming_models/traditional/programming_model.mk

# application specific LCD driver (selected via makefile variable)
include $(MIOS32_PATH)/modules/app_lcd/$(LCD)/app_lcd.mk
# (for the case that the LCD driver hasn't already included the GLCD font package)
# include $(MIOS32_PATH)/modules/glcd_font/glcd_font.mk

# DMX 
include $(MIOS32_PATH)/modules/dmx/dmx.mk

# UWindows
include $(MIOS32_PATH)/modules/uwindows/uwindows.mk

# DOSFS Driver
include $(MIOS32_PATH)/modules/dosfs/dosfs.mk

# UIP driver
include $(MIOS32_PATH)/modules/uip/uip.mk

# common make rules
# Please keep this include statement at the end of this Makefile. Add new modules above.
include $(MIOS32_PATH)/include/makefile/common.mk
