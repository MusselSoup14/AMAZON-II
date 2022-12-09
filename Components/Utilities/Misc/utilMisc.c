/**
  ******************************************************************************
  * @file    utilMisc.c
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    12-August-2014
  * @brief   This file provides APIs for Misc.
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
  * <h2><center>&copy; Copyright 1997-2016 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
//#include <ctype.h> //tolower(), toupper()
#include "utilMiscAPI.h"
#include "dvCPUAPI.h"
#include "utilExecutiveAPI.h"
#include "utilHostIfAPI.h"
#include "appUserInterfaceAPI.h"
#include "appSystemStateAPI.h"

#define _MOD_NAME_ "\"MISC\""
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
//#define UFI_DEBUG
#ifdef UFI_DEBUG
	#define DEBUGUFI	debugprintf
#else
	#define DEBUGUFI(...)
#endif

#define   END_LEN		                    0x07		// Endpoint Dscriptor Length
#define   DSCR_STRING                   3   // Descriptor type: String 
#define   TEST_UNIT_READY      	        0x00
#define   REQUEST_SENSE               	0x03
#define   INQUIRY                     	0x12
#define   READ_FORMAT			              0x23
#define   STOP_START_UNIT             	0x1B
#define   PREVENT_ALLOW_MEDIUM_REMOVAL	0x1E
#define   READ_CAPACITY               	0x25
#define   READ_10                     	0x28
#define   VERIFY_10                   	0x2F
#define   MODE_SELECT_06			          0x15
#define   MODE_SENSE_06 			          0x1A
#define   WRITE_10			                0x2A

#define   BCODE	  0
#define   BSTRUC	1
#define   SHALF	  2
#define   LLBA	  3
#define   ROOTE	  4
#define   L0x60	  5
#define	  LFAT	  6
#define   LFAT2	  7

#define   RESIDUE_ADDR	0xff80
#define   RESIDUE_ADDR_B	0xffc0

union byte2
{
    unsigned short bw;

    struct
    {
        unsigned char bl;
        unsigned char bh;
    } wordsp;
};

union BYTE4
{
    unsigned long b4;

    struct
    {
        unsigned char ll;
        unsigned char lh;
        unsigned char hl;
        unsigned char hh;
    } longsp;
};

typedef struct tagCBW
{
    unsigned char Sign[4];
    unsigned char Tag[4];
    unsigned long DTL;
    unsigned char Flag;
    unsigned char LUN;
    unsigned char CBWCBLen;
}MISC_MASS_CBW, *PMISC_MASS_CBW;

//****************************************************************************
//****************** START OF PRIVATE CONFIGURATION SECTION ******************
//****************************************************************************
//----------------------------------------------------------------------------------------
// Value to Mass storage Descriptor 
//----------------------------------------------------------------------------------------
static CROMDATA BYTE	m_acDevDescriptorMass[] =
{
  DEV_LEN,			  // bLength: Size of descriptor
  0x01,		        // bDescriptorType: Device
  0x00,0x01,			// bcdUSB: USB 1.1
  0x00,				    // bDeviceClass: none
  0x00,				    // bDeviceSubClass: none
  0x00,				    // bDeviceProtocol: none
  EP0_LEN,			  // bMaxPacketSize0: 16 bytes
  0xDC,0x0A,		  // idVendor:  - ADC
  0x22,0x00,			// idProduct:
  0x01,0x00,			// bcdDevice: device release
  0x00,				    // iManufacturer:
  0x00,				    // iProduct:
  0x00,				    // iSerialNumber: none
  0x01
};

//----------------------------------------------------------------------------------------
// CONFIGURATION DESCRIPTOR
//----------------------------------------------------------------------------------------
static CROMDATA BYTE	m_acCFGDescriptorMass[] = 
{
  0x09,				// bLength: Size of descriptor
  0x02,		    // bDescriptorType: Configuration
  39,0x00,    // wTotalLength: Cfg+Ifc+Class+Ep = 39 bytes
  0x01,				// bNumInterfaces: 1 interface
  0x01,				// bConfigurationValue: 1
  0x00,				// iConfiguration: none
  0x60,				// bmAttributes: Self power
  0xFA,				// MaxPower: 500mA

  /* INTERFACE DESCRIPTOR */
  0x09,				    // bLength: Size of descriptor
  0x04,		        // bDescriptorType: Interface
  0x00,				    // bInterfaceNumber: #1
  0x00,				    // bAlternateSetting: #0
  0x02,				    // bNumEndpoints: 2
  0x08,				    // mass storage class
  0x06,				    // SCSI transparent
  0x50,				    // Bulk Only Transfer
  0x00,				    // iInterface: none

  /* ENDPOINT 1 DESCRIPTOR  (BULK OUT) */
  7,			        // bLength: Size of descriptor
  0x05,				    // bDescriptorType: Endpoint
  0x01,				    // bEndpointAddress: OUT, EP1
  0x02,				    // bmAttributes: Bulk
  EP1_LEN,0x00,		// wMaxPacketSize:
  0x00,	          // bInterval: 255ms  , ignored for Bulk

  /* ENDPOINT 2 DESCRIPTOR  (BULK IN) */
  7,			        // bLength: Size of descriptor
  0x05,				    // bDescriptorType: Endpoint
  0x82,				    // bEndpointAddress: IN, EP2
  0x02,				    // bmAttributes: Bulk
  EP2_LEN,0x00,		// wMaxPacketSize:
  0x00,	          // bInterval: 255ms, ignored for Bulk
  
  /* ENDPOINT 3 DESCRIPTOR  (Interrupt IN) */
  7,			        // bLength: Size of descriptor
  0x05,
  0x83,
  0x03,
  02,0x00,
  0x01,
};

