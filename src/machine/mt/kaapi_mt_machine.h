/*
** kaapi_mt_machine.h
** xkaapi
** 
** Created on Tue Mar 31 15:20:42 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
** christophe.laferriere@imag.fr
** thierry.gautier@inrialpes.fr
** fabien.lementec@gmail.com / fabien.lementec@imag.fr
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
#ifndef _KAAPI_MT_MACHINE_H_
#define _KAAPI_MT_MACHINE_H_ 1

#ifndef _KAAPI_IMPL_H
#  error This file must not be directly included. Use kaapi_impl.h instead
#endif
#include <stdint.h>
#include <pthread.h>

#if defined(KAAPI_USE_SETJMP)
#  include <setjmp.h>
#elif defined(KAAPI_USE_UCONTEXT)
#  include <ucontext.h>
#endif

#if defined(KAAPI_USE_NUMA)
#  include <numa.h>
#  include <numaif.h>
#  define KAAPI_KPROCESSOR_ALIGNMENT_SIZE 4096  /* page size */
#else
#  define KAAPI_KPROCESSOR_ALIGNMENT_SIZE KAAPI_CACHE_LINE
#endif

#if defined(KAAPI_USE_CUDA)
# if KAAPI_USE_CUDA
#  include "../cuda/kaapi_cuda_proc.h"
# endif
#endif
#include "../../memory/kaapi_mem.h"

#if (SIZEOF_VOIDP == 4)
#  define KAAPI_ATOMIC_CASPTR(a, o, n) \
    KAAPI_ATOMIC_CAS( (kaapi_atomic_t*)a, (kaapi_uint32_t)o, (kaapi_uint32_t)n )
#else
#  define KAAPI_ATOMIC_CASPTR(a, o, n) \
    KAAPI_ATOMIC_CAS64( (kaapi_atomic64_t*)a, (kaapi_uint64_t)o, (kaapi_uint64_t)n )
#endif
/* ========================================================================== */



/* ============================= Documentation ============================ */
/* This is the multithreaded definition of machine type for X-Kaapi.
   The purpose of the machine.h file is to give all machine dependent functions.

   TODO: implements NUMA features:
          - allocation des processeurs sur les bons bancs memoires
          - creation des threads sur les bons bancs memoires (entre autre pile + internal thread descriptor)
          en 1/ migrant the thread courant sur le processeur cible 2/ creation du thread sur ce processeur
          3/ revenir à la fin sur le processeur main.
*/


/** \ingroup THREAD
   Define the minimum stack size. 
*/
#define KAAPI_STACK_MIN 8192


//struct kaapi_processor_t;
//typedef kaapi_uint32_t kaapi_processor_id_t;


/* ========================================================================= */
/** Termination detecting barrier
*/
extern kaapi_atomic_t kaapi_term_barrier;

/* ========================================================================= */
/** Termination detecting barrier
*/
extern volatile int kaapi_isterm;

/* ========================================================================= */
/* Functions to manipulate thread context 
   These functions are intended to be portability layer on top of ucontext.h
   interfance or setjmp/longjmp.
   A context, in Kaapi sense, is both the machine register, C-stack, Kaapi-stack
   and table of thread data specific value. A context is represented by kaapi_thread_context_t
   and allows to set flags in order to avoid saving some of the data structure.
   This feature is interesting in the case if only the kaapi stack (and the thread data specific table)
   are required to be saved and restore, without taking care of the C-stack.
   
   The function called always takes the kaapi_thread_context data structure as parameter.
   \ingroup THREAD
 */
/*@{*/

/** WS queue of thread context: used
    This data structure should be extend in case where the C-stack is required to be suspended and resumed.
    The data structure inherits from kaapi_stack_t the stackable field in order to be linked in stack.
*/
struct kaapi_wsqueuectxt_cell_t;
typedef struct kaapi_wsqueuectxt_cell_t* kaapi_wsqueuectxt_cell_ptr_t;

