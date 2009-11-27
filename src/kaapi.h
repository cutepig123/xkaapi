/*
** kaapi.h
** xkaapi
** 
** Created on Tue Mar 31 15:19:14 2009
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
#ifndef _KAAPI_H
#define _KAAPI_H 1

#include <stdint.h>
#include <errno.h>
#include "kaapi_error.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(KAAPI_USE_VARIADIC_MACRO)
#  define KAAPI_USE_VARIADIC_MACRO 1
#endif

#if !defined(KAAPI_MAX_DATA_ALIGNMENT)
#  define KAAPI_MAX_DATA_ALIGNMENT 8
#endif

#ifdef __linux__
#  define KAAPI_USE_LINUX 1
#  ifdef HAVE_UCONTEXT_H
#    define KAAPI_USE_UCONTEXT
#  elif HAVE_SETJMP_H
#    error "Not implemented yet"
#    define KAAPI_USE_SETJMP
#  endif
#endif

#if !defined(KAAPI_USE_APPLE)
#  ifdef __APPLE__
#    define KAAPI_USE_APPLE 1
#    ifdef HAVE_SETJMP_H
#      define KAAPI_USE_SETJMP
#    elif HAVE_UCONTEXT_H
#      define KAAPI_USE_UCONTEXT
#    endif
#  endif
#endif

#ifdef __i386__
#  define KAAPI_USE_ARCH_X86 1
#endif

#ifdef __x86_64
#  define KAAPI_USE_ARCH_X86_64 1
#  define KAAPI_USE_ARCH_X86 1
#endif

#ifdef __ia64__
#  define KAAPI_USE_ARCH_IA64 1
#endif

#ifdef __PPC
#  define KAAPI_USE_ARCH_PPC 1
#endif

#ifdef __arm
#  define KAAPI_USE_ARCH_ARM 1
#endif
 
#ifdef __sparc__
#  error "Unsupported Architecture"
#endif

#ifdef __mips
#  error "Unsupported Architecture"
#endif


/* Kaapi name for stdint typedefs.
 */
typedef uintptr_t kaapi_uintptr_t;
typedef uint8_t   kaapi_uint8_t;
typedef uint16_t  kaapi_uint16_t;
typedef uint32_t  kaapi_uint32_t;
typedef uint64_t  kaapi_uint64_t;

typedef intptr_t  kaapi_intptr_t;
typedef int8_t    kaapi_int8_t;
typedef int16_t   kaapi_int16_t;
typedef int32_t   kaapi_int32_t;
typedef int64_t   kaapi_int64_t;

/** Atomic type
*/
typedef struct kaapi_atomic_t {
#if defined(KAAPI_USE_APPLE)
  volatile kaapi_int32_t  _counter;
#else
  volatile kaapi_uint32_t _counter;
#endif
} kaapi_atomic_t;
typedef kaapi_atomic_t kaapi_atomic32_t;


typedef struct kaapi_atomic64_t {
#if defined(KAAPI_USE_APPLE)
  volatile kaapi_int64_t  _counter;
#else
  volatile kaapi_uint64_t _counter;
#endif
} kaapi_atomic64_t;


/** \ingroup THREAD
   Define the minimum stack size. 
*/
#define KAAPI_STACK_MIN 8192

/** Define the cache line size. 
*/
#define KAAPI_CACHE_LINE 64

/** Maximal number of architecture
    Current naming is:
    - KAAPI_PROC_TYPE_CPU:   core of multicore machine
    - KAAPI_PROC_TYPE_GPU:   core of GPU card (Nvidia GPU)
    - KAAPI_PROC_TYPE_MPSOC: core of a MPSoC chip
*/
#define KAAPI_MAX_ARCHITECTURE 3

#define KAAPI_PROC_TYPE_NO    0x0
#define KAAPI_PROC_TYPE_CPU   0x1
#define KAAPI_PROC_TYPE_GPU   0x2
#define KAAPI_PROC_TYPE_MPSOC 0x3

/* ========================================================================== */

/** \ingroup WS
    Get the workstealing concurrency number, i.e. the number of kernel
    activities to execute the user level thread. 
    This function is machine dependent.
    \retval the number of active threads to steal tasks
 */
extern int kaapi_getconcurrency (void);

/** \ingroup WS
    Set the workstealing concurrency number, i.e. the number of kernel
    activities to execute the user level thread.
    If successful, the kaapi_setconcurrency() function will return zero.  
    Otherwise, an error number will be returned to indicate the error.
    This function is machine dependent.
    \retval EINVAL if no memory ressource is available
    \retval ENOMEM if no memory ressource is available
    \retval EAGAIN if the system laked the necessary ressources to create another thread
    on return, the concurrency number may has been set to a different number than requested.
 */
extern int kaapi_setconcurrency (unsigned int concurrency);

/* ========================================================================== */
/** kaapi_advance.
    The function kaapi_advance() makes progress of steal requests
*/
extern int kaapi_advance(void);

/* ========================================================================== */
/** kaapi_get_elapsedtime
    The function kaapi_get_elapsedtime() will return the elapsed time since an epoch.
*/
extern double kaapi_get_elapsedtime(void);


/* ========================================================================== */
/** COmpute a hash value from a string
*/
extern kaapi_uint32_t kaapi_hash_value(const char * data);

