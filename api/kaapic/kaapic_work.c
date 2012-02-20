/*
 ** xkaapi
 ** 
 ** Created on Tue Mar 31 15:19:14 2009
 ** Copyright 2009 INRIA.
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
 
/* Note on the implementation.
   The work structure is decomposed in two parts that follows the two
   scheduling level adopted here.
   At the upper level, the work is a kaapic_global_work_t. It represents
   the iteration space distributed among a set of participants.
   At the lower lelve, the work is a kaapic_local_work_t which is
   represented as a kaapi_workqueue_t.
   Initialisation of the global work must be made by a unique caller thread.
   Once created, the caller thread has initialized the global work and the
   local work information. The adaptive task publishes the splitter that will
   create new participant.
   
   Once a participant is enrolled to work on the global work instance, i.e. after
   he begins to execute the _kaapic_thief_entrypoint, he never returns until the
   global end of the work.
   This optimization allows to bypass a return to the scheduler. Nevertheless, 
   it can let work inactive until all the enrolled threads complete.
   
   The distribution is not an foreach_attribute, but it must be.
   The current distribution data structure is handle by the wa field of the global
   work data structure. A thread with given tid, correspond to initial slice 
   [startindex[pos]..starindex[pos+1]) where pos = tid2pos[tid].
   The table tid2pos is used to compact information at the begin of the array
   startindex. Only thread with tid such that the tid-th bit is set in the
   map field can steal the initial slice.
   T.G.
*/
#define USE_KPROC_LOCK 1 /* defined to use kprocessor lock, else use local lock */
 
#include "kaapi_impl.h"
#include "kaapic_impl.h"


/* set to 0 to disable workload */
#define CONFIG_USE_WORKLOAD 1

#if CONFIG_USE_WORKLOAD
extern void kaapi_set_self_workload(unsigned long);
#define KAAPI_SET_SELF_WORKLOAD(__w)		\
do {						\
  kaapi_set_self_workload(__w);			\
} while (0)
#else 
#define KAAPI_SET_SELF_WORKLOAD(__w)
#endif


#define CONFIG_FOREACH_STATS 0
#if CONFIG_FOREACH_STATS
static double foreach_time = 0;
#endif


#define FATAL()						\
do {							\
  printf("fatal error @ %s::%d\n", __FILE__, __LINE__);	\
  exit(-1);						\
} while(0)


#if defined(KAAPI_DEBUG)
# define PRINTF(__s, ...) printf(__s, __VA_ARGS__)
#else
# define PRINTF(__s, ...)
#endif


#if CONFIG_MAX_TID
/* maximum thread number */
static volatile unsigned int xxx_max_tid;
#endif


int kaapic_foreach_globalwork_next(
  kaapic_local_work_t*     lwork,
  kaapi_workqueue_index_t* first,
  kaapi_workqueue_index_t* last
);

/* */
static int kaapic_local_work_steal
(
  kaapic_local_work_t* lwork,
  kaapi_processor_t*   kproc, 
  long* i, long* j,
  kaapi_workqueue_index_t unit_size
)
{
  int retval;
  _kaapi_workqueue_lock(&lwork->cr);
  retval = kaapi_workqueue_steal(&lwork->cr, i, j, unit_size) ==0;
  _kaapi_workqueue_unlock(&lwork->cr);
  return retval;
}

static void kaapic_global_getwork
(
  kaapic_global_work_t* gw,
  kaapi_processor_id_t tid, 
  kaapi_workqueue_index_t* i, 
  kaapi_workqueue_index_t* j
)
{
  kaapi_assert_debug(tid<KAAPI_MAX_PROCESSOR);
  int pos = gw->wa.tid2pos[tid];
  kaapi_assert_debug( pos > 0 );
  
  *i = gw->wa.startindex[pos];
  *j = gw->wa.startindex[pos+1];
}

int kaapic_global_work_pop
(
  kaapic_global_work_t* gw,
  kaapi_processor_id_t tid, 
  kaapi_workqueue_index_t* i, 
  kaapi_workqueue_index_t* j
)
{
  kaapi_assert_debug(tid<KAAPI_MAX_PROCESSOR);
  int pos = gw->wa.tid2pos[tid];
  kaapi_assert_debug( pos >= 0 );
  
  *i = gw->wa.startindex[pos];
  *j = gw->wa.startindex[pos+1];
  return 0 == kaapi_bitmap_unset(&gw->wa.map, pos);
}


