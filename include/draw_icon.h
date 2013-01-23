#ifndef DRAW_ICON_H
#define DRAW_ICON_H

#define LOGO_H	300
#define LOGO_W	200
#if 0
#define LOGO_INTERVAL	1000000
/*min interval for check power key, Example:100000 means 100ms*/
#define MIN_INTERVAL	100000
/*LONG_PRESS used for distinguish long press and short press.
<= LONG_PRESS means short press, > LONG_PRESS means long press.
short press used for suspend and resume lcd, Example: LONG_PRESS set to 10  means short press at MIN_INTERVAL*2ms~MIN_INTERVAL*10ms.
long press used for exit uboot and poweron kernel, Example: LONG_PRESS set to 10 means long press in the range of 10*MIN_INTERVALms~6000ms(over 6000ms will power off)*/
#define LONG_PRESS	5
/*logo change interval used for fix frequence of logo change, Example: 10 means 10 * MIN_INTERVAL*2 ms*/
#define LOGO_CHANGE_INTERVAL	10
/*SUSPEND_TIME used for if no powerkey event input, after how many times the device will suspend. Example: SUSPEND_TIME set to 100 means after SUSPEND_TIME*MIN_INTERVAL*2ms device will suspend*/
#define SUSPEND_TIME		100
#endif
#define BLACK	0x0000
#define WHITE	0xFFFF
#define BLUE	0x00FF
#define RED	0x9900

extern void draw_init(unsigned short*fbr,unsigned sh,unsigned sw);
extern int draw_bat_logo(unsigned icon_h,unsigned icon_w,int status,int ischarging);
extern void clear_logo(void);
#endif
