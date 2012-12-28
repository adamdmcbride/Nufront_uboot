#include <common.h>
#include <asm/io.h>

#if defined(CONFIG_PRCM_CLOCK)
#define EINVAL 1
#define LOCAL_TIMER     0x05040600
#define TIMER_ADDR      0x06150000
#define TIMER_CLK_MHZ   12 


unsigned long pll0_rate = 800000000;
unsigned long pll1_rate = 666000000;
unsigned long pll2_rate = 800000000;
unsigned long pll3_rate = 297000000;
unsigned long pll4_rate = 297000000;
unsigned long pll5_rate = 983040000;
unsigned long pll6_rate = 480000000;
unsigned long i2s0_rate = 98304000;


/* SECTION 1: Common Tasks Definition */
void reg_clr_bits(volatile void *addr, unsigned int clr)
{
	*((unsigned int*) addr)  &= ~clr;
}

void reg_set_bits(volatile void *addr, unsigned int set)
{
        *((unsigned int*) addr) |= set;
}

void sys_pwr_ctrl_set_bits(unsigned int set, unsigned int status) 
{
    volatile void *sys_pwr_ctrl = (PRCM_SYS_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Set System Power Control Bits
    reg_set_bits(sys_pwr_ctrl, set);

    // Wait until Power Status is ALIVE
    while ((sys_pwr_status_reg & status) == 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

void sys_pwr_ctrl_clr_bits(unsigned int clr, unsigned int status) 
{
    volatile void *sys_pwr_ctrl = (PRCM_SYS_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Clear System Power Control Bits
    reg_clr_bits(sys_pwr_ctrl, clr);

    // Wait until Power Status is NOT ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & status) != 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

int get_offset_pos(unsigned int mask) 
{
    int offset;
    for(offset=0; offset<32; offset++) {
        if(((0x01 << offset) & mask) != 0) break;
    }

    return offset;
}

void clk_set_rate_bits(volatile void *addr, unsigned int set_en, 
                       unsigned int set_mask, unsigned int set_bits)
{
    int set_offset_pos;

    // Clear Enable Bits
    reg_clr_bits(addr, set_en);

    // Set New Bits
    set_offset_pos = get_offset_pos(set_mask);
    set_bits  <<= set_offset_pos;
    reg_clr_bits(addr, set_mask);
    reg_set_bits(addr, set_bits);

    // Set Enable Bits
    reg_set_bits(addr, set_en);
}



/* SECTION 2: Clock Control Function/Tasks Definition */

// PLL configuration function/tasks
void pll_config_reg(volatile void *base, unsigned int pll_nr, unsigned int pll_nf, unsigned int pll_od, unsigned int pll_nb) 
{
        unsigned int reg_value;

        // PLLn_CFG0
        reg_value = (((pll_nf-1)&0x1fff) << 8) | ((pll_nr-1)&0x3f);
	*((unsigned int*) base) = reg_value;

        // PLLn_CFG1
        reg_value = (0x1<<18) | (((pll_nb-1)&0xfff)<<4) | ((pll_od -1)&0xf);
	*((unsigned int*) (base+0x4)) = reg_value;
}

int ns2815_pll_get_param(unsigned long rate, unsigned int * pll_nr, unsigned int * pll_nf, unsigned int * pll_od, unsigned int * pll_nb) 
{
    switch (rate) {
    case 50000000:
        *pll_nr = PRCM_PLL_NR_50MHZ;
        *pll_nf = PRCM_PLL_NF_50MHZ;
        *pll_od = PRCM_PLL_OD_50MHZ;
        *pll_nb = PRCM_PLL_NB_50MHZ;
        break;
    case 200000000:
        *pll_nr = PRCM_PLL_NR_200MHZ;
        *pll_nf = PRCM_PLL_NF_200MHZ;
        *pll_od = PRCM_PLL_OD_200MHZ;
        *pll_nb = PRCM_PLL_NB_200MHZ;
        break;
    case 250000000:
        *pll_nr = PRCM_PLL_NR_250MHZ;
        *pll_nf = PRCM_PLL_NF_250MHZ;
        *pll_od = PRCM_PLL_OD_250MHZ;
        *pll_nb = PRCM_PLL_NB_250MHZ;
        break;
    case 297000000:
        *pll_nr = PRCM_PLL_NR_297MHZ;
        *pll_nf = PRCM_PLL_NF_297MHZ;
        *pll_od = PRCM_PLL_OD_297MHZ;
        *pll_nb = PRCM_PLL_NB_297MHZ;
        break;
    case 300000000:
        *pll_nr = PRCM_PLL_NR_300MHZ;
        *pll_nf = PRCM_PLL_NF_300MHZ;
        *pll_od = PRCM_PLL_OD_300MHZ;
        *pll_nb = PRCM_PLL_NB_300MHZ;
        break;
    case 350000000:
        *pll_nr = PRCM_PLL_NR_350MHZ;
        *pll_nf = PRCM_PLL_NF_350MHZ;
        *pll_od = PRCM_PLL_OD_350MHZ;
        *pll_nb = PRCM_PLL_NB_350MHZ;
        break;
    case 400000000:
        *pll_nr = PRCM_PLL_NR_400MHZ;
        *pll_nf = PRCM_PLL_NF_400MHZ;
        *pll_od = PRCM_PLL_OD_400MHZ;
        *pll_nb = PRCM_PLL_NB_400MHZ;
        break;
    case 450000000:
        *pll_nr = PRCM_PLL_NR_450MHZ;
        *pll_nf = PRCM_PLL_NF_450MHZ;
        *pll_od = PRCM_PLL_OD_450MHZ;
        *pll_nb = PRCM_PLL_NB_450MHZ;
        break;
    case 480000000:
        *pll_nr = PRCM_PLL_NR_480MHZ;
        *pll_nf = PRCM_PLL_NF_480MHZ;
        *pll_od = PRCM_PLL_OD_480MHZ;
        *pll_nb = PRCM_PLL_NB_480MHZ;
        break;
    case 500000000:
        *pll_nr = PRCM_PLL_NR_500MHZ;
        *pll_nf = PRCM_PLL_NF_500MHZ;
        *pll_od = PRCM_PLL_OD_500MHZ;
        *pll_nb = PRCM_PLL_NB_500MHZ;
        break;
    case 533000000:
        *pll_nr = PRCM_PLL_NR_533MHZ;
        *pll_nf = PRCM_PLL_NF_533MHZ;
        *pll_od = PRCM_PLL_OD_533MHZ;
        *pll_nb = PRCM_PLL_NB_533MHZ;
        break;
    case 550000000:
        *pll_nr = PRCM_PLL_NR_550MHZ;
        *pll_nf = PRCM_PLL_NF_550MHZ;
        *pll_od = PRCM_PLL_OD_550MHZ;
        *pll_nb = PRCM_PLL_NB_550MHZ;
        break;
    case 600000000:
        *pll_nr = PRCM_PLL_NR_600MHZ;
        *pll_nf = PRCM_PLL_NF_600MHZ;
        *pll_od = PRCM_PLL_OD_600MHZ;
        *pll_nb = PRCM_PLL_NB_600MHZ;
        break;
    case 650000000:
        *pll_nr = PRCM_PLL_NR_650MHZ;
        *pll_nf = PRCM_PLL_NF_650MHZ;
        *pll_od = PRCM_PLL_OD_650MHZ;
        *pll_nb = PRCM_PLL_NB_650MHZ;
        break;
    case 666000000:
        *pll_nr = PRCM_PLL_NR_666MHZ;
        *pll_nf = PRCM_PLL_NF_666MHZ;
        *pll_od = PRCM_PLL_OD_666MHZ;
        *pll_nb = PRCM_PLL_NB_666MHZ;
        break;
    case 700000000:
        *pll_nr = PRCM_PLL_NR_700MHZ;
        *pll_nf = PRCM_PLL_NF_700MHZ;
        *pll_od = PRCM_PLL_OD_700MHZ;
        *pll_nb = PRCM_PLL_NB_700MHZ;
        break;
    case 720000000:
        *pll_nr = PRCM_PLL_NR_720MHZ;
        *pll_nf = PRCM_PLL_NF_720MHZ;
        *pll_od = PRCM_PLL_OD_720MHZ;
        *pll_nb = PRCM_PLL_NB_720MHZ;
        break;
    case 750000000:
        *pll_nr = PRCM_PLL_NR_750MHZ;
        *pll_nf = PRCM_PLL_NF_750MHZ;
        *pll_od = PRCM_PLL_OD_750MHZ;
        *pll_nb = PRCM_PLL_NB_750MHZ;
        break;
    case 800000000:
        *pll_nr = PRCM_PLL_NR_800MHZ;
        *pll_nf = PRCM_PLL_NF_800MHZ;
        *pll_od = PRCM_PLL_OD_800MHZ;
        *pll_nb = PRCM_PLL_NB_800MHZ;
        break;
    case 850000000:
        *pll_nr = PRCM_PLL_NR_850MHZ;
        *pll_nf = PRCM_PLL_NF_850MHZ;
        *pll_od = PRCM_PLL_OD_850MHZ;
        *pll_nb = PRCM_PLL_NB_850MHZ;
        break;
    case 891000000:
        *pll_nr = PRCM_PLL_NR_891MHZ;
        *pll_nf = PRCM_PLL_NF_891MHZ;
        *pll_od = PRCM_PLL_OD_891MHZ;
        *pll_nb = PRCM_PLL_NB_891MHZ;
        break;
    case 900000000:
        *pll_nr = PRCM_PLL_NR_900MHZ;
        *pll_nf = PRCM_PLL_NF_900MHZ;
        *pll_od = PRCM_PLL_OD_900MHZ;
        *pll_nb = PRCM_PLL_NB_900MHZ;
        break;
    case 950000000:
        *pll_nr = PRCM_PLL_NR_950MHZ;
        *pll_nf = PRCM_PLL_NF_950MHZ;
        *pll_od = PRCM_PLL_OD_950MHZ;
        *pll_nb = PRCM_PLL_NB_950MHZ;
        break;
    case 983040000:
        *pll_nr = PRCM_PLL_NR_983M04HZ;
        *pll_nf = PRCM_PLL_NF_983M04HZ;
        *pll_od = PRCM_PLL_OD_983M04HZ;
        *pll_nb = PRCM_PLL_NB_983M04HZ;
        break;
    case 1000000000:
        *pll_nr = PRCM_PLL_NR_1000MHZ;
        *pll_nf = PRCM_PLL_NF_1000MHZ;
        *pll_od = PRCM_PLL_OD_1000MHZ;
        *pll_nb = PRCM_PLL_NB_1000MHZ;
        break;
    case 1050000000:
        *pll_nr = PRCM_PLL_NR_1050MHZ;
        *pll_nf = PRCM_PLL_NF_1050MHZ;
        *pll_od = PRCM_PLL_OD_1050MHZ;
        *pll_nb = PRCM_PLL_NB_1050MHZ;
        break;
    case 1100000000:
        *pll_nr = PRCM_PLL_NR_1100MHZ;
        *pll_nf = PRCM_PLL_NF_1100MHZ;
        *pll_od = PRCM_PLL_OD_1100MHZ;
        *pll_nb = PRCM_PLL_NB_1100MHZ;
        break;
    case 1150000000:
        *pll_nr = PRCM_PLL_NR_1150MHZ;
        *pll_nf = PRCM_PLL_NF_1150MHZ;
        *pll_od = PRCM_PLL_OD_1150MHZ;
        *pll_nb = PRCM_PLL_NB_1150MHZ;
        break;
    case 1200000000:
        *pll_nr = PRCM_PLL_NR_1200MHZ;
        *pll_nf = PRCM_PLL_NF_1200MHZ;
        *pll_od = PRCM_PLL_OD_1200MHZ;
        *pll_nb = PRCM_PLL_NB_1200MHZ;
        break;
    case 1250000000:
        *pll_nr = PRCM_PLL_NR_1250MHZ;
        *pll_nf = PRCM_PLL_NF_1250MHZ;
        *pll_od = PRCM_PLL_OD_1250MHZ;
        *pll_nb = PRCM_PLL_NB_1250MHZ;
        break;
    case 1300000000:
        *pll_nr = PRCM_PLL_NR_1300MHZ;
        *pll_nf = PRCM_PLL_NF_1300MHZ;
        *pll_od = PRCM_PLL_OD_1300MHZ;
        *pll_nb = PRCM_PLL_NB_1300MHZ;
        break;
    case 1350000000:
        *pll_nr = PRCM_PLL_NR_1350MHZ;
        *pll_nf = PRCM_PLL_NF_1350MHZ;
        *pll_od = PRCM_PLL_OD_1350MHZ;
        *pll_nb = PRCM_PLL_NB_1350MHZ;
        break;
    case 1400000000:
        *pll_nr = PRCM_PLL_NR_1400MHZ;
        *pll_nf = PRCM_PLL_NF_1400MHZ;
        *pll_od = PRCM_PLL_OD_1400MHZ;
        *pll_nb = PRCM_PLL_NB_1400MHZ;
        break;
    case 1450000000:
        *pll_nr = PRCM_PLL_NR_1450MHZ;
        *pll_nf = PRCM_PLL_NF_1450MHZ;
        *pll_od = PRCM_PLL_OD_1450MHZ;
        *pll_nb = PRCM_PLL_NB_1450MHZ;
        break;
    case 1500000000:
        *pll_nr = PRCM_PLL_NR_1500MHZ;
        *pll_nf = PRCM_PLL_NF_1500MHZ;
        *pll_od = PRCM_PLL_OD_1500MHZ;
        *pll_nb = PRCM_PLL_NB_1500MHZ;
        break;
    case 1550000000:
        *pll_nr = PRCM_PLL_NR_1550MHZ;
        *pll_nf = PRCM_PLL_NF_1550MHZ;
        *pll_od = PRCM_PLL_OD_1550MHZ;
        *pll_nb = PRCM_PLL_NB_1550MHZ;
        break;
    case 1600000000:
        *pll_nr = PRCM_PLL_NR_1600MHZ;
        *pll_nf = PRCM_PLL_NF_1600MHZ;
        *pll_od = PRCM_PLL_OD_1600MHZ;
        *pll_nb = PRCM_PLL_NB_1600MHZ;
        break;
    case 1650000000:
        *pll_nr = PRCM_PLL_NR_1650MHZ;
        *pll_nf = PRCM_PLL_NF_1650MHZ;
        *pll_od = PRCM_PLL_OD_1650MHZ;
        *pll_nb = PRCM_PLL_NB_1650MHZ;
        break;
    case 1700000000:
        *pll_nr = PRCM_PLL_NR_1700MHZ;
        *pll_nf = PRCM_PLL_NF_1700MHZ;
        *pll_od = PRCM_PLL_OD_1700MHZ;
        *pll_nb = PRCM_PLL_NB_1700MHZ;
        break;
    case 1750000000:
        *pll_nr = PRCM_PLL_NR_1750MHZ;
        *pll_nf = PRCM_PLL_NF_1750MHZ;
        *pll_od = PRCM_PLL_OD_1750MHZ;
        *pll_nb = PRCM_PLL_NB_1750MHZ;
        break;
    case 1800000000:
        *pll_nr = PRCM_PLL_NR_1800MHZ;
        *pll_nf = PRCM_PLL_NF_1800MHZ;
        *pll_od = PRCM_PLL_OD_1800MHZ;
        *pll_nb = PRCM_PLL_NB_1800MHZ;
        break;

// lcdc only
    case 50350000: /* 640*480*/
        *pll_nr = PRCM_PLL_NR_50M35HZ;
        *pll_nf = PRCM_PLL_NF_50M35HZ;
        *pll_od = PRCM_PLL_OD_50M35HZ;
        *pll_nb = PRCM_PLL_NB_50M35HZ;
        break;
    case 66528000: /* 800*480*/
        *pll_nr = PRCM_PLL_NR_66M528HZ;
        *pll_nf = PRCM_PLL_NF_66M528HZ;
        *pll_od = PRCM_PLL_OD_66M528HZ;
        *pll_nb = PRCM_PLL_NB_66M528HZ;
        break;
    case 56000000: /* 800*480*/
        *pll_nr = PRCM_PLL_NR_56MHZ;
        *pll_nf = PRCM_PLL_NF_56MHZ;
        *pll_od = PRCM_PLL_OD_56MHZ;
        *pll_nb = PRCM_PLL_NB_56MHZ;
        break;
    case 58464000: /* 800*480*/
        *pll_nr = PRCM_PLL_NR_58M464HZ;
        *pll_nf = PRCM_PLL_NF_58M464HZ;
        *pll_od = PRCM_PLL_OD_58M464HZ;
        *pll_nb = PRCM_PLL_NB_58M464HZ;
        break;
    case 80000000: /* 800*600*/
        *pll_nr = PRCM_PLL_NR_80MHZ;
        *pll_nf = PRCM_PLL_NF_80MHZ;
        *pll_od = PRCM_PLL_OD_80MHZ;
        *pll_nb = PRCM_PLL_NB_80MHZ;
        break;
    case 87500000:/*1024*600*/
        *pll_nr = PRCM_PLL_NR_87M5HZ;
        *pll_nf = PRCM_PLL_NF_87M5HZ;
        *pll_od = PRCM_PLL_OD_87M5HZ;
        *pll_nb = PRCM_PLL_NB_87M5HZ;
        break;
    case 100800000:/*1024*600*/
        *pll_nr = PRCM_PLL_NR_100M8HZ;
        *pll_nf = PRCM_PLL_NF_100M8HZ;
        *pll_od = PRCM_PLL_OD_100M8HZ;
        *pll_nb = PRCM_PLL_NB_100M8HZ;
        break;
     case 102400000:/*1024*600*/
        *pll_nr = PRCM_PLL_NR_102M4HZ;
        *pll_nf = PRCM_PLL_NF_102M4HZ;
        *pll_od = PRCM_PLL_OD_102M4HZ;
        *pll_nb = PRCM_PLL_NB_102M4HZ;
        break;
    case 130000000: /* 1024*768*/
        *pll_nr = PRCM_PLL_NR_130MHZ;
        *pll_nf = PRCM_PLL_NF_130MHZ;
        *pll_od = PRCM_PLL_OD_130MHZ;
        *pll_nb = PRCM_PLL_NB_130MHZ;
        break;
    case 142000000: /* 1280*800 (RB)*/
        *pll_nr = PRCM_PLL_NR_142MHZ;
        *pll_nf = PRCM_PLL_NF_142MHZ;
        *pll_od = PRCM_PLL_OD_142MHZ;
        *pll_nb = PRCM_PLL_NB_142MHZ;
        break;
    case 148500000: /* 1280*720*/
        *pll_nr = PRCM_PLL_NR_148M5HZ;
        *pll_nf = PRCM_PLL_NF_148M5HZ;
        *pll_od = PRCM_PLL_OD_148M5HZ;
        *pll_nb = PRCM_PLL_NB_148M5HZ;
        break;
    case 167000000: /* 1280*800 */
        *pll_nr = PRCM_PLL_NR_167MHZ;
        *pll_nf = PRCM_PLL_NF_167MHZ;
        *pll_od = PRCM_PLL_OD_167MHZ;
        *pll_nb = PRCM_PLL_NB_167MHZ;
        break;
    case 171000000: /* 1360*768*/
        *pll_nr = PRCM_PLL_NR_171MHZ;
        *pll_nf = PRCM_PLL_NF_171MHZ;
        *pll_od = PRCM_PLL_OD_171MHZ;
        *pll_nb = PRCM_PLL_NB_171MHZ;
        break;
    case 177500000: /* 1440*900*/
        *pll_nr = PRCM_PLL_NR_177M5HZ;
        *pll_nf = PRCM_PLL_NF_177M5HZ;
        *pll_od = PRCM_PLL_OD_177M5HZ;
        *pll_nb = PRCM_PLL_NB_177M5HZ;
        break;
    case 238000000: /* 1680*1050*/
        *pll_nr = PRCM_PLL_NR_238MHZ;
        *pll_nf = PRCM_PLL_NF_238MHZ;
        *pll_od = PRCM_PLL_OD_238MHZ;
        *pll_nb = PRCM_PLL_NB_238MHZ;
        break;
    //case 297000000: /* 1920*1080*/
    //    *pll_nr = PRCM_PLL_NR_297MHZ;
    //    *pll_nf = PRCM_PLL_NF_297MHZ;
    //    *pll_od = PRCM_PLL_OD_297MHZ;
    //    *pll_nb = PRCM_PLL_NB_297MHZ;
    //    break;
    case 324000000: /* 1600*1200 */
        *pll_nr = PRCM_PLL_NR_324MHZ;
        *pll_nf = PRCM_PLL_NF_324MHZ;
        *pll_od = PRCM_PLL_OD_324MHZ;
        *pll_nb = PRCM_PLL_NB_324MHZ;
        break;

	default:
		return -EINVAL;
		break;
    }

    return 0;
}

#define NS2815_PLL_CLK_CTRL(plln, PLLN)                                        \
int ns2815_##plln##_clk_enable(void)                                           \
{                                                                              \
    volatile void *sys_pll_ctrl = (PRCM_SYS_PLL_CTRL);                         \
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_##PLLN##_CLKEN);              \
                                                                               \
    printf("#plln clock enabled.\n");                                          \
    return 0;                                                                  \
}                                                                              \
                                                                               \
int ns2815_##plln##_clk_disable(void)                                          \
{                                                                              \
    volatile void *sys_pll_ctrl = (PRCM_SYS_PLL_CTRL);                         \
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_##PLLN##_CLKEN);              \
                                                                               \
    printf("#plln clock disabled.\n");                                         \
    return 0;                                                                  \
}

#define NS2815_PLL_PWR_DOWN_CTRL(plln, PLLN)                                   \
void ns2815_##plln##_pwr_down(void)                                            \
{                                                                              \
    volatile void *sys_pll_ctrl = (PRCM_SYS_PLL_CTRL);                         \
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);                     \
                                                                               \
    unsigned int sys_pwr_status_reg = 0;                                       \
                                                                               \
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_##PLLN##_PWR_DN);             \
                                                                               \
    while((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_##PLLN##_PWRDN) ==0) {     \
        udelay(100);                                                           \
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);                 \
    }                                                                          \
                                                                               \
    printf("#plln power down.\n");                                             \
}


int ns2815_pll0_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll0_cfg_base = (PRCM_PLL0_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll0_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    //reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL0_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_RST);
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_CLKEN); //(not needed for PLL 0/1/2)

    pll0_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll0, PLL0)
NS2815_PLL_PWR_DOWN_CTRL (pll0, PLL0)

void ns2815_pll0_pwr_up(void)
{
    volatile void *sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void *sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_SW_CFG_EN);

    // Disable PLL Clock (not needed for PLL 0/1/2)
    //reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_CLKEN);

    // Power Up PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_PWR_DN);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL0_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Disable PLL Clock (not needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL0_CLKEN);

    printf("pll0 power up.\n");
}

int ns2815_pll0_clk_set_rate(unsigned long rate, unsigned int nouse)
{
    ns2815_pll0_config(rate);

//    printf("pll0 clock rate set to %ld.\n", rate);
    return 0;
}

int ns2815_pll1_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll1_cfg_base = (PRCM_PLL1_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL1_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll1_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    //reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL1_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL1_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL1_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL1_RST);
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL1_CLKEN); //(not needed for PLL 0/1/2)

    pll1_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll1, PLL1)

int ns2815_pll1_clk_set_rate(unsigned long rate, unsigned int nouse)
{
    ns2815_pll1_config(rate);

    printf("pll1 clock rate set to %ld.\n", rate);
    return 0;
}

int ns2815_pll2_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll2_cfg_base = (PRCM_PLL2_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL2_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll2_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    //reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL2_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL2_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL2_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL2_RST);
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL2_CLKEN); //(not needed for PLL 0/1/2)

    pll2_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll2, PLL2)

