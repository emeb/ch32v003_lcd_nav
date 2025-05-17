/*
 * lcd.h - single-file header for RGB LED
 * 09-28-24 E. Brombaugh
 */

/*
 * Pin assignments for LCD on 20-pin package
 * Pin # | GPIO     | Signal   | Function
 *  13     PC3        LCD_NRST   Display low-rtue reset
 *  14     PC4        LCD_D/C    Display cmd/data 
 *  15     PC5        LCD_SCLK   Display serial clock
 *  16     PC6        LCD_MOSI   Display serial data
 *  17     PC7        LCD_CS     Display select 
 *  19     PD2        LCD_BKL    Display backlight 
 */

#ifndef __lcd__
#define __lcd__

#define LCD_NRST_PORT GPIOC
#define LCD_NRST_PIN 3
#define LCD_NRST_HIGH() LCD_NRST_PORT->BSHR = (1<<(LCD_NRST_PIN))
#define LCD_NRST_LOW() LCD_NRST_PORT->BSHR = (1<<(16+LCD_NRST_PIN))
#define LCD_DC_PORT GPIOC
#define LCD_DC_PIN 4
#define LCD_DC_DATA() LCD_DC_PORT->BSHR = (1<<(LCD_DC_PIN))
#define LCD_DC_CMD() LCD_DC_PORT->BSHR = (1<<(16+LCD_DC_PIN))
#define LCD_SCLK_PORT GPIOC
#define LCD_SCLK_PIN 5
#define LCD_MOSI_PORT GPIOC
#define LCD_MOSI_PIN 6
#define LCD_CS_PORT GPIOC
#define LCD_CS_PIN 7
#define LCD_CS_HIGH() LCD_CS_PORT->BSHR = (1<<(LCD_CS_PIN))
#define LCD_CS_LOW() LCD_CS_PORT->BSHR = (1<<(16+LCD_CS_PIN))
#define LCD_BKL_PORT GPIOD
#define LCD_BKL_PIN 2
#define LCD_BKL_HIGH() LCD_BKL_PORT->BSHR = (1<<(LCD_BKL_PIN))
#define LCD_BKL_LOW() LCD_BKL_PORT->BSHR = (1<<(16+LCD_BKL_PIN))

#define ST7735_TFTWIDTH 80
#define ST7735_TFTHEIGHT 160

#define ST_CMD            0x100
#define ST_CMD_DELAY      0x200
#define ST_CMD_END        0x400

#define ST77XX_NOP        0x00
#define ST77XX_SWRESET    0x01
#define ST77XX_RDDID      0x04
#define ST77XX_RDDST      0x09

#define ST77XX_SLPIN      0x10
#define ST77XX_SLPOUT     0x11
#define ST77XX_PTLON      0x12
#define ST77XX_NORON      0x13

#define ST77XX_INVOFF     0x20
#define ST77XX_INVON      0x21
#define ST77XX_DISPOFF    0x28
#define ST77XX_DISPON     0x29
#define ST77XX_CASET      0x2A
#define ST77XX_RASET      0x2B
#define ST77XX_RAMWR      0x2C
#define ST77XX_RAMRD      0x2E

#define ST77XX_PTLAR      0x30
#define ST77XX_COLMOD     0x3A
#define ST77XX_MADCTL     0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x08
#define ST77XX_MADCTL_MH  0x04


#define ST77XX_RDID1      0xDA
#define ST77XX_RDID2      0xDB
#define ST77XX_RDID3      0xDC
#define ST77XX_RDID4      0xDD

#define ST7735_SCRLAR     0x33
#define ST7735_VSCSAD     0x37

#define ST7735_FRMCTR1    0xB1
#define ST7735_FRMCTR2    0xB2
#define ST7735_FRMCTR3    0xB3
#define ST7735_INVCTR     0xB4
#define ST7735_DISSET5    0xB6

#define ST7735_PWCTR1     0xC0
#define ST7735_PWCTR2     0xC1
#define ST7735_PWCTR3     0xC2
#define ST7735_PWCTR4     0xC3
#define ST7735_PWCTR5     0xC4
#define ST7735_VMCTR1     0xC5

#define ST7735_GMCTRP1    0xE0
#define ST7735_GMCTRN1    0xE1

#define ST7735_PWCTR6     0xFC

