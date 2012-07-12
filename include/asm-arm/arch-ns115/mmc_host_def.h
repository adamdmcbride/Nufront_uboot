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

//mmc clock 
#define CFG_AHB_SDIO_CLOCK 200000	

#define RESPONSE_136BIT(cmd)  ((((cmd) >> 16) & 3) == 1)
#define RESPONSE_48BIT(cmd)   (((cmd) >> 16) & 2)

#define SDIO_ERR_NO_ERROR                   0x1
#define SDIO_ERR_TIMEOUT                    0x0
#define COMMANDS_TIMEOUT        100000ul

/// general information register
#define SDIO_REG_HSFR0 (*(volatile unsigned int  *)  (NUFRONT_SDMMC_BASE + 0x0000))
/// initialization setting register
#define SDIO_REG_HSFR1 (*(volatile unsigned int  *)  (NUFRONT_SDMMC_BASE + 0x0004))
/// DMA setting register
#define SDIO_REG_HSFR2 (*(volatile unsigned int  *)  (NUFRONT_SDMMC_BASE + 0x0008))


//SFR0
#define SOFT_RESET        (0x1 << 0)
//SFR1
#define DEBOUNCE_TIME         0x300000

//SRS3
#define DDIR_WRITE              (0x0 << 4)
#define DDIR_READ		(0x1 << 4)
#define RSP_TYPE_NORSP          (0x0 << 16)
#define RSP_TYPE_LGHT136 	(0x1 << 16)
#define RSP_TYPE_LGHT48         (0x2 << 16)
#define RSP_TYPE_LGHT48B        (0x3 << 16)
#define CRCCE_NOCHECK            (0x0 << 19)
#define CRCCE_CHECK              (0x1 << 19)
#define CICE_NOCHECK		 (0x0 << 20)
#define CICE_CHECK		(0x1 << 20)
#define	DP_DATA			(0x1 << 21) 
#define	DP_NO_DATA		(0x0 << 21) 
#define INDEX(i)                (i << 24)

//SRS9
#define CICMD_MASK 	(0x1 << 0)
#define CIDAT_MASK	(0x1 << 1)
#define CSS_MASK  	(0x1 << 17)

//SRS10
#define DATA_WIDTH_4BIT (0x1 << 1)
#define DATA_WIDTH_1BIT (0x0 << 1)
#define VOLT_MASK	(0xe << 9)
#define BUS_POWER	(0x1 << 8)
#define SET_BUS_3V3	(0xe << 9)

//SRS11
#define ICS_STABLE	(0x1 << 0)
#define SDCE_ENABLE	(0x1 << 2)
#define SDCLKFS_MASK	(0xff << 8)
#define DTCV_MASK	(0xf << 16)
#define DTCV_TIME	(0xb << 16)

//SRS12
#define CC_MASK		(0x1 << 0)
#define TC_MASK		(0x1 << 1)
#define BWR_MASK	(0x1 << 4)
#define BRR_MASK	(0x1 << 5)
#define CIN_CARD	(0x1 << 6)
#define CR_CARD		(0x1 << 7)
#define CINT		(0x1 << 8)
#define ERRI_MASK	(0x1 << 15)
#define ECL		(0x1 << 23)


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
	unsigned int  SRS0;
	unsigned int  SRS1;
	unsigned int  SRS2;
	unsigned int  SRS3;
	unsigned int  SRS4;
	unsigned int  SRS5;
	unsigned int  SRS6;
	unsigned int  SRS7;
	unsigned int  SRS8;
	unsigned int  SRS9;
	unsigned int  SRS10;
	unsigned int  SRS11;
	unsigned int  SRS12;
	unsigned int  SRS13;
	unsigned int  SRS14;
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

#endif
