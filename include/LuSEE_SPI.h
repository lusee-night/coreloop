#ifndef LUSEE_SPI_H
#define LUSEE_SPI_H

#include <stdint.h>

extern uint32_t Flash_Recov_Region_1;
extern void* SFL_RD_BUFF;
extern void* SFL_WR_BUFF;

void  SPI_EN_write();   //opcode 06h

void  SPI_read_page(uint32_t address);   //opcode 03h  read page

void  SPI_write_page(uint32_t address);   //opcode 02h  read page

void  SPI_4k_erase_step1(uint32_t address);   //opcode 20h  4k sector erase
void  SPI_4k_erase_step2();   //opcode 20h  4k sector erase
void  SPI_4k_erase_step3();   //opcode 20h  4k sector erase


void  SPI_write_page_step1(uint32_t address);   //opcode 02h  read page
void  SPI_write_page_step2();   //opcode 02h  read page
void  SPI_write_page_step3();   //opcode 02h  read page



#endif