/* ----------------------- private variables ----------------------- */
// Initialization command sequence
const static uint16_t
  initlst[] = {
    // mini is ST7735R
    // 7735R init, part 1 (red or green tab)
                                    // 15 commands in list:
    ST77XX_SWRESET | ST_CMD,        //  1: Software reset, 0 args, w/delay
    ST_CMD_DELAY | 15,             //     150 ms delay
    ST77XX_SLPOUT | ST_CMD,         //  2: Out of sleep mode, 0 args, w/delay
    ST_CMD_DELAY | 50,             //     500 ms delay
    ST7735_FRMCTR1 | ST_CMD,        //  3: Framerate ctrl - normal mode, 3 arg:
      0x01, 0x2C, 0x2D,             //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2 | ST_CMD,        //  4: Framerate ctrl - idle mode, 3 args:
      0x01, 0x2C, 0x2D,             //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3 | ST_CMD,        //  5: Framerate - partial mode, 6 args:
      0x01, 0x2C, 0x2D,             //     Dot inversion mode
      0x01, 0x2C, 0x2D,             //     Line inversion mode
    ST7735_INVCTR | ST_CMD,         //  6: Display inversion ctrl, 1 arg:
      0x07,                         //     No inversion
    ST7735_PWCTR1 | ST_CMD,         //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                         //     -4.6V
      0x84,                         //     AUTO mode
    ST7735_PWCTR2 | ST_CMD,         //  8: Power control, 1 arg, no delay:
      0xC5,                         //     VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
    ST7735_PWCTR3 | ST_CMD,         //  9: Power control, 2 args, no delay:
      0x0A,                         //     Opamp current small
      0x00,                         //     Boost frequency
    ST7735_PWCTR4 | ST_CMD,         // 10: Power control, 2 args, no delay:
      0x8A,                         //     BCLK/2,
      0x2A,                         //     opamp current small & medium low
    ST7735_PWCTR5 | ST_CMD,         // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 | ST_CMD,         // 12: Power control, 1 arg, no delay:
      0x0E,
    ST77XX_INVOFF | ST_CMD,         // 13: Don't invert display, no args
    ST77XX_MADCTL | ST_CMD,         // 14: Mem access ctl (directions), 1 arg:
      0xC8,                         //     row/col addr, bottom-top refresh
    ST77XX_COLMOD | ST_CMD,         // 15: set color mode, 1 arg, no delay:
      0x05,                         //     16-bit color#endif

                                    // 7735R init, part 2 (mini 160x128)
                                    //  2 commands in list:
    ST77XX_CASET | ST_CMD,          //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x7F,                   //     XEND = 127
    ST77XX_RASET | ST_CMD,          //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x9F,                   //     XEND = 159

                                    // 7735R init, part 3 (red or green tab)
                                    //  4 commands in list:
    ST7735_GMCTRP1 | ST_CMD,        //  1: Gamma Adjustments (pos. polarity), 16 args + delay:
      0x02, 0x1c, 0x07, 0x12,       //     (Not entirely necessary, but provides
      0x37, 0x32, 0x29, 0x2d,       //      accurate colors)
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1 | ST_CMD,        //  2: Gamma Adjustments (neg. polarity), 16 args + delay:
      0x03, 0x1d, 0x07, 0x06,       //     (Not entirely necessary, but provides
      0x2E, 0x2C, 0x29, 0x2D,       //      accurate colors)
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST77XX_NORON | ST_CMD,          //  3: Normal display on, no args, w/delay
    ST_CMD_DELAY | 10,              //     10 ms delay
    ST77XX_DISPON | ST_CMD,         //  4: Main screen turn on, no args w/delay
    ST_CMD_DELAY | 10,             //     100 ms delay
	ST_CMD_END                      //  END OF LIST
};

/* LCD state */
uint8_t rowstart, colstart;
uint16_t _width, _height, rotation;
	
/*
 * packet send for blocking polled operation via spi
 */
uint8_t lcd_pkt_send(uint8_t *data, uint8_t sz)
{
	// send data
	while(sz--)
	{
		// wait for TXE
		while(!(SPI1->STATR & SPI_STATR_TXE));
		
		// Send byte
		SPI1->DATAR = *data++;
	}
	
	// wait for not busy before exiting
	while(SPI1->STATR & SPI_STATR_BSY);
	
	// we're happy
	return 0;
}

