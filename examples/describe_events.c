#include <perfmon/pfmlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

void list_pmu_events(pfm_pmu_t pmu)
{
   pfm_event_info_t info;
   pfm_pmu_info_t pinfo;
   int i, ret;
   memset(&info, 0, sizeof(info));
   memset(&pinfo, 0, sizeof(pinfo));
   info.size = sizeof(info);
   pinfo.size = sizeof(pinfo);
   ret = pfm_get_pmu_info(pmu, &pinfo);
   if (ret != PFM_SUCCESS)
   {
      if(ret == PFM_ERR_NOINIT)
          errx(1,"noinit");
      errx(1, "cannot get pmu info: %d",ret);
   }
   for (i = pinfo.first_event; i != -1; i = pfm_get_event_next(i)) {
      ret = pfm_get_event_info(i,PFM_OS_NONE, &info);
      if (ret != PFM_SUCCESS)
        errx(1, "cannot get event info");
        printf("%s Event: %s::%s\n",
               pinfo.is_present ? "Active" : "Supported",
               pinfo.name, info.name);
  }
}

void main()
{
    pfm_initialize();
    list_pmu_events(PFM_PMU_ARM_CORTEX_A53);
}
