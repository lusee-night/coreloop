#include "LuSEE_SPI.h"
#include <string.h>
#include <stdio.h>

uint32_t  Flash_FS_Save, Flash_CAL_Save,  Flash_Region_1, Flash_Region_Size, Flash_Meta_Offset;
void*  SFL_RD_BUFF;
void* SFL_WR_BUFF;

char flash_sim[256*1024];


void  SPI_4k_erase(uint32_t address) {
    //printf("SPI_4k_erase: 0x%08X\n", address);
    memset(flash_sim + address, 0, 0x1000);

}

void memcpy_to_flash(uint32_t tgt, void *src, uint32_t size) {
    //printf("memcpy_to_flash: 0x%08X, size: %u\n", tgt, size);
    memcpy(flash_sim + tgt, src, size);

}

void memcpy_from_flash(void* tgt, uint32_t src, uint32_t size) {
    //printf("memcpy_from_flash: 0x%08X, size: %u\n", src, size);
    memcpy(tgt, flash_sim + src, size);
}

