#include "dma_330.h"

void dma_inten(unsigned int intnum)
{
	*DMA330_INTEN |= 1 << intnum;
}

void dma_intdisable(unsigned int intnum)
{
	*DMA330_INTEN &= ~(1 << intnum);
}

int dma_int_poll(unsigned int intnum)
{
	return ((*DMA330_INTSTATUS >> intnum) & 1);
}

void dma_int_clear(unsigned int intnum)
{
	*DMA330_INTCLEAR = (1 << intnum);
}

void dma_kill(unsigned int cha_num)
{
	int temp;

	temp = *DMA330_DBGSTATUS;
	while(temp == 1)
	     temp = *DMA330_DBGSTATUS;

	*DMA330_DBGINST0 = 0x0001 | cha_num << 8;
	*DMA330_DBGCMD   = 0x0;

	
}
unsigned int DMA330_readReg(unsigned int * addr)
{
	return *addr;
}

void set_inst0(unsigned int value)
{
	*DMA330_DBGINST0 = value; 
	value = *DMA330_DBGINST0;	//dummy read to prove execute
}

void set_inst1(unsigned int value)
{
	*DMA330_DBGINST1 = value; 
	value = *DMA330_DBGINST1;
}

void set_dbgcmd(void)
{
	*DMA330_DBGCMD = 0;
}

void dma_error(void)
{
	
	unsigned int temp;
	temp = 0x00A00000 | 1 << 17 | (0) << 24; 
	set_inst0(temp);
	
	//set inst 1
	temp = 0x02000000;  //the addr of code for dma channel to execute
	set_inst1(temp);
		
	//go
	set_dbgcmd();

}

void dma_go(	unsigned int cha_num,
		unsigned int no_secure,		//only non-secure used
 		dma_code_template_0 * code_tp,  //must be placed in ddr
		unsigned int ccr,
		unsigned int sar,		//must be in ddr
		unsigned int dar,		//must be in ddr
		unsigned char loop_cnt_0,	// <=255
		unsigned char loop_cnt_1,	// <=255
		unsigned char sev_num				   
				   )
{
	unsigned int temp;
	//poll dbg status 
	temp = *DMA330_DBGSTATUS;
	while(temp == 1)
	     temp = *DMA330_DBGSTATUS;
	
	//ccr
	code_tp->dmamovCCR_0_opcode_0 = 0xBC;
	code_tp->dmamovCCR_1_opcode_1 = 0x01;
	//*(unsigned int*)&(code_tp->dmamovCCR_2_ccr_0) = ccr;
	*(unsigned short*)&(code_tp->dmamovCCR_2_ccr_0) = ccr & 0xFFFF;
	*(unsigned short*)&(code_tp->dmamovCCR_4_ccr_2) = (ccr >> 16) & 0xFFFF;

	//sar
	code_tp->dmamovSAR_0_opcpde_0 = 0xBC;
	code_tp->dmamovSAR_1_opcode_1 = 0x00;
	//*(unsigned int*)&(code_tp->dmamovSAR_2_addr_0) = sar;
	*(unsigned short*)&(code_tp->dmamovSAR_2_addr_0) = sar & 0xFFFF;
	*(unsigned short*)&(code_tp->dmamovSAR_4_addr_2) = (sar >> 16) & 0xFFFF;


	//dar
	code_tp->dmamovDAR_0_opcpde_0 = 0xBC;
	code_tp->dmamovDAR_1_opcpde_1 = 0x02;
	//*(unsigned int*)&(code_tp->dmamovDAR_2_addr_0) = dar;
	*(unsigned short*)&(code_tp->dmamovDAR_2_addr_0) = dar & 0xFFFF;
	*(unsigned short*)&(code_tp->dmamovDAR_4_addr_2) = (dar >> 16) & 0xFFFF;


	//lp0
	code_tp->dmaloop0_0_opcode = 0x20;
	code_tp->dmaloop0_1_cnt = loop_cnt_0 - 1;

	//lp1
	code_tp->dmaloop1_0_opcode = 0x22;
	code_tp->dmaloop1_1_cnt = loop_cnt_1 - 1;
	
	//dmald
	code_tp->dmaload_0_opcode = 0x4;

	//dmast
	code_tp->dmastore_0_opcode = 0x8;

	//lpend 1
	code_tp->dmaloopend1_0_opcode = 0x3C;
	code_tp->dmaloopend1_1_bf = 0x2;

	//lpend 0
	code_tp->dmaloopend0_0_opcode = 0x38;
	code_tp->dmaloopend0_1_bf = 0x6;

	//sev
	code_tp->dmasev_0_opcode = 0x34;
	code_tp->dmasev_1_num = sev_num << 3;

	//end
	code_tp->dmaend_0_opcode = 0x0;



	//set inst 0
	set_inst0(0x00A00000 | (cha_num) << 8 | no_secure << 17 | (cha_num) << 24);
	
	//set inst 1
	set_inst1((unsigned int)code_tp);


	//go
	set_dbgcmd();

}




