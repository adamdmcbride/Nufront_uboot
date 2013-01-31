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
#include <asm/arch/gpio.h>

#define NS115_I2C1_BASE 0x06110000
#define NS115_MAX_VOLT_BUF 5

#define RICOH583_GPIO_PGSEL	0xA3
#define RICOH583_GPIO_IOSEL	0xA0
#define RICOH583_GPIO_IOOUT	0xA2
#define RICOH583_GPIO_MON_IOIN	0xAB

extern ulong get_timer (ulong base);

//#define DEBUG_EN
#ifdef DEBUG_EN
#define  PDBG(format,...)	\
	printf(format, ##__VA_ARGS__)
#define  PINFO(format,...)	\
	printf(format, ##__VA_ARGS__)
#else
#define  PDBG(format,...)	do{}while(0)
#define  PINFO(format,...)	\
	printf(format, ##__VA_ARGS__)
#endif
#define PERR(format,...)	\
	printf(format, ##__VA_ARGS__)

#define CONFIG_FILE_PATH "/misc/batter_config.ini"
/*add the version when modfiy the battery_parametero*/
/*It must be the same as the define in kernel*/
#define BAT_PARA_VERSION	3

typedef struct battery_parameter{
	int version;
	int check_no;
	int capacity;
	int resistor;
	int max_mAh;
	int full_vol;
}battery_parameter_t;

struct ns115_battery_mux {
	int inited;
	int batt_cc;
	int resistor_mohm;
	int init_mAs;
	int max_mAh;
	int chg_current;
	int i_term_times;
	int chg_done;
	int batt_volt_buf[NS115_MAX_VOLT_BUF];
	int batt_volt_pointer;
	int batt_volt_times;
	unsigned long record_seconds;
	unsigned long cal_seconds;
	unsigned long last_seconds;
	unsigned long seconds;
	unsigned long init_mAs_time;
};
static struct ns115_battery_mux ns115_batt;

static int __ricoh583_read(unsigned char reg,unsigned char *read_val)
{
	int ret;
	unsigned char wbuf[3],rbuf[3];
	unsigned int wlen = 1, rlen = 1;

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
	unsigned int len = 2;

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

static int ricoh583_init_adc(enum ricoh583_adc_channel channel)
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

static int ricoh583_get_mvolts(enum ricoh583_adc_channel channel)
{
	unsigned char data_l = 0, data_h = 0;
	unsigned char adc_reg = RICOH583_REG_VBATDATAH + channel * 2;
	int i, data;
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

int  read_battery_volt(void)
{
	int volt = 0;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	ricoh583_init_adc(RICOH583_ADC_CHANNEL_AIN1);
	volt = ricoh583_get_mvolts(RICOH583_ADC_CHANNEL_AIN1);

	return  volt;
}

#ifdef USB_GPIO_RICOH583
static int ricoh583_gpio_enable(int offset, int enable)
{
	unsigned char val;

	__ricoh583_read(RICOH583_GPIO_PGSEL, &val);
	if (enable){
		val &= ~(1 << offset);
	}else{
		val |= (1 << offset);
	}
	__ricoh583_write(RICOH583_GPIO_PGSEL, val);

	return 0;
}

static int ricoh583_gpio_input(int offset)
{
	unsigned char val;

	__ricoh583_read(RICOH583_GPIO_IOSEL, &val);
	val &= ~(1 << offset);
	__ricoh583_write(RICOH583_GPIO_IOSEL, val);

	return 0;
}

static int ricoh583_gpio_get(int offset)
{
	unsigned char val;
	unsigned char get_reg;

	__ricoh583_read(RICOH583_GPIO_IOSEL, &val);
	if (val & (1 << offset)){
		get_reg = RICOH583_GPIO_IOOUT;
	}else{
		get_reg = RICOH583_GPIO_MON_IOIN;
	}
	__ricoh583_read(get_reg, &val);

	return !!(val & (1 << offset));
}
#endif

/*calculate the capacity*/
static int capacity_table[][2] = {
	{100, 4160},
	{99, 4142},
	{98, 4125},
	{97, 4110},
	{96, 4102},
	{95, 4095},
	{94, 4088},
	{93, 4081},
	{92, 4070},
	{91, 4060},
	{90, 4053},
	{89, 4044},
	{88, 4038},
	{87, 4033},
	{86, 4028},
	{85, 4023},
	{84, 4017},
	{83, 4012},
	{82, 4003},
	{81, 3996},
	{80, 3990},
	{79, 3983},
	{78, 3978},
	{77, 3971},
	{76, 3965},
	{75, 3959},
	{74, 3952},
	{73, 3945},
	{72, 3940},
	{71, 3934},
	{70, 3928},
	{69, 3922},
	{68, 3915},
	{67, 3910},
	{66, 3904},
	{65, 3898},
	{64, 3893},
	{63, 3886},
	{62, 3880},
	{61, 3874},
	{60, 3866},
	{59, 3858},
	{58, 3849},
	{57, 3841},
	{56, 3835},
	{55, 3828},
	{54, 3823},
	{53, 3819},
	{52, 3815},
	{51, 3812},
	{50, 3807},
	{49, 3805},
	{48, 3802},
	{47, 3799},
	{46, 3796},
	{45, 3793},
	{44, 3791},
	{43, 3788},
	{42, 3786},
	{41, 3784},
	{40, 3781},
	{39, 3779},
	{38, 3778},
	{37, 3776},
	{36, 3775},
	{35, 3773},
	{34, 3772},
	{33, 3771},
	{32, 3770},
	{31, 3768},
	{30, 3767},
	{29, 3764},
	{28, 3761},
	{27, 3758},
	{26, 3755},
	{25, 3752},
	{24, 3748},
	{23, 3745},
	{22, 3740},
	{21, 3737},
	{20, 3735},
	{19, 3730},
	{18, 3726},
	{17, 3721},
	{16, 3715},
	{15, 3710},
	{14, 3704},
	{13, 3696},
	{12, 3688},
	{11, 3683},
	{10, 3681},
	{9, 3679},
	{8, 3678},
	{7, 3674},
	{6, 3670},
	{5, 3656},
	{4, 3623},
	{3, 3600},
	{2, 3575},
	{1, 3550},
	{0, 3500}
};

static int ns115_battery_get_config(battery_parameter_t *para)
{
	int size;
	const char *addr = 0x80007fc0;
	const char * filename = CONFIG_FILE_PATH;
	char *ext4_load[5]  = {"do_ext4_load", "mmc", "1:5", "0x80007fc0", filename,};

	if(!para){
		return -1;
	}

	size = do_ext4_load(NULL, 0, 5, ext4_load);
	if (size == 1){
		PERR("%s: read error!\n", __func__);
		return -1;
	}
	PINFO("read %s %dbytes\n", filename, size);
	memcpy(para, addr, size);

	return 0;
}

static int ns115_battery_set_config(battery_parameter_t *para)
{
	int size;
	char size_str[4];
	const char *addr = 0x80007fc0;
	const char * filename = CONFIG_FILE_PATH;
	char *ext4_update[6]  = {"do_ext4_write","mmc","1:5",filename,"0x80007fc0", size_str,};

	if(!filename || !para)
		return -1;
	sprintf(size_str, "%d", sizeof(battery_parameter_t));
	memcpy(addr, para, sizeof(battery_parameter_t));

	size = do_ext4_write(NULL, 0, 6, ext4_update);
	if (size == 1){
		PERR("%s: write error!\n", __func__);
		return -1;
	}
	PINFO("write %s OK!\n", filename);

	return 0;
}

static unsigned long ns115_get_seconds(void)
{
	unsigned long seconds;

	seconds = get_timer(0) / 1000;
	if (seconds < ns115_batt.last_seconds){
		ns115_batt.seconds += 357;
	}
	ns115_batt.last_seconds = seconds;
	seconds += ns115_batt.seconds;

	PDBG("seconds: %ds\n", seconds);

	return seconds;
}

static int is_usb_charge(void)
{
	int is_usb, val;

#ifdef USB_GPIO_WAKEUP
	wkgpio_enable(1);
	wkgpio_direction_input(USB_CHG_GPIO);
	val = get_wkgpio_value(USB_CHG_GPIO);
	is_usb = (val == USB_CHG_GPIO_EFFECT);
#endif
#ifdef USB_GPIO_RICOH583
	ricoh583_gpio_enable(USB_CHG_GPIO, 1);
	ricoh583_gpio_input(USB_CHG_GPIO);
	val = ricoh583_gpio_get(USB_CHG_GPIO);
	is_usb = (val == USB_CHG_GPIO_EFFECT);
#endif

	PINFO("charger type is usb: %d\n", is_usb);
	return is_usb;
}

static int get_average_voltage(int mvolts)
{
	int new_mvolts = mvolts;
	int i, sum;

	PDBG("votage before Calibration: %d\n", mvolts);

	if (ns115_batt.batt_volt_times++ >= NS115_MAX_VOLT_BUF){
		ns115_batt.batt_volt_times = NS115_MAX_VOLT_BUF;
	}
	ns115_batt.batt_volt_buf[ns115_batt.batt_volt_pointer] = new_mvolts;
	if (++ns115_batt.batt_volt_pointer >= NS115_MAX_VOLT_BUF){
		ns115_batt.batt_volt_pointer = 0;
	}

	for (i = 0, sum = 0; i < ns115_batt.batt_volt_times; ++i){
		sum += ns115_batt.batt_volt_buf[i];
	}
	new_mvolts = sum / ns115_batt.batt_volt_times;

	return new_mvolts;
}
static int get_open_circuit_volt(int vbat)
{
	int ocv ;
	int resistor = ns115_batt.resistor_mohm;
	int cur;

	cur = SYS_CONSUMPTION * 1000 / vbat;
#if CHARGER_NO_DELAY
	cur -= ns115_batt.chg_current;
#endif
	ocv = vbat + cur * resistor / 1000;
	PINFO("vbat: %d ocv: %d\n", vbat, ocv);

	return ocv;
}

static int ns115_get_batt_vol_cc(int mvolts)
{
	int i;
	int table_size = sizeof(capacity_table) / (sizeof(int) * 2);

	for (i = table_size -1; i >= 0; --i){
		if (mvolts < capacity_table[i][1]){
			 return capacity_table[i][0];
		}
	}

	return 100;
}

static int ns115_get_batt_cc_vol(int cc)
{
	int i;
	int table_size = sizeof(capacity_table) / (sizeof(int) * 2);

	for (i =  0; i < table_size; ++i){
		if (cc >= capacity_table[i][0]){
			 return capacity_table[i][1];
		}
	}

	return 3400;
}

int ns115_calbt_mah_full_vol(int full_mAh, int full_vol)
{
	int vol_offset;
	int i;

	ns115_batt.max_mAh = full_mAh;
	PINFO("calibate battery max_mAh: %d\n", full_mAh);
	capacity_table[0][1] = full_vol;
	PINFO("calibate capacity table[100:%d]\n", full_vol);

	for (i = 1; capacity_table[i - 1][1] <= capacity_table[i][1]; ++i){
		vol_offset = capacity_table[i][1] - capacity_table[i + 1][1];
		capacity_table[i][1] = capacity_table[i - 1][1] - vol_offset / 2;
		PINFO("calibate capacity table[%d:%d]\n",
				capacity_table[i][0], capacity_table[i][1]);
	}

	return 0;
}

static int fixed_capacity_from_record(battery_parameter_t *record)
{
	int ret;
	int check_no;

	ret = ns115_battery_get_config(record);
	if(ret){
		goto err;
	}
	if (record->version != BAT_PARA_VERSION){
		PERR("%s: battery_parameter version error!\n", __func__);
		goto err;
	}
	check_no = record->capacity ^ record->resistor
		^ record->max_mAh ^ record->full_vol;
	if (record->check_no != check_no){
		PERR("%s: the check numbers is error!\n", __func__);
		goto err;
	}
	PINFO("%s: max_mAh: %d full_vol: %d capacity: %d%%\n", __func__,
			record->max_mAh, record->full_vol, record->capacity);
	ns115_calbt_mah_full_vol(record->max_mAh, record->full_vol);

	PINFO("%s: resistor: %dmohm\n", __func__, record->resistor);
	ns115_batt.resistor_mohm = record->resistor;

	return 0;
err:
	PERR("%s: don't use the record battery parameter!\n", __func__);
	return -1;
}

static int ns115_battery_init(int vol)
{
	int ret;
	int cc, diff_cc;
	unsigned long cur_time;
	battery_parameter_t record;

	if (is_usb_charge()){
		ns115_batt.chg_current = USB_CHG_CURRENT;
	}else{
		ns115_batt.chg_current = AC_CHG_CURRENT;
	}

	ret = fixed_capacity_from_record(&record);
	if (ret){
		ns115_batt.max_mAh = BATT_MAX_MAH;
		ns115_batt.resistor_mohm = BATT_RESISTOR;
	}
	vol = get_open_circuit_volt(vol);
	cc = ns115_get_batt_vol_cc(vol);
	PINFO("get vol cc: %d%%\n", cc);
	if (!ret){
		diff_cc = record.capacity - cc;
		if ((diff_cc >= 0 && diff_cc <= 25) || (diff_cc <= 0 && diff_cc >= -35)){
			cc = record.capacity;
		}
	}

	ns115_batt.batt_cc = cc;
	ns115_batt.init_mAs = ns115_batt.max_mAh * cc * 36;
	ns115_batt.seconds = 0;
	ns115_batt.last_seconds = 0;
	cur_time = ns115_get_seconds();
	ns115_batt.init_mAs_time = cur_time;
	ns115_batt.record_seconds = cur_time;
	ns115_batt.cal_seconds = cur_time;
	ns115_batt.i_term_times = 0;
	ns115_batt.chg_done = 0;
	ns115_batt.batt_volt_pointer = 0;
	ns115_batt.batt_volt_times = 0;
	ns115_batt.inited = 1;

	PINFO("init_cc: %d%% init_mAs: %dmAh init_time: %ds\n",
			ns115_batt.batt_cc, ns115_batt.init_mAs / 3600,
			(int)ns115_batt.init_mAs_time);

	return 0;
}

static int ns115_save_capacity(void)
{
	battery_parameter_t para;

	para.version = BAT_PARA_VERSION;
	para.capacity = ns115_batt.batt_cc;
	para.resistor = ns115_batt.resistor_mohm;
	para.max_mAh = ns115_batt.max_mAh;
	para.full_vol = capacity_table[0][1];

	para.check_no = para.capacity ^ para.resistor ^ para.max_mAh ^ para.full_vol;
	if(ns115_battery_set_config(&para) < 0){
		PDBG("fatal error:%s:set config error!!\n", __func__);
		return -1;
	}
	PDBG("%s:save resistor[%dmohm] capcacity[%d%%]!\n",
			__func__, para.resistor, para.capacity);
	PDBG("%s: save max_mAh[%dmAh] full_vol[%dmV]\n",
			__func__, para.max_mAh, para.full_vol);

	return 0;
}

static int ns115_calculate_current(int vbat)
{
	int ocv, cur;

	ocv = ns115_get_batt_cc_vol(ns115_batt.batt_cc);
	cur = (ocv - vbat) * 1000 / ns115_batt.resistor_mohm;
	PDBG("ocv:%dmV vbat:%dmV\n", ocv, vbat);
	PDBG("resistor:%dmohm current:%dmA\n", ns115_batt.resistor_mohm, cur);

	return cur;
}

static int ns115_get_batt_mAs(int vbat)
{
	int cal_time;
	int cur, cur_mAs, cal_mAs;
	unsigned long cur_time;

	cur_time = ns115_get_seconds();
	cal_time = cur_time - ns115_batt.init_mAs_time;
	cur = ns115_calculate_current(vbat);

	cal_mAs = cal_time * cur;
	cur_mAs = ns115_batt.init_mAs - cal_mAs;
	ns115_batt.init_mAs_time = cur_time;

	if (!ns115_batt.chg_done && ns115_batt.batt_cc > 90 && -cur < ns115_batt.chg_current / 10){
		if (++ns115_batt.i_term_times < 10){
			goto done;
		}
		ns115_batt.chg_done = 1;
	}else{
		ns115_batt.i_term_times = 0;
	}

done:
	PDBG("batt cur_mAh: %dmAh cal_mAs: %dmAs cal_time: %ds\n",
			cur_mAs / 3600, cal_mAs, cal_time);

	return cur_mAs;
}

int ns115_get_capacity(void)
{
	int vbat;
	int cur_mAs;
	int cc;
	unsigned long cur_time;

	vbat = read_battery_volt();
	if (!ns115_batt.inited){
		ns115_battery_init(vbat);
		goto out;
	}
	cur_time = ns115_get_seconds();
	if (cur_time - ns115_batt.cal_seconds < 5){
		goto out;
	}
	ns115_batt.cal_seconds = cur_time;

	PDBG("*************************\n");
	vbat = get_average_voltage(vbat);
	cur_mAs = ns115_get_batt_mAs(vbat);
	ns115_batt.init_mAs= cur_mAs;

	cc = (cur_mAs / 36  + ns115_batt.max_mAh - 100)
		/ ns115_batt.max_mAh;
	if (cc < 0){
		cc = 0;
	}else if (cc > 100){
		cc = 100;
		ns115_batt.chg_done = 1;
	}
	if (ns115_batt.chg_done == 1){
		cc = 100;
	}
	ns115_batt.batt_cc  = cc;
	if (cur_time - ns115_batt.record_seconds > 2 * 60){
		ns115_batt.record_seconds = cur_time;
		ns115_save_capacity();
	}

	PINFO("time: %ds. capacity: %d%%\n", cur_time, ns115_batt.batt_cc);
	PDBG("*************************\n");
out:

	return ns115_batt.batt_cc;
}
