#include "core_loop.h"
#include <stdlib.h>
#include <stdint.h>
#include "LuSEE_IO.h"

int16_t encode_12plus4(int32_t val) {
    int16_t out = 0;
    uint8_t lz = __builtin_clz(abs(val));
    if (lz>15) lz=15;
    // Super important: since the MSB after shifting is zero, we can skip it.
    out = (val >> (16-lz-1)) & 0xFFF0 + lz; // the first 12 bits are the actual value, the last 4 are the number of leading zeros.
    return out;
}

int32_t decode_12plus4(int16_t val) {
    int16_t out = 0;
    uint8_t lz = val & 0x0F;
    out = (0x0100+ (val & 0xFFF0)) << (16-lz-1);
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


