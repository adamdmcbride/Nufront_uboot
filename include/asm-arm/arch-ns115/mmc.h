/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 * MA 02111-1307 USA
 */

#ifndef MMC_H
#define MMC_H

#include "mmc_host_def.h"

/* All supported commands */
#define  __CMD_CRC_CHECKED       (1 << 19)
#define  __CMD_RESPONSE_48BITS   (2 << 16)
#define  __CMD_RESPONSE_136BITS  (1 << 16)
#define  __CMD_DATA_PRESENT      (1 << 21)
#define  __CMD_INDEX_CHECKED     (1 << 20)
#define  __CMD_READ              (1 << 4)
#define  __CMD_WRITE            ~ (1 << 4)
#define  __CMD_BUSY              (3 << 16)



#define __SD_CMD_NO_RESP(cmd)          ((cmd) << 24)
#define __SD_CMD_48BIT_RESP(cmd) (((cmd) << 24) | __CMD_CRC_CHECKED | __CMD_INDEX_CHECKED | __CMD_RESPONSE_48BITS)
#define __SD_CMD_48BIT_RESP_WDATA(cmd)  (((cmd) << 24) | __CMD_CRC_CHECKED | \
	__CMD_INDEX_CHECKED | __CMD_RESPONSE_48BITS | __CMD_DATA_PRESENT & __CMD_WRITE)
#define __SD_CMD_48BIT_RESP_RDATA(cmd)  (((cmd) << 24) | __CMD_CRC_CHECKED | \
	__CMD_INDEX_CHECKED | __CMD_RESPONSE_48BITS | __CMD_DATA_PRESENT | __CMD_READ)
//#define __SD_CMD_136BIT_RESP(cmd)      (((cmd) << 24) | __CMD_CRC_CHECKED | __CMD_INDEX_CHECKED | __CMD_RESPONSE_136BITS)
#define __SD_CMD_136BIT_RESP(cmd)      (((cmd) << 24) | __CMD_CRC_CHECKED | __CMD_RESPONSE_136BITS)


//CMD0 have no responses
#define SDMMC_CMD0			__SD_CMD_NO_RESP(0)
#define SDMMC_CMD1			0x01020000
#define SDMMC_CMD2			__SD_CMD_136BIT_RESP(2)
#define SDMMC_CMD3			__SD_CMD_48BIT_RESP(3)
#define SDMMC_CMD4		        __SD_CMD_NO_RESP(4)	
#define SDMMC_CMD5			0x05020000
#define SDMMC_CMD7_DES		__SD_CMD_NO_RESP(7)
#define SDMMC_CMD7_SEL		((7 << 24) | __CMD_CRC_CHECKED | __CMD_INDEX_CHECKED | (3 << 16))
#define SDMMC_CMD8			__SD_CMD_48BIT_RESP(8)
#define SDMMC_CMD9		        __SD_CMD_136BIT_RESP(9)
#define SDMMC_CMD12			__SD_CMD_48BIT_RESP(12) | __CMD_BUSY
#define SDMMC_CMD13			__SD_CMD_48BIT_RESP(13)
#define SDMMC_CMD16			__SD_CMD_48BIT_RESP(16)
//single block read
#define SDMMC_CMD17			__SD_CMD_48BIT_RESP_RDATA(17)
#define SDMMC_CMD18			__SD_CMD_48BIT_RESP_RDATA(18)
#define SDMMC_CMD23			__SD_CMD_48BIT_RESP_RDATA(23)
#define SDMMC_CMD24			__SD_CMD_48BIT_RESP_WDATA(24)
#define SDMMC_CMD25			__SD_CMD_48BIT_RESP_WDATA(25)
#define SDMMC_ACMD41	    0x29020000

#define SDMMC_ACMD51  	    0x333A0010
#define SDMMC_CMD55  	    __SD_CMD_48BIT_RESP(55)

/*---------------------------------
//cmd status
#define E_OK               0
#define E_TUNING           26
#define E_ADMA             25
#define E_AUTO_CMD         24
#define E_CURRENT_LIMIT    23
#define E_DATA_END_BIT     22
#define E_DATA_CRC         21
#define E_DATA_TIMEOUT     20
#define E_COMMAND_INDEX    19
#define E_COMMAND_END_BIT  18
#define E_COMMAND_CRC      17
#define E_COMMAND_TIMEOUT  16
-----------------------------------*/

#define MMC_AC_CMD_RCA_MASK     	(unsigned int)(0xFFFF << 16)
#define MMC_BC_CMD_DSR_MASK     	(unsigned int)(0xFFFF << 16)
#define MMC_DSR_DEFAULT         	(0x0404)
#define SD_CMD8_CHECK_PATTERN		(0xAA)
#define SD_CMD8_2_7_3_6_V_RANGE		(0x01 << 8)

/* Clock Configurations and Macros */

//#define MMC_RELATIVE_CARD_ADDRESS	(0x1234)
#define MMC_RELATIVE_CARD_ADDRESS	(0x1)

