#include "core_loop.h"
#include "calibrator_interface.h"
#include "interface_utils.h"
#include "LuSEE_IO.h"
#include <stdlib.h>

const char* cal_zoom_filename = CORELOOP_ROOT "/data/cal_zoom.dat";
int32_t cal_zoom_data[NCHANNELS * 4];
struct timespec time_cal_start;
bool calibrator_enable = false;

void calib_pre_init()
{
    CAL_DF = malloc(128 * 1024);
    read_array_int(cal_zoom_filename, cal_zoom_data, NCHANNELS * 4);
}

void calib_init()
{
    clock_gettime(CLOCK_REALTIME, &time_cal_start);
}

void calib_enable(bool enable)
{
    calibrator_enable = enable;
    if (enable)
        clock_gettime(CLOCK_REALTIME, &time_cal_start);
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
    modes[2] = (ns_passed > to_pass_zoom);
}

// assuming a certain mode is ready above, transfer it over
void cal_transfer_data(int mode)
{
    if (mode == 2) {
        int32_t* cal_df = CAL_DF;
        double x;
        double dx = 2.0 * M_PI / (NCHANNELS - 1);

        for (int i = 0; i < NCHANNELS; i++) {
            x = i * dx;

            // ch1_re: 10000 * (3 * sin(x) - 4 * cos(3*x))
            cal_df[i] = (int32_t)(10000.0 * (3.0 * sin(x) - 4.0 * cos(30.0 * x)));

            // ch1_im: 10000 * (2 * cos(x) + 3 * sin(2*x))
            cal_df[NCHANNELS + i] = (int32_t)(10000.0 * (2.0 * cos(x) + 3.0 * sin(20.0 * x)));

            // ch2_re: 10000 * (sin(x) + cos(2*x))
            cal_df[2 * NCHANNELS + i] = (int32_t)(10000.0 * (sin(x) + cos(20.0 * x)));

            // ch2_im: 10000 * (2 * sin(x) - cos(x))
            cal_df[3 * NCHANNELS + i] = (int32_t)(10000.0 * (2.0 * sin(50.0 * x) - cos(x)));
        }

        // ignore PFB bin, just populate CAL_DF with data from array

//        bool add_noise = true;
//        for(int i = 0 ; i < NCHANNELS * 4 ; ++i) {
//            if (add_noise) {
//                double noise = generate_gaussian_variate();
//                cal_df[i] = (int32_t)(2 * cal_zoom_data[i] * noise);
//            } else {
//                cal_df[i] = cal_zoom_data[i];
//            }
//        }
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

uint32_t calib_get_slicer_errors(uint8_t antenna_mask) { return 0; }

uint32_t calib_get_errors() { return 0; }

int calib_get_readout_mode() { }

void calib_set_ddrift_max(int val) { }

void calib_set_gphase_max(int val) { }

void cal_copy_errors(struct calibrator_error_reg* err) { }