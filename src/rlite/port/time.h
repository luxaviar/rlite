#pragma once

#ifdef _WIN32
#include <time.h>
#include <winsock2.h>

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

int gettimeofday(struct timeval* tv, struct timezone* tz);

#else

#include <sys/time.h>

#endif
