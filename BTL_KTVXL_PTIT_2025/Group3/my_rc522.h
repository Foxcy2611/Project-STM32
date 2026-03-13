#ifndef RC522_H
#define RC522_H

#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include <stdbool.h>
#include <stdio.h>

// RFID Pin
#define RC522_PORT          GPIOB
#define RC522_SDA_PIN       GPIO_Pin_12
#define RC522_SCK_PIN       GPIO_Pin_13
#define RC522_MOSI_PIN      GPIO_Pin_15
#define RC522_MISO_PIN      GPIO_Pin_14
#define RC522_RST_PIN       GPIO_Pin_8

/* ----------- Register RC522 -------------------- */
// Page 0: Command and Status
#define CommandReg          0x01
#define CommIEnReg          0x02
#define DivIEnReg           0x03
#define CommIrqReg          0x04
#define DivIrqReg           0x05
#define ErrorReg            0x06
#define Status1Reg          0x07
#define Status2Reg          0x08
#define FIFODataReg         0x09
#define FIFOLevelReg        0x0A
#define WaterLevelReg       0x0B
#define ControlReg          0x0C
#define BitFramingReg       0x0D
#define CollReg             0x0E

// Page 1: Command
#define ModeReg             0x11
#define TxModeReg           0x12
#define RxModeReg           0x13
#define TxControlReg        0x14
#define TxASKReg            0x15
#define TxSelReg            0x16
#define RxSelReg            0x17
#define RxThresholdReg      0x18
#define DemodReg            0x19
#define MfTxReg             0x1C
#define MfRxReg             0x1D
#define SerialSpeedReg      0x1F

// Page 2: Configuration
#define CRCResultRegH       0x21
#define CRCResultRegL       0x22
#define ModWidthReg         0x24
#define RFCfgReg            0x26
#define GsNReg              0x27
#define CWGsPReg            0x28
#define ModGsPReg           0x29
#define TModeReg            0x2A
#define TPrescalerReg       0x2B
#define TReloadRegH         0x2C
#define TReloadRegL         0x2D
#define TCounterValueRegH   0x2E
#define TCounterValueRegL   0x2F

// Page 3: Test
#define TestSel1Reg         0x31
#define TestSel2Reg         0x32
#define TestPinEnReg        0x33
#define TestPinValueReg     0x34
#define TestBusReg          0x35
#define AutoTestReg         0x36
#define VersionReg          0x37
#define AnalogTestReg       0x38
#define TestDAC1Reg         0x39
#define TestDAC2Reg         0x3A
#define TestADCReg          0x3B

/* ----------L?nh c?a RC522------------------ */
#define PCD_IDLE            0x00
#define PCD_TRANSCEIVE      0x0C
#define PCD_RESETPHASE      0x0F

/* ----------L?nh c?a th?------------------ */
#define PICC_REQIDL         0x26
#define PICC_ANTICOLL       0x93

/* ---------- Nguyên m?u hàm Public------------------ */
void RC522_Init(void);
bool RC522_IsCardPresent(void);
uint8_t* RC522_GetUID(void);
uint8_t RC522_GetBCC(void);

#endif
