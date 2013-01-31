//#define DEBUG

#include <config.h>
#include <common.h>
#include <part.h>
#include <fat.h>


//#include <asm/arch/mmc.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/mmc_protocol.h>

mmc_controller_data cur_controller_data[2];
mmc_card_data cur_card_data[2];
static block_dev_desc_t mmc_blk_dev[2];

#define NUFRONT_SDMMC_BASE 0x05070000

static int nufront_mmc_read(mmc_card_data *mmc_c,mmc_controller_data *mmc_contr_cur,unsigned int *buff);
//=====================================================================================


block_dev_desc_t *mmc_get_dev(int dev)
{
	if ((dev == 0) || (dev == 1))
		return (block_dev_desc_t *) &mmc_blk_dev[dev];
	else
		return NULL;
}

static int WaitForValue(volatile unsigned int * addr,unsigned int mask,int is_set, unsigned int timeout)
{
	if(is_set == 0)
	{
		while ( timeout-- > 1 && (*addr & mask )!= 0 )
		{}
	}
	else
	{
		while ( ((timeout--) >1) && ((*addr & mask ) == 0))
		{}
	}
	return timeout  ? SDIO_ERR_NO_ERROR : SDIO_ERR_TIMEOUT;
}
//=======================================================================================

static int sdmmc_send_cmd(mmc_card_data *mmc_card_cur,mmc_controller_data *mmc_contr_cur,
			unsigned int cmd, unsigned int arg,unsigned int *response)
{
	int status;
	unsigned int resp[4];
	unsigned int timeout = 0xffff;

	debug("CMD%u: 0x%08x, ARG: 0x%08x\n", (cmd>>24), cmd, arg);
	mmc_card_cur->cmd = cmd;

	//check the command line busy or not SRS9_CMD_INHIBIT_CMD

	timeout = 0xffff;
	while((mmc_contr_cur->io->SRS9 & CICMD_MASK) && (timeout--))
		;
    if(mmc_contr_cur->io->SRS9 & CICMD_MASK){
        printf(" Error: SRS9 %x. cmd line busy\n", mmc_contr_cur->io->SRS9);
    }
	timeout = 0xffff;
	if((cmd & DP_DATA) || (RSP_TYPE_LGHT48B && (((cmd >> 24) & 0x3F) != 0xC))) {
		while((mmc_contr_cur->io->SRS9 & (CICMD_MASK | CIDAT_MASK)) && (timeout--))
			;
        if(mmc_contr_cur->io->SRS9 & (CICMD_MASK | CIDAT_MASK)){
            printf(" Error: SRS9 %x. cmd or data line busy\n", mmc_contr_cur->io->SRS9);

			/* reset slot data&cmd logic */
			mmc_contr_cur->io->SRS11 |= 0x3<<25;
			timeout = 0x10000;
			while( (mmc_contr_cur->io->SRS11 & (0x3<<25)) && timeout--)
				;

			if(mmc_contr_cur->io->SRS11 & (0x3<<25)){
				printf("Error. Timeout to wait slot controller reset done\n");
				return -1;
			}
        }
		//		status = WaitForValue(&mmc_contr_cur->io->SRS9,(CICMD_MASK | CIDAT_MASK), 0, timeout);
		//		if (status == 0)
		//			return status;
	}
	mmc_contr_cur->io->SRS12 &= (~(ECL | CINT | CR_CARD | CIN_CARD));

	if (cmd & DP_DATA){
		mmc_contr_cur->io->SRS1 = (mmc_card_cur->blkcnt << 16) | mmc_card_cur->blksize;
		if(mmc_card_cur->blkcnt > 1)
			cmd |= 0x22; /* Enable Multi Block, Block Count */
		else
			cmd |= 0x02; /* Enable Single Block, Block Count */

		debug( "BlockCount=%u, BlockSize=%u\n", mmc_card_cur->blkcnt, mmc_card_cur->blksize);
	}

	debug(" SRS12=0x%x SRS09=0x%x Info @L%d \n",mmc_contr_cur->io->SRS12,mmc_contr_cur->io->SRS9,__LINE__);

	mmc_contr_cur->io->SRS2 = arg;
	mmc_contr_cur->io->SRS3 = cmd;

	//	debug(" SRS12=0x%x  @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
	timeout = 0xffff;
	status = WaitForValue(&mmc_contr_cur->io->SRS12, (CC_MASK | ERRI_MASK), 1, timeout);
	if (status != 1){
		debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
		mmc_contr_cur->io->SRS12 = 0xffff0000;
		//		printf("mmc_contr_cur->io->SRS12 = 0x%x, %d\n",mmc_contr_cur->io->SRS12,__LINE__);
		return 0;
	}

	if(mmc_contr_cur->io->SRS12 & ERRI_MASK){
		debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
		mmc_contr_cur->io->SRS12 &= 0xFFFF0000;
		return 0;
	}
	mmc_contr_cur->io->SRS12 = CC_MASK;   //// clear command complete status
	if((cmd & DP_DATA) != DP_DATA){
		status = WaitForValue(&mmc_contr_cur->io->SRS12,ERRI_MASK,0,timeout);
		if(status != 1){
			debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
			mmc_contr_cur->io->SRS12 = 0xffff0000;
			//			printf("mmc_contr_cur->io->SRS12 = 0x%x, %d\n",mmc_contr_cur->io->SRS12,__LINE__);
			return 0;
		}
	}

	if ( RESPONSE_48BIT(cmd) ){
		resp[0] = mmc_contr_cur->io->SRS4;
		mmc_card_cur->response[0] = resp[0];
		debug("  resp48 [0]=0x%x\n",mmc_card_cur->response[0]);
	}
	//long responses
	if ( RESPONSE_136BIT(cmd) ) {
		resp[0] = mmc_contr_cur->io->SRS4;
		resp[1] = mmc_contr_cur->io->SRS5;
		resp[2] = mmc_contr_cur->io->SRS6;
		resp[3] = mmc_contr_cur->io->SRS7;
		mmc_card_cur->response[0] = resp[0] << 8;
		mmc_card_cur->response[1] = (resp[1] << 8) | ((resp[0] >> 24) & 0xff);
		mmc_card_cur->response[2] = (resp[2] << 8) | ((resp[1] >> 24) & 0xff);
		mmc_card_cur->response[3] = (resp[3] << 8) | ((resp[2] >> 24) & 0xff);

		debug("  resp136[0]=0x%x\n",mmc_card_cur->response[0]);
		debug("  resp136[1]=0x%x\n",mmc_card_cur->response[1]);
		debug("  resp136[2]=0x%x\n",mmc_card_cur->response[2]);
		debug("  resp136[3]=0x%x\n",mmc_card_cur->response[3]);
	}
	return 1;
}

