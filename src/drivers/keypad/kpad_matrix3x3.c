//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : kpad_matrix3x3.c - driver for matrix keypad 3x3.
// Date    : 30/09/2009 23:41
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

#include "kpad_matrix3x3.h"

//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define ROW1                            (1 << 18)
#define ROW2                            (1 << 19)
#define ROW3                            (1 << 21)
#define COL1                            (1 << 20)
#define COL2                            (1 << 22)
#define COL3                            (1 << 23)

#define KPAD_COLS_RD                    (AT91C_BASE_PIOA->PIO_PDSR & (COL1 | COL2 | COL3))
#define KPAD_COL1_ACT                   (COL2 | COL3)
#define KPAD_COL2_ACT                   (COL1 | COL3)
#define KPAD_COL3_ACT                   (COL1 | COL2)


#define KPAD_ROW1_PULSE                 {                                                 \
                                          AT91C_BASE_PIOA->PIO_SODR = ROW2 | ROW3;        \
                                          AT91C_BASE_PIOA->PIO_CODR = ROW1;               \
                                        }

#define KPAD_ROW2_PULSE                 {                                                 \
                                          AT91C_BASE_PIOA->PIO_SODR = ROW1 | ROW3;        \
                                          AT91C_BASE_PIOA->PIO_CODR = ROW2;               \
                                        }

#define KPAD_ROW3_PULSE                 {                                                 \
                                          AT91C_BASE_PIOA->PIO_SODR = ROW1 | ROW2;        \
                                          AT91C_BASE_PIOA->PIO_CODR = ROW3;               \
                                        }



#define KPAD_KEY_Q_SIZE                 10
#define KPAD_NUM_ROWS                   3
#define KEY_FLTR_MAX_TIME               30                                                // Maximal time for filtering pressed key.
#define KEY_FLTR_MIN_TIME               5                                                 // Minimal time for filtering pressed key.


//---------------------------------------------------------------------------------------
//
//                          L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


static uint8_t pulse_row1 (void);
static uint8_t pulse_row2 (void);
static uint8_t pulse_row3 (void);

static void hardware_init (void);


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


uint8_t (*kpad_scan[KPAD_NUM_ROWS]) (void) =
  {
    &pulse_row1,
    &pulse_row2,
    &pulse_row3
  };


typedef struct
  {
    xQueueHandle key_q_hdle;                                                              // Keypad keys buffer.
    uint8_t key_fltr_dynt;                                                                // Key dynamic filter timer.
  } kpad_t;

static kpad_t kpad;                                                                       // Keypad data.


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Matrix keypad task is responsible for reading pressed keys. This is done by driving 
// row 1 to 3 low and reading columns 1 to 3. If a key is found then it is filtered to
// prevent multiple reads for one keypress. Initial filter for keypress is set to be
// 300[ms]. However, if key is held, "key dynamic filter timer" is updated to accept key
// 10[ms] faster than the last time. If key is still held down then it will decrease
// filter time by another 10[ms] down to 50[ms]. This feature alowes user to perform 
// faster key reading for key being held down. When key is released, "key dynamic filter
// timer" increases gradually to 300[ms], unless a key is pressed again.
// Filtered key is copied to the keypad buffer, and LIN (last in) index for the key is
// updated. When buffer is full of keys and no read was performed, then the oldest key
// is overwritten (circullar buffer).
// 
// Arguments:
// N/A
//
// Return:
// N/A
//
//---------------------------------------------------------------------------------------
void
kpad_matrix_tsk (void)
{
  kpad_t  *p_kpad  = &kpad;                                                               // Pointer for keypad data.
  uint8_t key      = KPAD_NO_KEY;                                                         // Initialize with no key pressed.
  uint8_t last_key = KPAD_NO_KEY;                                                         // Initialize with no last key pressed.
  uint8_t fltr_tmr;                                                                       // Filter timer initialization
  uint8_t i;


  hardware_init();
  p_kpad->key_q_hdle = xQueueCreate(KPAD_KEY_Q_SIZE, sizeof(uint8_t));                    // Create key queue.
  if (0 == p_kpad->key_q_hdle)
    {
      sys_rst();                                                                          // Reset if queue has not been created.
    }
  p_kpad->key_fltr_dynt = KEY_FLTR_MAX_TIME;                                              // Initialize filter dynamic timer with max time.
  while (1)
    {
      for (i = 0; i < KPAD_NUM_ROWS; i++)                                                 // Activate rows one by one. 
        {
          key = kpad_scan[i]();                                                           // Check if anything is on the column.
          if (key != KPAD_NO_KEY)                                                         // Check received key.
            { 
              if (key != last_key)
                {
                  fltr_tmr = p_kpad->key_fltr_dynt;
                  last_key = key;
                }
            }
        }
      if (fltr_tmr > 0)
        {
          --fltr_tmr;
          if (0 == fltr_tmr)
            {
              if (p_kpad->key_fltr_dynt > KEY_FLTR_MIN_TIME)
                {
                  p_kpad->key_fltr_dynt -= 5;
                }
              xQueueSendToBack(p_kpad->key_q_hdle, (void *) &last_key, 0);
              last_key = KPAD_NO_KEY;
            }
        }
      else
        {
          if (p_kpad->key_fltr_dynt < KEY_FLTR_MAX_TIME)
            {
              ++p_kpad->key_fltr_dynt;
            }
        }
      vTaskDelay(DLY_10MS);
    }
}


