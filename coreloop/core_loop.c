#include <string.h> // Add include for memcpy
#include <stdlib.h>
#include <stdint.h>
#include "LuSEE_IO.h"
#include "LuSEE_SPI.h"
#include "LuSEE_Flash_cntrl.h"
#include "core_loop.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "flash_interface.h"
// autogenerated include files from documentation
#include "lusee_appIds.h"


/***************** GLOBAL STATE ******************/

struct core_state state;
uint16_t avg_counter = 0;
uint32_t unique_packet_id;
uint8_t leading_zeros_min[NSPECTRA];
uint8_t leading_zeros_max[NSPECTRA];
uint8_t housekeeping_request;
uint32_t section_break;
uint8_t range_adc, resettle, request_waveform;
bool tick_tock;
bool drop_df;
bool soft_reset_flag;

uint32_t heartbeat_packet_count;

// thing that are touched in the interrupt need to be proclaimed volatile
volatile uint32_t heartbeat_counter;
volatile uint32_t resettle_counter;
uint16_t flash_store_pointer;



void debug_helper(uint8_t arg) {
    flash_state_restore(arg);
}


void core_init_state(){
    default_seq (&state.seq);
    state.base.errors = 0;
    state.base.corr_products_mask=0b1111111111111111; //65535
    state.base.spectrometer_enable = false;
    state.base.rand_state = 0xFEEDD0D0;
    spec_set_spectrometer_enable(false);
    housekeeping_request = 0;
    range_adc = 0;
    for (int i=0; i<NINPUT; i++) state.base.actual_gain[i] = GAIN_MED;
    for (int i=0; i<NSPECTRA; i++) state.base.actual_bitslice[i] = MIN(state.seq.bitslice[i],0x1F); // to convert FF to 16
    spec_set_spectrometer_enable(false);
    state.base.sequencer_step = 0xFF;
    state.sequencer_enabled = false;
    state.program.Nseq = 0;
    state.cdi_dispatch.prod_count = 0xFF; // >0F so disabled.
    tick_tock = true;
    state.base.weight_current = state.base.weight_previous = 0;
    drop_df = false;
    update_time();
    unique_packet_id = state.base.time_32;

    set_spectrometer_to_sequencer();
    heartbeat_counter = HEARTBEAT_DELAY;
}

bool process_waveform() {
    if (!request_waveform) return false;
    wait_for_cdi_ready();
    spec_request_waveform(request_waveform & 7);
    request_waveform = 0;
    return true;
}


