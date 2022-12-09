/**
  ******************************************************************************
  * @file    utilHostIfCommandTable.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2017
  * @brief   This file provides the Host interface command
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
  * <h2><center>&copy; Copyright 1997-2017 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _UTILHOSTIFCOMMANDTABLE_H
#define _UTILHOSTIFCOMMANDTABLE_H

#include "common.h"

// ---------------------------------------------------------------------------
// Host Interface command mask
// ---------------------------------------------------------------------------
#define HIF_MSK_RSP   0x0100 // Mask for Reponse
#define HIF_MSK_NOTI  0x0200

// ---------------------------------------------------------------------------
// Host Interface SETUP commands
// ---------------------------------------------------------------------------
typedef enum
{
  hifSETUP_INVALID // Sentinel  
}GCC_PACKED eHIF_SETUP_CMD;

// ---------------------------------------------------------------------------
// Host Interface GUI commands (C:Client/GUI, H:Host/Main)
// ---------------------------------------------------------------------------
typedef enum
{
  /* Connection */
  hifGUI_CTH_ConnectionSet,       /* C->H. Connection Request to Host */
  hifGUI_CTH_HostDevInfoGet,      /* C->H. Host Device Information Get */
  hifGUI_CTH_ReadySet,           /* C->H. Loading finished notification to Host */
  hifGUI_CTH_Reboot,              /* C->H. Request Host Program reboot */

  /* DFU Host */
  hifGUI_CTH_DFUHost_Enable,       /* C->H. Host Program DFU Enable Request */
  hifGUI_CTH_DFUHost_Send,         /* C->H. Host Program DFU Data Send  */

  /* DFU Voice */
  hifGUI_CTH_DFUVoice_Enable,        /* C->H. Voice IC DFU Enable Request */
  hifGUI_CTH_DFUVoice_ChipErase,     /* C->H. Voice IC ChipErase Reuqest */
  hifGUI_CTH_DFUVoice_Send,          /* C->H. Voice IC DFU Data Send */
  hifGUI_HTC_DFUVoice_EnableNoti,    /* H->C. Noti. to Voice IC DFU Data Enable */
  hifGUI_HTC_DFUVoice_ChipEraseNoti, /* H->C. Noti. to Voice IC ChipErase Reuqest */

  /* DFU Module (Reserved) */
  hifGUI_CTH_DFUModule_Enable,        /* C->H. Additional Module DFU Enable Request */
  hifGUI_CTH_DFUModule_Send,          /* C->H. Additional Module DFU Data Send */
  hifGUI_HTC_DFUModule_EnableNoti,    /* H->C. Noti. to Additional Module DFU Enable Request */

  /* System Status */
  hifGUI_CTH_SysStatus_Get,     /* C->H. System Status Request */
  hifGUI_HTC_SysStatus_Noti,    /* H->C. System Status Noti. */
  hifGUI_CTH_Therapy_ModeSet,   /* C->H. Therapy Mode Change Request */
  hifGUI_CTH_Therapy_ReadySet,  /* C->H. Therapy Ready/Standby Request */
  hifGUI_CTH_Therapy_StartSet,  /* C->H. Therapy Start/Stop Request */
  hifGUI_CTH_Therapy_RSV0,      /* C->H. Reserved */
  hifGUI_CTH_Therapy_RSV1,      /* C->H. Reserved */
  hifGUI_CTH_Therapy_RSV2,      /* C->H. Reserved */
  hifGUI_CTH_Therapy_RSV3,      /* C->H. Reserved */
  hifGUI_CTH_Therapy_RSV4,      /* C->H. Reserved */

  /* Virtual Event Noti. */
  hifGUI_HTC_KeyEvent_Noti,     /* H->C. Key Event Noti. */

  /* Error/Info Clear. */
  hifGUI_CTH_Error_Clear,       /* C->H. Error Flags Clear Request. */
  hifGUI_CTH_Info_Clear,        /* C->H. Info Flags Clear Request */

  /* Parameter Set */
  hifGUI_CTH_Parameter_Set,     /* C->H. System Parameter Set Request */
  hifGUI_CTH_Parameter_IncDec,  /* C->H. System Parameter Inc/Dec Request */

  /* ------------------------- Client(GUI Module) Simulator Command START ----------------------------- */
  /* Client(GUI Module) Device Diagnotic */
  hifGUI_HTC_ClientDiagDev_Enter,
  hifGUI_HTC_ClientDiagDev_LEDGet,
  hifGUI_HTC_ClientDiagDev_LEDSet,
  hifGUI_HTC_ClientDiagDev_LCDGet,
  hifGUI_HTC_ClientDiagDev_LCDSet,
  hifGUI_HTC_ClientDiagDev_AudioGet,
  hifGUI_HTC_ClientDiagDev_AudioSet,
  hifGUI_HTC_ClientDiagDev_AudioPlay,
  hifGUI_HTC_ClientDiagDev_RTCGet,
  hifGUI_HTC_ClientDiagDev_RTCSet,
  hifGUI_HTC_ClientDiagDev_MonitorGet,

  /* Client(GUI Module) System Configuration */
  hifGUI_HTC_ClientSysConf_MiscGet,
  hifGUI_HTC_ClientSysConf_MiscSet,
  hifGUI_HTC_ClientSysConf_ElapTimeReset,
  hifGUI_HTC_ClientSysConf_FactoryReset,

  /* Client(GUI Module) System Data */
  hifGUI_HTC_ClientSysData_Get,
  hifGUI_HTC_ClientSysData_Set,

  /* Client(GUI Module) Bus Debugging */
  hifGUI_HTC_ClientDebugBus_SoCRead,
  hifGUI_HTC_ClientDebugBus_SoCWrite,
  hifGUI_HTC_ClientDebugBus_TwiRead,
  hifGUI_HTC_ClientDebugBus_TwiWrite,
  hifGUI_HTC_ClientDebugBus_SPIRead,
  hifGUI_HTC_ClientDebugBus_SPIWrite,
  hifGUI_HTC_ClientDebugBus_UARTRead,
  hifGUI_HTC_ClientDebugBus_UARTWrite,
  /* ------------------------- Client(GUI Module) Simulator Command END ----------------------------- */
  
  hifGUI_INVALID // Sentinel  
}GCC_PACKED eHIF_GUI_CMD;

#endif // _UTILHOSTIFCOMMANDTABLE_H
