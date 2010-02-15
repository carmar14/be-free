//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : rtt.c - source of real time timer driver.
// Date    : 26/10/2009 20:55
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


#include "./RTOS/include/FreeRTOS.h"
#include "./RTOS/include/task.h"
#include "./RTOS/include/queue.h"

#include "./types.h"
#include "./system.h"
#include "rtt.h"


//---------------------------------------------------------------------------------------
//
//                               D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define RTT_TIME_BASE                   0UL                                               // Time base for RTT.


//---------------------------------------------------------------------------------------
//
//                          L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static volatile uint32_t rtt_sys_time;


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Initialization of RTT.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
rtt_init (void)
{
  rtt_sys_time = RTT_TIME_BASE;
}


//---------------------------------------------------------------------------------------
// Read system time.
//
// Arguments:
// N/A
//
// Return:
// uint32_t rtt_sys_time - system time.
//---------------------------------------------------------------------------------------
uint32_t
rtt_get_time (void)
{
  return (rtt_sys_time);
}


//---------------------------------------------------------------------------------------
// Read time in FatFs format.
// Currnet time is returned with packed into a DWORD value. The bit field is as follows:
//
// bit31:25
//    Year from 1980 (0..127)
// bit24:21
//    Month (1..12)
// bit20:16
//    Day in month(1..31)
// bit15:11
//    Hour (0..23)
// bit10:5
//    Minute (0..59)
// bit4:0
//    Second / 2 (0..29) 
//
// Arguments:
// N/A
//
// Return:
// uint32_t ret - current time in FatFs format.
//---------------------------------------------------------------------------------------
uint32_t
get_fattime (void)
{
  return (0);
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
// RTT interrupt handler called every 1sec. This RTT is designed to work the same way as
// UNIX time. It has a time base in 01/01/1970 00:00 and counts seconds since then.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
rtt_irq_handler (void)
{
  rtt_sys_time = AT91C_BASE_RTTC->RTTC_RTVR;                                              // Update system timer (clear rtt irq).

  if (0 == (rtt_sys_time % 2))
    {
      AT91C_BASE_PIOA->PIO_CODR = 0x00000004;
    }
  else
    {
      AT91C_BASE_PIOA->PIO_SODR = 0x00000004;
    }
}




