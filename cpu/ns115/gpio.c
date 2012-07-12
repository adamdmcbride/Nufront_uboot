/*
 *
 	porta_nr = 0;
	portb_nr = 1;
	portc_nr = 2;
	portd_nr = 3;

	pin_nr = pin_nr + 32 *(portN_nr) - 1;(0< pin_nr < 111) 

 */
#include <common.h>
#define GPIO_BASE_ADDR 0x06140000
unsigned int gpio_pinmux_config(unsigned pin_nr)
{
	unsigned int addr;
	unsigned int port_nr,pin_32bit,reg_val;
	port_nr = pin_nr / 32;
	pin_32bit = pin_nr % 32;
	if(port_nr == 0){
		if((pin_32bit >= 0) && (pin_32bit <= 7)){
			reg_val = readl(0x05822020);
			reg_val &= ~(3 << (26 - (pin_32bit * 2)));
			writel(reg_val,0x05822020);
		}else if((pin_32bit >= 8) && (pin_32bit <= 13)){
			reg_val = readl(0x05822020);
			reg_val |= (1 << (26 - (pin_32bit * 2)));
			writel(reg_val,0x05822020);
		}else if((pin_32bit >= 14) && (pin_32bit <= 15)){
			reg_val = readl(0x0582201c);
			reg_val |= (1 << (30 -((pin_32bit - 14) *2)));
			writel(reg_val,0x0582201c);
		}else if((pin_32bit >= 16) && (pin_32bit <= 26)){
			reg_val = readl(0x0582201c);
			reg_val |= ~(3 << (30 -((pin_32bit - 14) *2)));
		}else if((pin_32bit >= 27) && (pin_32bit <= 29)){
			reg_val = readl(0x0582201c);
			reg_val |= (1 << ((29 - pin_32bit) *2));
			writel(reg_val,0x0582201c);
		}else if((pin_32bit >= 30) && (pin_32bit <= 31)){
			reg_val = readl(0x05822018);
			reg_val |= (1 << (30 - (pin_32bit - 30) * 2));
			writel(reg_val,0x05822018);
		}
	}else if(port_nr == 1){
		if((pin_32bit >= 0) && (pin_32bit <= 13)){
			reg_val = readl(0x05822018);
			reg_val |= (1 << (26 - (pin_32bit * 2)));
			writel(reg_val,0x05822018);
		}else if(pin_32bit == 14){
			reg_val = readl(0x05822014);
			reg_val |= (1 << 30);
			writel(reg_val,0x05822014);
		}else if((pin_32bit >= 15) && (pin_32bit <= 22)){
			reg_val = readl(0x05822014);
			reg_val |= (0x2 << ((29 - pin_32bit) *2));
			writel(reg_val,0x05822014);
		}else if((pin_32bit >= 23) && (pin_32bit <= 29)){
			reg_val = readl(0x05822014);
			reg_val |= (0x1 << ((29 - pin_32bit) *2));
			writel(reg_val,0x05822014);
		}else if((pin_32bit >= 30) && (pin_32bit <= 31)){
			reg_val = readl(0x05822010);
			reg_val |= (1 << (30 - (pin_32bit - 30) * 2));
			writel(reg_val,0x05822010);
		}
	}else if(port_nr == 2){
		if((pin_32bit >= 0) && (pin_32bit <= 13)){
			reg_val = readl(0x05822010);
			reg_val |= (1 << (26 - (pin_32bit * 2)));
			writel(reg_val,0x05822010);
		}else if((pin_32bit >= 14) && (pin_32bit <= 29)){
			reg_val = readl(0x0582200c);
			reg_val |= (1 << ((29 - pin_32bit) * 2));
			writel(reg_val,0x0582200c);
		}else if((pin_32bit == 30)){
			reg_val = readl(0x05822008);
			reg_val |= (1 << 24);
			writel(reg_val,0x05822008);
		}else if((pin_32bit == 31)){
			reg_val = readl(0x05822004);
			reg_val |= (0x2 << 4);
			writel(reg_val,0x05822004);
		}
	}else if(port_nr == 3){
		if((pin_32bit >= 0) && (pin_32bit <= 1)){
			reg_val = readl(0x05822004);
			//reg_val |= (1 << (26 - (pin_nr * 2)));
			reg_val |= (2 << (2 - (pin_32bit * 2)));
			writel(reg_val,0x05822004);
		}else if((pin_32bit >= 2) && (pin_32bit <= 13)){
			reg_val = readl(0x05822000);
			//reg_val |= (0x2 << ((30 - pin_nr) * 2));
			reg_val |= (0x2 << (34 - (pin_32bit * 2)));
			writel(reg_val,0x05822000);
		}
	}
		
}

unsigned int pin_to_addr(unsigned pin_nr)
{
	pin_nr /= 32;
	return (unsigned int )(GPIO_BASE_ADDR + (pin_nr * 0xc));
}

unsigned int pin_to_mask(unsigned pin_nr)
{
	return (unsigned int) (1 << (pin_nr % 32));
}

int gpio_direction_input(unsigned pin_nr)
{
	unsigned int addr;
	unsigned int pin_mask,reg_val;

 	addr = (pin_to_addr(pin_nr) + 0x4);
	pin_mask = pin_to_mask(pin_nr);

	reg_val = readl(addr);
	reg_val &= (~pin_to_mask(pin_nr));
	writel(reg_val,addr);
	return 0;
}

int gpio_direction_output(unsigned pin_nr)
{
	unsigned int drr_addr;
	unsigned int pin_mask,reg_val;

	drr_addr = (pin_to_addr(pin_nr) + 0x4);
	pin_mask = pin_to_mask(pin_nr);

	reg_val = readl(drr_addr);
	reg_val |= pin_to_mask(pin_nr);
	writel(reg_val,drr_addr);
	return 0;
}

int nufront_set_gpio_value(unsigned pin_nr,int value)
{
	unsigned int write_addr;
	unsigned int pin_mask,reg_val;
	unsigned int output_mask;

	write_addr = (pin_to_addr(pin_nr));
	gpio_direction_output(pin_nr);

	reg_val = readl(write_addr);
	if(value)
		reg_val |= pin_to_mask(pin_nr);
	else 
		reg_val &=(~pin_to_mask(pin_nr));
	writel(reg_val,write_addr);
	return 0;
}

int nufront_get_gpio_value(unsigned pin_nr)
{
	unsigned int tmp;
	unsigned int read_addr;
	unsigned int reg_val;
	
	tmp = pin_to_addr(pin_nr) - GPIO_BASE_ADDR;

	if(tmp == 0xc){
		read_addr = 0x06140054;
	}else if(tmp == 0x18){
		read_addr = 0x06140058;
	}else if(tmp == 0x24){
		read_addr = 0x0614005c;
	}else{
		read_addr = 0x06140050;
	}

	gpio_direction_input(pin_nr);
	//reg_val = readl(read_addr);
	tmp = readl(read_addr);
	reg_val = (tmp >> (pin_nr % 32)) & 0x01;

	return reg_val;
}










	
