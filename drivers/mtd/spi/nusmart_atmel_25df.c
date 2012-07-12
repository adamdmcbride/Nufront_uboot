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

#include "nusmart_atmel_25df.h"

volatile unsigned char temp=0;
void spi_0_init(void)
{
	//dma cfg
	
	*SPI_0_SSIENR = 0;
	*SPI_0_IMR  = 0x0;	
	*SPI_0_DMACR  = 0x2;
	*SPI_0_DMATDLR = 0x0;


	*SPI_0_BAUDR  = 33;	
	


	//*SPI_0_BAUDR  = 8;		// 8;
	*SPI_0_CTRLR0  = 0x507;
	*SPI_0_CTRLR1  = 0x0;
	
	
	*SPI_0_SER  = 0x1;	
	*SPI_0_SSIENR = 1;
	
}


void SPI_FLASH_enable_write(void)
{
	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1C7;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x6;
	while((temp & 0x4)==0)
	{
		temp = *SPI_0_SR;
	}	
	temp = *SPI_0_SR;
	while((temp & 0x1)==1)
	{
		temp = *SPI_0_SR;
	}
}


unsigned char SPI_FLASH_read_status(void)
{
	unsigned char temp = 0;	

	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x0CF;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x0500;
	while((temp & 0x4)==0)
	{
		temp = *SPI_0_SR;
	}	
	temp = *SPI_0_SR;
	while((temp & 0x1)==1)
	{
		temp = *SPI_0_SR;
	}
	temp = *SPI_0_DR;

	//printf("spi flash status = 0x%x\n", temp);
	
	return temp;
}



void SPI_FLASH_GLOBAL_UNPROTECT()
{
	SPI_FLASH_enable_write();

	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1CF;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x0100;
	while((temp & 0x4)==0)
	{
		temp = *SPI_0_SR;
	}	
	temp = *SPI_0_SR;
	while((temp & 0x1)==1)
	{
		temp = *SPI_0_SR;
	}

	SPI_FLASH_POLL_BUSY();


	
	
}



void SPI_FLASH_sector_unprotect(unsigned int addr)
{
	unsigned char temp_addr1,temp_addr2,temp_addr3;
	unsigned int j=0;
	unsigned int WAIT_CNT = 1000000;
	temp_addr1 = 0;
	temp_addr2 = 0;
	temp_addr3 = 0;

	
	SPI_FLASH_enable_write();


	temp_addr1 = addr/65536;
	temp_addr2 = addr/256;
	temp_addr3 = addr%256;
	*SPI_0_DR = 0x39;
	*SPI_0_DR = temp_addr1;
	*SPI_0_DR = temp_addr2;
	*SPI_0_DR = temp_addr3;


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
	unsigned char temp_addr1,temp_addr2,temp_addr3;
	temp_addr1 = 0;
	temp_addr2 = 0;
	temp_addr3 = 0;


	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR1  = 0;
	*SPI_0_CTRLR0  = 0x3c7;
	*SPI_0_RXFTLR = 0;
	temp_addr1 = addr/65536;
	temp_addr2 = addr/256;
	temp_addr3 = addr%256;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x3C;
	*SPI_0_DR = temp_addr1;
	*SPI_0_DR = temp_addr2;
	*SPI_0_DR = temp_addr3;
	temp = *SPI_0_RISR;
	while((temp & 0x00000010)==0)
	{
		temp = *SPI_0_RISR;
	}
	value = *SPI_0_DR;
	temp = *SPI_0_SR;
	while((temp & 0x00000001)==1)
	{
		temp = *SPI_0_SR;
	}	

	return value;
}


