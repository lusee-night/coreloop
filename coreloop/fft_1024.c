#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define FIXED_POINT_FRACTIONAL_BITS 16
#define FIXED_POINT_SCALE (1 << FIXED_POINT_FRACTIONAL_BITS)
#define FFT_SIZE 1024
#define TABLE_SIZE (FFT_SIZE / 4) // Only store values for the first quadrant


// Precomputed sine and cosine tables for the first quadrant
int32_t sine_table[TABLE_SIZE];

void precompute_tables() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        double angle = M_PI_2 * i / TABLE_SIZE;
        sine_table[i] = (int32_t)(sin(angle) * FIXED_POINT_SCALE);
    }
}

int32_t get_sine(int index) {
    index = index % FFT_SIZE;
    if (index < TABLE_SIZE) {
        return sine_table[index];
    } else if (index < 2 * TABLE_SIZE) {
        return sine_table[2 * TABLE_SIZE - index];
    } else if (index < 3 * TABLE_SIZE) {
        return -sine_table[index - 2 * TABLE_SIZE];
    } else {
        return -sine_table[4 * TABLE_SIZE - index];
    }
}

int32_t get_cosine(int index) {
    index = index % FFT_SIZE;
    if (index < TABLE_SIZE) {
        return sine_table[TABLE_SIZE - index];
    } else if (index < 2 * TABLE_SIZE) {
        return -sine_table[index - TABLE_SIZE];
    } else if (index < 3 * TABLE_SIZE) {
        return -sine_table[3 * TABLE_SIZE - index];
    } else {
        return sine_table[index - 3 * TABLE_SIZE];
    }
}



void bit_reversal_permutation(uint32_t *real, uint32_t *imag) {
    int j = 0;
    for (int i = 0; i < FFT_SIZE; i++) {
        if (i < j) {
            uint32_t temp_real = real[i];
            uint32_t temp_imag = imag[i];
            real[i] = real[j];
            imag[i] = imag[j];
            real[j] = temp_real;
            imag[j] = temp_imag;
        }
        int m = FFT_SIZE>> 1;
        while (j >= m && m >= 2) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
}

void fft(uint32_t *real, uint32_t *imag) {
    bit_reversal_permutation(real, imag);

    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half_len = len >> 1;
        int table_step = FFT_SIZE / len;
        for (int i = 0; i < FFT_SIZE; i += len) {
            for (int j = 0; j < half_len; j++) {
                int table_index = j * table_step;
                int32_t cos_val = get_cosine(table_index);
                int32_t sin_val = get_sine(table_index);                

                uint32_t t_real = (cos_val * real[i + j + half_len] - sin_val * imag[i + j + half_len]) >> FIXED_POINT_FRACTIONAL_BITS;
                uint32_t t_imag = (sin_val * real[i + j + half_len] + cos_val * imag[i + j + half_len]) >> FIXED_POINT_FRACTIONAL_BITS;

                real[i + j + half_len] = real[i + j] - t_real;
                imag[i + j + half_len] = imag[i + j] - t_imag;

                real[i + j] += t_real;
                imag[i + j] += t_imag;
            }
        }
    }
}


int main() {
    precompute_tables();

    uint32_t real[FFT_SIZE] = {0}; // Initialize real part to 0
    uint32_t imag[FFT_SIZE] = {0}; // Initialize imaginary part to 0

    fft(real, imag);

    for (int i = 0; i < TABLE_SIZE; i++) {
        printf ("%i \n", sine_table[i]); 
    }
    return 0;
    // Output the transformed data
    for (int i = 0; i < FFT_SIZE; i++) {
     
        printf("real[%d] = %u, imag[%d] = %u\n", i, real[i], i, imag[i]);
    }

    return 0;
}