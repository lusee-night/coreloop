#include "core_loop.h"
#include "calibrator_interface.h"
#include "interface_utils.h"
#include "LuSEE_IO.h"
#include <stdlib.h>

const char* cal_zoom_filename = CORELOOP_ROOT "/data/cal_zoom.dat";
int32_t cal_zoom_data[NCHANNELS * 4];
struct timespec time_cal_start;

void calib_init()
{
    // Placeholder implementation
    CAL_DF = malloc(128 * 1024);

    read_array_int(cal_zoom_filename, cal_zoom_data, NCHANNELS * 4);
}

void calib_enable(bool enable)
{
    // Placeholder implementation
}

void calib_set_readout_mode(int mode)
{
    // Placeholder implementation
}

void cal_copy_registers(uint32_t* buffer)
{
    // Placeholder implementation
}

int calib_get_PFB_index()
{
    // Placeholder implementation
    return 0;
}

void calib_set_PFB_index(int index)
{
    // Placeholder implementation
}

//returns true if a new calibration product is ready
void cal_new_cal_ready(bool* modes)
{
    long ns_passed = nanosecs_passed(time_cal_start);
    long to_pass_zoom = 1000000;
    modes[CAL_MODE_ZOOM] = (ns_passed > to_pass_zoom);
}

// assuming a certain mode is ready above, transfer it over
void cal_transfer_data(int mode)
{
    bool add_noise = true;
    if (mode == CAL_MODE_ZOOM) {
        // ignore PFB bin, just populate CAL_DF with data from array
        int32_t* cal_df = CAL_DF;
        for(int i = 0 ; i < NCHANNELS * 4 ; ++i) {
            if (add_noise) {
                double noise = generate_gaussian_variate();
                cal_df[i] = (int32_t)(2 * cal_zoom_data[i] * noise);
            } else {
                cal_df[i] = cal_zoom_data[i];
            }
        }
    }
    // reset time_cal_start
    clock_gettime(CLOCK_REALTIME, &time_cal_start);
}

bool cal_df_dropped()
{
    // Placeholder implementation
    return false;
}

void cal_clear_df_flag()
{
    // Placeholder implementation
}

void calib_set_Navg(int Navg2, int Navg3)
{
    // Placeholder implementation
}

void calib_set_drift_guard(int val)
{
    // Placeholder implementation
}

void calib_set_drift_step(int val)
{
    // Placeholder implementation
}

void calib_set_SNR_lock_on(int val)
{
    // Placeholder implementation
}

void calib_set_SNR_lock_off(int val)
{
    // Placeholder implementation
}

void calib_set_Nsettle(int val)
{
    // Placeholder implementation
}

void calib_set_delta_drift_corA(int val)
{
    // Placeholder implementation
}

void calib_set_delta_drift_corB(int val)
{
    // Placeholder implementation
}

void calib_antenna_mask(int mask)
{
    // Placeholder implementation
}

void calib_zero_weights()
{
    // Placeholder implementation
}

void calib_set_weight(int index, uint16_t value)
{
    // Placeholder implementation
}

void calib_hold_drift(bool hold)
{
}

void calib_set_notch_index(int index)
{
    // Placeholder implementation
}

uint16_t calib_get_weight(int index) { return index; }

void cal_reset() { };

void calib_set_slicers(uint8_t powertop_slice, uint8_t powerbot_slice, uint8_t sum1_slice, \
                        uint8_t sum2_slice, uint8_t fd_slice, uint8_t sd2_slice, uint8_t prod1_slice, uint8_t prod2_slice) { }

uint32_t calib_get_slicer_errors() { return 0; }

uint32_t calib_get_errors() { return 0; }

int calib_get_readout_mode() { }

void calib_set_ddrift_max(int val) { }

void calib_set_gphase_max(int val) { }

void cal_copy_errors(struct calibrator_errors* err) { }