/* ========================================================================= */
/* Shared object and access mode                                             */
/* ========================================================================= */
/** Kaapi Access mode
    \ingroup DFG
*/
/*@{*/
typedef enum kaapi_access_mode_t {
  KAAPI_ACCESS_MODE_VOID= 0,        /* 0000 0000 : */
  KAAPI_ACCESS_MODE_V   = 1,        /* 0000 0001 : */
  KAAPI_ACCESS_MODE_R   = 2,        /* 0000 0010 : */
  KAAPI_ACCESS_MODE_W   = 4,        /* 0000 0100 : */
  KAAPI_ACCESS_MODE_CW  = 8,        /* 0000 1000 : */
  KAAPI_ACCESS_MODE_P   = 16,       /* 0001 0000 : */
  KAAPI_ACCESS_MODE_RW  = KAAPI_ACCESS_MODE_R|KAAPI_ACCESS_MODE_W
} kaapi_access_mode_t;
/*@}*/

/** Kaapi access mode mask
    \ingroup DFG
*/
#define KAAPI_ACCESS_MASK_MODE   0x1F
#define KAAPI_ACCESS_MASK_MODE_R 0x2
#define KAAPI_ACCESS_MASK_MODE_W 0x4
#define KAAPI_ACCESS_MASK_MODE_P 0x10


/* ========================================================================== */
struct kaapi_task_t;
struct kaapi_stack_t;
/** Task body
    \ingroup TASK
*/
typedef void (*kaapi_task_body_t)(struct kaapi_task_t* /*task*/, struct kaapi_stack_t* /* stack */);

/* ========================================================================= */
/* Format of a task                                                          */
/* ========================================================================= */
/** \ingroup DFG
     Format identifier of data structure or task
*/
typedef kaapi_uint32_t kaapi_format_id_t;

/** \ingroup DFG
     Offset to access to parameter of a task
*/
typedef kaapi_uint32_t kaapi_offset_t;


/** \ingroup TASK
    Kaapi task format
    The format should be 1/ declared 2/ register before any use in task.
    The format object is only used in order to interpret stack of task.    
*/
typedef struct kaapi_format_t {
  kaapi_format_id_t          fmtid;                                   /* identifier of the format */
  short                      isinit;                                  /* ==1 iff initialize */
  const char*                name;                                    /* debug information */
  
  /* case of format for a structure or for a task */
  kaapi_uint32_t             size;                                    /* sizeof the object */  
  void                       (*cstor)( void* dest);
  void                       (*dstor)( void* dest);
  void                       (*cstorcopy)( void* dest, const void* src);
  void                       (*copy)( void* dest, const void* src);
  void                       (*assign)( void* dest, const void* src);

  /* only if it is a format of a task  */
  kaapi_task_body_t          entrypoint[KAAPI_MAX_ARCHITECTURE];      /* maximum architecture considered in the configuration */
  int                        count_params;                            /* number of parameters */
  kaapi_access_mode_t        *mode_params;                            /* only consider value with mask 0xF0 */
  kaapi_offset_t             *off_params;                             /* access to the i-th parameter: a value or a shared */
  struct kaapi_format_t*     *fmt_params;                             /* format for each params */
  kaapi_uint32_t             *size_params;                            /* sizeof of each params */

  struct kaapi_format_t      *next_bybody;                            /* link in hash table */
  struct kaapi_format_t      *next_byfmtid;                           /* link in hash table */
} kaapi_format_t;

/** predefined format 
*/
/*@{*/
extern kaapi_format_t kaapi_shared_format;
extern kaapi_format_t kaapi_char_format;
extern kaapi_format_t kaapi_short_format;
extern kaapi_format_t kaapi_int_format;
extern kaapi_format_t kaapi_long_format;
extern kaapi_format_t kaapi_longlong_format;
extern kaapi_format_t kaapi_uchar_format;
extern kaapi_format_t kaapi_ushort_format;
extern kaapi_format_t kaapi_uint_format;
extern kaapi_format_t kaapi_ulong_format;
extern kaapi_format_t kaapi_ulonglong_format;
extern kaapi_format_t kaapi_float_format;
extern kaapi_format_t kaapi_double_format;
/*@}*/


/* ========================================================================= */
/* Task and stack interface                                                  */
/* ========================================================================= */
struct kaapi_processor_t;

/* Stack identifier */
typedef kaapi_uint32_t kaapi_stack_id_t;

/** \ingroup WS
    Client side of the request
*/
typedef struct kaapi_reply_t {
  volatile kaapi_uint16_t  status;          /* reply status */
  struct kaapi_stack_t*    data;            /* output data */
} __attribute__((aligned (KAAPI_CACHE_LINE))) kaapi_reply_t;


/** \ingroup WS
    Server side of a request send by a processor.
    This data structure is pass in parameter of the splitter function.
*/
typedef struct kaapi_request_t {
  kaapi_uint16_t           status;         /* server status */
  struct kaapi_reply_t*    reply;          /* caller status */
  struct kaapi_stack_t*    stack;          /* stack of the thief where to store result of the steal operation */
} __attribute__((aligned (KAAPI_CACHE_LINE))) kaapi_request_t;


/** Flags for task
   \ingroup TASK 
   DEFAULT flags is for normal task that can be stolen and executed every where.
    - KAAPI_TASK_STICKY: if set, the task could not be theft else the task can (default).
    - KAAPI_TASK_ADAPTIVE: if set, the task is an adaptative task that could be stolen or preempted.
    - KAAPI_TASK_LOCALITY: if set, the task as locality constraint defined in locality data field.
    - KAAPI_TASK_SYNC: if set, the task does engender synchronisation, victim should stop on a stolen task
    before continuing the fast execution using RFO schedule.
*/
/*@{*/
#define KAAPI_TASK_STICKY     0x1
#define KAAPI_TASK_ADAPTIVE   0x2
#define KAAPI_TASK_LOCALITY   0x4
#define KAAPI_TASK_SYNC       0x8
#define KAAPI_TASK_DFG        KAAPI_TASK_SYNC
#define KAAPI_TASK_MASK_FLAGS 0xf

