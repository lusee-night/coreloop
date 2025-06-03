#include "core_loop.h"
#include "interface_utils.h"


double generate_gaussian_variate() {
    double u1 = rand() / (double)RAND_MAX;
    double u2 = rand() / (double)RAND_MAX;
    double z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    return z;
}

void read_array_uint(const char* fname, uint32_t* data, int size)
{
    FILE* file = fopen(fname, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", data);
        return;
    }
    for (int i = 0; i < size; i++) {
        if (fscanf(file, "%i", &data[i]) != 1) {
            printf("Error reading from file: %s\n", fname);
            fclose(file);
            return;
        }
    }
    fclose(file);
}

void read_array_int(const char* fname, int32_t* data, int size)
{
    FILE* file = fopen(fname, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", data);
        return;
    }
    for (int i = 0; i < size; i++) {
        if (fscanf(file, "%i", &data[i]) != 1) {
            printf("Error reading from file: %s\n", fname);
            fclose(file);
            return;
        }
    }
    fclose(file);
}

void read_array_double(const char* fname, double* data, int size)
{
    FILE* file = fopen(fname, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", data);
        return;
    }
    for (int i = 0; i < size; i++) {
        if (fscanf(file, "%lf", &data[i]) != 1) {
            printf("Error reading from file: %s\n", fname);
            fclose(file);
            return;
        }
    }
    fclose(file);
}

long nanosecs_passed(struct timespec time_start)
{
    struct timespec time_now;
    clock_gettime(CLOCK_REALTIME, &time_now);
    long ns_passed = (time_now.tv_sec - time_start.tv_sec) * 1e9 + time_now.tv_nsec - time_start.tv_nsec;
    return ns_passed;
}
