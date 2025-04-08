#include "LuSEE_SPI.h"

uint32_t  Flash_FS_Save, Flash_CAL_Save;
void*  SFL_RD_BUFF;
void* SFL_WR_BUFF;



void  SPI_4k_erase(uint32_t address) {}

void memcpy_to_flash(uint32_t tgt, void *src, uint32_t size) {}
void memcpy_from_flash(void* tgt, uint32_t src, uint32_t size) {}
