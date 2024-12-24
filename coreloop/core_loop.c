#include <string.h> // Add include for memcpy
#include <stdlib.h>
#include <stdint.h>
#include "LuSEE_IO.h"
#include "LuSEE_SPI.h"
#include "LuSEE_Flash_cntrl.h"
#include "core_loop.h"
#include "spectrometer_interface.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "flash_interface.h"
// autogenerated include files from documentation
#include "lusee_appIds.h"

// thing that are touched in the interrupt need to be proclaimed volatile

/***************** UNAVOIDABLE GLOBAL STATE ******************/
// flag to tell main we are doing a soft reset
bool soft_reset_flag;
// tap counter increased in the interrupt
volatile uint64_t tap_counter;





void mini_wait (uint32_t ticks) {
    uint64_t val = tap_counter+ticks;
    // since we are in a tight loop and the other thing is running on 100Hz, we should be fine
    // do not want <= sign here since there could be overflow
    #ifdef NOTREAL
    exit(1); // implement this!!
    #endif  
    while (tap_counter!=val) {}
}


void debug_helper(uint8_t arg, struct core_state* state) {
    flash_state_restore(arg, state);
}


void core_init_state(struct core_state* state){
    memset(state, 0, sizeof(struct core_state));
    default_state (&state->base);
    state->base.errors = 0;
    state->cmd_start = state->cmd_end = 0;
    state->base.corr_products_mask=0xFFFF; //65535, everything on
    state->base.spectrometer_enable = false;
    state->base.rand_state = 0xFEEDD0D0;
    spec_set_spectrometer_enable(false);
    state->housekeeping_request = 0;
    state->range_adc = 0;
    for (int i=0; i<NINPUT; i++) state->base.actual_gain[i] = GAIN_MED;
    for (int i=0; i<NSPECTRA; i++) state->base.actual_bitslice[i] = MIN(state->base.bitslice[i],0x1F); // to convert FF to 16
    spec_set_spectrometer_enable(false);
    state->dispatch_delay = DISPATCH_DELAY;
    state->cdi_dispatch.prod_count = 0xFF; // >0F so disabled.
    state->cdi_dispatch.tr_count = 0xFF; // >0F so disabled. 
    state->cdi_dispatch.cal_count = 0xFF; // >0F so disabled.            
    state->tick_tock = true;
    state->base.weight_current = state->base.weight_previous = 0;
    state->drop_df = false;
    state->avg_counter = 0;
    update_time(state);
    state->unique_packet_id = state->base.time_32;
    state->watchdog.FPGA_max_temp = 90;

    set_spectrometer(state);
    tap_counter = 0;
    state->timing.cdi_dispatch_counter = 0;
    state->timing.heartbeat_counter = HEARTBEAT_DELAY;
    state->timing.resettle_counter = 0;
    state->timing.cdi_wait_counter = 0;
    calib_init();
}

bool process_waveform(struct core_state* state) {
    if (!state->request_waveform) return false;
    wait_for_cdi_ready();
    spec_request_waveform(state->request_waveform & 7, 16+state->dispatch_delay*4);
    state->request_waveform = 0;
    return true;
}


void core_loop(struct core_state* state)
{
    soft_reset_flag = false;
    state->request_waveform = 0 ;
    state->range_adc = 0;
    spec_set_spectrometer_enable(false);
    spec_clear_df_flag();
    // now empty the CDI command buffer in case we are doing the reset.
    #ifndef NOTREAL
    uint8_t tmp;
    while (cdi_new_command(&tmp, &tmp, &tmp)) {};
    #endif

    send_hello_packet(state);
    core_init_state(state);
    #ifndef NOTREAL
    // restore state from flash if unscheduled reset occured
    restore_state(state);
    #endif

    for (;;)
    {
        
        // Check if we have a new CDI command and process it.
        // If this functions returns true, it means we got the time-to-die command
        if (process_cdi(state)) break;
        process_watchdogs(state);
        process_spectrometer(state);
        process_calibrator(state);
        process_gain_range(state);
        // we always process just one CDI interfacing things
        if (cdi_ready()) {
            process_hearbeat(state) | process_delayed_cdi_dispatch(state) | process_housekeeping(state) | process_waveform(state);
        }

#ifdef NOTREAL
        // if we are running inside the coreloop test harness we call the interrupt routine
        // every 100ms; 
        uint8_t  MSYS_EI5_IRQHandler();
        clock_gettime(CLOCK_REALTIME, &time_now);
        // Calculate the elapsed time in milliseconds
        long elapsed_ms = (time_now.tv_sec - time_start.tv_sec) * 1000 + (time_now.tv_nsec - time_start.tv_nsec) / 1000000;
        long elapsed_ticks = elapsed_ms/10; // 10Hz.
        if (elapsed_ticks != g_core_timer_0) {
            MSYS_EI5_IRQHandler();
            g_core_timer_0 = elapsed_ticks;
        }
#endif
    }
}

