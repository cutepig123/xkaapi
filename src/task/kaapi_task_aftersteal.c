/*
** kaapi_task_signal.c
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
#include <stdio.h>

/**
*/
void kaapi_aftersteal_body( void* taskarg, kaapi_thread_t* thread)
{
  int i, countparam;
  kaapi_format_t* fmt;   /* format of the task */
  void*           data_param;
  kaapi_format_t* fmt_param;
  kaapi_access_t* access_param;
  kaapi_task_t* task;
  
//  printf( "[taskaftersteal] task: @=%p, stack: @=%p\n", task, stack);
//  fflush(stdout);

  /* wait write appear in memory... */
  kaapi_readmem_barrier();

  /* the task has been stolen: the extra body contains the original task body */
  task = thread[-1].pc;
  fmt = kaapi_format_resolvebybody( kaapi_task_getextrabody(task) );
  kaapi_assert_debug( fmt !=0 );
  kaapi_assert_debug( task->body ==  kaapi_exec_body);
  kaapi_assert_debug( task->sp == taskarg );

  countparam = fmt->count_params;

  /* for each access parameter, we have:
     - data that points to the original effective parameter
     - version that points to the data that should have been used during the execution of the stolen task
     If the access is a W (or CW) access then, version contains the newly produced data.
     The purpose here is to merge the version into the original data
        - if W mode -> copy + free of the data
        - if CW mode -> accumulation (data is the left side of the accumulation, version the righ side)
  */
  for (i=0; i<countparam; ++i)
  {
    kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE(fmt->mode_params[i]);
    if (m == KAAPI_ACCESS_MODE_V) 
      continue;

    if (KAAPI_ACCESS_IS_ONLYWRITE(m))
    {
      data_param = (void*)(fmt->off_params[i] + (char*)taskarg);
      fmt_param = fmt->fmt_params[i];
      access_param = (kaapi_access_t*)(data_param);

#if 0
if ((unsigned long)access_param->data < 4042752UL) {
  printf("Invalid data: %p\n", access_param->data );
  exit(1);
}
#endif

      /* if m == W and data == version it means that data used by W was not copied due to non WAR dependency */
      if (access_param->data != access_param->version )
      {
#if 0
  printf("AfterSteal with copy, sp:%p\n", (void*)taskarg);
#endif
        /* add an assign + dstor function will avoid 2 calls to function, especially for basic types which do not
           required to be dstor.
        */
        (*fmt_param->assign)( access_param->data, access_param->version );
        if (fmt_param->dstor !=0) (*fmt_param->dstor) ( access_param->version );
        free(access_param->version);
      }
      access_param->version = 0;
    }
  }
}