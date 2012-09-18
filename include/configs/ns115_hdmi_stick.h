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

#define CONFIG_SYS_TIMERBASE  		0x06150000 
#define CONFIG_SYS_NS16550_COM1		0x06090000

#define CONFIG_NS115_HDMI_STICK

#define CONFIG_SYS_ALT_MEMTEST 1
#define CONFIG_SYS_MEMTEST_START       	0x80000000
#define CONFIG_SYS_MEMTEST_END		0xC0000000
#define CFG_HZ	       			(1000)
#define CONFIG_SYS_HZ			CFG_HZ
#define CONFIG_SYS_HZ_CLOCK		1000000		/* Timers clocked at 1Mhz */
#define CONFIG_SYS_TIMER_RELOAD		0xFFFFFFFF
#define TIMER_LOAD_VAL		CONFIG_SYS_TIMER_RELOAD

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs	*/
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_MISC_INIT_R		1	/* call misc_init_r during start up */
/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + 128 * 1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
//#define CONFIG_DRIVER_NUSMART_EMAC

#define CONFIG_BAUDRATE		115200
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
#define CONFIG_SERIAL1                  1       

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BDI
#define CONFIG_CMD_USB
//#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
//#define CONFIG_CMD_FLASH
//#define CONFIG_CMD_IMI
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVEENV
#define CONFIG_NET_MULTI
#define CONFIG_CMD_RUN
//#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_BRINGUP
#define CONFIG_CMD_SOURCE      1
#define CONFIG_CMD_ECHO        1
#define CONFIG_CMD_BMP
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN

#define CONFIG_CMDLINE_EDITING    1
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMD_AUTOSCRIPT
#define CONFIG_SYS_PROMPT_HUSH_PS2      "> "

//#define CONFIG_USB_ETHER_ASIX
//#define CONFIG_USB_HOST_ETHER

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
#define CONFIG_SERIAL_TAG

/*
 * Static configuration when assigning fixed address
 */
//#define CONFIG_NETMASK  255.255.255.0
//#define CONFIG_IPADDR   192.168.22.240    
//#define CONFIG_SERVERIP 192.168.22.113
#define CONFIG_BOOTFILE uImage 
//#define CONFIG_GATEWAYIP 192.168.22.1
#define CONFIG_STDIN serial
#define CONFIG_STDOUT serial
#define CONFIG_STDERR serial

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory		 */
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size	*/
/* Monitor Command Prompt   */
#define CONFIG_SYS_PROMPT	"NS115_HDMI_STICK # "
#define CONFIG_IDENT_STRING "\n\n***  NS115 HDMI STICK Board V1.0***\n\n"
#define CONFIG_FASTBOOT_GETVAR_VERSION  "NS115"
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	64		/* max number of command args	 */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size		*/
#undef	CONFIG_SYS_CLKS_IN_HZ		/* everything, incl board info, in Hz */
#define CONFIG_SYS_LOAD_ADDR	0x80007fc0	/* default load address */


#define CONFIG_EXTRA_ENV_SETTINGS       \
	"dispformat=16\0"       \
	"pmem_base=0xb3800000\0" \
        "splashimage=0x80007fc0\0"      \
        "bootcmd=run default_bootargs;ext4load mmc 1:2 0x80007fc0 uImage;ns115 cpu volt 1130000;bootm\0"       \
        "default_bootargs=setenv bootargs console=ttyS0,115200 pmem=${pmem_base} root=/dev/mmcblk1p2 rw rootwait mem=824M video=nusmartfb:1024x600-${dispformat} init=/init\0" \
        "mmcupdate=fatload mmc 0:1 0x80007fc0 mmc_update;autoscr 0x80007fc0\0"  \
        "usbupdate=fatload usb 0:1 0x80007fc0 usb_update;autoscr 0x80007fc0\0"  \
        "splashpos=m,m         \0"   

