#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "LuSEE_IO.h"
#include "core_loop.h"
#include "fft.h"

#undef LN_CORELOOP_FFT_TIMING

int32_t sine_table_int[TABLE_SIZE];
float sine_table_float[TABLE_SIZE];

float my_sin(float x) {
    float result = x;  // First term is x
    float term = x;    // Current term, initialized to x
    int sign = -1;     // Alternates between positive and negative

    for (int i = 1; i < 5; ++i) {
        int term_index = 2 * i + 1;
        term *= x * x / ((term_index - 1) * term_index);  // Compute the next term
        result += sign * term;
        sign = -sign;  // Alternate the sign
    }
    return result;
}

void fft_precompute_tables()
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        double angle = 2.0 * M_PI * i / FFT_SIZE;
        // Scale down to prevent overflow
        sine_table_int[i] = (int32_t)(my_sin(angle) * (FIXED_POINT_SCALE / 2.0));
        sine_table_float[i] = my_sin(angle);
    }
}


int32_t get_sine_int(int index) {
    index = index & (FFT_SIZE - 1);
    return sine_table_int[index];
}

int32_t get_cosine_int(int index) {
    return get_sine_int(index + TABLE_SIZE / 4);
}

float get_sine_float(int index) {
    index = index & (FFT_SIZE - 1);
    return sine_table_float[index];
}

float get_cosine_float(int index) {
    return get_sine_float(index + TABLE_SIZE / 4);
}

float complex get_exp_complex(int index) {
    return get_cosine_float(index) + I * get_sine_float(index);
}


void print_binary(uint8_t n) {
    unsigned int mask = 1 << 31; // Mask with the highest bit set
    for (int i = 0; i < 32; i++) {
        if (n & mask) {
            printf("1");
        } else {
            printf("0");
        }
        mask >>= 1;
    }
    printf("\n");
}

uint8_t reverse_bits(uint8_t index) {
    index = ((index & 0xAA) >> 1) | ((index & 0x55) << 1);
    index = ((index & 0xCC) >> 2) | ((index & 0x33) << 2);
    index = ((index & 0xF0) >> 4) | ((index & 0x0F) << 4);
    index >>= (8 - FFT_BIT_SIZE);
    return index;
}

void fft_int_in_place(int32_t *real_ptr, int32_t *imag_ptr)
{
#ifdef LN_CORELOOP_FFT_TIMING
    timer_start();
#endif

    // Bit reversal
    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        if (i < j) {
            // Swap only if i < j to avoid double swapping
            int32_t temp_real = real_ptr[i];
            int32_t temp_imag = imag_ptr[i];
            real_ptr[i] = real_ptr[j];
            imag_ptr[i] = imag_ptr[j];
            real_ptr[j] = temp_real;
            imag_ptr[j] = temp_imag;
        }
    }

    // FFT computation
    for (int stage = 1; stage <= FFT_BIT_SIZE; stage++) {
        int m = 1 << stage;
        int half_m = m >> 1;

        for (int k = 0; k < FFT_SIZE; k += m) {
            for (int j = 0; j < half_m; j++) {
                int idx1 = k + j;
                int idx2 = idx1 + half_m;

                int angle = (j * FFT_SIZE / m) & (FFT_SIZE - 1);

                int64_t twiddle_real = get_cosine_int(angle);
                int64_t twiddle_imag = -get_sine_int(angle);  // Note the negative sign

                // Complex multiplication
                int64_t temp_real = real_ptr[idx2];
                int64_t temp_imag = imag_ptr[idx2];

                int64_t product_real = ((twiddle_real * temp_real - twiddle_imag * temp_imag)
                                      >> (FIXED_POINT_FRACTIONAL_BITS - 1));  // Adjust shift
                int64_t product_imag = ((twiddle_real * temp_imag + twiddle_imag * temp_real)
                                      >> (FIXED_POINT_FRACTIONAL_BITS - 1));

                // Butterfly operation
                real_ptr[idx2] = (real_ptr[idx1] - product_real) >> 1;  // Scale down to prevent overflow
                imag_ptr[idx2] = (imag_ptr[idx1] - product_imag) >> 1;
                real_ptr[idx1] = (real_ptr[idx1] + product_real) >> 1;
                imag_ptr[idx1] = (imag_ptr[idx1] + product_imag) >> 1;
            }
        }
    }

#ifdef LN_CORELOOP_FFT_TIMING
    uint32_t elapsed = timer_stop();
    debug_print("FFT(int,single): ");
    debug_print_dec(elapsed);
    debug_print("\n ");
