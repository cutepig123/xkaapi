/*
** kaapi_mt_threadcontext.c
** xkaapi
** 
** Created on Tue Mar 31 15:16:47 2009
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

/**
*/
int kaapi_setcontext( kaapi_processor_t* proc, const kaapi_thread_context_t* ctxt )
{
  kaapi_assert_debug( proc == _kaapi_get_current_processor() );
  *proc->ctxt = *ctxt;
  proc->ctxt->requests = proc->hlrequests.requests;
  return 0;

#if 0  /* TODO: next version when also saving the stack context */
  proc->flags        = ctxt->flags;
  proc->dataspecific = ctxt->dataspecific;

  if (ctxt->flags & KAAPI_CONTEXT_SAVE_KSTACK)
  {
    proc->kstack     = ctxt->kstack;
  }

  if (ctxt->flags & KAAPI_CONTEXT_SAVE_CSTACK)
  {
#if defined(KAAPI_USE_UCONTEXT)
    setcontext( &proc->_ctxt );
#elif defined(KAAPI_USE_SETJMP)
    _longjmp( proc->_ctxt,  (int)(long)ctxt);
#endif
  }
  return 0;
#endif  /* TODO: next version when also saving the stack context */
}