#define KAAPI_TASK_MASK_PROC  0x70  /* bits 0x70 ie bit 5 to bit 7 encode the processor type of the task */
#define KAAPI_TASK_PROC_CPU   0x10
#define KAAPI_TASK_PROC_GPU   0x20
#define KAAPI_TASK_PROC_MPSOC 0x40
/*@}*/

/** Task splitter
    \ingroup TASK
    A splitter should always return the number of work returns to the list of requests.
*/
typedef int (*kaapi_task_splitter_t)(struct kaapi_stack_t* /*stack */, struct kaapi_task_t* /* task */, int /*count*/, struct kaapi_request_t* /*array*/);


/** Task reducer
    \ingroup TASK
*/
typedef int (*kaapi_task_reducer_t)(struct kaapi_task_t* /* task */, void* thief, ...);

/** Kaapi stack of tasks definition
   \ingroup STACK
   The stack store list of tasks as well as a stack of data.
   Both sizes are fixed at initialization of the stack object.
   The stack is truly a stack when used in conjonction with frame.
   A frame capture the state (pc, sp, sp_data) of the stack in order
   to restore it. The implementation also used kaapi_retn_body in order 
   to postpone the restore operation after a set of tasks (see kaapi_stack_taskexecall).

   Before and after the execution of a task, the state of the computation is only
   defined by the stack state (pc, sp, sp_data and the content of the stack). 
   The C-stack doesnot need to be saved.
   
   \TODO save also the C-stack if we try to suspend execution during a task execution
*/
typedef struct kaapi_stack_t {
  volatile int             *hasrequest;     /** points to the k-processor structure */
  struct kaapi_task_t      *pc;             /** task counter: next task to execute, 0 if empty stack */
  struct kaapi_task_t      *sp;             /** stack counter: next free task entry */
  struct kaapi_task_t*      end_sp;         /** past the last stack counter: next entry after the last task in stack array */
  struct kaapi_task_t*      task;           /** stack of tasks */

  char*                     sp_data;        /** stack counter for the data: next free data entry */
  char*                     end_sp_data;    /** past the last stack counter: next entry after the last task in stack array */
  char*                     data;           /** stack of data with the same scope than task */

  kaapi_request_t          *requests;       /** points to the processor structure */
  kaapi_uint32_t            size;           /** size of the data structure */
  struct kaapi_stack_t*     _next;          /** to be stackable */
  struct kaapi_processor_t* _proc;          /** (internal) access to the attached processor */
} kaapi_stack_t;


/** Kaapi frame definition
   \ingroup STACK
*/
typedef struct kaapi_frame_t {
    struct kaapi_task_t* pc;
    struct kaapi_task_t* sp;
    char*                sp_data;
} kaapi_frame_t;


/* ========================================================================= */
/* What is a task ?                                                          */
/* ========================================================================= */
/** Kaapi task definition
    \ingroup TASK
    A Kaapi task is the basic unit of computation. It has a constant size including some task's specific values.
    Variable size task has to store pointer to the memory where found extra data.
    The body field is the pointer to the function to execute. The special value 0 correspond to a nop instruction.
*/
typedef struct kaapi_task_t {
  kaapi_task_body_t     body;      /** C function that represent the body to execute */
  kaapi_task_splitter_t splitter;  /** C function that represent the body to split a task */
  void*                 sp;        /** data stack pointer of the data frame for the task  */
  kaapi_format_t*       format;    /** format, 0 if not def !!!  */
  kaapi_uint32_t        flag;      /** flags: after a padding on 64 bit architecture !!!  */
} kaapi_task_t;


/** Extent data structure for adaptive task
*/
typedef struct kaapi_taskadaptive_t {
  void*                  user_sp;           
  kaapi_atomic_t         thievescount;
} kaapi_taskadaptive_t;


/* ========================================================================= */
/* Shared object and access mode                                             */
/* ========================================================================= */

/** \ingroup DFG
*/
/*@{*/
#define KAAPI_ACCESS_GET_MODE( m ) \
  ((m) & KAAPI_ACCESS_MASK_MODE )

#define KAAPI_ACCESS_IS_READ( m ) \
  ((m) & KAAPI_ACCESS_MASK_MODE_R)

#define KAAPI_ACCESS_IS_WRITE( m ) \
  ((m) & KAAPI_ACCESS_MASK_MODE_W)

#define KAAPI_ACCESS_IS_POSTPONED( m ) \
  ((m) & KAAPI_ACCESS_MASK_MODE_P)

#define KAAPI_ACCESS_IS_ONLYWRITE( m ) \
  (KAAPI_ACCESS_IS_WRITE(m) && !KAAPI_ACCESS_IS_READ(m))

#define KAAPI_ACCESS_IS_READWRITE( m ) \
  (KAAPI_ACCESS_IS_WRITE(m) && KAAPI_ACCESS_IS_READ(m))
/*@}*/


/** \ingroup DFG
*/
typedef struct kaapi_gd_t {
  kaapi_access_mode_t last_mode;    /* last access mode to the data */
  void*               last_version; /* last verion of the data, 0 if not ready */
}  __attribute__((aligned(KAAPI_MAX_DATA_ALIGNMENT))) kaapi_gd_t;


/** \ingroup DFG
    Kaapi access
*/
typedef struct kaapi_access_t {
  void* data;
  void* version;
} kaapi_access_t;

#define kaapi_data(type, a)\
  ((type*)a.data)


