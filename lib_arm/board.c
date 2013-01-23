/* (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * To match the U-Boot user interface on ARM platforms to the U-Boot
 * standard (as on PPC platforms), some messages with debug character
 * are removed from the default U-Boot build.
 *
 * Define DEBUG here if you want additional info as shown below
 * printed upon startup:
 *
 * U-Boot code: 00F00000 -> 00F3C774  BSS: -> 00FC3274
 * IRQ Stack: 00ebff7c
 * FIQ Stack: 00ebef7c
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio_dev.h>
#include <timestamp.h>
#include <version.h>
#include <net.h>
#include <serial.h>
#include <nand.h>
#include <onenand_uboot.h>
#include <mmc.h>
#include <asm/arch/gpio.h>
//#include <asm/arch/mmc.h>
#include <battary.h>
#include <lcd.h>
#include <draw_icon.h>

#ifdef CONFIG_DRIVER_SMC91111
#include "../drivers/net/smc91111.h"
#endif
#ifdef CONFIG_DRIVER_LAN91C96
#include "../drivers/net/lan91c96.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

ulong monitor_flash_len;
unsigned int disp_flag = 0;
extern int  fastboot_flag;
int show_charging_logo(int volt);

#define BOOTUP	2
#define RESUME	1
#define SUSPEND	0
#define RELEASE	0
#define PRESSED	1
int suspend_flag = RESUME;
extern vidinfo_t panel_info;

#ifdef CONFIG_HAS_DATAFLASH
extern int  AT91F_DataflashInit(void);
extern void dataflash_print_info(void);
#endif

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" U_BOOT_DATE " - " U_BOOT_TIME ")"CONFIG_IDENT_STRING;

#ifdef CONFIG_DRIVER_RTL8019
extern void rtl8019_get_enetaddr (uchar * addr);
#endif

#if defined(CONFIG_HARD_I2C) || \
    defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#endif

/************************************************************************
 * Coloured LED functionality
 ************************************************************************
 * May be supplied by boards if desired
 */
void inline __coloured_LED_init (void) {}
void inline coloured_LED_init (void) __attribute__((weak, alias("__coloured_LED_init")));
void inline __red_LED_on (void) {}
void inline red_LED_on (void) __attribute__((weak, alias("__red_LED_on")));
void inline __red_LED_off(void) {}
void inline red_LED_off(void)	     __attribute__((weak, alias("__red_LED_off")));
void inline __green_LED_on(void) {}
void inline green_LED_on(void) __attribute__((weak, alias("__green_LED_on")));
void inline __green_LED_off(void) {}
void inline green_LED_off(void)__attribute__((weak, alias("__green_LED_off")));
void inline __yellow_LED_on(void) {}
void inline yellow_LED_on(void)__attribute__((weak, alias("__yellow_LED_on")));
void inline __yellow_LED_off(void) {}
void inline yellow_LED_off(void)__attribute__((weak, alias("__yellow_LED_off")));
void inline __blue_LED_on(void) {}
void inline blue_LED_on(void)__attribute__((weak, alias("__blue_LED_on")));
void inline __blue_LED_off(void) {}
void inline blue_LED_off(void)__attribute__((weak, alias("__blue_LED_off")));

/************************************************************************
 * Init Utilities							*
 ************************************************************************
 * Some of this code should be moved into the core functions,
 * or dropped completely,
 * but let's get it working (again) first...
 */

#if defined(CONFIG_ARM_DCC) && !defined(CONFIG_BAUDRATE)
#define CONFIG_BAUDRATE 115200
#endif
static int init_baudrate (void)
{
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));
	gd->bd->bi_baudrate = gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;

	return (0);
}

