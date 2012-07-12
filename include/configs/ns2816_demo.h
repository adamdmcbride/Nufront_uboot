/*
 * (C) Copyright 2003
 * Texas Instruments.
 * Kshitij Gupta <kshitij@ti.com>
 * Configuation settings for the TI OMAP Innovator board.
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
 * Configuration for Versatile PB.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H
/*
 *  Code, etc. common to all ARM supplied development boards
 */
//#include <armsupplied.h>
/*
 * Board info register
 */

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARCH_NS2816_DEMO   1

#define HDMI_I2C_BASE 0x05120000
//#define CONFIG_SYS_ALT_MEMTEST 1
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		0xC0000000
#define CFG_HZ	       			(1000)
#define CONFIG_SYS_HZ			CFG_HZ
#define CONFIG_SYS_HZ_CLOCK		1000000		/* Timers clocked at 1Mhz */
#define CONFIG_SYS_TIMERBASE		0x05100000	/* Timer 0 and 1 base	*/
#define CONFIG_SYS_TIMER_RELOAD	0xFFFFFFFF
#define TIMER_LOAD_VAL		CONFIG_SYS_TIMER_RELOAD

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs	*/
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_MISC_INIT_R		1	/* call misc_init_r during start up */
/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + 128*1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
#define CONFIG_DRIVER_NUSMART_EMAC

//#define CONFIG_DRIVER_SMC_RV
//#undef  CONFIG_DRIVER_SMC91111
//#undef  CONFIG_DRIVER_SMC9118

//serial option
#define CONFIG_BAUDRATE		115200//38400
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }


/*
 * serial
*/


#define V_NS16550_CLK                   66000000       

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE     (-4)
#define CONFIG_SYS_NS16550_CLK          V_NS16550_CLK

#define CONFIG_CONS_INDEX               1
#define CONFIG_SYS_NS16550_COM1         0x05150000
#define CONFIG_SERIAL1                  1       

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BDI
#define CONFIG_CMD_USB
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_IMI
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVEENV
#define CONFIG_NET_MULTI
#define CONFIG_CMD_RUN
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_LOADB
/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_MASK	CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_SERVERIP

#define CONFIG_BOOTDELAY	2



/*
 * Static configuration when assigning fixed address
 */
#define CONFIG_NETMASK  255.255.255.0
#define CONFIG_IPADDR   192.168.2.82    
#define CONFIG_SERVERIP 192.168.2.93
#define CONFIG_BOOTFILE uImage 
#define CONFIG_GATEWAYIP 192.168.2.1
#define CONFIG_STDIN serial
#define CONFIG_STDOUT serial
#define CONFIG_STDERR serial


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP	/* undef to save memory		 */
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size	*/
/* Monitor Command Prompt   */
#define CONFIG_SYS_PROMPT	"NS2816_DEMO # "
#define CONFIG_IDENT_STRING "\n\n***  Auto-detects ethernet chip ***\n\n"
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	64		/* max number of command args	 */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size		*/
#undef	CONFIG_SYS_CLKS_IN_HZ		/* everything, incl board info, in Hz */
#define CONFIG_SYS_LOAD_ADDR	0x80007fc0	/* default load address */

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS		1	/* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		       	0x80000000	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE		0x40000000	/* 128 MB */


/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
/*
 *  Use the CFI flash driver for ease of use
 */
//#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_NO_FLASH 1
//#define CONFIG_FLASH_CFI_DRIVER



//#define CONFIG_ENV_IS_NOWHERE 1
#define CONFIG_ENV_IS_IN_SPI_FLASH	1		/* env in flash */
#define CONFIG_SPI_FLASH_ATMEL_25DF


//#ifndef CONFIG_REALVIEW_PB1176
//# define CONFIG_SYS_FLASH_BASE		0x40000000
//#else
//# define CONFIG_SYS_FLASH_BASE		0x30000000
//#endif
//#define CONFIG_SYS_MAX_FLASH_BANKS	(1)		/* max number of memory banks */

/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(2*CFG_HZ)	/* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(2*CFG_HZ)	/* Timeout for Flash Write */

#define CONFIG_SYS_MAX_FLASH_SECT	(259)		/* The maximum number of sectors we may need to hold data about */
						/* 255 0x40000 sectors plus first or last sector may have 4 erase regions == 259 */
						/* Has room for pre-production boards which had 256 * 0x40000 sectors */
