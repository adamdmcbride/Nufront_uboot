/***********************************************************************************/
/*  Copyright (c) 2002-2009, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

// Standard C Library
//#include <stdio.h>

// Needs to be rearranged and minimized
//#include "MCU_Regs.h"
#include "SIIdefs.h"
#include "SIITypeDefs.h"
//#include "AMF_Lib.h"
#include "SIIConstants.h"
#include "externals.h"
#include "SIIMacros.h"
#include "SIITPI_Regs.h"
#include "SIITPI_Access.h"
#include "SIITPI.h"
//#include "util.h"
#include "SIIAV_Config.h"
//#include "i2c_master_sw.h"
#include "cpi.h"

// May be empty depending on project definitions
#include "SIIEDID.h"
#include "SIIHDCP.h"
#include "eHDMI.h"

static void TxHW_Reset(void);
static bool StartTPI(void);

static void ForceUsbIdSwitchOpen (void);
static void ReleaseUsbIdSwitchOpen (void);

static bool DisableInterrupts(byte);

static bool TestHPD_Connected(void);

static bool SetPreliminaryInfoFrames(void);

#ifdef MHL_CABLE_HPD
static void OnMhlCableConnected(void);
static void OnMhlCableDisconnected(void);
#endif

static void OnHdmiCableConnected(void);
static void OnHdmiCableDisconnected(void);

static void OnDownstreamRxPoweredDown(void);
static void OnDownstreamRxPoweredUp(void);

//static void TxPowerState(byte powerState);

#define T_EN_TPI       	10
#define T_HPD_DELAY    	10

#ifdef MHL_CABLE_HPD
static bool mhlCableConnected;
#endif

bool tmdsPoweredUp;
static bool hdmiCableConnected;
static bool dsRxPoweredUp;

//zhy + Begin
void VideoModeSetbymain(void);
void AudioModeSetbymain(void);
//zhy + End
//void TxPowerState(byte powerState) 
//{
//	TPI_DEBUG_PRINT(("TX Power State D%d\n", (int)powerState));
//	ReadModifyWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_MASK, powerState);
//}

//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : TPI_Init()
//
// PURPOSE       : TPI initialization: HW Reset, Interrupt enable.
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS      :   TRUE
//
//////////////////////////////////////////////////////////////////////////////
bool TPI_Init(void)
{
	int i = 0;

	i2c_master_init(0x39);

	TPI_TRACE_PRINT((">>TPI_Init()\n"));

	printf("TPI Firmware Version ");
	for (i=0; i < (int)sizeof(TPI_FW_VERSION); i++) {
		printf("%c", TPI_FW_VERSION[i]);
	}
	printf("\n");

	tmdsPoweredUp = FALSE;
	hdmiCableConnected = FALSE;
	dsRxPoweredUp = FALSE;

#ifdef DEV_SUPPORT_EDID
	edidDataValid = FALSE;							// Move this into EDID_Init();
#endif

	TxHW_Reset();									// Toggle TX reset pin

	if (StartTPI())									// Enable HW TPI mode, check device ID
	{
#ifdef DEV_SUPPORT_HDCP
		HDCP_Init();
#endif

#ifdef DEV_SUPPORT_CEC
		InitCPI();
#endif

#ifdef DEV_SUPPORT_EHDMI 
		EHDMI_Init();
#endif


#if defined SiI9232_OR_SiI9236
		OnMhlCableDisconnected();					// Default to USB Mode.
#endif

		return TRUE;
	}

	return FALSE;
}


#if defined SiI9232_OR_SiI9236
static void ForceUsbIdSwitchOpen (void) {
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x90, BIT_0, 0x00);				// Disable discovery
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x95, BIT_6, BIT_6);				// Force USB ID Switch
	WriteIndexedRegister(INDEXED_PAGE_0, 0x92, 0x86);								// Force USB, CBUS discovery
	WriteIndexedRegister(INDEXED_PAGE_0, 0x93, 0x0C);								// Disable CBUS pull-up during RGND measurement
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x79, BIT_5 | BIT_4, BIT_4);		// Force HPD to 0 when not in MHL mode.
	}


static void ReleaseUsbIdSwitchOpen (void) {
	DelayMS(25);
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x95, BIT_6, 0x00);
	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x90, BIT_0, BIT_0);				// Enable discovery
	}
#endif


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   HotPlugService()
//
// PURPOSE      :   Implement Hot Plug Service Loop activities
//
// INPUT PARAMS :   None
//
// OUTPUT PARAMS:   void
//
// GLOBALS USED :   LinkProtectionLevel
//
// RETURNS      :   An error code that indicates success or cause of failure
//
//////////////////////////////////////////////////////////////////////////////

void HotPlugService (void)
{
	TPI_TRACE_PRINT((">>HotPlugService()\n"));

	DisableInterrupts(0xFF);

#ifndef RX_ONBOARD
//#ifdef SYS_BOARD_FPGA
	//printf("** Only 480P supported: be sure input is 480P.");
	vid_mode = 2;												//480P for now
//#else
//	vid_mode = EDID_Data.VideoDescriptor[0];					// use 1st mode supported by sink
//#endif
#endif

	//zhy Modify for hotplug Begin
	VideoModeSetbymain();	

	if (IsHDMI_Sink())											// Set audio only if sink is HDMI compatible
	{
		AudioModeSetbymain();
	}
	else
	{
		SetAudioMute(AUDIO_MUTE_MUTED);
	}

	#if 0
	InitVideo(vid_mode, X1, AFTER_INIT, NO_3D_SUPPORT);			// Set PLL Multiplier to x1 upon power up

	TxPowerState(TX_POWER_STATE_D0);

	if (IsHDMI_Sink())											// Set InfoFrames only if HDMI output mode
	{
		SetPreliminaryInfoFrames();
		SetBasicAudio();										// set audio interface to basic audio (an external command is needed to set to any other mode
		dvi_mode = FALSE;
	}
	else
	{
		SetAudioMute(AUDIO_MUTE_MUTED);
		dvi_mode = TRUE;
	}

#ifdef DEV_SUPPORT_EHDMI
//	EHDMI_ARC_Common_Enable();
	EHDMI_ARC_Common_With_HEC_Enable();
#endif

	EnableInterrupts(HOT_PLUG_EVENT | RX_SENSE_EVENT | AUDIO_ERROR_EVENT | SECURITY_CHANGE_EVENT | V_READY_EVENT | HDCP_CHANGE_EVENT);

	// This check needs to be changed to if HDCP is required by the content... once support has been added by RX-side library.
	if (HDCP_TxSupports == TRUE)
	{
#ifdef USE_BLACK_MODE
		SetInputColorSpace (INPUT_COLOR_SPACE_BLACK_MODE);
		TPI_DEBUG_PRINT (("TMDS -> Enabled\n"));
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK, LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE);
#else	// AV MUTE
		TPI_DEBUG_PRINT (("TMDS -> Enabled (Video Muted)\n"));
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK, LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE | AV_MUTE_MUTED);
#endif
		tmdsPoweredUp = TRUE;
	}
	else
	{
		EnableTMDS();
	}
	#endif
	EnableInterrupts(HOT_PLUG_EVENT | RX_SENSE_EVENT | AUDIO_ERROR_EVENT | SECURITY_CHANGE_EVENT | V_READY_EVENT | HDCP_CHANGE_EVENT);
	//zhy Modify for hotplug End
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : HW_Reset()
//
// PURPOSE       : Send a
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : Void
//
//////////////////////////////////////////////////////////////////////////////

static void TxHW_Reset(void)
{
	TPI_TRACE_PRINT((">>TxHW_Reset()\n"));

	TXHAL_InitPreReset();
/*
#ifdef SCL_LOW_DURING_RESET
	PinI2CSCL = LOW;
	DelayMS(T_SCLLOW_DELAY_RESET);
#endif

	CYC_MICRO_RESET = LOW;
	DelayMS(TX_HW_RESET_PERIOD);
	CYC_MICRO_RESET = HIGH;

#ifdef F_9136
	RX_HW_Reset = LOW;
	DelayMS(RX_HW_RESET_PERIOD);
	RX_HW_Reset = HIGH;
#endif

#ifdef SCL_LOW_DURING_RESET
	DelayMS(T_RESET_DELAY_SCLHIGH);
	PinI2CSCL = HIGH;
#endif
*/
	TXHAL_InitPostReset();

	// Does this need to be done for every chip? Should it be moved into TXHAL_InitPostReset() for each applicable device?
	// HW Debouce of what?
	I2C_WriteByte(0x72, 0x7C, 0x14);				// HW debounce to 64ms (0x14)
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : StartTPI()
//
// PURPOSE       : Start HW TPI mode by writing 0x00 to TPI address 0xC7. This
//                 will take the Tx out of power down mode.
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : TRUE if HW TPI started successfully. FALSE if failed to.
//
//////////////////////////////////////////////////////////////////////////////