/* */
static int kaapic_global_work_steal
(
  kaapic_global_work_t* gwork,
  kaapi_processor_t*    kproc, 
  kaapi_workqueue_index_t* i, 
  kaapi_workqueue_index_t* j
)
{
  kaapi_workqueue_index_t unit_size;
  kaapi_workqueue_index_t range_size;

  /* here to do / to test:
     - any try to pop a slice closed to the tid of the thread
     - only 0 can pop a non poped slice
  */
#if 0 /* caller has already pop and finish its slice */
  int tid = kproc->kid;
  kaapi_assert_debug(tid<KAAPI_MAX_PROCESSOR);
  if (kaapic_global_work_pop(gwork, tid, i, j ))
    /* success */
    return 1;
#endif
    
  /* try to steal from a random selected local workqueue */
  /* the code here is closed to the emitsteal code except we bypass the request mechanism */
  kaapi_victim_t  victim;
  int             err;

redo_select:
  /* warning here the method does not return except: work is found or total work is done
  */
  if (KAAPI_ATOMIC_READ(&gwork->workremain) ==0) 
    return 0;

  /* select the victim processor */
  err = (*kproc->fnc_select)( kproc, &victim, KAAPI_SELECT_VICTIM );
  if (unlikely(err !=0)) 
    goto redo_select;

  /* never pass by this function for a processor to steal itself */
  if (kproc == victim.kproc) 
    goto redo_select;
    
  /* do not steal if range size <= par_grain */
  kaapic_local_work_t* lwork = gwork->lwork[victim.kproc->kid];
  if (lwork ==0)
    goto redo_select;

  /* try to steal the local work */
  const kaapic_work_info_t* const wi = &gwork->wi;

  range_size = kaapi_workqueue_size(&lwork->cr);
  if (range_size <= wi->par_grain)
    return 0;
  unit_size = range_size / 2;

  return kaapic_local_work_steal(
    lwork,
    kproc, 
    i,
    j,
    unit_size
  );
  
  return 0;
}


/* TODO: used to set the workload from the remaining size... */
static inline unsigned long work_array_size(const kaapic_work_distribution_t* wa)
{
  return kaapi_bitmap_count(&wa->map);
}


/* fwd decl */
static void _kaapic_thief_entrypoint(void*, kaapi_thread_t*,  kaapi_task_t* );


