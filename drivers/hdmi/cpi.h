/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#ifdef DEV_SUPPORT_CEC

// CPI Registers
//==============
#define REG_CEC_DEVICE_ID           0x80
#define REG_CEC_RELEASE             0x81
#define REG_CEC_SUBSYSTEM_SPECS     0x82
#define REG_CEC_FW                  0x83

#define REG_CEC_DEBUG_0             0x84
#define REG_CEC_DEBUG_1             0x85
#define REG_CEC_DEBUG_2             0x86
#define REG_CEC_DEBUG_3             0x87

#define REG_CEC_TX_INIT             0x88
#define REG_CEC_TX_DEST             0x89

#define REG_CEC_CONFIG_CPI          0x8E

#define REG_CEC_TX_OPCODE           0x8F
#define REG_CEC_TX_OPERAND_0        0x90

#define REG_CEC_TRANSMIT_CONTROL    0x9F

#define REG_CEC_CAPTURE_ID0         0xA2
#define REG_CEC_CAPTURE_ID1         0xA3

#define REG_CEC_INT_ENABLE_0        0xA4
#define REG_CEC_INT_ENABLE_1        0xA5
#define REG_CEC_INT_STATUS_0        0xA6
#define REG_CEC_INT_STATUS_1        0xA7

#define REG_CEC_RX_CONTROL          0xAC
#define REG_CEC_RX_COUNT            0xAD

#define REG_CEC_RX_CMD_HEADER       0xAE
#define REG_CEC_RX_OPCODE           0xAF
#define REG_CEC_RX_OPERAND_0        0xB0

#define REG_CEC_DISCOVERY_CTRL      0xE0
#define REG_CEC_DISCOVERY_STATUS_0  0xE1
#define REG_CEC_DISCOVERY_STATUS_1  0xE2


// CPI Bit Masks
//==============
#define BIT_SHORT_PULSE_DET         0x04
#define BIT_FRAME_RETRANSM_OV       0x02
#define BIT_START_IRREGULAR         0x01
#define BIT_CLR_RX_FIFO_CUR         0x01

#define BIT_CLR_RX_FIFO_ALL         0x02
#define BIT_MSG_ERROR               0x80
#define BIT_SNOOP_EN                0x01
#define BIT_FLUSH_TX_FIFO           0x80

#define BIT_SEND_POLL               0x80
#define BIT_TRANSMIT_CMD            0x10
#define BIT_TX_FIFO_FULL            0x20

#define BIT_START_DISC			   	0x01
#define BIT_CLEAR_DISC			    0x02
#define BIT_DISC_DONE		        0x80


// 0xA6 CPI Interrupt Status Register (R/W)
//=========================================
#define BIT_CEC_LINE_STATE          0x80
#define BIT_TX_MESSAGE_SENT         0x20
#define BIT_TX_HOTPLUG              0x10
#define BIT_POWER_STAT_CHANGE       0x08
#define BIT_TX_FIFO_EMPTY           0x04
#define BIT_RX_MSG_RECEIVED         0x02
#define BIT_CMD_RECEIVED            0x01

// 0xA7 CPI Interrupt Status Register (R/W)
//=========================================
#define BIT_RX_FIFO_OVERRUN         0x08
#define BIT_SHORT_PULSE_DET         0x04
#define BIT_FRAME_RETRANSM_OV       0x02
#define BIT_START_IRREGULAR         0x01

// Logical Address Mapping States
//===============================
#define STATE_MAP_WAIT_TO_SEND  0
#define STATE_MAP_WAIT_ACK      1


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CPI Enums and Constants
//------------------------

#define C_MAX_CMD_SIZE 16

typedef enum
{
    SI_TX_WAITCMD,
    SI_TX_SENDING,
    SI_TX_SEND_ACKED,
    SI_TX_SEND_FAILED
} SI_TxState_t;

typedef enum
{
    SI_CEC_SHORTPULSE       = 0x80,
    SI_CEC_BADSTART         = 0x40,
    SI_CEC_RXOVERFLOW       = 0x20,
    SI_CEC_ERRORS           = (SI_CEC_SHORTPULSE | SI_CEC_BADSTART | SI_CEC_RXOVERFLOW)
} SI_cecError_t;

//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

