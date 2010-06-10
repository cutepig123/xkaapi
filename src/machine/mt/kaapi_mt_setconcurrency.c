/*
** kaapi_mt_setconcurrency
** 
** Created on Tue Mar 31 15:17:57 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
** christophe.laferriere@imag.fr
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
#include "kaapi_impl.h"

static void* kaapi_sched_run_processor( void* arg );

/**
*/
static kaapi_atomic_t barrier_init = {0};

/**
*/
static kaapi_atomic_t barrier_init2 = {0};

#if HAVE_NUMA_H

#include <numa.h>
#include <numaif.h>

static inline kaapi_processor_t* allocate_kproc(void)
{
  kaapi_processor_t* const kproc =
    numa_alloc_local(sizeof(kaapi_processor_t));
  if (kproc == NULL)
    return NULL;

  memset(kproc, 0, sizeof(kaapi_processor_t));

  return kproc;
}

#else /* ! HAVE_NUMA_H */

static inline kaapi_processor_t* allocate_kproc(void)
{
  return calloc(sizeof(kaapi_processor_t), 1);
}

#endif /* HAVE_NUMA_H */

/** Create and initialize and start concurrency kernel thread to execute user threads
    TODO: faire une implementation dynamique
      - si appel dans un thread kaapi (kaapi_get_current_processor() !=0)
      stopper tous les threads -1
      - sinon stopper tous les threads kaapi.
      - si ajout -> simple
      - si retrait -> a) simple: stopper le thread, lui mettre tout son travail dans le thread main.
                      b) + compliquer: prendre le travail des threads à stopper + signaler (kill kaapi) à se terminer
                      (ou amortir une creation ultérieur des threads en les mettant en attente de se terminer apres un timeout)
      - proteger l'appel d'appel concurrents.
*/
int kaapi_setconcurrency( unsigned int concurrency )
{
  static int isinit = 0;
  pthread_attr_t attr;
  pthread_t tid;
  int i;
    
  if (concurrency <1) return EINVAL;
  if (concurrency > KAAPI_MAX_PROCESSOR) return EINVAL;

  if (isinit) return EINVAL;
  isinit = 1;
  
  /* */
  kaapi_all_kprocessors = calloc( (kaapi_uint32_t)concurrency, sizeof(kaapi_processor_t*) );
  if (kaapi_all_kprocessors ==0) return ENOMEM;

  /* default processor number */
  kaapi_count_kprocessors = concurrency;

  kaapi_barrier_td_init( &barrier_init, 0);
  kaapi_barrier_td_init( &barrier_init2, 1);

  pthread_attr_init(&attr);
      
  /* TODO: allocate each kaapi_processor_t of the selected numa node if it exist */
  for (i=0; i<kaapi_count_kprocessors; ++i)
  {
    if (i>0)
    {
      kaapi_barrier_td_setactive(&barrier_init, 1);

#ifdef KAAPI_USE_SCHED_AFFINITY
      if (kaapi_default_param.use_affinity)
      {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(kaapi_default_param.kid_to_cpu[i], &cpuset);
        kaapi_assert_m((!pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset)), "pthread_attr_setaffinity_np");
        sched_yield();
      }
#endif /* KAAPI_USE_SCHED_AFFINITY */

      if (EAGAIN == pthread_create(&tid, &attr, &kaapi_sched_run_processor, (void*)(long)i))
      {
        kaapi_count_kprocessors = i;
        kaapi_barrier_td_setactive(&barrier_init, 0);
        pthread_attr_destroy(&attr);
        return EAGAIN;
      }
    }
    else 
    {
#ifdef KAAPI_USE_SCHED_AFFINITY
      if (kaapi_default_param.use_affinity)
      {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(kaapi_default_param.kid_to_cpu[i], &cpuset);
        kaapi_assert_m((!pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset)), "pthread_attr_setaffinity_np");
        sched_yield();
      }
#endif /* KAAPI_USE_SCHED_AFFINITY */

      kaapi_all_kprocessors[i] = allocate_kproc();
      kaapi_assert(0 == pthread_setspecific( kaapi_current_processor_key, kaapi_all_kprocessors[0] ) );

      if (kaapi_all_kprocessors[i] ==0) 
      {
        pthread_attr_destroy(&attr);
        free(kaapi_all_kprocessors);
        kaapi_all_kprocessors = 0;
        return ENOMEM;
      }
      kaapi_assert( 0 == kaapi_processor_init( kaapi_all_kprocessors[i] ) );
      kaapi_all_kprocessors[i]->kid = 0;

      /* Initialize the hierarchy information and data structure */
      kaapi_assert( 0 == kaapi_processor_setuphierarchy( kaapi_all_kprocessors[i] ) );

#if defined(KAAPI_USE_PERFCOUNTER)
      /*  */
      kaapi_perf_thread_init(kaapi_all_kprocessors[i], KAAPI_PERF_USER_STATE);
#endif

      /* register the processor */
      kaapi_barrier_td_setactive(&kaapi_term_barrier, 1);
    }
  }

  pthread_attr_destroy(&attr);

  /* wait end of the initialization */
  kaapi_barrier_td_waitterminated( &barrier_init );

  /* here is the number of correctly initialized processor, may be less than requested */
  kaapi_count_kprocessors = KAAPI_ATOMIC_READ( &kaapi_term_barrier );
    
  /* Initialize the hierarchy information and data structure: AFTER kaapi_count_kprocessors is known  */
  kaapi_processor_setuphierarchy( kaapi_all_kprocessors[0] );

  /* broadcast to all threads that they have been started */
  kaapi_barrier_td_setactive(&barrier_init2, 0);
  
  kaapi_barrier_td_destroy( &barrier_init );    

