/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "sdcard.h"                                                                       // BugTraker - include SDcard interface.

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define ATA		0
#define MMC		1
#define USB		2



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = STA_NOINIT;


	switch (drv) {
	case MMC :
		if (SDC_MRC_INIT_OK == sdc_init())
                  {
                    stat = 0;
                  }
          break;

        default:
          break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = STA_NOINIT;


	switch (drv) {
	case MMC :
		if (SDC_MRC_INIT_OK == sdc_stat())
                  {
                    stat = 0;
                  }
          break;

        default  :
          break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	DRESULT res = RES_PARERR;


	switch (drv) {
	case MMC :
	    switch (sdc_rd(buff, sector, count))
              {
                case SDC_MRC_RD_OK:
                  res = RES_OK;
                  break;

                case SDC_MRC_RD_ERR:
                  res = RES_ERROR;
                  break;

                case SDC_MRC_NOT_RDY:
                default:
                  res = RES_NOTRDY;
                  break;
              }
          break;

        default:
          break;
	}
	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	DRESULT res = RES_PARERR;


	switch (drv) {
	case MMC :
          switch (sdc_wr(buff, sector, count))
            {
              case SDC_MRC_WR_OK:
                res = RES_OK;
                break;

              case SDC_MRC_WR_ERR:
                res = RES_ERROR;
                break;

              case SDC_MRC_INIT_ERR:
              default:
                break;
            }

        default:
          break;
	}
	return res;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res = RES_PARERR;
  
      
  if (SDC_MRC_INIT_OK == sdc_stat())
    {
      switch (drv)
        {
          case MMC:
	    switch (ctrl)
	      {
                case GET_SECTOR_SIZE:
                  *(DWORD*)buff = 512;
                  res = RES_OK;
                  break;

                case CTRL_SYNC:
                  if (SDC_MRC_RDY == sdc_ioctl(ctrl))                                     
                    {
                      res = RES_OK;
                    }
                  break;

                default:
                  *(DWORD*)buff = 0;
                  res = RES_ERROR;
                  break;
	      }
            break;

          default:
            break;
        }
    }
  else
    {
      res = RES_NOTRDY;
    }

  return res;
}