typedef struct
{
    byte OpCode;
    byte SrcDestAddr;            // Source in upper nybble, dest in lower nybble
    byte ArgCount;
    byte Args[C_MAX_CMD_SIZE];

} SI_CpiData_t;

typedef struct
{
    byte RxState;
    byte TxState;
    byte CEC_Error;

} SI_CpiStatus_t;

// CEC Constants
//==============

#define LA_UNREGISTERED 15
#define CPI_SLAVE_ADDR  0xC0
#define T_AUTO_DISC		100  // May be tweaked if needed


// Macro Definitions
//==================
#define CPI_WriteReg(ADDR, DATA)	I2C_WriteByte(CPI_SLAVE_ADDR, ADDR, DATA)
#define CPI_ReadReg(ADDR) 			I2C_ReadByte(CPI_SLAVE_ADDR, ADDR)

#define MAKE_SRCDEST(Src, Dest)    (( Src << 4) | Dest )


// CEC Device Types
//=================

#define CEC_LOGADDR_TV          0x00
#define CEC_LOGADDR_RECDEV1     0x01
#define CEC_LOGADDR_RECDEV2     0x02
#define CEC_LOGADDR_TUNER1      0x03     // STB1 in Spev v1.3
#define CEC_LOGADDR_DVD1       	0x04     // DVD1 in Spev v1.3
#define CEC_LOGADDR_AUDSYS      0x05
#define CEC_LOGADDR_TUNER2      0x06     // STB2 in Spec v1.3
#define CEC_LOGADDR_TUNER3      0x07     // STB3 in Spec v1.3
#define CEC_LOGADDR_DVD2        0x08     // DVD2 in Spec v1.3
#define CEC_LOGADDR_RECDEV3     0x09
#define CEC_LOGADDR_TUNER4      0x0A     // RES1 in Spec v1.3
#define CEC_LOGADDR_PLAYBACK3   0x0B     // RES2 in Spec v1.3
#define CEC_LOGADDR_RES3        0x0C
#define CEC_LOGADDR_RES4        0x0D
#define CEC_LOGADDR_FREEUSE     0x0E
#define CEC_LOGADDR_UNREG       0x0F