int ns2815_pll3_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll3_cfg_base = (PRCM_PLL3_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll3_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL3_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_RST);
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_CLKEN); //(not needed for PLL 0/1/2)

    pll3_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll3, PLL3)
NS2815_PLL_PWR_DOWN_CTRL (pll3, PLL3)

void ns2815_pll3_pwr_up(void)
{
    volatile void *sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void *sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_SW_CFG_EN);

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_CLKEN);

    // Power Up PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_PWR_DN);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL3_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL3_CLKEN);

    printf("pll3 power up.\n");
}

int ns2815_pll3_clk_set_rate(unsigned long rate, unsigned int nouse)
{
    ns2815_pll3_config(rate);

    printf("pll3 clock rate set to %ld.\n", rate);
    return 0;
}

int ns2815_pll4_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll4_cfg_base = (PRCM_PLL4_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll4_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL4_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_RST);
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_CLKEN); //(not needed for PLL 0/1/2)

    pll4_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll4, PLL4)
NS2815_PLL_PWR_DOWN_CTRL (pll4, PLL4)

void ns2815_pll4_pwr_up(void)
{
    volatile void *sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void *sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_SW_CFG_EN);

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_CLKEN);

    // Power Up PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_PWR_DN);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL4_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL4_CLKEN);

    printf("pll4 power up.\n");
}

int ns2815_pll4_clk_set_rate(unsigned long rate, unsigned int nouse)
{
    ns2815_pll4_config(rate);

    printf("pll4 clock rate set to %ld.\n", rate);
    return 0;
}

int ns2815_pll5_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll5_cfg_base = (PRCM_PLL5_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll5_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL5_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_RST);
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_CLKEN); //(not needed for PLL 0/1/2)

    pll5_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll5, PLL5)
NS2815_PLL_PWR_DOWN_CTRL (pll5, PLL5)

void ns2815_pll5_pwr_up(void)
{
    volatile void *sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void *sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_SW_CFG_EN);

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_CLKEN);

    // Power Up PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_PWR_DN);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL5_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL5_CLKEN);

    printf("pll5 power up.\n");
}

int ns2815_pll6_config(unsigned long rate)
{
    unsigned int pll_nr; 
    unsigned int pll_nf; 
    unsigned int pll_od; 
    unsigned int pll_nb;

    volatile void * pll6_cfg_base = (PRCM_PLL6_CFG0);
    volatile void * sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void * sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_SW_CFG_EN);

    // Configure New PLL Parameter
    ns2815_pll_get_param(rate, &pll_nr, &pll_nf, &pll_od, &pll_nb);
    pll_config_reg(pll6_cfg_base, pll_nr, pll_nf, pll_od, pll_nb);

    //Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_CLKEN);

    // Reset PLL
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_RST);
    udelay(100);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL6_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Clear Reset and Enable PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_RST);
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_CLKEN); //(not needed for PLL 0/1/2)

    pll6_rate = rate;
    return 0;
}

NS2815_PLL_CLK_CTRL (pll6, PLL6)
NS2815_PLL_PWR_DOWN_CTRL (pll6, PLL6)

void ns2815_pll6_pwr_up(void)
{
    volatile void *sys_pll_ctrl   = (PRCM_SYS_PLL_CTRL);
    volatile void *sys_pll_status = (PRCM_SYS_PLL_STATUS);

    unsigned int sys_pll_status_reg = 0;

    // Enable SW PLL Config (only needed for PLL 0/1/2)
    //reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_SW_CFG_EN);

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_CLKEN);

    // Power Up PLL
    reg_clr_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_PWR_DN);

    // Wait until PLL Stable
    while ((sys_pll_status_reg & PRCM_SYS_PLL_STATUS_PLL6_STABLE) == 0) {
        sys_pll_status_reg = *(unsigned int*)(sys_pll_status);
    }

    // Disable PLL Clock (not needed for PLL 0/1/2)
    reg_set_bits(sys_pll_ctrl, PRCM_SYS_PLL_CTRL_PLL6_CLKEN);

    printf("pll6 power up.\n");
}


// Coresight clock control function/tasks
int ns2815_coresight_clk_enable()
{
        unsigned int reg_value;
        volatile void *coresight_clk_ctrl = (PRCM_CORESIGHT_CLK_CTRL);

        reg_value = PRCM_CORESIGHT_CLK_CTRL_CS_ATCLKEN |
                    PRCM_CORESIGHT_CLK_CTRL_CS_HCLKEN;
        reg_set_bits(coresight_clk_ctrl, reg_value);

        printf("coresight clock enabled.\n");
        return 0;
}

int ns2815_coresight_clk_disable()
{
        unsigned int reg_value;
        volatile void *coresight_clk_ctrl = (PRCM_CORESIGHT_CLK_CTRL);

        reg_value = PRCM_CORESIGHT_CLK_CTRL_CS_ATCLKEN |
                    PRCM_CORESIGHT_CLK_CTRL_CS_HCLKEN;
        reg_clr_bits(coresight_clk_ctrl, reg_value);

        printf("coresight clock disabled.\n");
        return 0;
}

int ns2815_coresight_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *coresight_clk_ctrl = (PRCM_CORESIGHT_CLK_CTRL);
        unsigned int coresight_clk_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (coresight_clk_div >0x0F)
            return -EINVAL;

        clk_set_rate_bits(coresight_clk_ctrl, PRCM_CORESIGHT_CLK_CTRL_CS_AT_DIV_EN,
                          PRCM_CORESIGHT_CLK_CTRL_CS_AT_DIV_MASK, coresight_clk_div);

        rate = coresight_clk_div*rate;

        printf("coresight clock rate set to %ld.\n", rate);
        return 0;
}

// DDR clock control function/tasks
int ns2815_ddr_clk_enable()
{
        unsigned int reg_value;
        volatile void *ddr_clk_ctrl = (PRCM_DDR_CLK_CTRL);

        reg_value = PRCM_DDR_CLK_CTRL_DDR_CLKEN |
                    PRCM_DDR_CLK_CTRL_DDR_HCLKEN;
        reg_set_bits(ddr_clk_ctrl, reg_value);

        printf("ddr clock enabled.\n");
        return 0;
}

