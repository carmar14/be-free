//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : display.h - header for display application.
// Date    : 29/09/2009 23:45
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


#ifndef DISPLAY_H
#define DISPLAY_H


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "./types.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


// Return values for display module.
typedef enum
  {
    DPY_MRC_OK,
    DPY_MRC_ERR,
    DPY_MRC_TMO
  } dpy_mrc_t;


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


void display_init (void);

dpy_mrc_t display_str (char_t *str, uint8_t start, uint8_t end);
dpy_mrc_t display_clr (void);
dpy_mrc_t display_mv_to (uint8_t pos);
dpy_mrc_t display_scrl (char_t *str, uint8_t start, uint8_t end, uint8_t fast);


#endif