/* task splitter: interface between scheduler and kaapic works. 
   This function is called when an idle kproc emits a request.
   If the sequence has as many slices, one for each core, then
   each kproc only pass one through this function
*/
static int _kaapic_split_task
(
  struct kaapi_task_t*                 victim_task,
  void*                                arg,
  struct kaapi_listrequest_t*          lr,
  struct kaapi_listrequest_iterator_t* lri
)
{
  /* call by a thief to steal work */
  int tid;
  kaapic_local_work_t* const lwork = (kaapic_local_work_t*)arg;
  kaapic_global_work_t* const gwork = lwork->global;
  kaapi_bitmap_value_t mask;
  kaapi_bitmap_value_t negmask;
  kaapi_bitmap_value_t replymask;  
  kaapi_workqueue_index_t range_size, unit_size;
  kaapi_workqueue_index_t first, last;
  kaapi_request_t* req;
  kaapic_local_work_t* tw;

  /* count requests that will be served by root tasks.
  */
#if defined(KAAPI_DEBUG)
  kaapi_listrequest_iterator_t save_lri;
  save_lri = *lri;
#endif
  kaapi_listrequest_iterator_t cpy_lri;
  cpy_lri = *lri;
  kaapi_bitmap_value_clear( &mask );
  for (; !kaapi_listrequest_iterator_empty(&cpy_lri); ) 
  {
    kaapi_request_t* req = kaapi_listrequest_iterator_get(lr, &cpy_lri);
    kaapi_bitmap_value_set( &mask, gwork->wa.tid2pos[ req->ident ] );
    
    kaapi_listrequest_iterator_next(lr, &cpy_lri);
  }
  kaapi_bitmap_value_neg( &negmask, &mask );

  /* Steal toplevel request: mark to 0 all entries of a incomming request */
  kaapi_bitmap_and( &replymask, &gwork->wa.map, &negmask );
  
  /* reply to all requests in original_mask */
  kaapi_bitmap_value_and(&replymask, &mask);
  
  /* reply for first time to thread with reserved request (if !=0) */
  while ((tid = kaapi_bitmap_first1_and_zero(&replymask)) != 0)
  {
    --tid;
    /* use only get: the bitmap gwork->wa.map was updated atomically above */
    kaapic_global_getwork( gwork, tid, &first, &last );
    
    req = &kaapi_global_requests_list[tid];
    tw = kaapi_request_pushdata(req, sizeof(kaapic_local_work_t) );
    tw->global = gwork;
    tw->tid    = tid;
#if defined(USE_KPROC_LOCK)
    kaapi_workqueue_init_with_lock
      (&tw->cr, first, last, &kaapi_all_kprocessors[tid]->lock);
#else
    kaapi_atomic_initlock(&tw->lock);
    kaapi_workqueue_init_with_lock
      (&tw->cr, first, last, &tw->lock);
#endif    
    gwork->lwork[tid] = tw;

    kaapi_task_init_with_flag(
      kaapi_request_toptask(req), 
      (kaapi_task_body_t)_kaapic_thief_entrypoint, 
      tw,
      KAAPI_TASK_UNSTEALABLE
    );
#if 1 /* comment this line if you do not want thief to be thief by other */
    kaapi_request_pushtask_adaptive_tail( 
      req, 
      victim_task,
      _kaapic_split_task 
    );
#else // unstealable task 
    kaapi_request_pushtask(
      req,
      victim_task
    );
#endif
    kaapi_request_committask(req);
    kaapi_listrequest_iterator_unset_at( lri, tid );
  }
  if (kaapi_listrequest_iterator_empty(lri))
    return 0;
  
  /* else: remaining requests in lri was already steal their replied  
     here is code to reply to thread that do not have reserved slice
  */
  int nreq = kaapi_listrequest_iterator_count(lri);
  range_size = kaapi_workqueue_size(&lwork->cr);
  if (range_size <= gwork->wi.par_grain)
    /* no enough work: stop stealing this task */
    return 0;

  /* how much per non root req */
  unit_size = range_size / (nreq + 1);
  if (unit_size < gwork->wi.par_grain)
  {
    nreq = (range_size / gwork->wi.par_grain) - 1;
    unit_size = gwork->wi.par_grain;
    if (nreq ==0)
      return 0;
  }

#if defined(USE_KPROC_LOCK)
  kaapi_assert_debug( lwork->cr.lock 
    == &kaapi_get_current_processor()->victim_kproc->lock );
  kaapi_assert_debug( kaapi_atomic_assertlocked(lwork->cr.lock) );
#else
  _kaapi_workqueue_lock(&lwork->cr);
#endif

  if (kaapi_workqueue_steal(&lwork->cr, &first, &last, nreq * unit_size))
  {
#if defined(USE_KPROC_LOCK)
#else
    _kaapi_workqueue_unlock(&lwork->cr);
#endif
    return 0;
  }
  
#if defined(USE_KPROC_LOCK)
#else
  _kaapi_workqueue_unlock(&lwork->cr);
#endif
  for ( /* void */; 
        !kaapi_listrequest_iterator_empty(lri);
        kaapi_listrequest_iterator_next(lr, lri)
      )
  {
    req = kaapi_listrequest_iterator_get(lr, lri);
    tw = kaapi_request_pushdata(req, sizeof(kaapic_local_work_t) );
    tw->global = gwork;
    tw->tid = req->ident;
#if defined(USE_KPROC_LOCK)
    kaapi_workqueue_init_with_lock
      (&tw->cr, first, last, &kaapi_all_kprocessors[tw->tid]->lock);
    kaapi_assert_debug(first < last);
#else
    kaapi_atomic_initlock(&tw->lock);
    kaapi_workqueue_init_with_lock
      (&tw->cr, last-unit_size, last, &tw->lock);
    kaapi_assert_debug(unitsize > 0);
#endif    
    gwork->lwork[req->ident] = tw;
    kaapi_task_init_with_flag(
      kaapi_request_toptask(req), 
      (kaapi_task_body_t)_kaapic_thief_entrypoint, 
      tw,
      KAAPI_TASK_UNSTEALABLE
    );
#if 1 /* comment this line if you do not want thief to be thief by other */
    kaapi_request_pushtask_adaptive_tail( 
      req, 
      victim_task,
      _kaapic_split_task 
    );
#else // unstealable task 
    kaapi_request_pushtask(
      req,
      victim_task
    );
#endif
    kaapi_request_committask(req);
    last -= unit_size;
  }
  kaapi_assert_debug( last == first );
  return 0;
}


