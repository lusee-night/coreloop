#ifndef LN_CORELOOP_HIGH_PREC_AVG_H
#define LN_CORELOOP_HIGH_PREC_AVG_H

#include <stdio.h>
#include <string.h>
#include "spectrometer_interface.h"
#include "core_loop.h"
#include "LuSEE_IO.h"

//#define NAIVE_HIGH_PREC_AVG

#ifdef NAIVE_HIGH_PREC_AVG

//struct SpectraIn {
//    int32_t low[NCHANNELS * NSPECTRA];      /* Lower 32 bits */
//    int32_t high[NCHANNELS * NSPECTRA / 4]; /* Packed 8-bit values, 4 spectra per int32 */
//};


struct SpectraIn {
    int64_t values[NCHANNELS * NSPECTRA];      /* Lower 32 bits */
};

static inline int64_t get_packed_value(const struct SpectraIn* buf, int total_idx) {
    return buf->values[total_idx];
}

static inline int32_t get_averaged_value_int40(const struct SpectraIn* buf, int offset, int i, int Navgf, int shift_by)
{
    return (int32_t)(buf->values[offset + i] >> shift_by);
}

#else

struct SpectraIn {
    uint32_t low[2048 * 16];      /* Lower 32 bits */
};


/* Helper functions to work with packed high bits */
static inline int8_t get_high_bits(const uint32_t* buf_high, int total_idx) {
    int pack_idx = total_idx >> 2;
    int shift = (total_idx & 3) * 8;
    return (int8_t)((buf_high[pack_idx] >> shift) & 0x7F);
}

static inline int64_t get_packed_sign(const uint32_t* buf_high, int total_idx) {
    int pack_idx = total_idx >> 2;
    int shift = (total_idx & 3) * 8;
    return ((buf_high[pack_idx] >> shift) & 0x80) ? -1 : 1;
}

static inline int8_t is_negative(const uint32_t* buf_high, int total_idx) {
    int pack_idx = total_idx >> 2;
    int shift = (total_idx & 3) * 8;
    return ((buf_high[pack_idx] >> shift) & 0x80) ? 1 : 0;
}


static inline int64_t get_packed_value(const struct SpectraIn* buf, const uint32_t* buf_high, int total_idx) {
    return (((int64_t)get_high_bits(buf_high, total_idx) << 32) | (int64_t)buf->low[total_idx]) * get_packed_sign(buf_high, total_idx);
}



static inline int32_t get_averaged_value_int40(const struct SpectraIn* buf, int offset, int i, int Navgf, int shift_by, const uint32_t* buf_high)
{
    int64_t result;

    // to average over frequencies, we need additional shifts
    if (Navgf == 1) {
        result = get_packed_value(buf, buf_high, offset + i);
    } else if (Navgf == 2) {
        result = get_packed_value(buf, buf_high, offset + 2*i);
        result += get_packed_value(buf, buf_high, offset + 2*i + 1);
        shift_by += 1;
    } else if (Navgf == 3) {
        result = get_packed_value(buf, buf_high, offset + 4*i);
        result += get_packed_value(buf, buf_high, offset + 4*i + 1);
        result += get_packed_value(buf, buf_high, offset + 4*i + 2);
        shift_by += 2;
    } else if (Navgf == 4) {
        result = get_packed_value(buf, buf_high, offset + 4*i);
        result += get_packed_value(buf, buf_high, offset + 4*i + 1);
        result += get_packed_value(buf, buf_high, offset + 4*i + 2);
        result += get_packed_value(buf, buf_high, offset + 4*i + 3);
        shift_by += 2;
    }

    return (int32_t)(result >> shift_by);
}


#endif

static inline int32_t get_averaged_value_int32(const int32_t* buf, int offset, int i, int Navgf, int shift_by)
{
    int32_t result;

    // to average over frequencies, we need additional shifts
    if (Navgf == 1) {
        result = buf[offset + i];
    } else if (Navgf == 2) {
        result = buf[offset + 2*i] / 2;
        result += buf[offset + 2*i + 1] / 2;
    } else if (Navgf == 3) {
        result = buf[offset + 4*i] / 4;
        result = buf[offset + 4*i + 1] / 4;
        result = buf[offset + 4*i + 2] / 4;
    } else if (Navgf == 4) {
        result = buf[offset + 4*i] / 4;
        result = buf[offset + 4*i + 1] / 4;
        result = buf[offset + 4*i + 2] / 4;
        result = buf[offset + 4*i + 3] / 4;
    }

    return result;
}

static inline int32_t get_averaged_value_float(const void* _buf, int offset, int i, int Navgf, int shift_by)
{
    const float* buf = (float*)_buf;

    int64_t result;

    // to average over frequencies, we need additional shifts
    if (Navgf == 1) {
        result = (int64_t)buf[offset + i];
    } else if (Navgf == 2) {
        result = (int64_t)buf[offset + 2 * i];
        result += (int64_t)buf[offset + 2 * i + 1];
        shift_by += 1;
    } else if (Navgf == 3) {
        result = (int64_t)buf[offset + 4 * i];
        result += (int64_t)buf[offset + 4 * i + 1];
        result += (int64_t)buf[offset + 4 * i + 2];
        shift_by += 2;
    } else if (Navgf == 4) {
        result = (int64_t)buf[offset + 4 * i];
        result += (int64_t)buf[offset + 4 * i + 1];
        result += (int64_t)buf[offset + 4 * i + 2];
        result += (int64_t)buf[offset + 4 * i + 3];
        shift_by += 2;
    }

    return (int32_t)(result >> shift_by);
}

static inline int32_t get_averaged_value(const void* buf, int offset, int i, int Navgf, int shift_by, uint8_t averaging_mode, const uint32_t* buf_high)
{
    if (averaging_mode == AVG_INT32) {
        return get_averaged_value_int32(buf, offset, i, Navgf, shift_by);
    } else if (averaging_mode == AVG_INT_40_BITS) {
        return get_averaged_value_int40(buf, offset, i, Navgf, shift_by, buf_high);
    } else if (averaging_mode == AVG_FLOAT) {
        return get_averaged_value_float(buf, offset, i, Navgf, shift_by);
    }
}

#endif //LN_CORELOOP_HIGH_PREC_AVG_H
