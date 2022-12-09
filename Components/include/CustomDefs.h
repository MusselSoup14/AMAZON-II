/**
  ******************************************************************************
  * @file    CustomDefs.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    02-August-2014
  * @brief   This file provides Custom Configuration
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

// ---------------------------------------------------------------------------
// CPU Properties
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Components Properties
// ---------------------------------------------------------------------------
#define LAYER_IMAGE_DIR "../images/"
#define LAYER_FONT_DIR  "../fonts/"
#define LAYER_MOVIE_NAME "../movies/Intro.movie"

// ---------------------------------------------------------------------------
// DataManager Properties
// ---------------------------------------------------------------------------
//#define _DEF_NVRAM_CRC_CHECK        // CRC Check use
//#define _DEF_NONVOLATILE_DATA_STORAGE // Non-volatile Data Storage use, deprecated
#define NVRAM_UPDATE_COUNTER  (5000000L) // 5secs
#define USE_NVRAM_TYPE        (0)//(1) // 0 for no NVRAM, 1 I2C EEPROM, 2 for SPI NOR Flash
#if USE_NVRAM_TYPE == 1
#define _DEF_NV24LC64
#endif
#define USE_NVRAM_LOADFIRST     (1) // TRUE to Protected NVRAM Node, Load NVRAM first
#define USE_NVRAM_PROTECT_AREA  (0) // 1 to Use Protected Area for MP, 0 for ES
#define USE_FACTORY_RESET       (1) // 1 to use Factory Reset

// ---------------------------------------------------------------------------
// Application Properties
// ---------------------------------------------------------------------------
/* Maintenance */
#define USE_CONFIGIF_MODE               (1)     /* 1 to use the external Configuration Program protocol */
#define USE_DFUOR                       (1)     /* 1 to use Device Firmware Update over RS232c */
#if USE_DFUOR
#define APP_FWINFO_OFFSET               0x23800   /*  FW Info Fusing Adrress Offset */
#define APP_FWINFO_SIZE                 16        /* FW Info byte size */
#define FLASH_PAGE_SIZE                 (0x800)   /* 2048 Bytes */
#define FLASH_TOTAL_SIZE                (0x80000)         /* 512 K */
#define FLASH_TOTAL_PAGE_NUMBER    		  (0x100)  /* 256 PAGES */
#define FIRMWARE_KEY                    "CWM920EJUNE"
/* 2Page for Bootloader, 72Pages for User App, 72Pages for FW Fusing, 110Pages are Reserved */
#define FLASH_TOTAL_USER_PAGE_NUMBER    (72)
#define APP_TABLE_SIZE                  8192          /* FLASH_PAGE_SIZE * 4page */

/* Main Program */
#define FW_DATA_DIR       "../"
#define FW_DATA_NAME      "Main_Application.bin" 
#define FW_DATA_SIZE_MIN  ((FLASH_TOTAL_USER_PAGE_NUMBER-1)*FLASH_PAGE_SIZE)
#define FW_DATA_SIZE_MAX  (FLASH_TOTAL_USER_PAGE_NUMBER*FLASH_PAGE_SIZE)

/* Module Program */
#define MODULE_DATA_DIR      "../"
#define MODULE_DATA_NAME     "Module_Application.bin" 
#define MODULE_DATA_SIZE_MIN (FLASH_PAGE_SIZE)
#define MODULE_DATA_SIZE_MAX (APP_TABLE_SIZE)

/* Voice ROM */
#define VOICE_PAGE_SIZE     256
#define VOICE_DATA_DIR      "../"
#define VOICE_DATA_NAME     "ROMImage.bin" 
#define VOICE_DATA_SIZE_MIN (VOICE_PAGE_SIZE)
#define VOICE_DATA_SIZE_MAX (1024*1024) // 1MBytes
#endif

/* Maintenance */
#define USE_CONFIGIF_MODE               (1)       /* 1 to use the external Configuration Program protocol */

/* Power */
#define USE_POWER_STARTFROMON           (1)       /* 1 to start as pwrON */

/* Touch IC */
#define USE_IMPROVED_TOUCH_SPEED        (0)       /* 1 to use improving touch speed */

/* Audio */
#define USE_I2S_EXTERNAL_CLOCK          (1 && !USE_AMAZON_STK)       /* External Clock is not support STK */
#define USE_TAS2505_AUDIO_CODEC         (1 && !USE_AMAZON_STK)       /* 1 to use TAS2505 Audio Codec & AMP */