void SPI_FLASH_sector_erase(unsigned int addr)
{
	unsigned char temp_addr1,temp_addr2,temp_addr3;
	unsigned int j=0;
	volatile unsigned int WAIT_CNT = 1000000;
	temp_addr1 = 0;
	temp_addr2 = 0;
	temp_addr3 = 0;

	temp = SPI_FLASH_READ_SECTOR_STATUS(addr);
	
	SPI_FLASH_enable_write();

	temp_addr1 = addr/65536;
	temp_addr2 = addr/256;
	temp_addr3 = addr%256;
	*SPI_0_DR = 0x20;
	*SPI_0_DR = temp_addr1;
	*SPI_0_DR = temp_addr2;
	*SPI_0_DR = temp_addr3;

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
			temp =*SPI_0_SR;
		}while(temp != 0x6);
		

		*SPI_0_DR = 0x0500;
		
		do{
			temp =*SPI_0_SR;
		}while(temp != 0xE);
		
		
		temp = *SPI_0_DR;
	}while(temp & 0x1);

	return temp;
}
void SPI_FLASH_program_byte(unsigned int addr, unsigned char value)
{
	unsigned char temp_addr1,temp_addr2,temp_addr3;
	temp_addr1 = 0;
	temp_addr2 = 0;
	temp_addr3 = 0;

	SPI_FLASH_enable_write();

	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR0  = 0x1C7;
	*SPI_0_SSIENR = 1;	


	temp_addr1 = addr/65536;
	temp_addr2 = addr/256;
	temp_addr3 = addr%256;
	*SPI_0_DR = 0x2;
	*SPI_0_DR = temp_addr1;
	*SPI_0_DR = temp_addr2;
	*SPI_0_DR = temp_addr3;

	
	
	temp = *SPI_0_RISR;
	while((temp & 0x00000001)==0)
	{
		temp = *SPI_0_RISR;
	}
	*SPI_0_DR = value;

	while((temp & 0x4)==0)
	{
		temp = *SPI_0_SR;
	}	
	temp = *SPI_0_SR;
	while((temp & 0x1)==1)
	{
		temp = *SPI_0_SR;
	}	

	temp =100000;
	while(temp--);

}

int AT25DF_read(struct spi_flash *flash,unsigned int addr, unsigned int length, unsigned char * buf)
{
	unsigned int value;
	unsigned char temp_addr1,temp_addr2,temp_addr3;
	temp_addr1 = 0;
	temp_addr2 = 0;
	temp_addr3 = 0;


	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR1  = length - 1;
	*SPI_0_CTRLR0  = 0x3c7;
	*SPI_0_RXFTLR = 0;
	temp_addr1 = addr/65536;
	temp_addr2 = addr/256;
	temp_addr3 = addr%256;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x3;
	*SPI_0_DR = temp_addr1;
	*SPI_0_DR = temp_addr2;
	*SPI_0_DR = temp_addr3;

	do{
		temp = *SPI_0_SR;
		while((temp & 0x8)==0)
		{
			temp = *SPI_0_SR;
		}
		*buf++ = *SPI_0_DR;

		temp = *SPI_0_SR;
	}while((temp & 0x00000001)==1);
	

		

	return 0;
}

