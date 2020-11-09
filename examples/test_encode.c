#include <inttypes.h>
#include <err.h>
#include <perfmon/pfmlib.h>
#include <string.h>
#include <stdlib.h>
//#include <uapi/linux/perf_event.h>
int main(int argc, char **argv)
{
   pfm_pmu_encode_arg_t raw;
   int ret;
   char * fstr = NULL;
   ret = pfm_initialize();
   if (ret != PFM_SUCCESS)
      errx(1, "cannot initialize library %s", pfm_strerror(ret));
   memset(&raw, 0, sizeof(raw));
   raw.fstr = &fstr;
   ret = pfm_get_os_event_encoding("PERF_COUNT_HW_INSTRUCTIONS", PFM_PLM0, PFM_OS_NONE, &raw);
   if (ret != PFM_SUCCESS)
      err(1," cannot get encoding %s", pfm_strerror(ret));
   for(int i=0; i < raw.count; i++)
      printf("count[%d]=0x%" PRIx64 "\n", i, raw.codes[i]);
   printf("%s\n",fstr);
   free(raw.codes);
   return 0;
}