static bool StartTPI(void)
{
    byte devID = 0x00;
        word wID = 0x0000;

        TPI_TRACE_PRINT((">>StartTPI()\n"));

    WriteByteTPI(TPI_ENABLE, 0x00);            // Write "0" to 72:C7 to start HW TPI mode
        DelayMS(100);

        devID = ReadIndexedRegister(INDEXED_PAGE_0, 0x03);
        wID = devID;
        wID <<= 8;
        devID = ReadIndexedRegister(INDEXED_PAGE_0, 0x02);
        wID |= devID;

    devID = ReadByteTPI(TPI_DEVICE_ID);

        printf ("0x%04X\n", (int) wID);
        printf ("0x%04X\n", (int) devID);

        if (devID == SiI_DEVICE_ID) {
        //printf (SiI_DEVICE_STRING);
        return TRUE;
        }

    printf ("Unsupported TX\n");
    return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      :  EnableInterrupts()
//
// PURPOSE       :  Enable the interrupts specified in the input parameter
//
// INPUT PARAMS  :  A bit pattern with "1" for each interrupt that needs to be
//                  set in the Interrupt Enable Register (TPI offset 0x3C)
//
// OUTPUT PARAMS :  void
//
// GLOBALS USED  :  None
//
// RETURNS       :  TRUE
//
//////////////////////////////////////////////////////////////////////////////
bool EnableInterrupts(byte Interrupt_Pattern)
{
    TPI_TRACE_PRINT((">>EnableInterrupts()\n"));
    ReadSetWriteTPI(TPI_INTERRUPT_ENABLE_REG, Interrupt_Pattern);

#if defined SiI9232_OR_SiI9236
	WriteIndexedRegister(INDEXED_PAGE_0, 0x78, 0xDC);
#endif

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      :  DisableInterrupts()
//
// PURPOSE       :  Enable the interrupts specified in the input parameter
//
// INPUT PARAMS  :  A bit pattern with "1" for each interrupt that needs to be
//                  cleared in the Interrupt Enable Register (TPI offset 0x3C)
//
// OUTPUT PARAMS :  void
//
// GLOBALS USED  :  None
//
// RETURNS       :  TRUE
//
//////////////////////////////////////////////////////////////////////////////
static bool DisableInterrupts(byte Interrupt_Pattern)
{
        TPI_TRACE_PRINT((">>DisableInterrupts()\n"));
    ReadClearWriteTPI(TPI_INTERRUPT_ENABLE_REG, Interrupt_Pattern);

    return TRUE;
}


#if defined HAS_CTRL_BUS
static bool CBUS_Discovery (void) {

        byte i;

        for (i = 0; i < 20; i++) {

                WriteByteTPI (TPI_DEVICE_POWER_STATE_CTRL_REG, CTRL_PIN_DRIVEN_TX_BRIDGE | TX_POWER_STATE_D0);                  // Start CBUS self-discovery
                DelayMS (T_CBUSDISCOVERY_DELAY);

                if (ReadByteCBUS(0x0A) & 0x01) {
                        TPI_DEBUG_PRINT (("CBUS discovered in %d attempt(s).\n", (int)(i + 1)));
                        return TRUE;
                        }

                WriteByteTPI (TPI_DEVICE_POWER_STATE_CTRL_REG, CTRL_PIN_TRISTATE | TX_POWER_STATE_D0);
                DelayMS (T_CBUSDISCOVERY_DELAY);
                }

        TPI_DEBUG_PRINT (("CBUS downstream device not detected.\n0xC8:0x0A = %02X\n", (int)ReadByteCBUS(0x0A)));
        return FALSE;
        }
#endif


#ifdef MHL_CABLE_HPD
//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : TestHPD_Connected()
//
// PURPOSE       : Debounce HP signal; Find if Sink is connected to Tx.
//
// INPUT PARAMS  : None
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : TRUE if a sink is conneced to Tx. FALSE if not.
//
//////////////////////////////////////////////////////////////////////////////
static bool TestHPD_Connected(void)
{
#if defined C_9222_OR_C_9226
	static bool MHL_Mode = FALSE;

#ifdef FORCE_MHL_OUTPUT
	if (MHL_Mode == FALSE) {        // USB State
		TPI_Init();
		PI_SEL = 1;
		WriteByteTPI (TPI_DEVICE_POWER_STATE_CTRL_REG, CTRL_PIN_DRIVEN_TX_BRIDGE | TX_POWER_STATE_D0);		//Enable CBus
		MHL_Mode = TRUE;
		}
	return TRUE;
#else
	
	int i;

        // always prints for C_9222_OR_C_9226  TPI_TRACE_PRINT((">>TestHPD_Connected()\n"));

        if (MHL_Mode == FALSE) {        // USB State

                if ((VBUS_Sense == 1) && (uMHL_Sense == 0)) {

                        uMHL_Sense = 0;
                        uMHL_Sense = 1;

                        for (i = 0; i < 10; i++) {
                                if (uMHL_Sense == 1) {
                                        printf ("uMHL_Sense went high within 100us.\n");
                                        return FALSE;
                                        }
                                }

                        TPI_Init();
                        PI_SEL = 1;

                        if ((VBUS_Sense == 1) && (CBUS_Discovery() == TRUE)) {
                                MHL_Mode = TRUE;
                                return TRUE;
                                }
                        }
                }

        else {                                          // MHL State
                if (VBUS_Sense == 1) {
                        return TRUE;
                        }
                else {
                        for (i = 0; i < 25000; i++) {
                                if (VBUS_Sense == 1) {
                                        return TRUE;
                                        }
                                }
                        }
                }

        PI_SEL = 0;
        MHL_Mode = FALSE;
        return FALSE;
#endif
#else
    byte IntStatusImage;

        TPI_TRACE_PRINT((">>TestHPD_Connected()\n"));

    ReadSetWriteTPI(TPI_INTERRUPT_ENABLE_REG, HOT_PLUG_EVENT);  // Enable HPD interrupt bit

    // Repeat this loop while cable is bouncing:
    do
    {
        ReadSetWriteTPI(TPI_INTERRUPT_STATUS_REG, HOT_PLUG_EVENT);  // Clear HPD interrupt status bit
        DelayMS(T_HPD_DELAY); // Delay for metastability protection and to help filter out connection bouncing
        IntStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);    // Read Interrupt status register

    } while (IntStatusImage & HOT_PLUG_EVENT);              // loop as long as HP interrupts recur

    if (IntStatusImage & HOT_PLUG_STATE)
        return TRUE;                                        // Debounced and conneceted

    return FALSE;                                           // Debounced and disconneceted
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION      : SetPreliminaryInfoFrames()
//
// PURPOSE       : Set InfoFrames for default (initial) setup only
//
// INPUT PARAMS  : VIC, output mode,
//
// OUTPUT PARAMS : void
//
// GLOBALS USED  : None
//
// RETURNS       : TRUE
//
//////////////////////////////////////////////////////////////////////////////
static bool SetPreliminaryInfoFrames()
{
    byte i;
    API_Cmd Command;        // to allow using function SetAVI_InfoFrames()

        TPI_TRACE_PRINT((">>SetPreliminaryInfoFrames()\n"));

    for (i = 0; i < MAX_COMMAND_ARGUMENTS; i++)
        Command.Arg[i] = 0;

        Command.CommandLength = 0;      // fixes SetAVI_InfoFrames() from going into an infinite loop

    Command.Arg[0] = vid_mode;

#ifdef DEV_SUPPORT_EDID
        if (EDID_Data.YCbCr_4_4_4)
        {
                Command.Arg[3] = 0x01;
        }
        else
        {
                if (EDID_Data.YCbCr_4_2_2)
                {
                        Command.Arg[3] = 0x02;
                }
        }
#else
        Command.Arg[3] = 0x00;
#endif

    SetAVI_InfoFrames(Command);
    return TRUE;
}



void EnableTMDS (void) {

	TPI_DEBUG_PRINT(("TMDS -> Enabled\n"));
	ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);
	tmdsPoweredUp = TRUE;
	}


void DisableTMDS (void) {

	TPI_DEBUG_PRINT(("TMDS -> Disabled\n"));
#ifdef USE_BLACK_MODE
	ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_POWER_DOWN);
#else	// AV MUTE
	//ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_POWER_DOWN);
	ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK, TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
#endif
	tmdsPoweredUp = FALSE;
	}

