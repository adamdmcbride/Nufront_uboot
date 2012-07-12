/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#ifndef TIMER_LOAD_VAL
# define TIMER_LOAD_VAL 0xffffffff
#endif

/* macro to read the 32 bit timer */
#define READ_TIMER (*(volatile ulong *)(CONFIG_SYS_TIMERBASE+4))

static ulong timestamp;
static ulong lastdec;

#define TIMER_ENABLE	(1 << 7)
#define TIMER_MODE_MSK	(1 << 6)
#define TIMER_MODE_FR	(0 << 6)
#define TIMER_MODE_PD	(1 << 6)

#define TIMER_INT_EN	(1 << 5)
#define TIMER_PRS_MSK	(3 << 2)
#define TIMER_PRS_8S	(1 << 3)
#define TIMER_SIZE_MSK	(1 << 2)
#define TIMER_ONE_SHT	(1 << 0)

int timer_init (void)
{
	/*
	 * Set clock frequency in the system controller:
	 *	VERSATILE_REFCLK is 32KHz
	 *	VERSATILE_TIMCLK is 1MHz
	 */
	*(volatile unsigned int *)(VERSATILE_SCTL_BASE) |=
		((VERSATILE_TIMCLK << VERSATILE_TIMER1_EnSel) | (VERSATILE_TIMCLK << VERSATILE_TIMER2_EnSel) |
		 (VERSATILE_TIMCLK << VERSATILE_TIMER3_EnSel) | (VERSATILE_TIMCLK << VERSATILE_TIMER4_EnSel));
	/*
	 * Now setup timer0
	 */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0) = CONFIG_SYS_TIMER_RELOAD;	/* TimerLoad */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 4) = CONFIG_SYS_TIMER_RELOAD;	/* TimerValue */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8) |= 0x82;			/* Enabled,
									 * free running,
									 * no interrupt,
									 * 32-bit,
									 * wrapping
									 */
	reset_timer_masked();

	return 0;
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	tmo = usec/1000;

	tmp = get_timer (0);		/* get current timestamp */

	if( (tmo + tmp + 1) < tmp )	/* if setting this forward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER/1000;	/* capure current decrementer value time */
	timestamp = 0;			/* start "advancing" time stamp from 0 */
}

/* delay x useconds AND perserve advance timstamp value */
/* ASSUMES timer is ticking at 1 msec			*/
/* ASSUMES 1MHz timer */
ulong get_timer_masked (void)
{
	ulong now = READ_TIMER/1000;	/* current tick value @ 1 tick per msec */

	if (lastdec >= now) {		/* normal mode (non roll) */
		/* normal mode */
		timestamp += lastdec - now; /* move stamp forward with absolute diff ticks */
	} else {
		/* we have overflow of the count down timer */
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

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;		/* find number of "ticks" to wait to achieve target */
		tmo /= 1000;		/* finish normalize. */
	} else {			/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000*1000);
	}

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