/** Cell of the list
*/
typedef struct kaapi_wsqueuectxt_cell_t {
  kaapi_atomic_t               state;      /* 0: in the list, 1: out the list */
  kaapi_affinity_t             affinity;   /* bit i == 1 -> can run on procid i */
  kaapi_thread_context_t*      thread;     /* */
  kaapi_wsqueuectxt_cell_ptr_t next;       /* double link list */
  kaapi_wsqueuectxt_cell_ptr_t prev;       /* shared with thief, used to link in free list */
} kaapi_wsqueuectxt_cell_t;

#define KAAPI_WSQUEUECELL_INLIST    1  /* a thread has been pushed into the queue */
#define KAAPI_WSQUEUECELL_READY     3  /* see kaapi wakeup, a suspended thread has been put ready */
#define KAAPI_WSQUEUECELL_OUTLIST   4  /* */
#define KAAPI_WSQUEUECELL_STEALLIST 8  /* the thread is under stealing */

/** Type of bloc of kaapi_liststack_node_t
*/
KAAPI_DECLARE_BLOCENTRIES(kaapi_wsqueuectxt_cellbloc_t, kaapi_wsqueuectxt_cell_t);


/** List of thread context */
typedef struct kaapi_wsqueuectxt_t {
  kaapi_wsqueuectxt_cell_ptr_t  head;
  kaapi_wsqueuectxt_cell_ptr_t  tail;
  kaapi_wsqueuectxt_cell_ptr_t  headfreecell;
  kaapi_wsqueuectxt_cell_ptr_t  tailfreecell;
  kaapi_wsqueuectxt_cellbloc_t* allocatedbloc;
} kaapi_wsqueuectxt_t;


/** lfree data structure
*/
typedef struct kaapi_lfree
{
  kaapi_thread_context_t* _front;
  kaapi_thread_context_t* _back;
} kaapi_lfree_t;


/** lready data structure
*/
typedef struct kaapi_lready
{
  kaapi_thread_context_t* _front;
  kaapi_thread_context_t* _back;
} kaapi_lready_t;

/** \ingroup WS
    Higher level context manipulation.
    This function is machine dependent.
*/
extern int kaapi_makecontext( struct kaapi_processor_t* proc, kaapi_thread_context_t* thread, 
                              void (*entrypoint)(void* arg), void* arg 
                            );

/** \ingroup WS
    Higher level context manipulation.
    Assign context onto the running processor proc.
    This function is machine dependent.
*/
extern int kaapi_setcontext( struct kaapi_processor_t* proc, kaapi_thread_context_t* thread );

/** \ingroup WS
    Higher level context manipulation.
    Get the context of the running processor proc.
    This function is machine dependent.
*/
extern int kaapi_getcontext( struct kaapi_processor_t* proc, kaapi_thread_context_t* thread );
/*@}*/


/* ============================= Kprocessor ============================ */
#define  KAAPI_USE_BITMAP_REQUEST
/* #define KAAPI_USE_CIRBUF_REQUEST */

#if defined(KAAPI_USE_BITMAP_REQUEST)

#if (KAAPI_MAX_PROCESSOR <=32)

typedef kaapi_atomic32_t kaapi_bitmap_t;
typedef kaapi_uint32_t kaapi_bitmap_value_t;

static inline void kaapi_bitmap_clear( kaapi_bitmap_t* b ) 
{ KAAPI_ATOMIC_WRITE(b, 0); }

static inline kaapi_bitmap_value_t kaapi_bitmap_swap0( kaapi_bitmap_t* b ) 
{ return KAAPI_ATOMIC_AND_ORIG(b, 0); }

static inline int kaapi_bitmap_set( kaapi_bitmap_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<KAAPI_MAX_PROCESSOR) && (i>=0) );
  KAAPI_ATOMIC_OR(b,1<<i); 
  return 0;
}

static inline int kaapi_bitmap_count( kaapi_bitmap_value_t b ) 
{ return __builtin_popcount(b); }

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero( kaapi_bitmap_value_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  unsigned int fb = __builtin_ffs( *b );
  if (fb ==0) return 0;
  *b &= ~( 1 << (fb-1) );
  return fb;
}

#elif (KAAPI_MAX_PROCESSOR <=64)

typedef kaapi_atomic64_t kaapi_bitmap_t;
typedef kaapi_uint64_t kaapi_bitmap_value_t;