/* RTC */
#define USE_DS1374_RTC                (0) /* 1to use DS1374 RTC */
#define USE_DS1374_TRICKLECHARGER     (1 && USE_DS1374_RTC) /* 1 to use Trickle Charger */
#define USE_HW_RTC                    (1 && USE_DS1374_RTC) /* 1 to USE HW RTC in HAL */

/* Syslog */
#define USE_SYSLOG                      (1)   /* 1 to use SYSLOG */

/* CAN Bus  */
#define USE_CAN_BUS                   (0)                 /* 1 to use CAN Bus */
#define USE_CAN_AUTORETRANMISSION     (0 && USE_CAN_BUS)  /* 1 to automatic retransmission mode */
#define USE_CAN_LOW_SPEED             (0 && USE_CAN_BUS)  /* 0: 1Mbps, 
                                                             1: 800Kbps, 
                                                             2: 500Kbps, 
                                                             3: 250Kbps, 
                                                             4: 125Kbps, 
                                                             5: 100Kbps */
// CAN Node Id                                                        
#define CANTP_NODE_ID_HP              (0x020) /* CAN-TP Node Id to Hand-Piece */
#define CANTP_NODE_ID_HIFU            (0x030) /* CAN-TP Node Id to HIFU */
#define CANTP_NODE_ID_SETUP           (0x002) /* CAN-TP Node Id to SETUP Tool */
#define CANTP_NODE_ID_SPAD            (0x010) /* CAN-TP Node Id to S-Pad */
#define CANTP_NODE_ID_T10             (0x012) /* CAN-TP Node Id to T10 */
#define CANTP_NODE_ID_RFM             (0x014) /* CAN-TP Node Id to RFM */   

// ---------------------------------------------------------------------------
// Common Properties
// ---------------------------------------------------------------------------
#define TICK_TIME_uS          787L          /* Timer3, 101.1358025uS base */
#define USE_DOWN_SYSTEM_TICK  (0)           /* 0 for Upward Tick Counter */
#define USE_WATCHDOG          (0)           /* unstoppable Watchdog , was 1 temp */

// ---------------------------------------------------------------------------
// User interface Properties
// ---------------------------------------------------------------------------
#define USE_REQUEST_STATUS              (0)
#define USE_FONT_DRAW                   (1)     /* 1 to use Font */
#define USE_LOADING_CONFIGIRM_FROM_HOST (0)     /* 1 to use confirming the loading step from Host I/F */
#define USE_DOUBLE_FRAME_BUFFER         (0)     /* 1 to use Double Frame Buffer but Double Frame Buffer TBD */
#define USE_INTRO_MOVIE                 (0)     /* 1 to use Intro Movie */

// ---------------------------------------------------------------------------
// HOST INTERFACE Properties
// ---------------------------------------------------------------------------
#define USE_HOSTIF_INPUT_DUMP       (0) // Debug Purpose Only
#define USE_HOSTIF_OUTPUT_DUMP      (0) // Debug Purpose Only
#define USE_HOSTIF_PACKET_CRC       (1) // 1 to use CRC checking 
#define USE_HOSTIF_PAYLOAD_CHECKSUM (1) // 1 to use checksum in payload
#define USE_HOSTIF_PACKET_SEQ_INC   (1) // 1 to use Host I/F Frame number increasement
#define USE_HOSTIF_NOPACKETERR_PROC (1) // 0 to use badpacket process

// ---------------------------------------------------------------------------
// Debugging Purpose Only
// ---------------------------------------------------------------------------
#define USE_DEBUG_RUNTIME   (1 && !defined(NDEBUG))     /* Debug purpose the Runtime Debug. */
#define USE_DEBUG_UI        (1 && USE_DEBUG_RUNTIME)    /* Debug purpose the UI print out */
#define USE_DEBUG_HOST      (1 && USE_DEBUG_RUNTIME)    /* Debug purpose the HOST I/F print out */
#define USE_DEBUG_SYSSTATE  (1 && USE_DEBUG_RUNTIME)    /* Debug purpose the System State print out */
#define USE_DEBUG_MISC      (1 && USE_DEBUG_RUNTIME)
#define USE_DEBUG_TWI       (1 && USE_DEBUG_RUNTIME)    /* Debug purpose the I2C Bus print out */
#define USE_DEBUG_CANTP     (0 && USE_DEBUG_RUNTIME)    /* Debug CAN ISO-TP */
#define USE_DEBUG_CANHOST   (1 && USE_DEBUG_RUNTIME)    /* Debug Host CAN */
#define USE_LCD_TEST_PATTERN        (1) /* USE_WATCHDOG to 0 first */


