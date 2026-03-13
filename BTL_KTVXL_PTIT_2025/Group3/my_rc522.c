#include "my_rc522.h"

// Bi?n static n?i b?
static uint8_t uid_buffer[4]; // Luu UID sau khi d?c
static uint8_t bcc_received;  // Luu BCC sau khi doc

/* ------------- Hàm static n?i b? -----------------*/
static void RC522_GPIO_Init(void);                          // Kh?i t?o GPIO cho SPI
static void RC522_SPI_Init(void);                           // KH?i t?o SPI
static void RC522_Select(void);                             // Ch?n module RFID (SPI)
static void RC522_Unselect(void);                           // B? ch?n
static void RC522_Reset(void);                              // Reset module
static uint8_t RC522_SPI_Transfer(uint8_t data);            // Ham gui 1 byte trong giao tiep SPI, dong thoi nhan lai 1 byte (giao tiep song cong)
static void RC522_WriteRegister(uint8_t addr, uint8_t val); // Vi?t vào thanh ghi c?a RC22
static uint8_t RC522_ReadRegister(uint8_t addr);            // Ð?c thanh ghi RC522
static void RC522_SetBitMask(uint8_t reg, uint8_t mask);    // Set Bit trong thanh ghi
static void RC522_ClearBitMask(uint8_t reg, uint8_t mask);  // Clear Bit trong thanh ghi
static uint8_t RC522_ToCard(uint8_t command, uint8_t *sendData,
                            uint8_t sendLen, uint8_t *backData, uint16_t *backLen); // Hàm giao ti?p v?i th?
static void RC522_AntennaOn(void);                                                  // Hàm b?t anten

static void RC522_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    // SDA (CS)
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = RC522_SDA_PIN;
    GPIO_Init(RC522_PORT, &gpio);

    // MOSI + SCK
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = RC522_MOSI_PIN | RC522_SCK_PIN;
    GPIO_Init(RC522_PORT, &gpio);

    // MISO
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = RC522_MISO_PIN;
    GPIO_Init(RC522_PORT, &gpio);

    // RST
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = RC522_RST_PIN;
    GPIO_Init(RC522_PORT, &gpio);

    GPIO_SetBits(RC522_PORT, RC522_SDA_PIN); // B? ch?n module khi v?a c?u hình xong
}

static void RC522_SPI_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    SPI_InitTypeDef spi;

    SPI_I2S_DeInit(SPI2);
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &spi);
    SPI_Cmd(SPI2, ENABLE);
}

static uint8_t RC522_SPI_Transfer(uint8_t data)
{
    SPI_I2S_SendData(SPI2, data);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
        ;
    return (uint8_t)SPI_I2S_ReceiveData(SPI2);
}

static void RC522_Select(void)
{
    GPIO_ResetBits(RC522_PORT, RC522_SDA_PIN);
}

static void RC522_Unselect(void)
{
    GPIO_SetBits(RC522_PORT, RC522_SDA_PIN);
}

static void RC522_Reset(void)
{
    GPIO_ResetBits(RC522_PORT, RC522_RST_PIN);
    for (volatile int i = 0; i < 100000; i++)
        ;
    GPIO_SetBits(RC522_PORT, RC522_RST_PIN);
    for (volatile int i = 0; i < 100000; i++)
        ;
}

static void RC522_WriteRegister(uint8_t addr, uint8_t val)
{
    RC522_Select();
    RC522_SPI_Transfer((addr << 1) & 0x7E);
    RC522_SPI_Transfer(val);
    RC522_Unselect();
}

static uint8_t RC522_ReadRegister(uint8_t addr)
{
    RC522_Select();
    RC522_SPI_Transfer(((addr << 1) & 0x7E) | 0x80);
    uint8_t val = RC522_SPI_Transfer(0x00);
    RC522_Unselect();
    return val;
}

static void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = RC522_ReadRegister(reg);
    RC522_WriteRegister(reg, tmp | mask);
}

static void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = RC522_ReadRegister(reg);
    RC522_WriteRegister(reg, tmp & (~mask));
}

static void RC522_AntennaOn(void)
{
    RC522_SetBitMask(TxControlReg, 0x03);
}

static uint8_t RC522_ToCard(uint8_t command, uint8_t *sendData,
                            uint8_t sendLen, uint8_t *backData, uint16_t *backLen)
{
    uint8_t status = 0;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t n, lastBits;
    uint16_t i;

    if (command == PCD_TRANSCEIVE)
    {
        irqEn = 0x77;
        waitIRq = 0x30;
    }

    RC522_WriteRegister(CommIEnReg, irqEn | 0x80);
    RC522_ClearBitMask(CommIrqReg, 0x80);
    RC522_SetBitMask(FIFOLevelReg, 0x80);

    for (uint8_t j = 0; j < sendLen; j++)
        RC522_WriteRegister(FIFODataReg, sendData[j]);

    RC522_WriteRegister(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
        RC522_SetBitMask(BitFramingReg, 0x80);

    i = 2000;
    do
    {
        n = RC522_ReadRegister(CommIrqReg);
        i--;
    } while ((i != 0) && !(n & waitIRq) && !(n & 0x01));

    RC522_ClearBitMask(BitFramingReg, 0x80);

    if (i && !(RC522_ReadRegister(ErrorReg) & 0x1B))
    {
        status = 1;
        n = RC522_ReadRegister(FIFOLevelReg);
        lastBits = RC522_ReadRegister(ControlReg) & 0x07;
        if (lastBits)
            *backLen = (n - 1) * 8 + lastBits;
        else
            *backLen = n * 8;
        if (n > 0 && n <= 64)
        {
            for (uint8_t j = 0; j < n; j++)
                backData[j] = RC522_ReadRegister(FIFODataReg);
        }
    }
    return status;
}

/* ----------- Hàm Public -------------------*/
bool RC522_IsCardPresent(void)
{
    uint8_t atqa[2];
    uint16_t backBits;
    RC522_WriteRegister(BitFramingReg, 0x07);
    atqa[0] = PICC_REQIDL;
    if (RC522_ToCard(PCD_TRANSCEIVE, atqa, 1, atqa, &backBits) && (backBits == 0x10))
        return true;
    return false;
}

uint8_t *RC522_GetUID(void)
{
    uint8_t buffer[5];
    uint16_t unLen;
    uint8_t status;

    RC522_WriteRegister(BitFramingReg, 0x00);
    buffer[0] = PICC_ANTICOLL;
    buffer[1] = 0x20;

    status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 2, buffer, &unLen);
    if (status != 1)
        return NULL;

    uint8_t check = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        uid_buffer[i] = buffer[i];
        check ^= buffer[i];
    }
    bcc_received = buffer[4];
    if (check != bcc_received)
        return NULL;

    return uid_buffer;
}

uint8_t RC522_GetBCC(void)
{
    return bcc_received;
}

void RC522_Init(void)
{
    RC522_GPIO_Init();
    RC522_SPI_Init();
    RC522_Reset();

    RC522_WriteRegister(TxModeReg, 0x00);
    RC522_WriteRegister(RxModeReg, 0x00);
    RC522_WriteRegister(ModWidthReg, 0x26);

    RC522_WriteRegister(TModeReg, 0x8D);
    RC522_WriteRegister(TPrescalerReg, 0x3E);
    RC522_WriteRegister(TReloadRegL, 30);
    RC522_WriteRegister(TReloadRegH, 0);

    RC522_WriteRegister(TxASKReg, 0x40);
    RC522_WriteRegister(ModeReg, 0x3D);
    RC522_AntennaOn();
}