#ifdef DEV_SUPPORT_HDCP
void RestartHDCP (void)
{
	TPI_DEBUG_PRINT (("HDCP -> Restart\n"));

	DisableTMDS();
	HDCP_Off();
	EnableTMDS();
}
#endif

void SetAudioMute (byte audioMute)
{
	ReadModifyWriteTPI(TPI_AUDIO_INTERFACE_REG, AUDIO_MUTE_MASK, audioMute);
}

#ifdef USE_BLACK_MODE
void SetInputColorSpace (byte inputColorSpace)
{
	ReadModifyWriteTPI(TPI_INPUT_FORMAT_REG, INPUT_COLOR_SPACE_MASK, inputColorSpace);
	ReadModifyWriteTPI(TPI_END_RIGHT_BAR_MSB, 0x00, 0x00);	// Must be written for previous write to take effect. Just write read value unmodified.

	#ifdef DEV_EMBEDDED
	EnableEmbeddedSync();
	#endif

}
#endif

//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION     :   TPI_Poll ()
//
// PURPOSE      :   Poll Interrupt Status register for new interrupts
//
// INPUT PARAMS :   None
//
// OUTPUT PARAMS:   None
//
// GLOBALS USED :   LinkProtectionLevel
//
// RETURNS      :   None
//
//////////////////////////////////////////////////////////////////////////////

