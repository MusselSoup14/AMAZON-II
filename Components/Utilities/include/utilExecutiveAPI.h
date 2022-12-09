/**
  ******************************************************************************
  * @file    utilExecutiveAPI.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    24-March-2016
  * @brief   This file provides the main procedure
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
#include "common.h"
#include "utilStructs.h"

#ifndef _UTILEXECUTIVEAPI_H_
#define _UTILEXECUTIVEAPI_H_


// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ---------------------------------------------------------------------------
// Sentinel value for no timeout
// ---------------------------------------------------------------------------
#define UE_INFINITE 0xFFFF

// ---------------------------------------------------------------------------
// shared resource management routines for single-thread system
// ---------------------------------------------------------------------------
#define RESOURCE_BUSY_SET(a) utilExecRscBusySet(a)
#define RESOURCE_BUSY_GET(a) utilExecRscBusyGet(a)
#define RESOURCE_BUSY_CLEAR(a) utilExecRscBusyClear(a)

// ---------------------------------------------------------------------------
// shared resource management routines for multi-thread system
// (defined to nothing in this single-thread executive)
// ---------------------------------------------------------------------------
#define RESOURCE_RESERVE(a)
#define RESOURCE_RELEASE(a)

// --------------------------------CONFIGURATION------------------------------
// Executive event user list - configure with an entry for each component that
// uses executive events.  See component specifications for event names.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* -------------- BEGIN CUSTOM AREA [020] utilExecutiveAPI.h -----------------
 * CONFIGURATION: Add event users here */
  euUSER_INPUT,
/* ---------------------------- END CUSTOM AREA --------------------------- */
  euINVALID                          // Used for enumerated type range checking (DO NOT REMOVE)
} eEVENT_USER;

// ---------------------------------------------------------------------------
// Types of events that executive can generate
// ---------------------------------------------------------------------------
typedef enum
{
  etTIMER,                           // Event triggered by timer expiration
  etRESOURCE,                        // Event triggered by shared resource aquisition
  etINVALID                          // Used for enumerated type range checking (DO NOT REMOVE)
} eEVENT_TYPE;

// --------------------------------CONFIGURATION------------------------------
// Executive tick time in microseconds - determines time granularity of timer
// events and counters.
// --------------------------------CONFIGURATION------------------------------ 
#define uS_TO_TICKS(t) (DWORD)(((DWORD)(t)+TICK_TIME_uS-1L) / TICK_TIME_uS)
//#define mS_TO_TICKS(t) ((DWORD)t)

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

// --------------------------------CONFIGURATION------------------------------
// Tick counter list - configure with an entry for each time (tick) counter
// used in the system.  See component specifications for counter names.
// The single-thread executive supports a maximum of 16 counters.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
  coLED_APP,
  coDATA_MGR_UPDATE,
    
  coHAL_UI_KEY,
  coHAL_UI_TOUCH,
  coHAL_UI_LVGL,

  coSOUND_PLAY,

  coUI_STATECHANGE_TIMEOUT,
  coUI_CLEAR,
  coUI_RENDER_DELAY,
  coUI_ACTIVE_DRAW,
  coUI_ACTIVE_CLEAR,
  coUI_COUNTER_1SEC, // Nothing yet
  coUI_COUNTER_100MSEC, // Splash screen loading
  coUI_COUNTER_SYSUPDATE, // System Status Update  
  
  coHOSTIF_SERIAL_0,
  coHOSTIF_SERIAL_2,
  coHOSTIF_SERIAL_3,
#if USE_CAN_BUS  
  coHOSTIF_CANBUS,
#endif   
  coHOST_STATUS_REQ,  
  coHOSTIF_ACTIVE_LED,  
  coUI_FACTORY_ACTIVE, // Factory Mode Enter Counter
  coTIMESTAMP, 
    
  coINVALID                          // Used for enumerated type range checking (DO NOT REMOVE)
} eCOUNTERS;

// --------------------------------CONFIGURATION------------------------------
// Signal group list - configure with an entry for each component that
// uses signals.  See component specifications for group and signal names.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
  sgSYSTEM,
  sgSERIAL_IO0,
  sgSERIAL_IO2,
  sgSERIAL_IO3,