unsigned char SPI_FLASH_read_byte(unsigned int addr)
{
	unsigned int value;
	unsigned char temp_addr1,temp_addr2,temp_addr3;
	temp_addr1 = 0;
	temp_addr2 = 0;
	temp_addr3 = 0;


	*SPI_0_SSIENR = 0;
	*SPI_0_CTRLR1  = 0;
	*SPI_0_CTRLR0  = 0x3c7;
	*SPI_0_RXFTLR = 0;
	temp_addr1 = addr/65536;
	temp_addr2 = addr/256;
	temp_addr3 = addr%256;
	*SPI_0_SSIENR = 1;
	*SPI_0_DR = 0x3;
	*SPI_0_DR = temp_addr1;
	*SPI_0_DR = temp_addr2;
	*SPI_0_DR = temp_addr3;
	temp = *SPI_0_RISR;
	while((temp & 0x00000010)==0)
	{
		temp = *SPI_0_RISR;
	}
	value = *SPI_0_DR;
	temp = *SPI_0_SR;
	while((temp & 0x00000001)==1)
	{
		temp = *SPI_0_SR;
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

int AT25DF_write(struct spi_flash *flash,
                u32 offset, size_t len, const void *buf)
{
	int i;

	for(i=0; i<len;)
	{
/*
		temp = SPI_FLASH_read_byte(offset+i);
		if(temp != 0xFF)
		{	
			printf("0x%x is not 0xFF, check erase process\n", offset+i);
			return -1;
		}


		SPI_FLASH_POLL_BUSY();
*/

		SPI_FLASH_program_byte(offset+i, *(((unsigned char*)buf)+i));
		
		temp = SPI_FLASH_POLL_BUSY();

		//temp = SPI_FLASH_read_status();

		if(temp & 0x20)
		{
			printf( "\r\nProgram SPI Flash fail");
			return 0;
		}


		temp = SPI_FLASH_read_byte(offset+i);

		if(temp == *(((unsigned char*)buf)+i))
		{
			i++;
			if((i*100/len) > ((i-1)*100/len))
				printf("\b\b\b\b%3d%%", i*100/len);
		}
		else
		{
			printf("one byte write failed\n");
		}
	}

	return 0;
}

int SPI_FLASH(unsigned int addr, unsigned int  size)
{
	unsigned int i=0;
	unsigned int value;
	unsigned int sec_cnt = 0;
	volatile delay = 0;
	

	//config spi
	*SPI_0_SSIENR = 0;
	*SPI_0_IMR  = 0;	
	*SPI_0_BAUDR  = 100;	
	*SPI_0_TXFTLR = 4;
	*SPI_0_SER  = 1;

	SPI_FLASH_GLOBAL_UNPROTECT();
	delay = 100000;
	while(delay--);
	SPI_FLASH_POLL_BUSY();
	temp = SPI_FLASH_read_status();

	//chip erase
	#define SECTOR_SIZE 4096
	sec_cnt = size / SECTOR_SIZE;
	sec_cnt += (size % SECTOR_SIZE)?1:0;	
	
	for(i=0; i<sec_cnt; i++)
	{
		//SPI_FLASH_sector_unprotect(addr + i * SECTOR_SIZE);
		SPI_FLASH_POLL_BUSY();
		temp = SPI_FLASH_READ_SECTOR_STATUS(addr + i * SECTOR_SIZE);

		if(temp != 0)
		{
			printf( "\r\nUnprotect SPI Flash fail");
			return 0;
		}


		SPI_FLASH_sector_erase(addr + i * SECTOR_SIZE);
		SPI_FLASH_POLL_BUSY();
		temp = SPI_FLASH_read_status();
		
		if(temp & 0x20)
		{
			printf( "\r\nErase SPI Flash fail");
			return 0;
		}

	}

	//for(i=0; i<size; i++)
	//{
	//		value = SPI_FLASH_read_byte(addr+i);
	//	*((char *)(SPI_BUF_1+i)) = value;
	//}	

	//temp = SPI_FLASH_read_status();
	//write one byte
	for(i=0; i<size; i++)
	{
		SPI_FLASH_POLL_BUSY();
		//SPI_FLASH_sector_unprotect(addr + i);
		//SPI_FLASH_POLL_BUSY();
		//temp = SPI_FLASH_READ_SECTOR_STATUS(addr+i);
		//SPI_FLASH_program_byte(addr+i, *((char *)(SPI_BUF_1+i)));
		SPI_FLASH_POLL_BUSY();
		temp = SPI_FLASH_read_status();

		if(temp & 0x20)
		{
			printf( "\r\nProgram SPI Flash fail");
			return 0;
		}


		value = SPI_FLASH_read_byte(addr+i);
		SPI_FLASH_POLL_BUSY();

		//if(value != *((char *)(SPI_BUF_1+i)))
		//	return i;
	}
	
	
	return i;
	
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
