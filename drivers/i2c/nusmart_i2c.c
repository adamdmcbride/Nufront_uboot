#include <common.h>
#include <command.h>
#include <nusmart_i2c.h>

static u32 i2c_dw_read_clear_intrbits(void* i2c_base)
{
	u32 stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = readl(IC_RAW_INTR_STAT) & readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = readl(i2c_base + DW_IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * readl(IC_INTR_STAT) to readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & DW_IC_INTR_RX_UNDER)
		readl(i2c_base + DW_IC_CLR_RX_UNDER);
	if (stat & DW_IC_INTR_RX_OVER)
		readl(i2c_base + DW_IC_CLR_RX_OVER);
	if (stat & DW_IC_INTR_TX_OVER)
		readl(i2c_base + DW_IC_CLR_TX_OVER);
	if (stat & DW_IC_INTR_RD_REQ)
		readl(i2c_base + DW_IC_CLR_RD_REQ);
	if (stat & DW_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		readl(i2c_base + DW_IC_CLR_TX_ABRT);
	}
	if (stat & DW_IC_INTR_RX_DONE)
		readl(i2c_base + DW_IC_CLR_RX_DONE);
	if (stat & DW_IC_INTR_ACTIVITY)
		readl(i2c_base + DW_IC_CLR_ACTIVITY);
	if (stat & DW_IC_INTR_STOP_DET)
		readl(i2c_base + DW_IC_CLR_STOP_DET);
	if (stat & DW_IC_INTR_START_DET)
		readl(i2c_base + DW_IC_CLR_START_DET);
	if (stat & DW_IC_INTR_GEN_CALL)
		readl(i2c_base + DW_IC_CLR_GEN_CALL);

	return stat;
}

static int
i2c_dw_read(void * i2c_base, u8 * buf, u32 len)
{
	int rx_valid, rd_len = 0;

	rx_valid = readl(i2c_base + DW_IC_RXFLR);

	for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
		*buf++ = readl(i2c_base + DW_IC_DATA_CMD);
		rd_len ++;
	}
	return rd_len;
}

static int
i2c_dw_xfer_msg(void * i2c_base, u8 * buf, u32 buf_len, int rd)
{
	u32 intr_mask;
	int tx_limit, rx_limit, wlen = 0;

	intr_mask = DW_IC_INTR_DEFAULT_MASK;
	
	u32 param1 = readl(i2c_base + DW_IC_COMP_PARAM_1), tx_fifo_depth, rx_fifo_depth;

	tx_fifo_depth = ((param1 >> 16) & 0xff) + 1;
	rx_fifo_depth = ((param1 >> 8)  & 0xff) + 1;


	while(1) {

		tx_limit = tx_fifo_depth - readl(i2c_base + DW_IC_TXFLR);
		rx_limit = rx_fifo_depth - readl(i2c_base + DW_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			if (rd > 0) {
				writel(0x100, i2c_base + DW_IC_DATA_CMD);
				rx_limit--;
			} else {
				writel(*buf++, i2c_base + DW_IC_DATA_CMD);
			}	
			tx_limit--; buf_len--;
			wlen ++;
		}
		

		if (buf_len > 0) {
			break;
		} else {
			intr_mask &= ~DW_IC_INTR_TX_EMPTY;
			//writel(intr_mask, i2c_base + DW_IC_INTR_MASK);
			break;
		}
	}

	return wlen;
}


static int dw_i2c_poll(void* i2c_base, unsigned char * out_buf, unsigned int wlen, unsigned char *rbuf, unsigned int rlen)
{
	unsigned int stat, w_offset = 0, r_offset = 0;

	while(1) {
		stat = i2c_dw_read_clear_intrbits(i2c_base);

		if (stat & DW_IC_INTR_TX_ABRT) {
			writel(0, i2c_base + DW_IC_INTR_MASK);
			goto tx_aborted;
		}

		if ((stat & DW_IC_INTR_RX_FULL) && (r_offset < rlen))
			r_offset += i2c_dw_read(i2c_base, rbuf + r_offset, rlen - r_offset);

		if ((stat & DW_IC_INTR_TX_EMPTY) && (w_offset < wlen))
			w_offset += i2c_dw_xfer_msg(i2c_base, out_buf + w_offset , wlen - w_offset, (rlen > 0));

tx_aborted:
		if (stat & (DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET))
			break;
	}

	return 0;
}


