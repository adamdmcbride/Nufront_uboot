#ifndef _SPI_INCLUDE_
#define _SPI_INCLUDE_

//MASTER CONFIG

#define SPI_0_BASE_ADDR (0x05110000)
#define SPI_0_CTRLR0	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x00))
#define SPI_0_CTRLR1	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x04))
#define SPI_0_SSIENR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x08))
#define SPI_0_MWCR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x0C))

#define SPI_0_SER		((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x10))
#define SPI_0_BAUDR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x14))
#define SPI_0_TXFTLR  ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x18))
#define SPI_0_RXFTLR  ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x1C))

#define SPI_0_TXFLR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x20))
#define SPI_0_RXFLR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x24))
#define SPI_0_SR		((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x28))
#define SPI_0_IMR		((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x2C))

#define SPI_0_ISR		((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x30))
#define SPI_0_RISR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x34))
#define SPI_0_TXOICR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x38))
#define SPI_0_RXOICR  ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x3C))

#define SPI_0_RXUICR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x40))
#define SPI_0_MSTICR  ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x44))
#define SPI_0_ICR		((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x48))
#define SPI_0_DMACR	((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x4C))

#define SPI_0_DMATDLR ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x50))
#define SPI_0_DMARDLR ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x54))
#define SPI_0_IDR		((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x58))
#define SPI_0_SSI_COMP_VERSION ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x5C))

#define SPI_0_DR ((volatile unsigned int*)(SPI_0_BASE_ADDR + 0x60))  //0X60 - 0X9C 

//SLAVE CONFIG


#define SPI_1_CTRLR0	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x00))
#define SPI_1_CTRLR1	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x04))
#define SPI_1_SSIENR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x08))
#define SPI_1_MWCR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x0C))

#define SPI_1_SER		((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x10))
#define SPI_1_BAUDR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x14))
#define SPI_1_TXFTLR  ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x18))
#define SPI_1_RXFTLR  ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x1C))

#define SPI_1_TXFLR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x20))
#define SPI_1_RXFLR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x24))
#define SPI_1_SR		((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x28))
#define SPI_1_IMR		((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x2C))

#define SPI_1_ISR		((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x30))
#define SPI_1_RISR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x34))
#define SPI_1_TXOICR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x38))
#define SPI_1_RXOICR  ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x3C))

#define SPI_1_RXUICR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x40))
#define SPI_1_MSTICR  ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x44))
#define SPI_1_ICR		((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x48))
#define SPI_1_DMACR	((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x4C))

#define SPI_1_DMATDLR ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x50))
#define SPI_1_DMARDLR ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x54))
#define SPI_1_IDR		((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x58))
#define SPI_1_SSI_COMP_VERSION ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x5C))

#define SPI_1_DR ((volatile unsigned int*)(SPI_1_BASE_ADDR + 0x60)) //0X60 - 0X9C 


//parameter
#define SSI_0_IS_MASTER (1)
#define SSI_1_IS_MASTER (0)
#define SSI_RX_FIFO_DEPTH (8)
#define SSI_RX_ABW (3)
#define SSI_TX_FIFO_DEPTH (0x20)
#define SSI_TX_ABW (5)
#define SSI_0_NUM_SLAVES (2)
#define SSI_1_NUM_SLAVES (1)
#define SSI_0_ID (0xFFFFFFFF)
#define SSI_1_ID (0xFFFFFFFE)
#define SSI_HAS_DMA (1)
#define SSI_INTR_IO (1)
#define SSI_INTR_POL (1)
#define SSI_SYNC_CLK (1)
#define SSI_CLK_EN_MODE (0)
#define SSI_HC_FRF (0)
#define SSI_DFLT_FRF (0)
#define SSI_DFLT_SCPOL (0)
#define SSI_DFLT_SCPH (0)

//func

void spi_0_init(void);
void spi_0_sendChar(char data);
int SPI_FLASH(unsigned int addr, unsigned int  size);
unsigned char SPI_FLASH_read_byte(unsigned int addr);


unsigned char SPI_FLASH_POLL_BUSY(void);
#endif
