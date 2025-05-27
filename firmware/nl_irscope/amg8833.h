/*
 * amg8833.h - single-file header for 8x8 IR thermocouple camera
 * 05-13-25 E. Brombaugh
 */

#ifndef __amg8833__
#define __amg8833__

// AMG8833 I2C address
#define AMG8833_I2C_ADDR 0x69

// I2C Bus clock rate - must be lower the Logic clock rate
#define AMG8833_I2C_CLKRATE 100000

// I2C Logic clock rate - must be higher than Bus clock rate
#define AMG8833_I2C_PRERATE 2000000

// uncomment this for high-speed 36% duty cycle, otherwise 33%
//#define AMG8833_I2C_DUTY

// I2C Timeout count
#define TIMEOUT_MAX 100000

// Register definitions
#define AMG8833_PCLT 0x00
#define AMG8833_RST 0x01
#define AMG8833_FPSC 0x02
#define AMG8833_INTC 0x03
#define AMG8833_STAT 0x04
#define AMG8833_SCLR 0x05
#define AMG8833_AVE 0x07
#define AMG8833_INTHL 0x08
#define AMG8833_INTHH 0x09
#define AMG8833_INTLL 0x0A
#define AMG8833_INTLH 0x0B
#define AMG8833_INTSL 0x0C
#define AMG8833_INTSH 0x0D
#define AMG8833_TTHL 0x0E
#define AMG8833_TTHH 0x0F
#define AMG8833_INT0 0x10
#define AMG8833_INT1 0x11
#define AMG8833_INT2 0x12
#define AMG8833_INT3 0x13
#define AMG8833_INT4 0x14
#define AMG8833_INT5 0x15
#define AMG8833_INT6 0x16
#define AMG8833_INT7 0x17
#define AMG8833_SETAVG 0x1F
#define AMG8833_T01L 0x80

// Reg Bit Values
#define AMG8833_PCLT_NORM 0x00
#define AMG8833_PCLT_SLEEP 0x10
#define AMG8833_RST_FLAG 0x30
#define AMG8833_RST_INIT 0x3F
#define AMG8833_FPSC_10HZ 0x00
#define AMG8833_FPSC_1HZ 0x01
#define AMG8833_INTC_INTMOD_ABS 0x02
#define AMG8833_INTC_INTEN 0x01
#define AMG8833_INTC_INTDIS 0x00

/*
 * reset and init the I2C port
 */
void amg8833_i2c_setup(void)
{
	uint16_t tempreg;
	
	// Reset I2C1 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;
	
	// set freq
	tempreg = I2C1->CTLR2;
	tempreg &= ~I2C_CTLR2_FREQ;
	tempreg |= (FUNCONF_SYSTEM_CORE_CLOCK/AMG8833_I2C_PRERATE)&I2C_CTLR2_FREQ;
	I2C1->CTLR2 = tempreg;
	
	// Set clock config
	tempreg = 0;
#if (AMG8833_I2C_CLKRATE <= 100000)
	// standard mode good to 100kHz
	tempreg = (FUNCONF_SYSTEM_CORE_CLOCK/(2*AMG8833_I2C_CLKRATE))&I2C_CKCFGR_CCR;
#else
	// fast mode over 100kHz
#ifndef AMG8833_I2C_DUTY
	// 33% duty cycle
	tempreg = (FUNCONF_SYSTEM_CORE_CLOCK/(3*AMG8833_I2C_CLKRATE))&I2C_CKCFGR_CCR;
#else
	// 36% duty cycle
	tempreg = (FUNCONF_SYSTEM_CORE_CLOCK/(25*AMG8833_I2C_CLKRATE))&I2C_CKCFGR_CCR;
	tempreg |= I2C_CKCFGR_DUTY;
#endif
	tempreg |= I2C_CKCFGR_FS;
#endif
	I2C1->CKCFGR = tempreg;
	
	// Enable I2C
	I2C1->CTLR1 |= I2C_CTLR1_PE;

	// set ACK mode
	I2C1->CTLR1 |= I2C_CTLR1_ACK;
}

