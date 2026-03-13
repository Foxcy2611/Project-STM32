#include "Lcd_i2c.h"

// ------ PCF8574 I2C Address ------
#define ADD_PCF8574      			 0x27

// ------ Control bits for PCF8574 -> LCD ------
#define LCD_BACKLIGHT         0x08
#define LCD_ENABLE_BIT        0x04
#define LCD_RS_CMD            0x00
#define LCD_RS_DATA           0x01

// ------ LCD Command Base Codes ------
#define LCD_CLEAR_DISPLAY     0x01
#define LCD_RETURN_HOME       0x02
#define LCD_ENTRY_MODE_SET    0x04
#define LCD_DISPLAY_CONTROL   0x08
#define LCD_CURSOR_SHIFT      0x10
#define LCD_FUNCTION_SET      0x20
#define LCD_SET_CGRAM_ADDR    0x40
#define LCD_SET_DDRAM_ADDR    0x80

// ------ Flags for display entry mode ------
#define LCD_ENTRY_RIGHT             0x00
#define LCD_ENTRY_LEFT              0x02
#define LCD_ENTRY_SHIFT_INCREMENT   0x01
#define LCD_ENTRY_SHIFT_DECREMENT   0x00

// ------ Flags for display on/off control ------
#define LCD_DISPLAY_ON         0x04
#define LCD_DISPLAY_OFF        0x00
#define LCD_CURSOR_ON          0x02
#define LCD_CURSOR_OFF         0x00
#define LCD_BLINK_ON           0x01
#define LCD_BLINK_OFF          0x00

// ------ Flags for display/cursor shift ------
#define LCD_DISPLAY_MOVE       0x08
#define LCD_CURSOR_MOVE        0x00
#define LCD_MOVE_RIGHT         0x04
#define LCD_MOVE_LEFT          0x00

// ------ Flags for function set ------
#define LCD_8BIT_MODE          0x10
#define LCD_4BIT_MODE          0x00
#define LCD_2LINE              0x08
#define LCD_1LINE              0x00
#define LCD_5x10DOTS           0x04
#define LCD_5x8DOTS            0x00

// ------ Size Config ------
#define LCD_COLS_DEFAULT       20
#define LCD_ROWS_DEFAULT       4

// ------ Timing constants ------
#define LCD_CMD_DELAY_US       37
#define LCD_CLEAR_DELAY_MS     3
#define LCD_HOME_DELAY_MS      2

static LCD_State_t currentState = LCD_STATE_NONE;
static bool blinkStage = false;

static uint8_t warningBitmap[8] = {0x1F, 0x1B, 0x1B, 0x1B, 0x1B, 0x1F, 0x1B, 0x1F};
static uint8_t smileyIcon[8] = {0x00, 0x0A, 0x0A, 0x00, 0x00, 0x11, 0x0E, 0x00};


static uint8_t _backlightval = LCD_BACKLIGHT;      	// Bit BL on
static uint8_t _displaycontrol = LCD_DISPLAY_ON; 		// display ON, cursor OFF, blink OFF
static uint8_t _cols = LCD_COLS_DEFAULT;
static uint8_t _rows = LCD_ROWS_DEFAULT;

void delay_us_lcd(uint32_t us);
void delay_us_lcd(uint32_t us)
{
    SysTick->LOAD = 72u - 1u;
    SysTick->VAL = 0u;
    SysTick->CTRL = 1u | 1u << 2;
    while (us--)
    {
        while ((SysTick->CTRL & (1 << 16)) == 0)
            ;
    }
    SysTick->CTRL = 0;
}

void delay_ms_lcd(uint32_t ms);
void delay_ms_lcd(uint32_t ms)
{
    SysTick->LOAD = 72000u - 1u;
    SysTick->VAL = 0u;
    SysTick->CTRL = 1u | 1u << 2;
    while (ms--)
    {
        while ((SysTick->CTRL & (1 << 16)) == 0)
            ;
    }
    SysTick->CTRL = 0;
}