#endif
}

void fft_int(int32_t *input_real, int32_t *input_imag, int32_t *output_real, int32_t *output_imag)
{
#ifdef LN_CORELOOP_FFT_TIMING
    timer_start();
#endif
    // Copy input to output initially
    for (int i = 0; i < FFT_SIZE; i++) {
        output_real[i] = input_real[i];
        output_imag[i] = input_imag[i];
    }

    // Bit reversal
    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        if (i < j) {
            // Swap only if i < j to avoid double swapping
            int32_t temp_real = output_real[i];
            int32_t temp_imag = output_imag[i];
            output_real[i] = output_real[j];
            output_imag[i] = output_imag[j];
            output_real[j] = temp_real;
            output_imag[j] = temp_imag;
        }
    }

    // FFT computation
    for (int stage = 1; stage <= FFT_BIT_SIZE; stage++) {
        int m = 1 << stage;
        int half_m = m >> 1;

        for (int k = 0; k < FFT_SIZE; k += m) {
            for (int j = 0; j < half_m; j++) {
                int idx1 = k + j;
                int idx2 = idx1 + half_m;

                int angle = (j * FFT_SIZE / m) & (FFT_SIZE - 1);

                int64_t twiddle_real = get_cosine_int(angle);
                int64_t twiddle_imag = -get_sine_int(angle);  // Note the negative sign

                // Complex multiplication
                int64_t temp_real = output_real[idx2];
                int64_t temp_imag = output_imag[idx2];

                int64_t product_real = ((twiddle_real * temp_real - twiddle_imag * temp_imag)
                                      >> (FIXED_POINT_FRACTIONAL_BITS - 1));  // Adjust shift
                int64_t product_imag = ((twiddle_real * temp_imag + twiddle_imag * temp_real)
                                      >> (FIXED_POINT_FRACTIONAL_BITS - 1));

                // Butterfly operation
                output_real[idx2] = (output_real[idx1] - product_real) >> 1;  // Scale down to prevent overflow
                output_imag[idx2] = (output_imag[idx1] - product_imag) >> 1;
                output_real[idx1] = (output_real[idx1] + product_real) >> 1;
                output_imag[idx1] = (output_imag[idx1] + product_imag) >> 1;
            }
        }
    }

#ifdef LN_CORELOOP_FFT_TIMING
    uint32_t elapsed = timer_stop();
    debug_print("FFT(int,single): ");
    debug_print_dec(elapsed);
    debug_print("\n ");
#endif
}


void fft_int_multiple(int32_t** input_real, int32_t** input_imag, int32_t** output_real, int32_t** output_imag)
{

#ifdef LN_CORELOOP_FFT_TIMING
    timer_start();
#endif

    for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
        // Copy input to output initially
        for (int i = 0; i < FFT_SIZE; i++) {
            output_real[arr_idx][i] = input_real[arr_idx][i];
            output_imag[arr_idx][i] = input_imag[arr_idx][i];
        }
    }

    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        if (i < j) {
            // Swap only if i < j to avoid double swapping
            for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
                int32_t temp_real = output_real[arr_idx][i];
                int32_t temp_imag = output_imag[arr_idx][i];
                output_real[arr_idx][i] = output_real[arr_idx][j];
                output_imag[arr_idx][i] = output_imag[arr_idx][j];
                output_real[arr_idx][j] = temp_real;
                output_imag[arr_idx][j] = temp_imag;
            }
        }
    }

    // FFT computation
    for (int stage = 1; stage <= FFT_BIT_SIZE; stage++) {
        int m = 1 << stage;
        int half_m = m >> 1;

        for (int k = 0; k < FFT_SIZE; k += m) {
            for (int j = 0; j < half_m; j++) {
                int idx1 = k + j;
                int idx2 = idx1 + half_m;

                int angle = (j * FFT_SIZE / m) & (FFT_SIZE - 1);

                int64_t twiddle_real = get_cosine_int(angle);
                int64_t twiddle_imag = -get_sine_int(angle);  // Note the negative sign

                for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
                    // Complex multiplication
                    int64_t temp_real = output_real[arr_idx][idx2];
                    int64_t temp_imag = output_imag[arr_idx][idx2];

                    int64_t product_real = ((twiddle_real * temp_real - twiddle_imag * temp_imag)
                                          >> (FIXED_POINT_FRACTIONAL_BITS - 1));  // Adjust shift
                    int64_t product_imag = ((twiddle_real * temp_imag + twiddle_imag * temp_real)
                                          >> (FIXED_POINT_FRACTIONAL_BITS - 1));

                    // Butterfly operation
                    output_real[arr_idx][idx2] = (output_real[arr_idx][idx1] - product_real) >> 1;  // Scale down to prevent overflow
                    output_imag[arr_idx][idx2] = (output_imag[arr_idx][idx1] - product_imag) >> 1;
                    output_real[arr_idx][idx1] = (output_real[arr_idx][idx1] + product_real) >> 1;
                    output_imag[arr_idx][idx1] = (output_imag[arr_idx][idx1] + product_imag) >> 1;
                }
            }
        }
    }

