#ifndef BATTARY_H
#define BATTARY_H

#define PRESS_DELAY	1000*100
#define LOGO_SHOW_DELAY	5
#define LONG_PRESS	10
#define IDLE_DELAY	100

#define RICOH583_REG_ADCCNT1    0x70
#define RICOH583_REG_ADCCNT2    0x71
#define RICOH583_REG_ADCCNT3    0x72
#define RICOH583_REG_VBATDATAH  0x74
#define RICOH583_REG_VBATTHL    0x80

#define RICOH583_AUTOTIM_250MS  0x0
#define RICOH583_AUTOTIM_500MS  0x1
#define RICOH583_AUTOTIM_1S             0x2
#define RICOH583_AUTOTIM_2S             0x3
#define RICOH583_AUTOTIM_4S             0x4
#define RICOH583_AUTOTIM_8S             0x5
#define RICOH583_AUTOTIM_16S    0x6
//if platform data is NULL, use the defualt value
#define RICOH583_ALARM_MVOLTS   3660 //15%
#define RICOH583_POWER_OFF_MVOLTS       3600
#define RICOH583_ADC_VDD_MVOLTS         2800

#define RICOH583_MIN_MVOLTS 3600
#define RICOH583_MAX_MVOLTS 4160
#define RICOH583_ADC_TIMEOUT (1 * HZ)

#define ADRQ_MASK       0x30
#define ADRQ_POS        4

enum ricoh583_adc_channel {
	RICOH583_ADC_CHANNEL_VBAT,
	RICOH583_ADC_CHANNEL_VIN3,
	RICOH583_ADC_CHANNEL_VIN8,
	RICOH583_ADC_CHANNEL_AIN1,//pad ref used
	RICOH583_ADC_CHANNEL_AIN2,
	RICOH583_ADC_CHANNEL_AIN3,
};

enum ricoh583_adc_mode{
	ADC_MODE_OFF,
        ADC_MODE_SINGLE,
        ADC_MODE_AUTO
};

int ricoh583_init_battery(enum ricoh583_adc_channel channel);
int ricoh583_get_mvolts(enum ricoh583_adc_channel channel);
void ricoh583_poweroff(void);
int read_battary_volt(void);

#endif
