/*
 ** kaapi_task_checkdenpendencies.c
 ** xkaapi
 ** 
 ** Created on Tue Feb 23 16:56:43 2010
 ** Copyright 2009 INRIA.
 **
 ** Contributors :
 ** thierry.gautier@imag.fr
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


/** task is the top task not yet pushed.
    This function is called is task is pushed into a specific thread using
    the C++ ka::SetPartition(site) attribut or the thread group access.
    
 */
int kaapi_threadgroup_computedependencies(kaapi_threadgroup_t thgrp, int threadindex, kaapi_task_t* task)
{
  kaapi_format_t*         task_fmt;
  kaapi_task_body_t       task_body;
  kaapi_taskdescr_t*      taskdescr =0;
  kaapi_thread_t*         thread =0;
  kaapi_tasklist_t*       tasklist =0;
  kaapi_hashentries_t*    entry;
  size_t                  cnt_notready;
  int                     isparamready;
  kaapi_version_t*        version;
  kaapi_taskdescr_t       dummy_taskdescr;

  /* pass in parameter ? cf C++ thread interface */
  kaapi_assert_debug( (threadindex >=-1) && (threadindex < thgrp->group_size) );

  kaapi_globalid_t gid = kaapi_threadgroup_tid2gid( thgrp, threadindex );

  task_body = kaapi_task_getbody( task );
  if (task_body!=0)
    task_fmt= kaapi_format_resolvebybody(task_body);
  else
    task_fmt = 0;

  if (task_fmt ==0) return EINVAL;
  
  if (thgrp->localgid == gid)
  {
    /* get the thread where to push the task */
    thread = kaapi_threadgroup_thread( thgrp, threadindex );
    
    /* allocate a new task descriptor for this task */
    tasklist =  thgrp->threadctxts[threadindex]->sfp->tasklist;
    taskdescr = kaapi_tasklist_allocate_td( tasklist, task );
  } 
  else 
  {
    taskdescr = &dummy_taskdescr;
    kaapi_taskdescr_init(taskdescr, task);
  }
  
  /* find the last writer for each args and in which partition id it  
     -> if all writers are in the same partition do nothing, push the task in the i-th partition
     -> if one of the writer is in a different partition, then change the body of the writer
     in order to add information to signal the task that are waiting for parameter
     
    ASSUMPTIONS:
    1- all the threads in the group are inactive and not subject to steal operation
    2- we only consider R,W and RW dependencies, not yet CW that implies multiple writers
  */
  void* sp = task->sp;
  size_t count_params = kaapi_format_get_count_params(task_fmt, sp );
  cnt_notready = count_params;
  for (unsigned int i=0; i < count_params; i++) 
  {
    kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE( kaapi_format_get_mode_param(task_fmt, i, sp) );
    if (m == KAAPI_ACCESS_MODE_V) 
    {
      --cnt_notready;
      continue;
    }
    
    /* its an access */
    kaapi_access_t access = kaapi_format_get_access_param(task_fmt, i, sp);
    entry = 0;

    /* find the version info of the data using the hash map */
    entry = kaapi_hashmap_find(&thgrp->ws_khm, access.data);
    if (entry ==0)
    {
      kaapi_memory_view_t view = kaapi_format_get_view_param(task_fmt, i, taskdescr->task->sp);

      /* no entry -> new version object: no writer */
      entry = kaapi_threadgroup_newversion( thgrp, &thgrp->ws_khm, threadindex, &access, &view );
    }

    /* have a look at the version and detect dependency or not etc... */
    version = entry->u.version;
    isparamready = 0;
    if (KAAPI_ACCESS_IS_CUMULWRITE(m))
      isparamready = kaapi_threadgroup_version_newwriter_cumulwrite( thgrp, version, threadindex, m, taskdescr, task_fmt, i, &access );
    else 
    {
      if (KAAPI_ACCESS_IS_READ(m))
      {
        isparamready = kaapi_threadgroup_version_newreader( thgrp, version, threadindex, m, taskdescr, task_fmt, i, &access );
      }
      if (KAAPI_ACCESS_IS_WRITE(m))
      {
        int retval = kaapi_threadgroup_version_newwriter( thgrp, version, threadindex, m, taskdescr, task_fmt, i, &access );
        /* else: responsability of the read part (just above) to compute readyness */
        if (KAAPI_ACCESS_IS_READWRITE(m))
          isparamready = isparamready && retval;
        else 
          isparamready = retval;
      }
    }
    
    if (isparamready) --cnt_notready;
    
  } /* end for all arguments of the task */

  if (thgrp->localgid == gid)
  {
//    if (cnt_notready ==0) 
    if (KAAPI_ATOMIC_READ(&taskdescr->counter) == 0)
      kaapi_tasklist_pushback_ready(tasklist, taskdescr);
    
    /* always push the task for local storage */
    kaapi_thread_pushtask( thgrp->threads[threadindex] );
    return 0;
  }
//  printf("Task not pushed !\n");
  return EINTR;
}

#if 0
int kaapi_threadgroup_add_mem_entry
(kaapi_threadgroup_t group, uintptr_t addr, size_t size)
{
  /* make the data valid in the address space
     of all the group threads (ie. partition)
   */

  int tid; /* thread index */

  /* foreach thread */
  for (tid = 0; tid < group->group_size; ++tid)
  {
    kaapi_access_t access;
    kaapi_hashentries_t* entry;
    kaapi_version_t* version;

    entry = kaapi_hashmap_find(&group->ws_khm, access.data);
    if (entry != NULL) continue ;

    /* create if not already in thread as */
    access.data = (void*)addr;
    entry = kaapi_threadgroup_newversion
      (group, &group->ws_khm, tid, &access);
    kaapi_assert_debug(entry != NULL);

    kaapi_address_space_id_t asid = kaapi_threadgroup_tid2asid(thgrp, tid);

    /* from version_new_reader */

    fail kaapi_version_findasid_in( ver, asid );
    kaapi_data_version_t* tov;

    version = entry->u.version;
  }

  return 0;
}
#endif