#if USE_CAN_BUS  
  sgCANSPI,
  sgCANTP,
#endif  
  sgSETUP_IF,
  sgGUI_IF,
    
  sgUI,  
  sgMISC,
  sgPWR_MGR,

  sgINVALID                          // Used for enumerated type range checking (DO NOT REMOVE)
} eSIGNAL_GROUPS;

//--------------------------------CONFIGURATION-------------------------------
// signals for System Global (sgSYSTEM)
//--------------------------------CONFIGURATION-------------------------------
#define SIGNAL_SYSTEM_INITIALIZED       (0x1 << 0) // system initialization is finished

//--------------------------------CONFIGURATION-------------------------------
// signals for serial IO group
//--------------------------------CONFIGURATION-------------------------------
#define SI_SIGNAL_READ_THRESHOLD    (0x1 << 0)
#define SI_SIGNAL_READ_FULL         (0x1 << 1)
#define SI_SIGNAL_WRITE_COMPLETE    (0x1 << 2)

//--------------------------------CONFIGURATION-------------------------------
// signals for Device Bus CAN group (sgCANSPI)
//--------------------------------CONFIGURATION-------------------------------
#define CANIO_SIGNAL_READ_FULL      (0x1 << 0)

//--------------------------------CONFIGURATION-------------------------------
// signals for CAN-TP group (sgHOSTCAN_xxx)
//--------------------------------CONFIGURATION-------------------------------
#define CANTP_SIGNAL_READ_THRESHOLD (0x1 << 0)
#define CANTP_SIGNAL_READ_FULL      (0x1 << 1)
#define CANTP_SIGNAL_WRITE_COMPLETE (0x1 << 2)

//--------------------------------CONFIGURATION-------------------------------
// signals for Config I/F Group
//--------------------------------CONFIGURATION-------------------------------
#define SIGNAL_CONF_SETUP_ENABLE      (0x0001 << 0)
#define SIGNAL_CONF_CONNECTED         (0x0001 << 1)

//--------------------------------CONFIGURATION-------------------------------
// signals for Host I/F
//--------------------------------CONFIGURATION-------------------------------
#define SIGNAL_GUI_READY               (0x0001 << 0) // GUI Image loading done
#define SIGNAL_GUI_DEVINFO_RECEIVED    (0x0001 << 1)
#define SIGNAL_GUI_CONNECTED           (0x0001 << 15) // Connected(Bind). Ready to send init BIND -> UIM IMAGE Loading -> READY

//--------------------------------CONFIGURATION--------------------
// signals for POWER group
//--------------------------------CONFIGURATION--------------------
#define SIGNAL_PM_RESET                         (0x1 << 0)
#define SIGNAL_PM_POWER_TRANSITIONING           (0x0001 << 1) // Power transitioning

//--------------------------------CONFIGURATION-------------------------------
// signals for Run-time Error group
//--------------------------------CONFIGURATION-------------------------------
// signals for Run-time Error group
#define ERR_NONE                      (0x0000)
#define ERR_UNKNOWN                   (0x0001 << 15)

#define ERR_ALL                       0xFFFF

//--------------------------------CONFIGURATION-------------------------------
// signals for Run-time Info group
//--------------------------------CONFIGURATION-------------------------------
#define INFO_NONE                       (0x0000)

#define INFO_ALL                        0xFFFF

//--------------------------------CONFIGURATION-------------------------------
// signals for MISC
//--------------------------------CONFIGURATION-------------------------------
#define SIGNAL_MISC_DFUMAIN_AVAIL           (0x0001 << 0)     /* FW_DATA_NAME Found and Available */
#define SIGNAL_MISC_DFUMOD_AVAIL            (0x0001 << 1)     /* MODULE_DATA_NAME Found and Available */
#define SIGNAL_MISC_DFUVOICE_AVAIL          (0x0001 << 2)     /* VOICE_DATA_NAME Found and Available */            

#define SIGNAL_MISC_DFUMAIN_ENABLED         (0x0001 << 3)     /* DFU Enable ACKED (Enabled) */

