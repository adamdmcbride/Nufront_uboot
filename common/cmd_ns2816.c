/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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

#include <common.h>
#include <command.h>

#if defined(CONFIG_CMD_NS2816)
#include <spi_flash.h>
#include <environment.h>
//extern struct spi_flash *env_flash;
extern env_t *env_ptr;
extern  spi_0_init();
volatile int nest = 0;
#if 0
void menuconfig()
{
	int os_num = 0;
	int r=0,o=0,arrow=0;
	char * cmd;
	char val;

	char os_name[CONFIG_MAX_OS][16];

	char cmd_name[CONFIG_MAX_OS][16];
	
	os_num = simple_strtoul(getenv("osnum"), NULL, 10);

	if(os_num == 0 || os_num > CONFIG_MAX_OS)
	{
		printf("No System option, exit\n");
		return;
	}

	for(o=0; o<os_num; o++)
	{
		sprintf(os_name[o], "menuos_%d", o+1);
		sprintf(cmd_name[o], "menucmd_%d", o+1);
	}
	

	for(r=0; r<CONFIG_LINE_BEGIN; r++)
		printf("\n");

	
	printf("\t\t\t\tPlease Select your System :\n");	
	printf("\n");
	r += 2;

	for(o=0; o<os_num; o++)
	{
		printf("\t\t\t[%d]\t%s\n", o+1, getenv(os_name[o]));	

		printf("\n");
		r += 2;
	}

	for(;r<CONFIG_ROW_NUM;r++)
		printf("\n");

		
	while(1)
	{
		val = getc();
		if(val == 0x03)   // ^C
			return;
		
		val = simple_strtoul(&val,NULL,10);

		if(val >= 1 && val <= os_num)
		{
			if ((cmd = getenv (cmd_name[val-1])) == NULL)
				printf ("## Error: \"%s\" not defined\n", cmd_name[val]);

			for(r=0; r<CONFIG_ROW_NUM; r++)
				printf("\n");
						
			run_command(cmd,0);
			return;
		}
			
		continue;
		
	}
	
}
#endif

