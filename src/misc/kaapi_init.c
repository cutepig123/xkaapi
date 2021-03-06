/*
** kaapi_init.c
** xkaapi
** 
**
** Copyright 2009,2010,2011,2012 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** fabien.lementec@imag.fr
** 
** This software is a computer program whose purpose is to execute
** multithreaded computation with data flow synchronization between
** threads.
** 
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software.  You can
** use, modify and/ or redistribute the software under the terms of
** the CeCILL-C license as circulated by CEA, CNRS and INRIA at the
** following URL "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to
** copy, modify and redistribute granted by the license, users are
** provided only with a limited warranty and the software's author,
** the holder of the economic rights, and the successive licensors
** have only limited liability.
** 
** In this respect, the user's attention is drawn to the risks
** associated with loading, using, modifying and/or developing or
** reproducing the software by the user in light of its specific
** status of free software, that may mean that it is complicated to
** manipulate, and that also therefore means that it is reserved for
** developers and experienced professionals having in-depth computer
** knowledge. Users are therefore encouraged to load and test the
** software's suitability as regards their requirements in conditions
** enabling the security of their systems and/or data to be ensured
** and, more generally, to use and operate it in the same conditions
** as regards security.
** 
** The fact that you are presently reading this means that you have
** had knowledge of the CeCILL-C license and that you accept its
** terms.
** 
*/
#include "kaapi_impl.h"
#include "kaapi_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if defined (_WIN32) || defined(__APPLE__)
#include <sys/sysctl.h>
#endif
#include <unistd.h>
#include <errno.h>

#if defined(KAAPI_USE_CUDA)
#include "machine/cuda/kaapi_cuda_impl.h"
#endif

/* == 0 if lib is not initialized
*/
static kaapi_atomic_t kaapi_count_init = {0};

/*
*/
kaapi_rtparam_t kaapi_default_param = {
   .startuptime = 0,
#if defined(KAAPI_USE_CUPTI)
   .cudastartuptime = 0,
#endif
#if defined(KAAPI_USE_CUDA)
   .cudawindowsize = 1,
   .cudapeertopeer = 0,
#endif
   .stacksize        = 64*4096, /**/
   .stacksize_master = 64*4096, /**/
   .cpucount    = 0,
   .gpucount    = 0,
   .kproc_list  = 0,
   .kid2cpu     = 0,
   .cpu2kid     = 0,
   .steal_by_affinity	= 0,
   .push_by_affinity	= 0,
   .ctpriority = 0,
   .eventmask   = KAAPI_EVT_MASK_COMPUTE|KAAPI_EVT_MASK_IDLE
};


