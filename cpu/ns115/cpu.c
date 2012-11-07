/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
 * CPU specific code for an ARM core tile
 * Some boards supporting core tiles allow CPU architecture
 * to be detected at run time. However, where architecture
 * specific functions are provided, they must allow for the
 * fact that not all toolchains
 * support all architectures
 */

#include <common.h>
#include <command.h>
#include <asm-arm/setup.h>
#include <timestamp.h>


#define READ_TIMER (*(volatile ulong *)(CONFIG_SYS_TIMERBASE+4))

#if defined(CONFIG_USE_IRQ) || defined (CONFIG_REALVIEW)
DECLARE_GLOBAL_DATA_PTR;
#endif

void icache_flush(void);
void dcache_flush(void);
void icache_flush_arch(int arch);

int cpu_init (void)
{
	/*
	 * setup up stacks if necessary
	 */
#ifdef CONFIG_USE_IRQ
	IRQ_STACK_START = _armboot_start - CONFIG_SYS_MALLOC_LEN - CONFIG_SYS_GBL_DATA_SIZE - 4;
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
#endif


	unsigned int sysmode = *(unsigned int *)0x0582207c;

	int bus_mode[] = {400,333,266,200};
	int cpu_mode[] = {1200,1000,800,600};

//	printf("cpu freq = %dMHz, bus freq = %dMhz\n",cpu_mode[(sysmode>>3)&0x3],bus_mode[(sysmode>>5)&0x3]);
//	update_freq();
	
	return 0;
}

/*
 * This function is called just before we call linux
 * it prepares the processor for linux
 * For details regarding the desired state see e.g.
 *	http://www.arm.linux.org.uk/developer/booting.php
 *	http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html
 * Here we take care of any CPU specific actions
 * - platform specific actions, e.g. ensuring DMA is quiescent, 
 *   should be performed where this function is called 
 *
 */
int cleanup_before_linux (void)
{
	disable_interrupts ();
	dcache_disable();
	icache_flush();
	dcache_flush();
	return (0);
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void reset_cpu (ulong addr);

	disable_interrupts ();
	reset_cpu (0);
	/*NOTREACHED*/
	return (0);
}

/* May not be cached processor on the CT - do nothing */
void icache_enable (void)
{
}

void icache_disable (void)
{
}

void icache_flush(void)
{
#ifdef CONFIG_REALVIEW
	icache_flush_arch(gd->bd->bi_arch_number);
#else
	/* Unimplemented */
#endif
}

void dcache_enable(void)
{
}

void dcache_disable(void)
{
}

void dcache_flush(void)
{
}

void icache_flush_arch(int arch)
{
	switch(arch)
	{
		case 0xF: /* Revised - ASSUME v6/7 */
			{
				unsigned long i = 0;

				/* flush I-cache */
				icache_disable();
				asm ("mcr p15, 0, %0, c7, c5, 0": :"r" (i));  /* invalidate whole I cache */
				asm ("mcr p15, 0, %0, c7, c5, 6": :"r" (i));  /* flush btb */
				asm ("mcr p15, 0, %0, c7, c5, 4": :"r" (i));  /* ISB - deprecated but OK in V7 */
				break;
			}
		default:
			/* Unimplemented */
			break;
	}
}




/*
 * The following functions are defined so that the code in do_bootelf
 * causes the correct cleanup before we run an ELF image.
 */
/* return "enabled" */
int dcache_status(void)
{
	return 1;

}

/* return "disabled" */
int icache_status (void)
{
	return 0;
}

void update_freq()
{
	char *ns115_cpu[] = {"ns115", "cpu", "dvfs", "1000","1200000"};
	do_ns115(NULL, 0, 5, ns115_cpu);

//	char *ns115_cpu[] = {"ns115", "cpu", "clk", "set","1000","mhz" };
//	do_ns115(NULL, 0, 6, ns115_cpu);
	
	char *ns115_mali[] = {"ns115", "mali", "clk", "set","400","mhz" };
	do_ns115(NULL, 0, 6, ns115_mali);
	
	char *ns115_gc[] = {"ns115", "gc300", "clk", "set","480","mhz" };
	do_ns115(NULL, 0, 6, ns115_gc);
}

#if 0
#define efuse_debug(fmt, args...)   printf(fmt, ##args)
#else
#define efuse_debug(...)
#endif