dma_code_template_0 dma_tp[8];
/*
void dma_330_test()
{
	int i = 0, j = 0,temp;
	unsigned char * pSA = (unsigned char *)DMASAR;
	unsigned char  * pDA = (unsigned char *)DMADAR;

	for(i=0; i<0x200; i++)
	{
	  	*pSA++ = i;
		*pDA++ = 0xFF;	
	}


	for(i=0; i<8; i++)
	{
		dma_inten(i);
		dma_go(	i,
			1,
 			&dma_tp[i],
			SRC_INCR | SRC_BURST_LEN_1 | SRC_PROT_CTRL_2 | SRC_BURST_SIZE_1 | \
			DST_INCR | DST_BURST_LEN_1 | DST_PROT_CTRL_2 | DST_BURST_SIZE_1,
			(DMASAR+i*LOOPCNT0*LOOPCNT1),
			(DMADAR+i*LOOPCNT0*LOOPCNT1),
			LOOPCNT0,
			LOOPCNT1,
			i);

		temp = dma_int_poll(i);			
		while(temp == 0)
		{
			temp = dma_int_poll(i);
		
		}
		dma_int_clear(i);
		dma_intdisable(i);

		pDA = (int*)(DMADAR+0x40*i),
		pSA = (int*)0x02000000;
		for(j=0; j<(0x10); j++)
		{
	  		*pSA++ = *pDA++;
		}

	}


	while(1);
}
*/
unsigned int irqNum = 0;
void dma330_handler(unsigned int ack)
{
	dma_int_clear(ack-95);
	*(unsigned int *)(0x90000000 + (ack-94)*4) = ack;


	irqNum++;
	if(irqNum == 9)
	{
		
		*(unsigned int *)(0xFFFFFFFC) = ack;
	}
}

int dma330_cpy(void* dst, void* src, int len)
{
	int i=0, j=0;
	volatile int temp = 0;
	int LOOPCNT1 = 256;	//1024x768x2 bytes
	int LOOPCNT0 = len / 128 / 256;

	for(i=0; i<8; i++)
	{
		if((*(DMA330_CSR0+2*i) & 0xF) != 0)  //status = stoped
			continue;
		else
		{
			dma_inten(i);
			dma_go(	i,
			1,
 			&dma_tp[i],
			SRC_INCR | SRC_BURST_LEN_8 | SRC_PROT_CTRL_2 | SRC_BURST_SIZE_16 | 
			DST_INCR | DST_BURST_LEN_8 | DST_PROT_CTRL_2 | DST_BURST_SIZE_16,
			src,
			dst,
			LOOPCNT0,
			LOOPCNT1,
			i);

			
			while(!((*DMA330_INTSTATUS) & (1 << i)));
			/*
			while(temp == 0)
			{
				temp = dma_int_poll(i);
		
			}*/
			dma_int_clear(i);
			dma_intdisable(i);

			return 1;
		}
	}

	return 0;


}
