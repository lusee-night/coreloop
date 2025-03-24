#ifndef CORE_LOOP_H
#define CORE_LOOP_H
#pragma pack(1)

// This 16 bit version ID goes with metadata and startup packets.
// MSB is code version, LSB is metatada version
#define VERSION_ID 0x00000300


#include <inttypes.h>
#include <stddef.h>
#include "spectrometer_interface.h"
#include "calibrator.h"
#include "core_loop_errors.h"



// Constants
#define DISPATCH_DELAY 6 // number of timer interrupts to wait before sending CDI
#define RESETTLE_DELAY 5 // number of timer interrupts to wait before settling after a change
#define HEARTBEAT_DELAY 1024 // number of timer interrupts to wait before sending heartbeat
#define CMD_BUFFER_SIZE 512 // size of command buffer for 0x10 commands
#define MAX_LOOPS 4   // how many nested loops we can do


#define ADC_STAT_SAMPLES 16000

#define MAX_STATE_SLOTS 16


/***************** UNAVOIDABLE GLOBAL STATE ******************/
// flag to tell main we are doing a soft reset
extern bool soft_reset_flag;
// tap counter increased in the interrupt
extern volatile uint64_t tap_counter;
// TVS sensors averaged in timer interrupt
extern volatile uint32_t TVS_sensors_avg[4];
// loop count in timer interrupt
extern volatile uint16_t loop_count_min, loop_count_max;


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
    OUTPUT_16BIT_10_PLUS_6,
    OUTPUT_16BIT_4_TO_5,
    OUTPUT_16BIT_SHARED_LZ
};

struct route_state {
    uint8_t plus, minus;  // we route "plus" - "minus". if minus is FF, it is ground;
};

struct time_counters {
    uint64_t heartbeat_counter;
    uint64_t resettle_counter;
    uint64_t cdi_wait_counter;
    uint64_t cdi_dispatch_counter;
};


// core state base contains the crucial state that is dumped with every metadata packet

struct core_state_base {
    uint64_t uC_time;
    uint32_t time_32;
    uint16_t time_16;    
    uint16_t TVS_sensors[4]; // temperature and voltage sensors, registers 1.0V, 1.8V, 2.5V and Temp
    uint16_t loop_count_min, loop_count_max;
    // former sequencer state starts here
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
    // former sequencer state ends here

    uint32_t errors;
    uint16_t corr_products_mask; // which of 16 products to be used, starting with LSB
    uint8_t actual_gain[NINPUT]; // this defines the actual gain state (can only be low, med, high);
    uint8_t actual_bitslice[NSPECTRA];
    uint16_t spec_overflow;  // mean specta overflow mask
    uint16_t notch_overflow; // notch filter overflow mask
    struct ADC_stat ADC_stat[4];    
    bool spectrometer_enable; // spectrometer_enable is true when FFT enegine is running
    bool calibrator_enable; // calibrator enable is true will enable calibrator with enabling the FFT engine.
    uint32_t rand_state;
    uint8_t weight_previous, weight_current;
};

struct cdi_stats {
    uint32_t cdi_total_command_count;
    uint32_t cdi_packets_sent;
    uint64_t cdi_bytes_sent;
};

struct delayed_cdi_sending {
    uint32_t appId;
    uint32_t tr_appId;
    uint16_t int_counter; // counter that will be decremented every timer interrupt
    uint8_t format;
    uint8_t prod_count; // product ID that needs to be sent
    uint8_t tr_count; // time-resolved packet number that needs to be sent
    uint8_t cal_count; // number of calibrator packets that need to be sent;
    uint16_t Nfreq; // number of frequencies that actually need to be sent
    uint16_t Navgf; // frequency averaging factor
    uint32_t packet_id;
    uint32_t cal_packet_id;
    uint32_t cal_appId;
    uint32_t cal_size, cal_packet_size;
};


struct watchdog_config {
    uint8_t FPGA_max_temp;
    // here add watchdog configuration if needed
};

// core state cointains the seuqencer state and the base state and a number of utility variables
struct core_state {
    struct core_state_base base;
    struct cdi_stats cdi_stats;
    struct calibrator_state cal;
    // A number be utility values 
    struct delayed_cdi_sending cdi_dispatch;
    struct time_counters timing;
    struct watchdog_config watchdog;
    uint16_t cdi_wait_spectra;
    uint16_t avg_counter;
    uint32_t unique_packet_id;
    uint8_t leading_zeros_min[NSPECTRA];
    uint8_t leading_zeros_max[NSPECTRA];
    uint8_t housekeeping_request;
    uint8_t range_adc, resettle, request_waveform, request_eos; 
    bool tick_tock;
    bool drop_df;
    uint32_t heartbeat_packet_count;
    int8_t flash_slot;
    uint8_t cmd_arg_high[CMD_BUFFER_SIZE], cmd_arg_low[CMD_BUFFER_SIZE];
    // pointeres to the beginning and end of commands, also used during sequence upload
    uint16_t cmd_ptr, cmd_end;
    // are we uploading -- if so block sequences
    bool sequence_upload;
    // for loops
    uint8_t loop_depth; // how many nested loops
    uint16_t loop_start[MAX_LOOPS];
    uint8_t loop_count[MAX_LOOPS];


