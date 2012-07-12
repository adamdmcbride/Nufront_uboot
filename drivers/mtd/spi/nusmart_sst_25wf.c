/*  ============================================================================
 *   Copyright (c) Nufront 2007                 
 *   Author: Si Hongwei
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.             
 *   ===========================================================================
 */

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "nusmart_sst_25wf.h"

volatile unsigned int tmp_reg=0;
void spi_0_init(void)
{
	//dma cfg
	
	*SPI_0_SSIENR = 0;
	*SPI_0_IMR  = 0x0;	
	*SPI_0_DMACR  = 0x2;
	*SPI_0_DMATDLR = 0x0;


	*SPI_0_BAUDR  = 16;	
	


	//*SPI_0_BAUDR  = 8;		// 8;
	*SPI_0_CTRLR0  = 0x507;
	*SPI_0_CTRLR1  = 0x0;
	
	
	*SPI_0_SER  = 0x1;	
	*SPI_0_SSIENR = 1;
	
}

void SPI_FLASH_disable_write(void)
{
	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1C7;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x4;
	while((tmp_reg & 0x4)==0)
	{
		tmp_reg = *SPI_0_SR;
	}	
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x1)==1)
	{
		tmp_reg = *SPI_0_SR;
	}
}



void SPI_FLASH_enable_write(void)
{
	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1C7;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x6;
	while((tmp_reg & 0x4)==0)
	{
		tmp_reg = *SPI_0_SR;
	}	
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x1)==1)
	{
		tmp_reg = *SPI_0_SR;
	}
}


unsigned char SPI_FLASH_read_status(void)
{
	unsigned char tmp_reg = 0;	

	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x0CF;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x0500;
	while((tmp_reg & 0x4)==0)
	{
		tmp_reg = *SPI_0_SR;
	}	
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x1)==1)
	{
		tmp_reg = *SPI_0_SR;
	}
	tmp_reg = *SPI_0_DR;

	printf("spi flash status = 0x%x\n", tmp_reg);
	
	return tmp_reg;
}



void SPI_FLASH_GLOBAL_UNPROTECT()
{
	SPI_FLASH_enable_write();

	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1CF;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x0100;
	while((tmp_reg & 0x4)==0)
	{
		tmp_reg = *SPI_0_SR;
	}	
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x1)==1)
	{
		tmp_reg = *SPI_0_SR;
	}

	SPI_FLASH_POLL_BUSY();


	
	
}



void SPI_FLASH_sector_unprotect(unsigned int addr)
{
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	unsigned int j=0;
	unsigned int WAIT_CNT = 1000000;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;

	
	SPI_FLASH_enable_write();


	tmp_reg_addr1 = addr/65536;
	tmp_reg_addr2 = addr/256;
	tmp_reg_addr3 = addr%256;
	*SPI_0_DR = 0x39;
	*SPI_0_DR = tmp_reg_addr1;
	*SPI_0_DR = tmp_reg_addr2;
	*SPI_0_DR = tmp_reg_addr3;


}

int AT25DF_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	int i;

	SPI_FLASH_GLOBAL_UNPROTECT();

	for(i=0; i< len / CONFIG_ENV_SECT_SIZE; i++)
	{
		SPI_FLASH_sector_erase(offset + i*CONFIG_ENV_SECT_SIZE);
	}

	if(len % CONFIG_ENV_SECT_SIZE)
		SPI_FLASH_sector_erase(offset + i*CONFIG_ENV_SECT_SIZE);

	return 0;
	
}

unsigned char SPI_FLASH_READ_SECTOR_STATUS(unsigned int addr)
{
	unsigned int value;
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;


	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR1  = 0;
	*SPI_0_CTRLR0  = 0x3c7;
	*SPI_0_RXFTLR = 0;
	tmp_reg_addr1 = addr/65536;
	tmp_reg_addr2 = addr/256;
	tmp_reg_addr3 = addr%256;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x3C;
	*SPI_0_DR = tmp_reg_addr1;
	*SPI_0_DR = tmp_reg_addr2;
	*SPI_0_DR = tmp_reg_addr3;
	tmp_reg = *SPI_0_RISR;
	while((tmp_reg & 0x00000010)==0)
	{
		tmp_reg = *SPI_0_RISR;
	}
	value = *SPI_0_DR;
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x00000001)==1)
	{
		tmp_reg = *SPI_0_SR;
	}	

	return value;
}


