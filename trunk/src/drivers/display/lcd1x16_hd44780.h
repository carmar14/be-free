//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : lcd1x16_hd44780.h - header for lcd driver.
// Date    : 29/09/2009 23:38
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


#ifndef LCD_H
#define LCD_H


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "types.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


// LCD configure definitions
#define xCFG_SCRL_STR                   (0x1L << 0)                                       // Scroll string on the display.
#define xCFG_SCRL_BRDR_STOP             (0x1L << 1)                                       // Stop scroll when boarder is reached.
#define xCFG_SCRL_INFT                  (0x1L << 2)                                       // Infinite scroll.
#define xCFG_SCRL_LEFT                  (0x1L << 3)                                       // Scroll to the left.
#define xCFG_SCRL_RIGHT                 (0x0L << 3)                                       // Scroll to the right.
#define xCFG_SCRL_FAST                  (0x1L << 4)                                       // Fast scroll.
#define xCFG_SCRL_SLOW                  (0x0L << 4)                                       // Slow scroll.
#define xCFG_SCRL_CRSR                  (0x1L << 5)                                       // Scroll cursor.
#define xCFG_CLR_DPY                    (0x1L << 6)                                       // Clear screen.
#define xCFG_CRSR_MOV                   (0x1L << 7)                                       // Cursor move to position.
#define xCFG_DPY_CHR                    (0x1L << 8)                                       // Display one character.


// Return 
typedef enum
  {
    LCD_MRC_OK        = 0,
    LCD_MRC_BUSY      = 1,
    LCD_MRC_PARAM_OK  = 2,
    LCD_MRC_PARAM_INV = 3
  } lcd_mrc_t;


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


void lcd_tsk (void);
lcd_mrc_t lcd_printf (char_t *str, uint8_t start, uint8_t end, uint32_t cfg);
void lcd_putchar (void *ptr, char_t chr);
lcd_mrc_t lcd_clr_dpy (void);
lcd_mrc_t lcd_crsr_mv (uint8_t pos);


#endif
