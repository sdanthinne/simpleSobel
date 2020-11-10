#ifndef LIB_PERF_EVENT_OPEN_H
#define LIB_PERF_EVENT_OPEN_H
#include <asm/types.h>
void perf_start_count(__u32 type, int*fd);
long perf_end_count(int * fd)

#endif
