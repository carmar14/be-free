//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : keypad.h - header file.
// Date    : 19/01/2010 22:21
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


#ifndef KEYPAD_H
#define KEYPAD_H


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define KEY_NULL                        0
#define KEY_VOL_UP                      1
#define KEY_VOL_DWN                     2
#define KEY_NEXT                        3
#define KEY_PREV                        4
#define KEY_YES                         5
#define KEY_NO                          6
#define KEY_MENU                        7
#define KEY_F1                          8
#define KEY_F2                          9


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


void keypad_init (void);
uint8_t keypad_getkey (void);


#endif
