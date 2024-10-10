#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "core_loop.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// generate random positive int32_t
// lz: number of leading zeros
int32_t random_with_lz(int lz) {
    if (lz < 1 || lz > 31) {
        return 0;
    }

    int32_t result = 1 << (31 - lz);  // set MSB after lz zeros

    // Randomly set the remaining bits
    for (int i = 31 - lz - 1; i >= 0; --i) {
        if (rand() % 2) {
            result |= (1 << i);
        }
    }

    return result;
}

// test functions to encode int32_t into uint_16_t and decode back
// test that decode(encode(x)) / x differs from 1 by at most epsilon
bool test_encode_decode(uint16_t (*encode)(int32_t), int32_t (*decode)(uint16_t), long double epsilon)
{
    srand(42);

    // first 128 integers are covered in full
    for(int32_t x = 1; x < 128; ++x) {
            {
                int32_t y = x;
                uint16_t y_encoded = encode(y);
                int32_t y_decoded = decode(y_encoded);
                long double ratio = (long double) y_decoded / (long double) y;
                if (fabs(ratio - 1.0) > epsilon) {
                    printf("encode/decode FAILED: x = %d, x_encoded = %u, x_decoded = %d, ratio = %Lf\n", y, y_encoded, y_decoded, ratio);
                    return false;
                }
            }
            {
                int32_t y = -x;
                uint16_t y_encoded = encode(y);
                int32_t y_decoded = decode(y_encoded);
                long double ratio = (long double) y_decoded / (long double) y;
                if (fabs(ratio - 1.0) > epsilon) {
                    printf("encode/decode FAILED: x = %d, x_encoded = %u, x_decoded = %d, ratio = %Lf\n", y, y_encoded, y_decoded, ratio);
                    return false;
                }
            }
    }

    // check 16 random numbers for given leading zeros
    for(int lz = 7; lz < 32; ++lz) {
        for(int rep = 0; rep < 16; ++rep) {
            int32_t x = random_with_lz(lz);
            {
                uint16_t x_encoded = encode(x);
                int32_t x_decoded = decode(x_encoded);
                long double ratio = (long double) x_decoded / (long double) x;
                if (fabs(ratio - 1.0) > epsilon) {
                    printf("encode/decode FAILED: x = %d, x_encoded = %u, x_decoded = %d, ratio = %Lf\n", x, x_encoded, x_decoded, ratio);
                    return false;
                }
            }

            x = -x;

            {
                uint16_t x_encoded = encode(x);
                int32_t x_decoded = decode(x_encoded);
                long double ratio = (long double) x_decoded / (long double) x;
                if (fabs(ratio - 1.0) > epsilon) {
                    printf("encode/decode FAILED: x = %d, x_encoded = %u, x_decoded = %d, ratio = %Lf\n", x, x_encoded, x_decoded, ratio);
                    return false;
                }
            }
        }
    }

    return true;
}

int main()
{
    int n_failed = 0;

    if (!test_encode_decode(encode_10plus6, decode_10plus6, 0.005)) {
        n_failed++;
    }

    // fails for small numbers
    if (!test_encode_decode(encode_12plus4, decode_12plus4, 0.005)) {
        n_failed++;
    }

    return n_failed;
}