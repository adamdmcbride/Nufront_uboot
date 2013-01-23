/*
 * Battery driver for ricoh583 PMIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <command.h>
#include <common.h>
#include <nusmart_i2c.h>
#include <battary.h>

#define NS115_I2C1_BASE 0x06110000

static int __ricoh583_read(unsigned char reg,unsigned char *read_val)
{
	int ret;
	unsigned char wbuf[3],rbuf[3];
	unsigned char wlen = 1, rlen = 1;

	wbuf[0] =(unsigned char) reg;

	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,wlen,read_val,rlen);
	if (ret ) {
		TRACE(KERN_ERROR,"I2C read error ret = %d\n",ret);
		return ret;
	}

	TRACE(KERN_DEBUG,"I2C read reg = 0x%x rbuf = 0x%x\n",wbuf[0],rbuf[0]);
	return 0;
}

static  int __ricoh583_write(unsigned char reg,unsigned char reg_val)
{
	int ret;
	unsigned char wbuf[3];
	unsigned char len = 2;

	wbuf[0] = reg;
	wbuf[1] = reg_val;

	ret = dw_i2c_send_bytes(NS115_I2C1_BASE,wbuf,len);
	if (ret < 0) {
		TRACE(KERN_ERROR,"failed writing 0x%02x to 0x%02x\n",
		wbuf[1], wbuf[0]);
		return ret;
	}

	TRACE(KERN_DEBUG,"I2C write val = 0x%x to = 0x%x\n",wbuf[1],wbuf[0]);
	return 0;
}

int ricoh583_update(unsigned char reg,unsigned char val,unsigned char mask)
{
	unsigned char reg_val;
	int ret = 0;

	ret = __ricoh583_read(reg,&reg_val);

	if ((reg_val & mask) != val) {
		reg_val = (reg_val & ~mask) | (val & mask);
		ret = __ricoh583_write(reg, reg_val);
	}
	return ret;
}



static int ricoh583_adc_mode_set(enum ricoh583_adc_mode mode)
{
	int ret;

	ret =ricoh583_update(RICOH583_REG_ADCCNT3, mode << ADRQ_POS, ADRQ_MASK);

	return ret;
}

int ricoh583_init_battery(enum ricoh583_adc_channel channel)
{
	int ret = 0;
	unsigned char val = 0;

	if (channel <= RICOH583_ADC_CHANNEL_VIN8){
		val = 1 << (channel + 3);
	}else{
		val = 1 << (channel - 3);
	}
	ret |= __ricoh583_write(RICOH583_REG_ADCCNT1, val); //disable auto-mode for channel
	val = channel;
	ret |= __ricoh583_write(RICOH583_REG_ADCCNT3, val); //one time, channel
	if (ret < 0){
		TRACE(KERN_ERROR, "%s: ricoh583 write error:%d\n", __func__, ret);
		return -1;
	}
	return ret;
}

int ricoh583_get_mvolts(enum ricoh583_adc_channel channel)
{
	unsigned char data_l = 0, data_h = 0;
	unsigned char adc_reg = RICOH583_REG_VBATDATAH + channel * 2;
	int i, ret, data;
	int adc_vdd_mvolts = 2800;
	int multiple = 300;

	ricoh583_adc_mode_set(ADC_MODE_SINGLE);
	for (i = 0; i < 5; i++){
		udelay(5);
		__ricoh583_read(0x96, &data_h);
		if (data_h & 0x01){
			i = 5;
		}
	}
	__ricoh583_read(adc_reg, &data_h);
	__ricoh583_read(adc_reg + 1, &data_l);
	data = ((data_h & 0xff) << 4) | (data_l & 0x0f);
	data = data * adc_vdd_mvolts / 4095;
	data = data * multiple / 100;
	if (channel <= RICOH583_ADC_CHANNEL_VIN8){
			data = data * 3;
	}

	return data;
}

void ricoh583_poweroff(void)
{
	unsigned char wbuf[2];
	wbuf[0] = 0x13;
	wbuf[1] = 0x01;
	dw_i2c_send_bytes(NS115_I2C1_BASE,wbuf,2);
}

int  read_battary_volt(void)
{
	int volt = 0;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	ricoh583_init_battery(RICOH583_ADC_CHANNEL_AIN1);
	volt = ricoh583_get_mvolts(RICOH583_ADC_CHANNEL_AIN1);
	printf("volt = %d \n",volt);

	return  volt;
}
