#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "cdi_interface.h"

#include <lusee_commands.h>

#include "cdi_options.h"


#define MAX_COMMANDS 1000
// FIFO size as per jack
#define CDI_BUF_SIZE 64 

#define STAGING_AREA_SIZE 32768

const char* commands_filename = "data/commands.dat";
const char* cdi_output = "data/cdi_output";
// These are used to store the commands read from the file
uint8_t comm_list[MAX_COMMANDS], arg_high_list[MAX_COMMANDS], arg_low_list[MAX_COMMANDS];
int wait_list[MAX_COMMANDS];


int cmd_ndx = 0; // current command
int Ncommands = 0; // number of commands
int wait_ndx = 0;
int out_packet_ndx = 0;
uint32_t cdi_command_counter;
struct outlier_info outliers;

void* TLM_BUF;
// sockets
int sockfd_in;
struct sockaddr_in serverAddr;



void cdi_init(){
    cdi_command_counter = 0;
    TLM_BUF = malloc(STAGING_AREA_SIZE);
    switch(cdi_format) {
        case CMD_FILE: {
            printf ("Reading CDI commands from file %s\n", cdi_in.file);
            FILE *file = fopen((char*) cdi_in.file, "r");
            if (file == NULL) {
                printf("Failed to open file.\n");
                exit(1);
                return;
            }

            int i = 0;

            while (fscanf(file, "%d %hhx %hhx %hhx", &wait_list[i], (unsigned char *) &comm_list[i], &arg_high_list[i],
                          &arg_low_list[i]) == 4) {
                i++;
                if (i >= MAX_COMMANDS) {
                    break;
                }
            }
            fclose(file);
            Ncommands = i;
            cmd_ndx = 0;
            wait_ndx = wait_list[0];
            printf("Read %i CDI commands.\n", Ncommands);
            break;
        }
        case CMD_PORT: {
            printf ("Reading CDI commands from UDP port %d\n", cdi_in.port);
            sockfd_in = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd_in < 0) {
                perror("Failed to create socket");
                exit(1);
            }
            // Set the socket to non-blocking mode
            
            int flags = fcntl(sockfd_in, F_GETFL, 0);
            fcntl(sockfd_in, F_SETFL, flags | O_NONBLOCK);

            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(cdi_in.port);
            serverAddr.sin_addr.s_addr = INADDR_ANY;

            if (bind(sockfd_in, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
                perror("Failed to bind socket");
                exit(1);
            }
            printf ("Bound to port %d\n", cdi_in.port);
            Ncommands = 0;
        }
        default:
            break;
    }
}

void cdi_fill_command_buffer() {
    if (cdi_format == CMD_FILE) return;
    if (cdi_format == CMD_PORT) {
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            uint8_t buffer[4];
            
            // Read from the socket
            ssize_t bytesRead = recvfrom(sockfd_in, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *)&clientAddr, &addrLen);
            if (bytesRead > 0) { 
                if (bytesRead != sizeof(buffer)) {
                    printf("Unexpected number of bytes read from socket.\n");
                    exit(1);
                }
                if (buffer[0]!='C') {
                    printf ("Unexpected command received from socket.\n");
                    exit(1);
                }
                comm_list[Ncommands] = buffer[1];
                arg_high_list[Ncommands] = buffer[2];
                arg_low_list[Ncommands] = buffer[3];
                wait_list[Ncommands] = 0;
                Ncommands++;
                if (Ncommands >= MAX_COMMANDS) {
                    printf("Too many commands received from socket.\n");
                    exit(1);
                }
                if ((Ncommands - cmd_ndx) > CDI_BUF_SIZE) {
                    printf ("WARNING: IN REALITY YOU WOULD BE HITTING THE 64 COMMAND BUFFER SIZE!\n");
                }
        }
    }   
}


bool cdi_new_command(uint8_t *cmd, uint8_t *arg_high, uint8_t *arg_low ) {
    // this should now work in either case.

    if (cmd_ndx >= Ncommands) return false;
    if (wait_ndx>0) {
        wait_ndx--;
        return false;
    }

    *cmd = comm_list[cmd_ndx];
    *arg_high = arg_high_list[cmd_ndx];
    *arg_low = arg_low_list[cmd_ndx];
    cmd_ndx++;
    if (cmd_ndx < Ncommands) wait_ndx = wait_list[cmd_ndx]; else wait_ndx = 0;
    cdi_command_counter++;
    uint16_t arg = (*arg_high << 8) | *arg_low;
    printf("Coreloop making new command\n");
    switch (arg) {
        case CTRL_OUTLIER_NUM: {
            outliers.num = arg;
            break;
        }
        case CTRL_OUTLIER_AMP: {
            outliers.amp = arg;
            break;
        }
        case CTRL_OUTLIER_BINS: {
            outliers.bins = arg;
            break;
        }
        default: {
            break;
        }
    }
    return true;
}


uint32_t cdi_command_count() {
    return cdi_command_counter;
}

bool cdi_ready() {return true;}
void wait_for_cdi_ready() {}

void cdi_dispatch (uint16_t appID, uint32_t length) {
    char filename[512];
    sprintf(filename, "%s/%05d_%04x.bin", (char*) cdi_out.file, out_packet_ndx, appID);
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }
    fwrite(TLM_BUF, sizeof(uint8_t), length, file);
    fclose(file);
    out_packet_ndx++;

}