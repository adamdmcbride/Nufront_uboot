#ifndef __DMA_DRIVER_H
#define __DMA_DRIVER_H

//defines

#define DMA330_BASE_ADDR 0x05020000
#define DMA330_INTEN 	 (unsigned int*)(DMA330_BASE_ADDR + 0x20)
#define DMA330_INTSTATUS (unsigned int*)(DMA330_BASE_ADDR + 0x28)
#define DMA330_INTCLEAR (unsigned int*)(DMA330_BASE_ADDR + 0x2C)
#define DMA330_DBGSTATUS (unsigned int*)(DMA330_BASE_ADDR + 0xD00)
#define DMA330_DBGCMD    (unsigned int*)(DMA330_BASE_ADDR + 0xD04)
#define DMA330_DBGINST0  (unsigned int*)(DMA330_BASE_ADDR + 0xD08)
#define DMA330_DBGINST1  (unsigned int*)(DMA330_BASE_ADDR + 0xD0C)

#define DMA330_FSRD	(unsigned int*)(DMA330_BASE_ADDR + 0x30)
#define DMA330_FSRC	(unsigned int*)(DMA330_BASE_ADDR + 0x34)
#define DMA330_FTRD	(unsigned int*)(DMA330_BASE_ADDR + 0x38)

#define DMA330_FTR0	(unsigned int*)(DMA330_BASE_ADDR + 0x40)

#define DMA330_CSR0	(unsigned int*)(DMA330_BASE_ADDR + 0x100)
#define DMA330_CSR1	(unsigned int*)(DMA330_BASE_ADDR + 0x108)
#define DMA330_CSR2	(unsigned int*)(DMA330_BASE_ADDR + 0x110)
#define DMA330_CSR3	(unsigned int*)(DMA330_BASE_ADDR + 0x118)
#define DMA330_CSR4	(unsigned int*)(DMA330_BASE_ADDR + 0x120)
#define DMA330_CSR5	(unsigned int*)(DMA330_BASE_ADDR + 0x128)
#define DMA330_CSR6	(unsigned int*)(DMA330_BASE_ADDR + 0x130)
#define DMA330_CSR7	(unsigned int*)(DMA330_BASE_ADDR + 0x138)

#define DMA330_CPC0	(unsigned int*)(DMA330_BASE_ADDR + 0x104)
#define DMA330_SAR0	(unsigned int*)(DMA330_BASE_ADDR + 0x400)
#define DMA330_DAR0	(unsigned int*)(DMA330_BASE_ADDR + 0x404)

#define SRC_FIXED (0x0)
#define SRC_INCR  (0x1)
#define DST_FIXED (0x0 << 14)
#define DST_INCR   (0x1 << 14)

#define SRC_BURST_SIZE_1 (0x0 << 1)
#define SRC_BURST_SIZE_2 (0x1 << 1)
#define SRC_BURST_SIZE_4 (0x2 << 1)
#define SRC_BURST_SIZE_8 (0x3 << 1)
#define SRC_BURST_SIZE_16 (0x4 << 1)

#define DST_BURST_SIZE_1 (0x0 << 15)
#define DST_BURST_SIZE_2 (0x1 << 15)
#define DST_BURST_SIZE_4 (0x2 << 15)
#define DST_BURST_SIZE_8 (0x3 << 15)
#define DST_BURST_SIZE_16 (0x4 << 15)

#define SRC_BURST_LEN_1 (0x0 << 4)
#define SRC_BURST_LEN_2 (0x1 << 4)
#define SRC_BURST_LEN_3 (0x2 << 4)
#define SRC_BURST_LEN_4 (0x3 << 4)
#define SRC_BURST_LEN_5 (0x4 << 4)
#define SRC_BURST_LEN_6 (0x5 << 4)
#define SRC_BURST_LEN_7 (0x6 << 4)
#define SRC_BURST_LEN_8 (0x7 << 4)
#define SRC_BURST_LEN_9 (0x8 << 4)
#define SRC_BURST_LEN_10 (0x9 << 4)
#define SRC_BURST_LEN_11 (0xA << 4)
#define SRC_BURST_LEN_12 (0xB << 4)
#define SRC_BURST_LEN_13 (0xC << 4)
#define SRC_BURST_LEN_14 (0xD << 4)
#define SRC_BURST_LEN_15 (0xE << 4)
#define SRC_BURST_LEN_16 (0xF << 4)

#define DST_BURST_LEN_1 (0x0 << 18)
#define DST_BURST_LEN_2 (0x1 << 18)
#define DST_BURST_LEN_3 (0x2 << 18)
#define DST_BURST_LEN_4 (0x3 << 18)
#define DST_BURST_LEN_5 (0x4 << 18)
#define DST_BURST_LEN_6 (0x5 << 18)
#define DST_BURST_LEN_7 (0x6 << 18)
#define DST_BURST_LEN_8 (0x7 << 18)
#define DST_BURST_LEN_9 (0x8 << 18)
#define DST_BURST_LEN_10 (0x9 << 18)
#define DST_BURST_LEN_11 (0xA << 18)
#define DST_BURST_LEN_12 (0xB << 18)
#define DST_BURST_LEN_13 (0xC << 18)
#define DST_BURST_LEN_14 (0xD << 18)
#define DST_BURST_LEN_15 (0xE << 18)
#define DST_BURST_LEN_16 (0xF << 18)

