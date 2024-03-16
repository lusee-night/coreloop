#include "global.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "lusee_settings.h"
#include "core_loop.h"
#include <string.h> // Add include for memcpy


/***************** GLOBAL STATE ******************/

uint16_t Navg2 = 2048;
uint16_t Navg2_shift = 11;




/**************************************************/



void cdi_process_command(uint8_t cmd, uint8_t arg_high, uint8_t arg_low)
{
    // Do something with the command
    printf("Received command: %x %x %x\n", cmd, arg_high, arg_low);
    if (cmd==RFS_Settings)  {
        switch (arg_high) {
            case RFS_SET_START:
                spec_set_spectrometer_enable(true);
                return;
            case RFS_SET_STOP:
                spec_set_spectrometer_enable(false);
                return;
        }
    }
    printf ("   Commmand not implemented, ignoring.\n");
    return;
}


void core_loop()
{
    uint8_t cmd, arg_high, arg_low;
    uint16_t avg_counter = 0;


    for (;;)
    {
        // Check if we have a new command from the CDI
        if (cdi_new_command(&cmd, &arg_high, &arg_low)) {
            if ((cmd==RFS_Settings) && (arg_high==RFS_SET_TIME_TO_DIE)) break;
            cdi_process_command(cmd, arg_high, arg_low);
        }    
        // Check if we have a new spectrum packet from the FPGA
        if (spec_new_spectrum_ready())
        {
            // Want to now transfer all 16 pks worth of data to DDR memory
            int32_t *df_ptr = (int32_t *)DF_BASE_ADDR;
            int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;

            for (uint16_t i = 0; i < (NCHANNELS*NSPECTRA); i++)
            {
                *ddr_ptr += ((*df_ptr) / (2 << Navg2_shift));
                df_ptr++;
                ddr_ptr++;
            }
            //printf ("Processing spectrum %i\n", avg_counter);
            avg_counter++;
            spec_clear_df_flag(); // Clear the flag to indicate that we have read the data

            // Check if we have reached filled up Stage 2 averaging
            // and if so, push things out to CDI
            if (avg_counter == Navg2)
            {
                avg_counter = 0;
                // Now one by one, we will loop through the packets placed in DDR Memory
                // For each channel, set the APID, send it to the SRAM
                // Then check to see if this software or the client will control the CDI writes
                int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;
                int32_t *cdi_ptr = (int32_t *)CDI_BASE_ADDR;
                printf ("Dumping averaged spectra to CDI\n");
                for (uint8_t ch = 0; ch < NSPECTRA; ch++)
                {
                    while (!cdi_ready()) {}
                    memcpy(cdi_ptr, ddr_ptr, NCHANNELS * sizeof(uint32_t));
                    memset(ddr_ptr, 0, NCHANNELS * sizeof(uint32_t));
                    printf("   Writing spectrum for ch %i\n",ch);
                    cdi_dispatch(0x210+ch, NCHANNELS*sizeof(int32_t));
                    ddr_ptr += NCHANNELS;
                }
            }
            // make sure we have done this in time
            if (spec_df_flag())
            {
                printf("Error: missed a spectrum packet\n");
            }
        }
    }
}