static void LCD_InitWarning(void)
{
	LCD_CreateChar(0, warningBitmap);
	LCD_CreateChar(1, smileyIcon);
}

// ------ I2C Low-level (SPL API) ------
static void I2C_Start(void)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
}

static void I2C_Send_Addr(uint8_t addr)
{
    I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
}

static void I2C_Send_Data(uint8_t data)
{
    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

static void I2C_Stop(void)
{
    I2C_GenerateSTOP(I2C1, ENABLE);
}

// ------ LCD Write Layer ------
static void Expander_Write(uint8_t data)
{
	I2C_Start();
	I2C_Send_Addr(ADD_PCF8574 << 1);
	I2C_Send_Data(data | _backlightval);
	I2C_Stop();
}

static void Pulse_Enable(uint8_t data)
{
	Expander_Write(data | LCD_ENABLE_BIT);
	delay_us_lcd(500);
	Expander_Write(data & ~LCD_ENABLE_BIT);
	delay_us_lcd(500);
}

static void Write4bits(uint8_t value)
{
	Expander_Write(value);
	Pulse_Enable(value);
}

static void Send(uint8_t value, uint8_t mode)
{
	uint8_t high = value & 0xF0;
	uint8_t low = (value << 4) & 0xF0;
	Write4bits(high | mode);
	Write4bits(low | mode);
}

static void I2C_BusRecovery(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	for (int i = 0; i < 9; i++) {
		GPIO_SetBits(GPIOB, GPIO_Pin_6);
		delay_us_lcd(10);
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);
		delay_us_lcd(10);
	}

	GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
	delay_ms_lcd(1);
}

static void LCD_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	// PB6 -> SCL, PB7 -> SDA
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// I2C Config
	I2C_DeInit(I2C1);
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 300000; // ? 300kHz
	I2C_Init(I2C1, &I2C_InitStruct);

	I2C_Cmd(I2C1, ENABLE);
}

// ------ Public Functions ------
void LCD_Begin(uint8_t cols, uint8_t rows)
{
	I2C_BusRecovery();
	delay_ms_lcd(100);
	LCD_I2C_Init();
	_cols = cols;
	_rows = rows;

	delay_ms_lcd(50);
	
	Write4bits(0x30);
	delay_ms_lcd(6);
	Write4bits(0x30);
	delay_us_lcd(150);
	Write4bits(0x30);
	delay_us_lcd(150);
	
	Write4bits(0x20); 	// 4-bit mode

	Send(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8DOTS, 0); 			// 4-bit, 2-line
	LCD_Clear();
	LCD_DisplayOn();
	LCD_SetEntryMode(1, 0);    // Set entry mode incre cursor and no shift display
	
	LCD_InitWarning();
}

void LCD_Clear(void)
{
	Send(LCD_CLEAR_DISPLAY, LCD_RS_CMD);
	delay_ms_lcd(LCD_CLEAR_DELAY_MS);
}

void LCD_Home(void)
{
	Send(LCD_RETURN_HOME, LCD_RS_CMD);
	delay_ms_lcd(LCD_HOME_DELAY_MS);
}

void LCD_DisplayOn(void)
{
	_displaycontrol |= LCD_DISPLAY_ON;
	Send(LCD_DISPLAY_CONTROL  | _displaycontrol, LCD_RS_CMD);
}
void LCD_DisplayOff(void)
{
	_displaycontrol &= ~LCD_DISPLAY_ON;
	Send(LCD_DISPLAY_CONTROL  | _displaycontrol, LCD_RS_CMD);
}
void LCD_CursorOn(void)
{
	_displaycontrol |= LCD_CURSOR_ON;
	Send(LCD_DISPLAY_CONTROL  | _displaycontrol, LCD_RS_CMD);
}
void LCD_CursorOff(void)
{
	_displaycontrol &= ~LCD_CURSOR_ON;
	Send(LCD_DISPLAY_CONTROL  | _displaycontrol, LCD_RS_CMD);
}
void LCD_BlinkOn(void)
{
	_displaycontrol |= LCD_BLINK_ON;
	Send(LCD_DISPLAY_CONTROL  | _displaycontrol, LCD_RS_CMD);
}
void LCD_BlinkOff(void)
{
	_displaycontrol &= ~LCD_BLINK_ON;
	Send(LCD_DISPLAY_CONTROL  | _displaycontrol, LCD_RS_CMD);
}