/*
 * event checking
 */
enum events
{
	NOT_BUSY = 0,
	MSTR_MODE,
	TX_MODE,
	TX_EMPTY,
	TX_CMPLT,
	RX_MODE,
	RX_FULL,
	RX_CMPLT,
};

/*
 * event descriptions
 */
char *event_name[] =
{
	"not busy",
	"master mode",
	"transmit mode",
	"tx empty",
	"transmit complete",
	"receive mode",
	"rx full",
	"receive complete",
};

/*
 * error handler
 */
uint8_t amg8833_i2c_error(uint8_t evt)
{
	// report error
	printf("amg8833_i2c_error - timeout waiting for %s\n\r", event_name[evt]);
	
	// reset & initialize I2C
	amg8833_i2c_setup();

	return 1;
}

// event codes we use
#define  AMG8833_I2C_EVENT_MASTER_MODE_SELECT ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
#define  AMG8833_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  AMG8833_I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
#define  AMG8833_I2C_EVENT_MASTER_BYTE_TRANSMITTED ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */
#define  AMG8833_I2C_EVENT_MASTER_BYTE_RECEIVED ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

/*
 * check for 32-bit event codes
 */
uint8_t amg8833_i2c_chk_evt(uint32_t event_mask)
{
	/* read order matters here! STAR1 before STAR2!! */
	uint32_t status = I2C1->STAR1 | (I2C1->STAR2<<16);
	return (status & event_mask) == event_mask;
}

/*
 * low-level packet send for blocking polled operation via i2c
 */