// CEC Messages
//=============
typedef enum                    // CEC Messages
{
    CECOP_FEATURE_ABORT             = 0x00,
    CECOP_IMAGE_VIEW_ON             = 0x04,
    CECOP_TUNER_STEP_INCREMENT      = 0x05,     // N/A
    CECOP_TUNER_STEP_DECREMENT      = 0x06,     // N/A
    CECOP_TUNER_DEVICE_STATUS       = 0x07,     // N/A
    CECOP_GIVE_TUNER_DEVICE_STATUS  = 0x08,     // N/A
    CECOP_RECORD_ON                 = 0x09,     // N/A
    CECOP_RECORD_STATUS             = 0x0A,     // N/A
    CECOP_RECORD_OFF                = 0x0B,     // N/A
    CECOP_TEXT_VIEW_ON              = 0x0D,
    CECOP_RECORD_TV_SCREEN          = 0x0F,     // N/A
    CECOP_GIVE_DECK_STATUS          = 0x1A,
    CECOP_DECK_STATUS               = 0x1B,
    CECOP_SET_MENU_LANGUAGE         = 0x32,
    CECOP_CLEAR_ANALOGUE_TIMER      = 0x33,     // Spec 1.3A
    CECOP_SET_ANALOGUE_TIMER        = 0x34,     // Spec 1.3A
    CECOP_TIMER_STATUS              = 0x35,     // Spec 1.3A
    CECOP_STANDBY                   = 0x36,
    CECOP_PLAY                      = 0x41,
    CECOP_DECK_CONTROL              = 0x42,
    CECOP_TIMER_CLEARED_STATUS      = 0x43,     // Spec 1.3A
    CECOP_USER_CONTROL_PRESSED      = 0x44,
    CECOP_USER_CONTROL_RELEASED     = 0x45,
    CECOP_GIVE_OSD_NAME             = 0x46,
    CECOP_SET_OSD_NAME              = 0x47,
    CECOP_SET_OSD_STRING            = 0x64,
    CECOP_SET_TIMER_PROGRAM_TITLE   = 0x67,     // Spec 1.3A
    CECOP_SYSTEM_AUDIO_MODE_REQUEST = 0x70,     // Spec 1.3A
    CECOP_GIVE_AUDIO_STATUS         = 0x71,     // Spec 1.3A
    CECOP_SET_SYSTEM_AUDIO_MODE     = 0x72,     // Spec 1.3A
    CECOP_REPORT_AUDIO_STATUS       = 0x7A,     // Spec 1.3A
    CECOP_GIVE_SYSTEM_AUDIO_MODE_STATUS = 0x7D, // Spec 1.3A
    CECOP_SYSTEM_AUDIO_MODE_STATUS  = 0x7E,     // Spec 1.3A
    CECOP_ROUTING_CHANGE            = 0x80,
    CECOP_ROUTING_INFORMATION       = 0x81,
    CECOP_ACTIVE_SOURCE             = 0x82,
    CECOP_GIVE_PHYSICAL_ADDRESS     = 0x83,
    CECOP_REPORT_PHYSICAL_ADDRESS   = 0x84,
    CECOP_REQUEST_ACTIVE_SOURCE     = 0x85,
    CECOP_SET_STREAM_PATH           = 0x86,
    CECOP_DEVICE_VENDOR_ID          = 0x87,
    CECOP_VENDOR_COMMAND            = 0x89,
    CECOP_VENDOR_REMOTE_BUTTON_DOWN = 0x8A,
    CECOP_VENDOR_REMOTE_BUTTON_UP   = 0x8B,
    CECOP_GIVE_DEVICE_VENDOR_ID     = 0x8C,
    CECOP_MENU_REQUEST              = 0x8D,
    CECOP_MENU_STATUS               = 0x8E,
    CECOP_GIVE_DEVICE_POWER_STATUS  = 0x8F,
    CECOP_REPORT_POWER_STATUS       = 0x90,
    CECOP_GET_MENU_LANGUAGE         = 0x91,
    CECOP_SELECT_ANALOGUE_SERVICE   = 0x92,     // Spec 1.3A    N/A
    CECOP_SELECT_DIGITAL_SERVICE    = 0x93,     //              N/A
    CECOP_SET_DIGITAL_TIMER         = 0x97,     // Spec 1.3A
    CECOP_CLEAR_DIGITAL_TIMER       = 0x99,     // Spec 1.3A
    CECOP_SET_AUDIO_RATE            = 0x9A,     // Spec 1.3A
    CECOP_INACTIVE_SOURCE           = 0x9D,     // Spec 1.3A
    CECOP_CEC_VERSION               = 0x9E,     // Spec 1.3A
    CECOP_GET_CEC_VERSION           = 0x9F,     // Spec 1.3A
    CECOP_VENDOR_COMMAND_WITH_ID    = 0xA0,     // Spec 1.3A
    CECOP_CLEAR_EXTERNAL_TIMER      = 0xA1,     // Spec 1.3A
    CECOP_SET_EXTERNAL_TIMER        = 0xA2,     // Spec 1.3A
    CECOP_ABORT                     = 0xFF
} CEC_OPCODE_t;


// Operands for <Feature Abort> Opcode

typedef enum
{
    CECAR_UNRECOG_OPCODE            = 0x00,
    CECAR_NOT_CORRECT_MODE,
    CECAR_CANT_PROVIDE_SOURCE,
    CECAR_INVALID_OPERAND,
    CECAR_REFUSED
} CEC_ABORT_REASON_t;

// Function Prototypes
//====================

void InitCPI (void);
void MapLogicalAddresses(void);
void AssignInitiatorLA(void);
void CEC_ReportPhysicalAddress(void);
void InitCaptureID_Regs(void);
bool ReadCPI_Message(SI_CpiData_t *);
void CEC_ParseRXMessage(SI_CpiData_t *);
void CEC_HandleFeatureAbort(SI_CpiData_t *);
void GetCPI_Status(SI_CpiStatus_t *);
void SendCPI_Message(SI_CpiData_t *);
void CEC_SendActiveSource(void);
void CEC_SendImageViewOn(byte);
void CPI_ReadBlock(byte, word, byte *);
void CPI_WriteBlock(byte, word, byte *);
void CEC_Monitor(void);
void CEC_OneTouchPlay(void);
void CEC_SendFeatureAbort(SI_CpiData_t *, byte);

#endif