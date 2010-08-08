//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : spi.c - SPI bus driver.
// Date    : 09/12/2009 23:09
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
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#ifndef _SPI_C_
#define _SPI_C_


#include "AT91SAM7S64.h"
#include "types.h"
#include "spi.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                              L O C A L   D A T A
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
// SPI initialization.
//
// Arguments:
// N/A
//
// Return:
// N/A
//
//---------------------------------------------------------------------------------------
void
spi_init (void)
{
  AT91C_BASE_PIOA->PIO_PDR = AT91C_PA11_NPCS0 | AT91C_PA12_MISO |                         // PIO Disable SPI.
                             AT91C_PA13_MOSI  | AT91C_PA14_SPCK;
  AT91C_BASE_PIOA->PIO_ASR = AT91C_PA11_NPCS0 | AT91C_PA12_MISO |                         // PIO Enable Peripheral Function A.
                             AT91C_PA13_MOSI  | AT91C_PA14_SPCK;

  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI;                                           // Enable clock for SPI.

  AT91C_BASE_SPI->SPI_CR     = AT91C_SPI_SWRST;                                           // SPI software reset, enable SPI.
  AT91C_BASE_SPI->SPI_MR     = AT91C_SPI_MSTR | AT91C_SPI_PS_FIXED | AT91C_SPI_MODFDIS;   // SPI: Master mode, PS fixed (cs0).
  AT91C_BASE_SPI->SPI_CSR[0] = (AT91C_SPI_BITS & AT91C_SPI_BITS_8) | 0x08000000 |         // 8 bit mode.
                               (AT91C_SPI_SCBR & 0x00004000) | AT91C_SPI_NCPHA;           // BR: CLK/0xFF. (294kHz).
  AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SPIEN;
}


//---------------------------------------------------------------------------------------
// SPI send byte.
//
// Arguments:
// uint8_t data - byte to send.
//
// Return:
// N/A
//
//---------------------------------------------------------------------------------------

void
spi_wr (uint8_t data)
{
  uint8_t ret;


  while (0 == (AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE));
  AT91C_BASE_SPI->SPI_TDR = data;
  while (0 == (AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF));
  ret = AT91C_BASE_SPI->SPI_RDR;
}


//---------------------------------------------------------------------------------------
// SPI receive byte.
//
// Arguments:
// N/A
//
// Return:
// uint8_t ret - read byte.
//
//---------------------------------------------------------------------------------------


uint8_t
spi_rd (void)
{
  uint8_t ret;


  while (0 == (AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE));
  AT91C_BASE_SPI->SPI_TDR = 0xFF;
  while (0 == (AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF));
  ret = AT91C_BASE_SPI->SPI_RDR;

  return (ret);
}


#endif
