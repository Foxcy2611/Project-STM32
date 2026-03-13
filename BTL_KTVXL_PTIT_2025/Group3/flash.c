#include "flash.h"

void Flash_Erase_LastPage(void) {
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
  FLASH_ErasePage(FLASH_ADDR_LAST_PAGE);
  FLASH_Lock();
}

void Flash_Write_Card(uint32_t index, FlashCard_t *card) {
  uint32_t addr = FLASH_ADDR_LAST_PAGE + index * sizeof(FlashCard_t);
  uint32_t *data = (uint32_t*)card;

  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
  FLASH_ProgramWord(addr, data[0]);
  FLASH_ProgramWord(addr + 4, data[1]);
  FLASH_Lock();
}

void Flash_Read_Card(uint32_t index, FlashCard_t *card) {
  uint32_t addr = FLASH_ADDR_LAST_PAGE + index * sizeof(FlashCard_t);
  uint32_t *data = (uint32_t*)card;
  data[0] = *(__IO uint32_t*)addr;
  data[1] = *(__IO uint32_t*)(addr + 4);
}

int Flash_Check_UID(uint32_t uid) {
  FlashCard_t card;
  for(int i = 0; i < FLASH_PAGE_SIZE / sizeof(FlashCard_t); i++) {
    Flash_Read_Card(i, &card);
    if(card.UID == 0xFFFFFFFF) break; 
    if(card.UID == uid) return i;
  }
  
  return -1;
}

void Flash_Save_Card(uint32_t uid, uint32_t username) {
  if(Flash_Check_UID(uid) >= 0) return;

  FlashCard_t card = {uid, username};
  for(int i = 0; i < FLASH_PAGE_SIZE / sizeof(FlashCard_t); i++) {
    Flash_Read_Card(i, &card);
    if(card.UID == 0xFFFFFFFF) {
      Flash_Write_Card(i, &(FlashCard_t){uid, username});
      break;
    }
  }
}
