#include "core_loop.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include <string.h>

#define CAL_MODE0_DATASIZE 20484
#define CAL_MODE1_CHUNKSIZE (1024*sizeof(uint32_t)*2)
#define CAL_MODE1_DATASIZE (CAL_MODE1_CHUNKSIZE*4)
#define CAL_MODE3_DATASIZE (24*1024*sizeof(uint32_t))
#define CAL_PACKET_OUT_SIZE (4096)

uint32_t register_scratch[CAL_NREGS];



void calib_set_mode(uint8_t mode) {
    state.cal.mode = mode;
    // bits 0-1 are readout-mode
    state.cal.readout_mode = mode & 0x03;
    calib_set_readout_mode(state.cal.readout_mode);    
}


void process_calibrator() {
    if (cal_new_cal_ready()) {
        debug_print("C");
        if (cal_df_dropped()) state.base.errors |= DF_CAL_DROPPED;
        cal_copy_registers(register_scratch);
        // now copy the data based on the mode;
        int size;
        switch (state.cal.readout_mode) {
            case 0: 
                memcpy ((void *) CAL_BUF, (void *)CAL_DATA,CAL_MODE0_DATASIZE);
                break;
            case 1: 
        
                // this looks wrong, but it is actually corret as per documents
                // there are large gaps
                for (int i = 0; i<4; i++) {
                    memcpy ((void *)(CAL_BUF + 2*i*CAL_MODE1_CHUNKSIZE), 
                            (void *)(CAL_DATA + i*CAL_MODE1_CHUNKSIZE), CAL_MODE1_CHUNKSIZE);
                }
                break;
            case 2:  break; // not implemented 
            case 3: 
                // let's copy everything for now
                size  = 24*1024*sizeof(uint32_t);
                memcpy ((void*) CAL_BUF, (void *)CAL_DATA, size);                
                break;
        }
        cal_clear_df_flag();
        state.cdi_dispatch.cal_count=0;
    }

}

void dispatch_calibrator_data() {
    new_unique_packet_id();
    update_time();
    uint32_t packet_size;
    wait_for_cdi_ready();

    * ((uint32_t *)(TLM_BUF)) = unique_packet_id;
    if (state.cdi_dispatch.cal_count == 0) {
        uint32_t* ptr  = (uint32_t *)(TLM_BUF+4); // first 4 bytes are packet id
        *ptr = state.base.time_32; ptr++;
        *ptr = state.base.time_16; ptr++;
        for (int i=0; i<CAL_NREGS; i++) {
            *ptr = register_scratch[i];
            ptr++;
        }
        packet_size = (3+CAL_NREGS)*sizeof(uint32_t);
    } else {
        uint32_t size;
        if (state.cal.readout_mode == 0) {
            size = CAL_MODE0_DATASIZE;
        } else if (state.cal.readout_mode == 1) {
            size = CAL_MODE1_DATASIZE;
        } else {
            size = CAL_MODE3_DATASIZE;
        }

        uint32_t start = (state.cdi_dispatch.cal_count-1)*CAL_PACKET_OUT_SIZE;
        if (start+CAL_PACKET_OUT_SIZE <= size) {
            packet_size = size-start;
            state.cdi_dispatch.cal_count=0xFF; //we're done
        } else {
            packet_size = CAL_PACKET_OUT_SIZE;
        }
        memcpy ((void *)(TLM_BUF+4), (void *)(CAL_BUF+start), packet_size);
        packet_size += 4; // account for unique packet id
    }
    debug_print("c");
    cdi_dispatch(AppID_Calibrator_Data+state.cdi_dispatch.cal_count, packet_size);
}