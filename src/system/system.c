//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : system.c - entry point to the system.
// Date    : 29/09/2009 23:18
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


#ifndef _SYSTEM_C_
#define _SYSTEM_C_

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "AT91SAM7S64.h"

#include "types.h"
#include "system.h"

#include "display.h"
#include "keypad.h"
#include "gui.h"
#include "rtt.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define LED_A_ON            (AT91C_BASE_PIOA->PIO_SODR = 0x00000004)
#define LED_A_OFF           (AT91C_BASE_PIOA->PIO_CODR = 0x00000004)


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


void AT91F_LowLevelInit (void);
static void prvSetupHardware (void);
static void pio_init (void);


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Entry to the system.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
int
main (void)
{
  uint32_t i;


  prvSetupHardware();                                                                     // Setup the ports.
  pio_init();	                                                                          // Initialize pio.

  LED_A_OFF;                                                                              // Signal power-ip sequence.
  for (i = 0; i < 100000; i++);
  LED_A_ON;
  for (i = 0; i < 100000; i++);
  LED_A_OFF;
  for (i = 0; i < 100000; i++);
  LED_A_ON;
  for (i = 0; i < 100000; i++);
  LED_A_OFF;


  display_init();
  keypad_init();
  rtt_init();
  gui_init();                                                                             // GUI can run after hardware initialization.

  vTaskStartScheduler();                                                                  // Start FreeRTOS.

  while (1)
    {
      sys_rst();                                                                          // Reset if code gets here.
    }

  return 0;
}


//---------------------------------------------------------------------------------------
// Hardware initialization.
// This function is a part of FreeRTOS project. 
//---------------------------------------------------------------------------------------

static void
prvSetupHardware (void)
{
  // When using the JTAG debugger the hardware is not always initialised to
  // the correct default state.  This line just ensures that this does not
  // cause all interrupts to be masked at the start.
  AT91C_BASE_AIC->AIC_EOICR = 0;

  // Disable watchdog.
  AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

  // Configure CKGR (Clock Generator).
  AT91C_BASE_CKGR->CKGR_MOR = AT91C_CKGR_MOSCEN;                                         // Enable main clock.

  // Configure PMC (Power Management Controller).
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;                                         // Power peripheral clock.
  AT91C_BASE_PMC->PMC_MCKR = AT91C_PMC_CSS_MAIN_CLK;                                     // Power main clock.
}


//---------------------------------------------------------------------------------------
// Low level clk initialization.
// This function is a part of FreeRTOS project.
//---------------------------------------------------------------------------------------
void
AT91F_LowLevelInit(void)
{
 AT91PS_PMC     pPMC = AT91C_BASE_PMC;

  //* Set Flash Waite sate
  //  Single Cycle Access at Up to 30 MHz, or 40
  //  if MCK = 47923200 I have 50 Cycle for 1 useconde ( flied MC_FMR->FMCN
  AT91C_BASE_MC->MC_FMR = ((AT91C_MC_FMCN)&(75 <<16)) | AT91C_MC_FWS_1FWS ;

  //* Watchdog Disable
  AT91C_BASE_WDTC->WDTC_WDMR= AT91C_WDTC_WDDIS;

  //* Set MCK at 47 923 200
  // 1 Enabling the Main Oscillator:
  // SCK = 1/32768 = 30.51 uSeconde
  // Start up time = 8 * 6 / SCK = 56 * 30.51 = 1,46484375 ms
  pPMC->PMC_MOR = ((( AT91C_CKGR_OSCOUNT & (0x06 <<8)) | AT91C_CKGR_MOSCEN ));

  // Wait the startup time
  while(!(pPMC->PMC_SR & AT91C_PMC_MOSCS));

  // 2 Checking the Main Oscillator Frequency (Optional)
  // 3 Setting PLL and divider:
  // - div by 5 Fin = 3,6864 =(18,432 / 5)
  // 95,8464 =(3,6864 *26)
  // for 96 MHz the erroe is 0.16%
  //eld out NOT USED = 0 Fi
  pPMC->PMC_PLLR = ((AT91C_CKGR_DIV & 5) |
                   (AT91C_CKGR_PLLCOUNT & (28<<8)) |
                   (AT91C_CKGR_MUL & (25<<16)));

  // Wait the startup time
  while(!(pPMC->PMC_SR & AT91C_PMC_LOCK));

  // 4. Selection of Master Clock and Processor Clock
  // select the PLL clock divided by 2
  pPMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2 ;
  while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

  pPMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK ;
  while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));
}


//---------------------------------------------------------------------------------------
// Perform system reset.
//
// Arguments:
// N/A
//
// Reset:
// N/A
//---------------------------------------------------------------------------------------
void
sys_rst (void)
{
  taskDISABLE_INTERRUPTS();
  while (1);
}


//---------------------------------------------------------------------------------------
//
//                         L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Initialize PIO.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
pio_init (void)
{
  AT91C_BASE_PIOA->PIO_PER  = (1 << 2);
  AT91C_BASE_PIOA->PIO_OER  = (1 << 2);
}


#endif
