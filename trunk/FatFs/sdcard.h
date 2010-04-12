//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : sdcard.h - header file for SD card driver.
// Date    : 19/12/2009 00:51
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


#ifndef _SDCARD_H_
#define _SDCARD_H_


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


// SD card module return code.
typedef enum
  {
    SDC_MRC_INIT_OK  = 0,
    SDC_MRC_INIT_ERR = 1,
    SDC_MRC_NOT_RDY  = 2,
    SDC_MRC_RD_OK    = 3,
    SDC_MRC_RD_ERR   = 4,
    SDC_MRC_WR_OK    = 5,
    SDC_MRC_WR_ERR   = 6
  } sdc_mrc_t;


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


sdc_mrc_t sdc_init (void);
sdc_mrc_t sdc_stat (void);
sdc_mrc_t sdc_rd (uint8_t *buff, uint32_t sector, uint8_t count);
sdc_mrc_t sdc_wr (const uint8_t *buff, uint32_t sector, uint8_t count);
sdc_mrc_t sdc_ioctl (uint8_t ctrl, void *buff);


#endif
