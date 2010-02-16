//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : gui.c - GUI source (menu system).
// Date    : 03/10/2009 00:00
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

#include "./system.h"


#include "./display/display.h"
#include "./keypad/keypad.h"
#include "./rtt/rtt.h"
#include "./tools/tools.h"
#include "gui.h"


//DEBUG
#include "./FatFs/sdcard.h"
#include "./FatFs/ff.h"
#include "./stream/stream.h"
#include "./FatFs/ffconf.h"

#include "./antsh/antsh.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define MAIN_MENU_NUM_SM                3


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


static void gui_tsk (void);
static void gui_ent (void);

static void test_spi (void);
static void test_filesystem (void);
static void fatfs_info (void);
static void test_shell (void);

static inline void show_menu (char_t *name);


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


typedef struct
  {
    char_t  *name;
    void   (*exe) (void);
    struct menu_struct_t *ent;
    struct menu_struct_t *esc;
    struct menu_struct_t *next;
    struct menu_struct_t *prev;
  } menu_struct_t;

static const menu_struct_t main_menu[MAIN_MENU_NUM_SM];
static const menu_struct_t stgs_menu[1];

static const menu_struct_t main_menu[MAIN_MENU_NUM_SM] =
  {
    { 
      "Test SPI         ", &test_spi,        &main_menu[0], &main_menu[0], &main_menu[MAIN_MENU_NUM_SM - 1], &main_menu[1]
    },

    { 
      "Test File System ", &test_filesystem, &main_menu[1], &main_menu[1], &main_menu[0],                    &main_menu[2]
    },

    {
      "Test Shell       ", &test_shell,      &stgs_menu[0], &main_menu[2], &main_menu[1],                    &main_menu[0]
    }
  };

static const menu_struct_t stgs_menu[1] = 
  {
    {
      "FatFs Info      ", &fatfs_info,       &stgs_menu[0], &main_menu[2], &stgs_menu[0],                    &stgs_menu[0]
    },
  };


static FATFS FileSystemObj;

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
              (void *) param,
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
  uint8_t key = KEY_NULL;
  uint8_t i = 0;

  
  vTaskDelay(DLY_1SEC);
  display_str("befree", 6, 15);
  vTaskDelay(DLY_1SEC);

  for (i = 0; ; i++)
    {
      time2str(buff, rtt_get_time(), FMT_ISO_8601);
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
  menu_struct_t *p_menu = NULL;
  uint8_t key = KEY_NULL;
  uint8_t ent = 0;
  uint8_t esc = 0;


  p_menu = &main_menu[0];
  display_str(p_menu->name, 0, 15);
  while (1)
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
      if (KEY_NULL != key)
        {
          display_str(p_menu->name, 0, 15);
          key = KEY_NULL;
        }
      vTaskDelay(DLY_200MS);
    }
}


//---------------------------------------------------------------------------------------
// System menus.
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Test SPI interface.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
test_spi (void)
{
  switch (sdc_init())
    {
      case SDC_MRC_INIT_OK:
        display_str("SPI INIT OK     ", 0, 15);
        break;

      case SDC_MRC_INIT_ERR:
      default:
        display_str("SPI INIT ERR    ", 0, 15);
        break;
    }
  vTaskDelay(DLY_1SEC); 
}


//---------------------------------------------------------------------------------------
// Test file system.
// File system expects file called test.txt to be on the SD card. It then open that file
// and displays first 16 characters on the display.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
test_filesystem (void)
{
  FRESULT ret;
  FIL FileObject;
  uint8_t buff[20];
  uint32_t num_b_rd = 0;


  ret = f_mount(1, &FileSystemObj);
  if (FR_OK == ret)
    {
      display_str("Drive Mounted   ", 0, 15);
      vTaskDelay(DLY_1SEC);
      ret = f_open(&FileObject, "1:test.txt", FA_READ);
      if (FR_OK == ret)
        {
          sprintf(buff, "No data         ");
          vTaskDelay(DLY_1SEC);
          if (FR_OK == f_read(&FileObject, buff, 16, &num_b_rd))
            {
              display_str(buff, 0, 15);
            }
          else
            {
              display_str("File not read   ", 0, 15);
            }

        }
      else
        {
          sprintf(buff, "FileOpErr:%d     ", ret);
          display_str(buff, 0, 15);
        }
    }
  else
    {
      sprintf(buff, "Not mnt:%d       ", ret);
      display_str(buff, 0, 15);
    }

  vTaskDelay(DLY_1SEC);
}


//---------------------------------------------------------------------------------------
// Display FatFs info: SD card size, free space.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
fatfs_info (void)
{
  display_str("Changs FatFs   ", 0, 15);
  vTaskDelay(DLY_1SEC);
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
  antsh_cmd("xxx\x0d");
  vTaskDelay(DLY_2SEC); 
  antsh_cmd("mount 1\x0d");
  vTaskDelay(DLY_2SEC); 
  antsh_cmd("ls\x0d");
  vTaskDelay(DLY_2SEC); 
}


//---------------------------------------------------------------------------------------
// Test file system.
// File system expects file called test.txt to be on the SD card. It then open that file
// and displays first 16 characters on the display.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static inline void
show_menu (char_t *name)
{
  display_str(name, 0, 15);
}

