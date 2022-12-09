/**
  ******************************************************************************
  * @file    main.c (bootloader)
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    03-01-2019
  * @brief   This file provides the bootloader main procedure
  ******************************************************************************
  * @attention
  *
  * MadeFactory owns the sole copyright to this software. Under international
  * copyright laws you (1) may not make a copy of this software except for
  * the purposes of maintaining a single archive copy, (2) may not derive
  * works herefrom, (3) may not distribute this work to others. These rights
  * are provided for information clarification, other restrictions of rights
  * may apply as well.
  *
  * <h2><center>&copy; Copyright 1997-2019 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "BSPDefs.h"
#include "dvGPIOType.h"

// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************

//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
///void display_clock_init();
void run_app();
void mass_storage_main();

/**
 *	@brief		Initialize the bootloader device driver
 *	@param[in]	None
 *	@return		None
 */
static void bootDeviceInit(void)
{
  ///display_clock_init();
  
  /* Detect USB Host(PC) Connection */
  *R_GPIDIR(USB_DETECT_PORT) |= (1 << USB_DETECT_PIN);// Set USB Detect Pin to Input  

  /* SDHC AF Set to DRIVE_SDCARD */
  *R_PAF(2) = AF2_0_SDHC_D0 | AF2_1_SDHC_D1 | AF2_2_SDHC_D2 | AF2_3_SDHC_D3 
                | AF2_4_SDHC_CLK | AF2_5_SDHC_CMD | AF2_6_GP6 | AF2_7_GP7;
}

/**
 *	@brief		load application to RAM and jumpt to application
 *	@param[in]	None
 *	@return		None
 */
static void bootStartApplicationFromFileSystem(void)
{
  FATFS   sFileSystem;
  FIL     sFileObject;
  U32*    pStartAddress = (U32* )0xc0000000;
  int     nFileSize     = 0;
  U32     nRead;
  
#if	(FAT_APP_TYPE == DRIVE_NAND)
  f_mount(DRIVE_NAND,&sFileSystem);	
  f_chdrive(DRIVE_NAND);
  ///debugstring("Execute Mode: Run "APPLICATOPN_FILE_NAME" from NAND Filesystem \r\n");
#else
  f_mount(DRIVE_SDCARD,&sFileSystem);
  f_chdrive(DRIVE_SDCARD);
  ///debugstring("Execute Mode: Run "APPLICATOPN_FILE_NAME" from SD Card Filesystem \r\n");  
#endif
  
  if(FR_OK!= f_open(&sFileObject,APPLICATOPN_FILE_NAME,FA_READ|FA_OPEN_EXISTING))
  {
    debugstring("FAILED to open the \""APPLICATOPN_FILE_NAME"\" !!\r\n");
    while(1);
  }
  
  nFileSize = f_size(&sFileObject);

  f_read(&sFileObject,pStartAddress,nFileSize,&nRead);
  f_close(&sFileObject);    

  void ( *startfp )() = ( void ( * )() )pStartAddress[0]; // get start address
  *( volatile U32* )0x80000008 = 0; // graphic engine off.
  
  startfp();  
}

/**
  * @brief  bootloader Main Procedure.
  * @param  None
  * @retval None
  */
int main()
{	
  BYTE  cRunMode = 0;
  
  bootDeviceInit();

  debugstring("\r\n======================================================================\r\n");
  debugprintf(" LCD Module bootloader V1.0 by Made Factory June. System Clock:%dMhz\r\n"
                , get_axi_clock() / 1000000);
  debugstring("======================================================================\r\n");

  cRunMode = (BYTE)((*R_GPILEV(USB_DETECT_PORT))) & USB_DETECT_MASK; //  Get Portx.x Input Data Level

#if USE_AMAZON_STK  
  cRunMode = (cRunMode) ? 0 : 1;
#endif

  if(cRunMode)
  {
    /* USB mass-storage Mode */
    debugstring("\r\nUSB Mass-Storage Mode\r\n");
    mass_storage_main();    
  }
  else
  {
    /* User application load from filesystem */
    bootStartApplicationFromFileSystem();    
  }  

  /* Error */
  PRINTLINE;
  
  while(1);
  
  return 0;  
}