#define SIGNAL_MISC_DFUVOICE_ENABLED            (0x0001 << 4)     /* DFU Enable ACKED (Enabled) */
#define SIGNAL_MISC_DFUVOICE_CHIPERASE_STARTED  (0x0001 << 5)     /* Chip Erased ACKED (Started) */
#define SIGNAL_MISC_DFUVOICE_CHIPERASE_FINISHED (0x0001 << 6)    /* Chip Erased Finished */
#define SIGNAL_MISC_DFUVOICE_CHIPERASE_FAILED   (0x0001 << 7)     /* Chip Erased Failed */

#define SIGNAL_MISC_DFU_SEND_ACK            (0x0001 << 13)    /* DATA Send ACKED */
#define SIGNAL_MISC_MASS_ACTIVATED          (0x0001 << 14)    /* Activated USB Mass-Storage */
#define SIGNAL_MISC_HOST_REBOOT_ACK         (0x0001 << 15)    /* REBOOT ACKED */


//--------------------------------CONFIGURATION-------------------------------
// signals for UI
//--------------------------------CONFIGURATION-------------------------------
#define SIGNAL_UI_FACTORY_ACTIVATED       (0x0001 << 0)


// --------------------------------CONFIGURATION------------------------------
// Pulse group list - configure with an entry for each component that
// uses pulses.  See component specifications for group and pulse names.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* -------------- BEGIN CUSTOM AREA [035] utilExecutiveAPI.h -----------------
 * CONFIGURATION: Add pulse groups here.  Note: The order of these values
 * must match the order used in m_acPulseType */
  pgSYS_MGR,
  pgSYS_MGR_UI_WATCHER, // set when pgSYS_MGR pulses are set

  pgMISC,
  pgMISC_UI_WATCHER,  
/* ---------------------------- END CUSTOM AREA --------------------------- */
  pgINVALID                          // Used for enumerated type range
                                       // checking (DO NOT REMOVE)
} ePULSE_GROUPS;

//--------------------------------CONFIGURATION-------------------------------
// Pulses for System State group
//--------------------------------CONFIGURATION-------------------------------
#define PULSE_SYS_ERROR_CHANGED               (0x0001 << 0)
#define PULSE_SYS_INFO_CHANGED                (0x0001 << 1)

//--------------------------------CONFIGURATION-------------------------------
// Pulses for MISC group
//--------------------------------CONFIGURATION-------------------------------
// NTD

//--------------------------------CONFIGURATION-------------------------------
// Pulses for System manager group
//--------------------------------CONFIGURATION-------------------------------

// --------------------------------CONFIGURATION------------------------------
// Shared resource list - configure with an entry for each shared resource
// used in the system.  See component specifications for resource names.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
  reDEVICE_BUS,
    
  reINVALID                          // Used for enumerated type range
                                     // checking (DO NOT REMOVE)
} eRESOURCES;

// --------------------------------CONFIGURATION------------------------------
// Variable list - configure with an entry for each component that uses
// variables. See component specifications for variable name.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* ---------------------------- BEGIN CUSTOM AREA  --------------------------
 * CONFIGURATION: Add Variable groups here */
  vgDEBUG,
  
  vgINVALID              // ** Used for enumerated type range checking
/* ---------------------------- END CUSTOM AREA --------------------------- */    
} eVAR_GROUPS;

//--------------------------------CONFIGURATION-------------------------------
// Vars Runtime Error Group
//--------------------------------CONFIGURATION-------------------------------
#define VG_DEBUG_NONE (0x0000)

// --------------------------------CONFIGURATION------------------------------
// Message queue list - configure with an entry for each message queue used in
// the system.  See component specifications for queue names.
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* -------------- BEGIN CUSTOM AREA [060] utilExecutiveAPI.h -----------------
 * CONFIGURATION: Add message ques here */
  mqUSER_INPUT, // appUserinterface components Consumer only
  mqUSER_TOUCH, // appUserinterface components Consumer only
/* ---------------------------- END CUSTOM AREA --------------------------- */
  mqINVALID                          // Used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eMESSAGE_QUEUES;


// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
/**
 *  @brief      Initializes the internal state of the single-thread executive
                    
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecInitialize(void);

/**
 *  @brief      Release the internal state of the single-thread executive
                    
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecRelease(void);

/**
 *  @brief      This API sets the specified signal or signals in the specified group.
                    
 *  @param[in]  eGroup   : The signal group in which the signal flag(s) will be set
                        wSignals : The signal flag(s) that will be set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecSignalSet(eSIGNAL_GROUPS eGroup, WORD wSignals);

/**
 *  @brief      This API clears the specified signal or signals from the specified group.
                    
 *  @param[in]  eGroup   : The signal group from which the signal flag(s) will be cleared
                        wSignals : The signal flag(s) that will be cleared
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecSignalClear(eSIGNAL_GROUPS eGroup, WORD wSignals);

/**
 *  @brief      This API reads the current state of the signal flags in the specified group.
                    
 *  @param[in]  eGroup : The signal group from which the signal flags will be read
                        wMask  : A mask for the signals to be read (only signals that are set in the mask will be read)
 *  @param[out] None
 *  @return     State of the signals selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecSignalGet(eSIGNAL_GROUPS eGroup, WORD wMask);

/**
 *  @brief      This API suspends execution of the caller until one or more of the signals
                    in the specified group are set, or until the specified timeout expires. 
                    The caller is suspended in a loop (busy wait).
                    
 *  @param[in]  wMask  : A mask for the signals to be monitored (only signals that are set in the mask will be checked)
                        dwTicks : Number of ticks to wait for a signal
 *  @param[out] None
 *  @return     State of the signals selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecSignalWaitAny(eSIGNAL_GROUPS eGroup,
                           WORD           wMask,
                           DWORD           dwTicks);

/**
 *  @brief      This API suspends execution of the caller until all of the signals in the
                    specified group are set, or until the specified timeout expires.  The
                    caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eGroup : The signal group from which the signal flags will be monitored
                        wMask  : A mask for the signals to be monitored (only signals that are set in the mask will be checked)
                        dwTicks : Number of ticks to wait for the signals
 *  @param[out] None
 *  @return     State of the signals selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecSignalWaitAll(eSIGNAL_GROUPS eGroup,
                           WORD           wMask,
                           DWORD           dwTicks);

/**
 *  @brief      This API sets the specified pulse or pulses in the specified group.
                    
 *  @param[in]  eGroup  : The pulse group in which the pulse flag(s) will be set
                        wPulses : The pulse flag(s) that will be set
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecPulseSet(ePULSE_GROUPS eGroup,
                      WORD          wPulses);

/**
 *  @brief      This API clears the specified pulse or pulses from the specified group.
                    
 *  @param[in]  eGroup  : The pulse group from which the pulse flag(s) will be cleared
                        wPulses : The pulse flag(s) that will be cleared
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecPulseClear(ePULSE_GROUPS eGroup,
                        WORD          wPulses);

/**
 *  @brief      This API reads the current state of the pulse flags in the specified group.
                    
 *  @param[in]  eGroup : The pulse group from which the pulse flags will be read
                        wMask  : A mask for the pulses to be read (only pulses that are set in the mask will be read)
 *  @param[out] None
 *  @return     State of the pulses selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecPulseGet(ePULSE_GROUPS eGroup,
                      WORD          wMask);

/**
 *  @brief      This API suspends execution of the caller until one or more of the pulses
                    in the specified group are set, or until the specified timeout expires. 
                    The caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eGroup : The signal group in which the signal flags will be monitored
                        wMask  : A mask for the pulses to be monitored (only pulses that are set in the mask will be checked)
                        dwTicks : Number of ticks to wait for a pulse
 *  @param[out] None
 *  @return     State of the pulses selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecPulseWaitAny(ePULSE_GROUPS eGroup,
                          WORD          wMask,
                          DWORD          dwTicks);

/**
 *  @brief      This API suspends execution of the caller until all of the pulses in the
                    specified group are set, or until the specified timeout expires.  The
                    caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eGroup : The pulse group from which the pulse flags will be monitored
                        wMask  : A mask for the pulses to be monitored (only pulses that are set
                                     in the mask will be checked)
                        dwTicks : Number of ticks to wait for the signals
 *  @param[out] None
 *  @return     State of the pulses selected by the mask
 *  @note       None
 *  @attention  None
 */
WORD utilExecPulseWaitAll(ePULSE_GROUPS eGroup,
                          WORD          wMask,
                          DWORD          dwTicks);