//===============================================================================================
unsigned char mmc_detect_card(mmc_card_data *mmc_card_cur,
			mmc_controller_data *mmc_contr_cur)
{
	unsigned char err;
	unsigned int argument = 0;
	unsigned int ocr_value = 0;
	unsigned int ocr_recvd = 0;
	unsigned int ret_cmd41 = 0;
	unsigned int hcs_val = 0;
	unsigned int resp[4];
	unsigned short retry_cnt = 2000;
	unsigned int frequency = 400;


	mmc_card_cur->RCA = MMC_RELATIVE_CARD_ADDRESS;
	argument = 0x00000000;

	err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD0,argument,resp);
	if (err != 1)
	{
		printf("mmc_detect_card: MMC_CMD0 failed err %d\n", err);
		return err;
	}
	argument = SD_CMD8_CHECK_PATTERN | SD_CMD8_2_7_3_6_V_RANGE;
	err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, MMC_SDCMD8, argument, resp);
	hcs_val = (err == 1) ?
		MMC_OCR_REG_HOST_CAPACITY_SUPPORT_SECTOR :
		MMC_OCR_REG_HOST_CAPACITY_SUPPORT_BYTE;

	argument = 0x0000 << 16;
	err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, MMC_CMD55, argument, resp);
	if (err == 1) {
		mmc_card_cur->card_type = SD_CARD;
		ocr_value = (0x1FF << 15);
		ocr_value |= hcs_val;
		ret_cmd41 = MMC_ACMD41;
	} else {
		mmc_card_cur->card_type = MMC_CARD;
		ocr_value = (0x1FF << 15);  /* for foresee 3v3 tsd */
		ocr_value |= MMC_OCR_REG_ACCESS_MODE_SECTOR;
		ret_cmd41 = MMC_CMD1;
		argument = 0x00000000;
		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD0,argument,resp);
		if (err != 1)
		{
			printf("mmc_detect_card: MMC_CMD0 failed err %d\n", err);
			return err;
		}
	}

	debug("mmc_detect_card: MMC_CMD55 try %s\n",
				mmc_card_cur->card_type == SD_CARD ? "SD_CARD": "MMC_CARD");
	argument = ocr_value;
	err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, ret_cmd41, argument, resp);
	if (err == 0){
		debug("mmc_detect_card: ret_cmd41 cmd 0x%x failed err %d\n", ret_cmd41, err);
		return err;
	}

	ocr_recvd = ((mmc_resp_r3 *)mmc_card_cur->response)->ocr;

	while (!(ocr_recvd & (0x1 << 31)) && (retry_cnt > 0)) {
		retry_cnt--;
		if (mmc_card_cur->card_type == SD_CARD) {
			argument = 0x0000 << 16;
			err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD55,
						argument,resp);
		}

		argument = ocr_value;
		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, ret_cmd41,
					argument, resp);
		if (err != 1)
			return err;
		ocr_recvd = ((mmc_resp_r3 *)mmc_card_cur->response)->ocr;
	}

	if (!(ocr_recvd & (0x1 << 31)))
	{
		printf("mmc_detect_card: failed to wait card init done, ocr_recvd 0x%x\n", ocr_recvd);
		return 0;
	}

	if (mmc_card_cur->card_type == MMC_CARD) {
		if ((ocr_recvd & MMC_OCR_REG_ACCESS_MODE_MASK) ==
					MMC_OCR_REG_ACCESS_MODE_SECTOR) {
			mmc_card_cur->mode = SECTOR_MODE;
		} else {
			mmc_card_cur->mode = BYTE_MODE;
		}
		ocr_recvd &= ~MMC_OCR_REG_ACCESS_MODE_MASK;
	}else {
		if ((ocr_recvd & MMC_OCR_REG_HOST_CAPACITY_SUPPORT_MASK)
					== MMC_OCR_REG_HOST_CAPACITY_SUPPORT_SECTOR) {
			mmc_card_cur->mode = SECTOR_MODE;
		} else {
			mmc_card_cur->mode = BYTE_MODE;
		}
		ocr_recvd &= ~MMC_OCR_REG_HOST_CAPACITY_SUPPORT_MASK;
	}
	ocr_recvd &= ~(0x1 << 31);
	if (!(ocr_recvd & ocr_value))
		return 0;

	err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, MMC_CMD2, 0, resp);
	if (err != 1){
		printf("mmc_detect_card: MMC_CMD2 failed err %d\n", err);
		return err;
	}

	/* store card CID */
	mmc_card_cur->CID[0] = mmc_card_cur->response[0];
	mmc_card_cur->CID[1] = mmc_card_cur->response[1];
	mmc_card_cur->CID[2] = mmc_card_cur->response[2];
	mmc_card_cur->CID[3] = mmc_card_cur->response[3];

	if (mmc_card_cur->card_type == MMC_CARD) {
		argument = mmc_card_cur->RCA << 16;
		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, MMC_CMD3,
					argument, resp);
		if (err != 1)
			return err;
	} else {
		argument = 0x00000000;
		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur, MMC_CMD3,
					argument, resp);
		if (err != 1)
			return err;

		mmc_card_cur->RCA =
			((mmc_resp_r6 *) mmc_card_cur->response)->newpublishedrca;
	}
