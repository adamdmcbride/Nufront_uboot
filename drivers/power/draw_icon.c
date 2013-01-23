/*
filename:draw_icon.c

History
	hujian	2012/10/31	Creat file
	xujing	2012/12/21	update functions
	xujing	2012/12/29	add suspend/resume feature
*/
#include <draw_icon.h>
#include <lcd.h>

extern vidinfo_t panel_info;
static unsigned short*fb;
static unsigned int sh, sw;//pixel

#define VERTICAL_DISP
void set_bgfg(void);
void draw_init(unsigned short*fbr, unsigned int high, unsigned int width){
	fb = fbr;
#ifdef VERTICAL_DISP
	sh = high;
	sw = width;
#else
	sw = high;
	sh = width;
#endif
	lcd_ctrl_init ((unsigned)fbr);
	memset ((char *)fbr,0,sh*sw*2);
	set_bgfg();
}

void draw_lines(unsigned stx,unsigned sty,unsigned endx,unsigned endy,unsigned short rgb){
	int i, j;
	int dx = endx - stx;
	int dy = endy - sty;

	for(i = 0; i < dy; i++){
		for(j = 0; j < dx; j++){
			*(unsigned short*)((unsigned char*)fb + (sty+i)*sw*2 + (stx+j)*2) = rgb;
		}
	}
}

void clear_logo(void){
	int i, j;

	for(i = 0; i < sh; i++){
		for(j = 0; j < sw; j++){
			*(unsigned short*)((unsigned char*)fb + i*sw*2 + j*2) = BLACK;
		}
	}
}

/*draw a battary logo*/
void show_bat_framework(unsigned icon_h, unsigned icon_w)
{
	unsigned stx, sty, endx, endy, dy, stx1, sty1, endx1, endy1;

	stx = sw/2 - icon_w/2;
	sty = (sh - icon_h)/2;
	endx = sw/2 + icon_w/2;
	endy = (sh + icon_h)/2;
	draw_lines(stx, sty, endx, endy, WHITE);

	/*draw top of battary*/
	stx1 = stx+(endx-stx)/4;
	endx1 = stx1+(endx-stx)/2;
	sty1 = sty-(endy-sty)/10;
	endy1 = sty;
	draw_lines(stx1, sty1, endx1, endy1, WHITE);

	stx += 5;
	sty += 5;
	endx -= 5;
	endy -= 5;
	draw_lines(stx, sty, endx, endy, BLACK);
}

/*draw battary power percent logo*/
void show_bat_status(unsigned icon_h,unsigned icon_w,int status)
{
	unsigned stx, sty, endx, endy, dy, stx1, sty1, endx1, endy1, background_sty;

	stx = sw/2 - icon_w/2;
	sty = (sh - icon_h)/2;
	endx = sw/2 + icon_w/2;
	endy = (sh + icon_h)/2;

	stx += 5;
	sty += 5;
	endx -= 5;
	endy -= 5;

	dy = endy - sty;

	switch(status){
		case 0:
			draw_lines(stx, sty, endx, endy, BLUE);
			break;
		case 1:
			background_sty = sty;
			sty += dy*10/100;
			draw_lines(stx, background_sty, endx, sty, BLACK);
			draw_lines(stx, sty, endx, endy, BLUE);
			break;
		case 2:
			background_sty = sty;
			sty += dy*30/100;
			draw_lines(stx, background_sty, endx, sty, BLACK);
			draw_lines(stx, sty, endx, endy, BLUE);
			break;
		case 3:
			background_sty = sty;
			sty += dy*50/100;
			draw_lines(stx, background_sty, endx, sty, BLACK);
			draw_lines(stx, sty, endx, endy, BLUE);
			break;
		case 4:
			background_sty = sty;
			sty += dy*70/100;
			draw_lines(stx, background_sty, endx, sty, BLACK);
			draw_lines(stx, sty, endx, endy, BLUE);
			break;
		case 5:
			background_sty = sty;
			sty += dy*90/100;
			draw_lines(stx, background_sty, endx, sty, BLACK);
			draw_lines(stx, sty, endx, endy, RED);
			break;
		default:
			printf("error status value\n");
	}

}

