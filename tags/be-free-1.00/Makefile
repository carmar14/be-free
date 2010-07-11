##---------------------------------------------------------------------------------------
## Author  : BugTraker
## File    : Makefile for building BeFree project.
## Date    : 02/10/2009 01:13
## Website : http://www.bugtraker.pl 
##
## BeFree - Copyright (C) 2009 BugTraker (http://www.bugtraker.pl).
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


LDSCRIPT=atmel-rom.ld
ARCH=arm-elf-ar
CC=arm-elf-gcc
OBJCOPY=arm-elf-objcopy
CRT0=boot.s
OPTIM=-O0
DEBUG=-g
OBJDIR = Obj

# CFLAGS for THUMB and ARM builds.
CFLAGS=                                 \
-I.                                     \
-I./RTOS                                \
-I./RTOS/include                        \
-I./RTOS/portable                       \
-I./display                             \
-I./keypad                              \
-I./GIU                                 \
-I./rtt                                 \
-I./tools                               \
-I./FatFs                               \
-I./stream                              \
-I./antsh                               \
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


# RTOS files that are build in THUMB mode.
RTOS_THUMB_SRC=                         \
  RTOS/portable/port.c                  \
  RTOS/tasks.c                          \
  RTOS/queue.c                          \
  RTOS/list.c                           \
  RTOS/heap_1.c


# Application files that are build in THUMB mode.
APP_THMUB_SRC=                          \
  display/display.c                     \
  display/lcd1x16_hd44780.c             \
  keypad/kpad_matrix3x3.c               \
  keypad/keypad.c                       \
  GUI/gui.c                             \
  tools/tools.c                         \
  stream/stream.c                       \
  FatFs/sdcard.c                        \
  FatFs/spi.c                           \
  system.c                              \
  FatFs/diskio.c                        \
  FatFs/ff.c                            \
  antsh/antsh.c                         \
  antsh/antsh_io.c                      \



# RTOS files that are build in ARM mode.
ARM_SRC=                                \
  RTOS/portable/portISR.c               \
  rtt/rtt.c                             \


# Application files that are build in ARM mode.
#


# Definitions of object files.
ARM_OBJ = $(ARM_SRC:.c=.o)
RTOS_THUMB_OBJ = $(RTOS_THUMB_SRC:.c=.o)
APP_THMUB_OBJ = $(APP_THMUB_SRC:.c=.o)

$(OBJDIR)/befree.bin : $(OBJDIR)/befree.elf
	$(OBJCOPY) $(OBJDIR)/befree.elf -O binary $(OBJDIR)/befree.bin

$(OBJDIR)/befree.elf : $(ARM_OBJ) $(APP_THMUB_OBJ) $(LWIP_THUMB_OBJ) $(RTOS_THUMB_OBJ) $(CRT0) Makefile ./RTOS/FreeRTOSConfig.h
	$(CC) $(CFLAGS) $(ARM_OBJ) $(APP_THMUB_OBJ) $(LWIP_THUMB_OBJ) $(RTOS_THUMB_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)


$(APP_THMUB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./RTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(RTOS_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./RTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile ./RTOS/FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	touch Makefile

