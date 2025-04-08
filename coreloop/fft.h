#ifndef LN_CORELOOP_FFT_H
#define LN_CORELOOP_FFT_H

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

//#define LN_CORELOOP_FFT_TIMING

#define FIXED_POINT_FRACTIONAL_BITS 16
#define FIXED_POINT_SCALE (1 << FIXED_POINT_FRACTIONAL_BITS)
#define FFT_BIT_SIZE 6
#define FFT_SIZE (1 << FFT_BIT_SIZE)
#define TABLE_SIZE FFT_SIZE
#define NUM_FFTS_IN_ONE_GO 4

extern int32_t sine_table_int[TABLE_SIZE];
extern float sine_table_float[TABLE_SIZE];

void fft_precompute_tables();

int32_t get_sine_int(int index);
int32_t get_cosine_int(int index);

float get_sine_float(int index);
float get_cosine_float(int index);

void print_binary(uint8_t index);

uint8_t reverse_bits(uint8_t index);
void fft_int(int32_t *input_real, int32_t *input_imag, int32_t *output_real, int32_t *output_imag);
void fft_int_in_place(int32_t *real_ptr, int32_t *imag_ptr);
void fft_int_multiple(int32_t** input_real, int32_t** input_imag, int32_t** output_real, int32_t** output_imag);
void fft_float(int32_t *input_real, int32_t *input_imag, float* output_real, float* output_imag);
void fft_float_multiple(int32_t** input_real, int32_t** input_imag, float** output_real, float** output_imag);

#endif //LN_CORELOOP_FFT_H