int ns2815_ddr_clk_disable()
{
        unsigned int reg_value;
        volatile void *ddr_clk_ctrl = (PRCM_DDR_CLK_CTRL);

        reg_value = PRCM_DDR_CLK_CTRL_DDR_CLKEN |
                    PRCM_DDR_CLK_CTRL_DDR_HCLKEN;
        reg_clr_bits(ddr_clk_ctrl, reg_value);

        printf("ddr clock disabled.\n");
        return 0;
}

int ns2815_ddr_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *ddr_clk_ctrl = (PRCM_DDR_CLK_CTRL);
        unsigned long pll1_rate;

        if (rate == 0)
            return -EINVAL;
        
        switch (rate) {
        case 400000000:
            pll1_rate = 800000000;
            break;
        case 333000000:
            pll1_rate = 666000000;
            break;
        case 266000000:
            pll1_rate = 533000000;
            break;
        case 200000000:
            pll1_rate = 400000000;
            break;
	    default:
	    	return -EINVAL;
	    	break;
        }

        reg_clr_bits(ddr_clk_ctrl, PRCM_DDR_CLK_CTRL_DDR_CLKEN|PRCM_DDR_CLK_CTRL_DDR_HCLKEN);

        ns2815_pll1_clk_set_rate(pll1_rate, 0);

        reg_set_bits(ddr_clk_ctrl, PRCM_DDR_CLK_CTRL_DDR_CLKEN|PRCM_DDR_CLK_CTRL_DDR_HCLKEN);

        printf("ddr clock rate set to %ld.\n", rate);
        return 0;
}

// PL330 DMA clock control function/tasks
int ns2815_pl330dma_clk_enable()
{
        unsigned int reg_value;
        volatile void *dma_clk_ctrl = (PRCM_DMA_CLK_CTRL);

        reg_value = PRCM_DMA_CLK_CTRL_DMA_ACLKEN;
        reg_set_bits(dma_clk_ctrl, reg_value);

        printf("pl330 dma clock enabled.\n");
        return 0;
}

int ns2815_pl330dma_clk_disable()
{
        unsigned int reg_value;
        volatile void *dma_clk_ctrl = (PRCM_DMA_CLK_CTRL);

        reg_value = PRCM_DMA_CLK_CTRL_DMA_ACLKEN;
        reg_clr_bits(dma_clk_ctrl, reg_value);

        printf("pl330 dma clock disabled.\n");
        return 0;
}

// MALI clock control function/tasks
void ns2815_mali_pwr_up()
{
        sys_pwr_ctrl_set_bits(PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_L2C | PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_PP | PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_GP,
                              PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_TOTAL | PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_L2C |PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_PP |PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_GP);
        printf("mali power up.\n");
}

void ns2815_mali_pwr_down()
{
        sys_pwr_ctrl_clr_bits(PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_L2C|PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_PP|PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_GP,
                              PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_TOTAL|PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_L2C|PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_PP|PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_GP);
        printf("mali power down.\n");
}

int ns2815_mali_clk_enable()
{
        unsigned int reg_value;
        volatile void *mali_clk_ctrl = (PRCM_MALI_CLK_CTRL);

        // Power Up MALI
	//ns2815_mali_pwr_up();

        // Enable MALI clocks
        reg_value = PRCM_MALI_CLK_CTRL_MALI_CLKEN;
        reg_set_bits(mali_clk_ctrl, reg_value);

        printf("mali clock enabled.\n");
        return 0;
}

int ns2815_mali_clk_disable()
{
        unsigned int reg_value;
        volatile void *mali_clk_ctrl = (PRCM_MALI_CLK_CTRL);
        
        // Disable MALI clocks
        reg_value = PRCM_MALI_CLK_CTRL_MALI_CLKEN;
        reg_clr_bits(mali_clk_ctrl, reg_value);

        printf("mali clock disabled.\n");

        // Power Down MALI
	//ns2815_mali_pwr_down();

        return 0;
}

int ns2815_mali_pll2_set_rate(unsigned long rate) 
{
        unsigned int mali_clk_sel = 0x0;
        volatile void *mali_clk_ctrl = (PRCM_MALI_CLK_CTRL);

	// set mali clock source to PLL6
        clk_set_rate_bits(mali_clk_ctrl, PRCM_MALI_CLK_CTRL_MALI_CLKEN, 
                          PRCM_MALI_CLK_CTRL_MALI_CLK_SEL, mali_clk_sel);

    	ns2815_pll2_config(rate);

	return 0;
}

int ns2815_mali_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *mali_clk_ctrl = (PRCM_MALI_CLK_CTRL);
        unsigned int mali_clk_sel, mali_clk_sel_cur;
        unsigned int mali_div0 = 0;
        unsigned int mali_div1 = 0;

        if (rate == 0)
            return -EINVAL;
        
        switch (rate) {
        case 160000000:
            mali_clk_sel = 0x0;
            mali_div0 = 0x3;
            break;
        case 500000000:
	    ns2815_mali_pll2_set_rate(1000000000);
            mali_clk_sel = 0x1;
            mali_div1 = 0x2;
            break;
        case 475000000:
	    ns2815_mali_pll2_set_rate(950000000);
            mali_clk_sel = 0x1;
            mali_div1 = 0x2;
            break;
        case 450000000:
	    ns2815_mali_pll2_set_rate(900000000);
            mali_clk_sel = 0x1;
            mali_div1 = 0x2;
            break;
        case 425000000:
	    ns2815_mali_pll2_set_rate(850000000);
            mali_clk_sel = 0x1;
            mali_div1 = 0x2;
            break;
        case 400000000:
            mali_clk_sel = 0x1;
            mali_div1 = 0x2;
            break;
	case 360000000:
  	    ns2815_pll6_config(720000000);
            mali_clk_sel = 0x0;
            mali_div0 = 0x2;
        case 200000000:
            mali_clk_sel = 0x1;
            mali_div1 = 0x4;
            break;
        case 266000000:
            mali_clk_sel = 0x1;
            mali_div1 = 0x3;
            break;
        case 240000000:
            mali_clk_sel = 0x0;
            mali_div0 = 0x2;
            break;
	    default:
	    	return -EINVAL;
	    	break;
        }


        if (mali_clk_sel == 0) {
            clk_set_rate_bits(mali_clk_ctrl, PRCM_MALI_CLK_CTRL_MALI_DIV0_EN, 
                              PRCM_MALI_CLK_CTRL_MALI_DIV0_MASK, mali_div0);
        }
        else {
            clk_set_rate_bits(mali_clk_ctrl, PRCM_MALI_CLK_CTRL_MALI_DIV1_EN, 
                              PRCM_MALI_CLK_CTRL_MALI_DIV1_MASK, mali_div1);
        }

        mali_clk_sel_cur = *(unsigned int*)(mali_clk_ctrl) & PRCM_MALI_CLK_CTRL_MALI_CLK_SEL;
        if ((mali_clk_sel << get_offset_pos(PRCM_MALI_CLK_CTRL_MALI_CLK_SEL)) != mali_clk_sel_cur) {
            clk_set_rate_bits(mali_clk_ctrl, PRCM_MALI_CLK_CTRL_MALI_CLKEN, 
                              PRCM_MALI_CLK_CTRL_MALI_CLK_SEL, mali_clk_sel);
        }


        //printf("mali clock rate set to %ld.\n", rate);
        return 0;
}

// GC300 clock control function/tasks
void ns2815_gc300_pwr_up()
{
        sys_pwr_ctrl_set_bits(PRCM_SYS_PWR_CTRL_GC300_PWR_STATE, PRCM_SYS_PWR_STATUS_GC300_PWR_STATUS);
        printf("gc300 power up.\n");
}

void ns2815_gc300_pwr_down()
{
        sys_pwr_ctrl_clr_bits(PRCM_SYS_PWR_CTRL_GC300_PWR_STATE, PRCM_SYS_PWR_STATUS_GC300_PWR_STATUS);
        printf("gc300 power down.\n");
}

int ns2815_gc300_clk_enable()
{
        unsigned int reg_value;
        volatile void *gc300_clk_ctrl = (PRCM_GC300_CLK_CTRL);

        // Power Up GC300
	//ns2815_gc300_pwr_up();

        // Enable GC300 clocks
        reg_value  = PRCM_GC300_CLK_CTRL_GC300_CLKEN  |
                     PRCM_GC300_CLK_CTRL_GC300_HCLKEN |
                     PRCM_GC300_CLK_CTRL_GC300_ACLKEN;
        reg_set_bits(gc300_clk_ctrl, reg_value);

        printf("gc300 clock enabled.\n");
        return 0;
}

int ns2815_gc300_clk_disable()
{
        unsigned int reg_value;
        volatile void *gc300_clk_ctrl = (PRCM_GC300_CLK_CTRL);

        // Disable GC300 clocks
        reg_value  = PRCM_GC300_CLK_CTRL_GC300_CLKEN  |
                     PRCM_GC300_CLK_CTRL_GC300_HCLKEN |
                     PRCM_GC300_CLK_CTRL_GC300_ACLKEN;
        reg_clr_bits(gc300_clk_ctrl, reg_value);

        printf("gc300 clock disabled.\n");

        // Power Down GC300
	//ns2815_gc300_pwr_down();

        return 0;
}

int ns2815_gc300_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *gc300_clk_ctrl = (PRCM_GC300_CLK_CTRL);
        unsigned int gc300_div = 0;
        unsigned int gc300_clk_sel, gc300_clk_sel_cur;

        if (rate == 0) 
            return -EINVAL;

        switch (rate) {
        case 480000000:
            gc300_clk_sel = 0x1;
            break;
        case 400000000: 
            gc300_clk_sel = 0x0;
            gc300_div = 0x2;
            break;
        default:
            gc300_clk_sel = 0x0;
            gc300_div = 800000000/rate;
            rate = 800000000/gc300_div;
            break;
        }


        if (gc300_div >0x0FF)
            return -EINVAL;

        if (gc300_clk_sel == 0) {
            clk_set_rate_bits(gc300_clk_ctrl, PRCM_GC300_CLK_CTRL_GC300_DIV_EN,
                              PRCM_GC300_CLK_CTRL_GC300_DIV_MASK, gc300_div);
        }

        gc300_clk_sel_cur = *(unsigned int*)(gc300_clk_ctrl) & PRCM_GC300_CLK_CTRL_GC300_CLK_SEL;

        if ((gc300_clk_sel << get_offset_pos(PRCM_GC300_CLK_CTRL_GC300_CLK_SEL)) != gc300_clk_sel_cur) {
            clk_set_rate_bits(gc300_clk_ctrl, PRCM_GC300_CLK_CTRL_GC300_CLKEN, 
                              PRCM_GC300_CLK_CTRL_GC300_CLK_SEL, gc300_clk_sel);
        }


//        printf("gc300 clock rate set to %ld.\n", rate);
        return 0;
}

// VPU_G1 clock control function/tasks
void ns2815_vpu_g1_pwr_up()
{
        sys_pwr_ctrl_set_bits(PRCM_SYS_PWR_CTRL_VPU_G1_PWR_STATE, PRCM_SYS_PWR_STATUS_VPU_G1_STATUS);
        printf("vpu_g1 power up.\n");
}

void ns2815_vpu_g1_pwr_down()
{
        sys_pwr_ctrl_clr_bits(PRCM_SYS_PWR_CTRL_VPU_G1_PWR_STATE, PRCM_SYS_PWR_STATUS_VPU_G1_STATUS);
        printf("vpu_g1 power down.\n");
}

int ns2815_vpu_g1_clk_enable()
{
        unsigned int reg_value;
        volatile void *vpu_g1_clk_ctrl = (PRCM_VPU_G1_CLK_CTRL);

        // Power Up VPU_G1
	//ns2815_vpu_g1_pwr_up();

        // Enable VPU_G1 clocks
        reg_value = PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLKEN;
        reg_set_bits(vpu_g1_clk_ctrl, reg_value);

        printf("vpu_g1 clock enabled.\n");
        return 0;
}

int ns2815_vpu_g1_clk_disable()
{
        unsigned int reg_value;
        volatile void *vpu_g1_clk_ctrl = (PRCM_VPU_G1_CLK_CTRL);

        // Disable VPU_G1 clocks
        reg_value = PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLKEN;
        reg_clr_bits(vpu_g1_clk_ctrl, reg_value);

        printf("vpu_g1 clock disabled.\n");

        // Power Down VPU_G1
	//ns2815_vpu_g1_pwr_down();

        return 0;
}

int ns2815_vpu_g1_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *vpu_g1_clk_ctrl = (PRCM_VPU_G1_CLK_CTRL);
        unsigned int vpu_g1_div0 = 0;
        unsigned int vpu_g1_div1 = 0;
        unsigned int vpu_g1_clk_sel, vpu_g1_clk_sel_cur;
        unsigned long pll4_clk_rate = pll4_rate;

        if (rate == 0) 
            return -EINVAL;
            
        switch (rate) {
        case 297000000:
            if (pll4_clk_rate == 297000000) {
                //vpu_g1_clk_sel  = 0x1;
                //vpu_g1_div1     = 0x1;
  		printf("vpu_g1 clock rate cannot set to %ld when pll4_clk_rate is 297MHz.\n", rate);
                return -EINVAL;

            }
            else if (pll4_clk_rate == 891000000) {
                vpu_g1_clk_sel  = 0x1;
                vpu_g1_div1     = 0x3;
            }
            else {
                vpu_g1_clk_sel  = 0x0;
                vpu_g1_div0     = 800000000/rate;
            }
            break;
	case 400000000:
            vpu_g1_clk_sel  = 0x0;
            vpu_g1_div0     = 0x2;
            break;
	case 266000000:
            vpu_g1_clk_sel  = 0x0;
            vpu_g1_div0     = 0x3;
            break;
        case 200000000:
            vpu_g1_clk_sel  = 0x0;
            vpu_g1_div0     = 0x4;
            break;
        default:
            vpu_g1_clk_sel  = 0x0;
            vpu_g1_div0     = 800000000/rate;
            break;
        }


        if (vpu_g1_div0 >0x0FF)
            return -EINVAL;

        if (vpu_g1_clk_sel == 0) {
            clk_set_rate_bits(vpu_g1_clk_ctrl, PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV0_EN,
                              PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV0_MASK, vpu_g1_div0);
        }
        else {
            clk_set_rate_bits(vpu_g1_clk_ctrl, PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV1_EN,
                              PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV1_MASK, vpu_g1_div1);
        }

        vpu_g1_clk_sel_cur = *(unsigned int*)(vpu_g1_clk_ctrl) & PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLK_SEL;

        if ((vpu_g1_clk_sel << get_offset_pos(PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLK_SEL)) != vpu_g1_clk_sel_cur) {
            clk_set_rate_bits(vpu_g1_clk_ctrl, PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLKEN, 
                              PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLK_SEL, vpu_g1_clk_sel);
        }


        printf("vpu_g1 clock rate set to %ld.\n", rate);
        return 0;
}