/* thief entrypoint, lwork is already initialized */
static void _kaapic_thief_entrypoint(
  void*                 arg, 
  kaapi_thread_t*       thread,
  kaapi_task_t*         pc
)
{
  /* range to process */
  kaapi_workqueue_index_t i;
  kaapi_workqueue_index_t j;

  /* process the work */
  kaapic_local_work_t* const lwork = (kaapic_local_work_t*)arg;
  kaapic_global_work_t* const gwork = lwork->global;
  
  /* work info */
  const kaapic_work_info_t* const wi = &gwork->wi;

  /* retrieve tid */
  kaapi_assert_debug(kaapi_get_self_kid() == lwork->tid);

#if defined(KAAPI_DEBUG)
  kaapi_processor_t* kproc = kaapi_get_current_processor();
#endif
#if defined(USE_KPROC_LOCK)
  kaapi_assert_debug( &kproc->lock == lwork->cr.lock );
#else
#endif
  kaapi_assert_debug( kproc->kid == lwork->tid );

  /* while there is sequential work to do in local work */
  while (kaapi_workqueue_pop(&lwork->cr, &i, &j, wi->seq_grain) ==0)
  {
    KAAPI_SET_SELF_WORKLOAD(kaapi_workqueue_size(&lwork->cr));
    lwork->workdone += j-i;
redo_local_work:
    kaapi_assert_debug( i < j );
    /* apply w->f on [i, j[ */
    gwork->body_f((int)i, (int)j, (int)lwork->tid, gwork->body_args);
  }

  /* */
  KAAPI_SET_SELF_WORKLOAD(0);
//  printf("Work finish: work done:%i\n", lwork->workdone);
  kaapi_assert_debug(lwork->workdone >=0);
  KAAPI_ATOMIC_SUB(&gwork->workremain, lwork->workdone);
  kaapi_assert_debug(KAAPI_ATOMIC_READ(&gwork->workremain) >=0);
  lwork->workdone = 0;
  if (kaapic_foreach_globalwork_next( lwork, &i, &j ))
  {
//    printf("Master steal [%i, %i)\n", i, j);
    goto redo_local_work;
  }
  
  /* suppress lwork reference */
  gwork->lwork[lwork->tid] = 0;
  
#if defined(USE_KPROC_LOCK)
#else
  kaapi_atomic_destroylock(&lwork->lock);
#endif
}



/* Used to start parallel region if required */
extern unsigned int kaapic_do_parallel;


