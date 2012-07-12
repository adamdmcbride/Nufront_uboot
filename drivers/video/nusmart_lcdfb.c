/*
 * Driver for Nusmart Env LCD Controller
 *
 * Copyright (C) 2007 Nufront Corporation
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
#include <asm/io.h>
#include <lcd.h>
#include <nusmart_lcdc.h>
#include <asm/arch/gpio.h>

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;
void *lcd_base;				/* Start of framebuffer memory	*/
void *lcd_base_pri;			/* Start of primary framebuffer memory*/
void *lcd_base_sec;			/* Start of secondary framebuffer memory*/
void *lcd_console_address;		/* Start of console buffer	*/
void *lcd_console_address_sec;
void *lcd_console_address_pri;

short console_col;
short console_row;

/* configurable parameters */

#define lcdc_readl(mmio, reg)		__raw_readl((mmio)+(reg))
#define lcdc_writel(mmio, reg, val)	__raw_writel((val), (mmio)+(reg))
#define NUSMART_LCDC_POLARITY		0x07000000
#define NUSMART_LCDC_RGB565	(0x1 << 6)
#define NUSMART_LCDC_RGB888	(0x2 << 6)


#if defined(CONFIG_LCD_1024_600_60)
vidinfo_t panel_info = {    //NULL, 60, 1024, 600, 19841, 240, 48, 12, 3, 32, 10        0, FB_VMODE_NONINTERLACED
	vl_col:		1024,
	vl_row:		600,
	vl_bpix:	5,
	vl_hsync_len:	32,
	vl_left_margin:	240,
	vl_right_margin:48,
	vl_vsync_len:	10,
	vl_upper_margin:8,
	vl_lower_margin:3,
	mmio:		NUSMART_LCDC_BASE,
};
#endif

#if defined(CONFIG_LCD_1024_768_60)
vidinfo_t panel_info = {
	vl_col:		1024,
	vl_row:		768,
	vl_bpix:	4,
	vl_hsync_len:	136,
	vl_left_margin:	24,
	vl_right_margin:160,
	vl_vsync_len:	6,
	vl_upper_margin:3,
	vl_lower_margin:29,
	mmio:		NUSMART_LCDC_BASE,
};
#endif
#if defined(CONFIG_LCD_640_480_60)
vidinfo_t panel_info = {
	vl_col:		640,
	vl_row:		480,
	vl_bpix:	4,
	vl_hsync_len:	96,
	vl_left_margin:	16,
	vl_right_margin:48,
	vl_vsync_len:	2,
	vl_upper_margin:10,
	vl_lower_margin:33,
	mmio:		NUSMART_LCDC_BASE,
};
#endif
/*
   void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
   {
   }
 */
void bpix_init(void)
{
	if(disp_flag == 0)
		panel_info.vl_bpix = 4;		//16bit
	else if(disp_flag == 1)
		panel_info.vl_bpix = 5;		//32bit
}

void lcd_ctrl_init(void *lcdbase)
{
	unsigned long value;

	debug("entering lcd_ctrl_init");

#if defined(CONFIG_LCD_1024_600_60)
	ns2815_pxl0_clk_set_rate(50400000,1);	
//	ns2815_pxl0_clk_set_rate(43750000,0);	
#endif

	bpix_init();
	lcd_line_length = (panel_info.vl_col * NBITS (panel_info.vl_bpix)) / 8;
	/* Turn off the LCD controller */
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_CTRL,0);

	/* Vertical timing */
	value = INTV(panel_info.vl_upper_margin,\
			panel_info.vl_lower_margin,\
			panel_info.vl_vsync_len );
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_VINTV, value);

	/* Horizontal timing */
	value = INTV(panel_info.vl_left_margin,\
			panel_info.vl_right_margin,\
			panel_info.vl_hsync_len);
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_HINTV, value);

	/* Display size */
	value = HV_SIZE(panel_info.vl_col,\
			panel_info.vl_row);
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_HVSIZE, value);

	lcd_base_pri = lcdbase;
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_PRIPTR, (u_long)lcdbase);

	lcd_base_sec = (lcdbase + (panel_info.vl_col * panel_info.vl_row * NBITS(panel_info.vl_bpix))/8);
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_SECPTR, (u_long)lcd_base_sec);

	value = lcdc_readl(NUSMART_LCDC_BASE,NUSMART_LCDC_AXI);
	if(panel_info.vl_bpix <= 4){					//set burst length
		value |= NUSMART_BURST_LENGTH;
	}else{
		value |= NUSMART_BURST_LENGTH;
	}
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_AXI, value);

	value = lcdc_readl(NUSMART_LCDC_BASE,NUSMART_LCDC_TRAN_NUM);	//set transfer num
	if(panel_info.vl_bpix <= 4){
		value = 0x257f;
	}else{
		value = 0x4aff;
	}
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_TRAN_NUM, value);

	debug("exiting lcd_ctrl_init");
}

