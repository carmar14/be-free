//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : sdcard.c - SD card driver.
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


#ifndef _SDCARD_C_
#define _SDCARD_C_


#include "AT91SAM7S64.h"
#include "FreeRTOS.h"
#include "task.h"
#include "system.h"
#include "types.h"
#include "display.h"
#include "stream.h"
#include "sdcard.h"
#include "spi.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


#define SDC_RX_BUFF_SIZE                1024U                                             // Size (in bytes) of a SPI rx buffer.
#define CMD_LEN                         6U                                                // LenGth of SPI cmd (bytes).
#define OCR_SIZE                        4U                                                // OCR register size.
#define NUM_TRIES                       10U                                               // Number of tries to receive data.
#define BLK_SIZE                        512U                                              // Size of a sector.

#define NUM_CMDS                        11U                                               // Num supported commands.
#define NUM_CMD_HDLRS                   NUM_CMDS                                          // Number of handlers for commands.
#define CMD0                            0
#define CMD8                            1
#define CMD41                           2
#define CMD55                           3
#define CMD58                           4
#define CMD16                           5
#define CMD17                           6
#define CMD18                           7
#define CMD12                           8
#define CMD24                           9
#define CMD25                           10

// bld_cmd() function opts.
#define xBLD_CMD_USE_ARGS               0x01U                                             // Flag bld_cmd() to use given arguments.

// R1 flags.
#define xR1_IN_IDLE_STATE               0x01U                                             // The card is in idle state.
#define xR1_ERASE_RESET                 0x02U                                             // Erase was cleared before executing.
#define xR1_ILLEGAL_CMD                 0x04U                                             // Last command was illegal.
#define xR1_COM_CRC_ERR                 0x08U                                             // CRC failed.
#define xR1_ERASE_ESQ_ERR               0x10U                                             // Error in sequence of erase cmds.
#define xR1_ADDR_ERR                    0x20U                                             // A misaligned address (blk len did not match).
#define xR1_PARAM_ERR                   0x40U                                             // Command's arguments was invalid.
#define xR1_MSB                         0x80U                                             // Always cleared.

// OCR flags.
#define xOCR_CCS                        0x40U                                             // Card Capacity Status.
#define xOCR_BUSY                       0x80U                                             // Card power up status (busy).

// SD card status flags.
#define xSTAT_INIT_OK                   0x01U

// SD card type flags.
#define TYPE_HCSD                       1U                                                // High Capacity SD card.
#define TYPE_SCSD                       2U                                                // Standard Capacity SD card.

// Data transfer tokens.
#define TKN_START_BLK                   0xFE                                              // Single block read/write, multiple block read.
#define TKN_START_MUL_BLK_WR            0xFC                                              // Multiple block write.
#define TKN_STOP_MUL_BLK_WR             0xFD                                              // In case of multiple block write stop - this token is received.
#define TKN_DATA_RESP                   0x1F                                              // Data response token |XXX0|Status|1| - 3bits for status.

// Data response token status flags.
#define xSTAT_DATA_OK                   0x05                                              // Bit 2 set - data accepted, bit 0 is always set.


typedef enum
  {
    RPLY_OK        = 1,
    RPLY_ERR       = 2,
    RPLY_ADDR_ERR  = 3,
  } card_rply_t;

typedef struct 
  {
    uint8_t hdr;
    uint32_t args;
    uint8_t crc;
  } sdc_cmd_t;

typedef card_rply_t (*sdc_rply_hdlr_t) (void);

typedef struct
  {
    uint8_t cmd[CMD_LEN];
    sdc_rply_hdlr_t *hdlr;
    uint8_t rply[SDC_RX_BUFF_SIZE];
    uint8_t stat;
    uint8_t r1;
    uint8_t ocr[OCR_SIZE];
    uint8_t type;
  } sdc_t;


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


static void bld_cmd (uint8_t *buf, uint32_t args, uint8_t cmd_num, uint8_t opts);
static void snd_cmd (uint8_t *cmd);