int dw_i2c_send_bytes(void* i2c_base, unsigned char * out_buf, unsigned int len)
{

	u8 * base = (u8 *)i2c_base, i = 1;

        writel(out_buf[0], (char*)(i2c_base + DW_IC_DATA_CMD));
      	while((readl((char*)(base + DW_IC_RAW_INTR_STAT)) & DW_IC_INTR_START_DET) != DW_IC_INTR_START_DET);
	
        while((i < len) && ((readl((char*)(i2c_base + DW_IC_INTR_STAT)) & DW_IC_INTR_STOP_DET) != DW_IC_INTR_STOP_DET)) {
                if((readl((char*)(base + DW_IC_INTR_STAT)) & DW_IC_INTR_TX_EMPTY) == DW_IC_INTR_TX_EMPTY) {
                	writel(out_buf[i], (char*)(base + DW_IC_DATA_CMD));
                    i++;
        	}
        }
        while((readl((char*)(base + DW_IC_INTR_STAT)) & DW_IC_INTR_STOP_DET) != DW_IC_INTR_STOP_DET);

        readl((char*)(base + DW_IC_CLR_INTR));
        readl((char*)(base + DW_IC_CLR_RX_UNDER));
        readl((char*)(base + DW_IC_CLR_RX_OVER));
        readl((char*)(base + DW_IC_CLR_TX_OVER));
        readl((char*)(base + DW_IC_CLR_RD_REQ));
        readl((char*)(base + DW_IC_CLR_TX_ABRT));
        readl((char*)(base + DW_IC_CLR_RX_DONE));
        readl((char*)(base + DW_IC_CLR_ACTIVITY));
        readl((char*)(base + DW_IC_CLR_STOP_DET));
        readl((char*)(base + DW_IC_CLR_START_DET));
        readl((char*)(base + DW_IC_CLR_GEN_CALL));

	if(i != len)
		return I2C_WRITE_FAILED;

	return 0;
}


int  dw_i2c_smbus_read(void* i2c_base, unsigned char * out_buf, unsigned int wlen, unsigned char *rbuf, unsigned rlen)
{
	unsigned int i = 1, j = 0, len = rlen;
	unsigned char* base = (unsigned char*)i2c_base;
	unsigned int   stat, rx_valid;
	int ret = 0;
	
	u32 param1 = readl(i2c_base + DW_IC_COMP_PARAM_1), tx_fifo_depth, rx_fifo_depth, rx_limit;

	tx_fifo_depth = ((param1 >> 16) & 0xff) + 1;
	rx_fifo_depth = ((param1 >> 8)  & 0xff) + 1;

	i = 0;
	while(i < wlen ) {
		if((readl((char*)(base + DW_IC_INTR_STAT)) & DW_IC_INTR_TX_EMPTY) == DW_IC_INTR_TX_EMPTY) {
			writel(out_buf[i], (char*)(base + DW_IC_DATA_CMD));
			i++;
		}
	}

	i = 0;
	j = 0;


	while(i < rlen) { 
		stat = readl((char*)(base + DW_IC_INTR_STAT));

		if (stat & DW_IC_INTR_RX_FULL) {
			rx_valid = readl(i2c_base + DW_IC_RXFLR);

			for (; len > 0 && rx_valid > 0; len--, rx_valid--)
				rbuf[i++] = readl(i2c_base + DW_IC_DATA_CMD);
		}
	
		rx_limit = rx_fifo_depth - readl(i2c_base + DW_IC_RXFLR);
		if(rx_limit > rlen -j)
			rx_limit = rlen -j;
	
		if(j < rlen && (( stat & DW_IC_INTR_TX_EMPTY) == DW_IC_INTR_TX_EMPTY)) {
			while(rx_limit > 0) {
				writel(0x100, (char*)(base + DW_IC_DATA_CMD));
				j++;
				rx_limit --;
			}
		}
		
		if((readl((char*)(base + DW_IC_INTR_STAT)) & DW_IC_INTR_STOP_DET) == DW_IC_INTR_STOP_DET) {
			stat = readl((char*)(base + DW_IC_INTR_STAT));
			if(stat & DW_IC_INTR_TX_ABRT) {
				if(readl(i2c_base + DW_IC_TX_ABRT_SOURCE) & DW_IC_TX_ABRT_7B_ADDR_NOACK){
					ret = 1;
					goto end;
				} 
			}
			
			if(i == rlen)
				break;
		}
	}

	while((readl((char*)(base + DW_IC_INTR_STAT)) & DW_IC_INTR_STOP_DET) != DW_IC_INTR_STOP_DET);

	if((i != rlen) || (j !=rlen)){
		ret = 2;
		goto end;
	}

end:
	readl((char*)(base + DW_IC_CLR_INTR));
	readl((char*)(base + DW_IC_CLR_RX_UNDER));
	readl((char*)(base + DW_IC_CLR_RX_OVER));
	readl((char*)(base + DW_IC_CLR_TX_OVER));
	readl((char*)(base + DW_IC_CLR_RD_REQ));
	readl((char*)(base + DW_IC_CLR_TX_ABRT));
	readl((char*)(base + DW_IC_CLR_RX_DONE));
	readl((char*)(base + DW_IC_CLR_ACTIVITY));
	readl((char*)(base + DW_IC_CLR_STOP_DET));
	readl((char*)(base + DW_IC_CLR_START_DET));
	readl((char*)(base + DW_IC_CLR_GEN_CALL));

	switch(ret){
		case 1:
			return I2C_SLAVE_NO_ACK;
		case 2:
			return I2C_READ_FAILED;
	}

	return ret;
}