/*
 * send single byte via SPI - cmd or data depends on bit 8
 */
void lcd_write_byte(uint16_t dat)
{
	uint8_t dat8 = dat & 0xff;
	
	if((dat & ST_CMD) == ST_CMD)
		LCD_DC_CMD();
	else
		LCD_DC_DATA();

	LCD_CS_LOW();
	
	lcd_pkt_send(&dat8, 1);
	
	LCD_CS_HIGH();
}

/*
 * opens a window into display mem for bitblt
 */
void lcd_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t tx_buf[4];
	uint16_t sum;

	lcd_write_byte(ST77XX_CASET | ST_CMD); // Column addr set
	sum = x0+colstart;
	tx_buf[0] = sum>>8;
	tx_buf[1] = sum&0xff;
	sum = x1+colstart;
	tx_buf[2] = sum>>8;
	tx_buf[3] = sum&0xff;
	LCD_DC_DATA();
	LCD_CS_LOW();
	lcd_pkt_send(tx_buf, 4);
	LCD_CS_HIGH();

	lcd_write_byte(ST77XX_RASET | ST_CMD); // Row addr set
	sum = y0+rowstart;
	tx_buf[0] = sum>>8;
	tx_buf[1] = sum&0xff;
	sum = y1+rowstart;
	tx_buf[2] = sum>>8;
	tx_buf[3] = sum&0xff;
	LCD_DC_DATA();
	LCD_CS_LOW();
	lcd_pkt_send(tx_buf, 4);
	LCD_CS_HIGH();

	lcd_write_byte(ST77XX_RAMWR | ST_CMD); // write to RAM
}

// draw single pixel
void lcd_drawPixel(int16_t x, int16_t y, uint16_t color)
{
	// clipping
	if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

	lcd_setAddrWindow(x,y,x+1,y+1);

	LCD_DC_DATA();
	LCD_CS_LOW();
	
	lcd_pkt_send((uint8_t *)&color, 2);

	LCD_CS_HIGH();
}

// fill a rectangle
void lcd_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t color)
{
	// clipping 
	if((x >= _width) || (y >= _height) || (x+w < 0) || (y+h < 0))
	{
		// entirely offscreen
		return;
	}
	if(x<0)
	{
		// off left
		w = w + x;	// trim width
		x = 0;	// clip to left boundary
	}
	if(y<0)
	{
		// off top
		h = h + y;	// trim height
		y = 0;	// clip to top boundary
	}
	if((x + w - 1) >= _width)
	{
		// off right
		w = _width  - x;	// trim width
	}
	if((y + h - 1) >= _height)
	{
		// off bottom
		h = _height - y;	// trim height
	}
	lcd_setAddrWindow(x, y, x+w-1, y+h-1);
	
	w*=h;
	
	/* prep tos end data */
	LCD_DC_DATA();
	LCD_CS_LOW();

	/* faster version keeps pipes full */
	while(w--)
		lcd_pkt_send((uint8_t *)&color, 2);

	LCD_CS_HIGH();
}

