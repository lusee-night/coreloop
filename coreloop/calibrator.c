#include "core_loop.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include <string.h>

#define CAL_MODE0_CHUNKSIZE (1024*sizeof(uint32_t))  
#define CAL_MODE0_DATASIZE (CAL_MODE0_CHUNKSIZE*5+1*sizeof(uint32_t)) // 5 chunks + 1 register
#define CAL_MODE0_PACKETSIZE (CAL_MODE0_CHUNKSIZE*2) 
#define CAL_MODE0_APPID_OFFSET 0

#define CAL_MODE1_CHUNKSIZE (2048*sizeof(uint32_t)) // REAL or IMage Size
#define CAL_MODE1_DATASIZE (CAL_MODE1_CHUNKSIZE*2*4)  // x 4 channels
#define CAL_MODE1_PACKETSIZE (CAL_MODE1_CHUNKSIZE)
#define CAL_MODE1_APPID_OFFSET 3


#define CAL_MODE3_DATASIZE (24*1024*sizeof(uint32_t))
#define CAL_MODE3_PACKETSIZE (3*1024*sizeof(uint32_t)) 
#define CAL_MODE3_APPID_OFFSET 11
uint32_t register_scratch[CAL_NREGS];
bool pckt=false;


void calib_set_mode(uint8_t mode) {
    state.cal.mode = mode;
    // bits 1-2 are readout-mode
    state.cal.readout_mode = (mode & 0b110) >> 1;
    calib_set_readout_mode(state.cal.readout_mode);   
    // bits 3-7 are special modes
    int special_mode = (mode & 0b11111000) >>3;
    switch(special_mode){
        case 1:
            calib_hold_drift(true);
            break;
        default:
            calib_hold_drift(false);
    }
    
    // debug 
    memset((void *) CAL_BUF, 0, CAL_MODE0_DATASIZE);

}


void process_calibrator() {
    if (cal_new_cal_ready()||pckt) {
        
        debug_print("C");
        uint32_t *tst = (uint32_t *)(CAL_BUF+CAL_MODE0_CHUNKSIZE*4);
        debug_print_dec(*tst);
        
        if (state.cdi_dispatch.cal_count <0x20) {
            debug_print("!");
            cal_clear_df_flag();
            return;
        }
        pckt=false;
        
        if (cal_df_dropped()) state.base.errors |= DF_CAL_DROPPED;
        cal_copy_registers(register_scratch);
        // now copy the data based on the mode;
        int size;
        if (state.cal.readout_mode==0)
            size = CAL_MODE0_DATASIZE;
        else if (state.cal.readout_mode==0)
            size = CAL_MODE1_DATASIZE;
        else
            size = CAL_MODE3_DATASIZE;
        memcpy (CAL_DATA, CAL_BUF, size);
        /*uint32_t *src = (uint32_t *)CAL_BUF;
        uint32_t *dst = (uint32_t *)CAL_DATA;
        for (int i=0; i<size/sizeof(uint32_t); i++) *dst++ = *src++;*/
            
        
        cal_clear_df_flag();
        state.cdi_dispatch.cal_count=0;
        new_unique_packet_id();
        state.cdi_dispatch.cal_packet_id = unique_packet_id;
    }

}

void dispatch_calibrator_data() {
    
    update_time();
    uint32_t size, packet_size;
    uint32_t appid_offset=0;
    wait_for_cdi_ready();

    * ((uint32_t *)(TLM_BUF)) = state.cdi_dispatch.cal_packet_id;
    
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
        
        if (state.cal.readout_mode == 0) {
            size = CAL_MODE0_DATASIZE;
            packet_size = CAL_MODE0_PACKETSIZE;
            appid_offset = CAL_MODE0_APPID_OFFSET+state.cdi_dispatch.cal_count;
        } else if (state.cal.readout_mode == 1) {
            size = CAL_MODE1_DATASIZE;
            packet_size = CAL_MODE1_PACKETSIZE;
            appid_offset = CAL_MODE1_APPID_OFFSET+state.cdi_dispatch.cal_count;            
        } else {
            size = CAL_MODE3_DATASIZE;
            packet_size = CAL_MODE3_PACKETSIZE;
            appid_offset = CAL_MODE3_APPID_OFFSET+state.cdi_dispatch.cal_count;
        }

        uint32_t start = (state.cdi_dispatch.cal_count-1)*packet_size;
        if (start+packet_size >= size) {
            packet_size = size-start;
            state.cdi_dispatch.cal_count=0xFE; //we're done (+1 will make it go to 0xFF)
        }
        memcpy ((void *)(TLM_BUF+4), (void *)(CAL_BUF+start), packet_size);
    }
    debug_print("c");
    //debug_print_dec(appid_offset);
    //debug_print_dec(packet_size);
    cdi_dispatch(AppID_Calibrator_MetaData+appid_offset, packet_size+4); // +4 for packet id
}