/* Initialize the global work:
   - save the frame of the calling thread
   - allocate the global work into the stack
*/
kaapic_global_work_t* kaapic_foreach_global_workinit
(
  kaapi_thread_context_t* self_thread,
  kaapi_workqueue_index_t first, 
  kaapi_workqueue_index_t last,
  const kaapic_foreach_attr_t*  attr,
  kaapic_foreach_body_t   body_f,
  kaapic_body_arg_t*      body_args
)
{
  kaapic_global_work_t* gwork;
  kaapi_bitmap_value_t mask;
  kaapi_frame_t save_frame;
  int sizemap;

  /* if empty gwork 
  */
  if (first >= last) 
    return 0;
  
  /* is_format true if called from kaapif_foreach_with_format */
  kaapi_thread_t* const thread = kaapi_threadcontext2thread(self_thread);
  int localtid = self_thread->stack.proc->kid;

  /* save frame */
  kaapi_thread_save_frame(thread, &save_frame);
  gwork = kaapi_thread_pushdata(thread, sizeof(kaapic_global_work_t));
  kaapi_assert_debug(gwork !=0);
  gwork->frame = save_frame;
  
  /* work array, reserve range in [first,last) for each thief */
  if (attr == 0) attr = &kaapic_default_attr;
  int concurrency = kaapi_getconcurrency();
  kaapi_workqueue_index_t range_size;
  long off;
  long scale;

#if CONFIG_FOREACH_STATS
  const double time = kaapif_get_time_();
#endif

  /* compute the map of kid where to pre-reserve local work. Exclude the calling kproc */
  kaapi_bitmap_value_clear(&mask);
  kaapi_bitmap_value_set_low_bits(&mask, concurrency);

  /* mask with specified cpuset */
  kaapi_bitmap_value_and(&mask, (kaapi_bitmap_value_t*)&attr->cpuset);
  kaapi_bitmap_init( &gwork->wa.map, &mask );

  /* concurrency now = size of the set */
  sizemap = kaapi_bitmap_value_count(&mask);

  /* split the range in equal slices */
  range_size = last - first;
  KAAPI_ATOMIC_WRITE(&gwork->workremain, range_size);

  /* handle concurrency too high case */
  if (range_size < sizemap) sizemap = range_size;

  /* round range to be multiple of concurrency 
     tid with indexes 0 will get the biggest part
     Here it is an uniform block distribution. An other distribution should be used.
  */
  off = range_size % sizemap;
  range_size -= off;
  scale = range_size / sizemap;
  
  /* init logical mapping from tid to [0, ..., n] such that localtid is attached to 0 if
     is in the set.
  */
  uint16_t localcount = 0;
  if (kaapi_bitmap_value_get(&mask, localtid))
    gwork->wa.tid2pos[localtid] = localcount++;
  int i;
  for (i=0; i<localtid; ++i)
  {
    if (kaapi_bitmap_value_get(&mask, i)  && (localcount < sizemap))
      gwork->wa.tid2pos[i] = localcount++;
    else 
      gwork->wa.tid2pos[i] = (uint16_t)-1; /* not in the set */
  }
  for (i=localtid+1; i<concurrency; ++i)
  {
    if (kaapi_bitmap_value_get(&mask, i) && (localcount < sizemap))
      gwork->wa.tid2pos[i] = localcount++;
    else 
      gwork->wa.tid2pos[i] = (uint16_t)-1; /* not in the set */
  }
  
  /* fill the start indexes: here it should be important to
     allocate slices depending of the futur thread id... ?
  */
  gwork->wa.startindex[0] = first;
  gwork->wa.startindex[1] = first+off+scale;
  for (i=1; i<sizemap; ++i)
    gwork->wa.startindex[i+1] = gwork->wa.startindex[i]+scale;

  kaapi_assert_debug(gwork->wa.startindex[sizemap] == last);

  gwork->wi.par_grain = attr->p_grain;
  gwork->wi.seq_grain = attr->s_grain;

  gwork->body_f    = body_f;
  gwork->body_args = body_args;

  KAAPI_SET_SELF_WORKLOAD(range_size);

  return gwork;
}  



/* Initialize the local work + the global work
   Return a pointer to the local work to execute. 
*/
kaapic_local_work_t* kaapic_foreach_local_workinit
(
  kaapi_thread_context_t* self_thread, /* for storage */
  kaapic_global_work_t*   gwork
)
{
  kaapic_local_work_t*    lwork;
  
  /* is_format true if called from kaapif_foreach_with_format */
  kaapi_thread_t* const thread = kaapi_threadcontext2thread(self_thread);
  const int tid = self_thread->stack.proc->kid;

  lwork = kaapi_thread_pushdata(thread, sizeof(kaapic_local_work_t));
  kaapi_assert_debug(lwork !=0);
  lwork->workdone = 0;

  lwork->global     = gwork;
  lwork->tid        = tid;
  gwork->lwork[tid] = lwork;

  /* begin a parallel region */
  if (kaapic_do_parallel) kaapic_begin_parallel();

  return lwork;
}  