// VPU_H1 clock control function/tasks
void ns2815_vpu_h1_pwr_up()
{
        sys_pwr_ctrl_set_bits(PRCM_SYS_PWR_CTRL_VPU_H1_PWR_STATE, PRCM_SYS_PWR_STATUS_VPU_H1_STATUS);
        printf("vpu_h1 power up.\n");
}

void ns2815_vpu_h1_pwr_down()
{
        sys_pwr_ctrl_clr_bits(PRCM_SYS_PWR_CTRL_VPU_H1_PWR_STATE, PRCM_SYS_PWR_STATUS_VPU_H1_STATUS);
        printf("vpu_h1 power down.\n");
}

int ns2815_vpu_h1_clk_enable()
{
        unsigned int reg_value;
        volatile void *vpu_h1_clk_ctrl = (PRCM_VPU_H1_CLK_CTRL);

        // Power Up VPU_H1
	//ns2815_vpu_h1_pwr_up();

        // Enable VPU_H1 clocks
        reg_value = PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLKEN;
        reg_set_bits(vpu_h1_clk_ctrl, reg_value);

        printf("vpu_h1 clock enabled.\n");
        return 0;
}

int ns2815_vpu_h1_clk_disable()
{
        unsigned int reg_value;
        volatile void *vpu_h1_clk_ctrl = (PRCM_VPU_H1_CLK_CTRL);

        // Disable VPU_H1 clocks
        reg_value = PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLKEN;
        reg_clr_bits(vpu_h1_clk_ctrl, reg_value);

        printf("vpu_h1 clock disabled.\n");

        // Power Down VPU_H1
	//ns2815_vpu_h1_pwr_down();

        return 0;
}

int ns2815_vpu_h1_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *vpu_h1_clk_ctrl = (PRCM_VPU_H1_CLK_CTRL);
        unsigned int vpu_h1_div0 = 0;
        unsigned int vpu_h1_div1 = 0;
        unsigned int vpu_h1_clk_sel, vpu_h1_clk_sel_cur;

        if (rate == 0) 
            return -EINVAL;
            
        switch (rate) {
        case 160000000:
            vpu_h1_clk_sel  = 0x0;
            vpu_h1_div0     = 0x3;
            break;
        case 200000000:
            vpu_h1_clk_sel  = 0x1;
            vpu_h1_div1     = 0x4;
            break;
        case 240000000:
            vpu_h1_clk_sel  = 0x0;
            vpu_h1_div0     = 0x2;
            break;
        default:
            return -EINVAL;
            break;
        }

        if (vpu_h1_clk_sel == 0) {
            clk_set_rate_bits(vpu_h1_clk_ctrl, PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV0_EN,
                              PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV0_MASK, vpu_h1_div0);
        }
        else {
            clk_set_rate_bits(vpu_h1_clk_ctrl, PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV1_EN,
                              PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV1_MASK, vpu_h1_div1);
        }

        vpu_h1_clk_sel_cur = *(unsigned int*)(vpu_h1_clk_ctrl) & PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLK_SEL;

        if ((vpu_h1_clk_sel << get_offset_pos(PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLK_SEL)) != vpu_h1_clk_sel_cur) {
            clk_set_rate_bits(vpu_h1_clk_ctrl, PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLKEN, 
                              PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLK_SEL, vpu_h1_clk_sel);
        }


        printf("vpu_h1 clock rate set to %ld.\n", rate);
        return 0;
}

// ISP JPEG clock control function/tasks
void ns2815_isp_pwr_up()
{
        sys_pwr_ctrl_set_bits(PRCM_SYS_PWR_CTRL_ISP_PWR_STATE, PRCM_SYS_PWR_STATUS_ISP_STATUS);
        printf("isp power up.\n");
}

void ns2815_isp_pwr_down()
{
        sys_pwr_ctrl_clr_bits(PRCM_SYS_PWR_CTRL_ISP_PWR_STATE, PRCM_SYS_PWR_STATUS_ISP_STATUS);
        printf("isp power down.\n");
}

int ns2815_isp_jpeg_clk_enable()
{
        unsigned int reg_value;
        volatile void *isp_jpeg_clk_ctrl = (PRCM_ISP_CLK_CTRL);

        // Power Up ISP 
	//ns2815_isp_pwr_up();

        // Enable ISP clocks
        reg_value = PRCM_ISP_CLK_CTRL_ISP_JPEGCLKEN |
                    PRCM_ISP_CLK_CTRL_ISP_S_HCLKEN  |
                    PRCM_ISP_CLK_CTRL_ISP_M_HCLKEN;
        reg_set_bits(isp_jpeg_clk_ctrl, reg_value);

        printf("isp_jpeg clock enabled.\n");
        return 0;
}

int ns2815_isp_jpeg_clk_disable()
{
        unsigned int reg_value;
        volatile void *isp_jpeg_clk_ctrl = (PRCM_ISP_CLK_CTRL);

        // Disable ISP clocks
        reg_value = PRCM_ISP_CLK_CTRL_ISP_JPEGCLKEN |
                    PRCM_ISP_CLK_CTRL_ISP_S_HCLKEN  |
                    PRCM_ISP_CLK_CTRL_ISP_M_HCLKEN;
        reg_clr_bits(isp_jpeg_clk_ctrl, reg_value);

        printf("isp_jpeg clock disabled.\n");

        // Power Down ISP 
	//ns2815_isp_pwr_down();

        printf("isp power down.\n");
        return 0;
}

int ns2815_isp_jpeg_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *isp_clk_ctrl = (PRCM_ISP_CLK_CTRL);
        unsigned int isp_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (isp_div > 0x0F)
            return -EINVAL;

        clk_set_rate_bits(isp_clk_ctrl, PRCM_ISP_CLK_CTRL_ISP_JPEG_DIV_EN,
                          PRCM_ISP_CLK_CTRL_ISP_JPEG_DIV_MASK, isp_div);
        
        rate = 800000000/isp_div;

        printf("isp_jpeg clock rate set to %ld.\n", rate);
        return 0;

}

// ISP clock control function/tasks
int ns2815_isp_clk_enable()
{
        unsigned int reg_value;
        volatile void *isp_clk_ctrl = (PRCM_ISP_CLK_CTRL);

        // Power Up ISP 
	//ns2815_isp_pwr_up();

        // Enable ISP clocks
        reg_value = PRCM_ISP_CLK_CTRL_ISP_CLKEN    |
                    PRCM_ISP_CLK_CTRL_ISP_S_HCLKEN |
                    PRCM_ISP_CLK_CTRL_ISP_M_HCLKEN;
        reg_set_bits(isp_clk_ctrl, reg_value);

        printf("isp clock enabled.\n");
        return 0;
}

int ns2815_isp_clk_disable()
{
        unsigned int reg_value;
        volatile void *isp_clk_ctrl = (PRCM_ISP_CLK_CTRL);

        // Disable ISP clocks
        reg_value = PRCM_ISP_CLK_CTRL_ISP_CLKEN    |
                    PRCM_ISP_CLK_CTRL_ISP_S_HCLKEN |
                    PRCM_ISP_CLK_CTRL_ISP_M_HCLKEN;
        reg_clr_bits(isp_clk_ctrl, reg_value);

        printf("isp clock disabled.\n");

        // Power Down ISP 
	//ns2815_isp_pwr_down();

        return 0;
}

// SD clock control function/tasks
int ns2815_sd_clk_enable()
{
        unsigned int reg_value;
        volatile void *sd_clk_ctrl = (PRCM_SD_CLK_CTRL);

        reg_value = PRCM_SD_CLK_CTRL_SD_CLKEN |
                    PRCM_SD_CLK_CTRL_SD_HCLKEN;
        reg_set_bits(sd_clk_ctrl, reg_value);

        printf("sd clock enabled.\n");
        return 0;
}

int ns2815_sd_clk_disable()
{
        unsigned int reg_value;
        volatile void *sd_clk_ctrl = (PRCM_SD_CLK_CTRL);

        reg_value = PRCM_SD_CLK_CTRL_SD_CLKEN |
                    PRCM_SD_CLK_CTRL_SD_HCLKEN;
        reg_clr_bits(sd_clk_ctrl, reg_value);

        printf("sd clock disabled.\n");
        return 0;
}

int ns2815_sd_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *sd_clk_ctrl = (PRCM_SD_CLK_CTRL);
        unsigned int sd_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (sd_div > 0x0F)
            return -EINVAL;

        clk_set_rate_bits(sd_clk_ctrl, PRCM_SD_CLK_CTRL_SD_DIV_EN,
                          PRCM_SD_CLK_CTRL_SD_DIV_MASK, sd_div);
        
        rate = 800000000/sd_div;

        printf("sd clock rate set to %ld.\n", rate);
        return 0;
}

// ZSP clock control function/tasks
void ns2815_zsp_pwr_up()
{
        sys_pwr_ctrl_set_bits(PRCM_SYS_PWR_CTRL_ZSP_PWR_STATE, PRCM_SYS_PWR_STATUS_ZSP_STATUS);
        printf("zsp power up.\n");
}

void ns2815_zsp_pwr_down()
{
        sys_pwr_ctrl_clr_bits(PRCM_SYS_PWR_CTRL_ZSP_PWR_STATE, PRCM_SYS_PWR_STATUS_ZSP_STATUS);
        printf("zsp power down.\n");
}

int ns2815_zsp_clk_enable()
{
        unsigned int reg_value;
        volatile void *zsp_clk_ctrl = (PRCM_ZSP_CLK_CTRL);

        // Power Up ZSP
	//ns2815_zsp_pwr_up();

        // Enable ZSP clocks
        reg_value = PRCM_ZSP_CLK_CTRL_ZSP_CLKEN |
                    PRCM_ZSP_CLK_CTRL_ZSP_ACLKEN;
        reg_set_bits(zsp_clk_ctrl, reg_value);

        printf("zsp clock enabled.\n");
        return 0;
}

int ns2815_zsp_clk_disable()
{
        unsigned int reg_value;
        volatile void *zsp_clk_ctrl = (PRCM_ZSP_CLK_CTRL);

        // Disable ZSP clocks
        reg_value = PRCM_ZSP_CLK_CTRL_ZSP_CLKEN |
                    PRCM_ZSP_CLK_CTRL_ZSP_ACLKEN;
        reg_clr_bits(zsp_clk_ctrl, reg_value);

        printf("zsp clock disabled.\n");

        // Power Down ZSP
	//ns2815_zsp_pwr_down();

        return 0;
}

// SPI0 clock control function/tasks
int ns2815_spi0_clk_enable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI0_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI0_PCLKEN;
        reg_set_bits(spi_clk_ctrl, reg_value);

        printf("spi0 clock enabled.\n");
        return 0;
}

int ns2815_spi0_clk_disable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI0_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI0_PCLKEN;
        reg_clr_bits(spi_clk_ctrl, reg_value);

        printf("spi0 clock disabled.\n");
        return 0;
}

int ns2815_spi01_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);
        unsigned int spi01_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (spi01_div > 0x3F)
            return -EINVAL;

        clk_set_rate_bits(spi_clk_ctrl, PRCM_SPI_CLK_CTRL_SPI01_DIV_EN,
                          PRCM_SPI_CLK_CTRL_SPI01_DIV_MASK, spi01_div);
        
        rate = 800000000/spi01_div;

        printf("spi01 clock rate set to %ld.\n", rate);
        return 0;
}

// SPI1 clock control function/tasks
int ns2815_spi1_clk_enable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI1_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI1_PCLKEN;
        reg_set_bits(spi_clk_ctrl, reg_value);

        printf("spi1 clock enabled.\n");
        return 0;
}

int ns2815_spi1_clk_disable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI1_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI1_PCLKEN;
        reg_clr_bits(spi_clk_ctrl, reg_value);

        printf("spi1 clock disabled.\n");
        return 0;
}

// SPI2 clock control function/tasks
int ns2815_spi2_clk_enable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI2_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI2_PCLKEN;
        reg_set_bits(spi_clk_ctrl, reg_value);

        printf("spi2 clock enabled.\n");
        return 0;
}

int ns2815_spi2_clk_disable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI2_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI2_PCLKEN;
        reg_clr_bits(spi_clk_ctrl, reg_value);

        printf("spi2 clock disabled.\n");
        return 0;
}

int ns2815_spi23_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);
        unsigned int spi23_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (spi23_div > 0x7)
            return -EINVAL;

        clk_set_rate_bits(spi_clk_ctrl, PRCM_SPI_CLK_CTRL_SPI2_DIV_EN,
                          PRCM_SPI_CLK_CTRL_SPI2_DIV_MASK, spi23_div);
        
        rate = 800000000/spi23_div;

        printf("spi23 clock rate set to %ld.\n", rate);
        return 0;
}

// SPI3 clock control function/tasks
int ns2815_spi3_clk_enable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI3_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI3_PCLKEN;
        reg_set_bits(spi_clk_ctrl, reg_value);

        printf("spi3 clock enabled.\n");
        return 0;
}

int ns2815_spi3_clk_disable()
{
        unsigned int reg_value;
        volatile void *spi_clk_ctrl = (PRCM_SPI_CLK_CTRL);

        reg_value = PRCM_SPI_CLK_CTRL_SPI3_CLKEN |
                    PRCM_SPI_CLK_CTRL_SPI3_PCLKEN;
        reg_clr_bits(spi_clk_ctrl, reg_value);

        printf("spi3 clock disabled.\n");
        return 0;
}

// I2C0 clock control function/tasks
int ns2815_i2c0_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C0_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C0_PCLKEN;
        reg_set_bits(i2c_clk_ctrl, reg_value);

        printf("i2c0 clock enabled.\n");
        return 0;
}

int ns2815_i2c0_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C0_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C0_PCLKEN;
        reg_clr_bits(i2c_clk_ctrl, reg_value);

        printf("i2c0 clock disabled.\n");
        return 0;
}

int ns2815_i2c_fs_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);
        unsigned int i2c_fs_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (i2c_fs_div > 0x3F)
            return -EINVAL;

        clk_set_rate_bits(i2c_clk_ctrl, PRCM_I2C_CLK_CTRL_I2C_FS_DIV_EN,
                          PRCM_I2C_CLK_CTRL_I2C_FS_DIV_MASK, i2c_fs_div);
        
        rate = 800000000/i2c_fs_div;

        printf("i2c_fs clock rate set to %ld.\n", rate);
        return 0;
}

// I2C1 clock control function/tasks
int ns2815_i2c1_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C1_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C1_PCLKEN;
        reg_set_bits(i2c_clk_ctrl, reg_value);

        printf("i2c1 clock enabled.\n");
        return 0;
}

int ns2815_i2c1_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C1_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C1_PCLKEN;
        reg_clr_bits(i2c_clk_ctrl, reg_value);

        printf("i2c1 clock disabled.\n");
        return 0;
}

// I2C2 clock control function/tasks
int ns2815_i2c2_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C2_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C2_PCLKEN;
        reg_set_bits(i2c_clk_ctrl, reg_value);

        printf("i2c2 clock enabled.\n");
        return 0;
}

int ns2815_i2c2_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C2_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C2_PCLKEN;
        reg_clr_bits(i2c_clk_ctrl, reg_value);

        printf("i2c2 clock disabled.\n");
        return 0;
}

