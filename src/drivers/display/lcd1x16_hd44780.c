//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : lcd1x16_hd44780.c - source of lcd 1x16 driver.
// Date    : 29/09/2009 23:36
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


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "types.h"
#include "system.h"

#include "lcd1x16_hd44780.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// LCD Command Control Codes.
//---------------------------------------------------------------------------------------


// Initialize by Instruction.
#define LCD_INIT_8BIT_MODE              0x3F                                              // Initializing 8-bit mode by instruction.


// Clear display - Command (0x01)
#define LCD_CLR_DPY                     0x01                                              // Clear display.


// Display and cursor home - Command (0x02)
#define LCD_DPY_CRSR_HOME               0x02                                              // Sets the display and cursor home.


// Character Entry Mode - Command (0x04)
#define LCD_CHR_ENT_MODE                0x04                                              // Character Entry Mode command.
#define LCD_INC                         0x02                                              // LCD increment DDRAM address.


// Display and Cursor - Command (0x08)
#define LCD_CRSR_BLNK                   0x01
#define LCD_CRSR_UNDERLN                0x02
#define LCD_DPY_ON                      0x04                                              // Display on.
#define LCD_DPY_AND_CRSR                0x08                                              // Display and Cursor command.


// Cursor or display shift - Command (0x10)
#define LCD_CRSR_DPY_SHIFT              0x10                                              // LCD cursor or display shift.
#define LCD_SHIFT_RIGHT                 (1 << 2)                                          // LCD shift right.
#define LCD_SHIFT_LEFT                  (0 << 2)                                          // LCD shift left.
#define LCD_DPY_SHIFT                   (1 << 3)                                          // LCD display shift.
#define LCD_CRSR_SHIFT                  (0 << 3)                                          // LCD cursor shift.


// Function Set - Command (0x20)
#define LCD_FUN_SET                     0x20                                              // Function set command.
#define LCD_8BIT_ITF                    0x10                                              // Initializing 8-bit interface.
#define LCD_2LN_MODE                    0x08                                                        // Initializing 2-line mode.


// Set CGRAM address. - Command (0x40)
#define LCD_SET_CGRAM_ADDR              0x40                                              // Set cgram address to introduce user character.


// Set Display Address - Command (0x80)
#define LCD_SET_DPY_ADDR                0x80                                              // Indicate that DDRAM addres is to be set.
#define LCD_COL0                        0x00                                              // Initial col0 address.
#define LCD_COL8                        0x40                                              // Initial col8 address.
#define LCD_ADDR_LAST_8B                0x60                                              // Set address 0x60.


//---------------------------------------------------------------------------------------
// Display interface (use of pins).
//---------------------------------------------------------------------------------------


// Display pins.
#define LCD_RW                          0x00000100
#define LCD_EN                          0x00000200
#define LCD_RS                          0x00000400
#define LCD_D0                          0x01000000
#define LCD_D1                          0x02000000
#define LCD_D2                          0x04000000
#define LCD_D3                          0x08000000
#define LCD_D4                          0x10000000
#define LCD_D5                          0x20000000
#define LCD_D6                          0x40000000
#define LCD_D7                          0x80000000


// Bus management.
#define LCD_BUS_ENA         (AT91C_BASE_PIOA->PIO_PER  = LCD_EN | LCD_RS | LCD_RW | \
                                                         LCD_D0 | LCD_D1 | LCD_D2 | \
                                                         LCD_D3 | LCD_D4 | LCD_D5 | \
                                                         LCD_D6 | LCD_D7 )
#define LCD_BUS_CFG         (AT91C_BASE_PIOA->PIO_OER  = LCD_EN | LCD_RS | LCD_RW | \
                                                         LCD_D0 | LCD_D1 | LCD_D2 | \
                                                         LCD_D3 | LCD_D4 | LCD_D5 | \
                                                         LCD_D6 | LCD_D7 )
#define LCD_BUS_LO          (AT91C_BASE_PIOA->PIO_CODR = LCD_EN | LCD_RS | LCD_RW | \
                                                         LCD_D0 | LCD_D1 | LCD_D2 | \
                                                         LCD_D3 | LCD_D4 | LCD_D5 | \
                                                         LCD_D6 | LCD_D7 )
