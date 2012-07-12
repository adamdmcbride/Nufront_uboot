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

