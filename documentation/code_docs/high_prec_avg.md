## high_prec_avg.h

Header file with inline functions for high-precision averaging:
- Helper functions for packed high bits manipulation
- Functions to get averaged values in different modes (int32, int40, float)


### `get_high_bits(const uint32_t* buf_high, int total_idx)`
Extracts the packed high-order bits (7 bits) for a specific channel from the compressed storage format.

### `get_packed_sign(const uint32_t* buf_high, int total_idx)`
Determines the sign of a value from the packed sign bit.

### `is_negative(const uint32_t* buf_high, int total_idx)`
Checks if a packed value is negative.

### `get_packed_value(const uint32_t* buf, const uint32_t* buf_high, int total_idx)`
Reconstructs a full 40-bit signed integer from the packed representation.

### `get_averaged_value_int40(const uint32_t* buf, int offset, int i, int Navgf, int shift_by, const uint32_t* buf_high)`
Performs frequency averaging for 40-bit integer mode, summing the appropriate number of adjacent frequency bins and applying the specified shift.

### `get_averaged_value_int32(const int32_t* buf, int offset, int i, int Navgf)`
Performs frequency averaging for standard 32-bit integer mode.

### `get_averaged_value_float(const void* _buf, int offset, int i, int Navgf, int shift_by)`
Performs frequency averaging for floating-point mode, converting from float to integer with shifting.

### `get_averaged_value(const void* buf, int offset, int i, int Navgf, int shift_by, uint8_t averaging_mode, const uint32_t* buf_high)`
Dispatcher function that calls the appropriate averaging implementation based on the configured averaging mode.