#define SRC_PROT_CTRL_0 (0x0 << 8)
#define SRC_PROT_CTRL_1 (0x1 << 8)
#define SRC_PROT_CTRL_2 (0x2 << 8)
#define SRC_PROT_CTRL_3 (0x3 << 8)
#define SRC_PROT_CTRL_4 (0x4 << 8)
#define SRC_PROT_CTRL_5 (0x5 << 8)
#define SRC_PROT_CTRL_6 (0x6 << 8)
#define SRC_PROT_CTRL_7 (0x7 << 8)

#define DST_PROT_CTRL_0 (0x0 << 22)
#define DST_PROT_CTRL_1 (0x1 << 22)
#define DST_PROT_CTRL_2 (0x2 << 22)
#define DST_PROT_CTRL_3 (0x3 << 22)
#define DST_PROT_CTRL_4 (0x4 << 22)
#define DST_PROT_CTRL_5 (0x5 << 22)
#define DST_PROT_CTRL_6 (0x6 << 22)
#define DST_PROT_CTRL_7 (0x7 << 22)

#define SRC_CACHE_CTRL_0 (0x0 << 11)
#define SRC_CACHE_CTRL_1 (0x1 << 11)
#define SRC_CACHE_CTRL_2 (0x2 << 11)
#define SRC_CACHE_CTRL_3 (0x3 << 11)
#define SRC_CACHE_CTRL_4 (0x4 << 11)
#define SRC_CACHE_CTRL_5 (0x5 << 11)
#define SRC_CACHE_CTRL_6 (0x6 << 11)
#define SRC_CACHE_CTRL_7 (0x7 << 11)

#define DST_CACHE_CTRL_0 (0x0 << 25)
#define DST_CACHE_CTRL_1 (0x1 << 25)
#define DST_CACHE_CTRL_2 (0x2 << 25)
#define DST_CACHE_CTRL_3 (0x3 << 25)
#define DST_CACHE_CTRL_4 (0x4 << 25)
#define DST_CACHE_CTRL_5 (0x5 << 25)
#define DST_CACHE_CTRL_6 (0x6 << 25)
#define DST_CACHE_CTRL_7 (0x7 << 25)

#define ENDIAN_SWAP_SIZE_0  (0x0 << 28)
#define ENDIAN_SWAP_SIZE_16 (0x1 << 28)
#define ENDIAN_SWAP_SIZE_32 (0x2 << 28)
#define ENDIAN_SWAP_SIZE_64 (0x3 << 28)
#define ENDIAN_SWAP_SIZE_128 (0x4 << 28)



typedef struct {
	//DMAMOV CCR
	unsigned char dmamovCCR_0_opcode_0;
	unsigned char dmamovCCR_1_opcode_1;
	unsigned char dmamovCCR_2_ccr_0;
	unsigned char dmamovCCR_3_ccr_1;
	unsigned char dmamovCCR_4_ccr_2;
	unsigned char dmamovCCR_5_ccr_3;
	
	//DMAMOV SAR
	unsigned char dmamovSAR_0_opcpde_0;
	unsigned char dmamovSAR_1_opcode_1;
	unsigned char dmamovSAR_2_addr_0;
	unsigned char dmamovSAR_3_addr_1;
	unsigned char dmamovSAR_4_addr_2;
	unsigned char dmamovSAR_5_addr_3;

	//DMAMOV DAR
	unsigned char dmamovDAR_0_opcpde_0;
	unsigned char dmamovDAR_1_opcpde_1;
	unsigned char dmamovDAR_2_addr_0;
	unsigned char dmamovDAR_3_addr_1;
	unsigned char dmamovDAR_4_addr_2;
	unsigned char dmamovDAR_5_addr_3;

	//DMALP 0
	unsigned char dmaloop0_0_opcode;
	unsigned char dmaloop0_1_cnt;

	//DMALP 1
	unsigned char dmaloop1_0_opcode;
	unsigned char dmaloop1_1_cnt;

	//DMALD
	unsigned char dmaload_0_opcode;

	//DMAST
	unsigned char dmastore_0_opcode;

	//DMALPEND 1
	unsigned char dmaloopend1_0_opcode;
	unsigned char dmaloopend1_1_bf;

	//DMALPEND 0
	unsigned char dmaloopend0_0_opcode;
	unsigned char dmaloopend0_1_bf;

	//DMA SEV
	unsigned char dmasev_0_opcode;
	unsigned char dmasev_1_num;

	//DMAEND
	unsigned char dmaend_0_opcode;

	int align_dummy;

}dma_code_template_0;

//functions
unsigned int DMA330_readReg(unsigned int * addr);
void dma_inten(unsigned int intnum);
int dma_int_poll(unsigned int intnum);
void dma_int_clear(unsigned int intnum);
void dma_go(	unsigned int cha_num,
		unsigned int no_secure,
 		dma_code_template_0 * code_tp,
		unsigned int ccr,
		unsigned int sar,
		unsigned int dar,
		unsigned char loop_cnt_0,
		unsigned char loop_cnt_1,
		unsigned char sev_num				   
				   );


int dma330_cpy(void* dst, void* src, int len);
#endif


