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

#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "gfx.h"
#include "lcd.h"
#include "amg8833.h"
#include "menu.h"

/* build version in simple format */
const char *fwVersionStr = "V1.0";

/* build time */
const char *bdate = __DATE__;
const char *btime = __TIME__;

/*
 * convert Thermistor to int/frac in C or F
 */
void therm2if(uint16_t therm, uint8_t *c_int, uint16_t *c_frac, uint8_t f)
{
	if(f)
	{
		/* convert to fahrenheit: 1843/1024 + (32*16) */
		uint32_t temp = ((1843*(uint32_t)therm) >> 10) + (32<<4);
		therm = temp;
	}
	*c_frac = (therm&0xf) * 625;
	*c_int = (therm>>4);
}

/*
 * convert IR element to int/frac in C or F
 */
void ir2if(uint16_t ir, uint8_t *c_int, uint8_t *c_frac, uint8_t f)
{
	if(f)
	{
		/* convert to fahrenheit: 1843/1024 + (32*4) */
		uint32_t temp = ((1843*(uint32_t)ir) >> 10) + (32<<2);
		ir = temp;
	}
	*c_frac = (ir&3) * 25;
	*c_int = (ir>>2);
}

/*
 * convert byte to RGB color
 */
GFX_COLOR color_map(uint8_t scale, uint8_t map)
{
	GFX_COLOR result;
	
	switch(map)
	{
		case 0: /* Red */
			result = scale << 16;
			break;
		
		case 1: /* Green */
			result = scale << 8;
			break;
		
		case 2: /* Blue */
			result = scale;
			break;
		
		case 3: /* White */
		default:
			result = (scale << 16) | (scale << 8) | scale ;\
			break;
	}
	return result;
}

/*
 * Start here
 */
int main()
{
	// setup basic stuff - clocks, serial, etc
	SystemInit();

	// start serial @ default 115200bps
	Delay_Ms( 100 );
	printf("\r\r\n\nNL IRScope\n\r");
	printf("Version: %s\n\r", fwVersionStr);
	printf("Build Date: %s\n\r", bdate);
	printf("Build Time: %s\n\r", btime);
	
	/* init systick */
	systick_init();
	printf("initialized systick IRQ\n\r");
	
	/* init lcd */
	gfx_init(&ST7735_drvr);
	gfx_clrscreen();
	lcd_bkl(1);
	printf("initialized graphics & LCD\n\r");
	
	/* init am8833 IR sensor */
	gfx_set_forecolor(GFX_WHITE);
	gfx_drawstrctr(80, 40-4, "Initializing Sensor");
	Delay_Ms(100);
	if(amg8833_init())
	{
		gfx_set_forecolor(GFX_RED);
		gfx_drawstrctr(80, 40-4, "  IR Sensor failed  ");
		printf("IR sensor init failed... halting.\n\r");
		while(1) {}
	}
	else
	{	gfx_clrscreen();
		printf("initialized IR sensor\n\r");
	}

	/* start menu */
	menu_init();
	printf("initialized menu\n\r");

	printf("Looping...\n\r");
	while(1)
	{
		// readout built-in thermistor
		uint16_t temp, tf;
		uint8_t ti;
		amg8833_get_thermistor(&temp);
		therm2if(temp, &ti, &tf, menu_item_vals[0]);
		//printf("Thermistor: %d.%04d\n\r", ti, tf);
		sprintf(textbuf, "%d.%04d", ti, tf);
		gfx_set_forecolor(GFX_WHITE);
		gfx_drawstr(MNU_XSTART, 0, textbuf);
		
		// get array
		uint16_t ir_array[64];
		uint8_t ci, cf;
		amg8833_get_array(ir_array);
		
		// readout center element
		ir2if(ir_array[3*8+3], &ci, &cf, menu_item_vals[0]);
		sprintf(textbuf, "%3d.%02d", ci, cf);
		gfx_drawstr(MNU_XSTART, MNU_YSPACE, textbuf);
		
		// render 8x8 array grid
		GFX_RECT rect;
		for(int y = 0;y<8;y++)
		{
			/* render graphics */
			for(int x = 0;x<8;x++)
			{
				int16_t scale = ((int16_t)ir_array[y*8+x]-110 + menu_item_vals[2])<<menu_item_vals[3];
				scale = scale < 0 ? 0 : scale;
				scale = scale > 255 ? 255 : scale;
				rect.x0 = x*10;
				rect.y0 = y*10;
				rect.x1 = rect.x0+9;
				rect.y1 = rect.y0+9;
				gfx_colorrect(&rect, color_map(scale, menu_item_vals[1]));
				
				// draw a box around center rect
				if((x==3)&&(y==3))
					gfx_drawrect(&rect);

			}
		}
		
		/* handle menu */
		menu_proc();
		
		Delay_Ms(100);
	}
}