#define LCD_DATA_BUS(data)  (AT91C_BASE_PIOA->PIO_SODR = (((unsigned int)data) << 24 ))
#define LCD_EN_HI           (AT91C_BASE_PIOA->PIO_SODR = LCD_EN)                         // EN pin hi.
#define LCD_EN_LO           (AT91C_BASE_PIOA->PIO_CODR = LCD_EN)                         // EN pin low - LCD data lines latch.
#define LCD_RS_HI           (AT91C_BASE_PIOA->PIO_SODR = LCD_RS)                         // RS pin hi (used in data mode).
#define LCD_RS_LO           (AT91C_BASE_PIOA->PIO_CODR = LCD_RS)                         // RS pin low (used in cmd mode).
#define LCD_RW_HI           (AT91C_BASE_PIOA->PIO_SODR = LCD_RW)                         // RW pin hi.
#define LCD_RW_LO           (AT91C_BASE_PIOA->PIO_CODR = LCD_RW)                         // RW pin low.


// Other
#define LCD_SIZE                        16U                                               // Display length.
#define LCD_DDRAM_SIZE                  0x28                                              // Size of lcd cache.
#define LCD_QUEUE_LEN                   2U                                                // Number of cached LCDs.
#define LCD_COL8_OFFSET                 8U                                                // Offset used when 'start' starts from column 8.

// CGRAM data.
#define CGRAM_CHR_SIZE                  8U                                                // Size of a char in CGRAM (num lines).
#define CGRAM_NUM_CHRS                  8U                                                // Number of charasters in CGRAM.


// CGRAM symbols.
#define CGRAM_SYM_PLAY                  0U                                                // CGRAM address of a play symbol.
#define CGRAM_SYM_PAUSE                 1U                                                // CGRAM address of a pause symbol.
#define CGRAM_SYM_STOP                  2U                                                // CGRAM address of a stop symbol.
#define CGRAM_SYM_REV_PLAY              3U                                                // CGRAM address of a rev. play symbol.
#define CGRAM_SYM_REV_PLAY_BAR          4U                                                // CGRAM address of a rev. play with a bar symbol.
#define CGRAM_SYM_PLAY_BAR              5U                                                // CGRAM address of a play with a bar symbol.


// LCD state definitions.
#define xSTS_UPDATE_LCD                 0x00000001                                        // Display is to be updated.
#define xSTS_SCRL_INFT                  0x00000002                                        // Display is scrolling.


// LCD stcrol status


// LCD FSM definitions.
#define FSM_LCD_IDLE                    0U                                                // LCD is idle.
#define FSM_CFG_LCD                     1U                                                // Configure LCD.
#define FSM_DISP_STR                    2U                                                // Display string on LCD.
#define FSM_SCRL_STR                    3U                                                // Scroll string on LCD.
#define FSM_CRSR_MOV                    4U                                                // Move cursor.


//---------------------------------------------------------------------------------------
//
//                          L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


static void handle_lcd (void);
static void cfg_lcd (void);
static void disp_str (void);
static void scrl_str (void);
static void mv_to (void);
static void lcd_idle (void);

static void lcd_init (void);
static void cgram_init (void);
static void hardware_init (void);
static void send_data (char_t data);
static void send_cmd (char_t cmd);


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


typedef struct
  {
    uint32_t cfg;                                                                         // LCD configuration.
    uint32_t sts;                                                                         // Status of active lcd.
    uint8_t  buff[LCD_DDRAM_SIZE];                                                        // String to display.
    uint8_t  num_chars;                                                                   // Number of characters to display.
    uint8_t  start;                                                                       // LCD start position.
    uint8_t  end;                                                                         // LCD end position.
    uint8_t  fsm;                                                                         // Fsm for contrilling LCD.
  } lcd_t;


typedef struct
  {
    uint8_t start;                                                                        // Start position for scroll.
    uint8_t end;                                                                          // End position for scroll.
    uint8_t head;                                                                         // Buffer head.
    uint32_t spd;                                                                         // Scroll speed.
    uint8_t cfg;                                                                          // Scroll configuration.
    uint8_t num_chars;                                                                    // Number of characters in the buffer.
    uint8_t fl_pos;                                                                       // Specifies position of first letter from buffer.
    char_t  buff[LCD_DDRAM_SIZE];                                                         // Buffer containing a string.
  } scrl_win_t;


typedef struct
  {
    uint8_t addr;                                                                         // CGRAM address to program.
    uint8_t chr[CGRAM_CHR_SIZE];                                                          // New character data.
  } cgram_cfg_t;