// I2C3 clock control function/tasks
int ns2815_i2c3_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C3_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C3_PCLKEN;
        reg_set_bits(i2c_clk_ctrl, reg_value);

        printf("i2c3 clock enabled.\n");
        return 0;
}

int ns2815_i2c3_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);

        reg_value = PRCM_I2C_CLK_CTRL_I2C3_CLKEN |
                    PRCM_I2C_CLK_CTRL_I2C3_PCLKEN;
        reg_clr_bits(i2c_clk_ctrl, reg_value);

        printf("i2c3 clock disabled.\n");
        return 0;
}

int ns2815_i2c_hs_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *i2c_clk_ctrl = (PRCM_I2C_CLK_CTRL);
        unsigned int i2c_hs_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (i2c_hs_div > 0x0F)
            return -EINVAL;

        clk_set_rate_bits(i2c_clk_ctrl, PRCM_I2C_CLK_CTRL_I2C_HS_DIV_EN,
                          PRCM_I2C_CLK_CTRL_I2C_HS_DIV_MASK, i2c_hs_div);
        
        rate = 800000000/i2c_hs_div;

        printf("i2c_hs clock rate set to %ld.\n", rate);
        return 0;
}

// UART0 clock control function/tasks
int ns2815_uart0_clk_enable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART0_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART0_PCLKEN;
        reg_set_bits(uart_clk_ctrl, reg_value);

        printf("uart0 clock enabled.\n");
        return 0;
}

int ns2815_uart0_clk_disable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART0_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART0_PCLKEN;
        reg_clr_bits(uart_clk_ctrl, reg_value);

        printf("uart0 clock disabled.\n");
        return 0;
}

int ns2815_uart01_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *uart_clk_ctrl0 = (PRCM_UART_CLK_CTRL0);
        unsigned int uart01_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (uart01_div > 0x1F)
            return -EINVAL;

        clk_set_rate_bits(uart_clk_ctrl0, PRCM_UART_CLK_CTRL0_UART01_DIV_EN,
                          PRCM_UART_CLK_CTRL0_UART01_DIV_MASK, uart01_div);
        
        rate = 800000000/uart01_div;

        printf("uart01 clock rate set to %ld.\n", rate);
        return 0;
}

// UART1 clock control function/tasks
int ns2815_uart1_clk_enable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART1_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART1_PCLKEN;
        reg_set_bits(uart_clk_ctrl, reg_value);

        printf("uart1 clock enabled.\n");
        return 0;
}

int ns2815_uart1_clk_disable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART1_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART1_PCLKEN;
        reg_clr_bits(uart_clk_ctrl, reg_value);

        printf("uart1 clock disabled.\n");
        return 0;
}

// UART2 clock control function/tasks
int ns2815_uart2_clk_enable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART2_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART2_PCLKEN;
        reg_set_bits(uart_clk_ctrl, reg_value);

        printf("uart2 clock enabled.\n");
        return 0;
}

int ns2815_uart2_clk_disable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART2_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART2_PCLKEN;
        reg_clr_bits(uart_clk_ctrl, reg_value);

        printf("uart2 clock disabled.\n");
        return 0;
}

int ns2815_uart2_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *uart_clk_ctrl0 = (PRCM_UART_CLK_CTRL0);
        unsigned int uart2_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (uart2_div > 0x1F)
            return -EINVAL;

        clk_set_rate_bits(uart_clk_ctrl0, PRCM_UART_CLK_CTRL0_UART2_DIV_EN,
                          PRCM_UART_CLK_CTRL0_UART2_DIV_MASK, uart2_div);
        
        rate = 800000000/uart2_div;

        printf("uart2 clock rate set to %ld.\n", rate);
        return 0;
}

// UART3 clock control function/tasks
int ns2815_uart3_clk_enable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART3_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART3_PCLKEN;
        reg_set_bits(uart_clk_ctrl, reg_value);

        printf("uart3 clock enabled.\n");
        return 0;
}

int ns2815_uart3_clk_disable()
{
        unsigned int reg_value;
        volatile void *uart_clk_ctrl = (PRCM_UART_CLK_CTRL1);

        reg_value = PRCM_UART_CLK_CTRL1_UART3_CLKEN |
                    PRCM_UART_CLK_CTRL1_UART3_PCLKEN;
        reg_clr_bits(uart_clk_ctrl, reg_value);

        printf("uart3 clock disabled.\n");
        return 0;
}

int ns2815_uart3_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *uart_clk_ctrl0 = (PRCM_UART_CLK_CTRL0);
        unsigned int uart3_div = 800000000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (uart3_div > 0x1F)
            return -EINVAL;

        clk_set_rate_bits(uart_clk_ctrl0, PRCM_UART_CLK_CTRL0_UART3_DIV_EN,
                          PRCM_UART_CLK_CTRL0_UART3_DIV_MASK, uart3_div);
        
        rate = 800000000/uart3_div;

        printf("uart3 clock rate set to %ld.\n", rate);
        return 0;
}

// RGBO clock control function/tasks
int ns2815_rgbo_clk_enable()
{
        unsigned int reg_value;
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);

        reg_value = PRCM_DIS0_CLK_CTRL_RGBO_CLKEN;
        reg_set_bits(dis0_clk_ctrl, reg_value);

        printf("rgbo clock enabled.\n");
        return 0;
}

int ns2815_rgbo_clk_disable()
{
        unsigned int reg_value;
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);

        reg_value =PRCM_DIS0_CLK_CTRL_RGBO_CLKEN;
        reg_clr_bits(dis0_clk_ctrl, reg_value);

        printf("rgbo clock disabled.\n");
        return 0;
}

int ns2815_pxl0_clk_set_rate(unsigned long rate, unsigned int rgbo_inv_clk_sel)
{
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);
        unsigned int pxl0_div = 2;
	unsigned int nouse = 0;

        if (rate == 0) 
            return -EINVAL;

        if (pxl0_div > 0x0F)
            return -EINVAL;

	pll3_rate = rate*pxl0_div;
	ns2815_pll3_clk_set_rate(pll3_rate, nouse);

        clk_set_rate_bits(dis0_clk_ctrl, PRCM_DIS0_CLK_CTRL_PXL0_DIV_EN,
                          PRCM_DIS0_CLK_CTRL_PXL0_DIV_MASK, pxl0_div);

        clk_set_rate_bits(dis0_clk_ctrl, PRCM_DIS0_CLK_CTRL_RGBO_CLKEN,
                          PRCM_DIS0_CLK_CTRL_RGBO_INV_CLK_SEL, rgbo_inv_clk_sel);
        
        printf("pxl0 clock rate set to %ld.\n", rate);
        return 0;
}

// LCDC0 clock control function/tasks
int ns2815_lcdc0_clk_enable()
{
        unsigned int reg_value;
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);

        reg_value = PRCM_DIS0_CLK_CTRL_LCDC0_CLKEN  ;
        reg_set_bits(dis0_clk_ctrl, reg_value);

        printf("lcdc0 clock enabled.\n");
        return 0;
}

int ns2815_lcdc0_clk_disable()
{
        unsigned int reg_value;
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);

        reg_value = PRCM_DIS0_CLK_CTRL_LCDC0_CLKEN  ;
        reg_clr_bits(dis0_clk_ctrl, reg_value);

        printf("lcdc0 clock disabled.\n");
        return 0;
}

int ns2815_lcdc0bus_clk_enable()
{
        unsigned int reg_value;
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);

        reg_value = PRCM_DIS0_CLK_CTRL_LCDC0_HCLKEN |
                    PRCM_DIS0_CLK_CTRL_LCDC0_ACLKEN;
        reg_set_bits(dis0_clk_ctrl, reg_value);

        printf("lcdc0 clock enabled.\n");
        return 0;
}

int ns2815_lcdc0bus_clk_disable()
{
        unsigned int reg_value;
        volatile void *dis0_clk_ctrl = (PRCM_DIS0_CLK_CTRL);

        reg_value = PRCM_DIS0_CLK_CTRL_LCDC0_HCLKEN |
                    PRCM_DIS0_CLK_CTRL_LCDC0_ACLKEN;
        reg_clr_bits(dis0_clk_ctrl, reg_value);

        printf("lcdc0 clock disabled.\n");
        return 0;
}

// LCDC1 clock control function/tasks
int ns2815_lcdc1_clk_enable()
{
        unsigned int reg_value;
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);

        reg_value = PRCM_DIS1_CLK_CTRL_LCDC1_CLKEN  ;
        reg_set_bits(dis1_clk_ctrl, reg_value);

        printf("lcdc1 clock enabled.\n");
        return 0;
}

int ns2815_lcdc1_clk_disable()
{
        unsigned int reg_value;
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);

        reg_value = PRCM_DIS1_CLK_CTRL_LCDC1_CLKEN  ;
        reg_clr_bits(dis1_clk_ctrl, reg_value);

        printf("lcdc1 clock disabled.\n");
        return 0;
}

int ns2815_lcdc1bus_clk_enable()
{
        unsigned int reg_value;
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);

        reg_value = PRCM_DIS1_CLK_CTRL_LCDC1_HCLKEN |
                    PRCM_DIS1_CLK_CTRL_LCDC1_ACLKEN;
        reg_set_bits(dis1_clk_ctrl, reg_value);

        printf("lcdc1 clock enabled.\n");
        return 0;
}

int ns2815_lcdc1bus_clk_disable()
{
        unsigned int reg_value;
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);

        reg_value = PRCM_DIS1_CLK_CTRL_LCDC1_HCLKEN |
                    PRCM_DIS1_CLK_CTRL_LCDC1_ACLKEN;
        reg_clr_bits(dis1_clk_ctrl, reg_value);

        printf("lcdc1 clock disabled.\n");
        return 0;
}

int ns2815_pxl1_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);
        unsigned int pxl1_div = 2;

        if (rate == 0) 
            return -EINVAL;

        if (pxl1_div > 0x0F)
            return -EINVAL;

	pll4_rate = rate*pxl1_div;
	ns2815_pll4_clk_set_rate(pll4_rate, nouse);

        clk_set_rate_bits(dis1_clk_ctrl, PRCM_DIS1_CLK_CTRL_PXL1_DIV_EN,
                          PRCM_DIS1_CLK_CTRL_PXL1_DIV_MASK, pxl1_div);
        

        printf("pxl1 clock rate set to %ld.\n", rate);
        return 0;
}

// HDMI_PXL clock control function/tasks
int ns2815_hdmi_pxl_clk_enable()
{
        unsigned int reg_value;
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);

        reg_value = PRCM_DIS1_CLK_CTRL_HDMI_PXL_CLKEN |
                    PRCM_DIS1_CLK_CTRL_HDMI_CEC_CLKEN |
                    PRCM_DIS1_CLK_CTRL_HDMI_BCLKEN;
        reg_set_bits(dis1_clk_ctrl, reg_value);

        printf("hdmi_pxl clock enabled.\n");
        return 0;
}

int ns2815_hdmi_pxl_clk_disable()
{
        unsigned int reg_value;
        volatile void *dis1_clk_ctrl = (PRCM_DIS1_CLK_CTRL);

        reg_value = PRCM_DIS1_CLK_CTRL_HDMI_PXL_CLKEN |
                    PRCM_DIS1_CLK_CTRL_HDMI_CEC_CLKEN |
                    PRCM_DIS1_CLK_CTRL_HDMI_BCLKEN;
        reg_clr_bits(dis1_clk_ctrl, reg_value);

        printf("hdmi_pxl clock disabled.\n");
        return 0;
}

// I2S0 clock control function/tasks
int ns2815_i2s0_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);

        reg_value = PRCM_I2S_CLK_CTRL_I2S0_CLKEN |
                    PRCM_I2S_CLK_CTRL_I2S0_PCLKEN;
        reg_set_bits(i2s_clk_ctrl, reg_value);

        printf("i2s0 clock enabled.\n");
        return 0;
}

int ns2815_i2s0_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);

        reg_value = PRCM_I2S_CLK_CTRL_I2S0_CLKEN |
                    PRCM_I2S_CLK_CTRL_I2S0_PCLKEN;
        reg_clr_bits(i2s_clk_ctrl, reg_value);

        printf("i2s0 clock disabled.\n");
        return 0;
}

int ns2815_i2s_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);
        unsigned long pll5_clk_rate = pll5_rate;
        unsigned int i2s_div = pll5_clk_rate/rate;

        if (rate == 0) 
            return -EINVAL;

        if (i2s_div > 0x0F)
            return -EINVAL;

        clk_set_rate_bits(i2s_clk_ctrl, PRCM_I2S_CLK_CTRL_I2S_DIV_EN,
                          PRCM_I2S_CLK_CTRL_I2S_DIV_MASK, i2s_div);
        
        rate = pll5_clk_rate/i2s_div;

        printf("i2s clock rate set to %ld.\n", rate);

	i2s0_rate = rate;
        return 0;
}

// MCLK clock control function/tasks
int ns2815_mclk_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);

        reg_value = PRCM_I2S_CLK_CTRL_MCLKEN;
        reg_set_bits(i2s_clk_ctrl, reg_value);

        printf("i2s0 clock enabled.\n");
        return 0;
}

int ns2815_mclk_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);

        reg_value = PRCM_I2S_CLK_CTRL_MCLKEN;
        reg_clr_bits(i2s_clk_ctrl, reg_value);

        printf("i2s0 clock disabled.\n");
        return 0;
}

// I2S1 clock control function/tasks
int ns2815_i2s1_clk_enable()
{
        unsigned int reg_value;
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);

        reg_value = PRCM_I2S_CLK_CTRL_I2S1_CLKEN  |
                    PRCM_I2S_CLK_CTRL_I2S1_PCLKEN ;
        reg_set_bits(i2s_clk_ctrl, reg_value);

        printf("i2s1 clock enabled.\n");
        return 0;
}

int ns2815_i2s1_clk_disable()
{
        unsigned int reg_value;
        volatile void *i2s_clk_ctrl = (PRCM_I2S_CLK_CTRL);

        reg_value = PRCM_I2S_CLK_CTRL_I2S1_CLKEN  |
                    PRCM_I2S_CLK_CTRL_I2S1_PCLKEN ;
        reg_clr_bits(i2s_clk_ctrl, reg_value);

        printf("i2s1 clock disabled.\n");
        return 0;
}

// USB OTG clock control function/tasks
int ns2815_usb_otg_clk_enable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_OTG_HCLKEN |
                    PRCM_USB_CLK_CTRL_USB_REF_CLKEN;
        reg_set_bits(usb_clk_ctrl, reg_value);

        printf("usb otg clock enabled.\n");
        return 0;
}

int ns2815_usb_otg_clk_disable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_OTG_HCLKEN |
                    PRCM_USB_CLK_CTRL_USB_REF_CLKEN;
        reg_clr_bits(usb_clk_ctrl, reg_value);

        printf("usb otg clock disabled.\n");
        return 0;
}

