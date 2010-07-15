/*
** xkaapi
** 
** Created on Tue Mar 31 15:18:04 2009
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
void kaapi_taskreturn_body( void* taskarg, kaapi_thread_t* thread )
{
  kaapi_taskadaptive_t* ta = (kaapi_taskadaptive_t*)taskarg;
  kaapi_assert_debug( ta !=0 );

  /* wait end of pending thief, if any */
  while (KAAPI_ATOMIC_READ(&ta->sc.is_there_thief) !=0)
    kaapi_slowdown_cpu();

  kaapi_readmem_barrier(); /* avoid read reorder before the barrier, for instance reading some data */

  kaapi_assert(ta->head ==0)
  kaapi_assert(ta->tail ==0)
  kaapi_assert( KAAPI_ATOMIC_READ(&ta->thievescount) ==0 ); /* else pb -> call kaapi_steal_finalize ! */
}


/**
*/
int kaapi_steal_thiefreturn( kaapi_stealcontext_t* stc )
{
  kaapi_taskadaptive_t* ta = (kaapi_taskadaptive_t*)stc;
  kaapi_assert_debug( ta !=0 );

  ta->sc.splitter = 0;
  ta->sc.argsplitter = 0;
  kaapi_task_setbody( ta->sc.ownertask, kaapi_nop_body );
  kaapi_task_setextrabody( ta->sc.ownertask, kaapi_nop_body );

#if defined(KAAPI_DEBUG)
  /* push task to wait childs */
  kaapi_task_t* task = kaapi_thread_toptask(stc->thread);
  kaapi_task_init( task, kaapi_taskreturn_body, ta );
  kaapi_thread_pushtask(stc->thread);
#endif

  return 0;
}