void SPI_FLASH_sector_erase(unsigned int addr)
{
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	unsigned int j=0;
	volatile unsigned int WAIT_CNT = 1000000;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;

	tmp_reg = SPI_FLASH_READ_SECTOR_STATUS(addr);
	
	SPI_FLASH_enable_write();

	tmp_reg_addr1 = addr/65536;
	tmp_reg_addr2 = addr/256;
	tmp_reg_addr3 = addr%256;
	*SPI_0_DR = 0x20;
	*SPI_0_DR = tmp_reg_addr1;
	*SPI_0_DR = tmp_reg_addr2;
	*SPI_0_DR = tmp_reg_addr3;

	WAIT_CNT = 100000;
	while(WAIT_CNT--);

	SPI_FLASH_POLL_BUSY();
}

unsigned char SPI_FLASH_POLL_BUSY(void)
{
	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x0CF;
	*SPI_0_CTRLR1  = 0;
	*SPI_0_SSIENR = 1;
	do{
		
		do{
			tmp_reg =*SPI_0_SR;
		}while(tmp_reg != 0x6);
		

		*SPI_0_DR = 0x0500;
		
		do{
			tmp_reg =*SPI_0_SR;
		}while(tmp_reg != 0xE);
		
		
		tmp_reg = *SPI_0_DR;
		//printf("Poll status = 0x%x\n", tmp_reg);
	}while(tmp_reg & 0x1);

	return tmp_reg;
}
void SPI_FLASH_program_byte(unsigned int addr, unsigned char value)
{
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;

	SPI_FLASH_enable_write();

	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1C7;
	*SPI_0_SSIENR = 1;	


	tmp_reg_addr1 = addr/65536;
	tmp_reg_addr2 = addr/256;
	tmp_reg_addr3 = addr%256;
	*SPI_0_DR = 0x2;
	*SPI_0_DR = tmp_reg_addr1;
	*SPI_0_DR = tmp_reg_addr2;
	*SPI_0_DR = tmp_reg_addr3;

	
	
	tmp_reg = *SPI_0_RISR;
	while((tmp_reg & 0x00000001)==0)
	{
		tmp_reg = *SPI_0_RISR;
	}
	*SPI_0_DR = value;

	while((tmp_reg & 0x4)==0)
	{
		tmp_reg = *SPI_0_SR;
	}	
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x1)==1)
	{
		tmp_reg = *SPI_0_SR;
	}	

	tmp_reg =100000;
	while(tmp_reg--);

}

int AT25DF_read(struct spi_flash *flash,unsigned int addr, unsigned int length, unsigned char * buf)
{
	unsigned int value;
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;


	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR1  = length - 1;
	*SPI_0_CTRLR0  = 0x3c7;
	*SPI_0_RXFTLR = 0;
	tmp_reg_addr1 = addr/65536;
	tmp_reg_addr2 = addr/256;
	tmp_reg_addr3 = addr%256;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x3;
	*SPI_0_DR = tmp_reg_addr1;
	*SPI_0_DR = tmp_reg_addr2;
	*SPI_0_DR = tmp_reg_addr3;

	do{
		tmp_reg = *SPI_0_SR;
		while((tmp_reg & 0x8)==0)
		{
			tmp_reg = *SPI_0_SR;
		}
		*buf++ = *SPI_0_DR;

		tmp_reg = *SPI_0_SR;
	}while((tmp_reg & 0x00000001)==1);
	

		

	return 0;
}

unsigned char SPI_FLASH_read_byte(unsigned int addr)
{
	unsigned int value;
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;


	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR1  = 0;
	*SPI_0_CTRLR0  = 0x3c7;
	*SPI_0_RXFTLR = 0;
	tmp_reg_addr1 = addr/65536;
	tmp_reg_addr2 = addr/256;
	tmp_reg_addr3 = addr%256;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x3;
	*SPI_0_DR = tmp_reg_addr1;
	*SPI_0_DR = tmp_reg_addr2;
	*SPI_0_DR = tmp_reg_addr3;
	tmp_reg = *SPI_0_RISR;
	while((tmp_reg & 0x00000010)==0)
	{
		tmp_reg = *SPI_0_RISR;
	}
	value = *SPI_0_DR;
	tmp_reg = *SPI_0_SR;
	while((tmp_reg & 0x00000001)==1)
	{
		tmp_reg = *SPI_0_SR;
	}	

	return value;
}


int DisplaySPIFlash(int addr, int cnt)
{
	
	unsigned int i=0;
	unsigned int value;
	unsigned int sec_cnt = 0;
	

	//config spi
	*SPI_0_SSIENR = 0;
	*SPI_0_IMR  = 0;	
	*SPI_0_BAUDR  = 100;	
	*SPI_0_TXFTLR = 4;
	*SPI_0_SER  = 1;

	for(i=0; i<cnt; i++)
	{
//		*((char *)(SPI_BUF_2+i)) = 0;
//		value = SPI_FLASH_read_byte(addr+i);
//		*((char *)(SPI_BUF_2+i)) = value;
	}

	
}
int AT25DF_write(struct spi_flash *flash,u32 offset, size_t len, const void *buf)
{
	
	AT25DF_AAI_write(flash,offset,len, buf);
	return 0;

}