#ifdef DEBUG
	{
		char *type;
		char *mode;
		switch(mmc_card_cur->card_type){
			case SD_CARD:
				type = "SD_CARD";
				break;
			case MMC_CARD:
				type = "MMC_CARD";
				break;
			default:
				type = "none";
				break;
		}
		switch(mmc_card_cur->mode){
			case SECTOR_MODE:
				mode = "SECTOR_MODE";
				break;
			case BYTE_MODE:
				mode = "BYTE_MODE";
				break;
			default:
				mode = "none";
				break;
		}
		printf("mmc detect card: slot %d, type %s, mode %s\n",
					mmc_contr_cur->slot, type, mode);
	}
#endif
	return 1;
}
//=========================================================================================
unsigned char mmc_read_cardsize( mmc_card_data *mmc_dev_data,
			mmc_controller_data *mmc_contr_cur,mmc_csd_reg_t *cur_csd)
{
	mmc_extended_csd_reg_t ext_csd;
	unsigned int size, count, blk_len, blk_no, card_size, argument;
	unsigned char err;
	unsigned int resp[4];
    unsigned int multi;
    unsigned int block_len;

    debug("csd version %d\n", cur_csd->csd_structure);

	if (mmc_dev_data->mode == SECTOR_MODE)
	{
		if (mmc_dev_data->card_type == SD_CARD)
		{
            if(cur_csd->csd_structure == 1)
            {
			card_size = (((mmc_sd2_csd_reg_t *) cur_csd)->
						c_size_lsb & MMC_SD2_CSD_C_SIZE_LSB_MASK) |
				((((mmc_sd2_csd_reg_t *) cur_csd)-> c_size_msb
				  & MMC_SD2_CSD_C_SIZE_MSB_MASK)  <<
				 MMC_SD2_CSD_C_SIZE_MSB_OFFSET);
			mmc_dev_data->size = (card_size + 1) * 1024;
            }
            else if(cur_csd->csd_structure == 0)
            {
                multi = 1 << (2 + cur_csd->c_size_mult);
                block_len = 1 << (cur_csd->read_bl_len);
    			card_size = (cur_csd->c_size_lsb & MMC_CSD_C_SIZE_LSB_MASK) |
    				((cur_csd->c_size_msb & MMC_CSD_C_SIZE_MSB_MASK)
    				     << MMC_CSD_C_SIZE_MSB_OFFSET);
    			mmc_dev_data->size = (card_size + 1) * multi;
            }
            else
                mmc_dev_data->size = 0;
			if (mmc_dev_data->size == 0)
				return 0;
		}
		else
		{
            debug("mmc spec ver %d\n", cur_csd->spec_vers);
			argument = 0x00000000;
            mmc_dev_data->blkcnt = 1;
            mmc_dev_data->blksize = 512;
			err = sdmmc_send_cmd(mmc_dev_data,mmc_contr_cur,MMC_CMD8,
						argument, resp);
			if (err != 1)
			{
                printf("Error to read ExtCSD\n");
				return err;
			}
			err = nufront_mmc_read(mmc_dev_data,mmc_contr_cur,
						(unsigned int *)&ext_csd);
            mmc_dev_data->blkcnt = 0;
            mmc_dev_data->blksize = 0;
			if (err != 1)
			{
                printf("Error to read ExtCSD\n");
				return err;
            }
			mmc_dev_data->size = ext_csd.sectorcount;

		}
	}
	else
	{
		if (cur_csd->c_size_mult >= 8)
			return 0;

		if (cur_csd->read_bl_len >= 12)
			return 0;

		/* Compute size */
		count = 1 << (cur_csd->c_size_mult + 2);
		card_size = (cur_csd->c_size_lsb & MMC_CSD_C_SIZE_LSB_MASK) |
			((cur_csd->c_size_msb & MMC_CSD_C_SIZE_MSB_MASK)
			 << MMC_CSD_C_SIZE_MSB_OFFSET);
		blk_no = (card_size + 1) * count;
		blk_len = 1 << cur_csd->read_bl_len;
		size = blk_no * blk_len;
		mmc_dev_data->size = size / MMCSD_SECTOR_SIZE;
        debug("c_size_mult %d, read_bl_len %d, card_size %d\n",
            cur_csd->c_size_mult,
            cur_csd->read_bl_len,
            card_size);
		if (mmc_dev_data->size == 0)
			return 0;
	}
	return 1;
}

//=========================================================================================

int SDIOHost_SetSDCLK(mmc_controller_data *mmc_contr_cur, unsigned int *frequency )//kxh?
{

        unsigned int baseclk; /* in KHZ */
        unsigned int i,temp1,temp2;
        unsigned int status = 0;

	mmc_contr_cur->io->SRS11 &= (~SDCE_ENABLE);

	baseclk = CFG_AHB_SDIO_CLOCK;

        for ( i = 1; i < 2046; i++ )
        {
                if ( ( baseclk / i ) <= *frequency )
                {
                        break;
                }
		if(((baseclk / i) == *frequency) && (baseclk % i) == 0)
			break;
        }

	debug("SetClk %d=%d/%d\n",*frequency,baseclk,i);
	temp2 = ((i / 2) << 8);
	temp1 |= (temp2 & 0xff00) | ((temp2 & 0x30000) >> 10);

     	// clear old frequency base settings
        mmc_contr_cur->io->SRS11 &= (~SDCLKFS_MASK);

        // Set SDCLK Frequency Select and Internal Clock Enable
        mmc_contr_cur->io->SRS11 |= (temp1 | ICS_STABLE);

        // wait for clock stable is 1
	status = WaitForValue(&mmc_contr_cur->io->SRS11,ICS_STABLE, 1, 0xfffff);//COMMANDS_TIMEOUT );
	if ( !status )
		return status;
        // set SD clock on
        mmc_contr_cur->io->SRS11 |= SDCE_ENABLE;

        // write to frequency the real value of set frequecy
        *frequency = baseclk / i;
        return SDIO_ERR_NO_ERROR;

}

