/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
//#include "platform.h"
//#include "storage.h"

#include "sd_card.h"
#include "uart.h"

/* Example: Mapping of physical drive number for each drive */
#define DEV_FLASH	1	/* Map FTL to physical drive 0 */
#define DEV_MMC		0	/* Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Map USB MSD to physical drive 2 */

static volatile DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	if (pdrv != DEV_MMC) {
		return STA_NOINIT;
	}
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
if (pdrv != DEV_MMC) {
		return STA_NOINIT;
	}
	
	if (SD_Init() == 0) {
		// USART_SendString("disk_initialize: SD_Init OK\r\n");
		Stat &= ~STA_NOINIT; 
	} else {
		// USART_SendString("disk_initialize: SD_Init FAILED\r\n");
		Stat |= STA_NOINIT;  
	}
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if (pdrv != DEV_MMC || !count) {
		return RES_PARERR; 
	}
	
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY; 
	}
	
	for (UINT i = 0; i < count; i++) {
		if (SD_ReadBlock(buff, sector + i) != 0) {
			 USART_SendString("disk_read: SD_ReadBlock FAILED\r\n");
			return RES_ERROR; 
		}
		buff += 512; 
	}
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if (pdrv != DEV_MMC || !count) {
		return RES_PARERR; 
	}
	
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}
	
	for (UINT i = 0; i < count; i++) {

		if (SD_WriteBlock((BYTE*)buff, sector + i) != 0) {
			 USART_SendString("disk_write: SD_WriteBlock FAILED\r\n");
			return RES_ERROR; 
		}
		buff += 512;
	}
	
	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE csd[16];    
	LBA_t sector_count;
	DWORD c_size;
	UINT i;

	if (pdrv != DEV_MMC) {
		return RES_PARERR;
	}
	
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY; 
	}
	
	res = RES_ERROR; 
	
	switch (cmd) {
		case CTRL_SYNC:
			SD_CS_HIGH();
			SD_SPI_SendByte(0xFF); 
			res = RES_OK;
			break;
		
		case GET_SECTOR_COUNT:
			if (SD_SendCmd(CMD9, 0, 0xFF) == 0x00) {
				BYTE token;
				WORD timeout = 0xFFFF;
				
				do {
					token = SD_SPI_ReceiveByte();
				} while ((token != 0xFE) && (--timeout));
				
				if (timeout == 0) { 
           res = RES_ERROR;
           break;
				}
				
				if (token == 0xFE) {
					for (i = 0; i < 16; i++) {
						csd[i] = SD_SPI_ReceiveByte();
					}
					SD_SPI_ReceiveByte();
					SD_SPI_ReceiveByte();
				
					if (SD_Type == SD_TYPE_SDHC) {
						c_size = ((DWORD)(csd[7] & 0x3F) << 16) | 
										 ((DWORD)csd[8] << 8) | 
										 (DWORD)csd[9];
					
						sector_count = (c_size + 1) * 1024;
					} else {
						DWORD c_size_mult, read_bl_len;
					
						read_bl_len = csd[5] & 0x0F;
						c_size = ((DWORD)(csd[6] & 0x03) << 10) |
										 ((DWORD)csd[7] << 2) |
									   ((DWORD)(csd[8] & 0xC0) >> 6);
	
						c_size_mult = ((DWORD)(csd[9] & 0x03) << 1) |
													((DWORD)(csd[10] & 0x80) >> 7);
					
						sector_count = (c_size + 1) * (1 << (c_size_mult + 2));
						sector_count = sector_count * (1 << (read_bl_len - 9));
					}
					
					*(LBA_t*)buff = sector_count; 
					res = RES_OK;
				}
			}
			SD_CS_HIGH();
			SD_SPI_ReceiveByte(); 
			break;
		
		case GET_SECTOR_SIZE:
			*(WORD*)buff = 512;
			res = RES_OK;
			break;
		
		case GET_BLOCK_SIZE:
			if (SD_Type == SD_TYPE_SDHC) {
				*(DWORD*)buff = 128;
			} else {
				*(DWORD*)buff = 1;
			}
			res = RES_OK;
			break;
		
		case CTRL_TRIM:
			res = RES_OK;
			break;
		
		default:
			res = RES_PARERR;
			break;
	}
	
	return res;
}

