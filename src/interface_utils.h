#ifndef LN_CORELOOP_INTERFACE_UTILS_H
#define LN_CORELOOP_INTERFACE_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// read size entries from fname into data array
void read_array_uint(const char* fname, uint32_t* data, int size);
void read_array_int(const char* fname, int32_t* data, int size);
void read_array_double(const char* fname, double* data, int size);

// read number of entries from the 1st line of fname
// allocate memory for data
// return number of entries
int read_dynamic_array(const char* fname, int32_t** data);

extern char user_spectrum_filename[2048];

double generate_gaussian_variate();

long nanosecs_passed(struct timespec time_start);
#endif //LN_CORELOOP_INTERFACE_UTILS_H