//----------------------------------------------------------------------------------------
// DESCRIPTOR String
//----------------------------------------------------------------------------------------
static CROMDATA BYTE m_acStringDescriptorMass[] = 
{   
  4,            // String descriptor length
  DSCR_STRING,
  9,4,

  16,           // String descriptor length
  DSCR_STRING,
  'A',0,
  'D',0,
  'C',0,
  'h',0,
  'i',0,
  'p',0,
  's',0,

  18,           // Descriptor length
  DSCR_STRING,
  'S',0,
  't',0,
  'o',0,
  'r',0,
  'a',0,
  'g',0,
  'e',0,
  ' ',0
};


//****************************************************************************
//******************** START OF PRIVATE DATA DECLARATIONS ********************
//****************************************************************************
// File System Object Structure
static FATFS m_sFSNand, m_sFSCard;

/* Misc StateMachine */
static eMISC_STATE            m_eMiscState; 

static BYTE                 m_cSendCSW;
static BYTE                 m_acBulkBuf[64]     = {0};
static PMISC_MASS_CBW       m_psWrapp;
//static BYTE     m_cRemovalPrevent   = 0;
static BYTE     m_acInquiryData[36] =
{
  0x00,                                   // Device class
  0x80,                                   // RMB bit is set by inquiry data
  0x02,                                   // Version
  0x01,                                   // Data format = 1
  0x00, 0x00, 0x00, 0x00,
  'A', 'D', 'C', 'H', 'I', 'P', 'S', ' ', // Manufacturer
  'M', 'a', 's', 's', ' ', 'S', 't', 'o', // Product(Zip 100)
  'r', 'a', 'g', 'e', ' ', ' ', ' ', ' ',
  '1', '.', '0', '0'                      // Revision
};

static BYTE m_acReadFromatCapa[12] =
{
    0x00,0x00,0x00,0x08,  0x00,0x00,0x40,0x00,0x02,0x00,0x02,0x00
};   	// 8M

static BYTE m_acReadCapa[8] = {0,}; // end sector addres(4byte),pagesize(4byte), warning:big endian

static BYTE m_acSenseData[18] =
{
    0xf0,0x00,0x05,0x00,
    0x00,0x00,0x00,0x0b,
    0x00,0x00,0x00,0x00,
    0x24,0x00,0x00,0x00,
    0x00,0x00
};

static  INT               m_nSectorSize;
static  eMISC_MASS_SRC    m_eMiscMassSource = miscMASS_SRC_LOG;

// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
static void miscMassSendCSW(void)
{
  int i;
  U8 buf[64];
  i = 0;
    
  buf[i++] = 'U';
  buf[i++] = 'S';
  buf[i++] = 'B';
  buf[i++] = 'S';
  buf[i++] = m_acBulkBuf[4];
  buf[i++] = m_acBulkBuf[5];
  buf[i++] = m_acBulkBuf[6];
  buf[i++] = m_acBulkBuf[7];
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = m_cSendCSW;
  
  write_usb(buf,13);
}

static void miscMassWriteData(unsigned long baddr,U32 blkcnt)
{
	U32 buf32[m_nSectorSize/4];
	U8 *buf=(U8*)&buf32[0];
	int k;
  
	for(k=0;k<blkcnt;k++)
	{
		int i;
    
		for(i=0;i<m_nSectorSize;)
		{
			int rev = read_usb(buf+i);
			i+=rev;
		}

    if(m_eMiscMassSource)
  		sdhc_write_sect(buf,(baddr+k),1);
    else
  		nand_updatesects(baddr+k,buf,1);
	}
	
}

