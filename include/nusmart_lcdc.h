/*
 *  Header file for Nusmart Env LCD Controller
 *
 *  Data structure and register user interface
 *
 *  Copyright (C) 2007 Nufront Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __NUSMART_LCDC_H__
#define __NUSMART_LCDC_H__

#define 	NUSMART_LCDC_CTRL	0x00

#define		NUSMART_LCDC_EERF	(1<<18)
#define		NUSMART_LCDC_INTF_V	(1<<17)
#define		NUSMART_LCDC_INTF_H	(1<<16)
#define 	NUSMART_LCDC_PTRF	(1<<13)	/*primary=0,secondary=1*/
#define		NUSMART_LCDC_ENABLE	(1<<11)
#define		NUSMART_LCDC_PSF	(1<<10)
#define		NUSMART_LCDC_BLANK	(1<<9)
#define		NUSMART_LCDC_TEST	(1<<8)
#define		NUSMART_LCDC_RGBF	(1<<7)	/*RGB24*/
#define		NUSMART_LCDC_INTM_ERR	(1<<2)
#define		NUSMART_LCDC_INTM_V	(1<<1)
#define		NUSMART_LCDC_INTM_H	(1<<0)

#define		NUSMART_LCDC_HINTV	0x08
#define		NUSMART_LCDC_VINTV	0x0c
#define		INTV(fp,bp,syn)		((fp<<23)|(bp<<12)|(syn))

#define		NUSMART_LCDC_PRIPTR	0x10
#define		NUSMART_LCDC_SECPTR	0x14
#define		NUSMART_LCDC_MODULO	0x18
#define		NUSMART_LCDC_HVSIZE	0x1c
#define		NUSMART_LCDC_AXI	0x20
#define		NUSMART_LCDC_TRAN_NUM	0x30
#define 	NUSMART_BURST_LENGTH	(0x7 << 8)

#define		HV_SIZE(sh,sv)		((sv<<16)|sh)
//#define		NUSMART_LCDC_BASE	0x050d0000
#define		NUSMART_LCDC_BASE	0x05010000
#define		NUSMART_PCLK_BASE	0x051b0018
#define		NUSMART_PINMUX_BASE	0x051c004c
#define		NUSMART_LCDMUX_BASE	0x051c0008

#endif /* __NUSMART_LCDC_H__ */
