//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : antsh_conf.h - configuration file for ANT shell.
// Date    : 27/01/2010 22:38
// Website : http://www.bugtraker.pl 
//
// ANTsh - Copyright (C) 2009 BugTraker (http://www.bugtraker.pl).
//
// ANTsh is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// ANTsh is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeFree; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------------------


#ifndef ANTSH_CONF_H
#define ANTSH_CONF_H


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


#define ANTSH_CONF_USE_RTOS             0                                                 // ANTsh runs as a task.
#define ANTSH_CONF_USE_RTOS_DLY         0                                                 // ANTsh may use RTOS build in delay API, e.g cooperative RTOS.
#define ANTSH_DEBUG_API                 1                                                 // Enable debugging API.


#if ANTSH_CONF_USE_RTOS == 1 && ANTSH_CONF_USE_RTOS_DLY == 1
#define ANTSH_RTOS_DLY_API              vTaskDelay(1);
#endif

#define ANTSH_CONF_BUFF_IN_SIZE         40U                                               // Size of shell in buffer in bytes.
#define ANTSH_CONF_BUFF_OUT_SIZE        40U                                               // Size of shell out buffer in bytes.


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


#endif

