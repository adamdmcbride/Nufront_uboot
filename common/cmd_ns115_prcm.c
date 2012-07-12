/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>

#if defined(CONFIG_CMD_NS115)

#include <environment.h>
extern env_t *env_ptr;

#define DEV_CLK_CONFIG1(dev)							   \
	if(strcmp(argv[1], #dev) == 0) {             				   \
		if(strcmp(argv[2], "clk") == 0) {				   \
			if((strcmp(argv[3], "enable") == 0) && (argc == 4)) {      \
				ns2815_##dev##_clk_enable();			   \
				return 0;					   \
			}							   \
			if((strcmp(argv[3], "disable") == 0) && (argc == 4)) {     \
				ns2815_##dev##_clk_disable();			   \
				return 0;					   \
			}							   \
		}								   \
	}								

#define DEV_CLK_CONFIG2(dev, clksrc)						   \
	if(strcmp(argv[1], #dev) == 0) {             				   \
		if(strcmp(argv[2], "clk") == 0) {				   \
			if((strcmp(argv[3], "enable") == 0) && (argc == 4)) {      \
				ns2815_##dev##_clk_enable();			   \
				return 0;					   \
			}							   \
			if((strcmp(argv[3], "disable") == 0) && (argc == 4)) {     \
				ns2815_##dev##_clk_disable();			   \
				return 0;					   \
			}							   \
			if((strcmp(argv[3], "set") == 0) && (argc == 6)) {         \
				rate = simple_strtoul(argv[4], NULL, 10);	   \
				if(strcmp(argv[5], "hz") == 0) {		   \
					unit = 1;				   \
				}						   \
				else if(strcmp(argv[5], "khz") == 0) {		   \
					unit = 1000;				   \
				}						   \
				else if(strcmp(argv[5], "mhz") == 0) {		   \
					unit = 1000000;				   \
				}						   \
				else if(strcmp(argv[5], "ghz") == 0) {		   \
					unit = 1000000000;			   \
				}						   \
				else {						   \
				printf("ERROR: Not support such frequency unit\n");\
				return -1;					   \
				}						   \
										   \
				rate = rate*unit;				   \
				ns2815_##clksrc##_clk_set_rate(rate, nouse);       \
				return 0;					   \
			}						           \
		}								   \
	}								

#define DEV_PWR_CONFIG(dev)							   \
	if(strcmp(argv[1], #dev) == 0) {             				   \
		if(strcmp(argv[2], "pwr") == 0) {				   \
			if((strcmp(argv[3], "up") == 0) && (argc == 4)) {          \
				ns2815_##dev##_pwr_up();			   \
				return 0;					   \
			}							   \
			if((strcmp(argv[3], "down") == 0) && (argc == 4)) {        \
				ns2815_##dev##_pwr_down();	  		   \
				return 0;					   \
			}							   \
		}								   \
	}								


#define  DEV_DATAFLOW_TEST()   {                                                   \
	if(strcmp(argv[1], "dtflow") == 0) {                                       \
		ns2815_dataflow_test();                                            \
		return 0;                                                          \
	}                                                                          \
}

#define CPU_DVFS_CONFIG()							   \
	if(strcmp(argv[1], "cpu") == 0) {             				   \
		if(strcmp(argv[2], "dvfs") == 0) {				   \
			rate = simple_strtoul(argv[3], NULL, 10);	           \
			rate = rate*1000000;                                       \
			volt = simple_strtoul(argv[4], NULL, 10);	    	   \
			ns2815_cpu_dvfs(rate, volt);				   \
			return 0;                                                  \
		}								   \
	}

#define CPU_VOLT_CONFIG()							   \
	if(strcmp(argv[1], "cpu") == 0) {             				   \
		if(strcmp(argv[2], "volt") == 0) {				   \
			volt = simple_strtoul(argv[3], NULL, 10);	    	   \
			pmic_set(volt);						   \
			return 0;                                                  \
		}								   \
	}


#define  NS115_POWEROFF()   {                                                   \
	if(strcmp(argv[1], "poweroff") == 0) {                                       \
		ns2815_pmic_poweroff();                                            \
		return 0;                                                          \
	}                                                                          \
}

#define  NS115_RESET()   {                                                   \
	if(strcmp(argv[1], "reset") == 0) {                                       \
		ns2815_pmic_reset();                                            \
		return 0;                                                          \
	}                                                                          \
}