ulong calc_fbsize(void)
{
	return ((panel_info.vl_col * panel_info.vl_row *
				NBITS(panel_info.vl_bpix)) / 8) + PAGE_SIZE;
}
void lcd_enable(void)
{
	unsigned int value;

	value = lcdc_readl(NUSMART_LCDC_BASE,NUSMART_LCDC_CTRL);
	if(!(value & NUSMART_LCDC_ENABLE)){
		if(panel_info.vl_bpix <= 4){
			value |= (NUSMART_LCDC_ENABLE | NUSMART_LCDC_POLARITY | NUSMART_LCDC_RGB565);
		}else{
			value |= (NUSMART_LCDC_ENABLE | NUSMART_LCDC_POLARITY | NUSMART_LCDC_RGB888);
		}
	}
	lcdc_writel(panel_info.mmio, NUSMART_LCDC_CTRL, value);

	gpio_pinmux_config(4);	//lcd pannel on;gpioa4
	nufront_set_gpio_value(4,1);


	gpio_pinmux_config(4);	//lcd pannel on;gpioa4
	nufront_set_gpio_value(4,1);
	udelay(20000);

	gpio_pinmux_config(94);         //LVDS set to 1;gpioc30
	nufront_set_gpio_value(94,1);
	udelay(300000);

	//set pwm
	writel(0x00028428, 0x06160000);
	udelay(30000);
	gpio_pinmux_config(5);          //enable LCD_BL_ON;gpio5
	nufront_set_gpio_value(5,1);
}

void lcd_newline(void * row_sec, int scroll_size)
{
	unsigned int value = lcdc_readl(panel_info.mmio, NUSMART_LCDC_CTRL);
	if(value & NUSMART_LCDC_PTRF){ /*secondary screen*/
		lcd_console_address_sec = lcd_console_address;

#if defined(CONFIG_PL330_DMA)
		dma330_cpy(lcd_console_address_pri, row_sec, scroll_size);	
#else		
		memcpy(lcd_console_address_pri, row_sec, scroll_size);
#endif

		lcd_console_address = lcd_console_address_pri;

		lcd_base = lcd_base_pri;
		/* Clear the last one */
		/*memset(CONSOLE_ROW_LAST, COLOR_MASK(lcd_color_bg), CONSOLE_ROW_SIZE);*/
		value &= ~NUSMART_LCDC_PTRF;
	}
	else{ /*primary screen*/
		lcd_console_address_pri = lcd_console_address;
		lcd_console_address_sec = lcd_base_sec+(lcd_console_address_pri - lcd_base);
#if defined(CONFIG_PL330_DMA)
		dma330_cpy(lcd_console_address_sec, row_sec, scroll_size);
#else
		memcpy(lcd_console_address_sec, row_sec, scroll_size);
#endif

		lcd_console_address = lcd_console_address_sec;

		lcd_base = lcd_base_sec;
		/* Clear the last one */
		/*memset(CONSOLE_ROW_LAST, COLOR_MASK(lcd_color_bg), CONSOLE_ROW_SIZE);*/
		value |= NUSMART_LCDC_PTRF;
	}
	//	lcdc_writel(panel_info.mmio, NUSMART_LCDC_CTRL, value);
	lcd_enable();
}