static inline uint16_t wt_rdy (void);
static inline uint8_t rd_r1 (void);
static inline void rd_ocr (void);
static card_rply_t rd_data_blk (uint8_t *buff, uint16_t data_size);
static card_rply_t wr_data_blk (const uint8_t *buff, uint8_t tkn);

static card_rply_t cmd0_rply_hdlr (void);
static card_rply_t cmd8_rply_hdlr (void);
static card_rply_t cmd41_rply_hdlr (void);
static card_rply_t cmd55_rply_hdlr (void);
static card_rply_t cmd58_rply_hdlr (void);
static card_rply_t cmd16_rply_hdlr (void);
static card_rply_t cmd17_rply_hdlr (void);
static card_rply_t cmd18_rply_hdlr (void);
static card_rply_t cmd12_rply_hdlr (void);
static card_rply_t cmd24_rply_hdlr (void);
static card_rply_t cmd25_rply_hdlr (void);


//---------------------------------------------------------------------------------------
//
//                              L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static sdc_cmd_t sdc_cmds[NUM_CMDS] =
  {
    { (0x40 + 0),      0x00000000,   0x95},                                               // CMD0
    { (0x40 + 8),      0x000001AA,   0x87},                                               // CMD8
    { (0x40 + 41),     0x00000000,   0x01},                                               // CMD41 - CMD41 & CMD55 combo is ACMD41.
    { (0x40 + 55),     0x00000000,   0x01},                                               // CMD55
    { (0x40 + 58),     0x00000000,   0x01},                                               // CMD58 - read OCR.
    { (0x40 + 16),     0x00000000,   0x01},                                               // CMD16 - set block size.
    { (0x40 + 17),     0x00000000,   0x01},                                               // CMD17 - read single block.
    { (0x40 + 18),     0x00000000,   0x01},                                               // CMD18 - read multiple blocks.
    { (0x40 + 12),     0x00000000,   0x01},                                               // CMD12 - stop transaction.
    { (0x40 + 24),     0x00000000,   0x01},                                               // CMD24 - write single block.
    { (0x40 + 25),     0x00000000,   0x01}                                                // CMD25 - write multiple blocks.
  };

static const sdc_rply_hdlr_t sdc_rply_hdlrs[NUM_CMD_HDLRS] =
  {
    &cmd0_rply_hdlr,
    &cmd8_rply_hdlr,
    &cmd41_rply_hdlr,
    &cmd55_rply_hdlr,
    &cmd58_rply_hdlr,
    &cmd16_rply_hdlr,
    &cmd17_rply_hdlr,
    &cmd18_rply_hdlr,
    &cmd12_rply_hdlr,
    &cmd24_rply_hdlr,
    &cmd25_rply_hdlr
  };