static lcd_t lcd_act;
static lcd_t lcd_cache;
static scrl_win_t scrl_win;
static xQueueHandle lcd_q_hdle;
static const cgram_cfg_t cgram_cfg[CGRAM_NUM_CHRS] = 
  {
    {
      LCD_SET_CGRAM_ADDR,
      {
        0x18, 0x1C, 0x1E, 0x1F, 0x1E, 0x1C, 0x18, 0x00                                    // Play symbol.
      }                                    
    },

    {
      LCD_SET_CGRAM_ADDR | 0x08,
      {
        0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x00                                    // Pause symbol.
      }                                    
    },

    {
      LCD_SET_CGRAM_ADDR | 0x10,
      {
        0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00                                    // Stop symbol.
      }  
    },

    {
      LCD_SET_CGRAM_ADDR | 0x18,
      {
        0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00                                    // Reverse play symbol.
      }  
    },

    {
      LCD_SET_CGRAM_ADDR | 0x20,
      {
        0x11, 0x13, 0x17, 0x1F, 0x17, 0x13, 0x11, 0x00                                    // Reverse play with a bar symbol.
      }  
    },

    {
      LCD_SET_CGRAM_ADDR | 0x28,
      {
        0x11, 0x19, 0x1D, 0x1F, 0x1D, 0x19, 0x11, 0x00                                    // Play with a bar symbol.
      }  
    },

    {
      LCD_SET_CGRAM_ADDR | 0x30,
      {
        0x01, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x00
      }  
    },

    {
      LCD_SET_CGRAM_ADDR | 0x38,
      {
        0x01, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x00
      }  
    }

  };


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// LCD task responsible for receiving new data from the queue. Task receives from the 
// queue only if it is currently in idle mode. Before receiving from the queue a bkp
// of status is done, as status contains information about pending actions on LCD.
// If new data is received from the queue then fsm is set to call configure lcd. Fsm has
// always IDLE mode after any action, this ensures handling new data from the queue.
//
// Arguments:
// N/A
//
// Retpurn:
// N/A
//---------------------------------------------------------------------------------------
void
lcd_tsk (void)
{
  lcd_t *p_lcd = &lcd_act;
  uint8_t sts;


  lcd_init();
  p_lcd->fsm = FSM_LCD_IDLE;                                                              // Initialize active lcd as IDLE.
  lcd_q_hdle = xQueueCreate(LCD_QUEUE_LEN, sizeof(lcd_act));

  if (0 == lcd_q_hdle)
    {
      sys_rst();
    }

  while (1)
    {
      if (FSM_LCD_IDLE == p_lcd->fsm)                                                     // Try to receive from queue if in idle mode.
        {
          sts = p_lcd->sts;                                                               // Backup status.
          if (pdPASS == xQueueReceive(lcd_q_hdle, (void *) p_lcd, Q_WAIT_100MS))          // Receive from the queue.
            {
              p_lcd->fsm = FSM_CFG_LCD;                                                   // Fsm to configure lcd.
            }
          p_lcd->sts = sts;                                                               // Recover status.
        }
      handle_lcd();                                                                       // Handling lcd state.
      vTaskDelay(DLY_10MS);
    }
}