/* Initialize the local work + the global work
   Return a pointer to the local work to execute. 
*/
kaapic_local_work_t* kaapic_foreach_workinit
(
  kaapi_thread_context_t* self_thread,
  kaapi_workqueue_index_t first, 
  kaapi_workqueue_index_t last,
  const kaapic_foreach_attr_t*  attr,
  kaapic_foreach_body_t   body_f,
  kaapic_body_arg_t*      body_args
)
{
  kaapic_global_work_t* gwork;
  kaapic_local_work_t*  lwork;

  /* if empty gwork 
  */
  if (first >= last) 
    return 0;

  const int tid = self_thread->stack.proc->kid;

//  printf("\n----------Work init\n");
  gwork = kaapic_foreach_global_workinit(
      self_thread, 
      first, 
      last, 
      attr, 
      body_f, 
      body_args
  );
  
  if (gwork ==0) return 0;
  lwork = kaapic_foreach_local_workinit( self_thread, gwork );

#if defined(USE_KPROC_LOCK)
  kaapi_workqueue_init_with_lock(
    &lwork->cr,
    0, 0,
    &kaapi_all_kprocessors[tid]->lock
  );
#else
  kaapi_atomic_initlock(&lwork->lock);
  kaapi_workqueue_init_with_lock(
    &lwork->cr, 
    0, 0,
    &lwork->lock
  );
#endif
  return lwork;
}  


int kaapic_foreach_local_workend(
  kaapic_local_work_t*    lwork
)
{
#if defined(USE_KPROC_LOCK)
#else
  kaapi_atomic_destroylock(&lwork->cr.lock);
#endif
  return 0;
}

int kaapic_foreach_workend
(
  kaapi_thread_context_t* self_thread,
  kaapic_local_work_t*    lwork
)
{
  /* must the thread that initialize the global work */
  kaapi_thread_t* const thread = kaapi_threadcontext2thread(self_thread);
  KAAPI_SET_SELF_WORKLOAD(0);

  if (kaapic_do_parallel) kaapic_end_parallel(0);
  
#if defined(USE_KPROC_LOCK)
#else
  kaapi_atomic_destroylock(&lwork->cr.lock);
#endif  

  /* restore frame */
  kaapi_thread_restore_frame(thread, &lwork->global->frame);

//  printf("\n----------Work end\n");
  return 0;
}


/* 
  Return !=0 iff first and last have been filled for the next piece
  of work to execute.
  The function try to steal from registered lwork in the global work.
  The local workqueue is fill by poped range.
*/
int kaapic_foreach_globalwork_next(
  kaapic_local_work_t*     lwork,
  kaapi_workqueue_index_t* first,
  kaapi_workqueue_index_t* last
)
{
  kaapic_global_work_t* gwork = lwork->global;
  kaapi_processor_t* kproc = kaapi_get_current_processor();

  while (KAAPI_ATOMIC_READ(&gwork->workremain) !=0)
  {
    kaapi_assert_debug(KAAPI_ATOMIC_READ(&gwork->workremain) >=0);
    if (kaapic_global_work_steal(
          gwork,
          kproc,
          first,last)
       )
    {
      if (*last - *first <= gwork->wi.seq_grain) 
      {
        lwork->workdone += *last-*first;
        return 1;
      }
      _kaapi_workqueue_lock( &lwork->cr );
      kaapi_workqueue_reset(
        &lwork->cr, 
        *first+gwork->wi.seq_grain, 
        *last
      );
      _kaapi_workqueue_unlock( &lwork->cr );
      KAAPI_SET_SELF_WORKLOAD(
          kaapi_workqueue_size(&lwork->cr)
      );
      *last = *first + gwork->wi.seq_grain;
      lwork->workdone += *last-*first;
      return 1;
    }
  }
  return 0; /* means global is terminated */
}


/* 
  Return !=0 iff first and last have been filled for the next piece
  of work to execute.
  This method is dedicated to be used
*/
int kaapic_foreach_worknext(
  kaapic_local_work_t*     lwork,
  kaapi_workqueue_index_t* first,
  kaapi_workqueue_index_t* last
)
{
  kaapic_global_work_t* gwork = lwork->global;

  if (kaapi_workqueue_pop(&lwork->cr, first, last, gwork->wi.seq_grain) == 0)
  {
    KAAPI_SET_SELF_WORKLOAD(
        kaapi_workqueue_size(&lwork->cr)
    );

    lwork->workdone += *last-*first;
    return 1;
  }
  kaapi_assert_debug( kaapi_workqueue_isempty(&lwork->cr) );

  /* Empty local work: try to steal from global work 
     After updating the workremain of the global work
  */
  kaapi_assert_debug(lwork->workdone >=0);
  KAAPI_ATOMIC_SUB(&gwork->workremain, lwork->workdone);
  kaapi_assert_debug(KAAPI_ATOMIC_READ(&gwork->workremain) >=0);
  lwork->workdone = 0;

  return kaapic_foreach_globalwork_next( lwork, first, last );
}


