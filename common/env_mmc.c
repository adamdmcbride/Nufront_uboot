/*
 * (C) Copyright 2009 Texas Instruments, <www.ti.com>
 * Kishore Kadiyala <kishore.kadiyala@ti.com>
 * Moiz Sonasath <m-sonasath@ti.com>
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

#include <common.h>

#if defined(CONFIG_ENV_IS_IN_EMMC) /* Environment is in EMMC */

#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#ifdef CONFIG_NUFRONT_MMC
#include <asm/arch/mmc.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/mmc_protocol.h>
#else
#include <mmc.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* References to names in env_common.c */
extern uchar default_environment[];
//extern env_t *env_ptr = NULL;

#ifdef ENV_IS_VARIABLE
char *mmc_env_name_spec = "EMMC";
/* update these elsewhere */
extern env_t *env_ptr;

#else /* !ENV_IS_VARIABLE */

//extern env_t *env_ptr;
env_t *env_ptr;
char *env_name_spec = "EMMC";
#ifdef ENV_IS_EMBEDDED
extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);
#else /* ! ENV_IS_EMBEDDED */

//env_t *env_ptr = (env_t *)CONFIG_ENV_ADDR;
#endif /* ENV_IS_EMBEDDED */

#endif /* ENV_IS_VARIABLE */

#ifdef ENV_IS_VARIABLE
uchar mmc_env_get_char_spec(int index)
#else
uchar env_get_char_spec(int index)
#endif
{	
//	printf("gd->env_addr=0x%x\n",gd->env_addr);
//	printf("*((uchar *)(gd->env_addr + index))===%s\n",*((uchar *)(gd->env_addr + index)));
	return *((uchar *)(gd->env_addr + index));
}

#ifdef ENV_IS_VARIABLE
void mmc_env_relocate_spec(void)
#else
void env_relocate_spec(void)
#endif
{
	unsigned int mmc_cont = (*((unsigned int*)0x0704e04c)) ? 1 : 0;
	unsigned long env_addr;
	int use_default = 0;
	unsigned char *fb_buf = (unsigned char *)0x80007fc0;

	env_addr = CONFIG_ENV_ADDR;
	printf("Getting env from mmc slot[%d]\n",mmc_cont);
	mmc_init(mmc_cont);
	mmc_read(mmc_cont, env_addr, (u_char *) env_ptr,CONFIG_ENV_SIZE/CONFIG_ENV_SECT_SIZE);

	if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
		use_default = 1;

	if (use_default) {
		puts("*** Warning - bad CRC, using default environment\n\n");
		memcpy(env_ptr->data, default_environment, CONFIG_ENV_SIZE);
		env_ptr->crc = crc32(0, env_ptr->data, CONFIG_ENV_SIZE);
	}
}

#ifdef ENV_IS_VARIABLE
int mmc_saveenv(void)
#else
int saveenv(void)
#endif
{
	unsigned long env_addr = CONFIG_ENV_ADDR; //4592
	int    i, len;
	unsigned int mmc_cont = (*((unsigned int*)0x0704e04c)) ? 1 : 0;
	env_t * check_buf = NULL;	

	len      = (CONFIG_ENV_SIZE + CONFIG_ENV_SECT_SIZE - 1)/CONFIG_ENV_SECT_SIZE; //8192,512

//	for (i = 0;i < ENV_SIZE; i++)
//		printf("env_ptr->data[%d] = 0x%x\n",i,env_ptr->data[i]);

	/* update crc */
	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE); //ENV_SIZE = (CONFIG_ENV_SIZE - ENV_HEADER_SIZE)
	puts("Writing to MMC... ");
	mmc_init(mmc_cont);
	mmc_write(mmc_cont, (u_char *) env_ptr, env_addr, len);
	printf("done\n");

	check_buf = (env_t*)malloc(CONFIG_ENV_SIZE);
	mmc_init(mmc_cont);
	mmc_read(mmc_cont,env_addr,check_buf,len);
//	for (i = 0;i < ENV_SIZE; i++)
//		printf("env_ptr->data[%d] = 0x%x\n",i,check_buf->data[i]);
	
	
	if (crc32(0, check_buf->data, ENV_SIZE) != env_ptr->crc)
		printf("Crc check failed at written env, try again\n");

	free(check_buf);
	gd->env_valid = 1;
	return 0;

}

#ifdef ENV_IS_VARIABLE
int mmc_env_init(void)
#else
int  env_init(void)
#endif
{
	/* use default */
	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 1;
	return 0;
}

#endif /* CONFIG_ENV_IS_IN_EMMC */