uint8_t MSYS_EI5_IRQHandler(void)
{
    tap_counter++;
    TMR_clear_int(&g_core_timer_0);
    return (EXT_IRQ_KEEP_ENABLED);
}


void update_time(struct core_state* state) {
    // why is this not working is not clear.
    //spec_get_time(&state.base.time_seconds, &state.base.time_subseconds);
    uint32_t sec32;
    uint16_t sec16;
    spec_get_time(&sec32, &sec16);
    state->base.time_32 = sec32;
    state->base.time_16 = sec16;
    state->base.rand_state += sec32;
    state->base.uC_time = tap_counter;
}

// return batch size for stage 1 averaging
uint16_t get_Navg1(struct core_state *s)
{
    return 1 << s->base.Navg1_shift;
}

// return batch size for stage 2 averaging (to TICK/TOCK buffer)
uint16_t get_Navg2(struct core_state *s)
{
    return 1 << s->base.Navg2_shift;
}

// return number of frequencies in the outgoing spectra
uint16_t get_Nfreq(struct core_state *s)
{
    switch(s->base.Navgf) {
        case 1: return NCHANNELS;
        case 2: return NCHANNELS/2;
        case 3: return NCHANNELS/4;
        case 4: return NCHANNELS/4;
        default: return NCHANNELS;
    }
}

// return number of frequencies to average in time-resolved spectra
uint16_t get_tr_avg(struct core_state *s)
{
    return 1 << s->base.tr_avg_shift;
}

// return max gain (computed as min gain times multiplication factor)
uint16_t get_gain_auto_max(struct core_state *s, int i)
{
        return s->base.gain_auto_min[i] * s->base.gain_auto_mult[i];
}

// for symmetry: return min gain stored in seq struct
uint16_t get_gain_auto_min(struct core_state *s, int i)
{
        return s->base.gain_auto_min[i];
}

// return length (i.e., number of int_16_t, not bytes)
// of single
uint32_t get_tr_length(struct core_state *s)
{
    return (s->base.tr_stop - s->base.tr_start) >> s->base.tr_avg_shift;
}


void reset_errormasks(struct core_state* state) {
    state->base.errors = 0;
    state->base.spec_overflow = 0;
    state->base.notch_overflow = 0;
}


void RFS_stop(struct core_state* state) {
    debug_print ("\n\rStopping spectrometer\n\r");
    state->base.spectrometer_enable = false;
    spec_set_spectrometer_enable(false);
}


void RFS_start(struct core_state* state) {
    debug_print ("\n\rStarting spectrometer\n\r");
    state->base.spectrometer_enable = true;
    state->base.weight_previous = state->base.weight_current = 0;
    state->avg_counter = 0;
    memset((void *)SPEC_TICK, 0, NSPECTRA*NCHANNELS * sizeof(uint32_t));
    memset((void *)SPEC_TOCK, 0, NSPECTRA*NCHANNELS * sizeof(uint32_t));
    set_spectrometer(state);
    spec_set_spectrometer_enable(true);
    //drop_df = true;
}


void restart_spectrometer(struct core_state* state)
{
    RFS_stop(state);
    RFS_start(state);
}

void trigger_ADC_stat() {
    spec_trigger_ADC_stat(ADC_STAT_SAMPLES);
}





