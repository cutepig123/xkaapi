/*
** kaapi_mt_sched_idle.c
** xkaapi
** 
** Created on Tue Mar 31 15:18:04 2009
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

#define KAAPI_USE_AGGREGATION

#if defined(KAAPI_USE_AGGREGATION)
kaapi_thread_context_t* kaapi_sched_emitsteal ( kaapi_processor_t* kproc )
{
  kaapi_victim_t          victim;
  kaapi_reply_t*          replymemory ;
  int err;
  kaapi_listrequest_iterator_t lri;
  
  kaapi_assert_debug( kproc !=0 );
  kaapi_assert_debug( kproc->thread !=0 );
  kaapi_assert_debug( kproc == _kaapi_get_current_processor() );

  /* clear thief stack/thread that will receive tasks */
  kaapi_thread_clear( kproc->thread );
  
  /* map the reply data structure into the stack data */
  replymemory = kaapi_thread_pushdata( kaapi_threadcontext2thread(kproc->thread), 2*KAAPI_REPLY_DATA_SIZE_MIN );

redo_select:
  /* select the victim processor */
  err = (*kproc->fnc_select)( kproc, &victim );
  if (unlikely(err !=0)) goto redo_select;
  /* never pass by this function for a processor to steal itself */
  if (kproc == victim.kproc) return 0;

  /* mark current processor as stealing */
  kproc->issteal = 1;

  /* (1) 
     Fill & Post the request to the victim processor 
  */
  kaapi_request_post( kproc->kid, replymemory, victim.kproc );

#if defined(KAAPI_USE_PERFCOUNTER)
  ++KAAPI_PERF_REG(kproc, KAAPI_PERF_ID_STEALREQ);
#endif

wait_once:
  /* (2)
     lock and retest if they are yet posted requests on victim or not 
     if during tentaive of locking, a reply occurs, then return with reply
  */
  while (!kaapi_sched_trylock( victim.kproc ))
  {
    if (kaapi_reply_test( replymemory ) ) 
      goto return_value;

    kaapi_slowdown_cpu();
  }

  /* here becomes an aggregator... the trylock has synchronized memory */
  kaapi_listrequest_iterator_init(&victim.kproc->hlrequests, &lri);
  
  kaapi_assert_debug( (kaapi_listrequest_iterator_count(&lri) >0) || kaapi_reply_test( replymemory ) );
  
  if (kaapi_listrequest_iterator_empty(&lri)) 
    goto return_value;
  
  /* (3)
     process all requests on the victim kprocessor and reply failed to remaining requests
     
     Warning: In this version the aggregator has a lock on the victim processor.
  */
  kaapi_sched_stealprocessor( victim.kproc, &victim.kproc->hlrequests, &lri );
  kaapi_sched_unlock( victim.kproc );

#if defined(KAAPI_USE_PERFCOUNTER)
  ++KAAPI_PERF_REG(kproc, KAAPI_PERF_ID_STEALOP);
#endif

  /* est-ce que cela peut se produire ici ? */
  if (!kaapi_reply_test( replymemory )) 
    goto wait_once;

return_value:
  kaapi_sched_unlock( victim.kproc );
  
  /* mark current processor as no stealing anymore */
  kproc->issteal = 0;

  kaapi_assert_debug( (kaapi_reply_status(replymemory) != KAAPI_REQUEST_S_POSTED) ); 

  /* test if my request is ok
  */
  switch (kaapi_reply_status(replymemory))
  {
    case KAAPI_REQUEST_S_REPLY_OK:
    {
      kaapi_reply_steal_t* reply = (kaapi_reply_steal_t*)replymemory;
      kaapi_replysync_data( replymemory );
#if defined(KAAPI_USE_PERFCOUNTER)
      ++KAAPI_PERF_REG(kproc, KAAPI_PERF_ID_STEALREQOK);
#endif
      /* protocol for stealing, on reply: 
         - case of a stack: 
           replymemory->data[0] == op code= KAAPI_REPLY_S_TASK
           replymemory->data[1..6] == not used
           replymemory->data[7] == aligned on 64 bit boundary = body
           replymemory->data[
      */
      switch (reply->opcode) {
        case KAAPI_REPLY_S_TASK_FMT:
          /* convert fmtid to a task body */
          reply->u.s_task.body = kaapi_format_resolvebyfmit( reply->u.s_taskfmt.fmt )->entrypoint[kproc->proc_type];
        case KAAPI_REPLY_S_TASK:
          /* arguments already pushed, increment the stack pointer */
          kaapi_thread_pushdata( kaapi_threadcontext2thread(kproc->thread), KAAPI_CACHE_LINE);
          
          /* push a task with the body */
          kaapi_task_init( kaapi_thread_toptask(kaapi_threadcontext2thread(kproc->thread)), reply->u.s_task.body, reply->u.s_task.data );
          kaapi_thread_pushtask(kaapi_threadcontext2thread(kproc->thread));
          return kproc->thread;

        case KAAPI_REPLY_S_THREAD:
          return reply->u.s_thread.thread;
        default:
          kaapi_assert_m( 0, "Bad opcode in reply" );
      };
    } break;

    case KAAPI_REQUEST_S_REPLY_NOK:
      return 0;

    case KAAPI_REQUEST_S_ERROR:
      kaapi_assert_debug_m(0, "Error code in request status" );
    default:
      kaapi_assert_debug_m(0, "Bad request status" );
  }
  return 0;  
}

