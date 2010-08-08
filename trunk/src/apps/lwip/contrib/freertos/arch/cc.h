//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : cc.h - header file for lwIP port.
// Date    : 18/05/2010 22:48
// Website : http://www.bugtraker.pl 
//
// BeFree - Copyright (C) 2009,2010 BugTraker (http://www.bugtraker.pl).
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


#ifndef CC_H
#define CC_H
 

//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "stream.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------



typedef char c8_t;
typedef unsigned char u8_t;
typedef unsigned short int u16_t;
typedef unsigned int u32_t;
typedef signed char s8_t;
typedef signed short int s16_t;
typedef signed int s32_t;
typedef u32_t mem_ptr_t;


#define BYTE_ORDER                      BIG_ENDIAN
#define LWIP_CHKSUM_ALGORITHM           2

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_FIELD(x)            x
#define PACK_STRUCT_STRUCT              __attribute__((packed))
#define PACK_STRUCT_END

#define LWIP_PLATFORM_ASSERT(x)         printf("%s", x)


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


#endif