/** \ingroup DFG
    Splitter for DFG task
*/
int kaapi_task_splitter_dfg(kaapi_stack_t* stack, kaapi_task_t* task, int count, struct kaapi_request_t* array);


/* ========================================================================= */
/* Interface                                                                 */
/* ========================================================================= */


/** \ingroup TASK
    Return the flags of the task
*/
#define kaapi_task_getflags(task) ((task)->flag & KAAPI_TASK_MASK_FLAGS)

/** \ingroup TASK
    Return the flags of the task
*/
#define kaapi_task_setflags(task, f) ((task)->flag |= f)


/** \ingroup TASK
    Return the proc type of the task
*/
#define kaapi_task_proctype(task) ((task)->flag & KAAPI_TASK_MASK_PROC)


/** \ingroup TASK
    Return a pointer to parameter of the task (void*) pointer
*/
static inline void* kaapi_task_getargs(kaapi_task_t* task) 
{
  if (task->flag & KAAPI_TASK_ADAPTIVE) 
    return ((kaapi_taskadaptive_t*)task->sp)->user_sp;
  return task->sp;
}

/** \ingroup TASK
    Return a reference to parameter of the task (type*) pointer
*/
#define kaapi_task_getargst(task,type) ((type*)kaapi_task_getargs(task))



/** \ingroup TASK
    Set the pointer to parameter of the task (void*) pointer
*/
static inline void* kaapi_task_setargs(kaapi_task_t* task,void* arg) 
{
  if (task->flag & KAAPI_TASK_ADAPTIVE) 
    return ((kaapi_taskadaptive_t*)task->sp)->user_sp = arg;
  return task->sp = arg;
}

/** \ingroup TASK
    Set the pointer to parameter of the task (void*) pointer
*/
static inline kaapi_task_body_t kaapi_task_setbody(kaapi_task_t* task, kaapi_task_body_t body )
{
  return task->body = body;
}


/** Body of the nop task 
    \ingroup TASK
*/
extern void kaapi_nop_body( kaapi_task_t*, kaapi_stack_t*);

/** Body of the task that restore the frame pointer 
    \ingroup TASK
*/
extern void kaapi_retn_body( kaapi_task_t*, kaapi_stack_t*);

/** Body of the task that mark a task to suspend execution
    \ingroup TASK
*/
extern void kaapi_suspend_body( kaapi_task_t*, kaapi_stack_t*);

/** Body of the task that do signal to a task after steal op
    \ingroup TASK
*/
extern void kaapi_tasksig_body( kaapi_task_t* task, kaapi_stack_t* stack);


/** \ingroup TASK
    The function kaapi_task_isstealable() will return non-zero value iff the task may be stolen.
    \param task IN a pointer to the kaapi_task_t to test.
*/
inline static int kaapi_task_isstealable(const kaapi_task_t* task)
{ return (task !=0) && (task->body != &kaapi_retn_body) && (task->body != &kaapi_suspend_body)
     && !(task->flag & KAAPI_TASK_STICKY); }

/** \ingroup TASK
    The function kaapi_task_haslocality() will return non-zero value iff the task has locality constraints.
    In this case, the field locality my be read to resolved locality constraints.
    \param task IN a pointer to the kaapi_task_t to test.
*/
inline static int kaapi_task_haslocality(const kaapi_task_t* task)
{ return (task !=0) && (task->flag & KAAPI_TASK_LOCALITY); }

/** \ingroup TASK
    The function kaapi_task_isadaptive() will return non-zero value iff the task is an adaptive task.
    \param task IN a pointer to the kaapi_task_t to test.
*/
inline static int kaapi_task_isadaptive(const kaapi_task_t* task)
{ return (task !=0) && (task->flag & KAAPI_TASK_ADAPTIVE); }


/** \ingroup TASK
    The function kaapi_task_issync() will return non-zero value iff the such stolen task will introduce data dependency
    \param task IN a pointer to the kaapi_task_t to test.
*/
inline static int kaapi_task_issync(const kaapi_task_t* task)
{ return (task !=0) && (task->flag & KAAPI_TASK_SYNC); }


/** \ingroup STACK
    Return pointer to the self stack
*/
extern kaapi_stack_t* kaapi_self_stack (void);


/** \ingroup STACK
    The function kaapi_stack_init() initializes the stack using the buffer passed in parameter. 
    The buffer must point to a memory region with at least count bytes allocated.
    If successful, the kaapi_stack_init() function will return zero and the buffer should
    never be used again.
    Otherwise, an error number will be returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t to initialize.
    \param size_task_buffer IN the size in bytes of the buffer for the tasks.
    \param task_buffer INOUT the buffer to used to store the stack of tasks.
    \param size_data_buffer IN the size in bytes of the buffer for the data.
    \param data_buffer INOUT the buffer to used to store the stack of data.
    \retval EINVAL invalid argument: bad stack pointer or count is not enough to store at least one task or buffer is 0.
*/
extern int kaapi_stack_init( kaapi_stack_t* stack,  
                             kaapi_uint32_t size_task_buffer, void* task_buffer,
                             kaapi_uint32_t size_data_buffer, void* data_buffer 
);

/** \ingroup STACK
    The function kaapi_stack_clear() clears the stack.
    If successful, the kaapi_stack_clear() function will return zero.
    Otherwise, an error number will be returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t to clear.
    \retval EINVAL invalid argument: bad stack pointer.
*/
extern int kaapi_stack_clear( kaapi_stack_t* stack );

