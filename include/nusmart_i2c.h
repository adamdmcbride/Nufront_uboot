#ifndef _I2C_DEVICE_H
#define _I2C_DEVICE_H



/*
 * Registers offset
 */
#define DW_IC_CON           0x0
#define DW_IC_TAR           0x4
#define DW_IC_DATA_CMD      0x10
#define DW_IC_SS_SCL_HCNT   0x14
#define DW_IC_SS_SCL_LCNT   0x18
#define DW_IC_FS_SCL_HCNT   0x1c
#define DW_IC_FS_SCL_LCNT   0x20
#define DW_IC_INTR_STAT     0x2c
#define DW_IC_INTR_MASK     0x30
#define DW_IC_RAW_INTR_STAT 0x34
#define DW_IC_RX_TL         0x38
#define DW_IC_TX_TL         0x3c
#define DW_IC_CLR_INTR      0x40
#define DW_IC_CLR_RX_UNDER  0x44
#define DW_IC_CLR_RX_OVER   0x48
#define DW_IC_CLR_TX_OVER   0x4c
#define DW_IC_CLR_RD_REQ    0x50
#define DW_IC_CLR_TX_ABRT   0x54
#define DW_IC_CLR_RX_DONE   0x58
#define DW_IC_CLR_ACTIVITY  0x5c
#define DW_IC_CLR_STOP_DET  0x60
#define DW_IC_CLR_START_DET 0x64
#define DW_IC_CLR_GEN_CALL  0x68
#define DW_IC_ENABLE        0x6c
#define DW_IC_STATUS        0x70
#define DW_IC_TXFLR         0x74
#define DW_IC_RXFLR         0x78
#define DW_IC_COMP_PARAM_1  0xf4
#define DW_IC_TX_ABRT_SOURCE    0x80

#define DW_IC_CON_MASTER        0x1
#define DW_IC_CON_SPEED_STD     0x2
#define DW_IC_CON_SPEED_FAST        0x4
#define DW_IC_CON_10BITADDR_MASTER  0x10
#define DW_IC_CON_RESTART_EN        0x20
#define DW_IC_CON_SLAVE_DISABLE     0x40


#define DW_IC_INTR_RX_UNDER     0x001
#define DW_IC_INTR_RX_OVER      0x002
#define DW_IC_INTR_RX_FULL      0x004
#define DW_IC_INTR_TX_OVER      0x008
#define DW_IC_INTR_TX_EMPTY     0x010
#define DW_IC_INTR_RD_REQ       0x020
#define DW_IC_INTR_TX_ABRT      0x040
#define DW_IC_INTR_RX_DONE      0x080
#define DW_IC_INTR_ACTIVITY     0x100
#define DW_IC_INTR_STOP_DET     0x200
#define DW_IC_INTR_START_DET    0x400
#define DW_IC_INTR_GEN_CALL     0x800

#define DW_IC_INTR_DEFAULT_MASK     (DW_IC_INTR_RX_FULL | \
                     DW_IC_INTR_TX_EMPTY | \
                     DW_IC_INTR_TX_ABRT | \
                     DW_IC_INTR_STOP_DET)

#define DW_IC_STATUS_ACTIVITY   0x1

#define DW_IC_ERR_TX_ABRT   0x1

/*
 * status codes
 */
#define STATUS_IDLE         0x0
#define STATUS_WRITE_IN_PROGRESS    0x1
#define STATUS_READ_IN_PROGRESS     0x2

#define TIMEOUT         20 /* ms */

/*
 * hardware abort codes from the DW_IC_TX_ABRT_SOURCE register
 *
 * only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK  0
#define ABRT_10ADDR1_NOACK  1
#define ABRT_10ADDR2_NOACK  2
#define ABRT_TXDATA_NOACK   3
#define ABRT_GCALL_NOACK    4
#define ABRT_GCALL_READ     5
#define ABRT_SBYTE_ACKDET   7
#define ABRT_SBYTE_NORSTRT  9
#define ABRT_10B_RD_NORSTRT 10
#define ABRT_MASTER_DIS     11
#define ARB_LOST        12

#define DW_IC_TX_ABRT_7B_ADDR_NOACK (1UL << ABRT_7B_ADDR_NOACK)
#define DW_IC_TX_ABRT_10ADDR1_NOACK (1UL << ABRT_10ADDR1_NOACK)
#define DW_IC_TX_ABRT_10ADDR2_NOACK (1UL << ABRT_10ADDR2_NOACK)
#define DW_IC_TX_ABRT_TXDATA_NOACK  (1UL << ABRT_TXDATA_NOACK)
#define DW_IC_TX_ABRT_GCALL_NOACK   (1UL << ABRT_GCALL_NOACK)
#define DW_IC_TX_ABRT_GCALL_READ    (1UL << ABRT_GCALL_READ)
#define DW_IC_TX_ABRT_SBYTE_ACKDET  (1UL << ABRT_SBYTE_ACKDET)
#define DW_IC_TX_ABRT_SBYTE_NORSTRT (1UL << ABRT_SBYTE_NORSTRT)
#define DW_IC_TX_ABRT_10B_RD_NORSTRT    (1UL << ABRT_10B_RD_NORSTRT)
#define DW_IC_TX_ABRT_MASTER_DIS    (1UL << ABRT_MASTER_DIS)
#define DW_IC_TX_ARB_LOST       (1UL << ARB_LOST)

#define DW_IC_TX_ABRT_NOACK     (DW_IC_TX_ABRT_7B_ADDR_NOACK | \
                     DW_IC_TX_ABRT_10ADDR1_NOACK | \
                     DW_IC_TX_ABRT_10ADDR2_NOACK | \
                     DW_IC_TX_ABRT_TXDATA_NOACK | \
                     DW_IC_TX_ABRT_GCALL_NOACK)


/* i2c slave device address */
#define SLV_ADDR_G_SENSOR       0x0F
#define SLV_ADDR_L_SENSOR       (0x90/2)
#define SLV_ADDR_TP_GOODIX      0x55
#define SLV_ADDR_E_SENSOR       0x0E
#define SLV_ADDR_IO373X  		0xc4/2

/* i2c base address */
#define NS115_I2C0_BASE     0x06100000
#define NS115_I2C1_BASE     0x06110000
#define NS115_I2C2_BASE     0x06120000
#define NS115_I2C3_BASE     0x06130000


/* EC register address */
#define IO373X_EVENT_ADDR  0x8422
#define IO373X_IDX_ADDR    0x8420
#define IO373X_DAT_ADDR    0x8421

/* i2c write and read error mark */
#define I2C_WRITE_FAILED		-1
#define I2C_SLAVE_NO_ACK		-2
#define I2C_READ_FAILED			-3
#define I2C_COMPARE_UNMATCH		-4



extern void dw_i2c_master_init(void* i2c_base, unsigned char slv_addr);
extern int  dw_i2c_smbus_read(void* i2c_base, unsigned char * out_buf, unsigned int wlen, unsigned char *rbuf, unsigned rlen);
extern int dw_i2c_send_bytes(void* i2c_base, unsigned char * out_buf, unsigned int len);
extern int io373x_smbus_set_adr(void* i2c0_base, unsigned short reg);
extern int io373x_smbus_read_reg(void* i2c0_base, unsigned char slv_addr, unsigned short reg, unsigned char * rbuf, unsigned char len);
extern int io373x_smbus_write_reg(void* i2c0_base,unsigned char slv_addr, unsigned short reg, unsigned char * buf, unsigned char len);

#endif
