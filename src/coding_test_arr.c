#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "core_loop.h"

#define ARRAY_SIZE 32

int all_close(uint32_t* a, uint32_t* b, int size, double rtol, double atol) {
    for (int i = 0; i < size; i++) {
        double diff = fabs((double)a[i] - (double)b[i]);
        double tolerance = atol + rtol * fabs((double)b[i]);
        if (diff > tolerance) {
            printf("ERROR: i = %d, a[i] = %u, b[i] = %u, diff = %f\n", i, a[i], b[i], diff);
            return 0;  // Return false if any pair does not meet the tolerance criteria
        }
    }
    return 1;  // Return true if all elements meet the tolerance criteria
}

void test_small_constant() {
    uint32_t* a1 = malloc(ARRAY_SIZE * sizeof(uint32_t));
    unsigned char* a1_compressed = malloc(ARRAY_SIZE * 4 * sizeof(unsigned char));
    uint32_t* a1_back = malloc(ARRAY_SIZE * sizeof(uint32_t));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        a1[i] = 42;
    }

    int num_bytes_written = encode_shared_lz_positive(a1, a1_compressed, ARRAY_SIZE);
    decode_shared_lz_positive(a1_compressed, a1_back, ARRAY_SIZE);

    if (all_close(a1, a1_back, ARRAY_SIZE, 0.01, 1.0)) {
        printf("test_small_constant passed\n");
    } else {
        printf("test_small_constant failed\n");
    }

    free(a1);
    free(a1_compressed);
    free(a1_back);
}

void test_big_constant() {
    uint32_t* a2 = malloc(ARRAY_SIZE * sizeof(uint32_t));
    unsigned char* a2_compressed = malloc(ARRAY_SIZE * 4 * sizeof(unsigned char));
    uint32_t* a2_back = malloc(ARRAY_SIZE * sizeof(uint32_t));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        a2[i] = (1 << 27);
    }

    int num_bytes_written = encode_shared_lz_positive(a2, a2_compressed, ARRAY_SIZE);
    decode_shared_lz_positive(a2_compressed, a2_back, ARRAY_SIZE);

    if (all_close(a2, a2_back, ARRAY_SIZE, 0.01, 1.0)) {
        printf("test_big_constant passed\n");
    } else {
        printf("test_big_constant failed\n");
    }

    free(a2);
    free(a2_compressed);
    free(a2_back);
}

void test_medium_constants() {
    for (int lz = 15; lz <= 17; lz++) {
        uint32_t* a3 = malloc(ARRAY_SIZE * sizeof(uint32_t));
        unsigned char* a3_compressed = malloc(ARRAY_SIZE * 4 * sizeof(unsigned char));
        uint32_t* a3_back = malloc(ARRAY_SIZE * sizeof(uint32_t));

        uint32_t value = (1 << (32 - lz));
        for (int i = 0; i < ARRAY_SIZE; i++) {
            a3[i] = value;
        }

        int num_bytes_written = encode_shared_lz_positive(a3, a3_compressed, ARRAY_SIZE);
        decode_shared_lz_positive(a3_compressed, a3_back, ARRAY_SIZE);

        if (all_close(a3, a3_back, ARRAY_SIZE, 0.01, 1.0)) {
            printf("test_medium_constants with lz=%d passed\n", lz);
        } else {
            printf("test_medium_constants with lz=%d failed\n", lz);
        }

        free(a3);
        free(a3_compressed);
        free(a3_back);
    }
}

void test_smooth_values() {
    uint32_t* a4 = malloc(ARRAY_SIZE * sizeof(uint32_t));
    unsigned char* a4_compressed = malloc(ARRAY_SIZE * 4 * sizeof(unsigned char));
    uint32_t* a4_back = malloc(ARRAY_SIZE * sizeof(uint32_t));

    for (int amplitude_exp = 10; amplitude_exp <= 30; amplitude_exp += 10) {
        double amplitude = pow(2, amplitude_exp);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            a4[i] = (uint32_t)(sin((2 * M_PI * i) / ARRAY_SIZE) * amplitude);
        }

        int num_bytes_written = encode_shared_lz_positive(a4, a4_compressed, ARRAY_SIZE);
        decode_shared_lz_positive(a4_compressed, a4_back, ARRAY_SIZE);

        if (all_close(a4, a4_back, ARRAY_SIZE, 0.01, 1.0)) {
            printf("test_smooth_values with amplitude=2^%d passed\n", amplitude_exp);
        } else {
            printf("test_smooth_values with amplitude=2^%d failed\n", amplitude_exp);
        }
    }

    free(a4);
    free(a4_compressed);
    free(a4_back);
}

void test_random_input() {
    uint32_t* a5 = malloc(ARRAY_SIZE * sizeof(uint32_t));
    unsigned char* a5_compressed = malloc(ARRAY_SIZE * 4 * sizeof(unsigned char));
    uint32_t* a5_back = malloc(ARRAY_SIZE * sizeof(uint32_t));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        a5[i] = rand() % (1 << 31);
    }

    for (int i = 0; i < 10; i++) {
        int idx = rand() % ARRAY_SIZE;
        long long big = (1L << 32L) - 1L;
        a5[idx] = (rand() % 2) ? 0 : (uint32_t)big;
    }

    int num_bytes_written = encode_shared_lz_positive(a5, a5_compressed, ARRAY_SIZE);
    decode_shared_lz_positive(a5_compressed, a5_back, ARRAY_SIZE);

    if (all_close(a5, a5_back, ARRAY_SIZE, 0.01, 1.0)) {
        printf("test_random_input passed\n");
    } else {
        printf("test_random_input failed\n");
    }

    free(a5);
    free(a5_compressed);
    free(a5_back);
}

int main() {
    srand(1);
//    test_small_constant();
//    test_big_constant();
//    test_medium_constants();
    test_smooth_values();
//    test_random_input();
    return 0;
}