#define RANDOM_MAX 0x7FFFFFFF
static unsigned int next = 1;

/* return value no more than 2^31 - 1;
 * 2^31 - 1 = 0x7fffffff;
 * 127773 * (7^5) + 2836,7^5 = 16807 ;
 * t = (7^5 * t) mod (2^31 - 1);
 */
static unsigned int random_serial(unsigned int value)
{
	unsigned int quotient, remainder, t;

	quotient = value / 127773;
	remainder = value % 127773;
	t = 16807 * remainder - 2836 * quotient;

	
	if (t <= 0)
		t += 0x7FFFFFFF;
		return ((value = t) % ((unsigned int)RANDOM_MAX + 1));
}

static void srand_serial(unsigned int seed)
{
	next = seed;
}


void get_cpu_efuse_data(unsigned int* low, unsigned int* high) {

    	// efuse register define.
	volatile unsigned int* preg_status    = 0x06160100;
	volatile unsigned int* preg_cen       = 0x06160104;
	volatile unsigned int* preg_addr      = 0x0616010c;
	volatile unsigned int* preg_req       = 0x06160108;
	volatile unsigned int* preg_data      = 0x06160110;
	volatile unsigned int* preg_intmask   = 0x06160120;
	volatile unsigned int* preg_intunmask = 0x0616012C;
	volatile unsigned int* preg_int       = 0x05041d0c;


	unsigned int start_sector_no = 2052;
	int sector_buf[512];

	efuse_debug("ns115-efuse, a. %d\n", __LINE__);

	while((*preg_status & 0x7) != 0b11) {
		*preg_cen = 1;                          // enable e-fuse IP core;
		udelay(10);
	}

	efuse_debug("ns115-efuse, b. %d\n", __LINE__);

	*preg_intunmask = 1;                        // enable e-fuse read data interrupt;
	*preg_addr      = ((1024 / 8) | 07 << 9);   // send read address to e-fuse , that is 0xe80;
	*preg_req       = 1;

	efuse_debug("ns115-efuse, c. %d, int:%x\n", __LINE__, *preg_int);

	unsigned int regint;
	do {
		regint = *preg_int;
		udelay(100);

		efuse_debug("ns115-efuse, c.1 %d, int:%x\n", __LINE__, regint);
	}
	while((regint & 1 << 18) == 0) ;          // wait interrupt.

	efuse_debug("ns115-efuse, d.%d\n", __LINE__);

	if (low)
		*low = preg_data[0];
	if (high)
		*high = preg_data[1];

	*preg_intmask  = 1;                         // clear e-fuse interrupt.


	if ((0 == *low) && (0 == *high)) {
		// must read data from 1088;
		const int dataoffset = 1088;
		*preg_addr = (dataoffset / 8) | (07 << 9);
		*preg_req  = 1;
		do {
			regint = *preg_int;
			udelay(100);
			efuse_debug("ns115-efuse, c.2 %d, int:%x\n", __LINE__, regint);
		} while( (regint & (1 << 18)) == 0 );
		if (low)
			*low = preg_data[0];
		if (high)
			*high = preg_data[1];
		*preg_intmask = 1;
	}

	if((*low == 0) && (*high == 0)){
		mmc_read(1,start_sector_no,sector_buf,1);
		if(((char )sector_buf[0]) == 'S'){
			*low = sector_buf[1];
			*high = sector_buf[2];
		}else{
			sector_buf[0] = 'S';
			efuse_debug("READ_TIMER === %x\n",READ_TIMER);
			sector_buf[1] = (random_serial(READ_TIMER) & 0xfffffff8 | 0x2);
			sector_buf[2] = random_serial(READ_TIMER);
			efuse_debug("sector_buf[1] = %x\n",sector_buf[1]);
			efuse_debug("sector_buf[2] = %x\n",sector_buf[2]);
			mmc_write(1,sector_buf,start_sector_no,1);
			*low = sector_buf[1];
			*high = sector_buf[2];
		}
		efuse_debug("sector_buf[0] = %c\n",sector_buf[0]);
	}
	efuse_debug("e-fuse data:%x, %x \n", *low, *high);
}

void get_board_serial(struct tag_serialnr *serialnr) {
	get_cpu_efuse_data(&serialnr->low, &serialnr->high);
} 