static void miscMassSendReadData(unsigned long baddr,U32 blkcnt)
{
	ALIGN4 U8 buf[m_nSectorSize];
	int k;
  
	for(k=0;k<blkcnt;k++)
	{
	  if(m_eMiscMassSource)
  		sdhc_read_sect(buf,(baddr+k),1);
    else
    {
  		if(nand_readsects(baddr+k,buf,1)==FALSE)
  			debugprintf("nand_readpage(%d) error\r\n",baddr+k);
    }

		write_usb(&buf[0],m_nSectorSize);
	}
}

static void miscMassUFICmdCheck(void)
{
  unsigned char dir;//direction
  unsigned char cmd;
  unsigned short dataTransferLen;
  union byte2 TransLen;
  union BYTE4 BlkAddr;
  unsigned int bcnt1;


  dir = m_psWrapp->Flag;//direction

  dataTransferLen = m_psWrapp->DTL;
  cmd = m_acBulkBuf[0xf];
  m_cSendCSW = 0;

  TransLen.wordsp.bl=m_acBulkBuf[0x17];
  TransLen.wordsp.bh=m_acBulkBuf[0x16];
  BlkAddr.longsp.ll=m_acBulkBuf[0x14];
  BlkAddr.longsp.lh=m_acBulkBuf[0x13];
  BlkAddr.longsp.hl=m_acBulkBuf[0x12];
  BlkAddr.longsp.hh=m_acBulkBuf[0x11];


  if (m_psWrapp->Flag & 0x80) // device to host
  {
	  //DEBUGPRINTF("DEVICE TO HOST ( %#x)\r\n",cmd);
    switch (cmd)
    {
      case INQUIRY:               // 0x12
        DEBUGUFI("INQUIRY\r\n");
        write_usb(m_acInquiryData,sizeof(m_acInquiryData));
      break;

      case READ_CAPACITY:         		// 0x25
        DEBUGUFI("READ_CAPACITY\r\n");
        write_usb(m_acReadCapa,8);
      break;

      case READ_10:               // 0x28
        DEBUGUFI("Read10 : BlkAddr.b4 :0x%x, TransLen.bw:0x%x\r\n",BlkAddr.b4 ,TransLen.bw);
        miscMassSendReadData(BlkAddr.b4,TransLen.bw);
      break;
      
      case READ_FORMAT:       			// 0x23
        DEBUGUFI("READ_FORMAT\r\n");
        write_usb(m_acReadFromatCapa,12);
      break;
      
      case MODE_SENSE_06:         		// 0x1A
        DEBUGUFI("MODE_SENSE_06\r\n");
      	{
      		//test ??
      		U8 tempbuf[64];
      		tempbuf[0]=0x3;
      		tempbuf[1]=0x0;
      		tempbuf[2]=0x0;
      		tempbuf[3]=0x0;
      		write_usb(tempbuf,4);
      	}
  /*
          if (m_acBulkBuf[17]!=0x3f)
          {
              DEBUGUFI(" ==== TEST  ==== \r\n");
              m_acSenseData[12]=0x24;
              m_cSendCSW = 1;
              write_usb(ModeParam,0);
          }
          else
          {
              write_usb(ModeParam,12);
          }
  	*/
      break;
          
      case REQUEST_SENSE:         		// 0x03
        DEBUGUFI("REQUEST_SENSE\r\n");
        write_usb(m_acSenseData,18);
      break;


      default:
        DEBUGUFI("Unknown UFI Command 0x%x \r\n",cmd);
        m_cSendCSW = 1;
      break;
    }

  }
  else // host to device
  {
		//DEBUGPRINTF("HOST TO DEVICE ( %#x)\r\n",cmd);
    switch (cmd)
    {
      case WRITE_10:              // 0x2A
        bcnt1=TransLen.bw;
        DEBUGUFI("\r\nWRITE_10 , addr :0x%x , bcnt1 : 0x%x\r\n",BlkAddr.b4,bcnt1);
        miscMassWriteData(BlkAddr.b4,bcnt1);
      break;
      
      case PREVENT_ALLOW_MEDIUM_REMOVAL: 	// 0x1E
        DEBUGUFI("PREVENT_ALLOW_MEDIUM_REMOVAL\r\n");
        m_acSenseData[12]=0x20;
        m_cSendCSW = 1;
      break;

      case VERIFY_10:             		// 0x2F
        DEBUGUFI("VERIFY_10\r\n");
        if(!m_eMiscMassSource)
  	      nand_flushdata();
      break;

      case TEST_UNIT_READY:       		// 0x00
        if(!m_eMiscMassSource)
  	      nand_flushdata();
      break;

      case MODE_SELECT_06:        		// 0x15
        DEBUGUFI("MODE_SELECT_06\r\n");
      break;
    
      case STOP_START_UNIT:   			// 0x1B
        DEBUGUFI("STOP_START_UNIT\r\n");
      break;

      default:
        DEBUGUFI("Unknown UFI Command 0x%x \r\n",cmd);
        m_cSendCSW = 1;
//            while (1);
      break;

    }
  }
    
  miscMassSendCSW();
}