#define NS115_I2C1_BASE 0x06110000
void ns2815_pmic_poweroff()
{
	unsigned char rbuf[4];
	unsigned char wbuf[4];
	unsigned int ret = 0;

	rbuf[0] = 0x00;
 	wbuf[0] = 0x13;

	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
	if(ret){
		printf("ERROR: read PMIC register failed\r\n");
		return ;
	}
	wbuf[0] = 0x13;
	wbuf[1] = rbuf[0] | 0x01;
	ret = dw_i2c_send_bytes(NS115_I2C1_BASE,wbuf,2);
	
	if(ret){
    		printf("ERROR: read ecompass-sensor register failed\r\n");
    		return -1;
	}
	return 0;
}

void ns2815_pmic_reset()
{
	unsigned char r_buf[4],rbuf[4];
	unsigned char w_buf[4],wbuf[4];
	unsigned int ret = 0;

	r_buf[0] = rbuf[0] = 0x00;
	w_buf[0] = 0x13;
 	wbuf[0] = 0x14;



	dw_i2c_master_init(NS115_I2C1_BASE,0x34);
	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,wbuf,1,rbuf,1);
	if(ret){
		printf("ERROR: read PMIC register failed\r\n");
		return ;
	}
	ret = dw_i2c_smbus_read(NS115_I2C1_BASE,w_buf,1,r_buf,1);
	if(ret){
		printf("ERROR: read PMIC register failed\r\n");
		return ;
	}
	wbuf[0] = 0x14;
	wbuf[1] = rbuf[0] | 0x01;
	ret = dw_i2c_send_bytes(NS115_I2C1_BASE,wbuf,2);
	if(ret){
    		printf("ERROR: read ecompass-sensor register failed\r\n");
    		return -1;
	}

	w_buf[0] = 0x13;
	w_buf[1] = r_buf[0] | 0x01;
	ret = dw_i2c_send_bytes(NS115_I2C1_BASE,w_buf,2);
	
	if(ret){
    		printf("ERROR: read ecompass-sensor register failed\r\n");
    		return -1;
	}
	return 0;
}