void core_loop()
{
    soft_reset_flag = false;
    request_waveform = 0 ;
    range_adc = 0;
    section_break = 0;
    flash_clear = 0;
    flash_size = 0;
    flash_write = 0;
    flash_wait = 0;
    send_hello_packet();
    core_init_state();
    #ifndef NOTREAL
    // restore state from flash if unscheduled reset occured
    restore_state();
    #endif

    for (;;)
    {
        update_time();
        // Check if we have a new CDI command and process it.
        // If this functions returns true, it means we got the time-to-die command
        if (process_cdi()) break;
        process_spectrometer();
        process_gain_range();
        // we always process just one CDI interfacing things
        process_hearbeat() | process_delayed_cdi_dispatch() | process_housekeeping() | process_waveform();

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

    uint32_t tocpy;
    /* Clear the interrupt within the timer */
    if (resettle_counter > 0) resettle_counter--;
    if (state.cdi_dispatch.int_counter > 0) state.cdi_dispatch.int_counter--;
    if (heartbeat_counter > 0) heartbeat_counter--;

    #ifndef NOTREAL
     // flash processing.
    if (flash_wait>0) {
        flash_wait --;
    } else if (flash_clear>0) {
        /*debug_print("flash clear ")
        debug_print_dec(flash_clear);
        debug_print("\r\n");*/
        switch (flash_clear) {
            case 3:
                SPI_4k_erase_step1(flash_addr);
                break;
            case 2:
                SPI_4k_erase_step2();
                break;
            case 1:
                SPI_4k_erase_step3();
                break;
        }
        flash_clear --;
        flash_wait = 1;
    } else if (flash_size>0) {
        // after we cleared we can write if needed
      if (flash_write == 0) flash_write = 4;
      /*debug_print("flash write ")
      debug_print_dec(flash_write);
      debug_print (" ");
      debug_print_dec(flash_size);
      debug_print (" ");
      debug_print_hex(flash_addr);
      debug_print (" ");
      uint32_t *v = (uint32_t *)flash_buf;
      debug_print_hex(*v);
      debug_print("\r\n");*/
      switch (flash_write) {
          case 4:
              //int tocpy;
              tocpy = (flash_size<=256) ? flash_size : 256;
              //print_buf(flash_buf,50);
              //memcpy (SFL_WR_BUFF, flash_buf, tocpy);
              uint8_t *src = (uint8_t*) flash_buf;
              uint8_t *tgt = (uint8_t*) SFL_WR_BUFF;
              for (int i=0; i<tocpy; i++) tgt[i]=src[i];
              //print_buf(SFL_WR_BUFF,50);
              break;
          case 3:
              //debug_print (" ");
              ///uint32_t *v = (uint32_t *)SFL_WR_BUFF;
              //debug_print_hex(*v);
              //debug_print("\r\n");
              SPI_write_page_step1(flash_addr);
              break;
          case 2:
              SPI_write_page_step2();
              break;
          case 1:
              SPI_write_page_step3();
              if (flash_size>256) flash_size-=256; else flash_size=0;
              flash_addr+=256;
              flash_buf+=256;
              break;
      }
      flash_write --;
      flash_wait = 1;
  }
    #endif

    TMR_clear_int(&g_core_timer_0);
    return (EXT_IRQ_KEEP_ENABLED);
}


void update_time() {
    // why is this not working is not clear.
    //spec_get_time(&state.base.time_seconds, &state.base.time_subseconds);
    uint32_t sec32;
    uint16_t sec16;
    spec_get_time(&sec32, &sec16);
    state.base.time_32 = sec32;
    state.base.time_16 = sec16;
    state.base.rand_state += sec32;
}


void reset_errormasks() {
    state.base.errors = 0;
    state.base.spec_overflow = 0;
    state.base.notch_overflow = 0;
}


void RFS_stop() {
    debug_print ("\n\rStopping spectrometer\n\r");
    state.base.spectrometer_enable = false;
    spec_set_spectrometer_enable(false);
}


void RFS_start() {
    debug_print ("\n\rStarting spectrometer\n\r");
    state.base.spectrometer_enable = true;
    avg_counter = 0;
    memset((void *)SPEC_TICK, 0, NSPECTRA*NCHANNELS * sizeof(uint32_t));
    memset((void *)SPEC_TOCK, 0, NSPECTRA*NCHANNELS * sizeof(uint32_t));
    if (state.sequencer_enabled) {
        state.base.sequencer_counter = 0;
        state.base.sequencer_step = 0;
        state.base.sequencer_substep = state.program.seq_times[0];
        state.seq = state.program.seq[0];
    }
    set_spectrometer_to_sequencer();
    spec_set_spectrometer_enable(true);
    //drop_df = true;
}


void restart_spectrometer()
{
    RFS_stop();
    RFS_start();
}

void trigger_ADC_stat() {
    spec_trigger_ADC_stat(ADC_STAT_SAMPLES);
}

uint16_t get_Navg1(struct core_state s)
{
    return 1 << s.seq.Navg1_shift;
}

uint16_t get_Navg2(struct core_state s)
{
    return 1 << s.seq.Navg2_shift;
}

uint16_t get_Nfreq(struct core_state s)
{
    switch(s.seq.Navgf) {
        case 1: return NCHANNELS;
        case 2: return NCHANNELS/2;
        case 3: return NCHANNELS/4;
        case 4: return NCHANNELS/4;
        default: return NCHANNELS;
    }
}

uint16_t get_tr_avg(struct core_state s)
{
    return 1 << s.seq.tr_avg_shift;
}

uint16_t get_gain_auto_max(struct core_state s, int i)
{
        return s.seq.gain_auto_min[i] * s.seq.gain_auto_mult[i];
}

