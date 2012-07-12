/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 * MA 02111-1307 USA
 */

#ifndef MMC_H
#define MMC_H

#include "mmc_host_def.h"

/* All supported commands */
#define CMD				(CMD_START_CMD | CMD_CHECK_RESPONSE_CRC | CMD_RESPONSE_EXPECT)
//CMD0 have no responses
#define SDMMC_CMD0			(0 | CMD_START_CMD ) 
#define SDMMC_CMD1			(1 | CMD) 
#define MMC_CMD1			((1 | CMD) & ~CMD_CHECK_RESPONSE_CRC) 
//CMD2 have long responses
#define SDMMC_CMD2			(2 | CMD | CMD_RESPONSE_LENGTH) 
#define SDMMC_CMD3			(3 | CMD) 
//CMD4 have no responses
#define SDMMC_CMD4			(4 | CMD_START_CMD | CMD_WAIT_PRVDATA_COMPLETE) 
//#define SDMMC_CMD5			(5 | CMD | CMD_SEND_INITIALIZATION | CMD_WAIT_PRVDATA_COMPLETE) 
#define SDMMC_CMD5			((5 | CMD | CMD_WAIT_PRVDATA_COMPLETE) & ~CMD_CHECK_RESPONSE_CRC) 
#define SDMMC_CMD6			(6 | CMD) 
#define SDMMC_CMD6_DATA_EXPECTED	(6 | CMD | CMD_DATA_EXPECTED)
#define SDMMC_CMD7			(7 | CMD)
#define SDMMC_CMD8			(8 | CMD) 
#define SDMMC_CMD9			(9 | CMD | CMD_RESPONSE_LENGTH) 
#define SDMMC_CMD11			(11 | CMD) 
//#define SDMMC_CMD13			(13 | CMD_START_CMD ) 
#define SDMMC_CMD13			(13 | CMD ) 
#define SDMMC_CMD16			(16 | CMD_START_CMD | CMD_RESPONSE_EXPECT) 
//single block read
#define SDMMC_CMD17			(17 | CMD | CMD_DATA_EXPECTED)
//multiple block read 
#define SDMMC_CMD18			(18 | CMD | CMD_DATA_EXPECTED) 
//single block write
#define SDMMC_CMD24			(24 | CMD | CMD_DATA_EXPECTED | CMD_WRITE) 
//
#define SDMMC_CMD39			(39 | CMD)
#define SDMMC_ACMD41			((41 | CMD) & ~CMD_CHECK_RESPONSE_CRC)
#define SDMMC_CMD55			(55 | CMD)

//cmd status
#define SEND_CMD_OK			(0)
#define RESPONSE_TIMEOUT		(1)

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

typedef struct {
	unsigned not_used:1;
	unsigned crc:7;
	unsigned ecc:2;
	unsigned file_format:2;
	unsigned tmp_write_protect:1;
	unsigned perm_write_protect:1;
	unsigned copy:1;
	unsigned file_format_grp:1;
	unsigned content_prot_app:1;
	unsigned reserved_1:4;
	unsigned write_bl_partial:1;
	unsigned write_bl_len:4;
	unsigned r2w_factor:3;
	unsigned default_ecc:2;
	unsigned wp_grp_enable:1;
	unsigned wp_grp_size:5;
	unsigned erase_grp_mult:5;
	unsigned erase_grp_size:5;
	unsigned c_size_mult:3;
	unsigned vdd_w_curr_max:3;
	unsigned vdd_w_curr_min:3;
	unsigned vdd_r_curr_max:3;
	unsigned vdd_r_curr_min:3;
	unsigned c_size_lsb:2;
	unsigned c_size_msb:10;
	unsigned reserved_2:2;
	unsigned dsr_imp:1;
	unsigned read_blk_misalign:1;
	unsigned write_blk_misalign:1;
	unsigned read_bl_partial:1;
	unsigned read_bl_len:4;
	unsigned ccc:12;
	unsigned tran_speed:8;
	unsigned nsac:8;
	unsigned taac:8;
	unsigned reserved_3:2;
	unsigned spec_vers:4;
	unsigned csd_structure:2;
} mmc_csd_reg_t;

