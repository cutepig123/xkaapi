/*
** xkaapi
** 
**
** Copyright 2009,2010,2011,2012 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** francois.broquedis@imag.fr
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
#include "libgomp.h"
#include <stdio.h>

void
omp_set_num_threads (int n)
{
  kompctxt_t* ctxt = komp_get_ctxt();
  ctxt->icv.next_numthreads = n > 0 ? n : 1;
}

int
omp_get_num_threads (void)
{
  return omp_in_parallel () ? komp_get_ctxt()->teaminfo->numthreads : 1;
}

int
omp_get_thread_num (void)
{
  return komp_get_ctxt()->icv.thread_id;
}

/*
*/
int 
omp_get_max_threads (void)
{
  kompctxt_t* ctxt = komp_get_ctxt();
  return ctxt->icv.next_numthreads;
#if 0 /* should be an upper bound */
  if (ctxt->icv.next_numthreads < kaapi_getconcurrency())
  return kaapi_getconcurrency();
#endif
}

int omp_get_num_procs (void)
{
  return kaapi_getconcurrency();
}

/*
*/
int 
omp_in_parallel(void)
{
  kompctxt_t* ctxt = komp_get_ctxt();
  return (ctxt->teaminfo != 0) && (ctxt->teaminfo->numthreads > -1);
}

/*
*/
void 
omp_set_dynamic(int dynamic_threads __attribute__((unused)) )
{
  kompctxt_t* ctxt = komp_get_ctxt();
  ctxt->icv.dynamic_numthreads = dynamic_threads;
}

/*
*/
int 
omp_get_dynamic(void)
{
  kompctxt_t* ctxt = komp_get_ctxt();
  return ctxt->icv.dynamic_numthreads;
}

/*
*/
void 
omp_set_nested(int nested __attribute__((unused)))
{
  kompctxt_t* ctxt = komp_get_ctxt();
  ctxt->icv.nested_parallel = (nested !=0);
}

/*
*/
int 
omp_get_nested(void)
{
  kompctxt_t* ctxt = komp_get_ctxt();
  return (ctxt->icv.nested_parallel !=0);
}

/*
*/
void 
omp_set_schedule(omp_sched_t kind, int modifier )
{
  kompctxt_t* ctxt = komp_get_ctxt();
  kaapi_assert( (kind == omp_sched_dynamic)
             || (kind == omp_sched_static)
             || (kind == omp_sched_guided)
             || (kind == omp_sched_auto)
  );
  if (kind == omp_sched_auto) modifier = 0;
  ctxt->icv.run_sched  = kind;
  ctxt->icv.chunk_size = modifier;
}

/*
*/
void 
omp_get_schedule(omp_sched_t * kind, int * modifier )
{
  kompctxt_t* ctxt = komp_get_ctxt();
  *kind = ctxt->icv.run_sched;
  *modifier = ctxt->icv.chunk_size;
}

/*
*/
int 
omp_get_thread_limit(void)
{ 
  return kaapi_getconcurrency();
}


/* TODO: do not add thread is nested level > omp_max_active_levels
*/

/*
*/
void 
omp_set_max_active_levels (int max_levels )
{
  if (max_levels >=0)
    omp_max_active_levels = max_levels;
}

/*
*/
int 
omp_get_max_active_levels(void)
{
  return omp_max_active_levels;
}

/*
*/
int
omp_get_level(void)
{
  kompctxt_t* ctxt = komp_get_ctxt();
  return ctxt->icv.nested_level;
}

/* Returns the id of the level-th ancestor task/thread of the calling
task. */
int 
omp_get_ancestor_thread_num (int level)
{
  kompctxt_t *ctxt = komp_get_ctxt ();
  komp_teaminfo_t *prev_team, *teaminfo = ctxt->teaminfo;
  int hops = omp_get_level () - level;
  int error = -1;

  if (level == 0)
    return 0;

  if (hops == 0)
    return ctxt->icv.thread_id;
  
  if ((level < 0) || (hops < 0) || teaminfo == NULL)
    return -1;
 
  int i;
  for (i = 0; i < hops; i++)
    {
      if (teaminfo->previous_team == NULL)
	return error;

      prev_team = teaminfo;
      teaminfo = teaminfo->previous_team;
    }

  return prev_team->father_id;
}

/*
*/
int 
omp_get_team_size (int level)
{
  kompctxt_t *ctxt = komp_get_ctxt();
  komp_teaminfo_t *teaminfo = ctxt->teaminfo;
  int hops = omp_get_level () - level;
  int error = -1;

  if ((level < 0) || (hops < 0))
    return error;
  
  if (level == 0)
    return 1;

  if (hops == 0)
    return teaminfo->numthreads;

  int i;
  for (i = 0; i < hops; i++)
    {
      if (teaminfo->previous_team == NULL)
	return error;

      teaminfo = teaminfo->previous_team;
    }

  return teaminfo->numthreads;
}

/*
*/
int 
omp_get_active_level(void)
{
  kompctxt_t *ctxt = komp_get_ctxt ();
  
  if (ctxt == NULL)
    return -1;
  
  if (ctxt->teaminfo == NULL) 
    return -1;
  
  return ctxt->icv.active_level;
}

/*
*/
int 
omp_in_final(void)
{
  return 0;
}

double omp_get_wtime(void)
{
  return kaapi_get_elapsedtime();
}

double omp_get_wtick(void)
{
  return 1e-6; /* elapsed time is assumed to be in micro second ?? */
}


void 
komp_set_datadistribution_bloccyclic( unsigned long long size, unsigned int length )
{
#if defined(KAAPI_USE_FOREACH_WITH_DATADISTRIBUTION)
  printf("In komp_set_datadistribution_bloccyclic\n");
  kompctxt_t* ctxt = komp_get_ctxt();
  kaapic_foreach_attr_set_bloccyclic_datadistribution( &ctxt->icv.attr, size, length );
#endif
}
