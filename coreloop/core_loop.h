#ifndef CORE_LOOP_H
#define CORE_LOOP_H
#pragma pack(1)


#define VERSIO1N 0.1-DEV
// This 16 bit version ID goes with metadata and startup packets.
// MSB is code version, LSB is metatada version
#define VERSION_ID 0x00000105


#include <inttypes.h>
#include <stddef.h>
#include "spectrometer_interface.h"
#include "core_loop_errors.h"



// Constants
#define NSEQ_MAX 32
#define DISPATCH_DELAY 6 // number of timer interrupts to wait before sending CDI
#define RESETTLE_DELAY 2 // number of timer interrupts to wait before settling after a change
#define HEARTBEAT_DELAY 1024 // number of timer interrupts to wait before sending heartbeat

#define ADC_STAT_SAMPLES 16000

#define MAX_STATE_SLOTS 64
//consistent with 4k erases
#define PAGES_PER_SLOT 256

// global variables, will need to fix
extern struct core_state state;
extern uint16_t avg_counter;
extern uint32_t unique_packet_id;
extern uint8_t leading_zeros_min[NSPECTRA];
extern uint8_t leading_zeros_max[NSPECTRA];
extern uint8_t housekeeping_request;
extern uint32_t section_break;
extern uint8_t range_adc, resettle, request_waveform; 
extern bool tick_tock;
extern bool drop_df;
extern bool soft_reset_flag;
extern uint32_t heartbeat_packet_count;
extern volatile uint32_t heartbeat_counter;
extern volatile uint32_t resettle_counter;
extern volatile uint32_t cdi_wait_counter; 
extern volatile uint32_t mini_wait_counter;
extern uint16_t flash_store_pointer;



// note that gain auto is missing here, since these are actual spectrometer set gains
enum gain_state{
    GAIN_LOW,
    GAIN_MED,
    GAIN_HIGH,
    GAIN_DISABLE,
    GAIN_AUTO};

enum output_format {
    OUTPUT_32BIT,
    OUTPUT_16BIT_UPDATES,
    OUTPUT_16BIT_FLOAT1,
};

struct route_state {
    uint8_t plus, minus;  // we route "plus" - "minus". if minus is FF, it is ground;
};


// sequencer state describes the information needed to set the spectrometer to a given state
struct sequencer_state {
    uint8_t gain [NINPUT]; // this defines the commanded gain state (can be auto)
    uint16_t gain_auto_min[NINPUT];   
    uint16_t gain_auto_mult[NINPUT];
    struct route_state route[NINPUT];
    uint8_t Navg1_shift, Navg2_shift;   // Stage1 (FW) and Stage2 (uC) averaging
    uint8_t notch; // 0 = disable, 1 = x4, 2 = x16, 3=x64, 4=x256
    uint8_t Navgf; // frequency averaging
    uint8_t hi_frac, med_frac;
    uint8_t bitslice[NSPECTRA]; // for spectra 0x1F is all MSB, 0xFF is auto
    uint8_t bitslice_keep_bits; // how many bits to keep for smallest spectra
    uint8_t format; // output format to save data in
    uint8_t reject_ratio; // how far we should be to reject stuff, zero to remove rejection
    uint8_t reject_maxbad; // how many need to be bad to reject.
    uint16_t tr_start, tr_stop, tr_avg_shift; // time resolved start, stop and averaging
};


struct sequencer_program {
    uint8_t Nseq; // Number of sequencer steps in a cycle (See RFS_SET_SEQ_CYC)
    struct sequencer_state seq[NSEQ_MAX]; // sequencer states
    uint16_t seq_times[NSEQ_MAX]; // steps in each sequencer state;
    uint16_t sequencer_repeat; // number of sequencer repeats, 00 for infinite 
};


// core state base contains additional information that will be dumped with every metadata packet
struct core_state_base {
    uint32_t time_32;
    uint16_t time_16;
    uint16_t TVS_sensors[4]; // temperature and voltage sensors, registers 1.0V, 1.8V, 2.5V and Temp
    uint32_t errors;
    uint16_t corr_products_mask; // which of 16 products to be used, starting with LSB
    uint8_t actual_gain[NINPUT]; // this defines the actual gain state (can only be low, med, high);
    uint8_t actual_bitslice[NSPECTRA];
    uint16_t spec_overflow;  // mean specta overflow mask
    uint16_t notch_overflow; // notch filter overflow mask
    struct ADC_stat ADC_stat[4];    
    bool spectrometer_enable;
    uint8_t sequencer_counter; // number of total cycles in the sequencer (up to sequencer_repeat)
    uint8_t sequencer_step; // 0xFF is sequencer is disabled (up to Nseq)
    uint8_t sequencer_substep; // counting seq_times (up to seq_times[i])
    uint32_t rand_state;
    uint8_t weight_previous, weight_current;
};



struct delayed_cdi_sending {
    uint32_t appId;
    uint32_t tr_appId;
    uint16_t int_counter; // counter that will be decremented every timer interrupt
    uint8_t format;
    uint8_t prod_count; // product ID that needs to be sent
    uint8_t tr_count; // time-resolved packet number that needs to be sent
    uint16_t Nfreq; // number of frequencies that actually need to be sent
    uint16_t Navgf; // frequency averaging factor
    uint32_t packet_id;

};