int SDIOHost_SetBusWidth(mmc_card_data *mmc_dev_data,mmc_controller_data *mmc_contr_cur,int bus_width)
{
	unsigned char err;
	unsigned int argument;
	unsigned int resp[4];

	debug("SDIOHost_SetBusWidth(slot %d, width %d)\n", mmc_contr_cur->slot, bus_width);

	if (mmc_dev_data->card_type == SD_CARD){
		if(bus_width == 4) {
			argument = (3<<24) | (183<<16) | (1<<8) | (0<<0);
			err = sdmmc_send_cmd(mmc_dev_data,mmc_contr_cur, MMC_CMD6,
						argument, resp);
			if (err != 1)
			{
				printf("mmc_set_clk_data_width data width set error\n");
				return err;
			}
			// set 4 bit bus mode
			mmc_contr_cur->io->SRS10 |= DATA_WIDTH_4BIT;
			//dissable mmc8 mode
			SDIO_REG_HSFR0 &=  (~(1 << (24 + mmc_contr_cur->slot)));
		}else{
			// set 1 bit bus mode
			mmc_contr_cur->io->SRS10 &= ~DATA_WIDTH_4BIT;
			//dissable mmc8 mode
			SDIO_REG_HSFR0 &=  (~(1 << (24 + mmc_contr_cur->slot)));
		}
	}else{
		if(bus_width == 8){
			argument = (3<<24) | (183<<16) | (2<<8) | (0<<0);
			err = sdmmc_send_cmd(mmc_dev_data,mmc_contr_cur, MMC_CMD6,
						argument, resp);
			if (err != 1)
			{
				printf("mmc_set_clk_data_width data width set error\n");
				return err;
			}
			//enable mmc8 mode
			SDIO_REG_HSFR0 |= ((1 << (24 + mmc_contr_cur->slot)));
		}else if (bus_width == 4){
			argument = (3<<24) | (183<<16) | (1<<8) | (0<<0);
			err = sdmmc_send_cmd(mmc_dev_data,mmc_contr_cur, MMC_CMD6,
						argument, resp);
			if (err != 1)
			{
				printf("mmc_set_clk_data_width data width set error\n");
				return err;
			}
			// set 4 bit bus mode
			mmc_contr_cur->io->SRS10 |= DATA_WIDTH_4BIT;
			//dissable mmc8 mode
			SDIO_REG_HSFR0 &=  (~(1 << (24 + mmc_contr_cur->slot)));
		} else if (bus_width == 1) {
			// set 1 bit bus mode
			mmc_contr_cur->io->SRS10 &= ~DATA_WIDTH_4BIT;
			//dissable mmc8 mode
			SDIO_REG_HSFR0 &=  (~(1 << (24 + mmc_contr_cur->slot)));
		}
	}
	return 1;
}

//==================================================================================================

static int mmc_set_clk_data_width(mmc_card_data *mmc_dev_data, \
			mmc_controller_data *mmc_contr_cur,        \
			unsigned char data_width,unsigned int transfer_clk)
{
	unsigned char err;
	unsigned int argument;
	unsigned int resp[4];

	//set the clock
	SDIOHost_SetSDCLK(mmc_contr_cur,&transfer_clk);//KHZ
}

unsigned char sdmmc_init_setup(mmc_controller_data *mmc_contr_cur)
{
	unsigned char status=0;
	unsigned int frequency = 400;	//KHZ


	//reset sd/mmc host controller
	SDIO_REG_HSFR0 |= SOFT_RESET;
	while(SDIO_REG_HSFR0 & SOFT_RESET)
		;

	//set the DEBNCE_val;
	SDIO_REG_HSFR1 = DEBOUNCE_TIME;


	//set the time of stable
	if( (SDIO_REG_HSFR0 & (0x10000 << mmc_contr_cur->slot)) == 0 )
	{
		debug("slot [%d] is not stalbe\n",mmc_contr_cur->slot);
		return 0;
	}

	while((mmc_contr_cur->io->SRS9 & CSS_MASK) == 0)
		;

	//enable the interrupt status register
	mmc_contr_cur->io->SRS13 = 0xffffffff;
	mmc_contr_cur->io->SRS14 = 0;

	//enable the interrupt  register
	//set the time of date transfer timeout
	mmc_contr_cur->io->SRS11 &= DTCV_MASK;
	mmc_contr_cur->io->SRS11 |= DTCV_TIME;

	// set 1 bit bus mode
	mmc_contr_cur->io->SRS10 &= DATA_WIDTH_1BIT;
	//dissable mmc8 mode
	SDIO_REG_HSFR0 &=  (~(1 << (24 + mmc_contr_cur->slot)));

	// disable bus power
	mmc_contr_cur->io->SRS10 &= ~(VOLT_MASK | BUS_POWER);

	// set the bus power
	mmc_contr_cur->io->SRS10 |= (SET_BUS_3V3 | BUS_POWER);

	status = SDIOHost_SetSDCLK(mmc_contr_cur, &frequency);
	if ( status != 1 )
	{
		printf("SDIOHost_SetSDCLK() failed\n");
		return status;
	}
	return 1;
}
//==================================================================================================

