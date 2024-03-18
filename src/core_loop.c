#include <string.h> // Add include for memcpy
#include <stdlib.h>

#include "global.h"
#include "core_loop.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "lusee_settings.h"


/***************** GLOBAL STATE ******************/

struct core_state state;


/**************************************************/


void cdi_not_implemented(const char *msg)
{
    printf("CDI command not implemented: %s\n",msg);
    exit(1);
    return;
}


void set_spectrometer_to_state(struct core_state state)
{
    for (int i = 0; i < NINPUT; i++) {
        spec_set_gain(i, state.gain[i]);
        spec_set_route(i, state.route[i].plus, state.route[i].minus);
    }
    spec_set_avg1 (state.Navg1_shift);
    return;
}


void core_init_state(){
    for (int i = 0; i < NINPUT; i++) {
        state.gain[i] = GAIN_MED;
        state.auto_gain[i] = false;
        state.route[i].plus = i;
        state.route[i].minus = 0xFF;
    }
    state.Navg1 = 2048;
    state.Navg1_shift = 11;
    state.Navg2 = 512;
    state.Navg2_shift = 9;
    set_spectrometer_to_state(state);
}


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
            case RFS_SET_RESET:
                spec_set_reset();
                return;
            case RFS_SET_STORE:
                spec_store();
                return;
            case RFS_SET_RECALL:
                spec_recall();
                return;
            case RFS_SET_HK_REQ:
                cdi_not_implemented("RFS_SET_HK_REQ");
                return;
            case RFS_SET_TIME_TO_DIE:
                // this is handled in the main loop
                return;
            case RFS_SET_TEST_INT:
                cdi_not_implemented("RFS_SET_TEST_INT");
                return;
            case RFS_SET_TEST_SHORT:
                cdi_not_implemented("RFS_SET_TEST_SHORT");
                return;
            case RFS_SET_TEST_LONG:
                cdi_not_implemented("RFS_SET_TEST_LONG");
                return;
            case RFS_SET_TEST_GAIN:
                cdi_not_implemented("RFS_SET_TEST_GAIN");
                return;
            case RFS_SET_SCI_1:
                cdi_not_implemented("RFS_SET_SCI_1");
                return;
            case RFS_SET_SCI_2:
                cdi_not_implemented("RFS_SET_SCI_2");
                return;
            case RFS_SET_SCI_3:
                cdi_not_implemented("RFS_SET_SCI_3");
                return;
            case RFS_SET_SCI_4:
                cdi_not_implemented("RFS_SET_SCI_4");
                return;
            case RFS_SET_GAIN_ANA_SET:
                cdi_not_implemented("RFS_SET_GAIN_ANA_SET");
                return;
            case RFS_SET_GAIN_ANA_CFG:
                cdi_not_implemented("RFS_SET_GAIN_ANA_CFG");
                return;
            case RFS_SET_GAIN_DIG_SET:
                cdi_not_implemented("RFS_SET_GAIN_DIG_SET");
                return;
            case RFS_SET_GAIN_DIG_CFG:
                cdi_not_implemented("RFS_SET_GAIN_DIG_CFG");
                return;
            case RFS_SET_ROUTE_SET12:
                cdi_not_implemented("RFS_SET_ROUTE_SET12");
                return;
            case RFS_SET_ROUTE_SET34:
                cdi_not_implemented("RFS_SET_ROUTE_SET34");
                return;
            case RFS_SET_AVG_SET:
                cdi_not_implemented("RFS_SET_AVG_SET");
                return;
            case RFS_SET_AVG_OUTLIER:
                cdi_not_implemented("RFS_SET_AVG_OUTLIER");
                return;
            case RFS_SET_AVG_FREQ:
                cdi_not_implemented("RFS_SET_AVG_FREQ");
                return;
            case RFS_SET_AVG_SET_HI:
                cdi_not_implemented("RFS_SET_AVG_SET_HI");
                return;
            case RFS_SET_AVGI_SET_MID:
                cdi_not_implemented("RFS_SET_AVGI_SET_MID");
                return;
            case RFS_SET_CAL_FRAC_SET:
                cdi_not_implemented("RFS_SET_CAL_FRAC_SET");
                return;
            case RFS_SET_CAL_MAX_SET:
                cdi_not_implemented("RFS_SET_CAL_MAX_SET");
                return;
            case RFS_SET_CAL_LOCK_SET:
                cdi_not_implemented("RFS_SET_CAL_LOCK_SET");
                return;
            case RFS_SET_CAL_SNR_SET:
                cdi_not_implemented("RFS_SET_CAL_SNR_SET");
                return;
            case RFS_SET_CAL_BIN_ST:
                cdi_not_implemented("RFS_SET_CAL_BIN_ST");
                return;
            case RFS_SET_CAL_BIN_EN:
                cdi_not_implemented("RFS_SET_CAL_BIN_EN");
                return;
            case RFS_SET_CAL_ANT_MASK:
                cdi_not_implemented("RFS_SET_CAL_ANT_MASK");
                return;
            case RFS_SET_ZOOM_EN:
                cdi_not_implemented("RFS_SET_ZOOM_EN");
                return;
            case RFS_SET_ZOOM_SET1:
                cdi_not_implemented("RFS_SET_ZOOM_SET1");
                return;
            case RFS_SET_ZOOM_SET1_LO:
                cdi_not_implemented("RFS_SET_ZOOM_SET1_LO");
                return;
            case RFS_SET_ZOOM_SET1_HI:
                cdi_not_implemented("RFS_SET_ZOOM_SET1_HI");
                return;
            case RFS_SET_ZOOM_SET2:
                cdi_not_implemented("RFS_SET_ZOOM_SET2");
                return;
            case RFS_SET_ZOOM_SET2_LO:
                cdi_not_implemented("RFS_SET_ZOOM_SET2_LO");
                return;
            case RFS_SET_ZOOM_SET2_HI:
                cdi_not_implemented("RFS_SET_ZOOM_SET2_HI");
                return;
            case RFS_SET_SEQ_EN:
                cdi_not_implemented("RFS_SET_SEQ_EN");
                return;
            case RFS_SET_SEQ_REP:
                cdi_not_implemented("RFS_SET_SEQ_REP");
                return;
            case RFS_SET_SEQ_CYC:
                cdi_not_implemented("RFS_SET_SEQ_CYC");
                return;
            case RFS_SET_SEQ_STO:
                cdi_not_implemented("RFS_SET_SEQ_STO");
                return;                           
            default:
                cdi_not_implemented("UNRECOGNIZED COMMAND");
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

    core_init_state();

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
                *ddr_ptr += ((*df_ptr) / (1 << state.Navg2_shift));
                df_ptr++;
                ddr_ptr++;
            }
            //printf ("Processing spectrum %i\n", avg_counter);
            avg_counter++;
            spec_clear_df_flag(); // Clear the flag to indicate that we have read the data

            // Check if we have reached filled up Stage 2 averaging
            // and if so, push things out to CDI
            if (avg_counter == state.Navg2)
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