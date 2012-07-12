/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 * Syed Mohammed Khasim <khasim@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation's version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


/* Interrupt MMC/SD enable register; Set the value to the register that allows
   to get the status bits, on an event-by-event basis
 */

#ifndef MMC_HOST_DEFINITIONS_H
#define MMC_HOST_DEFINITIONS_H

/* Driver definitions */
#define MMCSD_SECTOR_SIZE	(512)
#define MMC_CARD		0
#define SD_CARD			1
#define BYTE_MODE		0
#define SECTOR_MODE		1
#define CLK_INITSEQ		0
#define CLK_400KHZ		1
#define CLK_MISC		2

#define RESPONSE_136BIT(cmd)  ((((cmd) >> 16) & 3) == 1)
#define RESPONSE_48BIT(cmd)   (((cmd) >> 16) & 2)

typedef struct {
	unsigned int card_type;
	unsigned int version;
	unsigned int mode;
	unsigned int size;
	unsigned int RCA;

	unsigned int cmd;
	unsigned int response[4];
	unsigned short blkcnt;
	unsigned short blksize;
} mmc_card_data;

typedef struct {
	volatile struct slot_register_set *io;
	unsigned int slot;
	unsigned int base;
 } mmc_controller_data;

#define mmc_reg_out(addr, mask, val) \
	(addr) = (((addr)) & (~(mask)) ) | ( (val) & (mask));

struct slot_register_set {
	volatile unsigned int  SRS0;
	volatile unsigned int  SRS1;
	volatile unsigned int  SRS2;
	volatile unsigned int  SRS3;
	volatile unsigned int  SRS4;
	volatile unsigned int  SRS5;
	volatile unsigned int  SRS6;
	volatile unsigned int  SRS7;
	volatile unsigned int  SRS8;
	volatile unsigned int  SRS9;
	volatile unsigned int  SRS10;
	volatile unsigned int  SRS11;
	volatile unsigned int  SRS12;
	volatile unsigned int  SRS13;
	volatile unsigned int  SRS14;
	unsigned int  SRS15;
	unsigned int  SRS16;
	unsigned int  SRS17;
	unsigned int  SRS18;
	unsigned int  SRS19;
	unsigned int  SRS20;
	unsigned int  SRS21;
	unsigned int  SRS22;
	unsigned int  SRS23;
	unsigned int  SRS24;
	unsigned int  SRS25;
	unsigned int  SRS26;
	unsigned int  SRS27;
	unsigned int  SRS28;
	unsigned int  SRS29;
	unsigned int  SRS30;
	unsigned int  SRS31;
	unsigned int  SRS32;
	unsigned int  SRS33;
	unsigned int  SRS34;
	unsigned int  SRS35;
	unsigned int  SRS36;
	unsigned int  SRS37;
	unsigned int  SRS38;
	unsigned int  SRS39;
	unsigned int  SRS40;
	unsigned int  SRS41;
	unsigned int  SRS42;
	unsigned int  SRS43;
	unsigned int  SRS44;
	unsigned int  SRS45;
	unsigned int  SRS46;
	unsigned int  SRS47;
	unsigned int  SRS48;
	unsigned int  SRS49;
	unsigned int  SRS50;
	unsigned int  SRS51;
	unsigned int  SRS52;
	unsigned int  SRS53;
	unsigned int  SRS54;
	unsigned int  SRS55;
	unsigned int  SRS56;
};

typedef unsigned long DWORD;


#define SFR11_SD_CLOCK_ENABLE              0x0000004ul
#define CFG_AHB_SDIO_CLOCK 24000
#define SFR11_SEL_FREQ_BASE_MASK            0x000FF00ul
#define SFR11_INT_CLOCK_STABLE             0x0000002ul
#define COMMANDS_TIMEOUT        100000ul
#define SDIO_ERR_NO_ERROR                   0x1
#define SDIO_ERR_TIMEOUT                    0x0
#define SFR11_INT_CLOCK_ENABLE             0x0000001ul
#define DEBOUNCING_TIME         0x300000ul
#define SFR1_DMA_BUFF_SIZE_512KB    0x00007000ul

//bus width define
#define BIT_MODE_1              (0)
#define BIT_MODE_4              (1)
#define BIT_MODE_8              (2)

#define SFR11_TIMEOUT_MASK                  0x00F0000ul
#define SFR11_TIMEOUT_TMCLK_X_2_POWER_24    0x00B0000ul
#define SFR10_DATA_WIDTH_4BIT               0x0000002ul
#define  __CMD_DATA_PRESENT      (1 << 21)

/// Wakeup event enable on SD card removal
#define SFR10_WAKEUP_EVENT_CARD_REM_ENABLE  0x4000000ul
/// Wakeup event enable on SD card inserted
#define SFR10_WAKEUP_EVENT_CARD_INS_ENABLE  0x2000000ul
/// Wakeup event enable on SD card interrupt
#define SFR10_WAKEUP_EVENT_CARD_INT_ENABLE  0x1000000ul
/// Interrupt at block gap for a muliple transfer 
#define SFR10_INTERRUPT_BLOCK_GAP           0x0080000ul
/// Read wait control
#define SFR10_READ_WAIT_CONTROL             0x0040000ul
/// Continue request    
#define SFR10_CONTINUE_REQUEST              0x0020000ul
/// Stop at block gap request
#define SFR10_STOP_AT_BLOCK_GAP             0x0010000ul
/// SD bus voltage - 3.3V
#define SFR10_SET_3_3V_BUS_VOLTAGE          0x0000E00ul
/// SD bus voltage - 3.0V
#define SFR10_SET_3_0V_BUS_VOLTAGE          0x0000C00ul
/// SD bus voltage - 1.8V
#define SFR10_SET_1_8V_BUS_VOLTAGE          0x0000A00ul
/// SD bus voltage mask
#define SFR10_BUS_VOLTAGE_MASK              0x0000E00ul
/// SD bus power. The SD device is powered.
#define SFR10_SD_BUS_POWER                  0x0000100ul


/// Card state stable
#define SFR9_CARD_STATE_STABLE  0x0020000ul
/// general information register
#define SDIO_REG_HSFR0 (*(volatile DWORD *)  (NUFRONT_SD_BASE + 0x0000))
/// initialization setting register
#define SDIO_REG_HSFR1 (*(volatile DWORD *)  (NUFRONT_SD_BASE + 0x0004))
/// DMA setting register
#define SDIO_REG_HSFR2 (*(volatile DWORD *)  (NUFRONT_SD_BASE + 0x0008))



#define HSFR0_SOFTWARE_RESET        (1ul << 0)

#endif