/** \ingroup STACK
    The function kaapi_stack_isempty() will return non-zero value iff the stack is empty. Otherwise return 0.
    If the argument is a bad pointer then the function kaapi_stack_isempty returns a non value as if the stack was empty.
    \param stack IN the pointer to the kaapi_stack_t data structure. 
    \retval !=0 if the stack is empty
    \retval 0 if the stack is not empty or argument is an invalid stack pointer
*/
static inline int kaapi_stack_isempty(const kaapi_stack_t* stack)
{
  return (stack ==0) || (stack->pc >= stack->sp);
}

/** \ingroup STACK
    The function kaapi_stack_pushdata() will return the pointer to the next top data.
    The top data is not yet into the stack.
    If successful, the kaapi_stack_pushdata() function will return a pointer to the next data to push.
    Otherwise, an 0 is returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval a pointer to the next task to push or 0.
*/
#if defined(KAAPI_DEBUG)
static inline void* kaapi_stack_pushdata(kaapi_stack_t* stack, kaapi_uint32_t count)
{
  void* retval;
  if (stack ==0) return 0;
  if (stack->sp_data+count >= stack->end_sp_data) return 0;
  retval = stack->sp_data;
  stack->sp_data += count;
  return retval;
}
#else
static inline void* kaapi_stack_pushdata(kaapi_stack_t* stack, kaapi_uint32_t count)
{
  void* retval = stack->sp_data;
  stack->sp_data += count;
  return retval;
}
#endif

/** \ingroup STACK
    The function kaapi_stack_pushdata() will return the pointer to the next top data.
    The top data is not yet into the stack.
    If successful, the kaapi_stack_pushdata() function will return a pointer to the next data to push.
    Otherwise, an 0 is returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval a pointer to the next task to push or 0.
*/
#if defined(KAAPI_DEBUG)
static inline kaapi_access_t kaapi_stack_pushshareddata(kaapi_stack_t* stack, kaapi_uint32_t count)
{
  kaapi_access_t retval = {0, 0};
  kaapi_gd_t* gd;
  if (stack ==0) return retval;
  if (stack->sp_data+count+sizeof(kaapi_gd_t) >= stack->end_sp_data) return retval;

  gd              = (kaapi_gd_t*)stack->sp_data;
  gd->last_mode   = KAAPI_ACCESS_MODE_VOID;
  stack->sp_data += sizeof(kaapi_gd_t);
  retval.data     = stack->sp_data;
  gd->last_version = retval.data;
  stack->sp_data += count;
  return retval;
}
#else
static inline kaapi_access_t kaapi_stack_pushshareddata(kaapi_stack_t* stack, kaapi_uint32_t count)
{
  kaapi_access_t retval = {0, 0};
  kaapi_gd_t* gd  = (kaapi_gd_t*)stack->sp_data;
  gd->last_mode   = KAAPI_ACCESS_MODE_VOID;
  stack->sp_data += sizeof(kaapi_gd_t);
  retval.data     = stack->sp_data;
  gd->last_version = retval.data;
  stack->sp_data += count;
  return retval;
}
#endif

#define kaapi_stack_topdata(stack) \
    (stack)->sp_data

/** \ingroup STACK
    The function kaapi_stack_bottom() will return the top task.
    The top task is not part of the stack.
    If successful, the kaapi_stack_top() function will return a pointer to the next task to push.
    Otherwise, an 0 is returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval a pointer to the next task to push or 0.
*/
#if defined(KAAPI_DEBUG)
static inline kaapi_task_t* kaapi_stack_bottomtask(kaapi_stack_t* stack) 
{
  if (stack ==0) return 0;
  if (stack->sp <= stack->pc) return 0;
  return stack->task;
}
#else
#define kaapi_stack_bottomtask(stack) \
  (stack)->task
#endif

/** \ingroup STACK
    The function kaapi_stack_top() will return the top task.
    The top task is not part of the stack.
    If successful, the kaapi_stack_top() function will return a pointer to the next task to push.
    Otherwise, an 0 is returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval a pointer to the next task to push or 0.
*/
#if defined(KAAPI_DEBUG)
static inline kaapi_task_t* kaapi_stack_toptask(kaapi_stack_t* stack) 
{
  if (stack ==0) return 0;
  if (stack->sp == stack->end_sp) return 0;
  return stack->sp;
}
#else
#  define kaapi_stack_toptask(stack) \
    (stack)->sp
#endif

/** \ingroup STACK
    The function kaapi_stack_push() pushes the top task into the stack.
    If successful, the kaapi_stack_push() function will return zero.
    Otherwise, an error number will be returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval EINVAL invalid argument: bad stack pointer.
*/
#if defined(KAAPI_DEBUG)
static inline int kaapi_stack_pushtask(kaapi_stack_t* stack)
{
  if (stack ==0) return EINVAL;
  if (stack->sp == stack->end_sp) return EINVAL;
  ++stack->sp;
  return 0;
}
#else
#  define kaapi_stack_pushtask(stack) \
        ++(stack)->sp
#endif

/** \ingroup STACK
    The function kaapi_stack_poptask() 
*/
#if defined(KAAPI_DEBUG)
static inline int kaapi_stack_poptask(kaapi_stack_t* stack)
{
  if (stack ==0) return EINVAL;
  if (stack->sp == stack->pc) return EINVAL;
  --stack->sp;
  return 0;
}
#else
#define kaapi_stack_poptask(stack) \
  --(stack)->sp
#endif


