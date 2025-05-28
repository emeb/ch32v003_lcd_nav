/*
 * test nav button & LCD
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
#include "rand.h"

/* build version in simple format */
const char *fwVersionStr = "V1.0";

/* build time */
const char *bdate = __DATE__;
const char *btime = __TIME__;

/* key to char mapping */
const char keychar[] =
{
	24,	// up
	26, // right
	4,	// center
	25,	// down
	27,	// left
};

/* key locations */
#define XCTR 80
#define YCTR 40
#define SPACING 26
#define RADIUS 10
const int16_t keyloc[] =
{
	XCTR, YCTR-SPACING,	// up
	XCTR+SPACING, YCTR,	// right
	XCTR, YCTR,			// center
	XCTR, YCTR+SPACING,	// down
	XCTR-SPACING, YCTR,	// left
};

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
	printf("\r\r\n\nTest Nav button and LCD\n\r");
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
	lcd_bkl(1);
#if 0
	printf("line test\n\r");
	gfx_set_forecolor(GFX_WHITE);
	gfx_drawline(0, 0, 159, 79);
	gfx_drawline(0, 79, 159, 0);
	gfx_set_forecolor(GFX_MAGENTA);
	gfx_drawstrctr(80, 40-4, "Hello World!");
#else
	printf("Font Test\n\r");
	gfx_set_forecolor(GFX_WHITE);
	for(int x=0;x<20;x++)
	{
		for(int y=0;y<10;y++)
		{
			gfx_drawchar(x*8,y*8,y*20+x);
		}
	}
#endif
	printf("waiting...\n\r");
	while(1) {}
#else
	gfx_clrscreen();
#endif
	lcd_bkl(1);
	

	while(1)
	{
		/* random text bubbles */
		GFX_COLOR color = rand()&0xffffff;
		gfx_set_forecolor(color);
		int16_t x = rand() % 160;
		int16_t y = rand() % 80;
		gfx_fillcircle(x, y, 15);
		gfx_set_forecolor(GFX_WHITE);
		gfx_set_backcolor(color);
		gfx_drawstrctr(x, y-4, "Hi");
		
		/* test buttons */
		gfx_set_txtscale(2);
		gfx_set_backcolor(GFX_BLACK);
		for(int i=0;i<NUM_BTNS;i++)
		{
			if(SysTick_get_button(i))
			{
				printf("%s\n\r", btn_name[i]);
				gfx_set_forecolor(GFX_BLACK);
				gfx_fillcircle(keyloc[2*i], keyloc[2*i+1], RADIUS);
				gfx_set_forecolor(GFX_WHITE);
				gfx_drawchar(keyloc[2*i]-8, keyloc[2*i+1]-8, keychar[i]);
			}
		}
		gfx_set_txtscale(1);
		
		Delay_Ms(100);
	}
}