void TPI_Poll (void)
{
	byte InterruptStatusImage;

#ifdef MHL_CABLE_HPD
#if defined SiI9232_OR_SiI9236

	byte cBusInt;
	byte intsHandled;
	byte bTemp;

	intsHandled = 0;

        InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);
        if (InterruptStatusImage & HOT_PLUG_EVENT) {
                cBusInt = ReadIndexedRegister(INDEXED_PAGE_0, 0x74);
/*
                if (cBusInt & 0x01) {           // SCDT Status Change
                        TPI_DEBUG_PRINT(("SCDT Status Change\n"));
                        intsHandled |= 0x01;
                        }
                if (cBusInt & 0x02) {           // RPWR5v Status Change
                        TPI_DEBUG_PRINT(("RPWR5v Status Change\n"));
                        intsHandled |= 0x02;
                        }
*/
                if (cBusInt & 0x04) {           // MHL Mode Established
                        TPI_DEBUG_PRINT(("MHL Mode Established\n"));
                        if (mhlCableConnected == FALSE) {
                                OnMhlCableConnected();
                                }
                        intsHandled |= 0x04;
                        }
                if (cBusInt & 0x08) {           // USB Mode Established
                        TPI_DEBUG_PRINT(("USB Mode Established\n"));
						DelayMS(100);
                        intsHandled |= 0x08;
                        }
                if (cBusInt & 0x10) {           // CBus Lockout
                        TPI_DEBUG_PRINT(("CBus Lockout\n"));
                        //intsHandled |= 0x10;
						WriteIndexedRegister(INDEXED_PAGE_0, 0x74, 0x10);					// Clear lockout interrupts
						ForceUsbIdSwitchOpen();
						ReleaseUsbIdSwitchOpen();
                        }
/*
                if (cBusInt & 0x20) {           // VBus stay low after MHL Established
                        TPI_DEBUG_PRINT(("VBus low after MHL\n"));
                        if (mhlCableConnected == TRUE) {
                                OnMhlCableDisconnected();
                                }
                        intsHandled |= 0x20;
                        }
*/
                if (cBusInt & 0x40) {           // RGND Detection
                        TPI_DEBUG_PRINT(("RGND Detection\n"));
						bTemp = ReadIndexedRegister(INDEXED_PAGE_0, 0x99);
						TPI_DEBUG_PRINT(("[99] -> %02X\n", (int)bTemp));
			
						bTemp &= 0x03;
						if (bTemp == 0x00 || bTemp == 0x03){
							ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x95, BIT_5, 0x00);
							}
						else {
							ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x95, BIT_5, BIT_5);
							}
			
						WriteIndexedRegister(INDEXED_PAGE_0, 0x93, 0xCC);					// Set CBUS pull-up to 20k during discovery
                        intsHandled |= 0x40;
                        }

                if (cBusInt & 0x80) {           // Heartbeat fail
                        TPI_DEBUG_PRINT(("Heartbeat fail\n"));
						if (mhlCableConnected == TRUE) {
							ForceUsbIdSwitchOpen();
							OnMhlCableDisconnected();
							ReleaseUsbIdSwitchOpen();
							}
			
						intsHandled |= 0x20;									// Also clear any VBus low interrupt
                        I2C_WriteByte(0xC8, 0x1E, BIT_1); // Clear Heartbeat Max Fail Interrupt
                        }

				if (intsHandled != 0x00) {
        	        WriteIndexedRegister(INDEXED_PAGE_0, 0x74, intsHandled); // Clear interrupts that were handled
					}
                }