static void masstorage_proc()
{
	read_usb(m_acBulkBuf);
	m_psWrapp = (PMISC_MASS_CBW)m_acBulkBuf;
	miscMassUFICmdCheck();
}


static void miscMassInitStorageInfo()
{
	unsigned int fatpagecnt;
  
  if(m_eMiscMassSource)
  {
  	sdhc_init();
  	fatpagecnt = sdhc_getsectorcount()-1;//make last address
  	m_nSectorSize = sdhc_getsectorsize();
  }
  else
  {
  	//nand_set_cfg(0x3444);
  	nand_init();
  	m_nSectorSize = nand_get_sectorsize();
  	fatpagecnt = nand_get_sectorcount()-1; //make last address
  }

	m_acReadCapa[7] =  m_nSectorSize & 0xff;
	m_acReadCapa[6] = (m_nSectorSize>>8) & 0xff;
	m_acReadCapa[5] = (m_nSectorSize>>16) & 0xff;
	m_acReadCapa[4] = (m_nSectorSize>>24) & 0xff;
	
	m_acReadCapa[3] = fatpagecnt & 0xff;
	m_acReadCapa[2] = (fatpagecnt>>8) & 0xff;
	m_acReadCapa[1] = (fatpagecnt>>16) & 0xff;
	m_acReadCapa[0] = (fatpagecnt>>24) & 0xff;
/*
	ModeParam[8] = fatpagecnt & 0xff;
	ModeParam[7] = (fatpagecnt >> 8) & 0xff;
	ModeParam[6] = (fatpagecnt >> 16) & 0xff;
	ModeParam[5] = (fatpagecnt >> 24) & 0xff;
	ModeParam[10] = 0x2;
	*/
	m_acReadFromatCapa[7] = (fatpagecnt & 0xff);
	m_acReadFromatCapa[6] = (fatpagecnt>>8) & 0xff;
	m_acReadFromatCapa[5] = (fatpagecnt>>16) & 0xff;
	m_acReadFromatCapa[4] = (fatpagecnt>>24) & 0xff;
	
	m_acReadFromatCapa[8] = 0x02;
	m_acReadFromatCapa[10] = 0x2;	
}

static void miscMassStorageStart(void)
{
#if 0
  /*usb_clock_init();
  nand_set_cfg(0x3333);
	miscMassInitStorageInfo();
  
	usb_set_description((BYTE*)m_acCFGDescriptorMass,(BYTE*)m_acDevDescriptorMass);
  
  usb_init();
  
  set_interrupt(INTNUM_USBDEV, miscMassUSBISR);*/
#else
  nand_set_cfg(0x3333);
  miscMassInitStorageInfo();
  usb_init();
#endif

  enable_interrupt(INTNUM_USBDEV, TRUE);
}

static void miscMassStorageStop(void)
{
  enable_interrupt(INTNUM_USBDEV, FALSE);
  ///usb_reset();
}

/**
 *  @brief         sets the Misc state. All flags are cleared
 *  @param[in]  eState  : New system state to set
 *  @param[out]  None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
static void MiscStateChange(eMISC_STATE eState)
{

  /* Exit Previous State */
  switch (m_eMiscState)
  {
    case miscSTATE_IDLE:
    break;

    case miscSTATE_MASS:
      miscMassStorageStop();
      utilExecSignalClear(sgMISC, SIGNAL_MISC_MASS_ACTIVATED);
      #if USE_DEBUG_MISC
      msgDbg("LOGVIEW EXIT");
      #endif
    break;    

    default:
      ASSERT_ALWAYS();
  }

  /* Enter New State */
  switch (eState)
  {
    case miscSTATE_IDLE:
    break;

    case miscSTATE_MASS:
      miscMassStorageStart();
      utilExecSignalSet(sgMISC, SIGNAL_MISC_MASS_ACTIVATED);
      #if USE_DEBUG_MISC
      msgDbg("LOGVIEW ENTER");
      #endif
    break;

    default:
      ASSERT_ALWAYS();
  }
  
  m_eMiscState = eState; // set new state
}