void LCD_ScrollLeft(void) 
{ 
	Send(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT, LCD_RS_CMD); 
}
void LCD_ScrollRight(void) 
{ 
	Send(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT, LCD_RS_CMD); 
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
	static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	if (row >= _rows)
		row = _rows - 1;
	if (col >= _cols)
		col = _cols - 1;
	Send(LCD_SET_DDRAM_ADDR  | (col + row_offsets[row]), LCD_RS_CMD);
}

void LCD_PrintChar(char kitu)
{
	Send(kitu, LCD_RS_DATA);
}

void LCD_Print(const char *str)
{
	while (*str)
	{
		Send(*str++, LCD_RS_DATA);
	}
}

void LCD_Printf(const char *format, ...)
{
	char buf[64];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	LCD_Print(buf);
}

void LCD_SetEntryMode(uint8_t leftToRight, uint8_t shiftDisplay)
{
	uint8_t mode = LCD_ENTRY_MODE_SET;
	if (leftToRight)
		mode |= LCD_ENTRY_LEFT;
	if (shiftDisplay)
		mode |= LCD_ENTRY_SHIFT_INCREMENT;
	Send(mode, LCD_RS_CMD);
}

void LCD_CreateChar(uint8_t index, uint8_t bitmap[8])
{
	if (index > 7) return;
	Send((uint8_t)(LCD_SET_CGRAM_ADDR  | (index << 3)), LCD_RS_CMD);
	for(uint8_t i = 0; i < 8; i++)
	{
		Send(bitmap[i], LCD_RS_DATA);
	}
	Send(LCD_SET_DDRAM_ADDR, LCD_RS_CMD);
}

void LCD_WriteChar(uint8_t index)
{
	if (index > 7) return;
	Send(index, LCD_RS_DATA);
}

static void LCD_ShowWarningStatic(void)
{
	LCD_SetCursor(0, 1);
	LCD_Printf("               ");
	LCD_SetCursor(3, 1);
	LCD_Print(" WARNING ");
}

static void LCD_UpdateWarningBlink(void)
{
	blinkStage = !blinkStage;
	if (blinkStage)
	{
		LCD_SetCursor(2, 1);
		LCD_WriteChar(0);
		LCD_SetCursor(12, 1);
		LCD_WriteChar(0);
		GPIOC->ODR |= (1 << 13);
	}
	else
	{
		LCD_SetCursor(2, 1);
		LCD_Print(" ");
		LCD_SetCursor(12, 1);
		LCD_Print(" ");
		GPIOC->ODR &= (uint16_t)(~(1 << 13));
	}
}

static void LCD_ShowSafe(void)
{
	LCD_SetCursor(0, 1);
	LCD_Printf("               ");
	LCD_SetCursor(4, 1);
	LCD_Print("NO GAS");
	LCD_SetCursor(2, 1);
	LCD_WriteChar(1);
	LCD_SetCursor(11, 1);
	LCD_WriteChar(1);
	GPIOC->ODR |= (1 << 13);
}

void LCD_CheckGasWarning(uint16_t gas_level, uint16_t threshold)
{
	if (gas_level > threshold)
	{
		if (currentState != LCD_STATE_WARNING)
		{
			currentState = LCD_STATE_WARNING;
			LCD_SetCursor(0, 1);
			LCD_Printf("            ");
			LCD_ShowWarningStatic();
		}
		
		LCD_UpdateWarningBlink();
	}
	else
	{
		if (currentState != LCD_STATE_SAFE)
		{
			currentState = LCD_STATE_SAFE;
			blinkStage = false;
			LCD_ShowSafe();
		}
	}
}