#else
        if (TestHPD_Connected() != mhlCableConnected) {

#ifdef FORCE_MHL_OUTPUT
				OnMhlCableConnected();
#else
                if (mhlCableConnected == TRUE) {
                        OnMhlCableDisconnected();
                        }

                else {
                        OnMhlCableConnected();
                        }
#endif
                }

        if (mhlCableConnected == FALSE) {
                return;
                }

        InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);
#endif

#ifdef FORCE_MHL_OUTPUT
	if (mhlCableConnected == TRUE && hdmiCableConnected == FALSE) {
        OnHdmiCableConnected();
		}
#else
        if (((InterruptStatusImage & HOT_PLUG_STATE) >> 2) != hdmiCableConnected) {

                if (hdmiCableConnected == TRUE) {
                        OnHdmiCableDisconnected();
                        }

                else {
                        if (mhlCableConnected == TRUE) {        // HDMI can only be connected if MHL is also connected.
                                OnHdmiCableConnected();
                                }
                        }
                }

        if (hdmiCableConnected == FALSE) {
                return;
                }
#endif
#else
 

    InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);

	//if (InterruptStatusImage & HOT_PLUG_EVENT)
	if(1)
	{
		TPI_DEBUG_PRINT (("HPD  -> "));

		ReadSetWriteTPI(TPI_INTERRUPT_ENABLE_REG, HOT_PLUG_EVENT);  // Enable HPD interrupt bit

		// Repeat this loop while cable is bouncing:
		/*
		do
		{
			WriteByteTPI(TPI_INTERRUPT_STATUS_REG, HOT_PLUG_EVENT);
			DelayMS(T_HPD_DELAY); // Delay for metastability protection and to help filter out connection bouncing
			InterruptStatusImage = ReadByteTPI(TPI_INTERRUPT_STATUS_REG);    // Read Interrupt status register
		} while (InterruptStatusImage & HOT_PLUG_EVENT);              // loop as long as HP interrupts recur
		*/

                //if (((InterruptStatusImage & HOT_PLUG_STATE) >> 2) != hdmiCableConnected)
		if(1)
                {
                        //if (hdmiCableConnected == TRUE)
			if(0)
                        {
                                OnHdmiCableDisconnected();
                        }
                        else
                        {
                                OnHdmiCableConnected();
				ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x0A, 0x08, 0x08);
                        }

                        if (hdmiCableConnected == FALSE)
                        {
                                return;
                        }
                }
        }