static int display_banner (void)
{
	printf ("\n\n%s\n\n", version_string);
#ifdef __ARMSUPPLIED_
	printf ("\n\nCode cloned from branch %s of git://linux-arm.org/u-boot-armdev.git\n", ARMLTD_BRANCH);
	printf ("\n\nRelease %s\n", ARMLTD_RELEASE);
	printf ("Remote commit cloned   %s\n", ARMLTD_COMMIT_REMOTE);
	printf ("Latest commit locally  %s\n", ARMLTD_COMMIT_LOCAL);
	printf ("git state %s\n", ARMLTD_SUPPLEMENTARY);
#endif
	debug ("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
	       _armboot_start, _bss_start, _bss_end);
#ifdef CONFIG_MODEM_SUPPORT
	debug ("Modem Support enabled\n");
#endif
#ifdef CONFIG_USE_IRQ
	debug ("IRQ Stack: %08lx\n", IRQ_STACK_START);
	debug ("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif

	return (0);
}

/*
 * WARNING: this code looks "cleaner" than the PowerPC version, but
 * has the disadvantage that you either get nothing, or everything.
 * On PowerPC, you might see "DRAM: " before the system hangs - which
 * gives a simple yet clear indication which part of the
 * initialization if failing.
 */
static int display_dram_config (void)
{
	int i;

#ifdef DEBUG
	puts ("RAM Configuration:\n");

	for(i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		printf ("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
		print_size (gd->bd->bi_dram[i].size, "\n");
	}
#else
	ulong size = 0;

	for (i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		size += gd->bd->bi_dram[i].size;
	}
	puts("DRAM:  ");
	print_size(size, "\n");
#endif

	return (0);
}

#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
#ifdef CONFIG_ENV_IS_IN_FLASH
# ifdef CONFIG_ENV_ADDR
#  ifdef CONFIG_REALVIEW
	gd->bd->bi_env = (struct environment_s *)(gd->bd->flash_base + CONFIG_ENV_OFFSET);
#  else
	gd->bd->bi_env = (struct environment_s *)CONFIG_ENV_ADDR;
#  endif
# endif
#endif
}
#endif /* CONFIG_SYS_NO_FLASH */

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
static int init_func_i2c (void)
{
	puts ("I2C:   ");
	i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	puts ("ready\n");
	return (0);
}
#endif

#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
#include <pci.h>
static int arm_pci_init(void)
{
	pci_init();
	return 0;
}
#endif /* CONFIG_CMD_PCI || CONFIG_PCI */

/*
 * Breathe some life into the board...
 *
 * Initialize a serial port as console, and carry out some hardware
 * tests.
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependent #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

int print_cpuinfo (void);

init_fnc_t *init_sequence[] = {
#if defined(CONFIG_ARCH_CPU_INIT)
	arch_cpu_init,		/* basic arch cpu dependent setup */
#endif
	cpu_init,		/* ns2816 */
	board_init,		/* basic board dependent setup */
#if defined(CONFIG_USE_IRQ)
	interrupt_init,		/* set up exceptions */
#endif
	timer_init,		/* initialize timer */
	env_init,		/* initialize environment */
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,		/* stage 1 init of console */
	display_banner,		/* say that we are here */
#if defined(CONFIG_DISPLAY_CPUINFO)
	print_cpuinfo,		/* display cpu info (and speed) */
#endif
#if defined(CONFIG_DISPLAY_BOARDINFO)
	checkboard,		/* display board info */
#endif
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
	init_func_i2c,
#endif

#if (!defined(CONFIG_ARCH_NS2816_DEMO) && !defined(CONFIG_ARCH_NS2816_NTNB) && !defined(CONFIG_ARCH_NS2816_NTPAD) \
	&& !defined(CONFIG_ARCH_NS2816_DEV) && !defined(CONFIG_ARCH_NS2816_TEST) && !defined(CONFIG_ARCH_NS2816_HUASHAN) \
	&& !defined (CONFIG_ARCH_NS2816_YANGCHENG))
	dram_init,		/* configure available RAM banks */
	display_dram_config,
#endif

#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
	arm_pci_init,
#endif
	NULL,
};