int AT25DF_byte_write(struct spi_flash *flash,
                u32 offset, size_t len, const void *buf)
{
	int i;

	for(i=0; i<len;)
	{
/*
		tmp_reg = SPI_FLASH_read_byte(offset+i);
		if(tmp_reg != 0xFF)
		{	
			printf("0x%x is not 0xFF, check erase process\n", offset+i);
			return -1;
		}


		SPI_FLASH_POLL_BUSY();
*/

		SPI_FLASH_program_byte(offset+i, *(((unsigned char*)buf)+i));
		
		tmp_reg = SPI_FLASH_POLL_BUSY();

		//tmp_reg = SPI_FLASH_read_status();

		if(tmp_reg & 0x20)
		{
			printf( "\r\nProgram SPI Flash fail");
			return 0;
		}


		tmp_reg = SPI_FLASH_read_byte(offset+i);

		if(tmp_reg == *(((unsigned char*)buf)+i))
		{
			i++;
			if(((i*100/len) > ((i-1)*100/len)) && (((i*100/len)%5)==4))
				printf("\b\b\b\b%3d%%", i*100/len+1);
		}
		else
		{
			printf("one byte write failed\n");
		}
	}

	return 0;
}



struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
                unsigned int max_hz, unsigned int spi_mode)
{
        struct spi_slave *spi;
        struct spi_flash *flash;
        int ret;
        u8 idcode[5];
	
	spi = malloc(sizeof(struct spi_slave));
	if(!spi)
		return NULL;
	spi->bus = bus;
	spi->cs = cs;

	flash = malloc(sizeof(struct spi_flash));
	if(!flash)
		return NULL;

	flash->spi = spi;

	flash->read = AT25DF_read;
	flash->erase = AT25DF_erase;
	flash->write = AT25DF_write;

	spi_0_init();

	return flash;

}

void spi_flash_free(struct spi_flash *flash)
{
        free(flash->spi);
        free(flash);
}

//AAI
int AT25DF_AAI_write(struct spi_flash *flash,u32 offset, size_t len, const void *buf)
{
	int i;
	unsigned char tmp_reg_addr1,tmp_reg_addr2,tmp_reg_addr3;
	tmp_reg_addr1 = 0;
	tmp_reg_addr2 = 0;
	tmp_reg_addr3 = 0;

	SPI_FLASH_enable_write();


	tmp_reg_addr1 = offset/65536;
	tmp_reg_addr2 = offset/256;
	tmp_reg_addr3 = offset%256;


	for(i=0; i<len; i+=2)
	{
		*SPI_0_SSIENR = 0;
		*SPI_0_CTRLR0  = 0x1C7;
		*SPI_0_SSIENR = 1;	

	//send cmd	
		*SPI_0_DR = 0xAD;

		//send addresses at beginning
		if(i==0)
		{
			*SPI_0_DR = tmp_reg_addr1;
			*SPI_0_DR = tmp_reg_addr2;
			*SPI_0_DR = tmp_reg_addr3;
		}

		//send two bytes	
		*SPI_0_DR = ((unsigned char*)buf)[i];
		*SPI_0_DR = ((unsigned char*)buf)[i+1];
		
		//wait tx fifo empty
		do{
			tmp_reg = *SPI_0_SR;
		}while((tmp_reg & 0x4)==0);
		//wait spi transfer done
		do{
			tmp_reg = *SPI_0_SR;
		}while((tmp_reg & 0x1)==1);


		//check for Flash busy status
		SPI_FLASH_POLL_BUSY();
		//printf("%d bytes\n", i);	
	}


	SPI_FLASH_disable_write();
	//SPI_FLASH_read_status();

	for(i=0;i<len;i++)
	{
		tmp_reg = SPI_FLASH_read_byte(offset+i);
		//printf("addr=0x%x, read=0x%x, expect=0x%x\n",i,tmp_reg,*(((unsigned char*)buf)+i));
                
		if(tmp_reg == *(((unsigned char*)buf)+i))
                {
                        if(((i*100/len) > ((i-1)*100/len)) && (((i*100/len)%10)==9))
                                printf("\b\b\b\b%3d%%", i*100/len+1);
                }
                else
                {
                        printf("verify failed, please retry\n");
			return 0;
                }
		
	}


	return 0;
}


