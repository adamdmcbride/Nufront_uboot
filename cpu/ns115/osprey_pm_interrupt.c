
#define	REG_WRITE(addr, value)	(*((volatile unsigned int*)(addr))) = (value)
#define REG_READ(addr)  (*((volatile unsigned int*)(addr)))


#define	CPU_SHUTDOWN	(1)
#define	CPU_ON		(0)
#define GIC_DIST_SOFTINT                0xf00
#define	GIC_BASE_OFFSET			0x1000
#define SCM_BASE                        (0x051c0000)
#define	A9_BASE				(0x05040000)
#define SCM_WFI_ENTRY_REG               ((SCM_BASE) + 0X38)
#define SCM_CPU_STAT_REG                ((SCM_BASE) + 0X3c)

volatile unsigned int os_delay = 0;

void prcm_reg_set_bits(unsigned int addr, unsigned int set);

void gic_raise_softirq(unsigned int cpux, unsigned int irq)
{
        unsigned long map = cpux; //1: cpu0, 2: cpu1
        REG_WRITE(A9_BASE + GIC_BASE_OFFSET + GIC_DIST_SOFTINT, (map << 16) | irq);
}

void a9_timer2watchdog_handler(unsigned int ack)
{

}

unsigned int delay(unsigned int loops)
{
	os_delay = loops;
	while(os_delay--);
}

void prcm_reg_clr_bits(unsigned int addr, unsigned int clr)
{
	unsigned int reg_data = REG_READ(addr);
	reg_data &= ~(clr);	
	REG_WRITE(addr, reg_data);
}

void prcm_reg_set_bits(unsigned int addr, unsigned int set)
{
	unsigned int reg_data = REG_READ(addr);
	reg_data |= (set);	
	REG_WRITE(addr, reg_data);
}

void NeonPowerDown(unsigned int cpu_id)
{
	unsigned int bit_mask;
	if(cpu_id == 1)	
		prcm_reg_set_bits(0x051b0014, (1 << 13));
	else
		prcm_reg_set_bits(0x051b0014, (1 << 12));


	if(cpu_id == 0)
	{
		bit_mask = 10;	
	}
	else if(cpu_id == 1)
	{
		bit_mask = 11;
	}
	else
		return;
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
}

void NeonPowerUp(unsigned int cpu_id)
{
	unsigned int bit_mask;
	if(cpu_id == 1)	
		prcm_reg_set_bits(0x051b0014, (1 << 13));
	else
		prcm_reg_set_bits(0x051b0014, (1 << 12));


	if(cpu_id == 0)
	{
		bit_mask = 10;	
	}
	else if(cpu_id == 1)
	{
		bit_mask = 11;
	}
	else
		return;
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
}

void CpuPowerDown(unsigned int cpu_id)
{
	unsigned int pw_status = REG_READ(0x05040008);
	if(cpu_id == 1)	
		prcm_reg_set_bits(0x051b0014, (1 << 13));
	else
		prcm_reg_set_bits(0x051b0014, (1 << 12));

	pw_status |= (3 << (cpu_id * 8));
	REG_WRITE(0x05040008, pw_status);
}

void DbgVSOCPowerDown()
{
	unsigned int bit_mask = 16;
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
}

void  DbgVSOCPowerOn()
{
	unsigned int bit_mask = 16;
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
}

void DbgPtmPowerDown()
{
	unsigned int bit_mask = 15;
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
}

void DbgPtmPowerOn()
{
	unsigned int bit_mask = 15;
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
}

void MbistPowerDown()
{
	unsigned int bit_mask = 14;
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
}

void MbistPowerOn()
{
	unsigned int bit_mask = 14;
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
}

void Scu_pl310PowerDown()
{
	unsigned int bit_mask = 17;
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
}

void Scu_pl310PowerOn()
{
	unsigned int bit_mask = 17;
	prcm_reg_set_bits(0x051b0014, (1 << bit_mask));
	prcm_reg_clr_bits(0x051b0014, (1 << bit_mask));
}


void SetCpu1ShutDownFlags()
{
	prcm_reg_set_bits(SCM_CPU_STAT_REG, CPU_SHUTDOWN); //cpu1 shutdown: 1
}

unsigned int GetCpu1ShutDownFlags()
{
	return (REG_READ(SCM_CPU_STAT_REG) & CPU_SHUTDOWN);
}

void WaitCpu1ShutDown()
{
	while(GetCpu1ShutDownFlags() == CPU_ON);
	delay(50);
}

void WaitCpu1FromShutDownToOn()
{
	unsigned int reg_data = 0;
	while((reg_data & 0x4) == 0) //1: cpu1 on
		reg_data = REG_READ(SCM_CPU_STAT_REG);
	
}

void ShutDownCpu1OrWFI()
{
}


/**************************************************************************/
extern void enter_wfi();

void ns2816_secondary_startup()
{
	__asm__ __volatile__(
	"MRC p15,0,r0,c1,c0,1\n"
	"BIC r0,r0,#1 << 6\n"
	"MCR p15,0,r0,c1,c0,1"
	);

        scu_enable(0x05040000);
       // gic_dist_init(0x05041000);
        gic_cpu_init(0x05040100);
	//printf("cpu1 wake up success\n");
	//*(unsigned int*)0x02000000 = 0x12345678;

	//delay(5000000);
	//printf("cpu1 shutdown  ....\n");

	SetCpu1ShutDownFlags();    
	CpuPowerDown(1);	//C0_C1->C0

	

	enter_wfi();
}


void cpu1shutdown()
{
	REG_WRITE(SCM_WFI_ENTRY_REG, (unsigned int)ns2816_secondary_startup);	
	
	gic_raise_softirq(2, 1); //wakeup cpu1 from wfi



}

void SwitchOnAllPowerDomainOrShutDownCpu0()
{
	MbistPowerDown();	//ALL_ON->DBG_C0N0C1N1
	delay(10);

	DbgPtmPowerDown();	//DBG_C0N0C1N1->DBGVSOC_C0N0C1N1
	delay(10);

	DbgVSOCPowerDown();	//DBGVSOC_C0N0C1N1->C0N0_C1N1
	delay(10);

	NeonPowerDown(1);	//C0N0_C1N1->C0N0_C1
	delay(10);
	
	NeonPowerDown(0);	//C0N0_C1->C0_C1
	delay(10);

//	wakeup cpu1
	REG_WRITE(SCM_WFI_ENTRY_REG, (unsigned int)ns2816_secondary_startup);	
	
	gic_raise_softirq(1, 1); //wakeup cpu1 from wfi

	WaitCpu1ShutDown(); //cpu1 from on to shutdown
	printf("cpu1 from on to shutdown success\n");

	delay(1000);
	gic_raise_softirq(1, 1);
	WaitCpu1FromShutDownToOn(); //C0->C0_C1
		
	NeonPowerUp(0);		//C0_C1->C0N0C1
	delay(10);

	NeonPowerUp(1);		//C0N0C1->C0N0C1N1
	delay(10);

	DbgVSOCPowerOn();	//C0N0_C1N1->DBGVSOC_C0N0C1N1 
	delay(10);

	DbgPtmPowerOn();	//DBGVSOC_C0N0C1N1->DBG_C0N0C1N1
	delay(10);

	MbistPowerOn();		//DBG_C0N0C1N1->ALL_ON
	delay(10);
	printf("all_on\n");
}


