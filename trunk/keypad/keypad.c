//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : keypad.c - application interface for accessing matrix keypad.
// Date    : 03/10/2009 00:01
// Website : http://www.bugtraker.pl 
//
// BeFree - Copyright (C) 2009 BugTraker (http://www.bugtraker.pl).
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
//                            I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "./RTOS/include/FreeRTOS.h"
#include "./RTOS/include/task.h"
#include "./RTOS/include/semphr.h"

#include "./types.h"
#include "./system.h"

#include "kpad_matrix3x3.h"
#include "keypad.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static xSemaphoreHandle key_mutex;


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Initialize keypad driver by creating matrix keypad task. 
// Initialize mutex for keypad interface.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
keypad_init (void)
{
  uint8_t param;


  key_mutex = xSemaphoreCreateMutex();
  if (0 == key_mutex)
    {
      sys_rst();
    }
  xTaskCreate(kpad_matrix_tsk, 
              "kpad", 
              configMINIMAL_STACK_SIZE,
              (void *) param,
              tskIDLE_PRIORITY,
              (xTaskHandle *) NULL);
}


//---------------------------------------------------------------------------------------
// Read key from matrix keypad buffer. Calls matrix keypad driver function
// which reads from a queue.
//
// Arguments:
// N/A
//
// Return:
// uint8_t key - pressed key.
//---------------------------------------------------------------------------------------
uint8_t 
keypad_getkey (void)
{
  uint8_t key;


  xSemaphoreTake(key_mutex, portMAX_DELAY);
  key = kpad_matrix_getkey();
  xSemaphoreGive(key_mutex);
  return (key);
}


//---------------------------------------------------------------------------------------
//
//                           L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


