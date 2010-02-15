//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : system.h - system header file.
// Date    : 29/09/2009 23:09
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


#ifndef SYSTEM_H
#define SYSTEM_H


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "./RTOS/portable/AT91SAM7S64.h"
#include "./RTOS/portable/portmacro.h"
#include "./RTOS/FreeRTOSConfig.h"
#include "display.h"
#include "./types.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define DLY_1MS                         (   1 / portTICK_RATE_MS)
#define DLY_2MS                         (   2 / portTICK_RATE_MS)
#define DLY_10MS                        (  10 / portTICK_RATE_MS)
#define DLY_100MS                       ( 100 / portTICK_RATE_MS)
#define DLY_200MS                       ( 200 / portTICK_RATE_MS)
#define DLY_500MS                       ( 500 / portTICK_RATE_MS)
#define DLY_1SEC                        (1000 / portTICK_RATE_MS)
#define DLY_2SEC                        (DLY_1SEC * 2)
#define DLY_5SEC                        (DLY_1SEC * 5)

#define Q_WAIT_100MS                    DLY_100MS

#define M_WAIT_1SEC                     DLY_1SEC


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


void delay_us (uint32_t delay);
void sys_rst (void);


#endif

