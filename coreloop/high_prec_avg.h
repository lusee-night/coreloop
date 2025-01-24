#ifndef LN_CORELOOP_HIGH_PREC_AVG_H
#define LN_CORELOOP_HIGH_PREC_AVG_H

#include <stdio.h>
#include <string.h>
#include "spectrometer_interface.h"

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

static inline int32_t get_averaged_value(const struct SpectraIn* buf, int offset, int i, int Navgf, int shift_by)
{
    return (int32_t)(buf->values[offset + i] >> shift_by);
}

#else

struct SpectraIn {
    int32_t low[NCHANNELS * NSPECTRA];      /* Lower 32 bits */
    int32_t high[NCHANNELS * NSPECTRA / 4]; /* Packed 8-bit values, 4 spectra per int32 */
};

/* Helper functions to work with packed high bits */
static inline int8_t get_high_bits(const struct SpectraIn* buf, int total_idx) {
    int pack_idx = total_idx >> 2;
    int shift = (total_idx & 3) * 8;
    return (int8_t)((buf->high[pack_idx] >> shift) & 0xFF);
}

static inline int64_t get_packed_value(const struct SpectraIn* buf, int total_idx) {
    return ((int64_t)get_high_bits(buf, total_idx) << 32) | (int64_t)buf->low[total_idx];
}

static inline int32_t get_averaged_value(const struct SpectraIn* buf, int offset, int i, int Navgf, int shift_by)
{
    int64_t result;

    // to average over frequencies, we need additional shifts
    if (Navgf == 1) {
        result = get_packed_value(buf, offset + i);
    } else if (Navgf == 2) {
        result = get_packed_value(buf, offset + 2*i);
        result += get_packed_value(buf, offset + 2*i + 1);
        shift_by += 1;
    } else if (Navgf == 3) {
        result = get_packed_value(buf, offset + 4*i);
        result += get_packed_value(buf, offset + 4*i + 1);
        result += get_packed_value(buf, offset + 4*i + 2);
        shift_by += 2;
    } else if (Navgf == 4) {
        result = get_packed_value(buf, offset + 4*i);
        result += get_packed_value(buf, offset + 4*i + 1);
        result += get_packed_value(buf, offset + 4*i + 2);
        result += get_packed_value(buf, offset + 4*i + 3);
        shift_by += 2;
    }

    return (int32_t)(result >> shift_by);
}

#endif

#endif //LN_CORELOOP_HIGH_PREC_AVG_H
