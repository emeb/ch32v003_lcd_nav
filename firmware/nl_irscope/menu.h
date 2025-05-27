/*
 * menu.h - single-file header for irscope menu
 * 05-26-25 E. Brombaugh
 */

#ifndef __menu__
#define __menu__

#include <stdio.h>
#include <string.h>
#include "gfx.h"
#include "systick.h"

#define MNU_NUM_ITEMS 4
#define MNU_YSPACE 10
#define MNU_YSTART 2
#define MNU_XSTART 92

int8_t menu_item, prev_menu_item, menu_item_vals[MNU_NUM_ITEMS];
char textbuf[16];	

const char *menu_item_names[MNU_NUM_ITEMS] =
{
	"deg",
	"clr",
	"off",
	"amp",
};

const int8_t menu_item_limits[2*MNU_NUM_ITEMS] =
{
	0, 1,
	0, 3,
	-10, 20,
	0, 5,
};

const char clr[4] =
{
	'R', 'G', 'B', 'W',
};

/*
 * draw the menu
 */
void menu_render(uint8_t mask)
{
	uint8_t itembit = 1;
	GFX_RECT rect;
	
	/* update item selector */
	if(menu_item != prev_menu_item)
	{
		/* erase previous */
		rect.y0 = (prev_menu_item+MNU_YSTART)*MNU_YSPACE-1;
		rect.y1 = rect.y0 + (MNU_YSPACE-1);
		rect.x0 = MNU_XSTART-1;
		rect.x1 = 159;
		gfx_set_forecolor(GFX_BLACK);
		gfx_drawrect(&rect);
		
		/* draw current */
		rect.y0 = (menu_item+MNU_YSTART)*MNU_YSPACE-1;
		rect.y1 = rect.y0 + (MNU_YSPACE-1);
		gfx_set_forecolor(GFX_WHITE);
		gfx_drawrect(&rect);
		
		prev_menu_item = menu_item;
	}
	
	/* update all marked items */
	for(int i=0;i<MNU_NUM_ITEMS;i++)
	{
		if(mask & itembit)
		{
			gfx_drawstr(MNU_XSTART, (i+MNU_YSTART)*MNU_YSPACE, (char *)menu_item_names[i]);
			switch(i)
			{
				case 0:	/* degree scale */
					gfx_set_txtmode(menu_item_vals[i] ? GFX_TXTNORM : GFX_TXTREV);
					gfx_drawstr(MNU_XSTART+32, (i+MNU_YSTART)*MNU_YSPACE, "C");
					gfx_set_txtmode(!menu_item_vals[i] ? GFX_TXTNORM : GFX_TXTREV);
					gfx_drawstr(MNU_XSTART+48, (i+MNU_YSTART)*MNU_YSPACE, "F");
					gfx_set_txtmode(GFX_TXTNORM);
					break;
				
				case 1: /* color scale */
					for(int c=0;c<4;c++)
					{
						if(c==menu_item_vals[i])
							gfx_set_txtmode(GFX_TXTREV);
						else
							gfx_set_txtmode(GFX_TXTNORM);
						gfx_drawchar(MNU_XSTART+32+8*c, (i+MNU_YSTART)*MNU_YSPACE, clr[c]);
					}
					gfx_set_txtmode(GFX_TXTNORM);
					break;
				
				case 2:	/* offset */
					
					//break;
				
				case 3: /* gain */
					sprintf(textbuf, "%d ", menu_item_vals[i]);
					gfx_drawstr(MNU_XSTART+32, (i+MNU_YSTART)*MNU_YSPACE, textbuf);
					break;
			
			}
		}
		itembit <<=1 ;
	}
}

/*
 * initialize the menu
 */
void menu_init(void)
{
	menu_item = 0;
	prev_menu_item = 3;	// to force redraw at start
	for(int i=0;i<MNU_NUM_ITEMS;i++)
		menu_item_vals[i] = menu_item_limits[2*i];
	
	menu_render(0xff);
}

/*
 * process the menu
 */
void menu_proc(void)
{
	uint8_t mask = 0;
	
	/* select item */
	if(SysTick_get_button(BTN_UP))
	{
		menu_item--;
		menu_item = menu_item < 0 ? 0 : menu_item;
	}
	else if(SysTick_get_button(BTN_DOWN))
	{
		menu_item++;
		menu_item = menu_item >= MNU_NUM_ITEMS ? MNU_NUM_ITEMS-1 : menu_item;
	}
	
	/* adjust value */
	if(SysTick_get_button(BTN_LEFT))
	{
		menu_item_vals[menu_item]--;
		if(menu_item_vals[menu_item] < menu_item_limits[2*menu_item])
			menu_item_vals[menu_item] = menu_item_limits[2*menu_item];
		else
			mask |= 1<< menu_item;
	}
	else if(SysTick_get_button(BTN_RIGHT))
	{
		menu_item_vals[menu_item]++;
		if(menu_item_vals[menu_item] > menu_item_limits[2*menu_item+1])
			menu_item_vals[menu_item] = menu_item_limits[2*menu_item+1];
		else
			mask |= 1<< menu_item;
	}
	
	menu_render(mask);
}

#endif