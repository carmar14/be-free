//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : sys_arch.h - lwIP operating system simulation layer. This file is a part of
//           BeFree project, and lwIP FreeRTOS port.
// Date    : 18/05/2010 23:20
// Website : http://www.bugtraker.pl 
//
// BeFree - Copyright (C) 2009,2010 BugTraker (http://www.bugtraker.pl).
//
// BeFree is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// BeFree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeFree; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------------------


#ifndef SYS_ARCH_H
#define SYS_ARCH_H


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define SYS_SEM_NULL                    NULL
#define SYS_MBOX_NULL                   NULL

typedef xSemaphoreHandle                sys_sem_t;
typedef xQueueHandle                    sys_mbox_t;
typedef xTaskHandle                     sys_thread_t;


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------

sys_sem_t sys_sem_new (u8_t count);
void sys_sem_free (sys_sem_t sem);
void sys_sem_signal (sys_sem_t sem);
u32_t sys_arch_sem_wait (sys_sem_t sem, u32_t timeout);
sys_mbox_t sys_mbox_new (s32_t size);
void sys_mbox_free (sys_mbox_t mbox);
void sys_mbox_post (sys_mbox_t mbox, void *msg);
err_t sys_mbox_trypost (sys_mbox_t mbox, void *msg);
u32_t sys_arch_mbox_fetch (sys_mbox_t mbox, void **msg, u32_t timeout);
u32_t sys_arch_mbox_tryfetch (sys_mbox_t mbox, void **msg);
sys_thread_t sys_thread_new (c8_t *name, 
                             void (*thread)(void *arg), 
                             void *arg, 
                             s32_t stacksize, 
                             s32_t prio);
struct sys_timeouts *sys_arch_timeouts (void);



#endif
