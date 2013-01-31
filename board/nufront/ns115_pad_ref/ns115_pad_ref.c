/*
 */
#include <common.h>
#include <netdev.h>
#include <asm/arch/gpio.h>
#include <search.h>
#include <nusmart_i2c.h>
#include <prcm.h>
#include "battary.h"

static ulong timestamp;
static ulong lastdec;

#define READ_TIMER (*(volatile ulong *)(CONFIG_SYS_TIMERBASE+4))

static void flash__init (void);
static void rvpb_timer_init(void);

#if defined(nONFIG_SHOW_BOOT_PROGRESS)
void show_boot_progress(int progress)
{
    printf("Boot reached stage %d\n", progress);
}
#endif

#define COMP_MODE_ENABLE ((unsigned int)0x0000EAEF)

static char bootargs_recovery[] = "set bootargs root=/dev/mmcblk1p8 rw rootwait mem=776M console=ttyS0,115200 init=/init video=nusmartfb:1024x600-${dispformat}";
static char bootcmd_recovery[] = "run default_bootargs;ext4load mmc 1:8 0x80007fc0 uImage_recovery;ns115 cpu volt 1230000;bootm";
static char bootargs_charge[] = "set bootargs root=/dev/mmcblk1p2 rw rootwait mem=776M console=ttyS0,115200 init=/init video=nusmartfb:1024x600-${dispformat} androidboot.mode=charger";

#define NUFRONT_LCD1_BASE 0xB4800000
static unsigned int recovery_flag = 0;
unsigned int fastboot_flag = 0;
extern int do_ext4_load(cmd_tbl_t *cmdtp, int flag, int argc,char *const argv[]);


static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	volatile unsigned int tmp = 0;
	int volt = 0;
	DECLARE_GLOBAL_DATA_PTR;


	gd->bd->bi_arch_number = MACH_TYPE_NS115_PAD_REF;
	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x80000100;

//	gd->flags = 0;

	icache_enable ();
	rvpb_timer_init();
	
	
	writel(0x1,0x05042c00);
	writel(0x10000000,0x05042c04);
	//set lcd pinmux 
	writel(0x0,0x05822004);    
	writel(0x01000000,0x05822008);

	//enable the 3v3
	gpio_pinmux_config(0);
	nufront_set_gpio_value(0,1);

	memset (NUFRONT_LCD1_BASE, 0x0, 0x00200000);
	gpio_pinmux_config(86);         //CAM_PWR_EN;gpioc22
	nufront_set_gpio_value(86,1);


	gpio_pinmux_config(85);         //VDD_5V_EN;gpioc21
	nufront_set_gpio_value(85,1);

#ifdef CONFIG_FASTBOOT_RECOVERY
	unsigned int key_vol_up,key_vol_down;
	key_vol_up = nufront_get_gpio_value(20);	//
	key_vol_down = nufront_get_gpio_value(18);
	if(key_vol_up == 0x0){
		fastboot_flag = 1;
	} else if(key_vol_down == 0x0){
		recovery_flag = 1;
	}
#endif
	return 0;
}

/* We know all the init functions have been run now */
int board_eth_init(bd_t *bis)
{
	/* 
	 * Currently ASSUMES only one device 
	 */
#if defined(CONFIG_DRIVER_SMC_RV)
	smc_rv_initialize(bis, CONFIG_SMC_RV_BASE);
#endif

#if defined(CONFIG_DRIVER_NUSMART_EMAC)
	emac_initialize(bis);
#endif

	return 0;
}


int misc_init_r (void)
{
	setenv("verify", "n");
	return (0);
}

/******************************
Routine:
Description:
 ******************************/
int dram_init (void)
{
	return 0;
}

/*
 * Start the timer
 * U-Boot expects a 32 bit timer, running at CFG_HZ == 1000
 * RealView Emulation BaseBoard provides 1 MHz timer
 */
/* Stub the global function */
int timer_init(void)
{
	return 0;
}

#define TIMER_MS_FACTOR 12000 //24000
#define TIMER_US_FACTOR   12 //24

static void rvpb_timer_init(void)
{
	/*
	 * Set clock frequency in system controller:
	 *	TIMER Clock is 25MHz
	 */


	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8) = 0;	//disable timer 

	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0) = CONFIG_SYS_TIMER_RELOAD; /* TimerLoad */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8) = 0x5;		/* Enabled,
	* free running,
	* no interrupt,
									 */
	reset_timer_masked();
}

int interrupt_init (void){
	return 0;
}

static inline void usleep(volatile int i) {
    for(; i >= 0; i--) ;
}
/*
 * Write the system control status register to cause reset
 */
void reset_cpu(ulong addr){
	*(volatile unsigned int *)(0x05821004) = 0x0;
	udelay(10);
	*(volatile unsigned int *)(0x05821004) = 0x1;
}

/* delay x useconds AND perserve advance timstamp value */
/* ASSUMES timer is ticking at 1 msec			*/
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	tmo = usec/1000;	//1000ms,1s

	tmp = get_timer (0);		/* get current timestamp */

	if( (tmo + tmp + 1) < tmp )	/* if setting this forward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER/TIMER_MS_FACTOR;  /* capure current decrementer value time */
	timestamp = 0;	       	    /* start "advancing" time stamp from 0 */
}

