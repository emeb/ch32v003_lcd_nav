/*
 * nav button & LCD
 * 04-29-2025 E. Brombaugh
 */
 
/*
 * Pin assignments for 20-pin package
 * Pin # | GPIO     | Signal   | Function
 *  1      PD4        USB_DP     USB Data +
 *  2      PD5        USB_DPU    USB pullup
 *  3      PD6    	  JS_NW      Joystick NW switch
 *  4      PD7    	  NRST       Low-true reset
 *  5      PA1        JS_NE		 Joystick NE switch
 *  6      PA2   	  JS_CTR	 Joystick Center switch
 *  7      VSS        GND		 Power
 *  8      PD0        JS_SE      Joystick SE switch
 *  9      VDD        +3.3V      Power
 *  10     PC0        JS_SW      Joystick SW switch
 *  11     PC1        SDA        I2C data / piezo
 *  12     PC2        SCL        I2C clock / piezo 
 *  13     PC3        LCD_NRST   Display low-rtue reset
 *  14     PC4        LCD_D/C    Display cmd/data 
 *  15     PC5        LCD_SCLK   Display serial clock
 *  16     PC6        LCD_MOSI   Display serial data
 *  17     PC7        LCD_CS     Display select 
 *  18     PD1        SWIO       Programmer 
 *  19     PD2        LCD_BKL    Display backlight 
 *  20     PD3        USB_DM     USB Data - 
 */

/* uncomment this to try pwm hue */
#define HUE

#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "gfx.h"
#include "lcd.h"
#include "rand.h"
#include "am8833.h"

/* build version in simple format */
const char *fwVersionStr = "V1.0";

/* build time */
const char *bdate = __DATE__;
const char *btime = __TIME__;

/*
 * convert Thermistor to deg C
 */
void therm2c(uint16_t therm, uint8_t *c_int, uint16_t *c_frac)
{
	*c_frac = (therm&0xf) * 625;
	*c_int = (therm>>4);
}

/*
 * convert IR to deg C
 */
void ir2c(uint16_t ir, uint8_t *c_int, uint8_t *c_frac)
{
	*c_frac = (ir&3) * 25;
	*c_int = (ir>>2);
}

/*
 * Start here
 */
int main()
{
	char textbuf[16];
	
	// setup basic stuff - clocks, serial, etc
	SystemInit();

	// start serial @ default 115200bps
	Delay_Ms( 100 );
	printf("\r\r\n\nButton/LED Driver\n\r");
	printf("Version: %s\n\r", fwVersionStr);
	printf("Build Date: %s\n\r", bdate);
	printf("Build Time: %s\n\r", btime);
	
	/* init systick */
	systick_init();
	printf("initialized systick IRQ\n\r");
	
	/* init lcd */
	gfx_init(&ST7735_drvr);
	printf("initialized graphics & LCD\n\r");
#if 0
	gfx_set_forecolor(GFX_WHITE);
	gfx_drawline(0, 0, 159, 79);
	gfx_drawline(0, 79, 159, 0);
	gfx_set_forecolor(GFX_MAGENTA);
	gfx_drawstrctr(80, 40-4, "Hello World!");
#endif
	lcd_bkl(1);
	
	/* init am8833 IR sensor */
	if(am8833_init())
	{
		printf("IR sensor init failed... halting.\n\r");
		while(1) {}
	}
	else
		printf("initialized IR sensor\n\r");

	while(1)
	{
#if 0
		/* random text bubbles */
		GFX_COLOR color = rand()&0xffffff;
		gfx_set_forecolor(color);
		int16_t x = rand() % 160;
		int16_t y = rand() % 80;
		gfx_fillcircle(x, y, 15);
		gfx_set_forecolor(GFX_WHITE);
		gfx_set_backcolor(color);
		gfx_drawstrctr(x, y-4, "Hi");
#else
		/* 8x8 IR sensor */
		// readout built-in thermistor
		uint16_t temp, tcf;
		uint8_t tci;
		am8833_get_thermistor(&temp);
		therm2c(temp, &tci, &tcf);
		//printf("Thermistor: %d.%04d\n\r", tci, tcf);
		sprintf(textbuf, "%d.%04d", tci, tcf);
		gfx_set_forecolor(GFX_WHITE);
		gfx_drawstr(100, 0, textbuf);
		
		// get array
		uint16_t ir_array[64];
		uint8_t ci, cf;
		am8833_get_array(ir_array);
		
		// readout center element
		ir2c(ir_array[3*8+3], &ci, &cf);
		sprintf(textbuf, "%3d.%02d", ci, cf);
		gfx_drawstr(100, 10, textbuf);
		
		// render 8x8 array grid
		GFX_RECT rect;
		GFX_COLOR color;
		uint8_t hsv[3];
		hsv[0] = 0;
		hsv[1] = 255;
		for(int y = 0;y<8;y++)
		{
			for(int x = 0;x<8;x++)
			{
#if 1
				int16_t scale = ((int16_t)ir_array[y*8+x]-110)<<3;
				scale = scale < 0 ? 0 : scale;
				scale = scale > 255 ? 255 : scale;
				hsv[2] = scale;
				color = gfx_hsv2rgb(hsv);
#else
				color = 
#endif
				rect.x0 = x*10;
				rect.y0 = (7-y)*10;
				rect.x1 = rect.x0+9;
				rect.y1 = rect.y0+9;
				gfx_colorrect(&rect, color);
				
				// draw a box around center rect
				if((x==3)&&(y==3))
					gfx_drawrect(&rect);

			}
		}		
#endif
		
#if 0
		/* test buttons */
		for(int i=0;i<NUM_BTNS;i++)
		{
			if(SysTick_get_button(i))
				printf("%s\n\r", btn_name[i]);
		}
#endif
		
		Delay_Ms(100);
	}
}