// USB UTMI clock control function/tasks
int ns2815_usb_utmi_clk_enable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_UTMI_PHY_CLKEN |
                    PRCM_USB_CLK_CTRL_USB2_HCLKEN;
        reg_set_bits(usb_clk_ctrl, reg_value);

        printf("usb utmi clock enabled.\n");
        return 0;
}

int ns2815_usb_utmi_clk_disable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_UTMI_PHY_CLKEN |
                    PRCM_USB_CLK_CTRL_USB2_HCLKEN;
        reg_clr_bits(usb_clk_ctrl, reg_value);

        printf("usb utmi clock disabled.\n");
        return 0;
}

// USB OHCI clock control function/tasks
int ns2815_usb_ohci_clk_enable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_OHCI_48M_CLKEN |
                    PRCM_USB_CLK_CTRL_USB2_HCLKEN;
        reg_set_bits(usb_clk_ctrl, reg_value);

        printf("usb ohci clock enabled.\n");
        return 0;
}

int ns2815_usb_ohci_clk_disable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_OHCI_48M_CLKEN |
                    PRCM_USB_CLK_CTRL_USB2_HCLKEN;
        reg_clr_bits(usb_clk_ctrl, reg_value);

        printf("usb ohci clock disabled.\n");
        return 0;
}

// USB HSIC clock control function/tasks
int ns2815_usb_hsic_clk_enable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_HSIC_480M_CLKEN |
                    PRCM_USB_CLK_CTRL_USB2_HCLKEN;
        reg_set_bits(usb_clk_ctrl, reg_value);

        printf("usb hsic clock enabled.\n");
        return 0;
}

int ns2815_usb_hsic_clk_disable()
{
        unsigned int reg_value;
        volatile void *usb_clk_ctrl = (PRCM_USB_CLK_CTRL);

        reg_value = PRCM_USB_CLK_CTRL_HSIC_480M_CLKEN |
                    PRCM_USB_CLK_CTRL_USB2_HCLKEN;
        reg_clr_bits(usb_clk_ctrl, reg_value);

        printf("usb hsic clock disabled.\n");
        return 0;
}

// CORE MEM clock control function/tasks
int ns2815_core_mem_clk_enable()
{
        unsigned int reg_value;
        volatile void *core_mem_clk_ctrl = (PRCM_CORE_MEM_CLK_CTRL);

        reg_value = PRCM_CORE_MEM_CLK_CTRL_ROM_HCLKEN;
        reg_set_bits(core_mem_clk_ctrl, reg_value);

        printf("core rom clock enabled.\n");
        return 0;
}

int ns2815_core_mem_clk_disable()
{
        unsigned int reg_value;
        volatile void *core_mem_clk_ctrl = (PRCM_CORE_MEM_CLK_CTRL);

        reg_value =PRCM_CORE_MEM_CLK_CTRL_ROM_HCLKEN;
        reg_clr_bits(core_mem_clk_ctrl, reg_value);

        printf("core rom clock disabled.\n");
        return 0;
}

// GPIO CORE clock control function/tasks
int ns2815_gpio_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *gpio_core_clk_ctrl = (PRCM_GPIO_CORE_CLK_CTRL);

        reg_value = PRCM_GPIO_CORE_CLK_CTRL_GPIO_CORE_PCLKEN;
        reg_set_bits(gpio_core_clk_ctrl, reg_value);

        printf("gpio core clock enabled.\n");
        return 0;
}

int ns2815_gpio_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *gpio_core_clk_ctrl = (PRCM_GPIO_CORE_CLK_CTRL);

        reg_value =PRCM_GPIO_CORE_CLK_CTRL_GPIO_CORE_PCLKEN;
        reg_clr_bits(gpio_core_clk_ctrl, reg_value);

        printf("gpio core clock disabled.\n");
        return 0;
}

// TIMER0 CORE clock control function/tasks
int ns2815_timer0_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER0_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer0 core clock enabled.\n");
        return 0;
}

int ns2815_timer0_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER0_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer0 core clock disabled.\n");
        return 0;
}

int ns2815_timer0_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;

        unsigned int timer0_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer0_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer0_core_clk_sel = 0x0;
        }
        else {
            printf("timer0 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER0_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER0_CORE_CLK_SEL, timer0_core_clk_sel);

        printf("timer0 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER1 CORE clock control function/tasks
int ns2815_timer1_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER1_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer1 core clock enabled.\n");
        return 0;
}

int ns2815_timer1_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER1_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer1 core clock disabled.\n");
        return 0;
}

int ns2815_timer1_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer1_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer1_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer1_core_clk_sel = 0x0;
        }
        else {
            printf("timer1 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER1_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER1_CORE_CLK_SEL, timer1_core_clk_sel);

        printf("timer1 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER2 CORE clock control function/tasks
int ns2815_timer2_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER2_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer2 core clock enabled.\n");
        return 0;
}

int ns2815_timer2_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER2_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer2 core clock disabled.\n");
        return 0;
}

int ns2815_timer2_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer2_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer2_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer2_core_clk_sel = 0x0;
        }
        else {
            printf("timer2 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER2_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER2_CORE_CLK_SEL, timer2_core_clk_sel);

        printf("timer2 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER3 CORE clock control function/tasks
int ns2815_timer3_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER3_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer3 core clock enabled.\n");
        return 0;
}

int ns2815_timer3_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER3_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer3 core clock disabled.\n");
        return 0;
}

int ns2815_timer3_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer3_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer3_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer3_core_clk_sel = 0x0;
        }
        else {
            printf("timer3 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER3_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER3_CORE_CLK_SEL, timer3_core_clk_sel);

        printf("timer3 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER4 CORE clock control function/tasks
int ns2815_timer4_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER4_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer4 core clock enabled.\n");
        return 0;
}

int ns2815_timer4_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER4_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer4 core clock disabled.\n");
        return 0;
}

int ns2815_timer4_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer4_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer4_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer4_core_clk_sel = 0x0;
        }
        else {
            printf("timer4 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER4_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER4_CORE_CLK_SEL, timer4_core_clk_sel);

        printf("timer4 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER5 CORE clock control function/tasks
int ns2815_timer5_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER5_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer5 core clock enabled.\n");
        return 0;
}

int ns2815_timer5_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER5_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer5 core clock disabled.\n");
        return 0;
}

int ns2815_timer5_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer5_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer5_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer5_core_clk_sel = 0x0;
        }
        else {
            printf("timer5 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER5_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER5_CORE_CLK_SEL, timer5_core_clk_sel);

        printf("timer5 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER6 CORE clock control function/tasks
int ns2815_timer6_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER6_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer6 core clock enabled.\n");
        return 0;
}

int ns2815_timer6_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER6_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer6 core clock disabled.\n");
        return 0;
}

int ns2815_timer6_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer6_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer6_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer6_core_clk_sel = 0x0;
        }
        else {
            printf("timer6 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER6_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER6_CORE_CLK_SEL, timer6_core_clk_sel);

        printf("timer6 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER7 CORE clock control function/tasks
int ns2815_timer7_core_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER7_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_set_bits(timer_core_clk_ctrl, reg_value);

        printf("timer7 core clock enabled.\n");
        return 0;
}

int ns2815_timer7_core_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);

        reg_value = PRCM_TIMER_CORE_CLK_CTRL_TIMER7_CORE_CLKEN |
                    PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN;
        reg_clr_bits(timer_core_clk_ctrl, reg_value);

        printf("timer7 core clock disabled.\n");
        return 0;
}

int ns2815_timer7_core_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_core_clk_ctrl = (PRCM_TIMER_CORE_CLK_CTRL);
        unsigned long i2s0_clk_rate = i2s0_rate;
        unsigned int timer7_core_clk_sel;

        if (rate == i2s0_clk_rate/8) {
            timer7_core_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer7_core_clk_sel = 0x0;
        }
        else {
            printf("timer7 clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }


        clk_set_rate_bits(timer_core_clk_ctrl, PRCM_TIMER_CORE_CLK_CTRL_TIMER7_CORE_CLKEN,
                          PRCM_TIMER_CORE_CLK_CTRL_TIMER7_CORE_CLK_SEL, timer7_core_clk_sel);

        printf("timer7 clock rate set to %ld.\n", rate);
        return 0;
}

// WKUP MEM clock control function/tasks
int ns2815_wkup_mem_clk_enable()
{
        unsigned int reg_value;
        volatile void *wkup_mem_clk_ctrl = (PRCM_WKUP_MEM_CLK_CTRL);

        reg_value = PRCM_WKUP_MEM_CLK_CTRL_RAM_WKUP_HCLKEN;
        reg_set_bits(wkup_mem_clk_ctrl, reg_value);

        printf("wkup_mem clock enabled.\n");
        return 0;
}

int ns2815_wkup_mem_clk_disable()
{
        unsigned int reg_value;
        volatile void *wkup_mem_clk_ctrl = (PRCM_WKUP_MEM_CLK_CTRL);

        reg_value =PRCM_WKUP_MEM_CLK_CTRL_RAM_WKUP_HCLKEN;
        reg_clr_bits(wkup_mem_clk_ctrl, reg_value);

        printf("wkup_mem clock disabled.\n");
        return 0;
}

// GPIO WKUP clock control function/tasks
int ns2815_gpio_wkup_clk_enable()
{
        unsigned int reg_value;
        volatile void *gpio_wkup_clk_ctrl = (PRCM_GPIO_WKUP_CLK_CTRL);

        reg_value = PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLKEN |
                    PRCM_GPIO_WKUP_CLK_CTRL_GPIO_WKUP_PCLKEN;
        reg_set_bits(gpio_wkup_clk_ctrl, reg_value);

        printf("gpio_wkup clock enabled.\n");
        return 0;
}

int ns2815_gpio_wkup_clk_disable()
{
        unsigned int reg_value;
        volatile void *gpio_wkup_clk_ctrl = (PRCM_GPIO_WKUP_CLK_CTRL);

        reg_value = PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLKEN |
                    PRCM_GPIO_WKUP_CLK_CTRL_GPIO_WKUP_PCLKEN;
        reg_clr_bits(gpio_wkup_clk_ctrl, reg_value);

        printf("gpio_wkup clock disabled.\n");
        return 0;
}

int ns2815_gpio_wkup_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *gpio_wkup_clk_ctrl = (PRCM_GPIO_WKUP_CLK_CTRL);
        unsigned int gpio_wkup_clk_div = 32000/rate;

        if (rate == 0) 
            return -EINVAL;

        if (gpio_wkup_clk_div >0x0FF)
            return -EINVAL;

        clk_set_rate_bits(gpio_wkup_clk_ctrl, PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLK_32K_DIV_EN,
                          PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLK_32K_DIV_MASK, gpio_wkup_clk_div);

        rate = 32000/gpio_wkup_clk_div;

        printf("gpio_wkup clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER0 WKUP clock control function/tasks
int ns2815_timer0_wkup_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_wkup_clk_ctrl = (PRCM_TIMER_WKUP_CLK_CTRL);

        reg_value = PRCM_TIMER_WKUP_CLK_CTRL_TIMER0_WKUP_CLKEN |
                    PRCM_TIMER_WKUP_CLK_CTRL_TIMER_WKUP_PCLKEN;
        reg_set_bits(timer_wkup_clk_ctrl, reg_value);

        printf("timer0_wkup clock enabled.\n");
        return 0;
}

int ns2815_timer0_wkup_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_wkup_clk_ctrl = (PRCM_TIMER_WKUP_CLK_CTRL);

        reg_value = PRCM_TIMER_WKUP_CLK_CTRL_TIMER0_WKUP_CLKEN |
                    PRCM_TIMER_WKUP_CLK_CTRL_TIMER_WKUP_PCLKEN;
        reg_clr_bits(timer_wkup_clk_ctrl, reg_value);

        printf("timer0_wkup clock disabled.\n");
        return 0;
}

int ns2815_timer0_wkup_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_wkup_clk_ctrl = (PRCM_TIMER_WKUP_CLK_CTRL);
        unsigned int timer0_wkup_clk_sel;

        if (rate == 32000) {
            timer0_wkup_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer0_wkup_clk_sel = 0x0;
        }
        else {
            printf("timer0_wkup clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_wkup_clk_ctrl, PRCM_TIMER_WKUP_CLK_CTRL_TIMER0_WKUP_CLKEN,
                          PRCM_TIMER_WKUP_CLK_CTRL_TIMER0_WKUP_CLK_SEL, timer0_wkup_clk_sel);

        printf("timer0 clock rate set to %ld.\n", rate);
        return 0;
}

// TIMER1 WKUP clock control function/tasks
int ns2815_timer1_wkup_clk_enable()
{
        unsigned int reg_value;
        volatile void *timer_wkup_clk_ctrl = (PRCM_TIMER_WKUP_CLK_CTRL);

        reg_value = PRCM_TIMER_WKUP_CLK_CTRL_TIMER1_WKUP_CLKEN |
                    PRCM_TIMER_WKUP_CLK_CTRL_TIMER_WKUP_PCLKEN;
        reg_set_bits(timer_wkup_clk_ctrl, reg_value);

        printf("timer1_wkup clock enabled.\n");
        return 0;
}

int ns2815_timer1_wkup_clk_disable()
{
        unsigned int reg_value;
        volatile void *timer_wkup_clk_ctrl = (PRCM_TIMER_WKUP_CLK_CTRL);

        reg_value = PRCM_TIMER_WKUP_CLK_CTRL_TIMER1_WKUP_CLKEN |
                    PRCM_TIMER_WKUP_CLK_CTRL_TIMER_WKUP_PCLKEN;
        reg_clr_bits(timer_wkup_clk_ctrl, reg_value);

        printf("timer1_wkup clock disabled.\n");
        return 0;
}

int ns2815_timer1_wkup_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *timer_wkup_clk_ctrl = (PRCM_TIMER_WKUP_CLK_CTRL);
        unsigned int timer1_wkup_clk_sel;

        if (rate == 32000) {
            timer1_wkup_clk_sel = 0x1;
        }
        else if (rate == 12000000) {
            timer1_wkup_clk_sel = 0x0;
        }
        else {
            printf("timer1_wkup clock rate cannot set to %ld.\n", rate);
            return -EINVAL;
        }

        clk_set_rate_bits(timer_wkup_clk_ctrl, PRCM_TIMER_WKUP_CLK_CTRL_TIMER1_WKUP_CLKEN,
                          PRCM_TIMER_WKUP_CLK_CTRL_TIMER1_WKUP_CLK_SEL, timer1_wkup_clk_sel);

        printf("timer1 clock rate set to %ld.\n", rate);
        return 0;
}

// ALT0 clock control function/tasks
int ns2815_alt0_clk_enable()
{
        unsigned int reg_value;
        volatile void *alt_clk_ctrl = (PRCM_ALT_CLK_CTRL);

        reg_value = PRCM_ALT_CLK_CTRL_ALT0_CLKEN;
        reg_set_bits(alt_clk_ctrl, reg_value);

        printf("alt0 clock enabled.\n");
        return 0;
}

