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
-I./FreeRTOS                            \
-I./FreeRTOS/include                    \
-I./FreeRTOS/portable                   \
-I./drivers/display                     \
-I./drivers/keypad                      \
-I./drivers/spi                         \
-I./drivers/sdcard                      \
-I./drivers/rtt                         \
-I./apps/GUI                            \
-I./apps/tools                          \
-I./apps/FatFs                          \
-I./apps/stream                         \
-I./apps/antsh                          \
-I./apps/lwip-1.3.2/src/include         \
-I./apps/lwip-1.3.2/src/include/ipv4    \
-I./apps/lwip-1.3.2/src/include/lwip    \
-I./apps/lwip-1.3.2/contrib/freertos    \
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


# RTOS files that are build in THUMB mode.
RTOS_THUMB_SRC=                         \
  FreeRTOS/tasks.c                      \
  FreeRTOS/queue.c                      \
  FreeRTOS/list.c                       \
  FreeRTOS/heap_1.c

RTOS_PORTABLE_THUMB_SRC=                \
  FreeRTOS/portable/port.c        


# Application files that are build in THUMB mode.
DRIVERS_DISPLAY_THUMB_SRC=              \
  drivers/display/display.c             \
  drivers/display/lcd1x16_hd44780.c     

DRIVERS_KEYPAD_THUMB_SRC=               \
  drivers/keypad/kpad_matrix3x3.c       \
  drivers/keypad/keypad.c               \

APPS_GUI_THUMB_SRC=                     \
  apps/GUI/gui.c

APPS_TOOLS_THUMB_SRC=                   \
  apps/tools/tools.c

APPS_STREAM_THUMB_SRC=                  \
  apps/stream/stream.c

SYSTEM_THUMB_SRC=                       \
  system/system.c

#  apps/lwip-1.3.2/src/core/tcp_out.c    \
#  apps/lwip-1.3.2/src/core/mem.c        \
#  apps/lwip-1.3.2/src/core/memp.c       \
#  apps/lwip-1.3.2/src/core/netif.c      \
#  apps/lwip-1.3.2/src/core/pbuf.c       \
#  apps/lwip-1.3.2/src/core/raw.c        \
#  apps/lwip-1.3.2/src/core/stats.c      \
#  apps/lwip-1.3.2/src/core/sys.c        \
#  apps/lwip-1.3.2/src/core/tcp.c        \
#  apps/lwip-1.3.2/src/core/tcp_in.c     \
#  apps/lwip-1.3.2/src/core/ipv4/ip.c    \
#  apps/lwip-1.3.2/src/core/ipv4/ip_addr.c \
#  apps/lwip-1.3.2/src/core/ipv4/icmp.c  \
#  apps/lwip-1.3.2/src/api/tcpip.c       \
#  apps/lwip-1.3.2/src/api/api_msg.c     \
#  apps/lwip-1.3.2/src/api/err.c         \
#  apps/lwip-1.3.2/src/api/api_lib.c     \
#  apps/lwip-1.3.2/src/api/netbuf.c      \
#  apps/lwip-1.3.2/src/netif/etharp.c    \
#  apps/lwip-1.3.2/src/netif/ethernetif.c\
#  apps/lwip-1.3.2/src/core/udp.c        \
#  apps/lwip-1.3.2/src/core/init.c       \
#  apps/lwip-1.3.2/src/core/ipv4/ip_frag.c     \
#  apps/lwip-1.3.2/src/core/ipv4/inet_chksum.c \
#  apps/lwip-1.3.2/contrib/freertos/sys_arch.c \

#  FatFs/sdcard.c                        \
#  FatFs/spi.c                           \
#  FatFs/diskio.c                        \
#  FatFs/ff.c                            \
#  antsh/antsh.c                         \
#  antsh/antsh_io.c                      \



# RTOS files that are build in ARM mode.
RTOS_PORTABLE_ARM_SRC=                  \
  FreeRTOS/portable/portISR.c

DRIVERS_RTT_ARM_SRC=                    \
  drivers/rtt/rtt.c


# Application files that are build in ARM mode.
RTOS_THUMB_OBJ = $(RTOS_THUMB_SRC:.c=.o)
RTOS_PORTABLE_THUMB_OBJ = $(RTOS_PORTABLE_THUMB_SRC:.c=.o)
DRIVERS_DISPLAY_THUMB_OBJ = $(DRIVERS_DISPLAY_THUMB_SRC:.c=.o)
DRIVERS_KEYPAD_THUMB_OBJ = $(DRIVERS_KEYPAD_THUMB_SRC:.c=.o)
APPS_GUI_THUMB_OBJ = $(APPS_GUI_THUMB_SRC:.c=.o)
APPS_TOOLS_THUMB_OBJ = $(APPS_TOOLS_THUMB_SRC:.c=.o)
APPS_STREAM_THUMB_OBJ = $(APPS_STREAM_THUMB_SRC:.c=.o)
SYSTEM_THUMB_OBJ = $(SYSTEM_THUMB_SRC:.c=.o)
RTOS_PORTABLE_ARM_OBJ = $(RTOS_PORTABLE_ARM_SRC:.c=.o)
DRIVERS_RTT_ARM_OBJ = $(DRIVERS_RTT_ARM_SRC:.c=.o)

$(OBJDIR)/befree.bin : $(OBJDIR)/befree.elf
	$(OBJCOPY) $(OBJDIR)/befree.elf -O binary $(OBJDIR)/befree.bin

$(OBJDIR)/befree.elf : $(RTOS_PORTABLE_ARM_OBJ) $(DRIVERS_RTT_ARM_OBJ) $(RTOS_THUMB_OBJ) $(RTOS_PORTABLE_THUMB_OBJ) $(DRIVERS_DISPLAY_THUMB_OBJ) $(DRIVERS_KEYPAD_THUMB_OBJ) $(APPS_GUI_THUMB_OBJ) $(APPS_TOOLS_THUMB_OBJ) $(APPS_STREAM_THUMB_OBJ) $(SYSTEM_THUMB_OBJ) $(CRT0) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) $(CFLAGS) $(RTOS_PORTABLE_ARM_OBJ) $(DRIVERS_RTT_ARM_OBJ) $(RTOS_THUMB_OBJ) $(RTOS_PORTABLE_THUMB_OBJ) $(DRIVERS_DISPLAY_THUMB_OBJ) $(DRIVERS_KEYPAD_THUMB_OBJ) $(APPS_GUI_THUMB_OBJ) $(APPS_TOOLS_THUMB_OBJ) $(APPS_STREAM_THUMB_OBJ) $(SYSTEM_THUMB_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)


$(RTOS_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(RTOS_PORTABLE_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(DRIVERS_DISPLAY_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(DRIVERS_KEYPAD_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(APPS_GUI_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(APPS_TOOLS_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(APPS_STREAM_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(SYSTEM_THUMB_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(RTOS_PORTABLE_ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

$(DRIVERS_RTT_ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile ./FreeRTOS/FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	touch Makefile