#ifdef CONFIG_REALVIEW
extern void icache_flush(void);
extern void icache_flush_v5t(void);
extern void icache_flush_v67(void);
void setup_realview_board_info(void)
{
	unsigned long id;
	unsigned long idwork;
	/* Read board HBI from System Control Register */
	int board_hbi =  *((unsigned int *)0x10000000);
	board_hbi &= 0x0FFF0000;
	/*
	 * Defaults
	 */
	gd->bd->is_pb1176 = 0;  
	gd->bd->flash_base = 0x40000000; 
	gd->bd->smcrv_base = 0x4E000000; 
	gd->bd->serial_base = 0x10009000;
	gd->bd->prompt = malloc(0x18);
	switch(board_hbi)
	{
	case 0x01400000:
		sprintf(gd->bd->prompt, "RealView EB #");
		gd->bd->mach_id = 827;
		break;
	case 0x01470000:
		sprintf(gd->bd->prompt, "RealView PB1176 #");
		gd->bd->mach_id = MACH_TYPE_REALVIEW_PB1176;
		gd->bd->is_pb1176 = 1;
		gd->bd->smcrv_base = 0x3A000000; 
		gd->bd->serial_base = 0x1010C000;
		gd->bd->flash_base = 0x30000000; 
		break;
	case 0x01590000:
		sprintf(gd->bd->prompt, "RealView PBMPCORE #");
		gd->bd->mach_id = MACH_TYPE_REALVIEW_PB11MP;
		break;
	case 0x01780000:
		sprintf(gd->bd->prompt, "RealView PBA8 #");
		gd->bd->mach_id = MACH_TYPE_REALVIEW_PBA8;
		break;
	case 0x01820000:
		sprintf(gd->bd->prompt, "RealView PBX #");
		gd->bd->mach_id = MACH_TYPE_REALVIEW_PBX;
		break;
	case 0x01900000:
		sprintf(gd->bd->prompt, "V2P CA9 #");
		gd->bd->mach_id = MACH_TYPE_V2P_CA9;
 		break;
	default:
		sprintf(gd->bd->prompt, "Unknown #");
		reset_cpu(0);
		break;
	}
	/* Now attempt to determine the CPU architecture & set the cache routines accordingly */
	asm ("mrc p15, 0, %0, c0, c0, 0":"=r" (id));

	/* Just using the architecture code at present to seperate v6, v7 */
	idwork = id << 12;
	idwork = idwork >>28;
	gd->bd->bi_arch_number = idwork;
}
#endif	/* CONFIG_REALVIEW */

#ifdef CONFIG_UBOOT_CHARGER
void resume(void)
{
	printf("resume\n");
	lcd_enable();
	suspend_flag = RESUME;
}

void suspend(void)
{
	printf("suspend\n");
	clear_logo();
	lcd_disable();
	suspend_flag = SUSPEND;
}
#endif