int ns2815_alt0_clk_disable()
{
        unsigned int reg_value;
        volatile void *alt_clk_ctrl = (PRCM_ALT_CLK_CTRL);

        reg_value = PRCM_ALT_CLK_CTRL_ALT0_CLKEN;
        reg_clr_bits(alt_clk_ctrl, reg_value);

        printf("alt0 clock disabled.\n");
        return 0;
}

int ns2815_alt0_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *alt_clk_ctrl = (PRCM_ALT_CLK_CTRL);
        unsigned long pll6_clk_rate = pll6_rate;
        unsigned int alt0_div = pll6_clk_rate/rate;

        if (rate == 0) 
            return -EINVAL;

        if (alt0_div >0x7FF)
            return -EINVAL;

        clk_set_rate_bits(alt_clk_ctrl, PRCM_ALT_CLK_CTRL_ALT0_DIV_EN,
                          PRCM_ALT_CLK_CTRL_ALT0_DIV_MASK, alt0_div);

        rate = pll6_clk_rate/alt0_div;

        printf("alt clock rate set to %ld.\n", rate);
        return 0;
}

// ALT1 clock control function/tasks
int ns2815_alt1_clk_enable()
{
        unsigned int reg_value;
        volatile void *alt_clk_ctrl = (PRCM_ALT_CLK_CTRL);

        reg_value = PRCM_ALT_CLK_CTRL_ALT1_CLKEN;
        reg_set_bits(alt_clk_ctrl, reg_value);

        printf("alt1 clock enabled.\n");
        return 0;
}

int ns2815_alt1_clk_disable()
{
        unsigned int reg_value;
        volatile void *alt_clk_ctrl = (PRCM_ALT_CLK_CTRL);

        reg_value = PRCM_ALT_CLK_CTRL_ALT1_CLKEN;
        reg_clr_bits(alt_clk_ctrl, reg_value);

        printf("alt1 clock disabled.\n");
        return 0;
}

int ns2815_alt1_clk_set_rate(unsigned long rate, unsigned int nouse)
{
        volatile void *alt_clk_ctrl = (PRCM_ALT_CLK_CTRL);
        unsigned long pll6_clk_rate = pll6_rate;
        unsigned int alt1_clk_sel, alt1_clk_sel_cur;
        unsigned int alt1_div = 0;

        if (rate == 0) 
            return -EINVAL;

        if (rate == 32000) {
            alt1_clk_sel = 0x1;
        }
        else {
            alt1_clk_sel = 0x0;
            alt1_div = pll6_clk_rate/rate;
            rate = pll6_clk_rate/alt1_div;
        }

        if (alt1_div >0x7FF)
            return -EINVAL;

        if (alt1_clk_sel == 0x0) {
            clk_set_rate_bits(alt_clk_ctrl, PRCM_ALT_CLK_CTRL_ALT1_DIV_EN,
                              PRCM_ALT_CLK_CTRL_ALT1_DIV_MASK, alt1_div);
        }

        alt1_clk_sel_cur = *(unsigned int*)(alt_clk_ctrl) & PRCM_ALT_CLK_CTRL_ALT1_32K_CLK_SEL;

        if ((alt1_clk_sel << get_offset_pos(PRCM_ALT_CLK_CTRL_ALT1_32K_CLK_SEL)) != alt1_clk_sel_cur) {
            clk_set_rate_bits(alt_clk_ctrl, PRCM_ALT_CLK_CTRL_ALT1_CLKEN, 
                              PRCM_ALT_CLK_CTRL_ALT1_32K_CLK_SEL, alt1_clk_sel);
        }


        printf("alt1 clock rate set to %ld.\n", rate);
        return 0;
}

// AUX clock control function/tasks
int ns2815_aux_clk_enable()
{
        unsigned int reg_value;
        volatile void *aux_clk_ctrl = (PRCM_AUX_CLK_CTRL);

        reg_value = PRCM_AUX_CLK_CTRL_AUX_PCLKEN;
        reg_set_bits(aux_clk_ctrl, reg_value);

        printf("aux clock enabled.\n");
        return 0;
}

int ns2815_aux_clk_disable()
{
        unsigned int reg_value;
        volatile void *aux_clk_ctrl = (PRCM_AUX_CLK_CTRL);

        reg_value = PRCM_AUX_CLK_CTRL_AUX_PCLKEN;
        reg_clr_bits(aux_clk_ctrl, reg_value);

        printf("aux clock disabled.\n");
        return 0;
}

// EFUSE clock control function/tasks
int ns2815_efuse_clk_enable()
{
        unsigned int reg_value;
        volatile void *aux_clk_ctrl = (PRCM_AUX_CLK_CTRL);

        reg_value = PRCM_AUX_CLK_CTRL_EFUSE_CLKEN;
        reg_set_bits(aux_clk_ctrl, reg_value);

        printf("efuse clock enabled.\n");
        return 0;
}

int ns2815_efuse_clk_disable()
{
        unsigned int reg_value;
        volatile void *aux_clk_ctrl = (PRCM_AUX_CLK_CTRL);

        reg_value = PRCM_AUX_CLK_CTRL_EFUSE_CLKEN;
        reg_clr_bits(aux_clk_ctrl, reg_value);

        printf("efuse clock disabled.\n");
        return 0;
}

// PWM clock control function/tasks
int ns2815_pwm_clk_enable()
{
        unsigned int reg_value;
        volatile void *aux_clk_ctrl = (PRCM_AUX_CLK_CTRL);

        reg_value = PRCM_AUX_CLK_CTRL_PWM_CLKEN;
        reg_set_bits(aux_clk_ctrl, reg_value);

        printf("pwm clock enabled.\n");
        return 0;
}

int ns2815_pwm_clk_disable()
{
        unsigned int reg_value;
        volatile void *aux_clk_ctrl = (PRCM_AUX_CLK_CTRL);

        reg_value = PRCM_AUX_CLK_CTRL_PWM_CLKEN;
        reg_clr_bits(aux_clk_ctrl, reg_value);

        printf("pwm clock disabled.\n");
        return 0;
}

// CPU clock gating off
int ns2815_cpu_clk_enable()
{
        printf("ERROR: Cannot enable cpu clock directly!");
        return -EINVAL;
}

int ns2815_cpu_clk_disable()
{
        __asm__ __volatile__("bl _wfi_asm");
        printf("cpu clock disabled.\n");
        return 0;
}

// NEON0 power control function/tasks
void ns2815_neon0_pwr_up() 
{
    volatile void *nustar_pwr_ctrl = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Set NEON0 Power Control bit
    reg_set_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_NEON0_PWR_CTRL);

    // Wait until Neon0 Power Status is NO ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_NEON0_PWR_STATUS) == 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

void ns2815_neon0_pwr_down() 
{
    volatile void *nustar_pwr_ctrl = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Clear NEON0 Power Control bit
    reg_clr_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_NEON0_PWR_CTRL);

    // Wait until Neon0 Power Status is NO ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_NEON0_PWR_STATUS) != 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

// NEON1 power control function/tasks
void ns2815_neon1_pwr_up() 
{
    volatile void *nustar_pwr_ctrl = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Set NEON1 Power Control bit
    reg_set_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_NEON1_PWR_CTRL);

    // Wait until Neon1 Power Status is NO ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_NEON1_PWR_STATUS) == 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

void ns2815_neon1_pwr_down() 
{
    volatile void *nustar_pwr_ctrl = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Clear NEON1 Power Control bit
    reg_clr_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_NEON1_PWR_CTRL);

    // Wait until Neon1 Power Status is NO ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_NEON1_PWR_STATUS) != 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

// L2C power control function/tasks
void ns2815_l2c_pwr_up() 
{
    volatile void *nustar_pwr_ctrl = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Set L2C Power Control bit
    reg_set_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_L2C_PWR_CTRL);

    // Wait until L2C Power Status is NO ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_L2C_PWR_STATUS) == 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

void ns2815_l2c_pwr_down() 
{
    volatile void *nustar_pwr_ctrl = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    unsigned int sys_pwr_status_reg = 0;

    // Clear L2C Power Control bit
    reg_clr_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_L2C_PWR_CTRL);

    // Wait until L2C Power Status is NO ALIVE
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_L2C_PWR_STATUS) != 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    }
}

// falcon0 power control function/tasks
void ns2815_falcon0_pwr_up()
{
    volatile void *nustar_pwr_ctrl  = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    volatile void *cpu_power_status = (SCU_CPU_POWER_STATUS);

    unsigned int sys_pwr_status_reg = 0;
    unsigned int cpu_power_status_reg;

    // set falcon0 Power Control bit
    reg_set_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_FALCON0_PWR_CTRL);

    // Read SYS_PWR_STATUS to confirm CPU0 already Power Up
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_FALCON0_PWR_STATUS) == 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
        printf("falcon0 is still not alive");
    }
    printf("falcon0 is already power up!");

    // set Power Status Control of CPU0 to 2'b00 in SCU
    cpu_power_status_reg = (0x3) << get_offset_pos(SCU_CPU_POWER_STATUS_CPU0_STATUS_MASK);
    reg_clr_bits(cpu_power_status, cpu_power_status_reg);
}

void ns2815_falcon0_pwr_down()
{
    volatile void *nustar_pwr_ctrl  = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    volatile void *cpu_power_status = (SCU_CPU_POWER_STATUS);

    unsigned int cpu_power_status_reg;
    unsigned int sys_pwr_status_reg;

    // clear falcon1 Power Control bit
    reg_clr_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_FALCON0_PWR_CTRL);

    // set Power Status Control of CPU1 to 2'b11 in SCU
    cpu_power_status_reg = (0x3) << get_offset_pos(SCU_CPU_POWER_STATUS_CPU0_STATUS_MASK);
    reg_set_bits(cpu_power_status, cpu_power_status_reg);

    // execute wfi after falcon1 finish its jobs
    __asm__ __volatile__("mrc p15, 0, r0, c0, c0, 5\n"
			 "and r0, #0xf\n"
                         "cmp r0, #0\n"
                         "bne cpu1_exit\n"
                         "bl _wfi_asm\n"
                         "cpu1_exit:");
                         
    // Read SYS_PWR_STATUS to confirm CPU1 already Power Down
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_FALCON0_PWR_STATUS) != 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
        printf("falcon0 is still alive");
    }
        
    printf("falcon0 is already power down!");

    return;
}

// falcon1 power control function/tasks
void ns2815_falcon1_pwr_up()
{
    volatile void *nustar_pwr_ctrl  = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    volatile void *cpu_power_status = (SCU_CPU_POWER_STATUS);

    unsigned int sys_pwr_status_reg = 0;
    unsigned int cpu_power_status_reg;

    // set falcon1 Power Control bit
    reg_set_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_FALCON1_PWR_CTRL);

    // Read SYS_PWR_STATUS to confirm CPU1 already Power Up
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_FALCON1_PWR_STATUS) == 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
        printf("falcon1 is still not alive");
    }
    printf("falcon1 is already power up!");

    // set Power Status Control of CPU1 to 2'b00 in SCU
    cpu_power_status_reg = (0x3) << get_offset_pos(SCU_CPU_POWER_STATUS_CPU1_STATUS_MASK);
    reg_clr_bits(cpu_power_status, cpu_power_status_reg);
}

void ns2815_falcon1_pwr_down()
{
    volatile void *nustar_pwr_ctrl  = (PRCM_NUSTAR_PWR_CTRL);
    volatile void *sys_pwr_status = (PRCM_SYS_PWR_STATUS);
    volatile void *cpu_power_status = (SCU_CPU_POWER_STATUS);

    unsigned int cpu_power_status_reg;
    unsigned int sys_pwr_status_reg;

    // clear falcon1 Power Control bit
    reg_clr_bits(nustar_pwr_ctrl, PRCM_NUSTAR_PWR_CTRL_FALCON1_PWR_CTRL);

    // set Power Status Control of CPU1 to 2'b11 in SCU
    cpu_power_status_reg = (0x3) << get_offset_pos(SCU_CPU_POWER_STATUS_CPU1_STATUS_MASK);
    reg_set_bits(cpu_power_status, cpu_power_status_reg);

    // execute wfi after falcon1 finish its jobs
    __asm__ __volatile__("mrc p15, 0, r0, c0, c0, 5\n"
			 "and r0, #0xf\n"
                         "cmp r0, #0\n"
                         "beq cpu0_exit\n"
                         "bl _wfi_asm\n"
                         "cpu0_exit:");
                         
    // Read SYS_PWR_STATUS to confirm CPU1 already Power Down
    sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
    while ((sys_pwr_status_reg & PRCM_SYS_PWR_STATUS_FALCON1_PWR_STATUS) != 0) {
        udelay(100);
        sys_pwr_status_reg = *(unsigned int*)(sys_pwr_status);
        printf("falcon1 is still alive");
    }
        
    printf("falcon1 is already power down!");

    return;
}

// CPU Clock Rate Setting 
int ns2815_cpu_clk_set_rate(unsigned long rate, unsigned int nouse)
{
	volatile void *cpu_clk_switch = (PRCM_CPU_CLK_SWITCH);

	unsigned int cpu_clk_switch_reg;

//        printf("current frequence is %ld Hz.\n", get_current_freq()); 
 
        // set to manual mode 
        //reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MU0_SEL_EN|PRCM_CPU_CLK_SWITCH_CPU_MU1_SEL_EN); 
        //reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MODE);

	// set to auto mode
	reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
        reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MODE);

	cpu_clk_switch_reg = *((unsigned int*) cpu_clk_switch);

	// Manual mode
	if (!(cpu_clk_switch_reg & PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MODE)) {
		printf("Change CPU clock at Manual mode\n");
		reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MU0_SEL_EN|PRCM_CPU_CLK_SWITCH_CPU_MU1_SEL_EN);
		if (!(cpu_clk_switch_reg & PRCM_CPU_CLK_SWITCH_CPU_MUX_SWITCH)) { // current SWITCH0
			// switch to PLL1_CLKOUT, use SWITCH1
			reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_MASK);
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_3); // Use PLL1_CLKOUT
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MU1_SEL_EN);
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX_SWITCH);// switch to SWITCH1

		        printf("current frequence is %ld Hz.\n", get_current_freq()); 

			// set clock rate for PLL0
			ns2815_pll0_clk_set_rate(rate, nouse);

			// Switch to PLL1_CLKOUT, use SWITCH0
			reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_MASK);
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_1); // Use PLL0_CLKOUT
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MU0_SEL_EN);
			reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX_SWITCH);// switch back to SWITCH0
		}
		else { // current SWITCH1
			// switch to PLL1_CLKOUT, use SWITCH0
			reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_MASK);
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_3); // Use PLL1_CLKOUT
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MU0_SEL_EN);
			reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX_SWITCH);// switch to SWITCH0

		        printf("current frequence is %ld Hz.\n", get_current_freq()); 

			// set clock rate for PLL0
			ns2815_pll0_clk_set_rate(rate, nouse);

			// Switch to PLL1_CLKOUT, use SWITCH1
			reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_MASK);
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_1); // Use PLL0_CLKOUT
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MU1_SEL_EN);
			reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_MUX_SWITCH);// switch back to SWITCH1
		}

 	}
	else { // Auto mode
		printf("Change CPU clock at Auto mode\n");

		// Switch to PLL1
		reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
		reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MASK);
		reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_3);
		reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);