//---------------------------------------------------------------------------------------
// Function reads the oldes key in the keypad buffer and updates index for oldest key.
// Arguments:
// N/A
//
// Return:
// uint8_t key - last pressed key.
//
//---------------------------------------------------------------------------------------
uint8_t
kpad_matrix_getkey (void)
{
  kpad_t *p_kpad = &kpad;
  uint8_t key;

  
  if (pdPASS != xQueueReceive(p_kpad->key_q_hdle, (void *) &key, 0))                      // Receive key from the queue.
    {
      key = KPAD_NO_KEY;                                                                  // No key received.
    }
  return (key);
}

//---------------------------------------------------------------------------------------
//
//                           L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------
//
//                           H A R D W A R E   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Scans row1.
//
// Arguments:
// N/A
//
// Return:
// uint8_t key  - pressed key
//---------------------------------------------------------------------------------------
static uint8_t 
pulse_row1 (void)
{
  uint32_t cols_rd;
  uint8_t key;


  KPAD_ROW1_PULSE;
  cols_rd = KPAD_COLS_RD;
  switch (cols_rd)
    {
      case KPAD_COL1_ACT:
        {
          key = KPAD_KEY_VOL_UP;
          break;  
        }

      case KPAD_COL2_ACT:
        {
          key = KPAD_KEY_NEXT;
          break;
        }

      case KPAD_COL3_ACT:
        {
          key = KPAD_KEY_VOL_DWN;
          break;
        }

      default:
        {
          key = KPAD_NO_KEY;
          break;
        }
    }
  return (key);
}


//---------------------------------------------------------------------------------------
// Pulses row2.
//
// Arguments:
// N/A
//
// Return:
// uint8_t key  - pressed key
//---------------------------------------------------------------------------------------
static uint8_t 
pulse_row2 (void)
{
  uint32_t cols_rd;
  uint8_t key;


  KPAD_ROW2_PULSE;
  cols_rd = KPAD_COLS_RD;
  switch (cols_rd)
    {
      case KPAD_COL1_ACT:
        {
          key = KPAD_KEY_F1;
          break;  
        }

      case KPAD_COL2_ACT:
        {
          key = KPAD_KEY_PREV;
          break;
        }

      case KPAD_COL3_ACT:
        {
          key = KPAD_KEY_F2;
          break;
        }

      default:
        {
          key = KPAD_NO_KEY;
          break;
        }
    }
  return (key);
}


//---------------------------------------------------------------------------------------
// Pulses row3.
//
// Arguments:
// N/A
//
// Return:
// uint8_t key  - pressed key
//---------------------------------------------------------------------------------------
static uint8_t 
pulse_row3 (void)
{
  uint32_t cols_rd;
  uint8_t key;


  KPAD_ROW3_PULSE;
  cols_rd = KPAD_COLS_RD;
  switch (cols_rd)
    {
      case KPAD_COL1_ACT:
        {
          key = KPAD_KEY_YES;
          break;  
        }

      case KPAD_COL2_ACT:
        {
          key = KPAD_KEY_MENU;
          break;
        }

      case KPAD_COL3_ACT:
        {
          key = KPAD_KEY_NO;
          break;
        }

      default:
        {
          key = KPAD_NO_KEY;
          break;
        }
    }
  return (key);
}


//---------------------------------------------------------------------------------------
// Initializes matrix keypad hardware (rows and cols).
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
  AT91C_BASE_PIOA->PIO_PER  |= (1 << 18) | (1 << 19) | (1 << 20) | 
                              (1 << 21) | (1 << 22) | (1 << 23);                                    // PIO enable 3cols and 3rows.
  AT91C_BASE_PIOA->PIO_OER  |= (1 << 18) | (1 << 19) | (1 << 21);                                   // Enable 3 outputs.
  AT91C_BASE_PIOA->PIO_ODR  |= (1 << 20) | (1 << 22) | (1 << 23);                                   // Eanble 3 inputs.
  AT91C_BASE_PIOA->PIO_IFER |= (1 << 20) | (1 << 22) | (1 << 23);                                   // Enable input filter.
}