int do_ns115(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned long rate;
	unsigned long volt;
	unsigned long unit;
	unsigned int  nouse = 0;

	/*PLL0*/
	DEV_CLK_CONFIG1(pll0)
	DEV_PWR_CONFIG(pll0)

	/*PLL1*/
	DEV_CLK_CONFIG1(pll1)

	/*PLL2*/
	DEV_CLK_CONFIG1(pll2)

	/*PLL3*/
	DEV_CLK_CONFIG1(pll3)
	DEV_PWR_CONFIG(pll3)

	/*PLL4*/
	DEV_CLK_CONFIG1(pll4)
	DEV_PWR_CONFIG(pll4)

	/*PLL5*/
	DEV_CLK_CONFIG1(pll5)
	DEV_PWR_CONFIG(pll5)

	/*PLL6*/
	DEV_CLK_CONFIG1(pll6)
	DEV_PWR_CONFIG(pll6)

	/*CPU*/
	DEV_CLK_CONFIG2(cpu, cpu)

	/*NEON0*/
	DEV_PWR_CONFIG(neon0)

	/*NEON1*/
	DEV_PWR_CONFIG(neon1)

	/*L2C*/
	DEV_PWR_CONFIG(l2c)

        /* Falcon0*/
	DEV_PWR_CONFIG(falcon0)

        /* Falcon1*/
	DEV_PWR_CONFIG(falcon1)

	/*CORESIGHT*/
	DEV_CLK_CONFIG1(coresight)

	/*DDR*/
	DEV_CLK_CONFIG2(ddr, ddr)

	/*PL330DMA*/
	DEV_CLK_CONFIG1(pl330dma)

	/*MALI*/
	DEV_CLK_CONFIG2(mali, mali)
	DEV_PWR_CONFIG(mali)

	/*GC300*/
	DEV_CLK_CONFIG2(gc300, gc300)
	DEV_PWR_CONFIG(gc300)

	/*VPU_G1*/
	DEV_CLK_CONFIG2(vpu_g1, vpu_g1)
	DEV_PWR_CONFIG(vpu_g1)

	/*VPU_H1*/
	DEV_CLK_CONFIG2(vpu_h1, vpu_h1)
	DEV_PWR_CONFIG(vpu_h1)

	/*ISP_JPEG*/
	DEV_CLK_CONFIG2(isp_jpeg, isp_jpeg)
	DEV_PWR_CONFIG(isp)

	/*isp*/
	DEV_CLK_CONFIG1(isp)

	/*SD*/
	DEV_CLK_CONFIG2(sd, sd)

	/*ZSP*/
	DEV_CLK_CONFIG1(zsp)
	DEV_PWR_CONFIG(zsp)

	/*spi0*/
	DEV_CLK_CONFIG2(spi0, spi01)

	/*spi1*/
	DEV_CLK_CONFIG2(spi1, spi01)

	/*spi2*/
	DEV_CLK_CONFIG2(spi2, spi23)

	/*spi3*/
	DEV_CLK_CONFIG2(spi3, spi23)

	/*I2C0*/
	DEV_CLK_CONFIG2(i2c0, i2c_fs)

	/*I2C1*/
	DEV_CLK_CONFIG2(i2c1, i2c_fs)

	/*I2C2*/
	DEV_CLK_CONFIG2(i2c2, i2c_fs)

	/*I2C3*/
	DEV_CLK_CONFIG2(i2c3, i2c_hs)

	/*UART0*/
	DEV_CLK_CONFIG2(uart0, uart01)

	/*UART1*/
	DEV_CLK_CONFIG2(uart1, uart01)

	/*UART2*/
	DEV_CLK_CONFIG2(uart2, uart2)

	/*UART3*/
	DEV_CLK_CONFIG2(uart3, uart3)

	/*RGBO*/
	DEV_CLK_CONFIG2(rgbo, pxl0)

	/*LCDC0*/
	DEV_CLK_CONFIG2(lcdc0, pxl0)

	/*LCDC1*/
	DEV_CLK_CONFIG2(lcdc1, pxl1)

	/*HDMI_PXL*/
	DEV_CLK_CONFIG2(hdmi_pxl, pxl1)

	/*i2s0*/
	DEV_CLK_CONFIG2(i2s0, i2s)

	/*i2s1*/
	DEV_CLK_CONFIG2(i2s1, i2s)

	/*USB_OTG*/
	DEV_CLK_CONFIG1(usb_otg)

	/*USB_UTMI*/
	DEV_CLK_CONFIG1(usb_utmi)

	/*USB_OHCI*/
	DEV_CLK_CONFIG1(usb_ohci)

	/*HSIC*/
	DEV_CLK_CONFIG1(usb_hsic)

	/*CORE_MEM*/
	DEV_CLK_CONFIG1(core_mem)

	/*GPIO_CORE*/
	DEV_CLK_CONFIG1(gpio_core)

	/*TIMER0*/
	DEV_CLK_CONFIG2(timer0_core, timer0_core)

	/*TIMER1*/
	DEV_CLK_CONFIG2(timer1_core, timer1_core)

	/*TIMER2*/
	DEV_CLK_CONFIG2(timer2_core, timer2_core)

	/*TIMER3*/
	DEV_CLK_CONFIG2(timer3_core, timer3_core)

	/*TIMER4*/
	DEV_CLK_CONFIG2(timer4_core, timer4_core)

	/*TIMER5*/
	DEV_CLK_CONFIG2(timer5_core, timer5_core)

	/*TIMER6*/
	DEV_CLK_CONFIG2(timer6_core, timer6_core)

	/*TIMER7*/
	DEV_CLK_CONFIG2(timer7_core, timer7_core)

	/*WKUP_MEM*/
	DEV_CLK_CONFIG1(wkup_mem)

	/*GPIO_WKUP*/
	DEV_CLK_CONFIG2(gpio_wkup, gpio_wkup)

	/*TIMER0_WKUP*/
	DEV_CLK_CONFIG2(timer0_wkup, timer0_wkup)

	/*TIMER1_WKUP*/
	DEV_CLK_CONFIG2(timer1_wkup, timer1_wkup)

	/*ALT0*/
	DEV_CLK_CONFIG2(alt0, alt0)

	/*ALT1*/
	DEV_CLK_CONFIG2(alt1, alt1)

	/*AUX*/
	DEV_CLK_CONFIG1(aux)

	/*EFUSE*/
	DEV_CLK_CONFIG1(efuse)

	/*PWM*/
	DEV_CLK_CONFIG1(pwm)

	/*CPU DVFS*/
	CPU_DVFS_CONFIG()
	CPU_VOLT_CONFIG()

	/*Dataflow Test*/
	DEV_DATAFLOW_TEST()

	/*PMIC_RESET*/
	NS115_RESET()

	/*PMIC_poweroff*/
	NS115_POWEROFF();

	cmd_usage(cmdtp);  	


	return 0;
}



U_BOOT_CMD(
	ns115, 6, 0, do_ns115,
	"ns115 sub system",
	"ns115 <dev> clk enable:  enable  clock for device <dev>\n"
	"ns115 <dev> clk disable: disable clock for device <dev>\n"
	"ns115 <dev> pwr up:   power up device <dev>\n"
	"ns115 <dev> pwr down: power down device <dev>\n"
	"ns115 <dev> clk set <rate> <unit>: set clock rate for device <dev>\n"
        "ns115 cpu dvfs <rate> <volt>: ns115 cpu dvfs test\n"
        "ns115 dtflow: ns115 dataflow tests\n");
#endif
