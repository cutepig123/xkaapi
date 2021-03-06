/*
 ** xkaapi
 ** 
 ** Created on Tue Mar 31 15:19:14 2009
 ** Copyright 2009,2010,2011,2012 INRIA.
 **
 ** Contributors :
 ** thierry.gautier@inrialpes.fr
 ** fabien.lementec@imag.fr
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
#include "kaapif.h"
#include "kaapic_impl.h"
#include <stdarg.h>


#define FATAL()						\
do {							\
  printf("fatal error @ %s::%d\n", __FILE__, __LINE__);	\
  kaapi_abort(); \
} while(0)

/* main entry point */
void kaapif_foreach_body2user(
  int32_t first, 
  int32_t last, 
  int32_t tid, 
  kaapic_body_arg_t* call 
)
{
  /* change [first, last[ to [first,last-1] as assumed in fortran.
  */
  --last;
#include "kaapif_adaptive_switch.h"
  KAAPIF_ADAPTIVE_SWITCH(call, first, last, tid);
}


/**
*/
int kaapif_foreach_(
  int32_t* first, int32_t* last, 
  int32_t* nargs, 
  void (*f)(int32_t*, int32_t*, int32_t*, ...), 
  ...
)
{
  int32_t k;
  kaapic_body_arg_t* body_arg;
  va_list va_args;

  kaapic_foreach_attr_t attr;

  if (*nargs >= KAAPIF_MAX_ARGS)
    return KAAPIF_ERR_EINVAL;

  body_arg = (kaapic_body_arg_t*)alloca(
    offsetof(kaapic_body_arg_t, args) + *nargs * sizeof(void*)
  ); 
  va_start(va_args, f);
  
  /* format of each effective parameter is a list of tuple:
       @
  */
  body_arg->u.f_f = f;
  body_arg->nargs = *nargs;

  for (k = 0; k < *nargs; ++k)
    body_arg->args[k] = va_arg(va_args, void*);
  va_end(va_args);
  
  kaapic_foreach_attr_init(&attr);
  kaapic_foreach_attr_set_grains(&attr, xxx_seq_grain, xxx_par_grain);

  if (kaapic_foreach_common( *first, *last+1, &attr, kaapif_foreach_body2user, body_arg) ==0)
    return KAAPIF_SUCCESS;
  return KAAPIF_ERR_FAILURE;
}


/**
*/
int kaapif_foreach_with_format_(
  int32_t* first, int32_t* last, 
  int32_t* nargs, 
  void (*f)(int32_t*, int32_t*, int32_t*, ...), 
  ...
)
{
  int32_t k;
  kaapic_body_arg_t* body_arg;
  va_list va_args;
  kaapic_foreach_attr_t attr;

  if (*nargs >= KAAPIF_MAX_ARGS)
    return KAAPIF_ERR_EINVAL;

  body_arg = (kaapic_body_arg_t*)alloca(
    offsetof(kaapic_body_arg_t, args) + *nargs * sizeof(void*)
  ); 
  va_start(va_args, f);
  
  /* format of each effective parameter is a list of tuple:
       @
  */
  body_arg->u.f_f = f;
  body_arg->nargs = *nargs;
  for (k = 0; k < *nargs; ++k)
  {
    /* skip mode */
    /* int* mode = (int*)*/va_arg(va_args, uintptr_t);

    body_arg->args[k] = va_arg(va_args, void*);

    /* skip count, type */
    /* int* count = (int*)*/va_arg(va_args, uintptr_t);
    /* int* type  = (int*)*/va_arg(va_args, uintptr_t);
  }
  va_end(va_args);
  
  kaapic_foreach_attr_init(&attr);
  kaapic_foreach_attr_set_grains(&attr, xxx_seq_grain, xxx_par_grain);

  if (kaapic_foreach_common( *first, *last+1, &attr,kaapif_foreach_body2user, body_arg) ==0)
    return KAAPIF_SUCCESS;
  return KAAPIF_ERR_FAILURE;
}