/** \ingroup TASK
    Initialize a task with given flag
*/
static inline int kaapi_task_init( kaapi_stack_t* stack, kaapi_task_t* task, kaapi_uint32_t flag ) 
{
#if defined(KAAPI_DEBUG)
  task->flag   = flag & KAAPI_TASK_MASK_FLAGS;
  task->format = 0;
#else
  task->flag   = flag;
#endif
  if (flag & KAAPI_TASK_ADAPTIVE)
  {
    kaapi_taskadaptive_t* ta = (kaapi_taskadaptive_t*) kaapi_stack_pushdata( stack, sizeof(kaapi_taskadaptive_t) );
    ta->user_sp = 0;
    ta->thievescount._counter = 0;
    kaapi_assert_debug( ta !=0 );
    task->sp = ta;
    task->body     = 0;
    task->splitter = 0;
  }
  else if (flag & KAAPI_TASK_DFG)
    task->splitter = &kaapi_task_splitter_dfg;
  return 0;
}

/** \ingroup TASK
    Initialize a task with given flag
*/
static inline int kaapi_task_initadaptive( kaapi_stack_t* stack, kaapi_task_t* task, kaapi_uint32_t flag ) 
{
#if defined(KAAPI_DEBUG)
  task->flag   = (flag | KAAPI_TASK_ADAPTIVE)& KAAPI_TASK_MASK_FLAGS;
  task->format = 0;
#else
  task->flag   = flag | KAAPI_TASK_ADAPTIVE;
#endif
  kaapi_taskadaptive_t* ta = (kaapi_taskadaptive_t*) kaapi_stack_pushdata( stack, sizeof(kaapi_taskadaptive_t) );
  ta->user_sp    = 0;
  ta->thievescount._counter = 0;
  kaapi_assert_debug( ta !=0 );
  task->sp       = ta;
  task->body     = 0;
  task->splitter = 0;
}

#if defined(KAAPI_DEBUG)
#  define kaapi_task_initdfg( stack, task, taskbody, buffer ) \
  do {  \
    (task)->body     = taskbody;\
    (task)->splitter = &kaapi_task_splitter_dfg;\
    (task)->sp       = (buffer);\
    (task)->flag     = KAAPI_TASK_DFG; \
    (task)->format   = 0; \
  } while (0)
#else
#  define kaapi_task_initdfg( stack, task, taskbody, buffer ) \
    (task)->body     = taskbody;\
    (task)->splitter = &kaapi_task_splitter_dfg; \
    (task)->sp       = (buffer);\
    (task)->flag     = KAAPI_TASK_DFG
#endif


/** \ingroup STACK
    The function kaapi_stack_save_frame() saves the current frame of a stack into
    the frame data structure.
    If successful, the kaapi_stack_save_frame() function will return zero.
    Otherwise, an error number will be returned to indicate the error.
    \param stack IN a pointer to the kaapi_stack_t data structure.
    \param frame OUT a pointer to the kaapi_frame_t data structure.
    \retval EINVAL invalid argument: bad pointer.
*/
static inline int kaapi_stack_save_frame( kaapi_stack_t* stack, kaapi_frame_t* frame)
{
#if defined(KAAPI_DEBUG)
  if ((stack ==0) || (frame ==0)) return EINVAL;
#endif
  frame->pc      = stack->pc;
  frame->sp      = stack->sp;
  frame->sp_data = stack->sp_data;
  return 0;  
}

/** \ingroup STACK
    The function kaapi_stack_restore_frame() restores the frame context of a stack into
    the stack data structure.
    If successful, the kaapi_stack_restore_frame() function will return zero.
    Otherwise, an error number will be returned to indicate the error.
    \param stack OUT a pointer to the kaapi_stack_t data structure.
    \param frame IN a pointer to the kaapi_frame_t data structure.
    \retval EINVAL invalid argument: bad pointer.
*/
static inline int kaapi_stack_restore_frame( kaapi_stack_t* stack, const kaapi_frame_t* frame)
{
#if defined(KAAPI_DEBUG)
  if ((stack ==0) || (frame ==0)) return EINVAL;
#endif
  stack->pc      = frame->pc;
  stack->sp      = frame->sp;
  stack->sp_data = frame->sp_data;
  return 0;  
}

/** TODO
*/
static inline int kaapi_stack_pushretn( kaapi_stack_t* stack, const kaapi_frame_t* frame)
{
  kaapi_task_t* retn;
  kaapi_frame_t* arg_retn;
  retn = kaapi_stack_toptask(stack);
  kaapi_task_init(stack, retn, KAAPI_TASK_STICKY);
  retn->body  = &kaapi_retn_body;
  arg_retn = (kaapi_frame_t*)kaapi_stack_pushdata(stack, sizeof(kaapi_frame_t));
  retn->sp = (void*)arg_retn;
  *arg_retn = *frame;
  kaapi_stack_pushtask(stack);
  return 0;
}

/** \ingroup STACK
    The function kaapi_stack_execone() execute the given task only.
    If successful, the kaapi_stack_execone() function will return zero.
    Otherwise, an error number will be returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval EINVAL invalid argument: bad stack pointer
    \retval EWOULDBLOCK the execution of the task will block the control flow.
    \retval EINTR the control flow has received a KAAPI interrupt.
*/
static inline int kaapi_stack_execone(kaapi_stack_t* stack, kaapi_task_t* task)
{
#if defined(KAAPI_DEBUG)
  if (stack ==0) return EINVAL;
  if (task ==0) return EINVAL;
#endif
  if (task->body == &kaapi_suspend_body) 
    return EWOULDBLOCK;
  else if (task->body !=0) 
    (*task->body)(task, stack);
  task->body = 0;    
  return 0;
}