/* csd for sd2.0 */
typedef struct {
	unsigned not_used:1;
	unsigned crc:7;
	unsigned reserved_1:2;
	unsigned file_format:2;
	unsigned tmp_write_protect:1;
	unsigned perm_write_protect:1;
	unsigned copy:1;
	unsigned file_format_grp:1;
	unsigned reserved_2:5;
	unsigned write_bl_partial:1;
	unsigned write_bl_len:4;
	unsigned r2w_factor:3;
	unsigned reserved_3:2;
	unsigned wp_grp_enable:1;
	unsigned wp_grp_size:7;
	unsigned sector_size:7;
	unsigned erase_blk_len:1;
	unsigned reserved_4:1;
	unsigned c_size_lsb:16;
	unsigned c_size_msb:6;
	unsigned reserved_5:6;
	unsigned dsr_imp:1;
	unsigned read_blk_misalign:1;
	unsigned write_blk_misalign:1;
	unsigned read_bl_partial:1;
	unsigned read_bl_len:4;
	unsigned ccc:12;
	unsigned tran_speed:8;
	unsigned nsac:8;
	unsigned taac:8;
	unsigned reserved_6:6;
	unsigned csd_structure:2;
} mmc_sd2_csd_reg_t;

/* extended csd - 512 bytes long */
typedef struct {
	unsigned char reserved_1[181];
	unsigned char erasedmemorycontent;
	unsigned char reserved_2;
	unsigned char buswidthmode;
	unsigned char reserved_3;
	unsigned char highspeedinterfacetiming;
	unsigned char reserved_4;
	unsigned char powerclass;
	unsigned char reserved_5;
	unsigned char commandsetrevision;
	unsigned char reserved_6;
	unsigned char commandset;
	unsigned char extendedcsdrevision;
	unsigned char reserved_7;
	unsigned char csdstructureversion;
	unsigned char reserved_8;
	unsigned char cardtype;
	unsigned char reserved_9[3];
	unsigned char powerclass_52mhz_1_95v;
	unsigned char powerclass_26mhz_1_95v;
	unsigned char powerclass_52mhz_3_6v;
	unsigned char powerclass_26mhz_3_6v;
	unsigned char reserved_10;
	unsigned char minreadperf_4b_26mhz;
	unsigned char minwriteperf_4b_26mhz;
	unsigned char minreadperf_8b_26mhz_4b_52mhz;
	unsigned char minwriteperf_8b_26mhz_4b_52mhz;
	unsigned char minreadperf_8b_52mhz;
	unsigned char minwriteperf_8b_52mhz;
	unsigned char reserved_11;
	unsigned int sectorcount;
	unsigned char reserved_12[288];
	unsigned char supportedcommandsets;
	unsigned char reserved_13[7];
} mmc_extended_csd_reg_t;

typedef struct __descriptor__ {
	unsigned int des0;
	unsigned int des1;
	unsigned int *buffer;
	struct __descriptor__ *next;
}sdmmc_descriptor;

/* mmc sd responce */
typedef struct {
	unsigned int ocr;
} mmc_resp_r3;

typedef struct {
	unsigned short cardstatus;
	unsigned short newpublishedrca;
} mmc_resp_r6;


typedef enum {
        CARD_STATE_EMPTY = -1,
        CARD_STATE_IDLE = 0,
        CARD_STATE_READY = 1,
        CARD_STATE_IDENT = 2,
        CARD_STATE_STBY = 3,
        CARD_STATE_TRAN = 4,
        CARD_STATE_DATA = 5,
        CARD_STATE_RCV = 6,
        CARD_STATE_PRG = 7,
        CARD_STATE_DIS = 8,
        CARD_STATE_INA = 9
} Card_state;

extern sdmmc_card_data mmc_dev;

unsigned int sdmmc_send_command(unsigned int cmd, unsigned int arg,
			       unsigned int *response);
#endif /* MMC_H */