unsigned char configure_controller(mmc_controller_data *cur_controller_data,int slot)
{
	int ret = 1;

	cur_controller_data->slot = slot;
	cur_controller_data->base = NUFRONT_SDMMC_BASE;
	cur_controller_data->io = (void *)(cur_controller_data->base + (cur_controller_data->slot + 1) * 0x100);

	return ret;
}
//============================================================================================

unsigned char configure_mmc(mmc_card_data *mmc_card_cur,mmc_controller_data *mmc_contr_cur)
{
	unsigned char ret_val=0;
	unsigned int argument;
	unsigned int resp[4];
	unsigned int trans_fact, trans_unit,max_dtr;
	unsigned int retries = 10;
	unsigned int i=0, cmd_retry=3;
	mmc_csd_reg_t Card_CSD;
	unsigned char trans_speed;
	int dsor,err;


	ret_val = sdmmc_init_setup(mmc_contr_cur);

	udelay(1000 * 10);
	if (ret_val != 1)
	{
		printf("sdmmc_init_setup() failed. ret %d\n", ret_val);
		return ret_val;
	}
	//	ret_val = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD0,
	//					0x00000000, resp);
	//      if (ret_val == 0)
	//	{
	//              printf("reset the card failed\n");
	//		return 0;
	//	}

	do {
		ret_val = mmc_detect_card(mmc_card_cur, mmc_contr_cur);
		retries--;
	} while ((retries > 0) && (ret_val != 1));

	if(ret_val != 1){
		printf("mmc_detect_card() failed. ret %d\n", ret_val);
		return ret_val;
	}
	//send cmd9 for get csd register contents
	argument = mmc_card_cur->RCA << 16;
	ret_val = sdmmc_send_cmd(mmc_card_cur, mmc_contr_cur,MMC_CMD9,
				argument, resp);
	if (ret_val != 1)
		return ret_val;

	((unsigned int *) &Card_CSD)[3] = mmc_card_cur->response[3];
	((unsigned int *) &Card_CSD)[2] = mmc_card_cur->response[2];
	((unsigned int *) &Card_CSD)[1] = mmc_card_cur->response[1];
	((unsigned int *) &Card_CSD)[0] = mmc_card_cur->response[0];

	if (mmc_card_cur->card_type == MMC_CARD)
		mmc_card_cur->version = Card_CSD.spec_vers;

	trans_speed = Card_CSD.tran_speed;

	ret_val = sdmmc_send_cmd(mmc_card_cur, mmc_contr_cur,MMC_CMD4,
				MMC_DSR_DEFAULT << 16, resp);
	if (ret_val != 1)
		return ret_val;

	trans_unit = trans_speed & MMC_CSD_TRAN_SPEED_UNIT_MASK;
	trans_fact = trans_speed & MMC_CSD_TRAN_SPEED_FACTOR_MASK;

	if (trans_unit > MMC_CSD_TRAN_SPEED_UNIT_100MHZ)
		return 0;

	if ((trans_fact < MMC_CSD_TRAN_SPEED_FACTOR_1_0) ||
				(trans_fact > MMC_CSD_TRAN_SPEED_FACTOR_8_0))
		return 0;

	trans_unit >>= 0;
	trans_fact >>= 3;

#if 0
	max_dtr = tran_exp[trans_unit] * tran_mant[trans_fact];
	dsor = CFG_AHB_SDIO_CLOCK / max_dtr;

	if (CFG_AHB_SDIO_CLOCK / dsor > max_dtr)
		dsor++;

	unsigned int frequency = CFG_AHB_SDIO_CLOCK / dsor;
#endif

	//send cmd7 for select card
	argument = mmc_card_cur->RCA << 16;

	while(i < cmd_retry)
	{
		ret_val = sdmmc_send_cmd(mmc_card_cur, mmc_contr_cur,
					MMC_CMD7_SELECT, argument, resp);
		if (ret_val != 1)
		{
			i++;
		}else{
			break;
		}
	}

	if(ret_val != 1 )
		return ret_val;

    ret_val = mmc_read_cardsize(mmc_card_cur, mmc_contr_cur, &Card_CSD);
    if (ret_val != 1)
        return ret_val;

	//attention: for MMC card ,the highest frequency is 20MHz
	//           for SD card, the highest frequency is 25MHz

//	unsigned int frequency = 25000;
	unsigned int frequency = 12500;
	ret_val = SDIOHost_SetSDCLK(mmc_contr_cur,&frequency);
	if (ret_val != 1)
		return ret_val;

	//get card status
	err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD13,
				mmc_card_cur->RCA<<16, resp);
	if(err != 1)
		return err;


	//set bus width
	unsigned int srs13;
	srs13 = mmc_contr_cur->io->SRS13;
	mmc_contr_cur->io->SRS13 &= (~(1 << 8));

	if(mmc_card_cur->card_type == SD_CARD){
		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD55,
					mmc_card_cur->RCA<<16, resp);
		if(err != 1)
			return err;

		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_ACMD6,
					2,resp);
		if(err != 1)
			return err;
		// set 4 bit bus mode
		mmc_contr_cur->io->SRS10 |= DATA_WIDTH_4BIT;
		//dissable mmc8 mode
		SDIO_REG_HSFR0 &=  (~(1 << (24 + mmc_contr_cur->slot)));
	}else{
		SDIOHost_SetBusWidth(mmc_card_cur,mmc_contr_cur,8);
	}
	mmc_contr_cur->io->SRS13 = srs13;

	/* wait card ready */
	retries = 20;
	do{
		err = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD13,
				mmc_card_cur->RCA<<16, resp);
		if(err != 1)
			continue;

		if( (mmc_card_cur->response[0] & 0x100)
			&& ((mmc_card_cur->response[0] >> 9) & 0xf) <= 4){
			break;
		}
	}while(--retries >= 0);

	if(retries == 0)
	{
		printf("Failed to wait card ready, after switch bus width\n");
	}
	/* Configure the block length to 512 bytes */
	argument = MMCSD_SECTOR_SIZE;
	ret_val = sdmmc_send_cmd(mmc_card_cur, mmc_contr_cur, MMC_CMD16,
				argument, resp);
	if (ret_val != 1)
		return ret_val;

	return 1;//ret_val;
}
//=======================================================================================
static int nufront_mmc_read(mmc_card_data *mmc_c,mmc_controller_data *mmc_contr_cur,
			unsigned int *buff)
{
	unsigned int i, blkcnt;
	int status;
	unsigned int blknum;
	unsigned int timeout = 0xfffff;

	blknum = mmc_c->blkcnt;
	blkcnt =  mmc_c->blkcnt;

	if(mmc_contr_cur->io->SRS12 & ERRI_MASK)
	{
		debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
		mmc_contr_cur->io->SRS12 = 0xffff0000;
		return 0;
	}
	while (blkcnt)
	{
        timeout = 0xfffff;
		while(((mmc_contr_cur->io->SRS12 & BRR_MASK) != BRR_MASK) && (timeout--))
			;
		mmc_contr_cur->io->SRS12 = BRR_MASK;

		for(i=0; i<mmc_c->blksize; i+=4)
		{
			unsigned int tmp = mmc_contr_cur->io->SRS8;
			//	 	        printf("read %d tmp 0x%08x\n",i, tmp);
			writel(tmp, buff);
			buff ++;
		}
		if((blkcnt%256)==0)
			printf("#");
		blkcnt--;
	}
	//	printf("\n");
	status = WaitForValue(&mmc_contr_cur->io->SRS12,(TC_MASK | ERRI_MASK),1, 0xfffff);
	if (status != 1){
		debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
		return status;
	}
	if(mmc_contr_cur->io->SRS12 & ERRI_MASK){
		debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
		mmc_contr_cur->io->SRS12 = 0xffff0000;
		return 0;
	}
	while((mmc_contr_cur->io->SRS12 & TC_MASK) != TC_MASK)
		;
	mmc_contr_cur->io->SRS12 = TC_MASK;
	return blknum;
}
//================================================================================================
static int nufront_mmc_write(mmc_card_data *mmc_c,mmc_controller_data *mmc_contr_cur,unsigned int *buff)
{
	unsigned int i, blkcnt,blknum;
	int status;
    unsigned int timeout = 0xffff0000;

	blkcnt = mmc_c->blkcnt;
	//	status = WaitForValue(&mmc_contr_cur->io->SRS12, BWR_MASK,1,0xffff);
	//printf("mmc_contr_cur->io->SRS12=0x%x %d \n",mmc_contr_cur->io->SRS12,__LINE__);
	if(mmc_contr_cur->io->SRS12 & ERRI_MASK){
        debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
		//		mmc_contr_cur->io->SRS12 = 0xffff0000;
		return 0;
	}

    status = WaitForValue(&mmc_contr_cur->io->SRS12, (BWR_MASK | ERRI_MASK),1,timeout);
    if((mmc_contr_cur->io->SRS12 & ERRI_MASK) || (status == SDIO_ERR_TIMEOUT) ){
        debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
        return 0;
    }
	while (blkcnt)
	{
//		while((mmc_contr_cur->io->SRS12 & BWR_MASK) != BWR_MASK)
//			;
		timeout = 0xffff0000;
        status = WaitForValue(&mmc_contr_cur->io->SRS12, (BWR_MASK | ERRI_MASK),1,timeout);
        if((mmc_contr_cur->io->SRS12 & ERRI_MASK) || (status == SDIO_ERR_TIMEOUT) ){
            debug(" SRS12=0x%x  ERROR @L%d \n",mmc_contr_cur->io->SRS12,__LINE__);
            return 0;
        }
		mmc_contr_cur->io->SRS12 = BWR_MASK;

		for(i = 0; i < mmc_c->blksize; i+=4)
		{
			mmc_contr_cur->io->SRS8 = *buff;
			//                        printf("0x%08x\n", mmc_contr_cur->io->SRS8);
			buff ++;
		}
		if((blkcnt%256)==0)
			printf("#");
		blkcnt--;
	}
	//printf("\n");
	status = WaitForValue(&mmc_contr_cur->io->SRS12,TC_MASK | ERRI_MASK,1,0xffff);
	if(mmc_contr_cur->io->SRS12 & ERRI_MASK){
		//		mmc_contr_cur->io->SRS12 = 0xffff0000;
		return 0;
	}
	while((mmc_contr_cur->io->SRS12 & TC_MASK) != TC_MASK)
		;
	mmc_contr_cur->io->SRS12 = TC_MASK;

	return 1;
}

