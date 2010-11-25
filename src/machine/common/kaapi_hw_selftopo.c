/*
 ** 
 ** Created on Jun 23 2010
 ** Copyright 2009 INRIA.
 **
 ** Contributors :
 **
 ** thierry.gautier@inrialpes.fr
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
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <unistd.h>
#include <sys/syscall.h>

#include "kaapi_impl.h"

/** Initialize the topology information from each thread
 Update kprocessor data structure only if kaapi_default_param.memory
 has been initialized by the kaapi_hw_init function.
 If hw cannot detect topology nothing is done.
 If KAAPI_CPUSET was not set but the topology is available, then the
 function will get the physical CPU on which the thread is running.
 */
int kaapi_processor_computetopo(kaapi_processor_t* kproc)
{
#if !defined(__linux__)
  return 0;
#else
  int	pid;
  char comm[64];
  char	state;
  int	ppid;
  int	pgrp;
  int	session;
  int	tty_nr;
  int	tpgid;
  unsigned int	flags;
  unsigned long int	minflt;
  unsigned long int	cminflt;
  unsigned long int	majflt;
  unsigned long int	cmajflt;
  long int	utime;
  long int	stime;
  long int	cutime;
  long int	cstime;
  long int	priority;
  long int	nice;
  long int	num_threads;
  unsigned long int	itrealvalue;
  unsigned long long int	starttime;
  unsigned long int	vsize;
  unsigned long int	rss;
  unsigned long int	rsslim;
  unsigned long int	startcode;
  unsigned long int	endcode;
  unsigned long int	startstack;
  unsigned long int	kstkesp;
  unsigned long int	kstkeip;
  long int	signal;
  long int	blocked;
  long int	sigignore;
  long int	sigcatch;
  unsigned long int	wchan;
  unsigned long int	nswap;
  unsigned long int	cnswap;
  int	exit_signal;
  int	processor;
  unsigned int	rt_priority;
  unsigned int	policy;
  unsigned long long int	delayacct_blkio_ticks;
  unsigned long int	guest_time;
  long int	cguest_time;
  
  char filename[256];
  FILE* file;
  int depth;
  
  if (kaapi_default_param.memory.depth == 0) 
    return ENOENT;
  if (kaapi_default_param.use_affinity ==0)
    return ENOENT;
  
  sprintf(filename, "/proc/%i/task/%i/stat", getpid(), gettid());
  file = fopen(filename, "rt");
  fscanf(file,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %ld %lu %llu %lu %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %lu %lu %lu %d %d %u %u %llu %lu %ld",
         &pid,
         comm,
         &state,
         &ppid,
         &pgrp,
         &session,
         &tty_nr,
         &tpgid,
         &flags,
         &minflt,
         &cminflt,
         &majflt,
         &cmajflt,
         &utime,
         &stime,
         &cutime,
         &cstime,
         &priority,
         &nice,
         &num_threads,
         &itrealvalue,
         &starttime,
         &vsize,
         &rss,
         &rsslim,
         &startcode,
         &endcode,
         &startstack,
         &kstkesp,
         &kstkeip,
         &signal,
         &blocked,
         &sigignore,
         &sigcatch,
         &wchan,
         &nswap,
         &cnswap,
         &exit_signal,
         &processor,
         &rt_priority,
         &policy,
         &delayacct_blkio_ticks,
         &guest_time,
         &cguest_time
  );
  
  /* test if processor has been read */
  if (err < 39)
  {
    return ESRCH;
  }
  
  /* ok: here we have the current running processor :
    - we recompute the stack of affinity_set from the memory hierarchy
  */
  kproc->cpuid = processor;
  kaapi_assert_m(kaapi_default_param.memory.depth <ENCORE_UNE_MACRO_DETAILLE, "To increase..." );

  kproc->hlevel.depth == 0;
  for (depth=0; depth < kaapi_default_param.memory.depth; ++depth)
  {
    for (i=0; i< kaapi_default_param.memory.levels[depth].count; ++i)
    {
      if (kaapi_cpuset_has(kaapi_default_param.memory.levels[depth].affinity[i].who, kproc->cpuid))
      {
        kproc->hlevel.levels[depth] = kaapi_default_param.memory.levels[depth].affinity[i];
        break;
      }
    }
  }
  kproc->hlevel.depth = depth;
  
  for (depth = 0; depth <kproc->hlevel.depth; ++depth)
  {
    const char* str = kaapi_cpuset2string(kaapi_default_param.syscpucount, kproc->hlevel.levels[depth]->who);
    printf("[size:%lu, cpuset:'%s', %lu, type:%u]   ", 
    (unsigned long)kproc->hlevel.levels[depth]->mem_size,
    str, strlen(str),
    (unsigned int)kproc->hlevel.levels[depth]->type
    );
  }

  return 0;
#endif
}
