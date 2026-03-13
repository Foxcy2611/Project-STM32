#include "lcd.h"

#define LCD_ADDR 0x27 << 1
#define PIN_RS 0x01
#define PIN_EN 0x04
#define PIN_BL 0x08

static void delay_ms(uint32_t t)
{
	volatile uint32_t i, j;
	for (i = 0; i < t; i++)
		for (j = 0; j < 8000; j++)
			;
}

// Kh?i t?o I2C1
static void I2C1_Init(void)
{
	
	GPIO_InitTypeDef gpio;
	I2C_InitTypeDef i2c;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	// PB6=SCL, PB7=SDA
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);

	I2C_DeInit(I2C1);
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_OwnAddress1 = 0;
	i2c.I2C_Ack = I2C_Ack_Disable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &i2c);

	I2C_Cmd(I2C1, ENABLE);
}

// G?i 1 byte qua I2C t?i PCF8574
static void I2C_WriteByte(uint8_t data)
{
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
			;

	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
			;

	I2C_Send7bitAddress(I2C1, LCD_ADDR, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			;

	I2C_SendData(I2C1, data);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			;

	I2C_GenerateSTOP(I2C1, ENABLE);
}

// G?i 4 bit (nibble) dã ch?a trong data
static void LCD_I2C_Write4Bits(uint8_t data)
{
	I2C_WriteByte(data | PIN_EN);
	for (volatile int i = 0; i < 200; i++)
		;
	I2C_WriteByte(data & ~PIN_EN);
	for (volatile int i = 0; i < 200; i++)
		;
}

static void LCD_I2C_Send(uint8_t data, uint8_t mode)
{
	uint8_t high = (data & 0xF0) | PIN_BL | mode;
	uint8_t low = ((data << 4) & 0xF0) | PIN_BL | mode;

	LCD_I2C_Write4Bits(high);
	LCD_I2C_Write4Bits(low);
}

static void LCD_Command(uint8_t cmd)
{
	LCD_I2C_Send(cmd, 0);
}

static void LCD_Data(uint8_t data)
{
	LCD_I2C_Send(data, PIN_RS);
}

void LCD_Clear(void)
{
	LCD_Command(0x01);
	delay_ms(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t row_offsets[2] = {0x00, 0x40};
	if (row > 1)
			row = 1;
	LCD_Command(0x80 | (col + row_offsets[row]));
}

void LCD_PrintString(char *str)
{
	while (*str)
	{
		LCD_Data(*str++);
	}
}

void LCD_Printf(uint32_t value)
{
	char buf[16];
	snprintf(buf, sizeof(buf), "%u", value);
	LCD_PrintString(buf);
}

void LCD_Init(void)
{
	I2C1_Init();
	delay_ms(50);

	LCD_I2C_Write4Bits(0x30);
	delay_ms(5);
	LCD_I2C_Write4Bits(0x30);
	delay_ms(5);
	LCD_I2C_Write4Bits(0x30);
	delay_ms(5);

	LCD_I2C_Write4Bits(0x20);
	delay_ms(5);

	LCD_Command(0x28);
	LCD_Command(0x0C);
	LCD_Command(0x06);
	LCD_Clear();
}
