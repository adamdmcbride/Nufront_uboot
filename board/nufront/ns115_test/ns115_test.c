/*
 */
#include <common.h>
#include <netdev.h>

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
	DECLARE_GLOBAL_DATA_PTR;


	gd->bd->bi_arch_number = MACH_TYPE_NS115_TEST;
	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x00000100;

	gd->flags = 0;

	icache_enable ();
	rvpb_timer_init();

	//enable lcd panel & backlight
	tmp = readl(0x06140000);
	tmp |= 0x30;
	writel(tmp,0x06140000);	
	tmp = readl(0x06140004);
	tmp |= 0x30;
	writel(tmp,0x06140004);	

	//set pwm
	writel(0x0002843a, 0x06160000);

	//set lcdc0 pin mux
	writel(0x0,0x05822004);
	writel(0x0,0x05822008);
	

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

#define TIMER_MS_FACTOR 24000
#define TIMER_US_FACTOR 24

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

/*
 * Write the system control status register to cause reset
 */
void reset_cpu(ulong addr){
	*(volatile unsigned int *)(0x051B0000) = 0x0;
	*(volatile unsigned int *)(0x051B0000) = 0x1;
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

/*
 *  u32 get_board_rev() for ARM supplied development boards
 */
//ARM_SUPPLIED_GET_BOARD_REV