static inline void kaapi_bitmap_clear( kaapi_bitmap_t* b ) 
{ KAAPI_ATOMIC_WRITE(b, 0); }

static inline kaapi_bitmap_value_t kaapi_bitmap_swap0( kaapi_bitmap_t* b ) 
{ return KAAPI_ATOMIC_AND64_ORIG(b, 0); }

static inline int kaapi_bitmap_set( kaapi_bitmap_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<KAAPI_MAX_PROCESSOR) && (i>=0) );
  KAAPI_ATOMIC_OR64(b, 1U<<i); 
  return 1;
}

static inline int kaapi_bitmap_count( kaapi_bitmap_value_t b ) 
{ return __builtin_popcountl(b); }

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero( kaapi_bitmap_value_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  unsigned int fb = __builtin_ffsl( *b );
  if (fb ==0) return 0;
  *b &= ~( 1 << (fb-1) );
  return fb;
}

#else // (KAAPI_MAX_PROCESSOR >64)

typedef kaapi_uint64_t kaapi_bitmap_t[ (KAAPI_MAX_PROCESSOR+63)/64 ];
typedef kaapi_uint64_t kaapi_bitmap_value_t[[(KAAPI_MAX_PROCESSOR+63)/64];

#endif //

/** \ingroup WS
*/
typedef struct kaapi_listrequest_t {
  kaapi_bitmap_t  bitmap __attribute__((aligned(KAAPI_CACHE_LINE)));
  kaapi_request_t requests[KAAPI_MAX_PROCESSOR+1];
} kaapi_listrequest_t __attribute__((aligned (KAAPI_CACHE_LINE)));

/* to iterate over list of request: once an iterator has been captured, 
   the bitmap is reset to 0 into the listrequest.
   All the futur iterations will be done on top of captured bitmap, not
   those associated with the listrequest which can continue to receive requests
*/
typedef struct kaapi_listrequest_iterator_t {
  kaapi_bitmap_value_t bitmap;
#if defined(KAAPI_DEBUG)
  kaapi_bitmap_value_t bitmap_t0;
#endif  
  int idcurr;
} kaapi_listrequest_iterator_t;

/* return !=0 iff the range is empty
*/
static inline int kaapi_listrequest_iterator_empty(kaapi_listrequest_iterator_t* lrrange)
{ return (lrrange->bitmap == 0) && (lrrange->idcurr == -1); }

/* return the number of entries in the range
*/
static inline int kaapi_listrequest_iterator_count(kaapi_listrequest_iterator_t* lrrange)
{ return kaapi_bitmap_count(lrrange->bitmap) + (lrrange->idcurr == -1 ? 0 : 1); }

/* get the first request of the range. range iterator should have been initialized by kaapi_listrequest_iterator_init 
*/
static inline kaapi_request_t* kaapi_listrequest_iterator_get( kaapi_listrequest_t* lrequests, kaapi_listrequest_iterator_t* lrrange )
{ return (lrrange->idcurr == -1 ? 0 : &lrequests->requests[lrrange->idcurr]); }

/* return the next entry in the request. return 0 if the range is empty.
*/
static inline kaapi_request_t* kaapi_listrequest_iterator_next( kaapi_listrequest_t* lrequests, kaapi_listrequest_iterator_t* lrrange )
{
  lrrange->idcurr = kaapi_bitmap_first1_and_zero( &lrrange->bitmap )-1;
  return (lrrange->idcurr == -1 ? 0 : &lrequests->requests[lrrange->idcurr]);
} 

/* atomically read the bitmap of the list of requests clear readed bits */
static inline void kaapi_listrequest_iterator_init(kaapi_listrequest_t* lrequests, kaapi_listrequest_iterator_t* lrrange)
{ 
  lrrange->idcurr = -1;
  lrrange->bitmap = kaapi_bitmap_swap0( &lrequests->bitmap );
#if defined(KAAPI_DEBUG)
  lrrange->bitmap_t0 = lrrange->bitmap;
#endif
  kaapi_listrequest_iterator_next( lrequests, lrrange );
}

#elif defined(KAAPI_USE_CIRBUF_REQUEST)

/** \ingroup WS
    Circular list implementation
*/
typedef struct kaapi_listrequest_t {
  kaapi_atomic_t  rpos __attribute__((aligned (KAAPI_CACHE_LINE)));
  kaapi_atomic_t  wpos __attribute__((aligned (KAAPI_CACHE_LINE)));
  kaapi_request_t requests[KAAPI_MAX_PROCESSOR+1];
} kaapi_listrequest_t __attribute__((aligned (KAAPI_CACHE_LINE)));

#endif /* type of request */


/** \ingroup WS
    This data structure defines a work stealer processor thread.
    The kid is a system wide identifier. In the current version it only contains a local counter value
    from 0 to N-1.
*/
typedef struct kaapi_processor_t {
  kaapi_thread_context_t*  thread;                        /* current thread under execution */
  kaapi_processor_id_t     kid;                           /* Kprocessor id */

  /* cache align */
  kaapi_atomic_t           lock                           /* all requests attached to each kprocessor ordered by increasing level */
    __attribute__((aligned(KAAPI_CACHE_LINE)));

  /* cache align */
  kaapi_listrequest_t      hlrequests;                    /* all requests attached to each kprocessor ordered by increasing level */

  kaapi_uint32_t           issteal;                       /* */
  
  kaapi_wsqueuectxt_t      lsuspend;                      /* list of suspended context */
  kaapi_lready_t	         lready;                        /* list of ready context, concurrent access locked by 'lock' */

  /* free list */
  kaapi_lfree_t		         lfree;                         /* queue of free context */
  int                      sizelfree;                     /* size of the queue */

  void*                    fnc_selecarg;                  /* arguments for select victim function, 0 at initialization */
  kaapi_selectvictim_fnc_t fnc_select;                    /* function to select a victim */

  void*                    dfgconstraint;                 /* TODO: for DFG constraints evaluation */

  /* performance register */
  kaapi_perf_counter_t	   perf_regs[2][KAAPI_PERF_ID_MAX];
  kaapi_perf_counter_t*	   curr_perf_regs;                /* either perf_regs[0], either perf_regs[1] */

  int			                 papi_event_set;
  unsigned int		         papi_event_count;
  kaapi_perf_counter_t     start_t[2];                    /* [KAAPI_PERF_SCHEDULE_STATE]= T1 else = Tidle */
   
  double                   t_preempt;                     /* total idle time in second pass in the preemption */           

  /* workload */
  kaapi_atomic_t	         workload;

  /* processor type */
  unsigned int			       proc_type;

  /* memory map */
  kaapi_mem_map_t          mem_map;

  /* cuda */
#if defined(KAAPI_USE_CUDA)
# if KAAPI_USE_CUDA
  kaapi_cuda_proc_t cuda_proc;
#endif
#endif /* KAAPI_USE_CUDA */

} kaapi_processor_t __attribute__ ((aligned (KAAPI_KPROCESSOR_ALIGNMENT_SIZE)));

/*
*/
struct kaapi_procinfo;
extern int kaapi_processor_init( kaapi_processor_t* kproc, const struct kaapi_procinfo*);

/*
*/
extern int kaapi_processor_setuphierarchy( kaapi_processor_t* kproc );

#if defined(KAAPI_USE_NUMA)
static inline kaapi_processor_t* kaapi_processor_allocate(void)
{
  kaapi_processor_t* const kproc = (kaapi_processor_t*)numa_alloc_local(sizeof(kaapi_processor_t));
  if (kproc == 0) return 0;

  memset(kproc, 0, sizeof(kaapi_processor_t));

  return kproc;
}

static inline void kaapi_processor_free(kaapi_processor_t* kproc)
{
  numa_free(kproc, sizeof(kaapi_processor_t));
}

#else /* ! KAAPI_USE_NUMA */

static inline kaapi_processor_t* kaapi_processor_allocate(void)
{
  return (kaapi_processor_t*)calloc(sizeof(kaapi_processor_t), 1);
}

static inline void kaapi_processor_free(kaapi_processor_t* kproc)
{
  free(kproc);
}

#endif /* KAAPI_USE_NUMA */


/* ........................................ PRIVATE INTERFACE ........................................*/
#if defined(KAAPI_DEBUG_MEM)
extern kaapi_atomic_t count_alloc_ctxt;
extern kaapi_atomic_t max_count_alloc_ctxt;
#endif
/** \ingroup TASK
    The function kaapi_context_alloc() allocates in the heap a context with a stack containing 
    at bytes for tasks and bytes for data.
    If successful, the kaapi_context_alloc() function will return a pointer to a kaapi_thread_context_t.  
    Otherwise, an error number will be returned to indicate the error.
    This function is machine dependent.
    \param kproc IN/OUT the kprocessor that make allocation
    \param size_data IN the amount of stack data.
    \retval pointer to the stack 
    \retval 0 if allocation failed
*/
extern kaapi_thread_context_t* kaapi_context_alloc( kaapi_processor_t* kproc );


/** \ingroup TASK
    The function kaapi_context_free() free the context successfuly allocated with kaapi_context_alloc.
    If successful, the kaapi_context_free() function will return zero.  
    Otherwise, an error number will be returned to indicate the error.
    This function is machine dependent.
    \param ctxt INOUT a pointer to the kaapi_thread_context_t to allocate.
    \retval EINVAL invalid argument: bad stack pointer.
*/
extern int kaapi_context_free( kaapi_thread_context_t* ctxt );


/* lfree list routines
 */

static inline void kaapi_lfree_clear(struct kaapi_processor_t* kproc)
{
  kproc->sizelfree = 0;
  kproc->lfree._front = NULL;
  kproc->lfree._back = NULL;
}

static inline int kaapi_lfree_isempty(struct kaapi_processor_t* kproc)
{
  return kproc->sizelfree == 0;
}

static inline void kaapi_lfree_push(struct kaapi_processor_t* kproc, kaapi_thread_context_t* node)
{
  kaapi_lfree_t* const list = &kproc->lfree;

  /* push the node */
  node->_next = list->_front;
  node->_prev = NULL;
  if (list->_front == NULL)
    list->_back = node;
  else
    list->_front->_prev = node;
  list->_front = node;

  /* pop back if new size exceeds max */
#  define KAAPI_MAXFREECTXT 4
  if (kproc->sizelfree >= KAAPI_MAXFREECTXT)
  {
    /* list size at least 2, no special case handling */
    node = list->_back;
    list->_back = list->_back->_prev;
    list->_back->_next = NULL;

    /* free the popped context */
    kaapi_context_free(node);

    /* see increment after */
    --kproc->sizelfree;
  }

  ++kproc->sizelfree;
}

static inline kaapi_thread_context_t* kaapi_lfree_pop(struct kaapi_processor_t* kproc)
{
  kaapi_lfree_t* const list = &kproc->lfree;
  kaapi_thread_context_t* const node = list->_front;

  --kproc->sizelfree;

  list->_front = node->_next;
  if (list->_front == NULL)
    list->_back = NULL;
  else
    list->_front->_prev = NULL;

  return node;
}


/** \ingroup WS
    Number of used cores
*/
extern kaapi_uint32_t volatile kaapi_count_kprocessors;

/** \ingroup WS
    One K-processors per core
*/
extern kaapi_processor_t** kaapi_all_kprocessors;

/** \ingroup WS
    A Kprocessor is a posix thread -> the current kprocessor
*/
extern pthread_key_t kaapi_current_processor_key;

#if defined(KAAPI_HAVE_COMPILER_TLS_SUPPORT)
  extern __thread kaapi_thread_t** kaapi_current_thread_key;
  extern __thread kaapi_threadgroup_t kaapi_current_threadgroup_key;
#endif

extern kaapi_processor_t* kaapi_get_current_processor(void);
extern kaapi_processor_id_t kaapi_get_current_kid(void);

/** \ingroup WS
*/
#define _kaapi_get_current_processor() \
  ((kaapi_processor_t*)pthread_getspecific( kaapi_current_processor_key ))

/** \ingroup WS
    Returns the current thread of tasks
*/
#define _kaapi_self_thread() \
  _kaapi_get_current_processor()->thread


/* ============================= Hierarchy ============================ */
/** The hierarchy level for this configuration of the library.
    TODO: Should be defined during the configuration of the library.
    The hierarchy level allows to code memory hierarchy.
    The default level is 1, meaning that all processors share a same memory.
    In case of coding other hierarchy level, for instance 2 where subset of processors
    are able to shared common memory (L2 cache) that are faster than main memory.

    Each of the MaxProc_level processor at the hierarchy level has a local index 
    from 0..MaxProc_level-1. The root processor of a given hierarchy level has always
    local index 0.
*/
extern kaapi_uint32_t kaapi_hierarchy_level;

/** \ingroup WS
    Definition of the neighbors type for a given processors at a given level.
*/
typedef struct kaapi_neighbors_t {
  kaapi_uint16_t      count;         /* number of neighbors */
  kaapi_processor_t** kproc;         /* list of neighbor processors, of size count */
  kaapi_uint32_t*     kplid;         /* list of neighbor processor local indexes, of size count */
} kaapi_neighbors_t;


/** \ingroup WS
    Definition of the neighbors for all processors for all level.
    The pointer points to an array of size kaapi_count_kprocessors of arrays of size kaapi_hierarchy_level.
    - kaapi_neighbors[kid][0] returns the neighbors information for the kprocessor with kid at the lowest hierarchy level.
    - kaapi_neighbors[kid][l] returns the neighbors information for the kprocessor with kid at level l.
    - kaapi_neighbors[kid][kaapi_hierarchy_level] returns the neighbors information for the kprocessor with kid 
    at the highest hierarchy level.
*/
extern kaapi_neighbors_t** kaapi_neighbors;


/** \ingroup WS
*/
extern int kaapi_setup_topology(void);




/* ========================================================================== */
/** Termination dectecting barrier
*/
/**
*/
#define kaapi_barrier_td_init( kpb, value ) \
  KAAPI_ATOMIC_WRITE( kpb, value )

/**
*/
#define kaapi_barrier_td_destroy( kpb )
  
/**
*/
#define kaapi_barrier_td_setactive( kpb, b ) \
  if (b) { KAAPI_ATOMIC_INCR( kpb ); } \
  else KAAPI_ATOMIC_DECR( kpb )

/**
*/
#define kaapi_barrier_td_isterminated( kpb ) \
  (KAAPI_ATOMIC_READ(kpb ) == 0)


/**
*/
#define kaapi_barrier_td_waitterminated( kpb ) \
  while (KAAPI_ATOMIC_READ(kpb ) != 0)


/** Private interface
*/
static inline int kaapi_isterminated(void)
{
  return kaapi_isterm !=0;
}

/** Initialize a request
    \param kpsr a pointer to a kaapi_steal_request_t
*/
static inline void kaapi_request_init( struct kaapi_processor_t* kproc, struct kaapi_request_t* pkr )
{
#if defined(KAAPI_USE_BITMAP_REQUEST)
  pkr->kid    = (kaapi_uint16_t)kproc->kid; 
#elif defined(KAAPI_USE_CIRBUF_REQUEST)
#else
#endif
}

/** \ingroup ADAPTIVE
    Reply to a steal request. If retval is !=0 it means that the request
    has successfully adapt to steal work. Else 0.
    This function is machine dependent.
*/
static inline int _kaapi_request_reply( 
  kaapi_request_t*        request, 
  int	                    flags
)
{
  kaapi_writemem_barrier();
  request->reply->status = flags;
  
#if defined(KAAPI_DEBUG)
  request->reply = 0;
#endif
  return 0;
}

/** \ingroup WS
*/
static inline int kaapi_listrequest_init( kaapi_processor_t* kproc, kaapi_listrequest_t* pklr ) 
{
  int i; 
#if defined(KAAPI_USE_BITMAP_REQUEST)
  KAAPI_ATOMIC_WRITE(&pklr->bitmap, 0);
#elif defined(KAAPI_USE_CIRBUF_REQUEST)
#endif  
  for (i=0; i<KAAPI_MAX_PROCESSOR+1; ++i)
  {  
    kaapi_request_init(kproc, &pklr->requests[i]);
  }
  return 0;
}

/** 
*/
#if (KAAPI_USE_STEALTASK_METHOD == KAAPI_STEALCAS_METHOD)
static inline int kaapi_task_casstate( kaapi_task_t* task, kaapi_task_body_t oldbody, kaapi_task_body_t newbody )
{
  kaapi_atomic_t* kat = (kaapi_atomic_t*)&task->body;
  return KAAPI_ATOMIC_CASPTR( kat, oldbody, newbody );
}
static inline int kaapi_task_cas_extrastate( kaapi_task_t* task, kaapi_task_body_t oldbody, kaapi_task_body_t newbody )
{
  kaapi_atomic_t* kat = (kaapi_atomic_t*)&task->ebody;
  return KAAPI_ATOMIC_CASPTR( kat, oldbody, newbody );
}
#elif (KAAPI_USE_STEALTASK_METHOD == KAAPI_STEALTHE_METHOD)
/*
static inline int kaapi_task_casstate( kaapi_task_t* task, kaapi_task_body_t oldbody, kaapi_task_body_t newbody )
{
  kaapi_atomic_t* kat = (kaapi_atomic_t*)&task->body;
  return KAAPI_ATOMIC_CASPTR( kat, oldbody, newbody );
}
static inline int kaapi_task_casstate( kaapi_task_t* task, kaapi_task_body_t oldbody, kaapi_task_body_t newbody )
{
  if (task->body != oldbody ) return 0;
  kaapi_task_setbody(task, newbody );
  return 1;
}
*/
#else
#  warning "NOT IMPLEMENTED"
#endif

/* ============================= Private functions, machine dependent ============================ */
/* */
extern kaapi_uint64_t kaapi_perf_thread_delayinstate(kaapi_processor_t* kproc);

/** Post a request to a given k-processor
  This method posts a request to victim k-processor. 
  \param kproc the sender of the request 
  \param reply where to receive result
  \param dest the receiver (victim) of the request
  \param return 0 if the request has been successully posted
  \param return !=0 if the request been not been successully posted and the status of the request contains the error code
*/
static inline int kaapi_request_post( kaapi_processor_id_t thief_kid, kaapi_reply_t* reply, kaapi_processor_t* victim )
{
  kaapi_request_t* req;
  if (victim ==0) return EINVAL;

  reply->status = KAAPI_REQUEST_S_POSTED;

#if defined(KAAPI_USE_BITMAP_REQUEST)
  req = &victim->hlrequests.requests[thief_kid];
  /* here do not write kid, because it was persistant to all local thread */
  req->reply = reply;
  kaapi_writemem_barrier();
  kaapi_bitmap_set( &victim->hlrequests.bitmap, thief_kid );
  return 0;
#elif defined(KAAPI_USE_CIRBUF_REQUEST)
#else
#error "Not implemented"
#endif
#if 0
#if defined(KAAPI_USE_PERFCOUNTER)
  req->delay       = kaapi_perf_thread_delayinstate(kproc);
#else  
  req->delay       = 0;
#endif
#endif
}


/** push: LIFO order with respect to pop. Only owner may push
*/
static inline int kaapi_wsqueuectxt_empty( kaapi_processor_t* kproc )
{ return (kproc->lsuspend.head ==0); }

/**
*/
extern int kaapi_wsqueuectxt_init( kaapi_wsqueuectxt_t* ls );

/**
*/
extern int kaapi_wsqueuectxt_destroy( kaapi_wsqueuectxt_t* ls );

/** \ingroup WS
   Push a ctxt. Must be call by the owner of the queue in case of concurrent execution.
   Return 0 in case of success
   Return ENOMEM if allocation failed
*/
extern int kaapi_wsqueuectxt_push( kaapi_processor_t* kproc, kaapi_thread_context_t* thread );

/** \ingroup WS
   Steal a ctxt on a specific cell
   Return a pointer to the stolen thread in case of success
   Return 0 if the thread was already stolen
*/
extern kaapi_thread_context_t* kaapi_wsqueuectxt_steal_cell( kaapi_wsqueuectxt_cell_t* cell );


#endif /* _KAAPI_MT_MACHINE_H */