// core state cointains the seuqencer state and the base state and a number of utility variables
struct core_state {
    struct sequencer_state seq;
    struct core_state_base base;
    // A number be utility values 
    struct delayed_cdi_sending cdi_dispatch;
    bool sequencer_enabled;
    struct sequencer_program program;
};

struct saved_core_state {
    uint32_t in_use;
    struct core_state state;
    uint32_t CRC;
};

struct startup_hello {
    uint32_t SW_version;
    uint32_t FW_Version;
    uint32_t FW_ID;
    uint32_t FW_Date;
    uint32_t FW_Time;
    uint32_t unique_packet_id;
    uint32_t time_32;
    uint16_t time_16;
};

struct heartbeat {
    uint32_t packet_count;
    uint32_t time_32;
    uint16_t time_16;
    char magic[6];
};

// metadata payload, compatible with core_state
struct meta_data {
    uint16_t version; 
    uint32_t unique_packet_id;
    struct sequencer_state seq;
    struct core_state_base base;
};

struct housekeeping_data_base {
    uint16_t version; 
    uint32_t unique_packet_id;
    uint32_t errors;
    uint16_t housekeeping_type;
};

struct housekeeping_data_0 {
    struct housekeeping_data_base base;
    struct core_state core_state;
};

struct housekeeping_data_1 {
    struct housekeeping_data_base base;
    struct ADC_stat ADC_stat[NINPUT];
    uint8_t actual_gain[NINPUT];
};

struct housekeeping_data_99 {
    uint32_t section_break;
};



extern struct core_state state;
extern bool soft_reset_flag;

// main function
void core_loop();

// process a CDI command
bool process_cdi();
//



// starts / stops / restarts the spectrometer
void RFS_stop();
void RFS_start();
void restart_spectrometer();

// derived quantities in the state
uint16_t get_Navg1(struct core_state *s);
uint16_t get_Navg2(struct core_state *s);
uint16_t get_Nfreq(struct core_state *s);
uint16_t get_tr_avg(struct core_state *s);
uint16_t get_gain_auto_max(struct core_state *s, int i);
uint32_t get_tr_length(struct core_state *s);


// set routing for a channel
void set_route (uint8_t ch, uint8_t arg_low);

// update spectrometer gain to match those in state.
void update_spec_gains();

// get ADC samples
void trigger_ADC_stat();

// reset errormask
void reset_errormasks();

// update times in global state
void update_time();

// see if new spectra are ready
void process_spectrometer();

// transfer data to CDI if needed
void transfer_to_cdi ();
// process delayed CDI dispatch
bool process_delayed_cdi_dispatch();

// automatic control for bit-slicing.
void process_gain_range();
bool bitslice_control();

// sequencer control
void set_spectrometer_to_sequencer();
void default_seq (struct sequencer_state *seq);
void advance_sequencer();

// debuggin functions
void debug_helper(uint8_t arg);
void cdi_not_implemented(const char *msg);


// housekeeping functions
void send_hello_packet();
bool process_hearbeat();
bool process_housekeeping();

// Update random stae in state.base.rand_state
inline static void update_random_state() {state.base.rand_state = 1103515245 * state.base.rand_state + 12345;}

inline static void new_unique_packet_id() {unique_packet_id++;}

// utility functions
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define IS_NEG(x) (((x) < 0) ? 1 : 0)
void mini_wait (uint32_t ticks);


// encode 32 bit value in 16 bits with 12 bits of data and 4 bits of mantissa
uint16_t encode_12plus4(int32_t val);
 int32_t decode_12plus4(uint16_t val);

// encode 32 bit value in 16 bits with 10 bits of data and 5 bits of mantissa
uint16_t encode_10plus6(int32_t val);
 int32_t decode_10plus6(uint16_t val);

// for first 4 auto spectra which are guaranteed to be positive: encode into uint16 with shared number of leading zeros
// format of encoded data: sequence of segments of the form: lz (1 byte) n (1 byte) a_1 ... a_n (all uint16_t)
// return number of bytes written
// TODO: remove size and always assume NCHANNELS?
int  encode_shared_lz_positive(const uint32_t* spectra, unsigned char* cdi_ptr, int size);
void decode_shared_lz_positive(const unsigned char* data_buf, uint32_t* x, int size);

// remaining spectra can be negative: encode into uint16 with shared number of leading zeros
// format of encoded data: sequence of segments of the form: neg_flag_lz (1 byte) n (1 byte) a_1 ... a_n (all uint16_t)
// MSB of neg_flag_lz is set, if values are negative; the last 5 bits of neg_flag_lz store number of leading zeros
// return number of bytes written
int  encode_shared_lz_signed(const int32_t* spectra, unsigned char* cdi_ptr, int size);
void decode_shared_lz_signed(const unsigned char* data_buf, int32_t* x, int size);

// CRC
uint32_t CRC(const void* data, size_t size);

#endif // CORE_LOOP_H