/** \ingroup WS
    Initialize from xkaapi runtime parameters from command line
    \param argc [IN] command line argument count
    \param argv [IN] command line argument vector
    \retval 0 in case of success 
    \retval EINVAL because of error when parsing then KAAPI_CPUSET string
    \retval E2BIG because of a cpu index too high in KAAPI_CPUSET
*/
static int kaapi_setup_param()
{
  const char* wsselect;
  const char* emitsteal;
  const char* affinity;
  const char* stealaffinity;
  const char* use_ctpath;
  const char* displayperf;
    
  /* compute the number of cpu of the system */
#if defined(__linux__)
  kaapi_default_param.syscpucount = sysconf(_SC_NPROCESSORS_CONF);
#elif defined(__APPLE__)
  {
    int mib[2];
    size_t len;
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(kaapi_default_param.syscpucount);
    sysctl(mib, 2, &kaapi_default_param.syscpucount, &len, 0, 0);
  }
#elif defined(_WIN32)
  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  kaapi_default_param.syscpucount = sys_info.dwNumberOfProcessors;
#else
#  warning "Could not compute number of physical cpu of the system. Default value==1"
  kaapi_default_param.syscpucount = 1;
#endif
  /* adjust system limit, if library is compiled with greather number of processors that available */
  if (kaapi_default_param.syscpucount > KAAPI_MAX_PROCESSOR_LIMIT)
    kaapi_default_param.syscpucount = KAAPI_MAX_PROCESSOR_LIMIT;

  kaapi_default_param.use_affinity = 0;

  kaapi_default_param.cpucount  = kaapi_default_param.syscpucount;
  
  displayperf = getenv("KAAPI_DISPLAY_PERF");
  if (displayperf !=0)
  {
    if ((strcmp(displayperf, "no") == 0) || (strcmp(displayperf, "0") == 0))
      kaapi_default_param.display_perfcounter = KAAPI_NO_DISPLAY_PERF;
    else if ((strcmp(displayperf, "full") == 0) || (strcmp(displayperf, "1") == 0))
      kaapi_default_param.display_perfcounter = KAAPI_DISPLAY_PERF_FULL;
    else if (strcmp(displayperf, "resume") == 0)
      kaapi_default_param.display_perfcounter = KAAPI_DISPLAY_PERF_RESUME;
    else {
      fprintf(stderr, "***Kaapi: bad value for variable KAAPI_DISPLAY_PERF. Use 'resume'\n");
      kaapi_default_param.display_perfcounter = KAAPI_DISPLAY_PERF_RESUME;
    }
  } else
    kaapi_default_param.display_perfcounter = KAAPI_NO_DISPLAY_PERF;

  if (getenv("KAAPI_STACKSIZE") !=0)
    kaapi_default_param.stacksize = atoll(getenv("KAAPI_STACKSIZE"));

  if (getenv("KAAPI_STACKSIZE_MASTER") !=0)
    kaapi_default_param.stacksize_master = atoll(getenv("KAAPI_STACKSIZE_MASTER"));
  else
    kaapi_default_param.stacksize_master = kaapi_default_param.stacksize;

  /* workstealing selection function */
  wsselect = getenv("KAAPI_WSSELECT");
  kaapi_default_param.wsselect = &kaapi_sched_select_victim_rand;
  if (wsselect !=0)
  {
    if (strcmp(wsselect, "rand") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_rand;
    else if (strcmp(wsselect, "workload") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_workload_rand;
    else if (strcmp(wsselect, "first0") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_rand_first0;
    else if (strcmp(wsselect, "hierarchical") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_hierarchy;
    else if (strcmp(wsselect, "hws") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_hwsn;
    else if (strcmp(wsselect, "pws") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_pws;
    else if (strcmp(wsselect, "aff") ==0)
      kaapi_default_param.wsselect = &kaapi_sched_select_victim_aff;

    else {
      fprintf(stderr, "***Kaapi: bad value for variable KAAPI_WSSELECT\n");
      return EINVAL;
    }
  }

  kaapi_default_param.emitsteal          = kaapi_sched_flat_emitsteal;
  kaapi_default_param.emitsteal_initctxt = kaapi_sched_flat_emitsteal_init;
  emitsteal = getenv("KAAPI_EMITSTEAL");
  if (emitsteal != NULL)
  {
#if 0
#if KAAPI_USE_HWLOC
    if (strcmp(emitsteal, "hws") == 0)
    {
      kaapi_default_param.emitsteal          = kaapi_hws_emitsteal;
      kaapi_default_param.emitsteal_initctxt = kaapi_hws_emitsteal_init;
    }
    else
#endif /* KAAPI_USE_HWLOC */
#endif
    if (strcmp(emitsteal, "flat") == 0)
    {
      kaapi_default_param.emitsteal          = kaapi_sched_flat_emitsteal;
      kaapi_default_param.emitsteal_initctxt = kaapi_sched_flat_emitsteal_init;
    }
    else {
      fprintf(stderr, "***Kaapi: bad value for variable KAAPI_EMITSTEAL\n");
      return EINVAL;
    }
  }
  
  /* event mask */
#if defined(KAAPI_USE_PERFCOUNTER)
  if (getenv("KAAPI_RECORD_TRACE") !=0)
  {
    if (getenv("KAAPI_RECORD_MASK") !=0)
    {
      /* actual grammar:
         eventno[,eventno]*
         eventno is an integer less than 2^sizeof(kaapi_event_mask_type_t)
         grammar must be more complex using predefined set
      */
      uint64_t mask = 0;
      int err = kaapi_util_parse_list( &mask, getenv("KAAPI_RECORD_MASK"), ',',
         3,
           "COMPUTE", (uint64_t)KAAPI_EVT_MASK_COMPUTE,
           "IDLE",    (uint64_t)KAAPI_EVT_MASK_IDLE,
           "STEAL",   (uint64_t)KAAPI_EVT_MASK_STEALOP
      );
      if (err !=0)
      {
        fprintf(stderr, "*** Kaapi: mal formed mask list 'KAAPI_RECORD_MASK': '%s'\n",
          getenv("KAAPI_RECORD_MASK")
        );
        return EINVAL;
      }
      /* always add startup set */
      kaapi_default_param.eventmask = mask;
    }
    /* always add startup set */
    kaapi_default_param.eventmask |= KAAPI_EVT_MASK_STARTUP;
  }
#endif  

  affinity = getenv("KAAPI_PUSH_AFFINITY");
  kaapi_default_param.push_by_affinity = &kaapi_push_by_affinity_default;
  if( affinity != 0 ) 
  {
    if (strcmp(affinity, "rand") ==0)
      kaapi_default_param.push_by_affinity = &kaapi_push_by_affinity_rand;
    else if (strcmp(affinity, "locality") ==0)
      kaapi_default_param.push_by_affinity = &kaapi_push_by_affinity_locality;
    else if (strcmp(affinity, "writer") ==0)
      kaapi_default_param.push_by_affinity = &kaapi_push_by_affinity_writer;
    else {
      fprintf(stderr, "*** Kaapi: bad value for 'KAAPI_PUSH_AFFINITY': '%s'\n",
        getenv("KAAPI_AFFINITY")
      );
      return EINVAL;
    }
  }

  stealaffinity = getenv("KAAPI_STEAL_AFFINITY");
  kaapi_default_param.steal_by_affinity = &kaapi_steal_by_affinity_first;
  if( stealaffinity != 0 ) 
  {
    if (strcmp(stealaffinity, "cp") ==0)
      kaapi_default_param.steal_by_affinity = &kaapi_steal_by_affinity_maxctpath;
    else if (strcmp(stealaffinity, "locality") ==0)
      kaapi_default_param.steal_by_affinity = &kaapi_steal_by_affinity_maxhit;
    else if (strcmp(stealaffinity, "writer") ==0)
      kaapi_default_param.steal_by_affinity = &kaapi_steal_by_affinity_writer;
    else if (strcmp(stealaffinity, "1") ==0)
      kaapi_default_param.steal_by_affinity = &kaapi_steal_by_affinity_first;
    else {
      fprintf(stderr, "*** Kaapi: bad value for 'KAAPI_STEAL_AFFINITY': '%s'\n",
        stealaffinity
      );
      return EINVAL;
    }
  }

  use_ctpath = getenv("KAAPI_CTPATH_PRIO");
  if( use_ctpath != 0 ) 
  {
    if ((strcmp(use_ctpath, "identity") ==0) || (strcmp(use_ctpath, "1") ==0))
      kaapi_default_param.ctpriority = &kaapi_ctpath2prio_identity;
    else if (strcmp(use_ctpath, "max") ==0)
      kaapi_default_param.ctpriority = &kaapi_ctpath2prio_max;
    else if (strcmp(use_ctpath, "linear") ==0) 
      kaapi_default_param.ctpriority = &kaapi_ctpath2prio_linear;
    else {
      fprintf(stderr, "***Kaapi: bad value for 'KAAPI_CTPATH_PRIO': '%s'\n",
        getenv("KAAPI_CTPATH_PRIO")
      );
      return EINVAL;
    }
  }

#if defined(KAAPI_USE_CUDA)
  /* change of name */
  if (getenv("KAAPI_CUDA_WINDOW_SIZE") !=0 ) {
      kaapi_default_param.cudawindowsize = atoll(getenv("KAAPI_CUDA_WINDOW_SIZE"));
  } else if (getenv("KAAPI_WINDOW_SIZE") !=0 ) {
    /* compatibility */
    kaapi_default_param.cudawindowsize = atoll(getenv("KAAPI_WINDOW_SIZE"));    
    fprintf(stderr, "%s:%d:%s: *** Kaapi: WARNING '%s' is deprecated, use '%s' instead.\n",
            __FILE__, __LINE__, __FUNCTION__, "KAAPI_WINDOW_SIZE", "KAAPI_CUDA_WINDOW_SIZE"
            );
    fflush(stderr);
  }
  
  if (getenv("KAAPI_CUDA_PEER") !=0 ) {
    kaapi_default_param.cudapeertopeer = 1;
  }
#endif
  
  return 0;
}



/**
*/
int kaapi_init(int flag, int* argc, char*** argv)
{
  if (KAAPI_ATOMIC_INCR(&kaapi_count_init) !=1) 
    return EALREADY;

  kaapi_format_init();
  kaapi_init_adaptive();
  
  /* set up runtime parameters */
  kaapi_assert_m( 0 == kaapi_setup_param(), "kaapi_setup_param" );
  
#if defined(KAAPI_USE_NETWORK)
  kaapi_network_init(argc, argv);
#endif

  kaapi_memory_init();
  int err = kaapi_mt_init();

#if defined(KAAPI_USE_CUDA)
  kaapi_cuda_init();
#endif

  if (flag)
    kaapi_begin_parallel(KAAPI_SCHEDFLAG_DEFAULT);
  return err;
}


/**
*/
int kaapi_finalize(void)
{
  kaapi_sched_sync();
  if (KAAPI_ATOMIC_DECR(&kaapi_count_init) !=0) 
    return EAGAIN;

  kaapi_memory_destroy();
  
  kaapi_mt_finalize();
  
  /* reclaim format' names allocations */
  kaapi_format_finalize();

#if defined(KAAPI_USE_NETWORK)
  kaapi_network_finalize();
#endif

#if defined(KAAPI_USE_CUDA)
//TODO:  kaapi_cuda_finalize();
#endif

  return 0;
}


/* Counter of enclosed parallel/begin calls.
*/
static kaapi_atomic_t kaapi_parallel_stack = {0};

/** begin parallel & and parallel
    - it should not have any concurrency on the first increment
    because only the main thread is running before parallel computation
    - after that serveral threads may declare parallel region that
    will implies concurrency
*/
void kaapi_begin_parallel( int schedflag )
{
  if (schedflag & KAAPI_SCHEDFLAG_STATIC)
  {
    kaapi_thread_set_unstealable(1);
  }
  /* if not static then wakeup thread here */
  else if (KAAPI_ATOMIC_INCR(&kaapi_parallel_stack) == 1)
  {
    kaapi_mt_resume_threads();
  }
}


/**
*/
void kaapi_end_parallel(int flag)
{
  if (flag & KAAPI_SCHEDFLAG_STATIC) 
  { /* end of the static parallel region: compute readylist + 
       set thread stealable + resume thread if 1rst // region*/
    kaapi_sched_computereadylist();
    kaapi_thread_set_unstealable(0);
    if (KAAPI_ATOMIC_INCR(&kaapi_parallel_stack) == 1)
      kaapi_mt_resume_threads();
  }
  
  if ((flag & KAAPI_SCHEDFLAG_NOWAIT) ==0) 
    kaapi_sched_sync();

  if (KAAPI_ATOMIC_DECR(&kaapi_parallel_stack) == 0)
  {
    kaapi_mt_suspend_threads_post();
  }
}

