
#define	REG_WRITE(value, addr)	(*((volatile unsigned int*)(addr))) = (value)
#define REG_READ(addr)  (*((volatile unsigned int*)(addr)))

#define GIC_CPU_CTRL                    0x00
#define GIC_CPU_PRIMASK                 0x04
#define GIC_CPU_BINPOINT                0x08
#define GIC_CPU_INTACK                  0x0c
#define GIC_CPU_EOI                     0x10
#define GIC_CPU_RUNNINGPRI              0x14
#define GIC_CPU_HIGHPRI                 0x18
        
#define GIC_DIST_CTRL                   0x000
#define GIC_DIST_CTR                    0x004
#define GIC_DIST_ENABLE_SET             0x100
#define GIC_DIST_ENABLE_CLEAR           0x180
#define GIC_DIST_PENDING_SET            0x200
#define GIC_DIST_PENDING_CLEAR          0x280
#define GIC_DIST_ACTIVE_BIT             0x300 
#define GIC_DIST_PRI                    0x400
#define GIC_DIST_TARGET                 0x800
#define GIC_DIST_CONFIG                 0xc00
#define GIC_DIST_SOFTINT                0xf00

#define SCU_CTRL                0x00
#define SCU_CONFIG              0x04
#define SCU_CPU_STATUS          0x08
#define SCU_INVALIDATE          0x0c
#define SCU_FPGA_REVISION       0x10

void scu_enable(unsigned int scu_base)
{
        unsigned int scu_ctrl;

        scu_ctrl = REG_READ(scu_base + SCU_CTRL);
        scu_ctrl |= 1;
        REG_WRITE(scu_ctrl, scu_base + SCU_CTRL);   
}

void gic_cpu_init(unsigned int base)
{     
        REG_WRITE(0xf0, base + GIC_CPU_PRIMASK);
        REG_WRITE(1, base + GIC_CPU_CTRL);
}

void gic_dist_init(unsigned int base)
{
        unsigned int cpumask = 3;
	unsigned int max_irq = 0;
	unsigned int i = 0;


        cpumask |= cpumask << 8;
        cpumask |= cpumask << 16;

        REG_WRITE(0, base + GIC_DIST_CTRL);

        /*
         * Find out how many interrupts are supported.
         */
        max_irq = REG_READ(base + GIC_DIST_CTR) & 0x1f;
        max_irq = (max_irq + 1) * 32;

        /*
         * The GIC only supports up to 1020 interrupt sources.
         * Limit this to either the architected maximum, or the
         * platform maximum.
         */
        //if (max_irq > max(1020, NR_IRQS))
        //        max_irq = max(1020, NR_IRQS);

        /*
         * Set all global interrupts to be level triggered, active low.
         */
        for (i = 32; i < max_irq; i += 16)
                REG_WRITE(0, base + GIC_DIST_CONFIG + i * 4 / 16);
	

        /*
         * Set all global interrupts to this CPU only.
         */
        for (i = 32; i < max_irq; i += 4) 
                REG_WRITE(cpumask, base + GIC_DIST_TARGET + i * 4 / 4);

        REG_WRITE(0x03030201, base + GIC_DIST_TARGET + 19 * 4 );
        /*
         * Set priority on all interrupts.
         */
        for (i = 0; i < max_irq; i += 4)
                REG_WRITE(0xa0a0a0a0, base + GIC_DIST_PRI + i * 4 / 4);
    /*
         * Disable all interrupts.
         */
        for (i = 0; i < max_irq; i += 32)
                REG_WRITE(0xffffffff, base + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);

        REG_WRITE(1, base + GIC_DIST_CTRL);
}

unsigned int snop_timer_1()
{
	REG_WRITE(0,0x05100008 + 0x14);


	REG_WRITE(1000, 0x05100000 + 0x14);
	REG_WRITE(1, 0x05100008 + 0x14);
	return 0;
}

unsigned int set_time_1()
{
	scu_enable(0x05040000);
	gic_dist_init(0x05041000);
        gic_cpu_init(0x05040100);
        snop_timer_1();
}