#if defined(KAAPI_USE_PERFCOUNTER)
  /*  */
  kaapi_perf_thread_start(kaapi_all_kprocessors[0]);
#endif
  return 0;
}

/**
*/
void* kaapi_sched_run_processor( void* arg )
{
  kaapi_processor_t* kproc =0;
  int kid = (long)arg;
  
  /* force reschedule of the posix thread, we that the thread will be mapped on the correct processor ? */
  sched_yield();

  kproc = kaapi_all_kprocessors[kid] = allocate_kproc();

  if (kproc ==0) {
    kaapi_barrier_td_setactive(&barrier_init, 0);
    return 0;
  }
  kaapi_assert( 0 == pthread_setspecific( kaapi_current_processor_key, kproc ) );
  kaapi_assert( 0 == kaapi_processor_init( kproc ) );
  kproc->kid = kid;

#if defined(KAAPI_USE_PERFCOUNTER)
  /*  */
  kaapi_perf_thread_init(kproc, KAAPI_PERF_SCHEDULE_STATE);
#endif

  /* kprocessor correctly initialize */
  kaapi_barrier_td_setactive(&kaapi_term_barrier, 1);

  /* quit first steap of the initialization */
  kaapi_barrier_td_setactive(&barrier_init, 0);

  /* Initialize the hierarchy information and data structure */
  kaapi_processor_setuphierarchy( kproc );
  
  /* wait end of the initialization */
  kaapi_barrier_td_waitterminated( &barrier_init2 );
  
#if defined(KAAPI_USE_PERFCOUNTER)
  /*  */
  kaapi_perf_thread_start(kproc);
#endif

  /* main work stealing loop */
  kaapi_sched_idle( kproc );

#if defined(KAAPI_USE_PERFCOUNTER)
  /*  */
  kaapi_perf_thread_stop(kproc);
#endif

  /* kprocessor correctly initialize */
  kaapi_barrier_td_setactive(&kaapi_term_barrier, 0);

#if defined(KAAPI_USE_PERFCOUNTER)
  /*  */
  kaapi_perf_thread_fini(kproc); 
#endif

  return 0;
}
