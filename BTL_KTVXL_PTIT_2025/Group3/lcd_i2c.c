#include "lcd_i2c.h"

static uint8_t displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
static uint8_t blv = LCD_BACKLIGHT;

static void TIM3_Delay_Init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseInitTypeDef tim;
  tim.TIM_ClockDivision = TIM_CKD_DIV1;
  tim.TIM_CounterMode = TIM_CounterMode_Up;
  tim.TIM_Period = 0xFFFF;
  tim.TIM_Prescaler = 7200 - 1; // 72MHz / 7200 = 10kHz
  tim.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &tim);
  TIM_Cmd(TIM3, ENABLE);
}

static void LCD_Delay_ms(uint32_t ms)
{
  uint32_t target = ms * 10; // 1 ms = 10 tick ở 10kHz

  TIM_SetCounter(TIM3, 0); // Reset CNT

  while (TIM_GetCounter(TIM3) < target)
    ;
}

static void I2C_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef gp;
  I2C_InitTypeDef i2c;

  gp.GPIO_Pin = I2C_SCL_Pin | I2C_SDA_Pin;
  gp.GPIO_Mode = GPIO_Mode_AF_OD;
  gp.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gp);

  I2C_DeInit(I2C1);
  i2c.I2C_ClockSpeed = 100000;
  i2c.I2C_Mode = I2C_Mode_I2C;
  i2c.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c.I2C_OwnAddress1 = 0x00;
  i2c.I2C_Ack = I2C_Ack_Enable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &i2c);

  I2C_Cmd(I2C1, ENABLE);
}

static void I2C1_Start(void)
{
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    ;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    ;
}

static void I2C1_SendAddr(uint8_t addr)
{
  I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    ;
}

static void I2C1_SendData(uint8_t data)
{
  I2C_SendData(I2C1, data);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    ;
}

static void I2C1_Stop(void)
{
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/////

static void LCD_SendByte(uint8_t data)
{
  I2C1_Start();
  I2C1_SendAddr(LCD_ADDR << 1);
  I2C1_SendData(data | blv);
  I2C1_Stop();
}

static void LCD_Pulse(uint8_t data)
{
  LCD_SendByte(data | ENABLE);
  LCD_SendByte(data & ~ENABLE);
}

static void LCD_Write_4BIT(uint8_t val)
{
  LCD_SendByte(val);
  LCD_Pulse(val);
}

static void LCD_Send(uint8_t val, uint8_t mode)
{
  uint8_t upper = val & 0xF0;
  uint8_t lower = (val << 4) & 0xF0;

  LCD_Write_4BIT(upper | mode);
  LCD_Write_4BIT(lower | mode);
}

void LCD_SendData(uint8_t data)
{
  LCD_Send(data, RS_DATA);
}

void LCD_SendCmd(uint8_t cmd)
{
  LCD_Send(cmd, RS_CMD);
}

void LCD_Clear(void)
{
  LCD_SendCmd(LCD_CLEARDISPLAY);
  LCD_Delay_ms(2);
}

void LCD_Display(void)
{
  displaycontrol |= LCD_DISPLAYON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  LCD_Delay_ms(1);
}

void LCD_Init(void)
{
  I2C_Config();
  LCD_Delay_ms(50);

  LCD_Write_4BIT(0x30);
  LCD_Delay_ms(5);
  LCD_Write_4BIT(0x30);
  LCD_Delay_ms(5);
  LCD_Write_4BIT(0x30);
  LCD_Delay_ms(5);

  LCD_Write_4BIT(0x20);
  LCD_Delay_ms(5);

  LCD_SendCmd(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE);

  LCD_Clear();
  LCD_Display();
}

void LCD_GotoXY(uint8_t x, uint8_t y)
{
  static const uint8_t Y_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (y > 3)
    y = 3;
  LCD_SendCmd(LCD_SETDDRAMADDR | (x + Y_offsets[y]));
}

void LCD_Puts(char *str)
{
  while (*str)
  {
    LCD_SendData(*str++);
  }
}