//---------------------------------------------------------------------------------------
// Display data on the LCD display. 
//
// Arguments:
// char_t   *str  - pointer to message to be displayed.
// uint8_t  start - where the scroll starts. 
// uint8_t  end   - where the scroll stops.
// uint32_t cfg   - configures the display.
//
// Return:
// lcd_mrc_t ret  - lcd module return code: invalid parameter(s), lcd is busy or OK.
//---------------------------------------------------------------------------------------
lcd_mrc_t
lcd_printf (char_t *str, uint8_t start, uint8_t end, uint32_t cfg)
{
  lcd_t     *p_lcd = &lcd_cache;
  lcd_mrc_t  ret   = LCD_MRC_PARAM_INV;
  uint8_t    i;


  if ((start < end) && (end < LCD_SIZE) && (str != NULL))                                 // Check parameters.
    {
      ret          = LCD_MRC_BUSY;                                                        // Assume that lcd is busy (2 scrs to display).
      p_lcd->start = start;
      p_lcd->end   = end;                                                                 // Where to end.
      p_lcd->cfg   = cfg;                                                                 // Copy configuration settings.
      for (i = 0; i < LCD_DDRAM_SIZE; i++)
        {
          if (0 == *str)                                                                  // Stop copying when 0 is found.
            {
              break;
            }
          p_lcd->buff[i] = *str++;                                                        // Copy data to the buffer.
        }
      p_lcd->num_chars = i;
      if (pdPASS == xQueueSendToBack(lcd_q_hdle, (void *) p_lcd, Q_WAIT_100MS))           // Check if inactive lcd can be updated.
        {
          ret = LCD_MRC_OK;                                                               // Function successful.
        }
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// Display character on the display.
//
// Arguments:
// char_t chr - character to display.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
lcd_putchar (void *ptr, char_t chr)
{
  lcd_t *p_lcd = &lcd_cache;


  p_lcd->cfg     = xCFG_DPY_CHR;                                                          // Configure display to display one character.
  p_lcd->buff[0] = chr;
  xQueueSendToBack(lcd_q_hdle, (void *) p_lcd, Q_WAIT_100MS);                             // Wait for queue to be free.
}


//---------------------------------------------------------------------------------------
// Move cursor to given position (0 to 15).
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
lcd_mrc_t
lcd_crsr_mv (uint8_t pos)
{
  lcd_t     *p_lcd = &lcd_cache;
  lcd_mrc_t  ret   = LCD_MRC_PARAM_INV;

  
  if (pos < LCD_SIZE)
    { 
      ret          = LCD_MRC_BUSY;                                                        // Initialize with busy.
      p_lcd->start = pos;
      p_lcd->cfg   = xCFG_CRSR_MOV;
      if (pdPASS == xQueueSendToBack(lcd_q_hdle, (void *) p_lcd, Q_WAIT_100MS))           // Check if inactive lcd can be updated.
        {
          ret = LCD_MRC_OK;                                                               // Function successful.
        }
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// Clear display, and set DDRAM address to 0x00.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
lcd_mrc_t
lcd_clr_dpy (void)
{
  lcd_mrc_t  ret   = LCD_MRC_BUSY;
  lcd_t     *p_lcd = &lcd_cache;


  p_lcd->cfg = xCFG_CLR_DPY;                                                      // Configure the display to be cleared.
  if (pdPASS == xQueueSendToBack(lcd_q_hdle, (void *) p_lcd, Q_WAIT_100MS))
    {
      ret = LCD_MRC_OK;                                                           // Function successful.
    }
  return (ret);
}

//---------------------------------------------------------------------------------------
//
//                           L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Handle LCD state.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
handle_lcd (void)
{
  lcd_t *p_lcd = &lcd_act;

  switch (p_lcd->fsm)
    {
      case FSM_CFG_LCD:
        cfg_lcd();
        break;

      case FSM_DISP_STR:
        disp_str();
        break;

      case FSM_SCRL_STR:
        scrl_str();
        break;

      case FSM_CRSR_MOV:
        mv_to();
        break;

      case FSM_LCD_IDLE:
        lcd_idle();
        break;

      default:
        break;
    }
}


//---------------------------------------------------------------------------------------
// Configure lcd display as indicated in given parameter. 
// Few actions can be performed:
// - clear
// - move cursor
// - display string
// - scroll string
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
cfg_lcd (void)
{
  lcd_t           *p_lcd = &lcd_act;
  scrl_win_t *p_scrl_win = &scrl_win;
  uint8_t  i;


  if (0 != (p_lcd->cfg & xCFG_CLR_DPY))                                                   // Check if LCD is to be cleared.
    {
      send_cmd(LCD_CLR_DPY);                                                              // Clear display.
      p_lcd->sts &= ~xSTS_SCRL_INFT;                                                      // Clear status from infinite scroll.
      p_lcd->fsm  = FSM_LCD_IDLE;                                                         // Go to idle mode.
    }
  else if (0 != (p_lcd->cfg & xCFG_CRSR_MOV))                                             // Check for cursor move.
    {
      p_lcd->fsm = FSM_CRSR_MOV;                                                          // Go to cursor move mode.
    }
  else
    {
      if (0 != (p_lcd->cfg & xCFG_SCRL_STR))                                              // Check if display is to be scrolled and copy
        {                                                                                 // needed settings.
          for (i = 0; i < p_lcd->num_chars; i++)                                          // Copy all characters to scroll buffer.
            {
              p_scrl_win->buff[i] = p_lcd->buff[i];
            }
          p_scrl_win->start     = p_lcd->start;                                           
          p_scrl_win->end       = p_lcd->end;                                             
          p_scrl_win->num_chars = p_lcd->num_chars;
          p_scrl_win->spd       = DLY_200MS;
          p_scrl_win->fl_pos    = p_scrl_win->start;                                      // Set first letter position to start.
          p_scrl_win->cfg       = p_lcd->cfg & (xCFG_SCRL_RIGHT | xCFG_SCRL_LEFT);        // Copy scroll direction.
          if (0 != (p_lcd->cfg & xCFG_SCRL_FAST))                                         // Check if scroll is fast.
            {
              p_scrl_win->spd = DLY_500MS;
            }
          p_scrl_win->head = 0;                                                           // Reset head for buffer.
          p_lcd->fsm       = FSM_SCRL_STR;                                                // Go to scroll string mode.
          p_lcd->sts      |= xSTS_SCRL_INFT;                                              // Scroll infinitly.
        }
      else                                                                                // String is to be displayed.
        {
          p_lcd->fsm = FSM_DISP_STR;                                                      // Go to display string.
        }
    }
}


//---------------------------------------------------------------------------------------
// Displays buffer on the LCD. All data needed are held in lcd structure.
//
// Arguments:
// N/A
// 
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
disp_str (void)
{
  uint8_t  i;
  lcd_t   *p_lcd = &lcd_act;

  
  if (p_lcd->start >= (LCD_SIZE / 2))                                                     // Check if start is above column 8.
    {
      send_cmd(LCD_SET_DPY_ADDR | LCD_COL8 | (p_lcd->start - LCD_COL8_OFFSET));           // Reconfiugre start address to higher part.
    }
  else                                                                                    // Lower half of the display.
    {
      send_cmd(LCD_SET_DPY_ADDR | p_lcd->start);                                          // Reconfigure start address to lower part.
    }
  for (i = 0; i < LCD_SIZE; i++)
    {
      if (i == p_lcd->num_chars)                                                          // Check if this is a last character.
        {
          break;
        }
      if ((LCD_SIZE / 2) == (p_lcd->start + i))                                           // Check if LCD hast to be reconfigured to display
        {                                                                                 // at higher part.
          send_cmd(LCD_SET_DPY_ADDR | LCD_COL8); 
        }
      send_data(p_lcd->buff[i]);                                                          // Send character.
    }
  p_lcd->fsm = FSM_LCD_IDLE;                                                              // Go to idle mode.
}


//---------------------------------------------------------------------------------------
// Scroll string.
// 
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
scrl_str (void)
{
  lcd_t      *p_lcd      = &lcd_act;
  scrl_win_t *p_scrl_win = &scrl_win;
  uint8_t i;
  uint8_t idx;


  if (p_scrl_win->start >= (LCD_SIZE / 2))
    {
      send_cmd(LCD_SET_DPY_ADDR | LCD_COL8 | (p_scrl_win->start - LCD_COL8_OFFSET));
    }
  else
    {
      send_cmd(LCD_SET_DPY_ADDR | p_scrl_win->start);
    }
  for (i = 0; i < LCD_SIZE; i++)
    {
      if (i > (p_scrl_win->end - p_scrl_win->start))
        {
          break;
        }
      if ((LCD_SIZE / 2) == (p_scrl_win->start + i))
        {
          send_cmd(LCD_SET_DPY_ADDR | LCD_COL8); 
        }
      idx  = p_scrl_win->head + i;
      idx %= p_scrl_win->num_chars;
      if (((p_scrl_win->end + i) - p_scrl_win->fl_pos) < (p_scrl_win->num_chars - 1))
        {
          send_data(p_scrl_win->buff[((p_scrl_win->end + i) - p_scrl_win->fl_pos) + 1]);
        }
      else if ((i < p_scrl_win->fl_pos) || (i >= (p_scrl_win->fl_pos + p_scrl_win->num_chars)))
        {
          send_data(0x20);
        }
      else
        {
          send_data(p_scrl_win->buff[idx]);
        }
    }
  if (0 != (p_scrl_win->cfg & xCFG_SCRL_RIGHT))
    {
      ++p_scrl_win->head;
      if (p_scrl_win->head >= p_scrl_win->num_chars)
        {
          p_scrl_win->head = 0;
        }
    }
  else
    {
      if (p_scrl_win->head > 0)
        {
          --p_scrl_win->head;
        }
      else
        {
          p_scrl_win->head = p_scrl_win->num_chars - 1;
        } 
    }
  ++p_scrl_win->fl_pos;
  if (p_scrl_win->fl_pos > p_scrl_win->end)
    {
      p_scrl_win->fl_pos = p_scrl_win->start;
      p_scrl_win->head   = 0;
    }
  p_lcd->fsm = FSM_LCD_IDLE;
  vTaskDelay(p_scrl_win->spd);
}


//---------------------------------------------------------------------------------------
// Move cursor to given position.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
mv_to (void)
{
  lcd_t   *p_lcd = &lcd_act;
  uint8_t  i;


  send_cmd(LCD_DPY_CRSR_HOME);                                                            // Set cursor to HOME postion.
  for (i = 0; i < p_lcd->start; i++)                                                      // Scroll cursor to given position.
    {
      send_cmd(LCD_CRSR_DPY_SHIFT | LCD_CRSR_SHIFT | LCD_SHIFT_RIGHT);
    }
  p_lcd->fsm = FSM_LCD_IDLE;                                                              // Go to idle mode.
}


//---------------------------------------------------------------------------------------
// LCD idle handling. However, status is checked so that FSM can be manipulated for
// further actions.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
lcd_idle (void)
{
  lcd_t *p_lcd = &lcd_act;

  
  if (0 != (p_lcd->sts & xSTS_SCRL_INFT))                                                 // Check for pending scroll.
    {
      p_lcd->fsm = FSM_SCRL_STR;                                                          // Go to scroll string mode.
    }
}


//---------------------------------------------------------------------------------------
//
//                           H A R D W A R E   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Initialize LCD (1x16).
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
lcd_init (void)
{
  hardware_init();                                                                        // Confiugre hardware bus. 
  send_cmd(LCD_INIT_8BIT_MODE);                                                           // LCD in 8bit mode - 1st time.  
  send_cmd(LCD_INIT_8BIT_MODE);                                                           // LCD in 8bit mode - 2nd time.
  send_cmd(LCD_INIT_8BIT_MODE);                                                           // LCD in 8bit mode - 3rd time.
  send_cmd(LCD_FUN_SET | LCD_8BIT_ITF | LCD_2LN_MODE);                                    // Function set, lcd in 2line mode.
  send_cmd(LCD_DPY_AND_CRSR | LCD_DPY_ON | LCD_CRSR_BLNK);                                                // Display on, cursor on.
  send_cmd(LCD_CHR_ENT_MODE | LCD_INC);                                                   // Character entry mode, increment.
  cgram_init();                                                                           // Initialize CGRAM with user defined charasters.
}


//---------------------------------------------------------------------------------------
// Initialize CGRAM, user defined characters.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
cgram_init (void)
{
  cgram_cfg_t const *p_cgram_cfg = cgram_cfg;
  uint8_t i;
  uint8_t j;

 
  for (i = 0; i < CGRAM_NUM_CHRS; i++)                                                    // Load all new CGRAM characters.
    {
      send_cmd(p_cgram_cfg->addr);                                                        // Configure addres for new character.
      for (j = 0; j < CGRAM_CHR_SIZE; j++)
        {
          send_data(p_cgram_cfg->chr[j]);                                                 // Load data for new character.
        }
      ++p_cgram_cfg;                                                                      // Move to next character.
    }
}


//---------------------------------------------------------------------------------------
// Configure LCD bus.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
hardware_init (void)
{
  LCD_BUS_ENA;                                                                           // Enable bus.
  LCD_BUS_CFG;                                                                           // Configure bus (set as IO).
  LCD_BUS_LO;                                                                            // All pins low.
}


//---------------------------------------------------------------------------------------
// Send data to LCD. In this case data is usually bytes (characters) to display.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
send_data (char_t data)
{
  LCD_RS_HI;                                                                             // RS pin low.
  LCD_RW_LO;                                                                             // RW pin low.
  LCD_DATA_BUS(data);                                                                    // Set data on data lines.
  LCD_EN_HI;                                                                             // EN pin hi.
  vTaskDelay(DLY_2MS);                                                                   // Give a time to LCD.
  LCD_EN_LO;                                                                             // EN pin low. This lets the LCD to latch data.
  vTaskDelay(DLY_2MS);                                                                   // Give a time to LCD.
  LCD_BUS_LO;
}


//---------------------------------------------------------------------------------------
// Send commands to LCD. This is used to configure LCD and to use functions.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
send_cmd (char_t data)
{
  LCD_RS_LO;                                                                             // RS pin low.
  LCD_RW_LO;                                                                             // RW pin low.
  LCD_DATA_BUS(data);                                                                    // Set data on data lines.
  LCD_EN_HI;                                                                             // EN pin hi.
  vTaskDelay(DLY_2MS);                                                                   // Give a time to LCD.
  LCD_EN_LO;                                                                             // EN pin low. This lets the LCD to latch data.
  vTaskDelay(DLY_2MS);                                                                   // Give a time to LCD.
  LCD_BUS_LO;
}