// ***************************************************************************
// ****************************** START OF ISR *******************************
// ***************************************************************************
static void miscMassUSBISR()
{
	BYTE ep_irq ;
	BYTE usb_irq ;
	//while(1)
	{    
		ep_irq  = *(volatile unsigned char*)__USBEI_REG;
		usb_irq = *(volatile unsigned char*)__USBUI_REG;
		
		if(ep_irq & EP0_IRQ)
		{
			ep0_isr(); //usb.c
		}
		
		if (ep_irq & EP1_IRQ)
		{
			masstorage_proc();
		}
		/*
		if (ep_irq & EP2_IRQ)
		{
			debugstring("EP2_IRQ\r\n");
		}
		*/
		if(usb_irq & USB_RESET_IRQ)
		{
			usb_reset();
		}
		if ( usb_irq & USB_RESUME_IRQ )
		{
			usb_resume();
		}
		if ( usb_irq & USB_SUSPEND_IRQ )
		{
			usb_suspend();
		}
		
		*(volatile U8*)__USBEI_REG = ep_irq;   // Clear Interrupt 
		*(volatile U8*)__USBUI_REG = usb_irq;  // Clear Interrupt
	}
}


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

/**
 *  @brief      This API initializes static data structures that are used subsequently at
                runtime and Initializes the device by setting registers that do not change
                frequently at runtime.                          
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscInitialize(void)
{
  /* Filesystem Mount */
  f_mount(DRIVE_NAND, &m_sFSNand);
  f_mount(DRIVE_SDCARD, &m_sFSCard);
  
#if (FAT_APP_TYPE == DRIVE_NAND)
  f_chdrive(DRIVE_NAND);
#else
  f_chdrive(DRIVE_SDCARD);
#endif

  usb_clock_init();
  ///nand_set_cfg(0x3333);  
	usb_set_description((BYTE*)m_acCFGDescriptorMass,(BYTE*)m_acDevDescriptorMass);  

  set_interrupt(INTNUM_USBDEV, miscMassUSBISR);

  utilExecSignalClear(sgMISC, 0xFFFF);

  m_eMiscState = miscSTATE_IDLE;
}

/**
 *  @brief      This API maintains the internal state machine and processes the request
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscProcess(void)
{
  switch(m_eMiscState)
  {
    case miscSTATE_IDLE:
    break;

    case miscSTATE_MASS:
    break;

    default: ASSERT_ALWAYS(); break;
  }
}

/**
 *  @brief      This API Release the internal Components
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscRelease(void)
{
}

/**
 *  @brief      This API Set to USB Mass-storage source
 *  @param[in]  bToSDCard: TRUE to SD Card, FALSE to NAND
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscMassStorageSourceSet(eMISC_MASS_SRC eMassSource)
{
  ASSERT(eMassSource < miscMASS_SRC_INVALID);
  
  m_eMiscMassSource = eMassSource;
}

/**
 *  @brief      This API Get to USB Mass-storage source
 *  @param[in]  None
 *  @param[out] None
 *  @return     USB Mass-storage source
 *  @note       None
 *  @attention  None
 */
eMISC_MASS_SRC utilMiscMassStorageSourceGet(void)
{
  return m_eMiscMassSource;
}

/**
 *  @brief      This API Perform connecting to USB Mass-storage source
 *  @param[in]  bConnect: TRUE to Connect, FALSE to Disconnect
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilMiscMassStorageConnectSet(BOOL bConnect)
{  
  if(miscSTATE_IDLE == m_eMiscState
    && bConnect)
  {
    MiscStateChange(miscSTATE_MASS);
  }
  else if(miscSTATE_MASS == m_eMiscState
    && !bConnect)
  {
    MiscStateChange(miscSTATE_IDLE);  
  }
}

/**
 *  @brief      This API Perform connecting to USB Mass-storage source
 *  @param[in]  None
 *  @param[out] None
 *  @return     TRUE if Connected, FALSE to Disconnected
 *  @note       None
 *  @attention  None
 */
BOOL utilMiscMassStorageConnectGet(void)
{
  return (utilExecSignalGet(sgMISC, SIGNAL_MISC_MASS_ACTIVATED)) ? TRUE : FALSE;
}


