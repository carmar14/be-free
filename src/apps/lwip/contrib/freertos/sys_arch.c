//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : sys_arch.c - lwIP operating system simulation layer. This file is a part of
//           BeFree project, and lwIP FreeRTOS port.
// Date    : 18/05/2010 23:10
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


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "lwip/sys.h"
#include "semphr.h"
#include "queue.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define LWIP_MBOX_DEBUG                 0                                                 // Set this to 1 to enable mbox debug.
#define LWIP_SYS_TMO_DEBUG              0                                                 // Set this to 1 to enable system timeouts debug.
#define LWIP_MBOX_POST_TMO              (portTickType) 1000                               // Number system ticks to post message.
#define LWIP_TSKS_MAX                   5                                                 // Maximal number of lwIP tasks.

typedef struct sys_tsk_info_t
  {
    sys_thread_t hdle;
    struct sys_timeouts tmo;
  } sys_tsk_info_t;


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static sys_tsk_info_t lwip_sys_tsk_info[LWIP_TSKS_MAX];


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// lwip system init.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
sys_init (void)
{}


//---------------------------------------------------------------------------------------
// Create system semaphore.
//
// Arguments:
// u8_t count - initial state of the semaphore. Currently lwIP 1.3.2, 'count' is either
//              0 or 1. In case of 0 it is created and immediately taken, and only 
//              created otherwise.
//
// Return:
// sys_sem_t sem - handle to semaphore, NULL if not created.
//---------------------------------------------------------------------------------------
sys_sem_t 
sys_sem_new (u8_t count)
{
  sys_sem_t sem;  


  vSemaphoreCreateBinary(sem);
  if (NULL == sem)                                                                        // Check if semaphore created.
    {
      sem = SYS_SEM_NULL;                                                                 // Convert NULL to SYS_SEM_NULL for lwIP.
    }
  else
    {
      if (0 == count)                                                                     // Check if initial count is zero.
        {
          xSemaphoreTake(sem, (portTickType) 0);                                          // Take the semaphore with no block time.
        }
    }

  return (sem);
}


//---------------------------------------------------------------------------------------
// Free system semaphore.
//
// Arguments:
// sys_sem_t sem - semaphore to free.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
sys_sem_free (sys_sem_t sem)
{
  if (SYS_SEM_NULL != sem)                                                                // Check if semaphore exists.
    {
      vQueueDelete(sem);
    }
}


//---------------------------------------------------------------------------------------
// Releases a semaphore.
//
// Arguments:
// sys_sem_t sem - semaphore to give.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
sys_sem_signal (sys_sem_t sem)
{
  xSemaphoreGive(sem);
}


//---------------------------------------------------------------------------------------
// Semaphore take.
//
// Arguments:
// sys_sem_t sem - semaphore to wait for;
// u32_t timeout - semaphore wait timeout.
//
// Return:
// u32_t ret     - amount of miliseconds elapsed.
//---------------------------------------------------------------------------------------
u32_t
sys_arch_sem_wait (sys_sem_t sem, u32_t timeout)
{
  portTickType tmr_start = xTaskGetTickCount();                                           // Get initial count.
  u32_t ret              = SYS_ARCH_TIMEOUT;                                              // Assume wait timeout.


  if (0 == timeout)                                                                       // Check if block indefinitely.
    {
      timeout = portMAX_DELAY;
    }
  if (pdTRUE == xSemaphoreTake(sem, timeout))
    {
      ret = xTaskGetTickCount() - tmr_start;                                              // The semaphore taken, now calculate elapsed time.
    }
    
  return (ret);
}


//---------------------------------------------------------------------------------------
// Create a new mailbox.
//
// Arguments:
// s32_t size - size of mbox. If '0' then mbox size if platform dependant.
//
// Return:
// sys_mbox_t mbox - created mailbox.
//---------------------------------------------------------------------------------------
sys_mbox_t
sys_mbox_new (s32_t size)
{
  sys_mbox_t mbox;


  if (0 == size)                                                                          // size 0 - platform dependant.
    {
      size = 1;
    }
  mbox = xQueueCreate(size, sizeof (void *));

  if (0 == mbox)
    {
      mbox = SYS_MBOX_NULL;                                                               // Could not create mbox.
    }

  return (mbox);
}


//---------------------------------------------------------------------------------------
// Free a mailbox.
//
// Arguments:
// sys_mbox_t mbox - mailbox to free.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
sys_mbox_free (sys_mbox_t mbox)
{
  if (SYS_MBOX_NULL != mbox)                                                              // Check if mbox exists.
    {

#if LWIP_MBOX_DEBUG == 1
  if (uxQueueMessagesWaiting(mbox) > 0)
    {
      // Insert your code here to say that mbox with messages has been deleted.
    }
#endif

      vQueueDelete(mbox);
    }
}