uint8_t amg8833_i2c_send(uint8_t addr, uint8_t *data, uint8_t sz)
{
	int32_t timeout;
	
	// wait for not busy
	timeout = TIMEOUT_MAX;
	while((I2C1->STAR2 & I2C_STAR2_BUSY) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(NOT_BUSY);

	// Set START condition
	I2C1->CTLR1 |= I2C_CTLR1_START;
	
	// wait for master mode select
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(MSTR_MODE);
	
	// send 7-bit address + write flag
	I2C1->DATAR = addr<<1;

	// wait for receive condition
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(TX_MODE);

	// send data one byte at a time
	while(sz--)
	{
		// wait for TX Empty
		timeout = TIMEOUT_MAX;
		while(!(I2C1->STAR1 & I2C_STAR1_TXE) && (timeout--));
		if(timeout==-1)
			return amg8833_i2c_error(TX_EMPTY);
		
		// send command
		I2C1->DATAR = *data++;
	}

	// wait for tx complete
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(TX_CMPLT);

	// set STOP condition
	I2C1->CTLR1 |= I2C_CTLR1_STOP;
	
	// we're happy
	return 0;
}

/*
 * high-level register write
 */
uint8_t amg8833_reg_set(uint8_t reg, uint8_t data)
{
	uint8_t packet[2];
	
	packet[0] = reg;
	packet[1] = data;
	return amg8833_i2c_send(AMG8833_I2C_ADDR, packet, 2);
}

/*
 * low-level Register read for blocking polled operation via i2c
 */
uint8_t amg8833_i2c_reg_receive(uint8_t addr, uint reg, uint8_t *data, uint8_t sz)
{
	int32_t timeout;
	
	// wait for not busy
	timeout = TIMEOUT_MAX;
	while((I2C1->STAR2 & I2C_STAR2_BUSY) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(NOT_BUSY);

	// Set START condition
	I2C1->CTLR1 |= I2C_CTLR1_START;
	
	// wait for master mode select
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(MSTR_MODE);
	
	// send 7-bit address + write flag
	I2C1->DATAR = addr<<1;

	// wait for transmit condition
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(TX_MODE);

	// wait for TX Empty
	timeout = TIMEOUT_MAX;
	while(!(I2C1->STAR1 & I2C_STAR1_TXE) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(TX_EMPTY);
	
	// send register address
	I2C1->DATAR = reg;
	
	// wait for tx complete
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(TX_CMPLT);

	// Set repeated START condition
	I2C1->CTLR1 |= I2C_CTLR1_START;
	
	// wait for master mode select
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(MSTR_MODE);
	
	// send 7-bit address + read flag
	I2C1->DATAR = addr<<1 | 1;

	// wait for receive condition
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(RX_MODE);

	// receive data one byte at a time
	while(sz--)
	{
		// wait for RX Full
		timeout = TIMEOUT_MAX;
		while(!(I2C1->STAR1 & I2C_STAR1_RXNE) && (timeout--))
			if(!sz)
				I2C1->CTLR1 &= CTLR1_ACK_Reset;	// no ack on last byte
			else
				I2C1->CTLR1 |= CTLR1_ACK_Set;	// ack on intermediate bytes

		if(timeout==-1)
			return amg8833_i2c_error(RX_FULL);
		
		// receive data
		*data++ = I2C1->DATAR;
	}

#if 0
	// this times out - doesn't seem to be needed based on example code
	// wait for rx complete
	timeout = TIMEOUT_MAX;
	while((!amg8833_i2c_chk_evt(AMG8833_I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--));
	if(timeout==-1)
		return amg8833_i2c_error(RX_CMPLT);
#endif
	
	// set STOP condition
	I2C1->CTLR1 |= I2C_CTLR1_STOP;
	
	// we're happy
	return 0;
}

/*
 * high-level register read single byte
 */
uint8_t amg8833_i2c_reg_get(uint8_t reg, uint8_t *data)
{
	return amg8833_i2c_reg_receive(AMG8833_I2C_ADDR, reg, data, 1);
}

/*
 * high-level register read multi-byte
 */
uint8_t amg8833_i2c_reg_get_multi(uint8_t reg, uint8_t *data, uint16_t sz)
{
	return amg8833_i2c_reg_receive(AMG8833_I2C_ADDR, reg, data, sz);
}

/*
 * high-level read thermistor value
 */
uint8_t amg8833_get_thermistor(uint16_t *temp)
{
	return amg8833_i2c_reg_receive(AMG8833_I2C_ADDR, AMG8833_TTHL, (uint8_t *)temp, 2);
}

/*
 * high-level read array values
 */
uint8_t amg8833_get_array(uint16_t *array)
{
	return amg8833_i2c_reg_receive(AMG8833_I2C_ADDR, AMG8833_T01L, (uint8_t *)array, 128);
}

/*
 * init the GPIO port, init the I2C port and prep the sensor
 */
uint8_t amg8833_init(void)
{
	// Enable GPIOC and I2C
	RCC->APB1PCENR |= RCC_APB1Periph_I2C1;
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;

	// PC1 is SDA, 10MHz Output, alt func, open-drain
	GPIOC->CFGLR &= ~(0xf<<(4*1));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_OD_AF)<<(4*1);
	
	// PC2 is SCL, 10MHz Output, alt func, open-drain
	GPIOC->CFGLR &= ~(0xf<<(4*2));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_OD_AF)<<(4*2);

	// init the I2C port
	amg8833_i2c_setup();
	
#if 0
	// test loop for HW debug
	while(1)
	{
		amg8833_reg_set(0, 0);
		Delay_Ms(1);
	}
#endif
	
	// Set sensor to normal power mode
	printf("amg8833_init: normal power\n\r");
	if(amg8833_reg_set(AMG8833_PCLT, AMG8833_PCLT_NORM))
		return 1;
	
	// Initial reset
	printf("amg8833_init: initial reset\n\r");
	if(amg8833_reg_set(AMG8833_RST, AMG8833_RST_INIT))
		return 1;
	
	// Disable interrupts
	printf("amg8833_init: disable IRQs\n\r");
	if(amg8833_reg_set(AMG8833_INTC, AMG8833_INTC_INTDIS))
		return 1;
	
	// 10FPS
	printf("amg8833_init: 10FPS\n\r");
	if(amg8833_reg_set(AMG8833_FPSC, AMG8833_FPSC_10HZ))
		return 1;

	// we're happy
	return 0;
}
#endif
