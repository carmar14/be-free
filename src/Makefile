##---------------------------------------------------------------------------------------
## Author  : BugTraker
## File    : Makefile for building BeFree software.
## Date    : 02/10/2009 01:13
## Website : http://www.bugtraker.pl 
##
## BeFree - Copyright (C) 2009, 2010 BugTraker (http://www.bugtraker.pl).
##
## BeFree is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## BeFree is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with BeFree; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##---------------------------------------------------------------------------------------


LDSCRIPT=system/atmel-rom.ld
ARCH=arm-elf-ar
CC=arm-elf-gcc
OBJCOPY=arm-elf-objcopy
CRT0=system/boot.s
OPTIM=-O0
DEBUG=-g
OBJDIR = ../Obj

# CFLAGS for THUMB and ARM builds.
CFLAGS=                                 \
-I.                                     \
-I./kernel/freertos                     \
-I./kernel/freertos/include             \
-I./kernel/freertos/portable            \
-I./drivers/display                     \
-I./drivers/keypad                      \
-I./drivers/spi                         \
-I./drivers/rtt                         \
-I./lib/sdcard                          \
-I./lib/dogm128                         \
-I./lib/fatfs                           \
-I./lib/stream                          \
-I./lib/antsh                           \
-I./apps/gui                            \
-I./system                              \
-Wall                                   \
-Wextra                                 \
-Wstrict-prototypes                     \
-Wmissing-prototypes                    \
-Wmissing-declarations                  \
-Wno-strict-aliasing                    \
-D SAM7_GCC                             \
-D THUMB_INTERWORK                      \
-mthumb-interwork                       \
-mcpu=arm7tdmi                          \
-T$(LDSCRIPT)                           \
$(DEBUG)                                \
$(OPTIM)                                \
-fomit-frame-pointer                    \


# Flags.
THUMB_FLAGS=-mthumb
LINKER_FLAGS=-Xlinker -o$(OBJDIR)/befree.elf -Xlinker -M -Xlinker -Map=$(OBJDIR)/befree.map


# Kernel 'FreeRTOS' source compiled in thumb mode
KERNEL_THUMB_SRC=                       \
  kernel/freertos/tasks.c               \
  kernel/freertos/queue.c               \
  kernel/freertos/list.c                \
  kernel/freertos/heap_1.c              \
  kernel/freertos/portable/port.c       \


# Drivers build for the system
DRIVERS_THUMB_SRC=                      \
  drivers/display/display.c             \
  drivers/display/lcd1x16_hd44780.c     \
  drivers/keypad/kpad_matrix3x3.c       \
  drivers/keypad/keypad.c               \
  drivers/spi/spi.c                     \


# Applications build for the system
APPS_THUMB_SRC=                         \
  apps/gui/gui.c


# Libraries build into the system
LIB_THUMB_SRC=                          \
  lib/stream/stream.c                   \
  lib/sdcard/sdcard.c                   \
  lib/fatfs/diskio.c                    \
  lib/fatfs/ff.c                        \
  lib/antsh/antsh.c                     \
  lib/antsh/antsh_io.c                  \
  lib/dogm128/dogm128.c                 \
  lib/dogm128/dogmcb.c                  \
  lib/dogm128/dogmch.c                  \
  lib/dogm128/dogmcp.c                  \
  lib/dogm128/dogmcv.c                  \
  lib/dogm128/dogmda.c                  \
  lib/dogm128/dogmdl.c                  \
  lib/dogm128/dogmdp.c                  \
  lib/dogm128/dogmds.c                  \
  lib/dogm128/dogmdsp.c                 \
  lib/dogm128/dogmfonc.c                \
  lib/dogm128/dogmitoa.c                \
  lib/dogm128/dogmma.c                  \
  lib/dogm128/dogmov.c                  \
  lib/dogm128/dogmsb.c                  \
  lib/dogm128/dogmsd.c                  \
  lib/dogm128/dogmsh.c                  \
  lib/dogm128/dogmsm.c                  \
  lib/dogm128/dogmsmp.c                 \
  lib/dogm128/dogmsp.c                  \
  lib/dogm128/dogmspi.c                 \
  lib/dogm128/dogmsv.c                  \
  lib/dogm128/dogmsysc.c                \
  lib/dogm128/dogmsysi.c                \
  lib/dogm128/dogmxb.c                  \
  lib/dogm128/dogmxh.c                  \
  lib/dogm128/dogmxp.c                  \
  lib/dogm128/dogmxv.c                  \
  lib/dogm128/font_4x6.c                \
  lib/dogm128/font_5x7.c                \
  lib/dogm128/font_5x8.c                \
  lib/dogm128/font_6x10.c               \
  lib/dogm128/font_6x12.c               \
  lib/dogm128/font_6x13.c               \
  lib/dogm128/font_6x9.c                \
  lib/dogm128/font_7x13.c               \
  lib/dogm128/font_7x14.c               \
  lib/dogm128/font_8x13.c               \
  lib/dogm128/font_8x16.c               \
  lib/dogm128/font_9x15.c               \
  lib/dogm128/font_9x18.c



# Main system files
SYSTEM_THUMB_SRC=                       \
  system/system.c


# Kernel files build in ARM mode
KERNEL_ARM_SRC=                         \
  kernel/freertos/portable/portISR.c


# Drivers build in ARM mode
DRIVERS_ARM_SRC=                        \
  drivers/rtt/rtt.c


# Application files that are build in ARM mode.
KERNEL_THUMB_OBJ = $(KERNEL_THUMB_SRC:.c=.o)
DRIVERS_THUMB_OBJ = $(DRIVERS_THUMB_SRC:.c=.o)
APPS_THUMB_OBJ = $(APPS_THUMB_SRC:.c=.o)
LIB_THUMB_OBJ = $(LIB_THUMB_SRC:.c=.o)
SYSTEM_THUMB_OBJ = $(SYSTEM_THUMB_SRC:.c=.o)
KERNEL_ARM_OBJ = $(KERNEL_ARM_SRC:.c=.o)
DRIVERS_ARM_OBJ = $(DRIVERS_ARM_SRC:.c=.o)

$(OBJDIR)/befree.bin : $(OBJDIR)/befree.elf
	$(OBJCOPY) $(OBJDIR)/befree.elf -O binary $(OBJDIR)/befree.bin

$(OBJDIR)/befree.elf : $(KERNEL_ARM_OBJ) $(DRIVERS_ARM_OBJ) $(KERNEL_THUMB_OBJ) $(DRIVERS_THUMB_OBJ) $(APPS_THUMB_OBJ) $(LIB_THUMB_OBJ) $(SYSTEM_THUMB_OBJ) $(CRT0) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) $(KERNEL_ARM_OBJ) $(DRIVERS_ARM_OBJ) $(KERNEL_THUMB_OBJ) $(DRIVERS_THUMB_OBJ) $(APPS_THUMB_OBJ) $(LIB_THUMB_OBJ) $(SYSTEM_THUMB_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)


$(KERNEL_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(DRIVERS_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(APPS_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(LIB_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(SYSTEM_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(KERNEL_ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

$(DRIVERS_ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile ./kernel/freertos/FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	touch Makefile