static u32
i2c_dw_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond, int offset)
{
	/*
	 * DesignWare I2C core doesn't seem to have solid strategy to meet
	 * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
	 * will result in violation of the tHD;STA spec.
	 */
	if (cond)
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	else
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * This is just experimental rule; the tHD;STA period turned
		 * out to be proportinal to (_HCNT + 3).  With this setting,
		 * we could meet both tHIGH and tHD;STA timing specs.
		 *
		 * If unsure, you'd better to take this alternative.
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in i2c_dw_scl_lcnt().
		 */
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
}

static u32 i2c_dw_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

void i2c_dw_init(void* i2c_base)
{
	u32 input_clock_khz = 66666;
	u32 ic_con, hcnt, lcnt;
	u32 param1 = readl(i2c_base + DW_IC_COMP_PARAM_1), tx_fifo_depth, rx_fifo_depth;

	tx_fifo_depth = ((param1 >> 16) & 0xff) + 1;
	rx_fifo_depth = ((param1 >> 8)  & 0xff) + 1;
//	BRINGUP_DBG("tx_fifo_depth = %d, rx_fifo_depth = %d\n", tx_fifo_depth, rx_fifo_depth);
//	BRINGUP_DBG("tx_flr = %d, rx_flr = %d\n", readl(i2c_base + DW_IC_TXFLR), readl(i2c_base + DW_IC_RXFLR));

	/* Disable the adapter */
	writel(0, i2c_base + DW_IC_ENABLE);

	/* set standard and fast speed deviders for high/low periods */

	/* Standard-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
				40,	/* tHD;STA = tHIGH = 4.0 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
				47,	/* tLOW = 4.7 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	lcnt = lcnt *  100 / 94;
	hcnt = hcnt *  100 / 94;

	writel(hcnt, i2c_base + DW_IC_SS_SCL_HCNT);
	writel(lcnt, i2c_base + DW_IC_SS_SCL_LCNT);
//	BRINGUP_DBG("Standard-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Fast-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
				6,	/* tHD;STA = tHIGH = 0.6 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
				13,	/* tLOW = 1.3 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	writel(hcnt, i2c_base + DW_IC_FS_SCL_HCNT);
	writel(lcnt, i2c_base + DW_IC_FS_SCL_LCNT);
//	BRINGUP_DBG("Fast-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Configure Tx/Rx FIFO threshold levels */
	writel(tx_fifo_depth - 1, i2c_base + DW_IC_TX_TL);
	writel(0, i2c_base + DW_IC_RX_TL);

	/* configure the i2c master */
	ic_con = DW_IC_CON_MASTER | DW_IC_CON_SLAVE_DISABLE |
		DW_IC_CON_RESTART_EN | DW_IC_CON_SPEED_STD;
	writel(ic_con, i2c_base + DW_IC_CON);
	writel(0, i2c_base + DW_IC_INTR_MASK); /* disable IRQ */
}

static void i2c_dw_xfer_init(void* i2c_base, unsigned short addr)
{
	u32 ic_con;

	/* Disable the adapter */
	writel(0, i2c_base + DW_IC_ENABLE);

	/* set the slave (target) address */
	writel(addr, i2c_base + DW_IC_TAR);

	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = readl(i2c_base + DW_IC_CON);
	ic_con &= ~DW_IC_CON_10BITADDR_MASTER;
	writel(ic_con, i2c_base + DW_IC_CON);

	/* Enable the adapter */
	writel(1, i2c_base + DW_IC_ENABLE);

	/* Enable interrupts */
	writel(DW_IC_INTR_DEFAULT_MASK, i2c_base + DW_IC_INTR_MASK);
}


void dw_i2c_master_init(void* i2c_base, unsigned char slv_addr)
{
	i2c_dw_init(i2c_base);

	while(readl(i2c_base + DW_IC_STATUS) & DW_IC_STATUS_ACTIVITY);

	i2c_dw_xfer_init(i2c_base, slv_addr);
}