#define R1CS_CURRENT_STATE_MASK		0x00001e00
#define R1CS_CURRENT_STATE(x)		(((x)&R1CS_CURRENT_STATE_MASK)>>9)

#define MMC_OCR_REG_ACCESS_MODE_MASK			(0x3 << 29)
#define MMC_OCR_REG_ACCESS_MODE_BYTE    		(0x0 << 29)
#define MMC_OCR_REG_ACCESS_MODE_SECTOR  		(0x2 << 29)

#define MMC_OCR_REG_HOST_CAPACITY_SUPPORT_MASK		(0x1 << 30)
#define MMC_OCR_REG_HOST_CAPACITY_SUPPORT_BYTE		(0x0 << 30)
#define MMC_OCR_REG_HOST_CAPACITY_SUPPORT_SECTOR	(0x1 << 30)
#define MMC_OCR_REG_VOLTAGE_SWITCH			(0x1 << 24)

#define MMC_SD2_CSD_C_SIZE_LSB_MASK         (0xFFFF)
#define MMC_SD2_CSD_C_SIZE_MSB_MASK         (0x003F)
#define MMC_SD2_CSD_C_SIZE_MSB_OFFSET       (16)
#define MMC_CSD_C_SIZE_LSB_MASK             (0x0003)
#define MMC_CSD_C_SIZE_MSB_MASK             (0x03FF)
#define MMC_CSD_C_SIZE_MSB_OFFSET           (2)

#define MMC_CSD_TRAN_SPEED_UNIT_MASK        (0x07 << 0)
#define MMC_CSD_TRAN_SPEED_FACTOR_MASK      (0x0F << 3)
#define MMC_CSD_TRAN_SPEED_UNIT_100MHZ      (0x3 << 0)
#define MMC_CSD_TRAN_SPEED_FACTOR_1_0       (0x01 << 3)
#define MMC_CSD_TRAN_SPEED_FACTOR_8_0       (0x0F << 3)

#define SD2_0_CHECK_PATTERN			(0xAA)
#define SD2_0_VHS				(0x1 << 8)
int mmc_read(int mmc_cont, unsigned int src, unsigned char *dst, int size);
int mmc_write(int mmc_cont, unsigned int *src, unsigned long dst, int size);
int mmc_init(int slot);
#define SRS12_TUNING_ERROR          (0x1uL << 26)
/// ADMA error
#define SRS12_ADMA_ERROR            (0x1uL << 25)
/// Auto CMD (CMD12 or CMD23) error
#define SRS12_AUTO_CMD_ERROR        (0x1uL << 24)
/// Current limit error host controller is not supplying power to SD card due some failure.
#define SRS12_CURRENT_LIMIT_ERROR   (0x1uL << 23)
/// Data end bit error
#define SRS12_DATA_END_BIT_ERROR    (0x1uL << 22)
/// Data CRC error
#define SRS12_DATA_CRC_ERROR        (0x1uL << 21)
/// Data timeout error
#define SRS12_DATA_TIMEOUT_ERROR    (0x1uL << 20)
/// Command index error. Index error occurs in the command response.
#define SRS12_COMMAND_INDEX_ERROR   (0x1uL << 19)
/// Command end bit error
#define SRS12_COMMAND_END_BIT_ERROR (0x1uL << 18)
/// Command CRC error
#define SRS12_COMMAND_CRC_ERROR     (0x1uL << 17)
/// Command timeout error
#define SRS12_COMMAND_TIMEOUT_ERROR (0x1uL << 16)
/// Error interrupt
#define SRS12_ERROR_INTERRUPT       (0x1uL << 15)
/// Re-Tuning Event 
#define SRS12_RETUNING_EVENT        (0x1uL << 12)
/// Interrupt on line C
#define SRS12_INTERRUPT_ON_LINE_C   (0x1uL << 11)
/// Interrupt on line B
#define SRS12_INTERRUPT_ON_LINE_B   (0x1uL << 10)
/// Interrupt on line A 
#define SRS12_INTERRUPT_ON_LINE_A   (0x1uL << 9)
// Card interrupt
#define SRS12_CARD_INTERRUPT        (0x1uL << 8)
/// Card removal
#define SRS12_CARD_REMOVAL          (0x1uL << 7)
/// Card insertion
#define SRS12_CARD_INSERTION        (0x1uL << 6)
/// Buffer read ready. Host is ready to read the buffer.
#define SRS12_BUFFER_READ_READY     (0x1uL << 5)
/// Buffer write ready. Host is ready for writing data to the buffer.
#define SRS12_BUFFER_WRITE_READY    (0x1uL << 4)
/// DMA interrupt
#define SRS12_DMA_INTERRUPT         (0x1uL << 3)
/// Block gap event
#define SRS12_BLOCK_GAP_EVENT       (0x1uL << 2)
/// Transfer complete
#define SRS12_TRANSFER_COMPLETE     (0x1uL << 1)
/// Command complete
#define SRS12_COMMAND_COMPLETE      (0x1uL << 0)




#endif /* MMC_H */
