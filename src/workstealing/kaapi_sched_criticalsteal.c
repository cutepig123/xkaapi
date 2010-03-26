/*
** xkaapi
** 
** Created on Tue Mar 31 15:19:14 2009
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
#include "kaapi_impl.h"

/**
*/
int kaapi_steal_begincritical( kaapi_stealcontext_t* stc )
{
  kaapi_taskadaptive_t* ta = (kaapi_taskadaptive_t*)stc;
  kaapi_processor_t* kproc = _kaapi_get_current_processor();

  ta->save_splitter    = stc->splitter;
  ta->save_argsplitter = stc->argsplitter;
  stc->splitter    = 0;
  stc->argsplitter = 0;
  kaapi_mem_barrier();

#if (KAAPI_USE_STEALFRAME_METHOD == KAAPI_STEALCAS_METHOD)
  while (1)
  {
    if ( (KAAPI_ATOMIC_READ( &kproc->lock ) == 0) && KAAPI_ATOMIC_CAS(&kproc->lock, 0, 1+kproc->kid) ) break;
    /* nop here ? */
  }
  KAAPI_ATOMIC_WRITE( &kproc->lock, 0 );

#elif (KAAPI_USE_STEALFRAME_METHOD == KAAPI_STEALTHE_METHOD) 
#  if (KAAPI_USE_STEALTASK_METHOD == KAAPI_STEALCAS_METHOD)
  /* solution to lock (lite) the frame 
    while (kproc->thread->thieffp == stc->thread)
      ;
    but we prefer to lock only the task..
  */
  while (!kaapi_task_cas_extrastate( stc->ownertask, kaapi_adapt_body, kaapi_suspend_body ))
    ;
  /* cas success, so the thief will view here splitter ==0 */
  kaapi_task_setextrabody(stc->ownertask, kaapi_adapt_body);
  
#  elif (KAAPI_USE_STEALTASK_METHOD == KAAPI_STEALTHE_METHOD)
  kaapi_mem_barrier();
  while (kproc->thread->thiefpc == stc->ownertask)
    ;
  /* here the thiefpc will view splitter ==0 */
#  endif
#endif
  return 0;
}


/**
*/
int kaapi_steal_endcritical( kaapi_stealcontext_t* stc )
{
  kaapi_taskadaptive_t* ta = (kaapi_taskadaptive_t*)stc;

  kaapi_mem_barrier();

  stc->splitter    = ta->save_splitter;
  stc->argsplitter = ta->save_argsplitter;
  
  return 0;
}