int io373x_smbus_set_adr(void* i2c0_base, unsigned short reg)
{
		int ret;
        unsigned char   wbuf[3];
        wbuf[0] = 0;    //CMD_SET_ADR
        wbuf[1] = reg >> 8;
        wbuf[2] = reg & 0xff;

        ret = dw_i2c_send_bytes(i2c0_base, wbuf, 3);

        return ret;
}

int io373x_smbus_read_reg(void* i2c0_base, unsigned char slv_addr, unsigned short reg, unsigned char * rbuf, unsigned char len)
{
		int ret;
        unsigned char   wbuf[3];
        wbuf[0] = 0x81; //CMD_READ_BYTE

    	dw_i2c_master_init(i2c0_base, slv_addr);
        ret = io373x_smbus_set_adr(i2c0_base, reg);
		if(ret)
			return ret;

        ret = dw_i2c_smbus_read(i2c0_base, wbuf, 1, rbuf, len);
        return ret;
}


int io373x_smbus_write_reg(void* i2c0_base,unsigned char slv_addr, unsigned short reg, unsigned char * buf, unsigned char len)
{
		int ret = 0;
        unsigned char   wbuf[16], i;
        if(len > 14)
                return -1;

        wbuf[0] = 0x03; //CMD_READ_BYTE
        wbuf[1] = len;
        for(i = 0; i< len; i++)
                wbuf[2+i] = buf[i];

    	dw_i2c_master_init(i2c0_base, slv_addr);

        ret = io373x_smbus_set_adr(i2c0_base, reg);
		if(ret)
			return ret;

        ret = dw_i2c_send_bytes(i2c0_base, wbuf, len + 2);
        return ret;
}

static unsigned int i2c_bus_num __attribute__ ((section (".data"))) = 0;
static unsigned int i2c_bus_reg_bases[] = {
    NS115_I2C0_BASE,
    NS115_I2C1_BASE,
    NS115_I2C2_BASE,
    NS115_I2C3_BASE,
};

#if defined(CONFIG_I2C_MULTI_BUS)
/*
 * Functions for multiple I2C bus handling
 */
unsigned int i2c_get_bus_num(void)
{
       return i2c_bus_num;
}

int i2c_set_bus_num(unsigned int bus)
{

       if (bus >= CONFIG_SYS_MAX_I2C_BUS)
               return -1;
       i2c_bus_num = bus;
       return 0;
}
#endif

int i2c_init(int speed, int slaveadd)
{
    return 0;
}
int i2c_probe(uchar chip)
{
    unsigned char rbuf[4];
    int *i2c_base = i2c_bus_reg_bases[i2c_bus_num];
    int ret = 0;

    dw_i2c_master_init(i2c_base, chip);

    ret = dw_i2c_smbus_read(i2c_base, NULL, 0, rbuf, 1);

    return ret;
}

int i2c_read(uchar chip, uint addr, int alen, uchar * buffer, int len)
{
    unsigned char wbuf[4];
    int *i2c_base = i2c_bus_reg_bases[i2c_bus_num];
    int ret = 0;
    int i,j;

    debug("i2c_read(chip 0x%x, addr 0x%x, alen %d, len %d,, buffer[0] 0x%02x\n",
        chip, addr, alen, len, buffer[0]);

    if(alen>2){
        printf("addr too long\n");
        return -1;
    }
    dw_i2c_master_init(i2c_base, chip);

    // address phase
    i = 0;
    while(i < alen)
    {
        wbuf[alen-i-1] = (addr >> (8*i)) & 0xff;
        i++;
    }

    ret = dw_i2c_smbus_read(i2c_base, wbuf, 1, buffer, len);

    return ret;
}

int i2c_write(uchar chip, uint addr, int alen, uchar * buffer, int len)
{
    unsigned char wbuf[64];
    int *i2c_base = i2c_bus_reg_bases[i2c_bus_num];
    int ret = 0;
    int i,j;

    debug("i2c_write(chip 0x%x, addr 0x%x, alen %d, len %d,, buffer[0] 0x%02x\n",
        chip, addr, alen, len, buffer[0]);
    if(alen>2){
        printf("addr too long\n");
        return -1;
    }
    // address phase
    i = 0;
    while(i < alen)
    {
        wbuf[alen-i-1] = (addr >> (8*i)) & 0xff;
        i++;
    }
    // data phase
    j=0;
    while(j < len)
    {
        wbuf[i+j] = buffer[j];
        j++;
    }

    dw_i2c_master_init(i2c_base, chip);
    ret = dw_i2c_send_bytes(i2c_base, wbuf, alen+len);

    return ret;
}



