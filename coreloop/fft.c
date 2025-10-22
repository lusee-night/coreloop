#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "LuSEE_IO.h"
#include "core_loop.h"
#include "fft.h"

const int32_t sine_table_int[TABLE_SIZE] = {
    (int32_t)(0.0 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.0980171403295606 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.19509032201612825 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.29028467725446233 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.3826834323650898 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.47139673682599764 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.5555702330196022 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.6343932841636455 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.7071067811865475 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.773010453362737 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.8314696123025452 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.8819212643483549 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9238795325112867 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9569403357322089 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9807852804032304 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9951847266721968 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(1.0 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9951847266721969 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9807852804032304 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9569403357322089 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.9238795325112867 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.881921264348355 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.8314696123025455 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.7730104533627371 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.7071067811865476 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.6343932841636455 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.5555702330196022 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.47139673682599786 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.3826834323650899 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.2902846772544624 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.1950903220161286 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(0.09801714032956083 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(1.2246467991473532e-16 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.09801714032956059 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.19509032201612836 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.2902846772544621 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.38268343236508967 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.47139673682599764 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.555570233019602 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.6343932841636453 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.7071067811865475 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.7730104533627367 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.8314696123025452 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.8819212643483549 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9238795325112865 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9569403357322088 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9807852804032303 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9951847266721969 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-1.0 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9951847266721969 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9807852804032304 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9569403357322089 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.9238795325112866 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.881921264348355 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.8314696123025455 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.7730104533627369 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.7071067811865477 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.6343932841636459 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.5555702330196022 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.4713967368259979 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.3826834323650904 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.2902846772544625 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.19509032201612872 * FIXED_POINT_SCALE / 2.0),
    (int32_t)(-0.0980171403295605 * FIXED_POINT_SCALE / 2.0),
};


const float sine_table_float[TABLE_SIZE] = {
 0.0,
 0.0980171403295606,
 0.19509032201612825,
 0.29028467725446233,
 0.3826834323650898,
 0.47139673682599764,
 0.5555702330196022,
 0.6343932841636455,
 0.7071067811865475,
 0.773010453362737,
 0.8314696123025452,
 0.8819212643483549,
 0.9238795325112867,
 0.9569403357322089,
 0.9807852804032304,
 0.9951847266721968,
 1.0,
 0.9951847266721969,
 0.9807852804032304,
 0.9569403357322089,
 0.9238795325112867,
 0.881921264348355,
 0.8314696123025455,
 0.7730104533627371,
 0.7071067811865476,
 0.6343932841636455,
 0.5555702330196022,
 0.47139673682599786,
 0.3826834323650899,
 0.2902846772544624,
 0.1950903220161286,
 0.09801714032956083,
 1.2246467991473532e-16,
 -0.09801714032956059,
 -0.19509032201612836,
 -0.2902846772544621,
 -0.38268343236508967,
 -0.47139673682599764,
 -0.555570233019602,
 -0.6343932841636453,
 -0.7071067811865475,
 -0.7730104533627367,
 -0.8314696123025452,
 -0.8819212643483549,
 -0.9238795325112865,
 -0.9569403357322088,
 -0.9807852804032303,
 -0.9951847266721969,
 -1.0,
 -0.9951847266721969,
 -0.9807852804032304,
 -0.9569403357322089,
 -0.9238795325112866,
 -0.881921264348355,
 -0.8314696123025455,
 -0.7730104533627369,
 -0.7071067811865477,
 -0.6343932841636459,
 -0.5555702330196022,
 -0.4713967368259979,
 -0.3826834323650904,
 -0.2902846772544625,
 -0.19509032201612872,
 -0.0980171403295605,
};

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
}

void fft_int(int32_t *input_real, int32_t *input_imag, int32_t *output_real, int32_t *output_imag)
{
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
}

void fft_int_multiple(int32_t* input_real_beg, int32_t* input_imag_beg, int32_t* output_real_beg, int32_t* output_imag_beg)
{
    for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
        // Copy input to output initially
        for (int i = 0; i < FFT_SIZE; i++) {
            output_real_beg[arr_idx * FFT_SIZE + i] = input_real_beg[arr_idx * FFT_SIZE + i];
            output_imag_beg[arr_idx * FFT_SIZE + i] = input_imag_beg[arr_idx * FFT_SIZE + i];
        }
    }

    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        if (i < j) {
            // Swap only if i < j to avoid double swapping
            for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
                int32_t temp_real = output_real_beg[arr_idx * FFT_SIZE + i];
                int32_t temp_imag = output_imag_beg[arr_idx * FFT_SIZE + i];
                output_real_beg[arr_idx * FFT_SIZE + i] = output_real_beg[arr_idx * FFT_SIZE + j];
                output_imag_beg[arr_idx * FFT_SIZE + i] = output_imag_beg[arr_idx * FFT_SIZE + j];
                output_real_beg[arr_idx * FFT_SIZE + j] = temp_real;
                output_imag_beg[arr_idx * FFT_SIZE + j] = temp_imag;
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
                    int64_t temp_real = output_real_beg[arr_idx * FFT_SIZE + idx2];
                    int64_t temp_imag = output_imag_beg[arr_idx * FFT_SIZE + idx2];

                    int64_t product_real = ((twiddle_real * temp_real - twiddle_imag * temp_imag)
                                          >> (FIXED_POINT_FRACTIONAL_BITS - 1));  // Adjust shift
                    int64_t product_imag = ((twiddle_real * temp_imag + twiddle_imag * temp_real)
                                          >> (FIXED_POINT_FRACTIONAL_BITS - 1));

                    // Butterfly operation
                    output_real_beg[arr_idx * FFT_SIZE + idx2] = (output_real_beg[arr_idx * FFT_SIZE + idx1] - product_real) >> 1;  // Scale down to prevent overflow
                    output_imag_beg[arr_idx * FFT_SIZE + idx2] = (output_imag_beg[arr_idx * FFT_SIZE + idx1] - product_imag) >> 1;
                    output_real_beg[arr_idx * FFT_SIZE + idx1] = (output_real_beg[arr_idx * FFT_SIZE + idx1] + product_real) >> 1;
                    output_imag_beg[arr_idx * FFT_SIZE + idx1] = (output_imag_beg[arr_idx * FFT_SIZE + idx1] + product_imag) >> 1;
                }
            }
        }
    }

}

