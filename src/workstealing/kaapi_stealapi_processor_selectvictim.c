/*
** xkaapi
** 
** Created on Tue Mar 31 15:21:00 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
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
#include "kaapi_stealapi.h"

int (*kaapi_steal_processor_select_victim_function)( kaapi_steal_processor_t* kpss ) =0;

/**
*/
int kaapi_steal_processor_select_victim( kaapi_steal_processor_t* kpss )
{
  if (kaapi_steal_processor_select_victim_function !=0) 
    return (*kaapi_steal_processor_select_victim_function)(kpss);

  unsigned int seed = 0;
  unsigned int victim = 0;
  int c1,c2;
  
  kaapi_steal_processor_t* victim_stack;

  victim=0;

  /** Select a victim 
  */
  do {
    int nbstack_steal = KAAPI_ATOMIC_READ( &kaapi_index_stacksteal );
    if ( nbstack_steal>0)
      victim = rand_r( &seed ) % nbstack_steal;
    else 
      victim = 0;

    /** Is terminated 
    */
    if (kaapi_stealapi_term >0) return -1;

    /** Process steal request from outside world 
    */    
    c1 = victim == KAAPI_STEAL_PROCESSOR_GETINDEX(kpss);
    if (c1) continue;
    victim_stack = kaapi_all_stealprocessor[victim];
    if (victim_stack ==0) continue; 
    c2 = KAAPI_QUEUE_EMPTY( victim_stack );
    if (c2) continue;
    return victim;

  } while(1);

  return 0;
}