//---------------------------------------------------------------------------------------
// Post a message to the mailbox.
//
// Arguments:
// sys_mbox_t mbox - mailbox to post the message to.
// void *msg       - message to post. 
//
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
sys_mbox_post (sys_mbox_t mbox, void *msg)
{
#if LWIP_MBOX_DEBUG == 1
  if (pdTRUE != xQueueSend(mbox, msg, LWIP_MBOX_POST_TMO))
    {
      // Insert your code here to say that message has not been posted to mbox.
    }
#else
  xQueueSend(mbox, msg, LWIP_MBOX_POST_TMO);
#endif
}


//---------------------------------------------------------------------------------------
// Post a message to the mailbox by polling.
//
// Arguments:
// sys_mbox_t mbox - mailbox to post the message to.
// void *msg       - message to post. 
//
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
err_t
sys_mbox_trypost (sys_mbox_t mbox, void *msg)
{
  err_t ret = ERR_MEM;


  if (pdTRUE == xQueueSend(mbox, msg, 0))
    {
      ret = ERR_OK;
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Fetch message from mailbox. If message does not arrive to mailbox within specified
// timeout function exits.
//
// Arguments:
// sys_mbox_t mbox - mailbox to fetch message from;
// void **msg      - storage for fetched message;
// u32_t timeout   - timeout to fetch message.
//
//
// Return:
// u32_t ret       - number of miliseconds elapsed while waiting for message.
//---------------------------------------------------------------------------------------
u32_t
sys_arch_mbox_fetch (sys_mbox_t mbox, void **msg, u32_t timeout)
{
  portTickType tmr_start = xTaskGetTickCount();
  u32_t ret              = SYS_ARCH_TIMEOUT;


  if (0 == timeout)                                                                       // Check if block indefinitely.
    {
      timeout = portMAX_DELAY;
    }
  if (pdTRUE == xQueueReceive(mbox, *msg, timeout))
    {
      ret = xTaskGetTickCount() - tmr_start;                                              // Received from the mbox, now calculate elapsed time.
    }
  
  return (ret);
}


//---------------------------------------------------------------------------------------
// Try to fetch message from mailbox with no timeout.
//
// Arguments:
// sys_mbox_t mbox - mailbox to check;
// void **msg      - container for message.
//
//
// Return:
// u32_t ret       - 0 on success, SYS_MBOX_EMPTY otherwise.
//---------------------------------------------------------------------------------------
u32_t
sys_arch_mbox_tryfetch (sys_mbox_t mbox, void **msg)
{
  u32_t ret = SYS_MBOX_EMPTY;                                                             // Assume nothing in the mbox.
  

  if (0 != xQueueReceive(mbox, *msg, 0))                                                  // Receive from the mbox without blocking.
    {
      ret = 0;                                                                            // Received from the mbox, return 0.
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Create a new thread.
//
// Arguments:
// c8_t *name                - name of the thread;
// void (*thread)(void *arg) - pointer to the thread;
// s32_t stacksize           - size of stack for thread;
// void *arg                 - arguments for thread;
// s32_t prio                - thread priority.
//
//
// Return:
// sys_thread_t tsk_hdle - thread handle.
//---------------------------------------------------------------------------------------
sys_thread_t
sys_thread_new (c8_t *name, void (*thread)(void *arg), 
                void *arg, s32_t stacksize, s32_t prio)
{
  static u8_t tsk_cnt   = 0;
  sys_thread_t tsk_hdle = NULL;
  

  if (pdPASS == xTaskCreate(thread, name, stacksize, arg, prio, &lwip_sys_tsk_info[tsk_cnt].hdle))
    {
      tsk_hdle = lwip_sys_tsk_info[tsk_cnt++].hdle;
       
    }
 
  return (tsk_hdle);
}


//---------------------------------------------------------------------------------------
// Returns current task timer.
//
// Arguments:
// N/A
//
// Return:
// struct sys_timeouts * &lwip_sys_tsk_info[].tmo - returns current task timer.
//---------------------------------------------------------------------------------------
struct sys_timeouts *
sys_arch_timeouts (void)
{
  u8_t i = 0;
  sys_thread_t tsk_hdle = xTaskGetCurrentTaskHandle();


  for (i = 0; i < LWIP_TSKS_MAX; i++)
    {
      if (tsk_hdle == lwip_sys_tsk_info[i].hdle)
        {
          break;
        }
    }

#if LWIP_SYS_TMO_DEBUG == 1
  if (LWIP_TSKS_MAX == i)
    {
    // Put your code here to handle this. This should never happen.
    }
#endif

  return (&lwip_sys_tsk_info[i].tmo);
}



//---------------------------------------------------------------------------------------
//
//                         L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