void fft_float(int32_t *input_real, int32_t *input_imag, float* output_real, float* output_imag)
{
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
}


void fft_float_multiple(int32_t* input_real_beg, int32_t* input_imag_beg, float* output_real_beg, float* output_imag_beg)
{

    // Bit reversal
    for (uint8_t i = 0; i < FFT_SIZE; i++) {
        uint8_t j = reverse_bits(i);

        for(int arr_idx = 0; arr_idx < NUM_FFTS_IN_ONE_GO; ++arr_idx) {
            output_real_beg[arr_idx * FFT_SIZE + i] = (float)input_real_beg[arr_idx * FFT_SIZE + j];
            output_imag_beg[arr_idx * FFT_SIZE + i] = (float)input_imag_beg[arr_idx * FFT_SIZE + j];
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
                    float temp_real = output_real_beg[arr_idx * FFT_SIZE + idx2] * cos_angle - output_imag_beg[arr_idx * FFT_SIZE +idx2] * sin_angle;
                    float temp_imag = output_real_beg[arr_idx * FFT_SIZE + idx2] * sin_angle + output_imag_beg[arr_idx * FFT_SIZE + idx2] * cos_angle;

                    float a_real = output_real_beg[arr_idx * FFT_SIZE + idx1];
                    float a_imag = output_imag_beg[arr_idx * FFT_SIZE + idx1];

                    output_real_beg[arr_idx * FFT_SIZE + idx1] = a_real + temp_real;
                    output_imag_beg[arr_idx * FFT_SIZE + idx1] = a_imag + temp_imag;

                    output_real_beg[arr_idx * FFT_SIZE + idx2] = a_real - temp_real;
                    output_imag_beg[arr_idx * FFT_SIZE + idx2] = a_imag - temp_imag;
                }
            }
        }
    }

}
