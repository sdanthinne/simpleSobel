#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <asm/types.h>
#include <linux/perf_event.h>
#include <string.h>
#include <err.h>

/**
 * Serves as a wrapper for the syscall that is perf_event_open.
 * Taken from the perf_event_open man page
 */
static long perf_event_open(struct perf_event_attr *hw_event, 
        pid_t pid, 
        int cpu, 
        int group_fd, 
        unsigned long flags)
{
    int ret;
    ret = syscall(__NR_perf_event_open, hw_event,pid,cpu,group_fd,flags);
    return ret;
}
/*
 * Starts the counting of the __u32 type specified. 
 * Pairs with perf_end_count
 */
void perf_start_count(__u32 type, int* fd)
{
    struct perf_event_attr pe;
    memset(&pe,0,sizeof(pe));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof (struct perf_event_attr);
    pe.config = type;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    *fd = perf_event_open(&pe,0,-1,-1,0);
    if(*fd==-1)
        errx(1,"error opening the perf event");
    ioctl(*fd,PERF_EVENT_IOC_RESET,0);
    ioctl(*fd,PERF_EVENT_IOC_ENABLE,0);
    //then, here we start counting the cpu cycles that are being used
}
/*
 * returns the result of the count, given the open FD.
 */
long perf_end_count(int * fd)
{
    //we will have to see how much the function call of this changes our result
    long long count;
    ioctl(*fd,PERF_EVENT_IOC_DISABLE,0);
    read(*fd,&count,sizeof(long long));
    close(*fd);
    return count;
}


