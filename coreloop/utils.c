#include "core_loop.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "LuSEE_IO.h"

uint16_t encode_10plus6(int32_t val) {
    if (val == 0) {
        return 0;
    }
    const uint16_t is_neg = (val < 0) ? 32 : 0;

    if (val < 0) val = -val;

    // leading zeros takes 5 bits max
    const uint16_t lz = __builtin_clz(val);

    const uint16_t lower_part = is_neg + lz;
    const uint16_t lower_part_mask = ~63;

    uint16_t upper_part;

    if (lz > 16) {
        upper_part = (val << (lz - 16));
    } else if (lz < 16) {
        upper_part = (val >> (16 - lz));
    } else {
        upper_part = val;
    }

    return (upper_part & lower_part_mask) + lower_part;
}


int32_t decode_10plus6(uint16_t val) {
    if (val == 0)
        return 0;
    bool is_neg = val & 32;
    uint16_t lz = val & 31;
    int32_t out = val & ~63;

    if (lz > 16) {
        out = out >> (lz - 16);
    } else if (lz < 16) {
        out = out << (16 - lz);
    }

    if (is_neg)
        out = -out;

    return out;
}

uint16_t encode_12plus4(int32_t val) {
    uint32_t out = 0;
    uint8_t lz = __builtin_clz(abs(val));
    if (lz>15) lz=15;
    // Super important: since the MSB after shifting is zero, we can skip it.
    out = ((val >> (16-lz-1)) & 0xFFF0) + lz; // the first 12 bits are the actual value, the last 4 are the number of leading zeros.
    return out;
}

int32_t decode_12plus4(uint16_t val) {
    uint8_t lz = val & 0x0F;
    uint32_t val_wo_msb = val & 0xFFF0;
    uint32_t val_wo_lz_1 = val_wo_msb + 0x10000;
    int32_t out = val_wo_lz_1 << (16 - lz -1);
    return out;
}

uint32_t CRC(const void* data, size_t size) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < size; i++) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}


uint32_t print_buf(const void* data, size_t size) {
    uint8_t *b = (uint8_t *)(data);
    for (int i=0; i<size; i++) {
        debug_print_hex(b[i]);
        debug_print(" ");
    }
    debug_print("\r\n")
}