#elif (KAAPI_USE_STEAL_METHOD == KAAPI_CAS_METHOD)
kaapi_thread_context_t* kaapi_sched_emitsteal ( kaapi_processor_t* kproc )
{
  kaapi_victim_t          victim;
  kaapi_reply_t*          replymemory ;
  int err;
  
  kaapi_assert_debug( kproc !=0 );
  kaapi_assert_debug( kproc->thread !=0 );
  kaapi_assert_debug( kproc == _kaapi_get_current_processor() );

  /* clear thief stack/thread that will receive tasks */
  kaapi_thread_clear( kproc->thread );
  
  /* map the reply data structure into the stack data */
  replymemory = kaapi_thread_pushdata( kaapi_threadcontext2thread(kproc->thread), 2*KAAPI_REPLY_DATA_SIZE_MIN );

redo_select:
  /* select the victim processor */
  err = (*kproc->fnc_select)( kproc, &victim );
  if (unlikely(err !=0)) goto redo_select;
  /* never pass by this function for a processor to steal itself */
  if (kproc == victim.kproc) return 0;

  /* mark current processor as stealing */
  kproc->issteal = 1;

  kaapi_request_post( kproc->kid, replymemory, victim.kproc );

#if defined(KAAPI_USE_PERFCOUNTER)
  ++KAAPI_PERF_REG(kproc, KAAPI_PERF_ID_STEALREQ);
#endif

  kaapi_sched_stealprocessor( victim.kproc, &victim.kproc->hlrequests );
#if defined(KAAPI_USE_PERFCOUNTER)
  ++KAAPI_PERF_REG(kproc, KAAPI_PERF_ID_STEALOP);
#endif
  kaapi_assert_debug(kaapi_reply_test( replymemory ));

  /* mark current processor as no stealing anymore */
  kproc->issteal = 0;

  /* test if my request is ok
  */
  switch (kaapi_reply_status(replymemory))
  {
    case KAAPI_REQUEST_S_REPLY_OK:
    {
      kaapi_reply_steal_t* reply = (kaapi_reply_steal_t*)replymemory;
      kaapi_replysync_data( replymemory );
#if defined(KAAPI_USE_PERFCOUNTER)
      ++KAAPI_PERF_REG(kproc, KAAPI_PERF_ID_STEALREQOK);
#endif
      /* protocol for stealing, on reply: 
         - case of a stack: 
           replymemory->data[0] == op code= KAAPI_REPLY_S_TASK
           replymemory->data[1..6] == not used
           replymemory->data[7] == aligned on 64 bit boundary = body
           replymemory->data[
      */
      switch (reply->opcode) {
        case KAAPI_REPLY_S_TASK_FMT:
          /* convert fmtid to a task body */
          reply->u.s_task.body = kaapi_format_resolvebyfmit( reply->u.s_taskfmt.fmt )->entrypoint[kproc->proc_type];
        case KAAPI_REPLY_S_TASK:
          /* push the task, arguments already pushed */
          return kproc->thread;

        case KAAPI_REPLY_S_THREAD:
          return reply->u.s_thread.thread;
        default:
          kaapi_assert_m( 0, "Bad opcode in reply" );
      };
    } break;

    case KAAPI_REQUEST_S_REPLY_NOK:
      return 0;

    case KAAPI_REQUEST_S_ERROR:
      kaapi_assert_debug_m(0, "Error code in request status" );
    default:
      kaapi_assert_debug_m(0, "Bad request status" );
  }
  return 0;  
}

#endif