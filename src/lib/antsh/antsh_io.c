//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : antsh_io.c - source file for ANTsh module IO.
// Date    : 29/01/2010 22:31
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


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "types.h"
#include "antsh_conf.h"

#if ANTSH_DEBUG_API == 0
#include "display.h"
#endif


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// ANTsh API to send result to the output. Output is user defined. It may be UART, File,
// ETH, USB etc.
// 
// Arguments:
// const char_t *data   - pointer to shell output
// const uint16_t count - number of bytes in shell output.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void 
antsh_out (const char_t *data, const uint16_t count)
{
  display_str((char_t *)data, 0, 15);
}
