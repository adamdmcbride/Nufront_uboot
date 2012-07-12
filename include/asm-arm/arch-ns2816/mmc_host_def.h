/*
 * FILE : mmc_host_def.h
 * Desc : define of mmc controler
 * Author : jinying.wu
 * Data : 
 * Modify :
 * Version :
 * Group : NuSmart
 */

#ifndef MMC_HOST_DEF_H
#define MMC_HOST_DEF_H

#define DWC_SDMMC_BASE_ADDRESS 		(0x05070000)
#define MMC_CLOCK_REFERENCE		(50000000)
#define MAX_DIVIDER_VALUE		(0xfe)
/*
 * Synopsys host controler register definitions
 */
#define DWC_CTRL		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x00))
#define DWC_PWREN		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x04))
#define DWC_CLKDIV		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x08))
#define DWC_CLKSRC		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x0c))
#define DWC_CLKENA		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x10))
#define DWC_TMOUT		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x14))
#define DWC_CTYPE		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x18))
#define DWC_BLKSIZ		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x1c))
#define DWC_BYTCNT		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x20))
#define DWC_INTMASK		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x24))
#define DWC_CMDARG		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x28))
#define DWC_CMD			(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x2c))
#define DWC_RESP0		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x30))
#define DWC_RESP1		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x34))
#define DWC_RESP2		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x38))
#define DWC_RESP3		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x3c))
#define DWC_MINTSTS		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x40))
#define DWC_RINTSTS		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x44))
#define DWC_STATUS		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x48))
#define DWC_FIFOTH		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x4c))
#define DWC_CDETECT		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x50))
#define DWC_WRTPRT		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x54))
#define DWC_GPIO		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x58))
#define DWC_TCBCNT		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x5c))
#define DWC_TBBCNT		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x60))
#define DWC_DEBNCE		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x64))
#define DWC_USRID		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x68))
#define DWC_VERID		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x6c))
#define DWC_HCON		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x70))
#define DWC_UHS_REG		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x74))
#define DWC_BMOD		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x80))
#define DWC_PLDMND		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x84))
#define DWC_DBADDR		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x88))
#define DWC_IDSTS		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x8c))
#define DWC_IDINTEN		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x90))
#define DWC_DSCADDR		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x94))
#define DWC_BUFADDR		(*(volatile unsigned int *) (DWC_SDMMC_BASE_ADDRESS + 0x98))

//define domain of descriptor
#define DESP0_OWN		(0x1 << 31)
#define DESP0_CES		(0x1 << 30)
#define DESP0_CH		(0x1 << 4)
#define DESP0_FIRST_DES		(0x1 << 3)
#define DESP0_LAST_DES		(0x1 << 2)
#define DESP0_DIC		(0x1 << 1)


//response
#define NO_RESPONSE		(DWC_RINTSTS & RINTSTS_RTO_BAR)
//RINTSTS register have error interrupt
#define INTERRUPT_ERRORS	(RINTSTS_EBE | RINTSTS_SBE | RINTSTS_HLE | RINTSTS_FRUN | RINTSTS_DCRC | RINTSTS_RCRC | RINTSTS_RE)

//bus width define
#define BIT_MODE_1 		(0)
#define BIT_MODE_4		(1)
#define BIT_MODE_8		(2)

#define TX_WMARK		(0xFFFFF000)
#define RX_WMARK		(0xF000FFFF)

/*
 * Synopsys host controler register Bit definitions
 */
//define domain of CTRL register
#define CTRL_CONTROLLER_RESET		(0x1 << 0)
#define CTRL_FIFO_RESET			(0x1 << 1)
#define CTRL_DMA_RESET			(0x1 << 2)
#define CTRL_INT_ENABLE			(0x1 << 4)
#define CTRL_ENABLE_OD_PULLUP 		(0x1 << 24)
#define CTRL_USE_INTERNAL_DMAC		(0x1 << 25)