#endif

#ifdef HAS_CTRL_BUS
        if (InterruptStatusImage == NON_MASKABLE_INT)           // Check if NMI has occurred
        {
                TPI_DEBUG_PRINT (("TP -> NMI Detected\n"));
                TPI_Init();                                                                             // Reset and re-initialize
                HotPlugService();
        }
#endif

#ifdef FORCE_MHL_OUTPUT
		if (mhlCableConnected == TRUE && hdmiCableConnected == TRUE && dsRxPoweredUp == FALSE)
		{
			OnDownstreamRxPoweredUp();
		}
#else
#if !defined SiI9232_OR_SiI9236
        // Check rx power
        //if (((InterruptStatusImage & RX_SENSE_STATE) >> 3) != dsRxPoweredUp)
	if(0)
        {
                if (hdmiCableConnected == TRUE)
                {
                        if (dsRxPoweredUp == TRUE)
                        {
                                OnDownstreamRxPoweredDown();
                        }
                        else
                        {
                                OnDownstreamRxPoweredUp();
                        }
                }

                ClearInterrupt(RX_SENSE_EVENT);
        }
#endif
#endif

        // Check if Audio Error event has occurred:
        if (InterruptStatusImage & AUDIO_ERROR_EVENT)
        {
                //TPI_DEBUG_PRINT (("TP -> Audio Error Event\n"));
                //  The hardware handles the event without need for host intervention (PR, p. 31)
                ClearInterrupt(AUDIO_ERROR_EVENT);
        }