#ifdef LN_CORELOOP_FFT_TIMING
    uint32_t elapsed = timer_stop();
    debug_print("FFT(int,multiple): ");
    debug_print_dec(elapsed);
    debug_print("\n ");
#endif

}

void fft_float(int32_t *input_real, int32_t *input_imag, float* output_real, float* output_imag)
{
#ifdef LN_CORELOOP_FFT_TIMING
    timer_start();
#endif
    // copy with bit reversal
    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        output_real[j] = (float)input_real[i];
        output_imag[j] = (float)input_imag[i];
    }

    // FFT computation
    for (int stage = 1; stage <= FFT_BIT_SIZE; stage++) {
        int m = 1 << stage;
        int half_m = m >> 1;

        for (int k = 0; k < FFT_SIZE; k += m) {
            for (int j = 0; j < half_m; j++) {
                int idx1 = k + j;
                int idx2 = idx1 + half_m;

                int angle = (j * FFT_SIZE / m) & (FFT_SIZE - 1);

                float sin_angle = -get_sine_float(angle);
                float cos_angle = get_cosine_float(angle);

                float temp_real = output_real[idx2] * cos_angle - output_imag[idx2] * sin_angle;
                float temp_imag = output_real[idx2] * sin_angle + output_imag[idx2] * cos_angle;

                float a_real = output_real[idx1];
                float a_imag = output_imag[idx1];

                output_real[idx1] = a_real + temp_real;
                output_imag[idx1] = a_imag + temp_imag;

                output_real[idx2] = a_real - temp_real;
                output_imag[idx2] = a_imag - temp_imag;
            }
        }
    }

#ifdef LN_CORELOOP_FFT_TIMING
    uint32_t elapsed = timer_stop();
    debug_print("FFT(float,single): ");
    debug_print_dec(elapsed);
    debug_print("\n ");
#endif
}

void fft_float_multiple(int32_t** input_real, int32_t** input_imag, float** output_real, float** output_imag)
{
#ifdef LN_CORELOOP_FFT_TIMING
    timer_start();
#endif

    // Bit reversal
    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
            output_real[arr_idx][i] = (float)input_real[arr_idx][j];
            output_imag[arr_idx][i] = (float)input_imag[arr_idx][j];
        }
    }

    // FFT computation
    for (int stage = 1; stage <= FFT_BIT_SIZE; stage++) {
        int m = 1 << stage;
        int half_m = m >> 1;

        for (int k = 0; k < FFT_SIZE; k += m) {
            for (int j = 0; j < half_m; j++) {
                int idx1 = k + j;
                int idx2 = idx1 + half_m;

                int angle = (j * FFT_SIZE / m) & (FFT_SIZE - 1);

                float sin_angle = -get_sine_float(angle);
                float cos_angle = get_cosine_float(angle);

                // Butterfly operation
                for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
                    float temp_real = output_real[arr_idx][idx2] * cos_angle - output_imag[arr_idx][idx2] * sin_angle;
                    float temp_imag = output_real[arr_idx][idx2] * sin_angle + output_imag[arr_idx][idx2] * cos_angle;

                    float a_real = output_real[arr_idx][idx1];
                    float a_imag = output_imag[arr_idx][idx1];

                    output_real[arr_idx][idx1] = a_real + temp_real;
                    output_imag[arr_idx][idx1] = a_imag + temp_imag;

                    output_real[arr_idx][idx2] = a_real - temp_real;
                    output_imag[arr_idx][idx2] = a_imag - temp_imag;
                }
            }
        }
    }

#ifdef LN_CORELOOP_FFT_TIMING
    uint32_t elapsed = timer_stop();
    debug_print("FFT(float,multiple): ");
    debug_print_dec(elapsed);
    debug_print("\n ");
#endif
}
