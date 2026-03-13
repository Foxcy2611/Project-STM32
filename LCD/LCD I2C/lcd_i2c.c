#include "lcd_i2c.h"
#include "delay.h"

// trang thái LCD
static uint8_t displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
// luu trang thai hien tai:         bat/tat     con tro bat/tat   nhap nhay
static uint8_t blv = LCD_BACKLIGHT;
// nêfn den
// --------- hàm I2C -----------
static void EX_Write(uint8_t data){
  I2C_Start(LCD_ADDR << 1, I2C_Direction_Transmitter);
	// gui bit start + dia chi lcd(r/w = 0 : ghi du lieu)
  I2C_Write(data | blv); // gui du lieu hoac lenh, luon kem theo neefn den
  I2C_Stop(); // dung
}

static void PL_EN(uint8_t data){
  EX_Write(data | ENABLE);
  delay_ms(1);
  EX_Write(data & ~ENABLE);
  delay_ms(1);
	// tao xung chan EN gui du lieu
	// khi muon gui lenh hay gi, gui xong phai tao xung nay 
}

static void WRITE_4BIT(uint8_t val){
  EX_Write(val); // gui 4 bit cao qua I2C
  PL_EN(val);// tao xung de gui
}

static void Send(uint8_t val, uint8_t mode){
  // 1 byte = 8 bit, upper lay 4 bit cao, lower lay 4 bit thap
	uint8_t upper = val & 0xF0;
  uint8_t lower = (val << 4) & 0xF0;

  WRITE_4BIT(upper | mode); // gui cao roi den thap
  WRITE_4BIT(lower | mode);
}

// --------- hàm gui data / cmd -----------
void LCD_SendData(uint8_t data){
  Send(data, RS_DATA);
}

void LCD_SendCmd(uint8_t cmd){
  Send(cmd, RS_CMD); // cmd la lenh
}

// --------- hàm kh?i t?o LCD -----------
void LCD_Init(uint8_t lines){
  delay_ms(50);

  WRITE_4BIT(0x30);
  delay_ms(5);
  WRITE_4BIT(0x30);
  delay_ms(5);
  WRITE_4BIT(0x30);
  delay_ms(5);
	// gui 3 lan 0x30 de khoi tao lcd 8 bit mode chuan

  WRITE_4BIT(0x20); // chuyen sang 4 bit
  delay_ms(5);

	// chuyen che do giao tiep 8 bit sang 4 bit vs lcd
	
  if(lines > 1){
    LCD_SendCmd(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE);
  } else {
    LCD_SendCmd(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE);
  }

  LCD_Clear();
  LCD_display();
}

// --------- các hàm dieu khien -----------
void LCD_Clear(void){ // xoa man hinh
  LCD_SendCmd(LCD_CLEARDISPLAY);
  delay_ms(2);
}

void LCD_Home(void){ // dua con tro ve x,y = (0 0)
  LCD_SendCmd(LCD_RETURNHOME);
  delay_ms(2);
}

void LCD_noDisplay(void){ // tat man hinh
  displaycontrol &= ~LCD_DISPLAYON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  delay_ms(1);
}

void LCD_display(void){ // bat man hinh
  displaycontrol |= LCD_DISPLAYON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  delay_ms(1);
}

void LCD_noBlink(void){ // tat blink con tro
  displaycontrol &= ~LCD_BLINKON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  delay_ms(1);
}

void LCD_blink(void){ // bat blink con tro
  displaycontrol |= LCD_BLINKON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  delay_ms(1);
}

void LCD_noCursor(void){ // // tat con tro
  displaycontrol &= ~LCD_CURSORON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  delay_ms(1);
}

void LCD_Cursor(void){ // bat con tro
  displaycontrol |= LCD_CURSORON;
  LCD_SendCmd(LCD_DISPLAYCONTROL | displaycontrol);
  delay_ms(1);
}

void LCD_scrollDisplayRight(void){ // dich noi dung sang phai
  LCD_SendCmd(LCD_CURSORSHIFT | 0x04);
  delay_ms(1);
}

void LCD_scrollDisplayLeft(void){ // dich noi dung sang trai
  LCD_SendCmd(LCD_CURSORSHIFT | 0x00);
  delay_ms(1);
}

void LCD_SetEntryMode(uint8_t leftToRight, uint8_t shiftDisplay){ 
  uint8_t mode = LCD_ENTRYMODESET;
  mode |= leftToRight ? LCD_ENTRYLEFT : LCD_ENTRYRIGHT;
  mode |= shiftDisplay ? LCD_ENTRYSHIFTINCREMENT : LCD_ENTRYSHIFTDECREMENT;
  LCD_SendCmd(mode);
  delay_ms(1);
}

void LCD_SetCursor(uint8_t col, uint8_t row){ // dat con tro 
	// vdu dat vi tri 0 1 va viet
  static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if(row > 3) row = 3;
  LCD_SendCmd(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_SendString(const char *str){ // nhap chu can viet
  while(*str){
    LCD_SendData(*str++);
  }
}