#ifdef DEV_SUPPORT_HDCP
		if ((hdmiCableConnected == TRUE) && (dsRxPoweredUp == TRUE))
		{
    	    HDCP_CheckStatus(InterruptStatusImage);
		}
#endif

#ifdef RX_ONBOARD
        if ((tmdsPoweredUp == TRUE) && (pvid_mode != vid_mode))
        {
                TPI_TRACE_PRINT(("TP -> vid_mode...\n"));
                DisableTMDS();
                HotPlugService();
                pvid_mode = vid_mode;
        }
#endif

#ifdef DEV_SUPPORT_CEC
	CEC_Monitor();
#endif

}


#ifdef MHL_CABLE_HPD
static void OnMhlCableConnected(void) {

	TPI_DEBUG_PRINT (("MHL Connected\n"));

	mhlCableConnected = TRUE;

#if defined SiI9232_OR_SiI9236
	//if (txPowerState == TX_POWER_STATE_D3) {
	//	TPI_Init();
	//	}

	//pinVBusIsolate = 1;

	//if (pinVBusSense == 0) {
	//	TPI_DEBUG_PRINT(("VBusSense 0\n"));
		//pinWol2MhlRxPwr = 0;
	//	}
	//else {
	//	TPI_DEBUG_PRINT(("VBusSense 1\n"));
	//	}

#ifdef SYS_BOARD_STARTERKIT
	pinMhlConn = 0;
	pinUsbConn = 1;
#endif

	WriteIndexedRegister(INDEXED_PAGE_0, 0xA0, 0x10);

	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x79, BIT_4, 0x00);		// Un-force HPD
	I2C_WriteByte(0xC8, 0x21, 0x81); // Heartbeat Enable
