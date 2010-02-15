//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : display.c - source of application level dealing with display.
// Date    : 29/09/2009 23:41
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

#include "./system.h"
#include "./types.h"

#include "display.h"
#include "lcd1x16_hd44780.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                          L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static const char_t befree_logo[] = "      befree    ";
static xSemaphoreHandle dpy_mutex;


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Initialize display application level. Function checks if LCD queue has been created
// yet, and creates mutext for display application interface.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
display_init (void)
{
  uint8_t param;

  dpy_mutex = xSemaphoreCreateMutex();
  if (0 == dpy_mutex)
    {
      sys_rst();
    }
  xTaskCreate(lcd_tsk, 
              "lcd", 
              configMINIMAL_STACK_SIZE, 
              (void *) param, 
              tskIDLE_PRIORITY, 
              (xTaskHandle *) NULL);
}


//---------------------------------------------------------------------------------------
// User interface to display string on the display.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
dpy_mrc_t 
display_str (char_t *str, uint8_t start, uint8_t end)
{
  dpy_mrc_t ret = DPY_MRC_TMO;


  if (pdTRUE == xSemaphoreTake(dpy_mutex, M_WAIT_1SEC))
    {
      if (LCD_MRC_OK == lcd_printf(str, start, end, 0))
        {
          ret = DPY_MRC_OK;
        }
      else
        {
          ret = DPY_MRC_ERR;
        }
      xSemaphoreGive(dpy_mutex);
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// User interface for clearing display. Code stays here until display is available to 
// clear.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
dpy_mrc_t
display_clr (void)
{
  dpy_mrc_t ret = DPY_MRC_TMO;


  if (pdTRUE == xSemaphoreTake(dpy_mutex, M_WAIT_1SEC))
    {
      if (LCD_MRC_OK == lcd_clr_dpy())
        {
          ret = DPY_MRC_OK;
        }
      else
        {
          ret = DPY_MRC_ERR;
        }
      xSemaphoreGive(dpy_mutex);
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// User interface for clearing display. Code stays here until display is available to 
// clear.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
dpy_mrc_t
display_mv_to (uint8_t pos)
{
  dpy_mrc_t ret = DPY_MRC_TMO;


  if (pdTRUE == xSemaphoreTake(dpy_mutex, M_WAIT_1SEC))
    {
      if (LCD_MRC_OK == lcd_crsr_mv(pos))
        {
          ret = DPY_MRC_OK;          
        }
      else
        {
          ret = DPY_MRC_ERR;
        }
      xSemaphoreGive(dpy_mutex);
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// Scrolles string on the display, in range start-end at given speed.
//
// Arguments:
// char_t *str   - string to display.
// uint8_t start - starting LCD position.
// uint8_t end   - ending LCD position.
// uint8_t fast  - indicates speed of scroll (fast/slow).
//
// Return:
// dpy_mrc_t ret - module return code.
//---------------------------------------------------------------------------------------
dpy_mrc_t
display_scrl (char_t *str, uint8_t start, uint8_t end, uint8_t fast)
{
  dpy_mrc_t ret = DPY_MRC_TMO;
  uint32_t  cfg = 0;


  if (pdTRUE == xSemaphoreTake(dpy_mutex, M_WAIT_1SEC))
    {
      ret = DPY_MRC_ERR;
      cfg = xCFG_SCRL_STR | xCFG_SCRL_INFT;
      if (true == fast)
        {
          cfg |= xCFG_SCRL_FAST;
        }
      if (LCD_MRC_OK == lcd_printf(str, start, end, cfg))
        {
          ret = DPY_MRC_OK;
        }
      xSemaphoreGive(dpy_mutex);
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
//
//                           L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------