/** \ingroup STACK
    The function kaapi_stack_execchild() execute the given and all childs task.
    If successful, the kaapi_stack_execchild() function will return zero.
    Otherwise, an error number will be returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval EINVAL invalid argument: bad stack pointer
    \retval EWOULDBLOCK the execution of the task will block the control flow.
    \retval EINTR the control flow has received a KAAPI interrupt.
*/
extern int kaapi_stack_execchild(kaapi_stack_t* stack, kaapi_task_t* task);

/** \ingroup STACK
    The function kaapi_stack_execall() execute all the tasks in the stack following
    the RFO order.
    If successful, the kaapi_stack_execall() function will return zero and the stack is empty.
    Otherwise, an error number will be returned to indicate the error.
    \param stack INOUT a pointer to the kaapi_stack_t data structure.
    \retval EINVAL invalid argument: bad stack pointer.
    \retval EWOULDBLOCK the execution of the stack will block the control flow.
*/
extern int kaapi_stack_execall(kaapi_stack_t* stack);

/** \ingroup WS
    Try to steal work from tasks in the stack, else call splitter of the task. 
*/
extern int kaapi_sched_stealtask( kaapi_stack_t* stack, kaapi_task_t* task, kaapi_task_splitter_t splitter );


/* ========================================================================= */
/* API for adaptive algorithm                                                */
/* ========================================================================= */


/* only exported for kaapi_stealpoint. kprocessor is a opaque structure here
*/
extern int kaapi_sched_stealprocessor(struct kaapi_processor_t* kproc);

/** \ingroup ADAPTIVE
    Test if the current execution should process steal request into the task.
    This function also poll for other requests on the thread of control,
    it may invoke processing of streal request of previous pushed tasks.
    \retval !=0 if they are a steal request(s) to process onto the given task.
    \retval 0 else
*/
static inline int kaapi_stealpoint_isactive( kaapi_stack_t* stack, kaapi_task_t* task )
{
  int count = *stack->hasrequest;
  if (count) 
  {
    /* \TODO: ici appel systematique a kaapi_sched_stealprocessor, qui retourne vite, (mais cout)
       a voir comment bi-passer cet appel (rentre visible kproc->lsuspend ?)...
    */
    kaapi_sched_stealprocessor(stack->_proc);
    count = *stack->hasrequest;
    return count;
  }
  return 0;
}

/** \ingroup ADAPTIVE
    Test if the current execution should process preemt request into the task
    and then call the splitter function with given arguments.
    \retval !=0 if they are a steal request(s) to process onto the given task.
    \retval 0 else
*/
static inline int kaapi_stealpoint( kaapi_stack_t* stack, kaapi_task_t* task, kaapi_task_splitter_t splitter)
{
  if (kaapi_stealpoint_isactive(stack, task)) 
  {
    /* with function call has interest to be inline */
    kaapi_sched_stealtask( stack, task, splitter );
  }
  return *stack->hasrequest !=0;
}


extern void _kaapi_post_invoke_splitter( kaapi_stack_t* stack, int count );

/** \ingroup ADAPTIVE
    Adaptor to invoke the splitter in the user code without putting atomic function in the
    public interface.
*/
#define kaapi_invoke_splitter( stack, splittercall ) \
    { \
      int __reval_count = splittercall; \
      _kaapi_post_invoke_splitter( stack, __reval_count );\
    }
    

#if defined(KAAPI_USE_VARIADIC_MACRO)
/** \ingroup ADAPTIVE
    Test if the current execution should process preemt request into the task
    and then call the splitter function with given arguments.
    \retval !=0 if they are a steal request(s) to process onto the given task.
    \retval 0 else
    
    TODO: should put ATOMIC OP into public interface. Cut & Paste kaapi_sched_stealtask.c
*/
#define kaapi_stealpoint_macro( stack, task, splitter, ...)  ((stack)->hasrequest !=0)

#endif

/** Return true iff the request correctly posted
  \param pksr kaapi_request_t
*/
static inline int kaapi_request_ok( kaapi_request_t* kpsr )
{ return (kpsr->status == 1 /*KAAPI_REQUEST_S_POSTED*/); }

/** \ingroup ADAPTIVE
    Test if the current execution should process preemt request into the task
    and then pass arg_victim argument to the victim and return !=0 value
    \retval !=0 if it exists a prending preempt request(s) to process onto the given task.
    \retval 0 else

   TODO 
*/
extern int kaapi_preemptpoint_isactive( kaapi_stack_t* stack, kaapi_task_t* task, void* arg_victim );


/** \ingroup ADAPTIVE
    Test if the current execution should process preemt request into the task
    and then pass arg_victim argument to the victim and call the reducer function with extra arguments.
    \retval !=0 if it exists a prending preempt request(s) to process onto the given task.
    \retval 0 else

   TODO 
*/
extern int kaapi_preemptpoint( kaapi_stack_t* stack, kaapi_task_t* task, kaapi_task_reducer_t reducer, void* arg_victim, ...);

#if defined(KAAPI_USE_VARIADIC_MACRO)
/** \ingroup ADAPTIVE
    Test if the current execution should process preemt request into the task
    and then pass arg_victim argument to the victim and call the reducer function with extra arguments.
    \retval !=0 if it exists a prending preempt request(s) to process onto the given task.
    \retval 0 else

   TODO 
*/
#define kaapi_preemptpoint_macro( stack, task, reducer, arg_victim, ...) (0)
#endif


/** \ingroup ADAPTIVE
    Reply a value to a steal request. If retval is !=0 it means that the request
    has successfully adapt to steal work. Else 0.
    This function is machine dependent.
*/
extern int kaapi_request_reply( kaapi_stack_t* stack, kaapi_task_t* task, kaapi_request_t* request, kaapi_stack_t* thief_stack, int retval );