#endif
	}

static void OnMhlCableDisconnected(void) {

	TPI_DEBUG_PRINT (("MHL Disconnected\n"));

	mhlCableConnected = FALSE;

	OnHdmiCableDisconnected();

#if defined SiI9232_OR_SiI9236
	//pinWol2MhlRxPwr = 1;
//	pinVBusIsolate = 0;
#ifdef SYS_BOARD_STARTERKIT
	pinMhlConn = 1;
	pinUsbConn = 0;
#endif

	WriteIndexedRegister(INDEXED_PAGE_0, 0xA0, 0xD0);
	
	I2C_WriteByte(0xC8, 0x21, 0x01); // Heartbeat Disable

	//TxPowerState(TX_POWER_STATE_D3);
#else
	ReadClearWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, 0x10);			// Disable self-discovery
	ReadSetWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, 0x10);
	ReadClearWriteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, 0x10);

	WriteByteTPI(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_D3);
#endif
	}
#endif


static void OnHdmiCableConnected(void)
{
	TPI_DEBUG_PRINT (("HDMI Connected ("));

#ifdef DEV_SUPPORT_MHL
	//Without this delay, the downstream BKSVs wil not be properly read, resulting in HDCP not being available.
	DelayMS(500);
#else
	// No need to call TPI_Init here unless TX has been powered down on cable removal.
	//TPI_Init();
#endif

	hdmiCableConnected = TRUE;

#ifdef DEV_SUPPORT_HDCP
	WriteIndexedRegister(INDEXED_PAGE_0, 0xCE, 0x00); // Clear BStatus
	WriteIndexedRegister(INDEXED_PAGE_0, 0xCF, 0x00);
#endif

#ifdef DEV_SUPPORT_EDID
	DoEdidRead();
#endif

#ifdef READKSV
	ReadModifyWriteTPI(0xBB, 0x08, 0x08);
#endif

	if (IsHDMI_Sink())              // select output mode (HDMI/DVI) according to sink capabilty
	{
		TPI_DEBUG_PRINT (("HDMI Sink)\n"));
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK, OUTPUT_MODE_HDMI);
	}
	else
	{
		TPI_DEBUG_PRINT (("DVI Sink)\n"));
		ReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, OUTPUT_MODE_MASK, OUTPUT_MODE_DVI);
	}

//zhy - for when TV power down, 9024 power up, then power up TV ,there is no video #if defined SiI9232_OR_SiI9236
	OnDownstreamRxPoweredUp();		// RX power not determinable? Force to on for now.
//zhy - #endif
}

static void OnHdmiCableDisconnected(void) {

	TPI_DEBUG_PRINT (("HDMI Disconnected\n"));

	hdmiCableConnected = FALSE;

#ifdef DEV_SUPPORT_EDID
	edidDataValid = FALSE;
#endif

	OnDownstreamRxPoweredDown();
	}


static void OnDownstreamRxPoweredDown(void) {

	TPI_DEBUG_PRINT (("DSRX -> Powered Down\n"));
	dsRxPoweredUp = FALSE;

#ifdef DEV_SUPPORT_HDCP
	HDCP_Off();
#endif

	DisableTMDS();
	}


static void OnDownstreamRxPoweredUp(void) {

	TPI_DEBUG_PRINT (("DSRX -> Powered Up\n"));
	dsRxPoweredUp = TRUE;

	HotPlugService();

	pvid_mode = vid_mode;
	}

