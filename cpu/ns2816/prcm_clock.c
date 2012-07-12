#include <common.h>

#if defined(CONFIG_PRCM_CLOCK)


#define PRCM_CLOCK_ENABLE_1 (volatile void*)0x051B0020
#define RSTORAGE_CCLKEN		(6)
#define RI2S_SCLKEN		(5)
#define RMALI_ACLKEN		(4)
#define RL2RAM_ACLKEN		(3)
#define RDDR_PORT0_ACLKEN	(2)
#define RDDR_PORT2_ACLKEN	(1)
#define RDMAC0_ACLKEN		(0)


#define PRCM_CLOCK_ENABLE_2 (volatile void*)0x051B0024

#define RCORESIGHT_HCLKEN 	(29)
#define RON2_HCLKEN   		(28)
#define RLCD_HCLKEN    		(27)
#define RDDRCFG_HCLKEN  	(26)
#define RUSB_HCLKEN   		(25)
#define RSATA_HCLKEN  		(24)
#define RGMAC_HCLKEN  		(23)
#define RSTORAGE_HCLKEN  	(22)
#define RDMAC1_HCLKEN   	(21)
#define RMEMCTL0_HCLKEN   	(20)
#define RMEMCTL1_HCLKEN  	(19)
#define RSCM_PCLKEN   		(18)
#define RGPIO_PCLKEN  		(17)
#define RI2C0_PCLKEN  		(16)
#define	RI2C1_PCLKEN  		(15)
#define RI2C2_PCLKEN  		(14)
#define RI2S_PCLKEN   		(13)
#define RSSI0_PCLKEN  		(12)
#define RSSI1_PCLKEN  		(11)
#define RTIMERS_PCLKEN 		(10)
#define RUART0_PCLKEN  		(9)
#define RUART1_PCLKEN   	(8)
#define RLPC_PCLKEN   		(7)
#define RRTC_PCLKEN   		(6)
#define RGPIO_INTR_CLKEN  	(5)
#define RI2C0_IC_CLKEN  	(4)
#define RI2C1_IC_CLKEN  	(3)
#define RI2C2_IC_CLKEN  	(2)
#define RSSI0_SSI_CLKEN  	(1)
#define RSSI1_SSI_CLKEN  	(0)


inline void SET_RCORESIGHT_HCLKEN() 
{	
	set_bit(RCORESIGHT_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void CLR_RCORESIGHT_HCLKEN()
{
	clear_bit(RCORESIGHT_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void SET_RON2_HCLKEN() 
{
	set_bit(RON2_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void CLR_RON2_HCLKEN() 
{
	clear_bit(RON2_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void SET_RUSB_HCLKEN() 
{
	set_bit(RUSB_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void CLR_RUSB_HCLKEN() 
{
	clear_bit(RUSB_HCLKEN, PRCM_CLOCK_ENABLE_2);
}


inline void SET_RSSI1_PCLKEN()
{
	set_bit(RSSI1_PCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void CLR_RSSI1_PCLKEN()
{
	clear_bit(RSSI1_PCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void SET_RSATA_HCLKEN()
{
	set_bit(RSATA_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void CLR_RSATA_HCLKEN()
{
	clear_bit(RSATA_HCLKEN, PRCM_CLOCK_ENABLE_2);
}


inline void SET_RGMAC_HCLKEN()
{
	set_bit(RGMAC_HCLKEN, PRCM_CLOCK_ENABLE_2);
}

inline void CLR_RGMAC_HCLKEN()
{
	clear_bit(RGMAC_HCLKEN, PRCM_CLOCK_ENABLE_2);
}


inline void SET_RSTORAGE_HCLKEN()
{
	set_bit(RSTORAGE_HCLKEN, PRCM_CLOCK_ENABLE_2);
	set_bit(RSTORAGE_CCLKEN, PRCM_CLOCK_ENABLE_1);
}

inline void CLR_RSTORAGE_HCLKEN()
{
	clear_bit(RSTORAGE_HCLKEN, PRCM_CLOCK_ENABLE_2);
	clear_bit(RSTORAGE_CCLKEN, PRCM_CLOCK_ENABLE_1);
}


inline void set_bit(int nr, volatile void *addr)
{
        *((unsigned int *) addr) |= (1U << (nr));
}

inline void clear_bit(int nr, volatile void *addr)
{
        *((unsigned int *) addr) &= ~(1U << (nr));
}

void prcm_clock_init()
{
	CLR_RSSI1_PCLKEN();
}


#endif
