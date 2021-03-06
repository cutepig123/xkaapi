/*
 ** xkaapi
 ** 
 **
 ** Copyright 2009,2010,2011,2012 INRIA.
 **
 ** Contributors :
 **
 ** Joao.Lima@imag.fr / joao.lima@inf.ufrgs.br
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

#include "kaapi_affinity.h"

#include "kaapi_tasklist.h"


kaapi_processor_t *kaapi_push_by_affinity_default(
  kaapi_processor_t * kproc,
	kaapi_taskdescr_t * td
)
{
  return kproc;
}

kaapi_processor_t *kaapi_push_by_affinity_locality(
                                             kaapi_processor_t * kproc,
                                             kaapi_taskdescr_t * td
)
{
  if (td->fmt == NULL)
    return kproc;
  
  return kaapi_memory_taskdescr_affinity_find_reduce_transfer(kproc, td);
}

kaapi_processor_t *kaapi_push_by_affinity_rand(kaapi_processor_t * kproc,
                                       kaapi_taskdescr_t * td)
{
  int nbproc, procid;
  
  if( td->fmt == 0 )
    return kproc;
  
  if (kproc->fnc_selecarg[0] == 0) 
    kproc->fnc_selecarg[0] = (uintptr_t)(long)rand();
  
  nbproc = kaapi_count_kprocessors;
redo_select:
  procid = rand_r( (unsigned int*)&kproc->fnc_selecarg ) % nbproc;
  if( procid == 0 )
    goto redo_select;
  
  return kaapi_all_kprocessors[ procid ];
}

kaapi_processor_t *kaapi_push_by_affinity_writer(
                                                 kaapi_processor_t * kproc,
                                                 kaapi_taskdescr_t * td
                                                 )
{
  if (td->fmt == 0)
    return kproc;
  
  return kaapi_memory_taskdescr_affinity_find_valid_wr(kproc, td);
}