#define FLASH_MAX_SECTOR_SIZE	(0x00010000)	/* 64 KB sectors */
#define FLASH_MIN_SECTOR_SIZE	(0x00001000)	/*  4 KB sectors */

/* Room required on the stack for the environment data */
#define CONFIG_ENV_SIZE         8192
/* 
 * Since we don't know which end has the small erase blocks 
 * or indeed whether they are present
 * we use the penultimate full sector location
 * for the environment - we save a full sector even tho 
 * the real env size CONFIG_ENV_SIZE is probably less
 * Maintain the distinction since we want to make stack size as small as possible 
 */
/* Amount of flash used for environment */
#define CONFIG_ENV_SECT_SIZE	FLASH_MIN_SECTOR_SIZE 
// #define CONFIG_SYS_MONITOR_LEN	       (4 * FLASH_SECTOR_SIZE)
// #define ARM_BM_START		(CONFIG_SYS_FLASH_BASE)
//
/* 
 * Top of flash must NOT be defined using sectors
 * Board variants may have differing flash components
 */
 
#define PHYS_FLASH_SIZE         (0x80000)	/* 512KB */
#define FLASH_TOP		(PHYS_FLASH_SIZE)

#define CONFIG_ENV_ADDR		(FLASH_TOP - (2 * CONFIG_ENV_SECT_SIZE))
#define CONFIG_ENV_OFFSET	(CONFIG_ENV_ADDR)

#define CONFIG_SYS_FLASH_PROTECTION	/* The devices have real protection */
#define CONFIG_SYS_FLASH_EMPTY_INFO	/* flinfo indicates empty blocks */


/// Nusmart option/
#define CONFIG_NUSMART_MMC 1
#define CONFIG_MMC 1
#define CONFIG_DOS_PARTITION 1
#define CONFIG_CMD_MMC
//#define CONFIG_ISO_PARTITION 1
#define CONFIG_CMD_FAT

//SATA
#define CONFIG_NUSMART_AHCI  1
#define CONFIG_SATA_ULI5288
//#define CONFIG_SYS_SCSI_MAX_SCSI_ID	4
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	1
#define CONFIG_SYS_SCSI_MAX_LUN	1
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
#define CONFIG_SYS_SCSI_MAXDEVICE	CONFIG_SYS_SCSI_MAX_DEVICE

#define CONFIG_LBA48	1

#define CONFIG_CMD_SCSI 1

#define CONFIG_SYS_CONSOLE_IS_IN_ENV 1
#define CONFIG_CONSOLE_MUX 1

#define CONFIG_LCD 
#define CONFIG_NUSMART_LCD
#define CONFIG_NUSMART_HDMI
#define LCD_BPP    LCD_COLOR8
#define CONFIG_SYS_WHITE_ON_BLACK
#define CONFIG_LCD_LOGO

//#define CONFIG_LOGICTILE 1
#define CONFIG_ISO_PARTITION
#define CONFIG_USB_STORAGE

//#define CONFIG_KEYBOARD
#define CONFIG_USB_KEYBOARD
#define CONFIG_SYS_USB_EVENT_POLL

//#define CONFIG_SYS_USB_EHCI_REGS_BASE   (0x05090000)
//#define CONFIG_USB_EHCI_NUSMART
//#define CONFIG_USB_EHCI

#define CONFIG_SYS_USB_OHCI_REGS_BASE   (0x05080000)
#define CONFIG_USB_OHCI_NEW
#define CONFIG_USB_OHCI_NUSMART

#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS      (4)
#define CONFIG_SYS_USB_OHCI_SLOT_NAME           "ohci"

#define CONFIG_PL330_DMA 1
#define CONFIG_CMD_NS2816 1

//#define CONFIG_PRCM_CLOCK 1


#define CONFIG_ROW_NUM 48
#define CONFIG_LINE_BEGIN 16
#define CONFIG_MAX_OS 8

//#define CONFIG_BOOTARGS "root=/dev/mmc0blk2 rw mem=320M console=ttyS0,38400"
//#define CONFIG_BOOTCOMMAND "mmc init; fatload mmc 0:1 0x80007fc0 uImage; bootm"

/////
#endif							/* __CONFIG_H */
