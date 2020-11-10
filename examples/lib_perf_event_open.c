#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

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
