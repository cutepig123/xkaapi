/*
** kaapi_task_pushstealcontext.c
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
kaapi_stealcontext_t* kaapi_task_begin_adaptive
(
  kaapi_thread_t*       thread,
  int                   flag,
  kaapi_task_splitter_t splitter,
  void*                 argsplitter
)
{
  kaapi_stealcontext_t*   sc;
  kaapi_thread_context_t* self_thread;
  kaapi_task_t*           task;
  kaapi_frame_t           frame;
  
  kaapi_thread_save_frame(thread, &frame);

  self_thread = kaapi_self_thread_context();
  
  /* todo: should be pushed cacheline aligned */
  sc = (kaapi_stealcontext_t*)kaapi_thread_pushdata_align
    (thread, sizeof(kaapi_stealcontext_t), sizeof(void*));
  kaapi_assert_debug(sc != 0);

  sc->preempt           = &self_thread->static_reply.preempt;
  sc->splitter          = splitter;
  sc->argsplitter       = argsplitter;
  sc->header.flag       = flag;
  sc->header.msc	      = sc; /* self pointer to detect master */
  sc->header.ktr	      = 0;

  if (flag & KAAPI_SC_PREEMPTION)
  {
    /* if preemption, thief list used ... */
    KAAPI_ATOMIC_WRITE(&sc->thieves.list.lock, 0);
    sc->thieves.list.head = 0;
    sc->thieves.list.tail = 0;
  }
  else
  {
    /* ... otherwise thief count */
    KAAPI_ATOMIC_WRITE(&sc->thieves.count, 0);
  }

  sc->frame                 = frame;
  sc->save_splitter         = 0;
  sc->save_argsplitter      = 0;

  task = kaapi_thread_toptask(thread);
  sc->ownertask = task;

  /* ok is initialized */
  sc->header.flag       |= KAAPI_SC_INIT;

  /* change our execution state before pushing the task.
     this is needed for the assumptions made by the
     kaapi_task_lock_steal rouines, see for comments.
   */
  const kaapi_uintptr_t state =
    kaapi_task_state_setexec(kaapi_task_body2state(kaapi_adapt_body));
  kaapi_task_init(task, kaapi_task_state2body(state), sc);


  /* barrier done by kaapi_thread_pushtask */
  kaapi_thread_pushtask(thread);

  return sc;
}