static int nufront_mmc_erase_sect(unsigned int start,mmc_controller_data *mmc_contr_cur,
			mmc_card_data *mmc_c,int size)
{
	unsigned char err;
	unsigned int resp[4];
	unsigned int start_sec, end_sec;

	if (mmc_c->mode == SECTOR_MODE) {
		start_sec = start;
		end_sec = start -1 + ((size + MMCSD_SECTOR_SIZE - 1)/MMCSD_SECTOR_SIZE);
	} else {
		start_sec = start * MMCSD_SECTOR_SIZE;
		end_sec = start * MMCSD_SECTOR_SIZE + size -1;
	}

	/*Secure Trim Step1 */
	/* CMD 35 : ERASE_START, give start address for erasing */
	err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD35,start_sec,resp);
	if (1 != err)
		return err;

	/* CMD 36 : ERASE_END,   give end address for erasing */
	err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD36,end_sec,resp);
	if (1 != err)
		return err;

	/* CMD 38 : ERASE */
	err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD38,0x80000001,resp);
	if (1 != err)
		return err;

	/* CMD 13 : SEND_STATUS */

	/*Secure Trim Step2 */
	/* CMD 35 : ERASE_START, give start address for erasing */
	err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD35,start_sec,resp);
	if (1 != err)
		return err;

	/* CMD 36 : ERASE_END,   give end address for erasing */
	err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD36,end_sec,resp);
	if (1 != err)
		return err;

	/* CMD 38 : ERASE */
	err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD38,0x80008000,resp);
	if (1 != err)
		return err;

	return 1;
}


