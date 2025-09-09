#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>

#include "core_loop.h"
#include "spectrometer_interface.h"
#include "LuSEE_IO.h"


static inline int32_t safe_abs_val(int32_t val)
{
    if (val >= 0) return val;
    if (val == INT32_MIN) return INT32_MAX;
    return -val;
}

uint16_t encode_10plus6(int32_t val) {
    if (val == 0) {
        return 0;
    }
    const uint16_t is_neg = (val < 0) ? 32 : 0;

    val = safe_abs_val(val);

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

static inline int8_t get_shift_by(uint32_t val)
{
    int8_t lz = __builtin_clz(val);
    return MAX(16 - lz + 1, 0);
}

int encode_shared_lz_positive(const uint32_t* spectra, unsigned char* cdi_ptr, int size) {
    const unsigned char* const orig_cdi_ptr = cdi_ptr;
    int i = 0;
    while (i < size) {
        int8_t shift_by = get_shift_by(spectra[i]);
        int j = i + 1;
        while (j < size && j - i < 255) {
            int8_t next_shift_by = get_shift_by(spectra[j]);
            if (abs(shift_by - next_shift_by) > 1)
                break;
            j++;
        }
        uint8_t n = (uint8_t)(j - i);
        // if we need to shift all the way to the right by 16 digits,
        // we don't need the leeway of 1 that we allowed ourselves
        if (shift_by == 17)
            shift_by = 16;
        assert(shift_by <= 16);
        memcpy(cdi_ptr, &shift_by, sizeof shift_by);
        cdi_ptr += sizeof shift_by;
        memcpy(cdi_ptr, &n, sizeof n);
        cdi_ptr += sizeof n;
        for (int k = i; k < j; k++) {
            uint16_t compressed_val = (uint16_t )(spectra[k] >> shift_by);
            // do not replace with simple cast and update, something like
            // *((*uint16_t)cdi_ptr) = compressed_val
            // fails the tests; memcpy is safe standard-compliant way
            memcpy(cdi_ptr, &compressed_val, sizeof compressed_val);
            cdi_ptr += sizeof compressed_val;
        }
        i = j;
    }
    return cdi_ptr - orig_cdi_ptr;
}

void decode_shared_lz_positive(const unsigned char* data_buf, uint32_t* x, int size) {
    int i = 0;
    while (i < size) {
        int8_t shift_by;
        uint8_t n;
        memcpy(&shift_by, data_buf, sizeof shift_by);
        data_buf += sizeof shift_by;
        memcpy(&n, data_buf, sizeof n);
        data_buf += sizeof n;
        for (int j = 0; j < n; j++) {
            uint16_t compressed_val;
            memcpy(&compressed_val, data_buf, sizeof compressed_val);
            assert((shift_by >= 0) && (shift_by <= 16));
            x[i] = compressed_val << shift_by;
            data_buf += sizeof compressed_val;
            i++;
        }
    }
}

int encode_shared_lz_signed(const int32_t* spectra, unsigned char* cdi_ptr, int size) {
    unsigned char* orig_cdi_ptr = cdi_ptr;
    int i = 0;
    while (i < size) {
        int8_t is_neg = IS_NEG(spectra[i]);
        int8_t shift_by = get_shift_by(is_neg ? -spectra[i] : spectra[i]);
        int j = i + 1;
        while (j < size && j - i < 255) {
            int32_t next_val = spectra[j];
            int next_is_neg = IS_NEG(next_val);
            if (next_is_neg != is_neg)
                break;
            uint32_t next_abs_val = safe_abs_val(next_val);
            int8_t next_shift_by = get_shift_by(next_abs_val);
            if (abs(next_shift_by - shift_by) > 1) {
                break;
            }
            j++;
        }

        if (shift_by == 17)
            shift_by = 16;
        assert(shift_by <= 16);

        uint8_t segment_len = (uint8_t)(j - i);
        // pack is_neg info and leading zeros together: MSB for is_neg
        assert(0 == ((is_neg << 7) & shift_by));
        uint8_t sign_and_shift = (is_neg << 7) | shift_by;


        memcpy(cdi_ptr, &sign_and_shift, sizeof(sign_and_shift));
        cdi_ptr += sizeof(sign_and_shift);

        memcpy(cdi_ptr, &segment_len, sizeof(segment_len));
        cdi_ptr += sizeof(segment_len);

        for (int k = i; k < j; k++) {
            assert(is_neg == IS_NEG(spectra[k]));
            uint32_t abs_val = safe_abs_val(spectra[k]);
            uint16_t compressed_val = abs_val >> shift_by;
            memcpy(cdi_ptr, &compressed_val, sizeof(compressed_val));
            cdi_ptr += sizeof(compressed_val);
        }
        i = j;
    }
    // return number of bytes written
    return cdi_ptr - orig_cdi_ptr;
}

void decode_shared_lz_signed(const unsigned char* data_buf, int32_t* x, int size) {
    int i = 0;
    while (i < size) {

        uint8_t sign_and_shift;
        memcpy(&sign_and_shift, data_buf, sizeof(sign_and_shift));
        data_buf += sizeof(sign_and_shift);
        int is_neg = (sign_and_shift >> 7) & 1;
        uint8_t shift_by = sign_and_shift & 31;

        uint8_t segment_len;
        memcpy(&segment_len, data_buf, sizeof(segment_len));
        data_buf += sizeof(segment_len);

        for (int j = 0; j < segment_len; j++) {
            uint16_t compressed_val;
            memcpy(&compressed_val, data_buf, sizeof(compressed_val));
            data_buf += sizeof(compressed_val);

            uint32_t abs_val = compressed_val << shift_by;
            x[i] = (is_neg == 1) ? -abs_val : abs_val;
            i++;
        }
    }
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

// encode vals_in[0],.., vals_in[3] into vals_out[0], ..., vals_out[4]
// 4 bits for lz: store difference from 16
// 32 bits -> lower 14 bits
// bit 16: sign
// bit 15: sign of lz
void encode_4_into_5(const int32_t* const vals_in, uint16_t* vals_out)
{
    uint16_t* const shifts = vals_out;
    *shifts = 0;
    uint16_t* compressed = vals_out + 1;
    for(int i = 0; i < 4; ++i) {
        uint16_t negative_bit = (vals_in[i] < 0) ? 1 << 15 : 0;
        int32_t abs_value = safe_abs_val(vals_in[i]);
        uint8_t lz = __builtin_clz(abs_value);
        uint16_t shift = (lz >= 18) ? 0 : 18 - lz;
        uint16_t in_place_shift_bit = (shift >= 16) ? 1 << 14 : 0;
        uint16_t stored_shift = (shift >= 16) ? shift - 16 : shift;
        assert(((stored_shift << (4 * i)) & (*shifts)) == 0);
        *shifts |= (stored_shift << (4 * i));
        compressed[i] = abs_value >> shift;
        assert((negative_bit & in_place_shift_bit) == 0);
        assert((negative_bit & compressed[i]) == 0);
        assert((in_place_shift_bit & compressed[i]) == 0);
        compressed[i] |= negative_bit;
        compressed[i] |= in_place_shift_bit;
    }
}

void decode_5_into_4(const uint16_t* const vals_in, int32_t* vals_out)
{
    const uint16_t shifts = *vals_in;
    const uint16_t* compressed = vals_in + 1;
    for(int i = 0; i < 4; ++i) {
        bool is_neg = compressed[i] & (1 << 15);
        uint16_t shift_adjustment = (compressed[i] & (1 << 14)) ? 16 : 0;
        uint16_t shift = ((shifts >> 4 * i) & 0xF) + shift_adjustment;
        int32_t abs_val = (compressed[i] & 0x3FFF) << shift;
        vals_out[i] = is_neg ? -abs_val : abs_val;
    }
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


void print_buf(const void* data, size_t size) {
    uint8_t *b = (uint8_t *)(data);
    for (int i=0; i<size; i++) {
        debug_print_hex(b[i]);
        debug_print(" ");
    }
    debug_print("\r\n")
}

#ifndef NOTREAL
extern char __stack_bottom;  // Defined in linker script
extern char __stack_top;    // Defined in linker script

size_t get_free_stack() {
    volatile char dummy;
    char *current_sp = (char*)&dummy;

    size_t total_stack = &__stack_top - &__stack_bottom;
    size_t used_stack = &__stack_top - current_sp;

    return total_stack - used_stack;
}
#endif