// swap high/low bytes in 16-bits
inline uint16_t lcd_revsh(uint16_t in)
{
	return ((in&0xff00)>>8) | ((in&0x00ff)<<8);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t lcd_Color565(uint32_t rgb24)
{
	uint16_t color16;
	color16 = 	(((rgb24>>16) & 0xF8) << 8) |
				(((rgb24>>8) & 0xFC) << 3) |
				((rgb24 & 0xF8) >> 3);
	
	return lcd_revsh(color16);
}

// Pass 16-bit packed color, get back 8-bit (each) R,G,B in 32-bit
uint32_t lcd_ColorRGB(uint16_t color16)
{
    uint32_t r,g,b;

	color16 = lcd_revsh(color16);
	
    r = (color16 & 0xF800)>>8;
    g = (color16 & 0x07E0)>>3;
    b = (color16 & 0x001F)<<3;
	return (r<<16) | (g<<8) | b;
}

// bitblt a region to the display - cannot clip here so caller must clip
void lcd_bitblt(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *buf)
{
	lcd_setAddrWindow(x, y, x+w-1, y+h-1);

	LCD_DC_DATA();
	LCD_CS_LOW();

	/* PIO buffer send */
	lcd_pkt_send((uint8_t *)buf, 2*w*h);

	LCD_CS_HIGH();
}


/*
 * set orientation of display
 */
void lcd_setRotation(uint8_t m)
{
	lcd_write_byte(ST77XX_MADCTL | ST_CMD);
	rotation = m % 4; // can't be higher than 3
	switch (rotation)
	{
		case 0:
			lcd_write_byte(ST77XX_MADCTL_RGB | ST77XX_MADCTL_MX | ST77XX_MADCTL_MY );
			_width  = ST7735_TFTWIDTH;
			_height = ST7735_TFTHEIGHT;
			rowstart = 0;
			colstart = 24;
			break;

		case 1:
			lcd_write_byte(ST77XX_MADCTL_RGB | ST77XX_MADCTL_MY | ST77XX_MADCTL_MV );
			_width  = ST7735_TFTHEIGHT;
			_height = ST7735_TFTWIDTH;
			rowstart = 24;
			colstart = 0;
			break;

		case 2:
			lcd_write_byte(ST77XX_MADCTL_RGB | 0);
			_width  = ST7735_TFTWIDTH;
			_height = ST7735_TFTHEIGHT;
			rowstart = 0;
			colstart = 24;
			break;

		case 3:
			lcd_write_byte(ST77XX_MADCTL_RGB | ST77XX_MADCTL_MX | ST77XX_MADCTL_MV );
			_width  = ST7735_TFTHEIGHT;
			_height = ST7735_TFTWIDTH;
			rowstart = 24;
			colstart = 0;
			break;
	}
}

/*
 * set backlight on/off
 */
void lcd_bkl(int8_t enable)
{
	if(!enable)
		LCD_BKL_LOW();
	else
		LCD_BKL_HIGH();
}

/*
 * init the LCD interface
 */
void lcd_init(void)
{
	uint32_t temp;
	
	/* power up GPIOC and SPI for control */
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;
	
	/* initial state of control lines */
	LCD_NRST_LOW();
	LCD_DC_CMD();
	LCD_CS_HIGH();
	
	/* setup gpio and spi outputs */
	temp = GPIOC->CFGLR & ~((0xf<<(4*LCD_NRST_PIN)) | (0xf<<(4*LCD_DC_PIN)) | 
		(0xf<<(4*LCD_SCLK_PIN)) | (0xf<<(4*LCD_MOSI_PIN)) | (0xf<<(4*LCD_CS_PIN)));
	temp |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*LCD_NRST_PIN);
	temp |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*LCD_DC_PIN);
	temp |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*LCD_SCLK_PIN);
	temp |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*LCD_MOSI_PIN);
	temp |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*LCD_CS_PIN);
	GPIOC->CFGLR = temp;
	
	/* Configure SPI */
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_8b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		SPI_BaudRatePrescaler_2;

	/* enable SPI port */
	SPI1->CTLR1 |= CTLR1_SPE_Set;
	
	/* power up GPIOD for backlight control */
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
	
	/* start in off condition */
	LCD_BKL_LOW();

	/* setup backlight output on PD2 */
	temp = GPIOD->CFGLR & ~((0xf<<(4*LCD_BKL_PIN)));
	temp |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*LCD_BKL_PIN);
	GPIOD->CFGLR = temp;
	
	/* init the controller */
	// default settings
	colstart = 24;
	rowstart = 0;
	_width  = ST7735_TFTWIDTH;
	_height = ST7735_TFTHEIGHT;
	rotation = 0;

	// Reset it
	LCD_NRST_LOW();
	Delay_Ms(10);
	LCD_NRST_HIGH();
	Delay_Ms(10);

	// Send init command list
	uint16_t *addr = (uint16_t *)initlst, ms;
	while(*addr != ST_CMD_END)
	{
		if((*addr & ST_CMD_DELAY) != ST_CMD_DELAY)
			lcd_write_byte(*addr++);
		else
		{
			ms = (*addr++)&0x1ff;        // strip delay time (ms)
			Delay_Ms(ms);
		}
	}
	
	// rotation?
	lcd_setRotation(0);
}

/* high level driver interface */
GFX_DRIVER ST7735_drvr =
{
	ST7735_TFTHEIGHT,
	ST7735_TFTWIDTH,
	lcd_init,
	lcd_setRotation,
    lcd_Color565,
    lcd_ColorRGB,
	lcd_fillRect,
	lcd_drawPixel,
	lcd_bitblt
};
#endif