//=================================================================================================

unsigned long nufront_mmc_read_sect(unsigned int start_sec, unsigned int num_bytes,
			mmc_controller_data *mmc_contr_cur,
			mmc_card_data *mmc_c, unsigned int *output_buf)
{
	unsigned char err;
	unsigned int argument;
	unsigned int resp[4];
	unsigned int num_sec_val = (num_bytes + (MMCSD_SECTOR_SIZE - 1)) / MMCSD_SECTOR_SIZE;
	unsigned int sec_inc_val,cmd,blknum,blk_cnt_current_tns;

	if (num_sec_val == 0) {
		return 0;
	}
	if (mmc_c->mode == SECTOR_MODE) {
		argument = start_sec;
		sec_inc_val = 1;
	} else {
		argument = start_sec * MMCSD_SECTOR_SIZE;
		sec_inc_val = MMCSD_SECTOR_SIZE;
	}

	//	mmc_c->blkcnt = num_sec_val;
	mmc_c->blksize = 0x200;

	while(num_sec_val){
		if (num_sec_val > 0xffff)
			blk_cnt_current_tns = 0xffff;
		else
			blk_cnt_current_tns = num_sec_val;

		mmc_c->blkcnt = blk_cnt_current_tns;
		if(blk_cnt_current_tns == 1){
			err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD17,argument,resp);
			if (err != 1)
				return err;
		}else{
			if (mmc_c->card_type == MMC_CARD){
				debug("MMC set multi_block_read count %d\n", blk_cnt_current_tns);
				err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD23,blk_cnt_current_tns,resp);
				if (err != 1)
					return err;
			}
			debug("MMC/SD multi block read: count %d\n", blk_cnt_current_tns);
			err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD18,argument,resp);
			if (err != 1)
				return err;
		}
		blknum = nufront_mmc_read(mmc_c,mmc_contr_cur,output_buf);

		if(blk_cnt_current_tns > 1){
			if (mmc_c->card_type == SD_CARD){
				err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD12,0,resp);
				if (err != 1)
					return err;
			}
		}

		output_buf += (MMCSD_SECTOR_SIZE / 4) * blk_cnt_current_tns;
		argument += sec_inc_val * blk_cnt_current_tns;
		num_sec_val -= blk_cnt_current_tns;
	}

	return blknum;
}

//=======================================================================================
unsigned char nufront_mmc_write_sect(unsigned int *input_buf,unsigned int num_bytes,
			mmc_controller_data *mmc_contr_cur,
			mmc_card_data *mmc_c,unsigned long start_sec)
{
	unsigned char err;
	unsigned int argument;
	unsigned int resp[4];
	unsigned int num_sec_val = (num_bytes + (MMCSD_SECTOR_SIZE - 1)) / MMCSD_SECTOR_SIZE;
	unsigned int sec_inc_val;
	unsigned int blk_cnt_current_tns,cmd;

	if (num_sec_val == 0) {
		printf("mmc write: Invalid size\n");
		return 0;
	}

	if (mmc_c->mode == SECTOR_MODE) {
		argument = start_sec  ;
		sec_inc_val = 1;
	} else {
		argument = start_sec * MMCSD_SECTOR_SIZE;
		sec_inc_val = MMCSD_SECTOR_SIZE;
	}

	mmc_c->blksize = 0x200;

	while(num_sec_val){
		if (num_sec_val >0xffff)
			blk_cnt_current_tns = 0xffff;
		else
			blk_cnt_current_tns = num_sec_val;

		mmc_c->blkcnt = blk_cnt_current_tns;
		if (blk_cnt_current_tns == 1){
			err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD24,argument,resp);
			if (err != 1)
				return err;
		}else{
			if (mmc_c->card_type == MMC_CARD){
				debug("MMC set multi_block_write count %d\n", blk_cnt_current_tns);
				err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD23,blk_cnt_current_tns,resp);
				if (err != 1)
					return err;
			}
			debug("MMC/SD multi block write: count %d\n", blk_cnt_current_tns);
			err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD25,argument,resp);
			if (err != 1)
				return err;
		}

		err = nufront_mmc_write(mmc_c,mmc_contr_cur,input_buf);
		if (err != 1)
			return err;
		if(blk_cnt_current_tns > 1){
			if (mmc_c->card_type == SD_CARD){
				err = sdmmc_send_cmd(mmc_c,mmc_contr_cur,MMC_CMD12,0,resp);
				if (err != 1)
					return err;
			}
		}

		input_buf += (MMCSD_SECTOR_SIZE / 4) * blk_cnt_current_tns;
		argument += sec_inc_val * blk_cnt_current_tns;
		num_sec_val -= blk_cnt_current_tns;
	}

	return 1;
}
//========================================================================================