static sdc_t sdc;


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Initialize SD card.
//
// Arguments:
// N/A
//
// Return:
// sdc_mrc_t result - initialization result.
//---------------------------------------------------------------------------------------
sdc_mrc_t
sdc_init (void)
{
  sdc_t *p_sdc  = &sdc;
  sdc_mrc_t ret = SDC_MRC_INIT_ERR;
  uint8_t i;
  uint16_t tmr;


  spi_init();
  p_sdc->hdlr = (sdc_rply_hdlr_t *) sdc_rply_hdlrs;

  for (i = 0; i < 200; i++)
    {
      spi_rd();
    }

  bld_cmd(p_sdc->cmd, 0, CMD0, 0);
  snd_cmd(p_sdc->cmd);
  if (RPLY_OK == p_sdc->hdlr[CMD0]())
    {
      bld_cmd(p_sdc->cmd, 0, CMD8, 0);
      for (tmr = DLY_5SEC; tmr > 0; tmr -= DLY_10MS)
	{
	  snd_cmd(p_sdc->cmd);
	  if (RPLY_OK == p_sdc->hdlr[CMD8]())
	    {
	      break;
	    }
	  vTaskDelay(DLY_10MS);
	}
      if (0 == tmr)
	{
	  for (tmr = DLY_1SEC; tmr > 0; tmr -= DLY_10MS)
            {
              bld_cmd(p_sdc->cmd, 0, CMD41, 0);
              snd_cmd(p_sdc->cmd);
              if (RPLY_OK == p_sdc->hdlr[CMD41]())
                {
                  break;
	        }
	      vTaskDelay(DLY_10MS);
	    }
	}
      else
	{
	  for (tmr = DLY_1SEC; tmr > 0; tmr -= DLY_10MS)
            {
              bld_cmd(p_sdc->cmd, 0, CMD55, 0);
              snd_cmd(p_sdc->cmd);
              p_sdc->hdlr[CMD55]();
              bld_cmd(p_sdc->cmd, 1UL << 30, CMD41, xBLD_CMD_USE_ARGS);
              snd_cmd(p_sdc->cmd);
              if (RPLY_OK == p_sdc->hdlr[CMD41]())
                {
                  bld_cmd(p_sdc->cmd, 0, CMD58, 0);
                  snd_cmd(p_sdc->cmd);
                  if (RPLY_OK == p_sdc->hdlr[CMD58]())
                    {
                      p_sdc->stat |= xSTAT_INIT_OK;                                       // Card is initialized.
                      ret          = SDC_MRC_INIT_OK;
                      if (TYPE_HCSD == p_sdc->type)
                        {
                        }
                      if (TYPE_SCSD == p_sdc->type)
                        {
                        }
                      if (sdc_rd(p_sdc->rply, 0, 2))
                        {
                        }

                    }
                  break;
                }
              vTaskDelay(DLY_10MS);
            }
        }
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// SD card status.
//
// Arguments:
// N/A
//
// Return:
// uint8_t result - card status.
//---------------------------------------------------------------------------------------
sdc_mrc_t
sdc_stat (void)
{
  sdc_t *p_sdc  = &sdc;
  sdc_mrc_t ret = SDC_MRC_INIT_ERR;


  if (0 != (p_sdc->stat & xSTAT_INIT_OK))
    {
      ret = SDC_MRC_INIT_OK;
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// SD card read.
//
// Arguments:
// uint8_t *buff - buffer for read data.
// uint32_t sector - secotr address (LBA)
// uint8_t count - number of sectors to read
//
// Return:
// uint8_t result - card read result.
//---------------------------------------------------------------------------------------
sdc_mrc_t
sdc_rd (uint8_t *buff, uint32_t sector, uint8_t count)
{
  sdc_t *p_sdc  = &sdc;
  sdc_mrc_t ret = SDC_MRC_NOT_RDY;
  uint8_t cmd   = CMD18;                                                                  // Assume multiple block read.


  if (0 != (p_sdc->stat & xSTAT_INIT_OK))
    {
      if (TYPE_SCSD == p_sdc->type)                                                       // For SCSD convert sector to byte.
        {
          sector *= BLK_SIZE;
        }
      if (1 == count)                                                                     // One block to read.
        {
          cmd = CMD17;
        }
      bld_cmd(p_sdc->cmd, sector, cmd, xBLD_CMD_USE_ARGS);
      snd_cmd(p_sdc->cmd);
      if (RPLY_OK == p_sdc->hdlr[cmd]())
        {
	  for ( ; count > 0; count--)
            {
              if (RPLY_ERR == rd_data_blk(buff, BLK_SIZE))                                // Read block of data.
                {
                  ret = SDC_MRC_RD_ERR;
                  break;
                }
              buff += BLK_SIZE;                                                           // Update pointer.
            }
          if (CMD18 == cmd)                                                               // Stop transmission for multiple block read.
            {
              bld_cmd(p_sdc->cmd, 0, CMD12, 0);
              snd_cmd(p_sdc->cmd);
              p_sdc->hdlr[CMD12]();
            }
          if (0 == count)                                                                 // All data block received ok.
            {
              ret = SDC_MRC_RD_OK;
            }
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// SD card write.
//
// Arguments:
// uint8_t *buff - data to be written.
// uint32_t sector - secotr address (LBA)
// uint8_t count - number of sectors to read
//
// Return:
// uint8_t result - card read result.
//---------------------------------------------------------------------------------------
sdc_mrc_t
sdc_wr (const uint8_t *buff, uint32_t sector, uint8_t count)
{
  sdc_t *p_sdc  = &sdc;
  sdc_mrc_t ret = SDC_MRC_INIT_ERR;
  uint16_t tmr  = 0;
  uint8_t cmd   = CMD25;
  uint8_t tkn   = TKN_START_MUL_BLK_WR;


  if (0 != (p_sdc->stat & xSTAT_INIT_OK))                                                 // Check if card is initialised.
    {
      ret = SDC_MRC_WR_ERR;
      if (TYPE_SCSD == p_sdc->type)                                                       // Check card type.
        {
          sector *= BLK_SIZE;
        }
      if (1 == count)                                                                     // If one block to write change cmd and token.
        {
          cmd = CMD24;
          tkn = TKN_START_BLK;
        }
      bld_cmd(p_sdc->cmd, sector, cmd, xBLD_CMD_USE_ARGS);
      snd_cmd(p_sdc->cmd);
      if (RPLY_OK == p_sdc->hdlr[cmd]())
        {
	  for ( ; count > 0; count--)
            {
              if (RPLY_ERR == wr_data_blk(buff, tkn))
                {
                  break;                                                                  // Block write error - stop here.
                }
              buff += BLK_SIZE;
            }
          if (0 == count)
            {
              ret = SDC_MRC_WR_OK;                                                        // All blocks written ok.
            }
          if (CMD25 == cmd)                                                               // Check if this was multiple block write.
            {
              if (RPLY_ERR == wr_data_blk(buff, TKN_STOP_MUL_BLK_WR))                     // Send stop transmition token.
                {
                  ret = SDC_MRC_WR_ERR;
                }
            }
          for (tmr = DLY_100MS; tmr > 0; tmr -= DLY_10MS)                                 // Wait till line is busy.
            {
              if (0xFF == spi_rd())                   
                {
                  break;                                                                  // Card is active again.
                }
              vTaskDelay(DLY_10MS);                                                       //bugtraker - this may be bottleneck for data write.
            }
        }

    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// SD card write.
//
// Arguments:
// uint8_t count - number of sectors to read
//
// Return:
// uint8_t result - card read result.
//---------------------------------------------------------------------------------------
sdc_mrc_t
sdc_ioctl (uint8_t ctrl)
{
  sdc_mrc_t ret = SDC_MRC_NOT_RDY;


  switch (ctrl)
    {
      default:
        if (0 != wt_rdy())
          {
            ret = SDC_MRC_RDY;
          }
        break;
    }
  
  return (ret);
}


//---------------------------------------------------------------------------------------
//
//                         L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Function build a command based on passed arguments.
//
// Arguments:
// uint8_t *buf - buffer to put command,
// uint32_t args - optional command arguments (4B),
// uint8_t cmd_num - command number (index to array of commands).
// uint8_t opts - options for build command.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
bld_cmd (uint8_t *buf, uint32_t args, uint8_t cmd_num, uint8_t opts)
{
  sdc_cmd_t *p_sdc_cmds = &sdc_cmds[cmd_num];


  *buf++ = p_sdc_cmds->hdr;

  if (0 == (opts & xBLD_CMD_USE_ARGS))
    {
      args = p_sdc_cmds->args;
    }
 
  *buf++ = args >> 24;
  *buf++ = args >> 16;
  *buf++ = args >> 8;
  *buf++ = args;
  *buf++ = p_sdc_cmds->crc;
}


//---------------------------------------------------------------------------------------
// Send command to SD card.
//
// Arguments:
// uint8_t *cmd - data to send.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
snd_cmd (uint8_t *cmd)
{
  uint8_t i;


  if (0 != wt_rdy())
    {
      for (i = 0; i < CMD_LEN; i++)
        {
          spi_wr(*cmd++);
        }
    }
}


//---------------------------------------------------------------------------------------
// Wait for card to be ready. When card is ready (nothing is pending) it responds 
// with 0xFF.
//
// Arguments:
// N/A
//
// Return:
// uint16_t tmr - time left from 500[ms].
//---------------------------------------------------------------------------------------
static inline uint16_t
wt_rdy (void)
{
  uint16_t tmr = DLY_500MS;


  while ((0xFF != spi_rd()) && tmr > 0)                                                   // Wait for card to be ready (500[ms] timeout).
    {
      vTaskDelay(DLY_10MS);
      tmr -= DLY_10MS;
    }

  return (tmr);
}


//---------------------------------------------------------------------------------------
// Read R1. Read stops when MSB in R1 is cleared.
//
// Arguments:
// N/A
//
// Return:
// uint8_t try - number of tryies to read r1.
//---------------------------------------------------------------------------------------
static inline uint8_t
rd_r1 (void)
{
  uint8_t try  = NUM_TRIES;
  sdc_t *p_sdc = &sdc;


  do
    {
      p_sdc->r1 = spi_rd();
    } while ((0 != (p_sdc->r1 & xR1_MSB)) && --try > 0);
  
  return (try);
}


//---------------------------------------------------------------------------------------
// Read OCR.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static inline void
rd_ocr (void)
{
  sdc_t *p_sdc  = &sdc;


  p_sdc->ocr[3] = spi_rd();
  p_sdc->ocr[2] = spi_rd();
  p_sdc->ocr[1] = spi_rd();
  p_sdc->ocr[0] = spi_rd();
}


//---------------------------------------------------------------------------------------
// Read a block of data.
//
// Arguments:
// uint8_t *buff      - pointer to data,
// uint16_t data_size - size of data to be read.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static card_rply_t
rd_data_blk (uint8_t *buff, uint16_t data_size)
{
  card_rply_t ret = RPLY_ERR;
  uint16_t tmr    = DLY_100MS;
  uint8_t tkn;


  do                                                                                      // Try to get token for 100ms.
    {
      tkn = spi_rd();
      vTaskDelay(DLY_10MS);
      tmr -= DLY_10MS;
    } while ((0xFF == tkn) && tmr > 0);

  if (TKN_START_BLK == tkn)                                                               // Check for start of block token.
    {
      for ( ; data_size > 0; data_size--)                                                     // Read block of data.
        {
          *buff++ = spi_rd();
        }
      spi_rd();                                                                           // Discard CRC.
      spi_rd();
      ret = RPLY_OK;
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Write a block of data.
//
// Arguments:
// uint8_t *buff - pointer to data,
// uint8_t tkn   - token.
//
// Return:
// card_rply_t ret - card reply.
//---------------------------------------------------------------------------------------
static card_rply_t
wr_data_blk (const uint8_t *buff, uint8_t tkn)
{
  card_rply_t ret = RPLY_ERR;
  uint16_t i      = 0;
  uint8_t data    = 0;
 

  if (0 != wt_rdy())                                                                            // Check if there was a timeout.
    {
      ret = RPLY_OK;
      spi_wr(tkn);
      if (TKN_STOP_MUL_BLK_WR != tkn)                                                     // Process only for multi block command.
        {
          for (i = 0; i < BLK_SIZE; i++)
            {
              spi_wr(*buff++);
            }
          spi_wr(0xFF);                                                                   // Dummy checksum.
          spi_wr(0xFF);
          data = spi_rd();

//bugtraker check what is the card response like.
          if ((data & TKN_DATA_RESP) != xSTAT_DATA_OK)                                                      // Check if successfuly written.
            {
              ret = RPLY_ERR;
            }
         }
    }  

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 0 handler, looks for R1 response with xR1_IN_IDLE_STATE bit set.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static card_rply_t
cmd0_rply_hdlr (void)
{
  card_rply_t ret = RPLY_ERR;
  sdc_t *p_sdc    = &sdc;
  uint8_t try     = rd_r1();


  if ((xR1_IN_IDLE_STATE == p_sdc->r1) && try > 0)
    {
      ret = RPLY_OK;
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 8 handler, looks for R1 and OCR echo.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static card_rply_t
cmd8_rply_hdlr (void)
{
  sdc_cmd_t *p_sdc_cmds = &sdc_cmds[CMD8];
  uint32_t buf          = 0;
  card_rply_t ret       = RPLY_ERR;
  uint8_t try           = rd_r1();


  if (try > 0)
    {
      buf  = (((uint32_t)spi_rd()) << 24);
      buf |= (((uint32_t)spi_rd()) << 16);
      buf |= (((uint32_t)spi_rd()) << 8);
      buf |= (uint32_t)spi_rd();

      if (buf == p_sdc_cmds->args)                                                        // Check if card ECHOed voltage info and pattern.
        {
          ret = RPLY_OK;
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 41 handler, looks for R1 with no xR1_IN_IDLE_STATE bit and OCR.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static card_rply_t
cmd41_rply_hdlr (void)
{
  sdc_t *p_sdc     = &sdc;
  card_rply_t ret  = RPLY_ERR;
  uint8_t try      = rd_r1();

  
  if (try > 0)                         
    {
      rd_ocr();
      if (0 == p_sdc->r1)                                                                 // Idle bit is cleared - card is in ready state.
        {
          ret = RPLY_OK;
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 55 handler. Read R1. This command is issued with CMD41 as a combo to create
// ACMD41. Response from this command shall be discarded.
//
// Arguments:
// N/A
//
// Return:
// card_rply_t RPLY_OK - always ok.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd55_rply_hdlr (void)
{
  rd_r1();

  return (RPLY_OK);
}


//---------------------------------------------------------------------------------------
// Command 58 handler. Read R3 (R1 and OCR), and check card capacity status. This bit
// is valid only when busy bit is set (power up sequence has completed).
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd58_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();
  

  if (try > 0 && (0 == p_sdc->r1))                                                        // Card ready and no errors in R1.
    {
      rd_ocr();
      if (0 != (p_sdc->ocr[3] & xOCR_BUSY))                                               // Check if CCS data is valid.
        {
          p_sdc->type = TYPE_HCSD;                                                        // Assume High Capacity SD.
          if (0 == (p_sdc->ocr[0] & xOCR_CCS))
            {
              p_sdc->type = TYPE_SCSD;                                                    // Standard Capacity SD.
            }
          ret = RPLY_OK;
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 16 - set SD card block size.
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd16_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();


  if ((0 == p_sdc->r1) && try > 0)
    {
      ret = RPLY_OK;
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 17 - read data token (R1).
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd17_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();
  

  if (try > 0)
    {
      if (0 != (p_sdc->r1 & xR1_ADDR_ERR))
        {
          ret = RPLY_ADDR_ERR;
        }
      if (0 == p_sdc->r1)
        {
          ret = RPLY_OK;
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 18 - 
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd18_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();


  if (try > 0)
    {
      if (0 != (p_sdc->r1 & xR1_ADDR_ERR))
        {
          ret = RPLY_ADDR_ERR;
        }
      if (0 == p_sdc->r1)
        {
          ret = RPLY_OK;
        }
    }

  return (ret);
}

//---------------------------------------------------------------------------------------
// Command 12 - Stop transmission in multiple block read operation.
//              After reception of R1 the host must poll the card for (busy) token. 
//              Zero indicates card is busy.
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd12_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();
  uint16_t tmr    = DLY_5SEC;


  if (try > 0)
    {
      if (0 == p_sdc->r1)                                                                 // Check any errors in R1.
        {
          while (tmr-- > 0)                                                               // Try to read R1b for up to 5sec.
            {
              vTaskDelay(DLY_100MS);
              if (0 != spi_rd())                                                          // Check if card is still busy.
                {
                  ret = RPLY_OK;
                  break;
                }
            }
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 24 - Single write.
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd24_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();


  if ((0 == p_sdc->r1) && try > 0)
    {
      ret = RPLY_OK;
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Command 25 - Single write.
//
// Arguments:
// N/A
//
// Return:
// card_rply_t ret - handler result.
//---------------------------------------------------------------------------------------
static card_rply_t
cmd25_rply_hdlr (void)
{
  sdc_t *p_sdc    = &sdc;
  card_rply_t ret = RPLY_ERR;
  uint8_t try     = rd_r1();


  if ((0 == p_sdc->r1) && try > 0)
    {
      ret = RPLY_OK;
    }

  return (ret);
}


#endif