/** \ingroup ADAPTIVE
    Set an splitter to be called in concurrence with the execution of the next instruction
    if a steal request is sent to the task.
    \retval !=0 if they are a steal request(s) to process onto the given task.
    \retval 0 else
*/
static inline int kaapi_task_setaction(kaapi_task_t* task, kaapi_task_splitter_t splitter)
{
  task->splitter = splitter;
  return 0;
}

/** \ingroup ADAPTIVE
    Erase the previously splitter action and avoid concurrent steal on return.
    \retval 0 in case of success
    \retval !=0 in case of error code
*/
static inline int kaapi_task_getaction(kaapi_task_t* task)
{
  task->splitter = 0;
  return 0;
}

/** \ingroup ADAPTIVE
   Try to preempt next thief in the reverse order defined by steal reponse.
   Return true iff some work have been preempted and should be processed locally.
   If no more thief can been preempted, it means then the function return false (0).
   If it exists a thief, then the call to kaapi_preempt_nextthief() will return the
   value the call to reducer function.
   
   reducer function should has the following signature:
      int (*)( kaapi_task_t* task, void* thief_work, ... )
   where ... is the same arguments as passed to kaapi_preempt_nextthief.

   TODO 
*/
extern int kaapi_preempt_nextthief( kaapi_stack_t* stack, kaapi_task_t* task, void* arg_thief, kaapi_task_reducer_t reducer, ... );  


#if defined(KAAPI_USE_VARIADIC_MACRO)
/** \ingroup ADAPTIVE
   Try to preempt next thief in the reverse order defined by steal reponse.
   Return true iff some work have been preempted and should be processed locally.
   If no more thief can been preempted, it means then the function return false (0).
   If it exists a thief, then the call to kaapi_preempt_nextthief() will return the
   value the call to reducer function.
   
   reducer function should has the following signature:
      int (*)( kaapi_task_t* task, void* thief_work, ... )
   where ... is the same arguments as passed to kaapi_preempt_nextthief.

   TODO
*/
#define kaapi_preempt_nextthief_macro( stack, task, arg_thief, reducer, ... ) 0
#endif

/** \ingroup ADAPTIVE
    Wait the end of all the stealer of the adaptive task 
    TODO with preemption
*/
extern int kaapi_finalize_steal( kaapi_stack_t* stack, kaapi_task_t* task );



/* ========================================================================= */
/* Format declaration                                                        */
/* ========================================================================= */
/** \ingroup TASK
    Register the task's format data structure
*/
extern kaapi_format_id_t kaapi_format_taskregister( 
        kaapi_format_t*           (*fmt_fnc)(void),
        kaapi_task_body_t           body,
        const char*                 name,
        size_t                      size,
        int                         count,
        const kaapi_access_mode_t   mode_param[],
        const kaapi_offset_t        offset_param[],
        const kaapi_format_t*       fmt_params[]
);

extern kaapi_format_id_t kaapi_format_structregister( 
        kaapi_format_t*           (*fmt_fnc)(void),
        const char*                 name,
        size_t                      size,
        void                       (*cstor)( void* ),
        void                       (*dstor)( void* ),
        void                       (*cstorcopy)( void*, const void*),
        void                       (*copy)( void*, const void*),
        void                       (*assign)( void*, const void*)
);

/** \ingroup TASK
    Resolve a format data structure from the body of a task
*/
extern kaapi_format_t* kaapi_format_resolvebybody(kaapi_task_body_t key);

/** \ingroup TASK
    Resolve a format data structure from the format identifier
*/
extern kaapi_format_t* kaapi_format_resolvebyfmit(kaapi_format_id_t key);

#if defined(KAAPI_USE_VARIADIC_MACRO)
#define KAAPI_REGISTER_TASKFORMAT( formatobject, name, fnc_body, ... ) \
  static inline kaapi_format_t* formatobject(void) \
  {\
    static kaapi_format_t formatobject##_object;\
    return &formatobject##_object;\
  }\
  static inline void __attribute__ ((constructor)) __kaapi_register_format_##formatobject (void)\
  { \
    static int isinit = 0;\
    if (isinit) return;\
    isinit = 1;\
    kaapi_format_taskregister( &formatobject, (fnc_body), name, ##__VA_ARGS__);\
  }


#define KAAPI_REGISTER_STRUCTFORMAT( formatobject, name, size, cstor, dstor, cstorcopy, copy, assign ) \
  static inline kaapi_format_t* fnc_formatobject(void) \
  {\
    static kaapi_format_t formatobject##_object;\
    return &formatobject##_object;\
  }\
  static inline void __attribute__ ((constructor)) __kaapi_register_format_##formatobject (void)\
  { \
    static int isinit = 0;\
    if (isinit) return;\
    isinit = 1;\
    kaapi_format_structregister( &formatobject, name, size, cstor, dstor, cstorcopy, copy, assign );\
  }
#endif


/**
 */
extern void _kaapi_dummy(void*);

/* ========================================================================= */
/* Initialization / destruction functions
 */
extern void __attribute__ ((constructor)) kaapi_init(void);

extern void __attribute__ ((destructor)) kaapi_fini(void);

#if !defined(KAAPI_COMPILE_SOURCE)

/** To force reference to kaapi_init.c in order to link against kaapi_init and kaapi_fini
 */
static void __attribute__((unused)) __kaapi_dumy_dummy(void)
{
  _kaapi_dummy(NULL);
}
#endif


#ifdef __cplusplus
}
#endif

#endif /* _KAAPI_H */