    uint32_t cmd_counter;
    uint16_t dispatch_delay; // number of timer interrupts to wait before sending CDI
    uint16_t reg_address; // address of the register to be written (for commands that do that)
    int32_t reg_value; // value to be written to the register
};

struct saved_state {
    uint32_t in_use;
    uint8_t cmd_arg_high[CMD_BUFFER_SIZE], cmd_arg_low[CMD_BUFFER_SIZE];
    uint16_t cmd_ptr, cmd_end;
    uint32_t CRC;
};

struct state_recover_notification {
    uint32_t slot;
    uint32_t size;
};

struct end_of_sequence {
    uint32_t unique_packet_id;
    uint32_t eos_arg;
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
    uint16_t TVS_sensors[4];
    uint16_t loop_count_min, loop_count_max;
    struct cdi_stats cdi_stats;
    uint32_t errors;
    char magic[6];
};

// metadata payload, compatible with core_state
struct meta_data {
    uint16_t version; 
    uint32_t unique_packet_id;
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




//extern struct core_state state;
extern bool soft_reset_flag;

// main function
void core_loop(struct core_state*);

// process a CDI command
bool process_cdi(struct core_state*);

// process watchdogs and temperature alarms
void process_watchdogs (struct core_state*);

// starts / stops / restarts the spectrometer
void RFS_stop(struct core_state*);
void RFS_start(struct core_state*);
void restart_spectrometer(struct core_state*);

// derived quantities in the state
uint16_t get_Navg1(struct core_state *s);
uint16_t get_Navg2(struct core_state *s);
uint16_t get_Nfreq(struct core_state *s);
uint16_t get_tr_avg(struct core_state *s);
uint16_t get_gain_auto_max(struct core_state *s, int i);
uint32_t get_tr_length(struct core_state *s);


// set routing for a channel
void set_route(struct core_state* state, uint8_t ch, uint8_t arg_low);

// update spectrometer gain to match those in state.
void update_spec_gains(struct core_state*);

// get ADC samples
void trigger_ADC_stat();

// reset errormask
void reset_errormasks(struct core_state*);

// update times in global state
void update_time(struct core_state*);

// see if new spectra are ready
void process_spectrometer(struct core_state*);

// transfer data to CDI if needed
void transfer_to_cdi(struct core_state*);
// process delayed CDI dispatch
bool process_delayed_cdi_dispatch(struct core_state*);

// automatic control for bit-slicing.
void process_gain_range(struct core_state*);
bool bitslice_control(struct core_state*);

// settings control
void set_spectrometer(struct core_state*);
void default_state(struct core_state_base *);

// debuggin functions
void debug_helper(uint8_t arg, struct core_state*);
void cdi_not_implemented(const char *msg);


// housekeeping functions
void send_hello_packet(struct core_state* state);
bool process_hearbeat(struct core_state*);
bool process_housekeeping(struct core_state*);

// create end-of-sequence packet
bool process_eos(struct core_state*); 

// cdi dispatch with counting
void cdi_dispatch_uC (struct cdi_stats* cdi_stats, uint16_t appID, uint32_t length);

//delayed dispatch;
bool delayed_cdi_dispatch_done (struct core_state*);

// calibrator functions
void calibrator_default_state (struct calibrator_state* cal);
void calib_set_mode (struct core_state* state, uint8_t mode);
void process_calibrator(struct core_state* state);
void dispatch_calibrator_data(struct core_state* state);

// Update random stae in state.base.rand_state
inline static void update_random_state(struct core_state* s) {s->base.rand_state = 1103515245 * s->base.rand_state + 12345;}
inline static void new_unique_packet_id(struct core_state* s) {s->unique_packet_id++;}

// utility functions
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define IS_NEG(x) (((x) < 0) ? 1 : 0)
void mini_wait (uint32_t ticks);


// starts a timer
void timer_start();
// returns the number of timer ticks starting from timer_start()
// each timer tick is 1/4096 of a second ~244us
uint32_t timer_stop();


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

// encode/decode 4 int32_t values into 5 int16_t values: first one for shift info
void encode_4_into_5(const int32_t* const vals_in, uint16_t* vals_out);
void decode_5_into_4(const int16_t* const vals_in, int32_t* vals_out);

// CRC
uint32_t CRC(const void* data, size_t size);

// fft
void fft_precompute_tables();
void fft(uint32_t *real, uint32_t *imag);

#endif // CORE_LOOP_H