/*
#define CONFIG_EXTRA_ENV_SETTINGS 								 \
	"dispformat=16\0"									 \
	"splashimage=0x80007fc0\0"								 \
	"loadbootscript=fatload mmc 0:1 0x80007fc0 boot.scr\0" 					 \
	"bootscript=echo Running bootscript from TF card ...; " 				 \
		"source 0x80007fc0\0" 								 \
	"tfload=fatload mmc 0:1 0x80007fc0 uImage;bootm \0" 					 \
	"emmcload=mmc init 1;ext4load mmc 1:2 0x80007fc0 uImage;bootm\0" 			 \
	"tfargs=" 										 \
		"setenv bootargs console=ttyS0,115200 " 					 \
		"video=nusmartfb:1024x600-${dispformat} " 					 \
		"root=/dev/mmcblk0p2 " 								 \
		"ro rootwait mem=776M init=\init;bootm \0" 					 \
	"emmcargs=" 										 \
		"setenv bootargs console=ttyS0,115200 " 					 \
		"video=nusmartfb:1024x600-${dispformat} " 					 \
		"root=/dev/mmcblk1p2 " 								 \
		"ro rootwait mem=776M init=\init;bootm \0" 					 \
	"tfboot=echo Load uImage from TF card ...; " 						 \
		"run tfargs\0" 									 \
	"emmcboot=echo Booting from emmc ...; " 						 \
		"run emmcargs; " 								 \
		"run emmcload \0"								 \
	"mmcupdate=mmc init 0;fatload mmc 0:1 0x80007fc0 mmc_update;autoscr 0x80007fc0\0"	 \
	"usbupdate=usb init;fatload usb 0:1 0x80007fc0 usb_update;autoscr 0x80007fc0\0"		 \
	"autoboot= "										 \
		  "mmc init 0;"    								 \
		  "if test $? -ne 0; then " 							 \
		  	"fatload mmc 0:1 0x80007fc0 uImage;" 					 \
		  	"if test $? -eq 0;then "   						 \
				"run tfboot; " 							 \
		  	"else; run emmcboot; fi;" 						 \
		  "else;run emmcboot;fi;\0"  							 \
	"splashpos=m,m	       \0"		
*/
//#define CONFIG_BOOTCOMMAND "run autoboot"

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

#define CONFIG_SYS_NO_FLASH 1
//#define CONFIG_CH7033_HDMI  1
//#define CONFIG_ENV_IS_NOWHERE 1
#define CONFIG_ENV_IS_IN_EMMC	1		/* env in EMMC */
//#define CONFIG_ENV_IS_IN_SPI_FLASH	1		/* env in flash */
//#define CONFIG_SPI_FLASH_SST_25WF
#define CONFIG_ENV_SECT_SIZE	512 
#define CONFIG_ENV_SIZE     (8192) 
#define CONFIG_ENV_OFFSET	(CONFIG_ENV_ADDR)
#define CONFIG_ENV_ADDR          4592
#define CONFIG_SYS_CONSOLE_IS_IN_ENV 1

#define CONFIG_LCD 
#define CONFIG_NUSMART_LCD
#define CONFIG_NUSMART_LCD_RGB555
//#define EVA_LCDC
//#define LCD_BPP    LCD_COLOR8
#define CONFIG_SYS_WHITE_ON_BLACK
//#define CONFIG_LCD_LOGO
#define CONFIG_LCD_1024_600_60
#define CONFIG_NUSMART_BMP


//#define CONFIG_LOGICTILE 1
#define CONFIG_ISO_PARTITION
#define CONFIG_USB_STORAGE

//#define CONFIG_KEYBOARD
//#define CONFIG_USB_KEYBOARD
//#define CONFIG_SYS_USB_EVENT_POLL

#define CONFIG_SYS_USB_EHCI_REGS_BASE   (0x05090000)
#define CONFIG_USB_EHCI_NUSMART
#define CONFIG_USB_EHCI

//#define CONFIG_SYS_USB_OHCI_REGS_BASE   (0x05080000)
//#define CONFIG_USB_OHCI_NEW
//#define CONFIG_USB_OHCI_NUSMART

//#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS      (4)
//#define CONFIG_SYS_USB_OHCI_SLOT_NAME           "ohci"

#define CONFIG_CMD_NS115 1
#define CONFIG_FASTBOOT_RECOVERY
#define CONFIG_RECOVERY
//#define CONFIG_PL330_DMA 1

#define CONFIG_PRCM_CLOCK 1
#define CONFIG_ENV_OVERWRITE    

/*
    MMC env and MMC driver 
*/

#define CONFIG_MMC 
#define CONFIG_NUFRONT_MMC
#define CONFIG_NUSMART_MMC 1
#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION 1

#define CONFIG_SYS_MEMTEST_SCRATCH 0x8FFFFFF0

#endif							/* __CONFIG_H */