void start_armboot (void)
{
	int i,size = 0;
	init_fnc_t **init_fnc_ptr;
	char *s;
#if defined(CONFIG_VFD) || defined(CONFIG_LCD)
	unsigned long addr;
#endif

/*
#if defined(CONFIG_NUSMART_MMC)
	puts ("SYNOPSYS MMC:   ");
	if (mmc_legacy_init(1)) 
	{
		puts("Success!\n");
		size = file_fat_read("u-boot.bin", 0x7E000000, 0);
		if (size > 0) 
		{
			puts("Loading u-boot.bin from mmc\n");
		}
	}
#endif
*/
	/* Pointer is writable since we allocated a register for it */
	gd = (gd_t*)(_armboot_start - CONFIG_SYS_MALLOC_LEN - sizeof(gd_t));
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

	memset ((void*)gd, 0, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
	memset (gd->bd, 0, sizeof (bd_t));

	gd->flags |= GD_FLG_RELOC;

#ifdef CONFIG_REALVIEW
	/*
	 * From here on we start using board dependent stuff
	 * - so we'd better determine the board id
	 */
	{
		setup_realview_board_info();
	}
#endif	/* CONFIG_REALVIEW */


	monitor_flash_len = _bss_start - _armboot_start;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}
	mem_malloc_init (_armboot_start - CONFIG_SYS_MALLOC_LEN,
			CONFIG_SYS_MALLOC_LEN);

#ifndef CONFIG_SYS_NO_FLASH
	/* configure available FLASH banks */
	display_flash_config (flash_init ());
#endif /* CONFIG_SYS_NO_FLASH */

#if defined(CONFIG_NS115_PAD_REF) || defined (CONFIG_NS115_PAD_PROTOTYPE)
	power_key_delay();
#endif

#ifdef CONFIG_VFD
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif
	/*
	 * reserve memory for VFD display (always full pages)
	 */
	/* bss_end is defined in the board-specific linker script */
	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	vfd_setmem (addr);
	gd->fb_base = 0xb3800000;
//	gd->fb_base = getenv("fbaddr");
#endif /* CONFIG_VFD */

	/* initialize environment */
	env_relocate ();
#ifdef CONFIG_LCD
	/* board init may have inited fb_base */
	if (!gd->fb_base) {
#		ifndef PAGE_SIZE
#		  define PAGE_SIZE 4096
#		endif
		/*
		 * reserve memory for LCD display (always full pages)
		 */
		/* bss_end is defined in the board-specific linker script */
		addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
		//addr = 0xF7000000;
		lcd_setmem (addr);
//		char pmem_addr[20];
//		int i = 0;
//		i = getenv_r ("pmem_base", pmem_addr, sizeof(pmem_addr));
//		unsigned int *a = getenv("fbaddr");
//		if(i > 0)
//			gd->fb_base = simple_strtoul (pmem_addr, NULL, 16);
//		else
#if defined(CONFIG_NS115_PAD_REF) || defined (CONFIG_NS115_PAD_PROTOTYPE)
		gd->fb_base = 0xb3800000;
#endif
#ifdef	CONFIG_NS115_HDMI_STICK
		gd->fb_base = 0xb4800000;
#endif
//		gd->fb_base = 0xb3800000;
//		gd->fb_base = addr;
	}
#endif /* CONFIG_LCD */

	unsigned int mmc_cont = (*((unsigned int*)0x0704e04c)) ? 1 : 0;
	unsigned int recovery_flag = 1;
#ifdef CONFIG_FASTBOOT_RECOVERY
	if(mmc_cont == 1){
		recovery_flag = read_boot_env();
	}
#endif

#ifdef CONFIG_UBOOT_CHARGER
if(recovery_flag != 0)
{
	unsigned int *s_volt;
	unsigned int low_bat_volt = 0;
	unsigned int lcd_en_volt = 0;
	unsigned int charge_flag = 0, pre_charge_flag = 0, bat_flag = 0;
	unsigned int lcd_flag = 0, is_adapter_pullout = 0;
	unsigned int volt = 0, ret = 0, key_pressed = 0, idle_time = 0, key_status = RELEASE;
#ifdef CONFIG_NS115_PAD_PROTOTYPE
	unsigned int lcd_on = 0, charger_type = 0;
#endif

	if ((s_volt = getenv ("poweroff_lcd")) != NULL){
		lcd_en_volt = simple_strtoul (s_volt, NULL, 10);
		printf("poweroff_lcd  = %d \n",lcd_en_volt);
	}
	else{
		lcd_en_volt = 3100;
		printf("poweroff_lcd default = %d \n",lcd_en_volt);
	}

	if ((s_volt = getenv ("poweroff_volt")) != NULL){
		low_bat_volt = simple_strtoul (s_volt, NULL, 10);
		printf("latch_volt = %d \n",low_bat_volt);
	}
	else{
		low_bat_volt = 3100;
		printf("latch_volt default = %d \n",low_bat_volt);
	}

	volt = read_battary_volt();
	draw_init(gd->fb_base, panel_info.vl_row, panel_info.vl_col);
#ifdef CONFIG_NS115_PAD_PROTOTYPE
	if(power_on_by_AC()){
		pmic_gpio_init();
		charger_type = detect_charge_status();//1 is AC and 2 is usb
		printf("now volt = %d charger_type=%d\n", volt,charger_type);
		if(charger_type == 2)//when usb pluged in,can't bright up the lcd!
			lcd_on = 0;
		else
			lcd_on = 1;
#endif
	while(1){
		bat_flag = charge_status();
		charge_flag = detect_charge_status();
		if ((0 == bat_flag) && (0 == pre_charge_flag)){
			break;
		}
		if ((0 == charge_flag) && (0 != pre_charge_flag)){
			is_adapter_pullout = 1;//judge is adapter pullout or never insert adapter
		}
		pre_charge_flag = charge_flag;
		volt = read_battary_volt();
		printf("test flag %d volt %d\n", charge_flag, volt);

		if (charge_flag){
			if (volt < lcd_en_volt){
				suspend_flag = SUSPEND;
				if (lcd_flag == 1){
					lcd_disable ();
					lcd_flag = 0;
				}
				//do nothing, wait for volt > lcd_en_volt
				udelay(5000*1000);//check volt every 5s
			}
			else if (volt > lcd_en_volt){
				if (suspend_flag == RESUME){
					int i = 0, draw_level = 0;

					if (volt >= 4200){
						draw_level = 0;
					}
					else if (volt < 4200 && volt >= 4000){
						draw_level = 1;
					}
					else if (volt < 4000 && volt >= 3700){
						draw_level = 2;
					}
					else if (volt < 3700 && volt >= 3400){
						draw_level = 3;
					}
					else if (volt < 3400 && volt >= 3200){
						draw_level = 4;
					}
					else if (volt < 3200){
						draw_level = 5;
					}

					if (lcd_flag == 0){
						show_bat_framework(LOGO_H, LOGO_W);
						lcd_enable();
						lcd_flag = 1;
					}

					for (i = draw_level; i >= 0; i--){
						int j = 0;
						show_bat_status(LOGO_H, LOGO_W, i);
						for (j = 0; j < LOGO_SHOW_DELAY; j++){
							udelay(PRESS_DELAY);
							if (power_key_status()){//key pressed
								key_pressed++;
								key_status = PRESSED;
								if(key_pressed > LONG_PRESS){
									key_pressed = 0;
									suspend_flag = BOOTUP;
									break;
								}
							} else {
								key_status = RELEASE;
							}

							if (key_status == RELEASE) {
								if(key_pressed > LONG_PRESS){//long pressed
									key_pressed = 0;
									idle_time = 0;
									suspend_flag = BOOTUP;
									break;//boot up
								} else if (key_pressed >= 1){
									suspend();
									key_pressed = 0;
									idle_time = 0;
								} else if (key_pressed > 0) {
									key_pressed = 0;
									idle_time = 0;
								} else {
									idle_time++;
								}

								if (idle_time >= IDLE_DELAY){
									suspend();
									key_pressed = 0;
									idle_time = 0;
								}
							}
						}
					}
				} else if (suspend_flag == SUSPEND){
					int j = 0;
					for (j = 0; j < LOGO_SHOW_DELAY; j++){
						udelay(PRESS_DELAY);
						if (power_key_status()){//key pressed
							key_pressed++;
							key_status = PRESSED;
							if(key_pressed > LONG_PRESS){
								key_pressed = 0;
								suspend_flag = BOOTUP;
								break;
							}
						} else {
							key_status = RELEASE;
						}

						if (key_status == RELEASE){
							if(key_pressed > LONG_PRESS){//long pressed
								key_pressed = 0;
								suspend_flag = BOOTUP;
							}else if(key_pressed >= 1){
								lcd_flag = 0;
								resume();
								key_pressed = 0;
								idle_time = 0;
							} else {
								key_pressed = 0;
							}
						}
					}
				} else if (suspend_flag == BOOTUP) {
					int volt_percent = (volt*100)/4200;
					if (volt_percent > 100){
						volt_percent = 100;
					}
					char *volt_buff = itoa(volt_percent);
					clear_logo();
					lcd_disable();
					setenv("volt_buff",volt_buff);
					printf("BOOTUP\n");
					break;
				}
			}
			else {
				printf("error proccess, please check codes\n");
			}
		}
		else {
			if (volt < lcd_en_volt){
				if (lcd_flag == 1){
					lcd_disable ();
					lcd_flag = 0;
				}
				ricoh583_poweroff();
			}
			else if (volt > lcd_en_volt && volt < low_bat_volt){
				if (lcd_flag == 0){
					draw_init(gd->fb_base, panel_info.vl_row, panel_info.vl_col);
					lcd_flag = 1;
				}

				show_bat_status(LOGO_H, LOGO_W, 5);
				udelay(3500*1000);
				ricoh583_poweroff();
			}
			else if (volt > low_bat_volt){
				if (lcd_flag == 1){
					clear_logo();
					lcd_disable ();
					lcd_flag = 0;
				}
				if (is_adapter_pullout){
					ricoh583_poweroff();
				}
				else{
					break;
				}
			}
		}
	}
#ifdef CONFIG_NS115_PAD_PROTOTYPE
	}
#endif
}
#endif

#ifdef  CONFIG_NS115_HDMI_STICK
	read_hdmi_resolution();
#endif

#if defined(CONFIG_CMD_NAND)
	puts ("NAND:  ");
	nand_init();		/* go init the NAND */
#endif

#if defined(CONFIG_CMD_ONENAND)
	onenand_init();
#endif

#ifdef CONFIG_HAS_DATAFLASH
	AT91F_DataflashInit();
	dataflash_print_info();
#endif

	/* initialize environment */
//	env_relocate ();
	
	char disp_format[3];
	char *str;
	char *disp32bit = "32";
	char *disp16bit = "16";
	if (( str = getenv("dispformat")) != NULL) {
		copy_filename(disp_format, str,sizeof (disp_format));
		if(strncmp(disp32bit,disp_format,3) == 0){
			debug("disp32bit = %s\n",disp_format);
			disp_flag = 1;
		}else if(strncmp(disp16bit,disp_format,3) == 0){
			debug("disp16bit = %s\n",disp_format);
			disp_flag = 0;
		}
	}else {
		disp_flag = 1;
	}

	if(fastboot_flag == 1){
		if(mmc_cont == 0){
			char *mmc_read[] = {"mmc", "read", "0", "0x80007fc0","7000","1000" };
			do_mmc(NULL, 0, 6, mmc_read);
		}else if(mmc_cont == 1) {
			char *mmc_read[] = {"mmc", "read", "1", "0x80007fc0","7000","1000" };
			do_mmc(NULL, 0, 6, mmc_read);
		}
	} else {
		if(disp_flag == 0){	//16bit
#if defined(CONFIG_NS115_HDMI_STICK)
			if(mmc_cont == 0){
				char *mmc_read[] = {"mmc", "read", "0", "0x80007fc0","8800","9000" };
				do_mmc(NULL, 0, 6, mmc_read);
			}else if(mmc_cont == 1) {
				char *mmc_read[] = {"mmc", "read", "1", "0x80007fc0","8800","9000" };
				do_mmc(NULL, 0, 6, mmc_read);
			}
#else
			if(mmc_cont == 0){
				char *mmc_read[] = {"mmc", "read", "0", "0x80007fc0","8800","1000" };
				do_mmc(NULL, 0, 6, mmc_read);
			}else if(mmc_cont == 1) {
				char *mmc_read[] = {"mmc", "read", "1", "0x80007fc0","8800","1000" };
				do_mmc(NULL, 0, 6, mmc_read);
			}
#endif
		}else if(disp_flag == 1){	//32bit
#if defined(CONFIG_NS115_HDMI_STICK)
			if(mmc_cont == 0){
				char *mmc_read[] = {"mmc", "read", "0", "0x80007fc0","8800","9000" };
				do_mmc(NULL, 0, 6, mmc_read);
			}else if(mmc_cont == 1) {
				char *mmc_read[] = {"mmc", "read", "1", "0x80007fc0","8800","9000" };
				do_mmc(NULL, 0, 6, mmc_read);
			}
#else
			if(mmc_cont == 0){
				char *mmc_read[] = {"mmc", "read", "0", "0x80007fc0","5210","1500" };
				do_mmc(NULL, 0, 6, mmc_read);
			}else if(mmc_cont == 1) {
				char *mmc_read[] = {"mmc", "read", "1", "0x80007fc0","5210","1500" };
				do_mmc(NULL, 0, 6, mmc_read);
			}
#endif
		}
	}
#if defined(CONFIG_NS115_HDMI_STICK)
	mmc_cont = (*((unsigned int*)0x0704e04c)) ? 1 : 0;
	if(mmc_cont == 1){
		char *mmc_read[] = {"mmc", "read", "1", "0x80006000","2053","1" };
		do_mmc(NULL, 0, 6, mmc_read);
		char *mmc_buf = (char *) 0x80006000;
		char mac_flag[] = "WIFIMAC0";
		char mac_buf[20];
		if(!strncmp(mmc_buf,mac_flag,strlen(mac_flag))){
			memcpy(mac_buf,mmc_buf + 28,6);
			sprintf((char *)mac_buf,"%02x:%02x:%02x:%02x:%02x:%02x",mmc_buf[28],mmc_buf[29],
				mmc_buf[30],mmc_buf[31],mmc_buf[32],mmc_buf[33]);
			setenv("wifimac",mac_buf);
		}
	}
#endif
		
#ifdef CONFIG_VFD
	/* must do this after the framebuffer is allocated */
	drv_vfd_init();
#endif /* CONFIG_VFD */

#ifdef CONFIG_SERIAL_MULTI
	serial_initialize();
#endif
	
	/* IP Address */
	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");

	stdio_init ();	/* get the devices list going. */

	jumptable_init ();

#if defined(CONFIG_API)
	/* Initialize API */
	api_init ();
#endif
#if defined(CONFIG_ARCH_MISC_INIT)
	/* miscellaneous arch dependent initialisations */
	arch_misc_init ();
#endif
#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif

	/* enable exceptions */
	enable_interrupts ();

	/* Perform network card initialisation if necessary */
#ifdef CONFIG_DRIVER_TI_EMAC
	/* XXX: this needs to be moved to board init */
extern void davinci_eth_set_mac_addr (const u_int8_t *addr);
	if (getenv ("ethaddr")) {
		uchar enetaddr[6];
		eth_getenv_enetaddr("ethaddr", enetaddr);
		davinci_eth_set_mac_addr(enetaddr);
	}
#endif

#if defined(CONFIG_DRIVER_SMC91111) || defined (CONFIG_DRIVER_LAN91C96)
	/* XXX: this needs to be moved to board init */
	if (getenv ("ethaddr")) {
		uchar enetaddr[6];
		eth_getenv_enetaddr("ethaddr", enetaddr);
		smc_set_mac_addr(enetaddr);
	}
#endif /* CONFIG_DRIVER_SMC91111 || CONFIG_DRIVER_LAN91C96 */

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if defined(CONFIG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif

#ifdef BOARD_LATE_INIT
	board_late_init ();
#endif

#ifdef CONFIG_GENERIC_MMC
	puts ("MMC:   ");
	mmc_initialize (gd->bd);
#endif


#if defined(CONFIG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
	puts ("Net:   ");
#endif
	eth_initialize(gd->bd);
#if defined(CONFIG_RESET_PHY_R)
	debug ("Reset Ethernet PHY\n");
	reset_phy();
#endif
#endif
#ifdef CONFIG_NUSMART_HDMI
/*
	printf("HDMI Init and POLL\n");
	TPI_Init();
	TPI_Poll();
	TPI_Poll();

	char tpireg[256];
	ReadBlockTPI(0,256,tpireg);	
*/
#endif
#if (defined(CONFIG_CMD_USB) && defined(CONFIG_ARCH_NS2816_DEMO))
	usb_stop();
	usb_init();
	
#endif

//#ifdef CONFIG_FASTBOOT_RECOVERY
//	if(mmc_cont == 1){
//		read_boot_env();
//	}
//#endif

	console_init_r ();	/* fully init console as a device */

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop ();
	}

	/* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
