/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "sdk.h"
#include "fatfs/diskio.h"

static U32 org_int_mask[2];
static U32 fs_int_disable[2]={0,};

/*
if you use file-io function at ISR, you should register interrupt-number
*/
void fs_enable_support_interrupt(int intnum,BOOL b)
{
	int n=0;
	if(intnum>31)
	{
		n=1;
		intnum-=32;
	}
	if(b)
		fs_int_disable[n]|=1<<intnum;
	else
		fs_int_disable[n]&=~(1<<intnum);

}

#if _FS_REENTRANT
/*
These functions is used for only firmware.
if you use FreeRTOS, Use semaphore
*/

/* Create a sync object */
int  __attribute__((weak)) ff_cre_syncobj  (BYTE b, _SYNC_t* t)
{
	*t = 1;
	return 1;
}

/* Lock sync object */
int __attribute__((weak)) ff_req_grant (_SYNC_t t)
{
	/*register Interrupt Number what used filesystem
	DMA(Sound) ISR is use FS
	*/
	org_int_mask[0]= *R_INTMASK(0);
	org_int_mask[1]= *R_INTMASK(1);
	
	*R_INTMASKCLR(0) = fs_int_disable[0];
	*R_INTMASKCLR(1) = fs_int_disable[1];

	return 1;
}
/* Unlock sync object */
void __attribute__((weak)) ff_rel_grant (_SYNC_t t)		
{
	*R_INTMASK(0) = org_int_mask[0];
	*R_INTMASK(1) = org_int_mask[1];
}
/* Delete a sync object */
int __attribute__((weak))  ff_del_syncobj (_SYNC_t t)
{
	return 1;
}
#endif


#if CONFIG_NAND == 0
#define nand_init(...)	0
#define nand_readsects(...)	0
#define nand_updatesects(...)	0
#define nand_get_sectorcount()	0
#define nand_getsectorsize() 0
#endif


//avoid SDCARD api linking 
#if CONFIG_SDCARD == 0
#define sdhc_init(...)	0
#define sdhc_read_sect(...)	0
#define sdhc_write_sect(...)	0
#define sdhc_getsectorcount()	0
#define sdhc_getsectorsize() 0
#endif
	

extern int UsbMemInit(void);
extern int read_disk (__u32 startblock, __u32 getsize, __u8 * bufptr);
extern int write_disk (__u32 startblock, __u32 getsize, __u8 * bufptr);
extern int USB_GetSectorCount();
extern int USB_GetSectorSize();

DSTATUS disk_initialize ( BYTE drv)
{
	if(drv == DRIVE_NAND )
	{
		if(nand_init())
			return RES_OK;
	}
#if CONFIG_SDCARD==1	
	else if(drv == DRIVE_SDCARD)
	{
		if(sdhc_init())
			return RES_OK;
	}	
#endif
#if CONFIG_USBHOST==1	
	else if(drv == DRIVE_USBHOST)
	{
		if(UsbMemInit())
			return RES_OK;
	}
#endif
	else
		DEBUGPRINTF("%d driver not supported\r\n",drv);
	return RES_ERROR;
}


/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
					 BYTE drv		/* Physical drive nmuber (0..) */
					 )
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
				   BYTE drv,		/* Physical drive nmuber (0..) */
				   BYTE *buff,		/* Data buffer to store read data */
				   DWORD sector,	/* Sector address (LBA) */
				   UINT count		/* Number of sectors to read (1..255) */
				   )
{
	if(drv == DRIVE_NAND )
	{
		if(nand_readsects(sector,buff,count))
			return RES_OK;
	}
#if CONFIG_SDCARD==1	
	else if(drv == DRIVE_SDCARD )
	{
		if(sdhc_read_sect(buff,sector,count))
		{
			return RES_OK;
		}
	}
#endif
#if CONFIG_USBHOST==1		
	else if(drv == DRIVE_USBHOST)
	{
		if(read_disk(sector, count, buff))
			return RES_OK;
	}
#endif
	return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

DRESULT disk_write (
					BYTE drv,			/* Physical drive nmuber (0..) */
					const BYTE *buff,	/* Data to be written */
					DWORD sector,		/* Sector address (LBA) */
					BYTE count			/* Number of sectors to write (1..255) */
					)
{
	if(drv == DRIVE_NAND)
	{
		if(nand_updatesects(sector,(void*)buff,count))
			return RES_OK;
	}
#if CONFIG_SDCARD==1		
	else if(drv == DRIVE_SDCARD)
	{
		if( sdhc_write_sect((void*)buff,sector,count) )
			return RES_OK;
	}
#endif
#if CONFIG_USBHOST==1		
	else if(drv == DRIVE_USBHOST)
	{
		if(write_disk(sector, count, (U8*)buff))
			return RES_OK;
	}
#endif
	return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res =RES_PARERR ;

	switch (ctrl) {
	case CTRL_SYNC:
		if(drv==DRIVE_NAND)
		{
#if CONFIG_NAND == 1			
			nand_flushdata();
#endif		
		}
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT:
#if CONFIG_NAND == 1
		if(drv==DRIVE_NAND)
		{
			*(DWORD*)buff = nand_get_sectorcount();
			res = RES_OK;
			break;
		}
#endif
#if CONFIG_SDCARD==1	
		if(drv==DRIVE_SDCARD)
		{
			*(DWORD*)buff = sdhc_getsectorcount();
			res = RES_OK;
			break;
		}
#endif
#if CONFIG_USBHOST==1	
		if(drv == DRIVE_USBHOST)
		{
			*(DWORD*)buff = USB_GetSectorCount();
			res = RES_OK;
		}
#endif
		break;

	case GET_SECTOR_SIZE:
#if CONFIG_NAND == 1
		if(drv==DRIVE_NAND)
		{
			*(WORD*)buff = nand_get_sectorsize();
			res = RES_OK;
			break;
		}
#endif
#if CONFIG_SDCARD==1		
		if(drv==DRIVE_SDCARD)
		{
			*(WORD*)buff = sdhc_getsectorsize();
			res = RES_OK;
			break;
		}
#endif
#if CONFIG_USBHOST==1	
		if(drv == DRIVE_USBHOST)
		{
			*(WORD*)buff = USB_GetSectorSize();
			res = RES_OK;
		}
#endif
		break;

	case GET_BLOCK_SIZE:
#if CONFIG_NAND == 1
		if (drv == DRIVE_NAND)
		{
			*(DWORD*)buff = nand_get_blocksize();
			res = RES_OK;
			break;
		}
#endif
#if CONFIG_SDCARD == 1
		if (drv == DRIVE_SDCARD)
		{
			*(DWORD*)buff = sdhc_getsectorsize();
			res = RES_OK;
			break;
		}
#endif
#if CONFIG_USBHOST == 1
		if (drv == DRIVE_USBHOST)
		{
			*(DWORD*)buff = USB_GetSectorSize();
			res = RES_OK;
			break;
		}
#endif
		break;
	}

	return res;
}
/*
Currnet time is returned with packed into a DWORD value. The bit field is as follows:

bit31:25 
Year from 1980 (0..127) 
bit24:21 
Month (1..12) 
bit20:16 
Day in month(1..31) 
bit15:11 
Hour (0..23) 
bit10:5 
Minute (0..59) 
bit4:0 
Second / 2 (0..29) 
*/
DWORD get_fattime (void)
{
	
	return 0;
}