/* exported foreach interface */
int kaapic_foreach_common
(
  kaapi_workqueue_index_t first,
  kaapi_workqueue_index_t last,
  kaapic_foreach_attr_t*  attr,
  kaapic_foreach_body_t   body_f,
  kaapic_body_arg_t*      body_args
)
{
  /* is_format true if called from kaapif_foreach_with_format */
  kaapi_thread_context_t* const self_thread = kaapi_self_thread_context();
  const int tid = self_thread->stack.proc->kid;

  int seq_grain;
  
  /* warning: interval includes j */
  kaapi_workqueue_index_t i;
  kaapi_workqueue_index_t j;
  
  /* local work */
  kaapic_local_work_t*  lwork;
  kaapic_global_work_t* gwork;

  gwork = kaapic_foreach_global_workinit(
      self_thread, 
      first, 
      last, 
      attr, 
      body_f, 
      body_args
  );
  kaapi_assert_debug(gwork !=0);

  /* */
  lwork = kaapic_foreach_local_workinit( 
                  self_thread, 
                  gwork 
  );
  kaapi_assert_debug(lwork !=0);

  /* initialize the lwork */

  /* initialize the local workqueue with the first poped state */
  if (kaapic_global_work_pop( gwork, tid, &first, &last))
  {    
#if defined(USE_KPROC_LOCK)
    kaapi_workqueue_init_with_lock(
      &lwork->cr,
      first, last,
      &kaapi_all_kprocessors[tid]->lock
    );
#else
    kaapi_atomic_initlock(&lwork->lock);
    kaapi_workqueue_init_with_lock(
      &lwork->cr, 
      first, last,
      &lwork->lock
    );
#endif
    lwork->global   = gwork;
    lwork->tid      = tid;
    gwork->lwork[tid] = lwork;
  }

  /* start adaptive region */
  lwork->context = kaapi_task_begin_adaptive(
     kaapi_threadcontext2thread(self_thread), 
     KAAPI_SC_CONCURRENT | KAAPI_SC_NOPREEMPTION,
     _kaapic_split_task,
     lwork
  );
  
  /* process locally */
#if defined(KAAPI_DEBUG)
  kaapi_processor_t* kproc = kaapi_get_current_processor();
  kaapi_assert_debug( kproc->kid == tid );
#endif

  gwork = lwork->global;
  seq_grain = gwork->wi.seq_grain;
  
  /* while there is sequential work to do in local work */
  while (kaapi_workqueue_pop(&lwork->cr, &i, &j, seq_grain) ==0)
  {
    KAAPI_SET_SELF_WORKLOAD(kaapi_workqueue_size(&lwork->cr));
    lwork->workdone += j-i;
redo_local_work:
    kaapi_assert_debug( i < j );
    /* apply w->f on [i, j[ */
    body_f((int)i, (int)j, (int)tid, body_args);
  }
  kaapi_assert_debug( kaapi_workqueue_isempty(&lwork->cr) );

  /* */
  KAAPI_SET_SELF_WORKLOAD(0);
//  printf("Master finish: work done:%i\n", lwork->workdone);
  kaapi_assert_debug(lwork->workdone >=0);
  KAAPI_ATOMIC_SUB(&gwork->workremain, lwork->workdone);
  kaapi_assert_debug(KAAPI_ATOMIC_READ(&gwork->workremain) >=0);
  lwork->workdone = 0;
  if (kaapic_foreach_globalwork_next( lwork, &i, &j ))
  {
//    printf("Master steal [%i, %i)\n", i, j);
    goto redo_local_work;
  }

  /* wait for thieves */
  kaapi_task_end_adaptive(lwork->context);

  kaapic_foreach_workend( self_thread, lwork );

#if CONFIG_FOREACH_STATS
  foreach_time += kaapif_get_time_() - time;
#endif
  return 0;
}