/**
 *  @brief      This API sets the value of the specified tick counter.
                    
 *  @param[in]  eCounter : Identifier of counter to set
                dwTicks   : Value to set timer to, in executive ticks.
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecCounterSet(eCOUNTERS eCounter,
                              DWORD      dwTicks);

/**
 *  @brief      This API reads the current value of the specified tick counter.
                    
 *  @param[in]  eCounter : Identifier of counter to read
 *  @param[out] None
 *  @return     Current value of timer, in executive ticks
 *  @note       None
 *  @attention  None
 */
DWORD utilExecCounterGet(eCOUNTERS eCounter); 

/**
 *  @brief      This API sends a message to the specified queue.
                    
 *  @param[in]  eQueue  : Queue to send message to
                        pacData : Pointer to message data
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecMsgSend(eMESSAGE_QUEUES eQueue,
                     PBYTE           pacData);

/**
 *  @brief      This API reads a message from the specified queue.
                    
 *  @param[in]  eQueue  : Queue to read message from                        
 *  @param[out] pacData : Pointer where message data will be stored
 *  @return     rcSUCCESS if a message was read, rcERROR if queue was empty
 *  @note       None
 *  @attention  None
 */
eRESULT utilExecMsgRead(eMESSAGE_QUEUES eQueue,
                        PBYTE           pacData);

/**
 *  @brief      This API clears all messages from the specified queue.
                    
 *  @param[in]  eQueue : Queue to clear
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecMsgClear(eMESSAGE_QUEUES eQueue);

/**
 *  @brief      This API checks if a message is present in the specified queue.
                    
 *  @param[in]  eQueue : Queue to check for messages
 *  @param[out] None
 *  @return     TRUE if there is a message in the queue, FALSE otherwise
 *  @note       None
 *  @attention  None
 */
BOOL utilExecMsgPresent(eMESSAGE_QUEUES eQueue);

/**
 *  @brief      This API suspends execution of the caller until a message is received in
                    the specified queue.  The caller is suspended in a loop (busy wait).
                    
 *  @param[in]  eQueue  : Queue to send message to
                        wTicks  : Time to wait for message before returning                        
 *  @param[out] pacData : Pointer to message data
 *  @return     rcSUCCESS if a message was read, rcERROR if queue was empty
 *  @note       None
 *  @attention  None
 */
eRESULT utilExecMsgWait(eMESSAGE_QUEUES eQueue,
                        WORD            wTicks,
                        PBYTE           pacData);

/**
 *  @brief      This API sets the specified variable.
                    
 *  @param[in]  eVar: the variable flag
                        wValue: the WORD value for variable
 *  @param[out] None
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void  utilExecVarSet (eVAR_GROUPS eVar, WORD wValue);

/**
 *  @brief      This API gets the specified variable.
                    
 *  @param[in]  eVar: the variable flag
                        wValue: the WORD value for variable
 *  @param[out] None
 *  @return     Return: Value the WORD value of variable
 *  @note       None
 *  @attention  None
 */
WORD  utilExecVarGet (eVAR_GROUPS eVar);

/**
 *  @brief      This API marks the specified shared resource as busy.
                    
 *  @param[in]  eResource : The resource to be marked busy (reserved)
 *  @param[out] None
 *  @return     rcSUCCESS if resource not busy, rcERROR if resource was already marked busy
 *  @note       None
 *  @attention  None
 */
eRESULT utilExecRscBusySet(eRESOURCES eResource);

/**
 *  @brief      This API reads the current state of the specified shared resource.
                    
 *  @param[in]  eResource : The resource to be read (reserved)
 *  @param[out] None
 *  @return     TRUE  if resource is busy, FALSE if resource is not
 *  @note       None
 *  @attention  None
 */
BOOL utilExecRscBusyGet(eRESOURCES eResource);

/**
 *  @brief      This API clears the busy status for the specified shared resource.
                    
 *  @param[in]  eResource : The resource to be cleared (released)
 *  @param[out] pacData : Pointer to message data
 *  @return     None
 *  @note       None
 *  @attention  None
 */
void utilExecRscBusyClear(eRESOURCES eResource);


#endif // _UTILEXECUTIVEAPI_H_
