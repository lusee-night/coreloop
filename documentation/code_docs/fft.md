## fft.c

This file implements FFT (Fast Fourier Transform) functionality with both fixed-point and floating-point variants. Let me go through each function:

1. `my_sin` - Taylor series sine computation (not used in production)
2. `get_sine_int` - Retrieves sine values from fixed-point lookup table
3. `get_cosine_int` - Retrieves cosine values (offset by 90 degrees)
4. `get_sine_float` - Retrieves sine values from floating-point lookup table
5. `get_cosine_float` - Retrieves cosine values (offset by 90 degrees)
6. `get_exp_complex` - Complex exponential (not used on real hardware)
7. `print_binary` - Debug function to print binary representation
8. `reverse_bits` - Bit reversal for FFT permutation
9. `fft_int_in_place` - In-place fixed-point FFT
10. `fft_int` - Fixed-point FFT with separate input/output buffers
11. `fft_int_multiple` - Batch processing of multiple fixed-point FFTs
12. `fft_float` - FFT with fixed-point input and floating-point output
13. `fft_float_multiple` - Batch processing with float output

### `my_sin(float x)`
Computes the sine of a value using Taylor series expansion, approximating sin(x) using the first 5 terms. This function is not used in production as sine values are pre-computed in lookup tables for performance.

### `get_sine_int(int index)`
Retrieves a sine value from the fixed-point integer lookup table. The index is automatically wrapped to the valid range [0, FFT_SIZE-1] using bitwise AND with (FFT_SIZE - 1). Returns the sine value as a fixed-point int32_t scaled by FIXED_POINT_SCALE/2.

### `get_cosine_int(int index)`
Retrieves a cosine value from the fixed-point integer lookup table by adding TABLE_SIZE/4 to the index, implementing the 90-degree phase shift relationship between sine and cosine.

### `get_sine_float(int index)`
Retrieves a sine value from the floating-point lookup table with automatic index wrapping to ensure the index stays within valid bounds.

### `get_cosine_float(int index)`
Retrieves a cosine value from the floating-point lookup table using the same 90-degree offset technique as the integer version.

### `get_exp_complex(int index)`
Computes a complex exponential e^(i*theta) returning cos(theta) + i*sin(theta). This function is not used on real hardware due to complex number support limitations.

### `print_binary(uint8_t n)`
Debug utility that prints the binary representation of an 8-bit value as 32 bits (with leading zeros) to stdout for visualization during development.

### `reverse_bits(uint8_t index)`
Performs bit reversal on an 8-bit index for the FFT bit-reversal permutation step. The function swaps bits in pairs, then nibbles, then bytes, and finally right-shifts by (8 - FFT_BIT_SIZE) to adjust for the actual FFT size being used.

### `fft_int_in_place(int32_t *real_ptr, int32_t *imag_ptr)`
Performs an in-place FFT using fixed-point integer arithmetic. First applies bit-reversal permutation by swapping elements according to reversed indices. Then executes the Cooley-Tukey FFT algorithm through log2(FFT_SIZE) stages, computing butterfly operations with twiddle factors from the lookup tables. Each butterfly operation includes a right shift by 1 to prevent overflow. The function modifies the input arrays directly, replacing them with the frequency-domain representation.

### `fft_int(int32_t *input_real, int32_t *input_imag, int32_t *output_real, int32_t *output_imag)`
Performs FFT with separate input and output buffers using fixed-point arithmetic. Copies input to output buffers, then applies the same bit-reversal and Cooley-Tukey algorithm as the in-place version. This variant is useful when the original time-domain data must be preserved. The scaling at each butterfly stage prevents overflow while maintaining precision.

### `fft_int_multiple(int32_t* input_real_beg, int32_t* input_imag_beg, int32_t* output_real_beg, int32_t* output_imag_beg)`
Processes NUM_FFTS_IN_ONE_GO separate FFTs in batch mode for improved performance. The input and output arrays are interleaved with each FFT's data stored at offsets of FFT_SIZE. The function optimizes computation by calculating twiddle factors once per stage and applying them to all FFTs in the batch. This approach significantly reduces redundant trigonometric lookups and improves cache utilization when processing multiple channels simultaneously.

### `fft_float(int32_t *input_real, int32_t *input_imag, float* output_real, float* output_imag)`
Performs FFT converting fixed-point input to floating-point output for higher precision analysis. During the initial copy, applies bit-reversal permutation while converting int32_t values to float. The Cooley-Tukey algorithm then operates entirely in floating-point without the scaling needed in fixed-point versions, providing better dynamic range for subsequent processing stages that require high precision.

### `fft_float_multiple(int32_t* input_real_beg, int32_t* input_imag_beg, float* output_real_beg, float* output_imag_beg)`
Batch processes NUM_FFTS_IN_ONE_GO FFTs converting from fixed-point input to floating-point output. Combines the efficiency of batch processing with the precision of floating-point arithmetic. The bit-reversal is performed during the type conversion phase, and twiddle factors are computed once per stage and shared across all FFTs. This function is particularly useful for calibration and high-precision spectral analysis where multiple channels need simultaneous processing.