/* ASSUMES 1MHz timer */
ulong get_timer_masked (void)
{
	ulong now = READ_TIMER/TIMER_MS_FACTOR;	/* current tick value @ 1 tick per msec */

	if (lastdec >= now) {		/* normal mode (non roll) */
		/* normal mode */
		timestamp += lastdec - now; /* move stamp forward with absolute diff ticks */
	} else {			/* we have overflow of the count down timer */
		/* nts = ts + ld + (TLV - now)
		 * ts=old stamp, ld=time that passed before passing through -1
		 * (TLV-now) amount of time after passing though -1
		 * nts = new "advancing time stamp"...it could also roll and cause problems.
		 */
		timestamp += lastdec + TIMER_LOAD_VAL - now;
	}
	lastdec = now;

	return timestamp;
}

#define NS115_I2C1_BASE 0x06110000
int  power_key_status()
{
	unsigned char rbuf[4];
	unsigned char wbuf[4];
	unsigned char rbuf_11[4];
	unsigned char wbuf_11[4];

	rbuf[0] = 0x00;
	wbuf[0] = 0x1b;

	rbuf_11[0] = 0x00;
	wbuf_11[0] = 0x11;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
	if(rbuf[0]  & 0x01){
		return 1;
	}
	return 0;
}

void power_key_delay()
{
	unsigned char rbuf[4];
	unsigned char wbuf[4];
	unsigned char rbuf_11[4];
	unsigned char wbuf_11[4];

	rbuf[0] = 0x00;
	wbuf[0] = 0x1b;

	rbuf_11[0] = 0x00;
	wbuf_11[0] = 0x11;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
	dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf_11,1,rbuf_11,1);
	printf("rbuf[0] = 0x%x %d\n",rbuf[0],__LINE__);
	printf("rbuf_11[0] = 0x%x %d\n",rbuf_11[0],__LINE__);
	if((rbuf_11[0] & 0x01) == 0x01){
		if((!(rbuf[0]  & 0x01))) 
		{
			ricoh583_poweroff();
		}
		udelay(1000000);
		dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
		if((!(rbuf[0]  & 0x1)))
//		if((!(rbuf[0]  & 0x01)) && (rbuf_11[0] & 0x01) == 0x01)
		{
			ricoh583_poweroff();
		}
	} else {
		return 0;
	}
}

int detect_charge_status()
{
	unsigned char rbuf[4];
	unsigned char wbuf[4];
	unsigned char rbuf_12[4];
	unsigned char wbuf_12[4];
	unsigned int ret = 0;
	unsigned int charge_flag = 0;

	rbuf[0] = 0x00;
	wbuf[0] = 0x1b;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
	if(ret){
		printf("ERROR: read PMIC register failed\r\n");
		return;
	}

	if(((rbuf[0]  & 0x2) == 0x2))
		charge_flag = 1;
	if(((rbuf[0] & 0x2) != 0x2))
		charge_flag = 0;
	return charge_flag;
}

int charge_status()
{
	unsigned char rbuf[4];
	unsigned char wbuf[4];
	unsigned char rbuf_12[4];
	unsigned char wbuf_12[4];
	unsigned int ret = 0;
	unsigned int charge_flag = 0;

	rbuf[0] = 0x00;
	wbuf[0] = 0x11;

	rbuf_12[0] = 0x00;
	wbuf_12[0] = 0x12;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
    	if(ret){
        	printf("ERROR: read PMIC register failed\r\n");
        	return;
    	}
//	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	
	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf_12,1,rbuf_12,1);
    	if(ret){
        	printf("ERROR: read PMIC register failed\r\n");
        	return;
    	}

	//printf("rbuf[0]= 0x%x\n",rbuf[0]);
	//printf("rbuf_12[0]= 0x%x\n",rbuf_12[0]);
	if(((rbuf[0]  & 0x12) == 0x02)) 
		charge_flag = 1;
	if(((rbuf[0] & 0x2) == 0x2) && ((rbuf_12[0] & 0x80) == 0x80))
		charge_flag = 0;
	return charge_flag;
		
}

unsigned int read_boot_env(void)
{
        int ret,selboot = 0;
        struct hsearch_data env_htab;
        struct bootsel bootsel;
        const char *addr = 0x80007fc0;
        char sep = '\n';
        unsigned int  size;
	unsigned int volt = 0;

    	extern unsigned int _xloader_boot_mode;
    
	if(fastboot_flag || _xloader_boot_mode == 0x02){
		printf("fastboot mode\n");
		do_fastboot(NULL,0,1,do_fastboot);
		return 0;
	} else if(recovery_flag){
		setenv("default_bootargs",bootargs_recovery);
		setenv("bootcmd",bootcmd_recovery);
		printf("recovery mode\n");
		return 0;
	} else {
		char *ext4_load[5]  = { "do_ext4_load","mmc","1:7","0x80007fc0","boot_config", };
		size = do_ext4_load(NULL,0,5,ext4_load);
                if (size == 1){
                        return -1;
		}
                hread_buf(&env_htab,addr,size,sep,&bootsel);
//	        printf("bootsystem = %s\n",bootsel.system);
//            	printf("bootdevice = %s\n",bootsel.device);
//		printf("bootsel.data = %s\n",bootsel.data);
//		printf("bootsel.cache = %s\n",bootsel.cache);
	        selboot=(strncmp(bootsel.system," recovery",strlen(" recovery"))&& strncmp(bootsel.data," yes",strlen(" yes")) 
			&& strncmp(bootsel.cache," yes",strlen(" yes")) && strncmp(bootsel.device," yes",strlen(" yes")));
		printf("selboot = %d\n",selboot);
		if((selboot == 0)){
                        setenv("default_bootargs",bootargs_recovery);
			setenv("bootcmd", bootcmd_recovery);
                }       
		return selboot;
        } 
}

/*
 *  u32 get_board_rev() for ARM supplied development boards
 */
//ARM_SUPPLIED_GET_BOARD_REV