unsigned long mmc_bread(int dev_num, ulong start_block, ulong blkcnt, ulong *dst)
{
	int retry=3;
	unsigned long ret;
	int retries = 20;
	unsigned int resp[4];
	mmc_card_data *mmc_card_cur = &cur_card_data[dev_num];
	mmc_controller_data *mmc_contr_cur = &cur_controller_data[dev_num];

	while(retry--){
		ret = (unsigned long)nufront_mmc_read_sect(start_block,(blkcnt * MMCSD_SECTOR_SIZE),
					&cur_controller_data[dev_num], &cur_card_data[dev_num],(unsigned int *)dst);
		if (ret != 0)
		{
			return ret;
		}

		ret = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD13,
						mmc_card_cur->RCA<<16, resp);
		ret = sdmmc_send_cmd(mmc_card_cur, mmc_contr_cur, MMC_CMD12, 0, resp);

		/* wait card ready */
		retries = 20;
		do{
			ret = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD13,
						mmc_card_cur->RCA<<16, resp);
			if(ret != 1)
				continue;

			if( (mmc_card_cur->response[0] & 0x100)
						&& ((mmc_card_cur->response[0] >> 9) & 0xf) <= 4){
				break;
			}
		}while(--retries >= 0);
	}
}

unsigned long mmc_bwrite(int dev_num, ulong start_block, ulong blkcnt, ulong *src)
{
	int retry=3;
	unsigned long ret;
	int retries = 20;
	unsigned int resp[4];
	mmc_card_data *mmc_card_cur = &cur_card_data[dev_num];
	mmc_controller_data *mmc_contr_cur = &cur_controller_data[dev_num];

	while(retry--){
		ret = (unsigned long)nufront_mmc_write_sect((unsigned int *)src,(blkcnt * MMCSD_SECTOR_SIZE),
					&cur_controller_data[dev_num], &cur_card_data[dev_num],start_block);
		if (ret != 0)
		{
			return ret;
		}

		ret = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD13,
						mmc_card_cur->RCA<<16, resp);
		ret = sdmmc_send_cmd(mmc_card_cur, mmc_contr_cur, MMC_CMD12, 0, resp);

		// wait card ready
		retries = 20;
		do{
			ret = sdmmc_send_cmd(mmc_card_cur,mmc_contr_cur,MMC_CMD13,
						mmc_card_cur->RCA<<16, resp);
			if(ret != 1)
				continue;

			if( (mmc_card_cur->response[0] & 0x100)
						&& ((mmc_card_cur->response[0] >> 9) & 0xf) <= 4){
				break;
			}
		}while(--retries >= 0);
	}
	return 0;
}

int mmc_read(int mmc_cont, unsigned int blknr, unsigned char *dst, int blkcnt)
{
	unsigned int read_cnt = 0;

	read_cnt = nufront_mmc_read_sect(blknr, blkcnt * 512, &cur_controller_data[mmc_cont],
				&cur_card_data[mmc_cont], (unsigned int *)dst);
	return read_cnt;
}
//==============================================================================
int mmc_write(int mmc_cont, unsigned int *src, unsigned long blknr, int blkcnt)
{
	unsigned int ret;

	ret = nufront_mmc_write_sect((unsigned int *)src, blkcnt * 512,
				&cur_controller_data[mmc_cont], &cur_card_data[mmc_cont], blknr);
	return ret;
}

int mmc_erase(int mmc_cont, unsigned int start, int size)
{
	int ret = 1;

	if (mmc_blk_dev[mmc_cont].dev == -1) {
		printf("Erase not permitted as Card on SLOT-%d"
					" not Initialized\n", mmc_cont);
	} else {
		ret = nufront_mmc_erase_sect(start, &cur_controller_data[mmc_cont],
					&cur_card_data[mmc_cont], size);
	}
	return ret;
}


//========================================================================================
int mmc_init(int slot)
{
	int ret;
    char *part_type;
	configure_controller(&cur_controller_data[slot], slot);
	ret = configure_mmc(&cur_card_data[slot], &cur_controller_data[slot]);
	if(ret !=1)
	{
		printf("Warning: configure_mmc(%d) ret %d.\n", slot, ret);
		return 1;   // no mmc found
	}
	mmc_blk_dev[slot].if_type = IF_TYPE_MMC;
	mmc_blk_dev[slot].part_type = PART_TYPE_UNKNOWN;
	mmc_blk_dev[slot].dev = cur_controller_data[slot].slot;
	mmc_blk_dev[slot].lun = 0;
	mmc_blk_dev[slot].type = 0;

	/* FIXME fill in the correct size (is set to 32MByte) */
	mmc_blk_dev[slot].blksz = MMCSD_SECTOR_SIZE;
	mmc_blk_dev[slot].lba = cur_card_data[slot].size;
	mmc_blk_dev[slot].removable = 0;
	mmc_blk_dev[slot].block_read = mmc_bread;
	mmc_blk_dev[slot].block_write = mmc_bwrite;
    init_part(&mmc_blk_dev[slot]);

    mmc_blk_dev[slot].lba = cur_card_data[slot].size;
    switch(mmc_blk_dev[slot].part_type)
    {
    #if CONFIG_DOS_PARTITION
        case PART_TYPE_DOS:
            part_type = "DOS"; break;
    #endif
    #if CONFIG_EFI_PARTITION
        case PART_TYPE_EFI:
            part_type = "EFI"; break;
    #endif
        default:
            part_type = "UNKNOWN"; break;
    }
    printf("mmc%d size: %dMB, %d sectors, partition type %s\n", slot,
        mmc_blk_dev[slot].lba >> (20-9), mmc_blk_dev[slot].lba, part_type);
	return 0;
}

int mmc_get_cid(int slot, unsigned int cid[4])
{
	memcpy(cid,  cur_card_data[slot].CID, 16);
	return 0;
}
//=========================================================================================
