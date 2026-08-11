#include <intrin.h>
#include "common.h"

static u64
GetOsTimerFreq(void)
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    return(Freq.QuadPart);
}

static u64
ReadOsTimer(void)
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    return(Value.QuadPart);
}

static u64
ReadCpuTimer(void)
{
    return __rdtsc();
}

static u64
GuessCpuFreq(u64 OsFreq, u64 OsElapsed, u64 CpuElapsed)
{
    u64 Result = 0;
    if(OsElapsed)
    {
        Result = CpuElapsed * OsFreq / OsElapsed;
    }
    return(Result);
}
