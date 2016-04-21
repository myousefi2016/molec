/*                   _
 *   _ __ ___   ___ | | ___  ___
 *  | '_ ` _ \ / _ \| |/ _ \/ __|
 *  | | | | | | (_) | |  __/ (__
 *  |_| |_| |_|\___/|_|\___|\___| - Molecular Dynamics Framework
 *
 *  Copyright (C) 2016  Carlo Del Don  (deldonc@student.ethz.ch)
 *                      Michel Breyer  (mbreyer@student.ethz.ch)
 *                      Florian Frei   (flofrei@student.ethz.ch)
 *                      Fabian Thuring (thfabian@student.ethz.ch)
 *
 *  This file is distributed under the MIT Open Source License.
 *  See LICENSE.txt for details.
 */

#ifndef MOLEC_TIMER_H
#define MOLEC_TIMER_H

#include <molec/Common.h>

#ifdef MOLEC_PLATFORM_WINDOWS
#include <intrin.h>
#else /* MOLEC_PLATFORM_POSIX */
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#endif

/**
 * Time stamp counter (TSC)
 */
typedef union {
    molec_uint64_t int64;
    struct
    {
        molec_uint32_t lo, hi;
    } int32;
} molec_TSC;
#define MOLEC_TSC_VAL(a) ((a).int64)

/**
 *
 */
typedef struct molec_Measurement_Node {
    molec_uint64_t value;
    struct molec_Measurement_Node* next;
} molec_Measurement_Node_t;

/**
 * @brief Count the number of cycles since last reset
 * @param cpu_c     molec_TSC to store the current tsc
 */
#ifdef MOLEC_PLATFORM_WINDOWS
#define MOLEC_RDTSC(cpu_c) (cpu_c).int64 = __rdtsc()
#else /* MOLEC_PLATFORM_POSIX */
#define MOLEC_RDTSC(cpu_c)                                                                         \
    MOLEC_ASM MOLEC_VOLATILE("rdtsc" : "=a"((cpu_c).int32.lo), "=d"((cpu_c).int32.hi))
#endif

/**
 * Query cpu-id (this is used to serialize the pipeline)
 */
#ifdef MOLEC_PLATFORM_WINDOWS
#define MOLEC_CPUID()                                                                              \
    {                                                                                              \
        int __cpuInfo__[4];                                                                        \
        __cpuid(__cpuInfo__, 0x0);                                                                 \
    }
#else /* MOLEC_PLATFORM_POSIX */
#define MOLEC_CPUID() MOLEC_ASM MOLEC_VOLATILE("cpuid" : : "a"(0) : "bx", "cx", "dx")
#endif

/**
 * Serialize the pipeline and query the TSC
 * @return 64-bit unsigned integer representing a tick count
 */
molec_uint64_t molec_start_tsc();

/**
 * @brief Stop the RDTSC timer and return diffrence from start (in cycles)
 *
 * @param start     64-bit unsigned integer representing a tick count
 * @return number of elapsed cycles since start
 */
molec_uint64_t molec_stop_tsc(molec_uint64_t start);

/**
 * Infrastructure to measure runtime using the Time Stamp Counter (TSC)
 *
 * To time an exection:
 * @code{.c}
 *     molec_measurement_init(2); // Number of independent timers
 *
 *     molec_measurement_start(0); // Start timer 0
 *
 *     for(int i = 0; i < 100; ++i)
 *     {
 *         molec_measurement_start(1); // Start timer 1
 *
 *         // Do something intresting ...
 *
 *         molec_measurement_stop(1); // Stop timer 1
 *     }
 *
 *     molec_measurement_start(0); // Stop timer 0
 *
 *
 *     printf("Meadian of elapsed cycles of 0: %llu\n", molec_measurement_get_median(0));
 * @endcode
 */
typedef struct molec_Measurement
{
    /** Measured runtimes (in cycles) */
    molec_Measurement_Node_t** value_list_heads;
    molec_Measurement_Node_t** value_list_tails;

    /** Number of concurrent timers */
    int num_timers;

    /** Number of measurements */
    int* num_measurements;

    /** Current tick count returned by molec_start_tsc() */
    molec_uint64_t* start;

} molec_Measurement_t;

/**
 * Start the measurement by allocating the molec_Measurement_t struct
 *
 * @param num_timers        Number of timers
 */
void molec_measurement_init(const int num_timers);

/**
 * Start the TSC
 *
 * @param timer_index   Index of the timer to start
 */
void molec_measurement_start(int timer_index);

/**
 * Stop the TSC and register the value
 *
 * @param timer_index   Index of the timer to stop
 */
void molec_measurement_stop(int timer_index) ;

/**
 * Compute the median of all measurements (in cycles) for timer Index
 *
 * @param timer_index   Index of the timer
 * @return meadian of all measurement of timer timer_index
 */
molec_uint64_t molec_measurement_get_median(int timer_index);

/**
 * @brief cleans the timing infrastructure
 */
void molec_measurement_finish();

#endif
