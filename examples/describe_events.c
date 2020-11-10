#include <perfmon/pfmlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <syscall.h>
#include <perfmon.h>

/*
 *So important things:
 * /usr/include/linux/perf_event.h < contains necessary structs for hte perf_event_open syscall
 * so we need to look at the perf_event_open syscall - not even sure if it is neccary to use libpfm?
 *
 * */
void list_pmu_events(pfm_pmu_t pmu)
{
   pfm_event_info_t info;
   pfm_pmu_info_t pinfo;
   pfm_event_attr_info_t info_s_event;

   int i,j, ret;
   pfarc_context_t ctxt;
   memset(&ctxt,0,sizeof(ctxt));
   long my_cmd = perfmonctl(0,PFM_CREATE_CONTEXT, ctxt,1);   

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
      for(j=0;j<info.nattrs;j++)
      {
        if(pfm_get_event_attr_info(i,j,PFM_OS_NONE,&info_s_event)!= PFM_SUCCESS)
        {
            errx(1,"cannot get info attributes");
        }
        printf(" attr: %s : %s : %s\n",info.name,info_s_event.name,info_s_event.desc);
      }
      
      printf("%s Event: %s::%s : %d\n",
               pinfo.is_present ? "Active" : "Supported",
               pinfo.name, info.name,info.nattrs);
  }
}

void main()
{
    pfm_initialize();
    list_pmu_events(PFM_PMU_ARM_CORTEX_A53);
    //list_pmu_events(PFM_PMU_ARM_CORTEX_A53);
}
