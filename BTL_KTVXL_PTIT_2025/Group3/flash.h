// flash_user.h
#ifndef __FLASH__H
#define __FLASH__H

#include "stm32f10x.h"
#include <stdbool.h>

#define FLASH_PAGE_SIZE       1024
#define FLASH_BASE_ADDR       0x08000000
#define FLASH_PAGE_COUNT      64
#define FLASH_ADDR_LAST_PAGE  (FLASH_BASE_ADDR + (FLASH_PAGE_COUNT - 1) * FLASH_PAGE_SIZE)

typedef struct {
    uint32_t UID;
    uint32_t UserName;   
} FlashCard_t;

int Flash_Check_UID(uint32_t uid);           
void Flash_Erase_LastPage(void);
void Flash_Write_Card(uint32_t index, FlashCard_t *card);
void Flash_Read_Card(uint32_t index, FlashCard_t *card);
void Flash_Save_Card(uint32_t uid, uint32_t username);

#endif
