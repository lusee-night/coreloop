#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "cdi_interface.h"


#define MAX_COMMANDS 1000
#define STAGING_AREA_SIZE 32768

const char* commands_filename = "data/commands.dat";
const char* cdi_output = "data/cdi_output";
uint8_t comm_list[MAX_COMMANDS], arg_high_list[MAX_COMMANDS], arg_low_list[MAX_COMMANDS];
int wait_list[MAX_COMMANDS];
int cmd_ndx = 0;
int wait_ndx = 0;
int out_packet_ndx = 0;
int Ncommands;
void* TLM_BUF;


void cdi_init()
{
    FILE *file = fopen(commands_filename, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    int i = 0;
    
    while (fscanf(file, "%d %hhx %hhx %hhx", &wait_list[i], (unsigned char*)&comm_list[i], &arg_high_list[i], &arg_low_list[i]) == 4) {
        i++;
        if (i >= MAX_COMMANDS) {
            break;
        }
    }
    fclose(file);
    Ncommands = i;
    cmd_ndx = 0;
    wait_ndx = wait_list[0];
    TLM_BUF = malloc(STAGING_AREA_SIZE);
    printf("Read %i CDI commands.\n", Ncommands);
    
}

bool cdi_new_command(uint8_t *cmd, uint8_t *arg_high, uint8_t *arg_low ) {
    if (cmd_ndx >= Ncommands) {
        return false;
    }
    if (wait_ndx == 0) {
        *cmd = comm_list[cmd_ndx];
        *arg_high = arg_high_list[cmd_ndx];
        *arg_low = arg_low_list[cmd_ndx];
        cmd_ndx++;
        wait_ndx = wait_list[cmd_ndx];
        return true;
    } else {
        wait_ndx--;
        return false;
    }
}

bool cdi_ready() {return true;}
void wait_for_cdi_ready() {}

void cdi_dispatch (uint16_t appID, uint32_t length) {
    char filename[512];
    sprintf(filename, "%s/%05d_%04x.bin", cdi_output, out_packet_ndx, appID);
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }
    fwrite(TLM_BUF, sizeof(uint8_t), length, file);
    fclose(file);
    out_packet_ndx++;
}