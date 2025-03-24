#ifndef LUSEE_SPI_H
#define LUSEE_SPI_H

#include <stdint.h>

extern uint32_t Flash_FS_Save;
extern void* SFL_RD_BUFF;
extern void* SFL_WR_BUFF;

// higher level functions
void memcpy_to_flash(uint32_t tgt, void *src, uint32_t size);
void memcpy_from_flash(void* tgt, uint32_t src, uint32_t size);



// higher level functions for 32k and chip erase

void  SPI_4k_erase(uint32_t address);   //opcode 20h  4k sector erase
void  SPI_32k_erase(uint32_t address);   //opcode 52h  32k sector erase
void  SPI_64k_erase(uint32_t address);   //opcode D8h  64k sector erase


// lower level functions for SPI
void   udealy( uint32_t time);
void  SPI_EN_write();   //opcode 06h
void  SPI_read_page(uint32_t address);   //opcode 03h  read page
void  SPI_write_page(uint32_t address);   //opcode 02h  read page



#endif