//define domain of CMD register
#define CMD_START_CMD			(0x1 << 31)
#define CMD_VOLT_SWITCH			(0x1 << 28)
#define CMD_BOOT_MODE			(0x1 << 27)
#define CMD_DISABLE_BOOT		(0x1 << 26)
#define CMD_EXPECT_BOOT_ACK		(0x1 << 25)
#define CMD_ENABLE_BOOT			(0x1 << 24)
#define CMD_CCS_EXPECTED		(0x1 << 23)
#define CMD_READ_CEATA_DEVICE		(0x1 << 22)
#define CMD_UPDATE_CLOCK_REGISTERS_ONLY (0x1 << 21)
#define CMD_SEND_INITIALIZATION		(0x1 << 15)
#define CMD_STOP_ABORT_CMD		(0x1 << 14)
#define CMD_WAIT_PRVDATA_COMPLETE	(0x1 << 13)
#define CMD_SEND_AUTO_STOP		(0x1 << 12)
#define CMD_TRANSFER_MODE		(0x1 << 11)
#define CMD_WRITE			(0x1 << 10)
#define CMD_READ			(0xfffffbff)		// and operation
#define CMD_DATA_EXPECTED		(0x1 << 9)
#define CMD_CHECK_RESPONSE_CRC		(0x1 << 8)
#define CMD_RESPONSE_LENGTH		(0x1 << 7)
#define CMD_RESPONSE_EXPECT		(0x1 << 6)

//define domain of BMOD register
#define BMOD_IDMAC			(0x1 << 7)
#define ABORT_READ_DATA			(0x1 << 8)
#define	G_INT_ENABLE			(0x1 << 4)
#define BMOD_FB				(0x1 << 1)
#define BMOD_SWR			(0x1 << 0)

//define domain of RINTSTS register
#define RINTSTS_EBE			(0x1 << 15)
#define RINTSTS_ACD			(0x1 << 14)
#define RINTSTS_SBE			(0x1 << 13)
#define RINTSTS_HLE			(0x1 << 12)
#define RINTSTS_FRUN			(0x1 << 11)
#define RINTSTS_HTO			(0x1 << 10)
#define RINTSTS_DRTO_BDS		(0x1 << 9)
#define RINTSTS_RTO_BAR			(0x1 << 8)
#define RINTSTS_DCRC			(0x1 << 7)
#define RINTSTS_RCRC			(0x1 << 6)
#define RINTSTS_RXDR			(0x1 << 5)
#define RINTSTS_TXDR			(0x1 << 4)
#define RINTSTS_DTO			(0x1 << 3)
#define RINTSTS_CD			(0x1 << 2)
#define RINTSTS_RE			(0x1 << 1)
#define RINTSTS_CARDDETECT		(0x1 << 0)

//define domain of IDSTS register
#define DMA_IDLE			(0x0 << 13)
#define DMA_SUSPEND			(0x1 << 13)
#define DESC_RD				(0x2 << 13)
#define DESC_CHK			(0x3 << 13)
#define DMA_RD_REQ_WAIT			(0x4 << 13)
#define DMA_WR_REQ_WAIT			(0x5 << 13)
#define DMA_RD				(0x6 << 13)
#define DMA_WR				(0x7 << 13)
#define DESC_CLOSE			(0x8 << 13)
#define IDSTS_AIS			(0x1 << 9)
#define IDSTS_NIS			(0x1 << 8)
#define IDSTS_CES			(0x1 << 5)
#define IDSTS_DU			(0x1 << 4)
#define IDSTS_FBE			(0x1 << 2)
#define IDSTS_RI			(0x1 << 1)
#define IDSTS_TI			(0x1 << 0)

//define domain of STATUS register
#define STATUS_DATA_BUSY		(0x1 << 9)

//DEBNCE        
#define DEFAULT_DEBNCE_VAL		0x0FFFFF
//fifo reset
#define	FIFO_RESET			(CTRL_FIFO_RESET | CTRL_INT_ENABLE | CTRL_USE_INTERNAL_DMAC)

#define VS30_3V0SUP		        (1 << 25)
#define VS18_1V8SUP			(1 << 26)

/* Driver definitions */
#define MMCSD_SECTOR_SIZE		(512)

#define MMC_CARD			(0)
#define SD_CARD				(1)
#define ATA_CARD			(2)
#define CEATA_CARD			(3)
#define SDIO_CARD			(4)

#define BYTE_MODE			(0)
#define SECTOR_MODE			(1)
#define CLK_INITSEQ		        (0)
#define CLK_400KHZ		        (1)
#define CLK_MISC			(2)

typedef struct {
	unsigned int card_type;
	unsigned int version;
	unsigned int mode;
	unsigned int size;
	unsigned int RCA;
} sdmmc_card_data;

#endif /* MMC_HOST_DEF_H */
