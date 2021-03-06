// Originally from GraphCT, version 0.6.0
// License: GeorgiaTech

#define BILLION 1000000000
#define MILLION 1000000

inline double timer(void) {
#if defined(__MTA__)
	return((double)mta_get_clock(0) / mta_clock_freq());
#elif defined(__MACH__)
	static mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	uint64_t now = mach_absolute_time();
	now *= info.numer;
	now /= info.denom;
	return 1.0e-9 * (double)now;
#else
	struct timespec tp;
#if defined(CLOCK_PROCESS_CPUTIME_ID)
#define CLKID CLOCK_PROCESS_CPUTIME_ID
#elif  defined(CLOCK_REALTIME_ID)
#define CLKID CLOCK_REALTIME_ID
#endif
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return (double)tp.tv_sec + (double)tp.tv_nsec / BILLION;
#endif
}

inline uint64_t timer_ns(void) {
#if defined(__MTA__)
	return((double)mta_get_clock(0) / mta_clock_freq());
#elif defined(__MACH__)
	static mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	uint64_t now = mach_absolute_time();
	now *= info.numer;
	now /= info.denom;
	return 1.0e-9 * (double)now;
#else
	struct timespec tp;
#if defined(CLOCK_PROCESS_CPUTIME_ID)
#define CLKID CLOCK_PROCESS_CPUTIME_ID
#elif  defined(CLOCK_REALTIME_ID)
#define CLKID CLOCK_REALTIME_ID
#endif
	clock_gettime(CLKID, &tp);
	return (double)tp.tv_sec * 1 + (double)tp.tv_nsec;
#endif
}

#define TIME(timevar, what) \
do { double _start; _start = timer(); what; timevar = timer() - _start; } while (0)