int do_ns2816(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i,rc = 0;

	//hdmi
	if(strcmp(argv[1], "hdmi") == 0)
	{
#if defined(CONFIG_NUSMART_HDMI)
		{
			TPI_Init();
			TPI_Poll();
			TPI_Poll();
			return 1;
		}
#endif
#if defined(CONFIG_CH7033_HDMI)
		{
			InitializeCH7033(argv[2]);
			return 1;
		}
#endif
	}
	//menu
#if 0
	if((strcmp(argv[1], "menu") == 0) && argc == 2)
	{
		menuconfig();
		return 1;

	}
#endif
	if((strcmp(argv[1], "mvenv") == 0) && argc == 4)
	{
		char * addr = (char *)simple_strtoul(argv[2], NULL, 16);
		int size = simple_strtoul(argv[3], NULL, 10);
		int i,j;		
	
		if (size > CONFIG_ENV_SIZE) 
		{   
	                puts ("*** Error - default environment is too large\n\n");
	                return;
        	}

		for(i=0; i<size-1; i++)
		{
			if(addr[i] == 0xD && addr[i+1] == 0xA)
			{
				addr[i] = 0x0;
				for(j=i+1;j<size;j++)
					addr[j] = addr[j+1];
			
				size--;
			}
		}
		addr[i+1] = 0;
		addr[i+2] = 0;
		memset(env_ptr->data,0,CONFIG_ENV_SIZE);
       		memcpy(env_ptr->data, addr,size);
       		env_crc_update ();
		printf("new env imported\n");
		return 1;		
	}

	if((strcmp(argv[1], "update") == 0)  && argc == 5)
	{
		unsigned char* src_offset = simple_strtoul(argv[3], NULL, 16);
		int size = simple_strtoul(argv[4], NULL, 10);
		spi_0_init();
		if(strcmp(argv[2], "xload") == 0)
		{
			//spi_flash_erase(env_flash, 0, size);
                        #if (defined(CONFIG_SPI_FLASH_ATMEL_25DF) || defined(CONFIG_SPI_FLASH_SST_25WF))
			printf("erasing xload ...");
			AT25DF_erase(0,0,size);
			printf("writing xload ...");
			//spi_flash_write(env_flash, 0, size, src_offset);
			AT25DF_write(0, 0, size, src_offset);
			printf("done\n");
			#endif
			return 1;
		}


                if(strcmp(argv[2], "qxload") == 0)
                {
                        //spi_flash_erase(env_flash, 0, size);
                        #if (defined(CONFIG_SPI_FLASH_ATMEL_25DF) || defined(CONFIG_SPI_FLASH_SST_25WF))
                     	printf("erasing q xload ...");
                        AT25DF_erase(0,0x8000,size);
                        printf("writing q xload ...");
                        //spi_flash_write(env_flash, 0, size, src_offset);
                        AT25DF_write(0, 0x8000, size, src_offset);
                        printf("done\n");
                        #endif
                        return 1;
                }

		if(strcmp(argv[2], "uboot") == 0)
		{
			//spi_flash_erase(env_flash, 0, size);
                        #if defined(CONFIG_SPI_FLASH_ATMEL_25DF) || defined(CONFIG_SPI_FLASH_SST_25WF)                       
			// 4 bytes size(uboot+crc) + uboot content + 4bytes crc	
			u32 crc = crc32(0, src_offset, size);  //generate a crc signature
			src_offset -= 4;
			size += 4;
			*((unsigned int*)src_offset) = size;
		
			*((unsigned int*)(src_offset+size)) = crc;
		
			size += 4;
			printf("erasing u-boot ...");
			AT25DF_erase(0,0x10000,size);
			
				
			printf("writing u-boot, crc = 0x%x ...    ",crc);
			//spi_flash_write(env_flash, 0, size, src_offset);
			AT25DF_write(0, 0x10000, size, src_offset);
			printf("done\n");
			#endif
			return 1;
		}
	}
	if((strcmp(argv[1], "wfi") == 0)  && argc == 2)
	{
//		*((unsigned int*)0x051b0014) = 0;
//		*((unsigned int*)0x051b0014) = 0x00004000;
		
//		*((unsigned int*)0x051b0014) = 0;
//		*((unsigned int*)0x051b0014) = 0x00008000;
		
//		*((unsigned int*)0x051b0014) = 0;
//		*((unsigned int*)0x051b0014) = 0x00010000;
		
//		*((unsigned int*)0x051b0014) = 0x00002000;
//		*((unsigned int*)0x051b0014) = 0x00002800;
		
//		*((unsigned int*)0x051b0014) = 0x00001000;
//		*((unsigned int*)0x051b0014) = 0x00001400;
		
//		*((unsigned int*)0x051b0014) = 0x00002000;
//		*((unsigned int*)0x05040008) = 0x00002300;

//		*((unsigned int*)0x051b0014) = 0x00001000;
//		*((unsigned int*)0x05040008) = 0x00001003;

		_wfi_asm();	
		
		return 1;

	}
	if((strcmp(argv[1], "i2c") == 0)  && argc == 4)
	{
		unsigned char i2c_bus = simple_strtoul(argv[2], NULL, 10); 	
		unsigned char slave_addr = simple_strtoul(argv[3], NULL, 16);
		u32	i2c_base = 0x0;

		if(i2c_bus == 0)
		{
			i2c_base = 0x05120000;
		}
		if(i2c_bus == 1)
		{
			i2c_base =  0x05190000;
		}
 		if(i2c_bus == 2)
		{
			i2c_base =  0x05180000;
		} 	 

		printf("I2C check 0x%x at bus %d, base = 0x%x\n",slave_addr,i2c_bus,i2c_base);	


		#define                 REG_READ(addr)          *((volatile unsigned int *)addr)
		#define                 REG_WRITE(addr, data)   *((volatile unsigned int *)addr) = data
		#define MASTER_MODE             (1 << 0)
		#define STD_SPEED               (1 << 1)
		#define IC_SLAVE_DISABLE        (1 << 6)        
		#define IC_RESTART_EN           (1 << 5) 
		#define         I2C_STOP_DET            0x200
		#define         I2C_START_DET           0x400

		//init master
        	REG_WRITE((i2c_base+0x6C), 0);

	   	REG_WRITE((i2c_base+0), MASTER_MODE | STD_SPEED | IC_SLAVE_DISABLE|IC_RESTART_EN);

               	REG_WRITE((i2c_base+0x4), slave_addr);

        	REG_WRITE((i2c_base+0x6C), 1);

        	REG_READ((i2c_base + 0x40));

		//send slave addr + reg adrr 0x0
		REG_WRITE((i2c_base+0x10), 0x00);
		
		// wait start bit
		while((REG_READ((i2c_base+0x34)) & I2C_START_DET) != I2C_START_DET);
		//wait stop bit
		while((REG_READ((i2c_base+0x34)) & I2C_STOP_DET) != I2C_STOP_DET);

		printf("The Intr Stat = 0x%x,",REG_READ((i2c_base+0x34)));
		
		if(REG_READ((i2c_base+0x34)) & 0x40)
		{
			
			printf("TX Abort! Abort Source = 0x%x\n",REG_READ((i2c_base+0x80)));
		}
		else
		{
			printf("I2C device Exits\n");
		}		

		REG_READ((i2c_base+0x60));
	        REG_READ((i2c_base+0x54));

		REG_WRITE((i2c_base+0x6C), 0);
		return 1;
	}

	cmd_usage(cmdtp);	
	return 0;
}



U_BOOT_CMD(
	ns2816, 6, 0, do_ns2816,
	"ns2816 sub system",
	"update xload <addr> <length>: update x-loader\n"
	"ns2816 update uboot <addr> <length>: update u-boot\n"
	"ns2816 mvenv <addr> <length>: import env\n"
	"ns2816 hdmi: init VGA & HDMI chip\n");
#endif