//	        printf("current frequence is %ld Hz.\n", get_current_freq()); 

		// set clock rate for PLL0
		ns2815_pll0_clk_set_rate(rate, nouse);

		// set autosel target to PLL0
		reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
		reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MASK);
		reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_1);
		reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
	}

//        printf("cpu clock rate should set to %ld Hz, and current frequence is %ld Hz.\n", rate, get_current_freq()); 
        printf("cpu frequence is %ld Hz.\n",get_current_freq()); 

	return 0;

}


int ns2815_cpu_dvfs(unsigned long rate, unsigned long volt)
{
	int pmic_status = 0;

	volatile void *cpu_clk_switch = (PRCM_CPU_CLK_SWITCH);
	volatile void *cpu_clk_div    = (PRCM_CPU_CLK_DIV);

	printf("current1-0 frequence is %ld Hz.\n", get_current_freq()); 

	// set to auto mode
	reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
        reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MODE);
	printf("Change CPU clock at Auto mode\n");

	// set divider1 divider factor
	reg_clr_bits(cpu_clk_div, PRCM_CPU_CLK_DIV_CPU_DIV1_MASK);
	reg_set_bits(cpu_clk_div, PRCM_CPU_CLK_DIV_CPU_DIV1_15);
	reg_set_bits(cpu_clk_div, PRCM_CPU_CLK_DIV_CPU_DIV1_EN);
	printf("current2-1 frequence is %ld Hz.\n", get_current_freq()); 

	// switch to divider1 clkout
	reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
	reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MASK);
	reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_4);
	reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);

	printf("current2 frequence is %ld Hz.\n", get_current_freq()); 

	// PMIC adjust
	pmic_status = pmic_set(volt);

	if (pmic_status != 0) {
        	printf("ERROR: Cannot set pmic voltage to correct voltage value %ld uV\n", volt);
		return -EINVAL;
	}

	udelay(100);

	// set clock rate for PLL0
	ns2815_pll0_clk_set_rate(rate, 0);

	// set autosel target to PLL0
	reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
	reg_clr_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MASK);
	reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_1);
	reg_set_bits(cpu_clk_switch, PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN);
	reg_clr_bits(cpu_clk_div, PRCM_CPU_CLK_DIV_CPU_DIV1_EN);

	printf("current3 frequence is %ld Hz.\n", get_current_freq()); 
	printf("cpu clock rate set to %ld Hz, voltage set to %ld uV\n", rate, volt);
	return 0;

}

int pmic_set_volt(unsigned long volt)
{
	unsigned int volt_set;

	unsigned int i2c_num = 1;
	void * i2c_base = (void*) (0x06100000+0x00010000*i2c_num);

	if (volt < 750000) {
		printf("ERROR: Voltage setting too small: at lease 0.75v\n");
		return -EINVAL;
	}
	else if (volt > 1500000) {
		printf("ERROR: Voltage setting too big: at most 1.5v\n");
		return -EINVAL;
	}

	dw_i2c_master_init(i2c_base, 0x12);

	// set volt
	volt_set = (volt-600000+12500)/12500;
	tps80032_i2c_set_reg(i2c_base, 0x5b, volt_set);

	// calculate the actual voltage
	volt = 600000-12500+volt_set*12500;
	printf("pmic voltage set to %ld uV\n", volt);

	return 0;
}

int pmic_set(unsigned long volt)
{
	unsigned int volt_set;

	unsigned int i2c_num = 1;
	void * i2c_base = (void*) (0x06100000+0x00010000*i2c_num);

	if (volt < 750000) {
		printf("ERROR: Voltage setting too small: at lease 0.75v\n");
		return -EINVAL;
	}
	else if (volt > 1500000) {
		printf("ERROR: Voltage setting too big: at most 1.5v\n");
		return -EINVAL;
	}

	i2c_dw_init(i2c_base);

	// close LDOSW
	pmic_i2c_set_reg(i2c_base, 0x5e, 0x0);

	// set volt
	volt_set = (volt-750000)/12500;
	pmic_i2c_set_reg(i2c_base, 0x35, volt_set);

	// calculate the actual voltage
	volt = 750000+volt_set*12500;
	printf("pmic voltage set to %ld uV\n", volt);

	return 0;
}

void pmic_i2c_set_reg(void * i2c_base, unsigned int addr, unsigned int value)
{
	unsigned char out_buf[2];
	unsigned char in_buf[1];

	out_buf[0] = (char) addr;
	out_buf[1] = (char) value;
	
	dw_i2c_master_init(i2c_base, 0x34);
	dw_i2c_send_bytes(i2c_base, out_buf, 0x2);
	dw_i2c_smbus_read(i2c_base, out_buf, 0x1, in_buf, 0x1);
	printf ("Set addr %x to value %x, readback is %x\n", addr, value, in_buf[0]);
}

void tps80032_i2c_set_reg(void * i2c_base, unsigned int addr, unsigned int value)
{
	unsigned char out_buf[2];
	unsigned char in_buf[1];

	out_buf[0] = (char) addr;

	dw_i2c_smbus_read(i2c_base,out_buf,1,in_buf,1);	//read in_buf from out_buf;

	out_buf[1] = (char)((in_buf[0] & 0xc0) | ((char)value));

	dw_i2c_send_bytes(i2c_base, out_buf, 0x2);	//write out_buf[0] and out_buf[1];

	dw_i2c_smbus_read(i2c_base, out_buf, 0x1, in_buf, 0x1);
	printf ("Set addr %x to value %x, readback is %x\n", addr, value, in_buf[0]);
}

// Data flow tests

int data_flow_test(unsigned int * addr, unsigned int data, unsigned int mask)
{
    volatile unsigned int tdata;
    *((volatile unsigned int*) addr) = data;
    tdata = *((volatile unsigned int*) addr);
    if (tdata == (data&mask)) {
        printf("address %x, get data %x same as expected\n", (unsigned int)addr, tdata);
        return 0;
    }
    else {
        printf("ERROR: address %x, get data %x, expected data: %x\n", (unsigned int)addr, tdata, data&mask);
	return -EINVAL;
    }
}

void ns2815_dataflow_test()
{
    int tresult = 0;

    // SDMMC
    ns2815_sd_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05070004, 0x5a5a5a5a, 0x00ffffff);

    // HDMI
    ns2815_hdmi_pxl_clk_enable();
    tresult += data_flow_test((unsigned int*)0x051100C8, 0x5a5a5a5a, 0x000000ff);

    // Wakeup RAM memctrl
    ns2815_wkup_mem_clk_enable();
    tresult += data_flow_test((unsigned int*)0x058200A0, 0x5a5a5a5a, 0x00000fff);

    // ROM Memctrl
    ns2815_core_mem_clk_enable();
    tresult += data_flow_test((unsigned int*)0x030000A0, 0x5a5a5a5a, 0x00000fff);

    // Wakeup GPIO
    ns2815_gpio_wkup_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05823000, 0x5a5a5a5a, 0x000000ff);

    // Wakeup Timer
    ns2815_timer0_wkup_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05824000, 0x5a5a5a5a, 0xffffffff);

    // Core GPIO
    ns2815_gpio_core_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06140000, 0x5a5a5a5a, 0xffffffff);

    // Core Timer
    ns2815_timer0_core_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06150000, 0x5a5a5a5a, 0xffffffff);

    // UART0
    //ns2815_uart0_clk_enable();
    //tresult += data_flow_test((unsigned int*)0x0609000C, 0x5a5a5a5a, 0x000000ff);

    // UART1
    ns2815_uart1_clk_enable();
    tresult += data_flow_test((unsigned int*)0x060A000C, 0x5a5a5a5a, 0x000000ff);

    // UART2
    ns2815_uart2_clk_enable();
    tresult += data_flow_test((unsigned int*)0x060B000C, 0x5a5a5a5a, 0x000000ff);

    // UART3
    ns2815_uart3_clk_enable();
    tresult += data_flow_test((unsigned int*)0x060C000C, 0x5a5a5a5a, 0x000000ff);

    // SSI_0
    ns2815_spi0_clk_enable();
    tresult += data_flow_test((unsigned int*)0x060D0000, 0x5a5a5a5a, 0x0000ffff);

    // SSI_1
    ns2815_spi1_clk_enable();
    tresult += data_flow_test((unsigned int*)0x060E0000, 0x5a5a5a5a, 0x0000ffff);

    // SSI_2
    ns2815_spi2_clk_enable();
    tresult += data_flow_test((unsigned int*)0x060F0000, 0x5a5a5a5a, 0x0000ffff);

    // SSI_3
    ns2815_spi3_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06170000, 0x5a5a5a5a, 0x0000ffff);

    // I2C_0
    ns2815_i2c0_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06100004, 0x5a5a5a5a, 0x00001fff);

    // I2C_1
    ns2815_i2c1_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06110004, 0x5a5a5a5a, 0x00001fff);

    // I2C_2
    ns2815_i2c2_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06120004, 0x5a5a5a5a, 0x00001fff);

    // I2C_3
    ns2815_i2c3_clk_enable();
    tresult += data_flow_test((unsigned int*)0x06130004, 0x5a5a5a5a, 0x00001fff);

    // MemMax
    tresult += data_flow_test((unsigned int*)0x08008024, 0x5a5a5a5a, 0xffffffff);

    // USB Host
    ns2815_usb_ohci_clk_enable();
    ns2815_usb_utmi_clk_enable();
    tresult += data_flow_test((unsigned int*)0x0509001C, 0x5a5a5a5a, 0x00003fff);

    // USB Otg
    ns2815_usb_otg_clk_enable();
    tresult += data_flow_test((unsigned int*)0x050A0914, 0x5a5a5a5a, 0xffffffff);

    // Decoder(G1)
    ns2815_vpu_g1_pwr_up();
    ns2815_vpu_g1_clk_enable();
    tresult += data_flow_test((unsigned int*)0x050C0008, 0x5a5a5a5a, 0xffffffff);
    tresult += data_flow_test((unsigned int*)0x050C00A0, 0x5a5a5a5a, 0xffffffff); // [31:0] all rw

    // Encoder(H1)
    ns2815_vpu_h1_pwr_up();
    ns2815_vpu_h1_clk_enable();
    tresult += data_flow_test((unsigned int*)0x050D0008, 0x5a5a5a5a, 0xffffffff);

    // ZSP
    ns2815_zsp_pwr_up();
    ns2815_zsp_clk_enable();
    tresult += data_flow_test((unsigned int*)0x07000000, 0x5a5a5a5a, 0xffffffff);
    tresult += data_flow_test((unsigned int*)0x0701fffc, 0x5a5a5a5a, 0xffffffff);
    tresult += data_flow_test((unsigned int*)0x07040000, 0x5a5a5a5a, 0xffffffff);
    tresult += data_flow_test((unsigned int*)0x0704fffc, 0x5a5a5a5a, 0xffffffff);

    // DDR
    //ns2815_ddr_clk_enable();
    //tresult += data_flow_test((unsigned int*)0x050E0088, 0x5a5a5a5a, 0xffffffff);
    //tresult += data_flow_test((unsigned int*)0x050E0200, 0x5a5a5a5a, 0xffffffff);

    // GPU_GP/GPU_PP/GPU_L2
    ns2815_mali_pwr_up();
    ns2815_mali_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05030004, 0x5a5a5a58, 0xfffffff8); // [2:0] Write 0, read undefined
    tresult += data_flow_test((unsigned int*)0x05038000, 0x5a5a5a58, 0xfffffff8); // [2:0] Write 0, read undefined
    tresult += data_flow_test((unsigned int*)0x05031018, 0x0000001f, 0x0000001f); // [31:5] Reserved, read as zero
    tresult += data_flow_test((unsigned int*)0x05031018, 0x0000001a, 0x0000001f);

    // ISP
    ns2815_isp_pwr_up();
    ns2815_isp_clk_enable();
    //tresult += data_flow_test((unsigned int*)0x05180000, 0x5a5a5a5a, 0x00000006);
    tresult += data_flow_test((unsigned int*)0x05180000, 0x5a5a5a5e, 0x00000006); // bit1 will be same as bit2: 1
    tresult += data_flow_test((unsigned int*)0x05180000, 0x5a5a5a59, 0x00000006); // bit1 will be same as bit2: 0

    // Display0
    ns2815_lcdc0_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05010030, 0x5a5a5a5a, 0xffffffff);

    // Display1
    ns2815_lcdc1_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05011030, 0x5a5a5a5a, 0xffffffff);

    // GC300
    ns2815_gc300_pwr_up();
    ns2815_gc300_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05140014, 0x5a5a5a5a, 0xffffffff);

    // DMA330
    ns2815_pl330dma_clk_enable();
    tresult += data_flow_test((unsigned int*)0x05021020, 0x5a5a5a5a, 0x0fffffff);// [27:0] RW

    // PRCM
    tresult += data_flow_test((unsigned int*)0x05821020, 0x5a5a5a5a, 0xffffffff);

    // SCM
    tresult += data_flow_test((unsigned int*)0x0582205C, 0x5a5a5a5a, 0x00001fff);

    printf ("##############################################\n");

    if (tresult == 0) {
        printf("PASS: All data flow tests are correct!\n");
    }
    else {
        printf("FAIL: There are %d errors in data flow tests!\n", -tresult);
    }
    printf ("##############################################\n");

}

unsigned long get_current_freq(void)
{
                unsigned int ret = 0;
                unsigned int ret1 = 0;
                unsigned int sub = 0;
                unsigned int val = 0;
                unsigned int load_max = 0xffffffff;
                writel(load_max, TIMER_ADDR + 0x3c);    //0x3c is timer 4
                writel(load_max, LOCAL_TIMER);          //private timer counter reg
                ret = readl(TIMER_ADDR + 0x3c + 0x04);
                ret1 = readl(LOCAL_TIMER + 0x04);
                val = readl(TIMER_ADDR + 0x3c + 0x08);  //enable
                val |= 1;
                writel(val, TIMER_ADDR + 0x3c + 0x08);
                val = readl(LOCAL_TIMER + 0x08);        //enable local_timer
                val |= 1;
                writel(val, LOCAL_TIMER + 0x08);
                sub = load_max - TIMER_CLK_MHZ*1000*100;//100ms
                while(readl(TIMER_ADDR + 0x3c + 0x04) > sub);
                /*read external timer*/
                ret = readl(TIMER_ADDR + 0x3c + 0x04);
                /*read local timer*/
                ret1 = readl(LOCAL_TIMER + 0x04);
                ret = load_max - ret;
                ret1 = load_max - ret1;
                //printf("freq:%uMHz\n", ret1*20/1000000);//time
 
                val = readl(TIMER_ADDR + 0x3c + 0x08);  //disable
                val &= (~1);
                writel(val, TIMER_ADDR + 0x3c + 0x08);
                val = readl(LOCAL_TIMER + 0x08);        //disable
                val &= (~1);
                writel(val, LOCAL_TIMER + 0x08);

                return  ret1*20; 
}



#endif
