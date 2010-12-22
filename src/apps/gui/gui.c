//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : gui.c - GUI source (menu system).
// Date    : 03/10/2009 00:00
// Website : http://bugtraker.pl 
//
// BeFree - Copyright (C) 2009-2010 BugTraker (http://bugtraker.pl).
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


#ifndef _GUI_C_
#define _GUI_C_

#include "FreeRTOS.h"
#include "task.h"
#include "system.h"
#include "display.h"
#include "keypad.h"
#include "rtt.h"
//bugtraker#include "./antsh/antsh.h"
#include "gui.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define MAIN_MENU_NUM_SM                1

typedef struct menu_struct_t
  {
    char_t  *name;
    void   (*exe) (void);
    const struct menu_struct_t *ent;
    const struct menu_struct_t *esc;
    const struct menu_struct_t *next;
    const struct menu_struct_t *prev;
  } menu_struct_t;


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


static void gui_tsk (void);
static void gui_ent (void);

static void test_shell (void);


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static const menu_struct_t main_menu[MAIN_MENU_NUM_SM] =
  {
    {
      "Test Shell       ",    &test_shell,   &main_menu[0], NULL,          &main_menu[0], &main_menu[0]
    }
  };


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// GUI initialization.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
gui_init (void)
{
  uint8_t param;

  
  xTaskCreate(gui_tsk,
              "gui",
              configMINIMAL_STACK_SIZE,
              (void *) &param,
              tskIDLE_PRIORITY,
              (xTaskHandle *) NULL);
}


//---------------------------------------------------------------------------------------
//
//                           L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// GUI task.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
gui_tsk (void)
{
  char_t buff[16] = "                ";
  uint32_t time = 0;
  uint8_t key   = KEY_NULL;
  uint8_t i     = 0;

  
  vTaskDelay(DLY_1SEC);
  display_str("befree", 6, 15);
  vTaskDelay(DLY_1SEC);

  for (i = 0; ; i++)
    {
      time = rtt_get_time();
      rtt_time_fmt(buff, &time, RTT_FMT_ISO_8601);

      AT91C_BASE_SSC->SSC_THR = 0xA5;

      if (0 == (i % 2))
        {
          buff[13] = ' ';
        }
      display_str(buff, 0, 15);
      key = keypad_getkey();
      if (KEY_MENU == key)
        {
          gui_ent();
        }
      vTaskDelay(DLY_500MS);
    }
}


//---------------------------------------------------------------------------------------
// GUI entry point. This function is called when system is idle and date and time can be
// seen on the display, and MENU button is pressed.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
gui_ent (void)
{
  const menu_struct_t *p_menu = NULL;
  uint8_t key = KEY_NULL;


  p_menu = &main_menu[0];
  display_str(p_menu->name, 0, 15);
  while (NULL != p_menu)
    {
      key = keypad_getkey();
      switch (key)
        {
          case KEY_NEXT:
            p_menu = p_menu->next;
            break;
          
          case KEY_PREV:
            p_menu = p_menu->prev;
            break;
          
          case KEY_YES:
            if (NULL != p_menu->exe)
              {
                p_menu->exe();
              }
            p_menu = p_menu->ent;
            break;

          case KEY_NO:
            p_menu = p_menu->esc;
            break;

          default:
            break;
        }
      if ((KEY_NULL != key) && (NULL != p_menu))
        {
          display_str(p_menu->name, 0, 15);
          key = KEY_NULL;
        }
      vTaskDelay(DLY_500MS);
    }
}


//---------------------------------------------------------------------------------------
// Test shell.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void 
test_shell (void)
{
}



#endif
