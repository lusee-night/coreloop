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

/**
 * Computes the sine of a value using Taylor series expansion.
 * Approximates sin(x) using the first 5 terms of the Taylor series.
 *
 * @param x The input angle in radians
 * @return The computed sine value
 * NB: is NOT used, sines are hardcoded above
 */
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

/**
 * Retrieves a sine value from the fixed-point integer lookup table.
 * Uses modulo wrapping to ensure the index stays within table bounds.
 *
 * @param index The table index (automatically wrapped to valid range)
 * @return The sine value as a fixed-point int32_t
 */
int32_t get_sine_int(int index) {
    index = index & (FFT_SIZE - 1);
    return sine_table_int[index];
}

/**
 * Retrieves a cosine value from the fixed-point integer lookup table.
 * Implements cosine by offsetting the sine lookup by 90 degrees (TABLE_SIZE/4).
 *
 * @param index The table index
 * @return The cosine value as a fixed-point int32_t
 */
int32_t get_cosine_int(int index) {
    return get_sine_int(index + TABLE_SIZE / 4);
}

/**
 * Retrieves a sine value from the floating-point lookup table.
 * Uses modulo wrapping to ensure the index stays within table bounds.
 *
 * @param index The table index (automatically wrapped to valid range)
 * @return The sine value as a float
 */
float get_sine_float(int index) {
    index = index & (FFT_SIZE - 1);
    return sine_table_float[index];
}

/**
 * Retrieves a cosine value from the floating-point lookup table.
 * Implements cosine by offsetting the sine lookup by 90 degrees (TABLE_SIZE/4).
 *
 * @param index The table index
 * @return The cosine value as a float
 */
float get_cosine_float(int index) {
    return get_sine_float(index + TABLE_SIZE / 4);
}

/**
 * Computes a complex exponential e^(i*theta) from the lookup table.
 * Returns the complex value cos(theta) + i*sin(theta).
 *
 * @param index The angle index into the lookup tables
 * @return A complex float value representing e^(i*theta)
 * NB: is NOT used, does not work on real hardware
 */
float complex get_exp_complex(int index) {
    return get_cosine_float(index) + I * get_sine_float(index);
}

/**
 * Prints the binary representation of an 8-bit unsigned integer.
 * Outputs 32 bits (treating the 8-bit value with leading zeros) to stdout.
 *
 * @param n The unsigned 8-bit value to print in binary format
 */
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

/**
 * Reverses the bits of an 8-bit index for FFT bit-reversal permutation.
 * Performs bit reversal appropriate for the configured FFT_BIT_SIZE.
 *
 * @param index The input index to reverse
 * @return The bit-reversed index shifted appropriately for FFT size
 */
uint8_t reverse_bits(uint8_t index) {
    index = ((index & 0xAA) >> 1) | ((index & 0x55) << 1);
    index = ((index & 0xCC) >> 2) | ((index & 0x33) << 2);
    index = ((index & 0xF0) >> 4) | ((index & 0x0F) << 4);
    index >>= (8 - FFT_BIT_SIZE);
    return index;
}

/**
 * Performs an in-place FFT using fixed-point integer arithmetic.
 * Applies bit-reversal permutation followed by Cooley-Tukey FFT algorithm.
 * Modifies the input arrays directly with scaling to prevent overflow.
 *
 * @param real_ptr Pointer to array of real components (modified in-place)
 * @param imag_ptr Pointer to array of imaginary components (modified in-place)
 */
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

/**
 * Performs FFT with separate input and output buffers using fixed-point arithmetic.
 * Copies input to output, then applies bit-reversal and Cooley-Tukey FFT algorithm.
 * Uses scaling at each stage to prevent overflow.
 *
 * @param input_real Pointer to input real components
 * @param input_imag Pointer to input imaginary components
 * @param output_real Pointer to output real components
 * @param output_imag Pointer to output imaginary components
 */
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

/**
 * Performs multiple FFTs in batch using fixed-point integer arithmetic.
 * Processes NUM_FFTS_IN_ONE_GO separate FFTs with interleaved data layout.
 * Optimizes computation by sharing twiddle factor calculations across batches.
 *
 * @param input_real_beg Pointer to beginning of interleaved input real arrays
 * @param input_imag_beg Pointer to beginning of interleaved input imaginary arrays
 * @param output_real_beg Pointer to beginning of interleaved output real arrays
 * @param output_imag_beg Pointer to beginning of interleaved output imaginary arrays
 */
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

/**
 * Performs FFT converting fixed-point input to floating-point output.
 * Applies bit-reversal during input copy, then executes Cooley-Tukey FFT
 * using floating-point arithmetic without scaling.
 *
 * @param input_real Pointer to fixed-point input real components
 * @param input_imag Pointer to fixed-point input imaginary components
 * @param output_real Pointer to floating-point output real components
 * @param output_imag Pointer to floating-point output imaginary components
 */
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

/**
 * Performs multiple FFTs in batch converting fixed-point input to floating-point output.
 * Processes NUM_FFTS_IN_ONE_GO separate FFTs with interleaved data layout.
 * Optimizes by sharing twiddle factor calculations and using floating-point precision.
 *
 * @param input_real_beg Pointer to beginning of interleaved fixed-point input real arrays
 * @param input_imag_beg Pointer to beginning of interleaved fixed-point input imaginary arrays
 * @param output_real_beg Pointer to beginning of interleaved floating-point output real arrays
 * @param output_imag_beg Pointer to beginning of interleaved floating-point output imaginary arrays
 */
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
