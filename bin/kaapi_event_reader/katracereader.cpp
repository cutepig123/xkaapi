/*
** xkaapi
** 
**
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

#define __STDC_FORMAT_MACROS 
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <queue>
#include <set>
#include "kaapi_impl.h"
#include "kaapi_trace_reader.h"
#include "poti.h"


/** see kaapi_event.h for coding of event name */
#if (__KAAPI_TRACE_VERSION__ == 1)
static const char* kaapi_event_name[] = {
/* 0 */  "K-ProcStart",
/* 1 */  "K-ProcEnd",
/* 2 */  "TaskBegin",
/* 3 */  "TaskEnd",
/* 4 */  "BeginFrame",
/* 5 */  "EndFrame",
/* 6 */  "StaticScheduleBeg",
/* 7 */  "StaticScheduleEnd",
/* 8 */  "StaticTaskBeg",
/* 9 */  "StaticTaskEnd",
/*10 */  "IdleBeg",
/*11 */  "IdleEnd",
/*12 */  "SuspendBeg",
/*13 */  "SuspendEnd",
/*14 */  "PostSuspend",
/*15 */  "PostWaitBeg",
/*16 */  "PostWaitEnd",
/*17 */  "RequestBeg",
/*18 */  "RequestEnd",
/*19 */  "StealOp",
/*20 */  "SendReply",
/*21 */  "RecvReply",
/*22 */  "*",
/*23 */  "*",
/*24 */  "*",
/*25 */  "ForEachBeg",
/*26 */  "ForEachEnd",
/*27 */  "ForEachSteal",
/*28 */  "CudaCPUHtoDBeg",
/*29 */  "CudaCPUHtoDEnd",
/*30 */  "CudaCPUDtoHBeg",
/*31 */  "CudaCPUDtoHEnd",
/*32 */  "CudaSyncBeg",
/*33 */  "CudaSyncEnd",
/*34 */  "CudaAllocBeg",
/*35 */  "CudaAllocEnd"
};
#else
#error "Undefined trace version"
#endif


/* Type of function to process data
*/
typedef void (*kaapi_fnc_event)( int, const char** );


/*
*/
static void print_usage()
{
  fprintf(stderr, "Merge and convert internal Kaapi trace format to human readeable formats\n");
  fprintf(stderr, "*** options: -a | -p. Only one of the options may be selected at a time.\n");
  fprintf(stderr, "  -a: display all data associated to each events\n");
//  fprintf(stderr, "  -m: display the mapping of tasks\n");
//  fprintf(stderr, "  -s: display stats about runtime of all the tasks\n");
  fprintf(stderr, "  -p: output Paje format for Gantt diagram, one row per core\n");
  fprintf(stderr, "      Output filename is gantt.paje\n");
  exit(1);
}


/*
*/
std::string Binary(uint64_t value)
{
	char digits[] = "0123456789";
	std::string output;
  do {
    output = digits[value % 2] + output;
    value /= 2;
  } while(value !=0);
	return output;
}


/*
*/
static void print_traceheader(const kaapi_eventfile_header* header)
{
  std::cout << "Kaapi version number :" << header->version << std::endl;
  std::cout << "Kaapi minor version  :" << header->minor_version << std::endl;
  std::cout << "Trace format version :" << header->trace_version << std::endl;
  std::cout << "Kaapi cpucount used  :" << header->cpucount << std::endl;
  std::cout << "Event mask           :" << Binary(header->event_mask) << std::endl;
  std::cout << "Kaapi package        :" << header->package << std::endl;
}

/* Print human readable version of each event */
void callback_print_event(
  char* container_name,
  const kaapi_event_t* event
)
{
  std::cout << event->date << ": " << event->kid << " -> " << kaapi_event_name[event->evtno] << " ";
  switch (event->evtno) {
    case KAAPI_EVT_KPROC_START:
      std::cout << event->kid << " type:" << event->d0.i;
    break;

    case KAAPI_EVT_KPROC_STOP:
      std::cout << event->kid;
    break;

    /* standard task exec */
    case KAAPI_EVT_TASK_BEG:
    break;

    case KAAPI_EVT_TASK_END:
    break;

    /* */
    case KAAPI_EVT_FRAME_TL_BEG:
    break;

    case KAAPI_EVT_FRAME_TL_END:
    break;

    /* unroll graph for static schedule */
    case KAAPI_EVT_STATIC_BEG:
    break;

    case KAAPI_EVT_STATIC_END:
    break;

    /* exec task in static graph partitioning */
    case KAAPI_EVT_STATIC_TASK_BEG:
    break;

    case KAAPI_EVT_STATIC_TASK_END:
    break;

    /* idle = steal state */
    case KAAPI_EVT_SCHED_IDLE_BEG:
    break;

    case KAAPI_EVT_SCHED_IDLE_END:
    break;

    /* suspend state: worker thread */
    case KAAPI_EVT_SCHED_SUSPEND_BEG:
    break;

    case KAAPI_EVT_SCHED_SUSPEND_END:
    break;

    /* suspend state: master thread */
    case KAAPI_EVT_SCHED_SUSPEND_POST:
    break;

    case KAAPI_EVT_SCHED_SUSPWAIT_BEG:
    break;

    case KAAPI_EVT_SCHED_SUSPWAIT_END:
    break;

    /* processing request */
    case KAAPI_EVT_REQUESTS_BEG:
      std::cout << " victimid:" << event->d0.i << ", serial:" << event->d1.i;
    break;

    case KAAPI_EVT_REQUESTS_END:
      std::cout << " victimid:" << event->d0.i << ", serial:" << event->d1.i;
    break;

    /* emit steal */
    case KAAPI_EVT_STEAL_OP:
      std::cout << " victimid:" << event->d0.i << ", serial:" << event->d1.i;
    break;

    /* emit reply */
    case KAAPI_EVT_SEND_REPLY:
      std::cout << " victimid:" << event->d0.i << ", thiefid:" << event->d1.i << ", serial:" << event->d2.i;
    break;

    /* recv reply */
    case KAAPI_EVT_RECV_REPLY:
      std::cout << " victimid:" << event->d0.i << ", serial:" << event->d1.i << ", status: " << event->d2.i;
    break;

    default:
      printf("***Unkown event number: %i\n", event->evtno);
      break;

    /* suspend state: master thread */
    case KAAPI_EVT_FOREACH_BEG:
    break;

    case KAAPI_EVT_FOREACH_END:
    break;

    case KAAPI_EVT_FOREACH_STEAL:
    break;
  }
  std::cout << std::endl;
}


/*
*/
static void fnc_print_evt( int count, const char** filenames )
{
  FileSet* fs;
  fs = OpenFiles( count, filenames );
  kaapi_eventfile_header header;
  if (GetHeader(fs, 0, &header) ==0)
  {
    print_traceheader(&header);
  }
  ReadFiles(fs, callback_print_event );
  CloseFiles(fs);
}


/*
*/
__attribute__((unused))
static void fnc_print_map_task_kid( int count, const char** filenames )  
{

#if 0
  kaapi_event_buffer_t evb;
  kaapi_event_t* e = evb.buffer;
  while (1)
  {
    ssize_t sz_read = read(fd, evb.buffer, KAAPI_EVENT_BUFFER_SIZE*sizeof(kaapi_event_t) );
    if (sz_read ==0) break;
    if (sz_read ==-1)
    {
      fprintf(stderr, "*** error while reading file '%s' : %s", filename, strerror(errno));
      break;
    }
    ssize_t sevents = sz_read/sizeof(kaapi_event_t);

    for (ssize_t i=0; i< sevents; ++i)
    {
      switch (e[i].evtno) {
        case KAAPI_EVT_TASK_BEG:
          std::cout << e[i].kid << " " << (void*)e[i].d0.p << std::endl;
          break;
        default:
          break;
      }
    }
  }
#endif
  
}


/*
*/
__attribute__((unused))
static void fnc_statistic_task_kid( int count, const char** filenames )
{
#if 0 /* simulator of states should be adapted to new events */
  double   t_min   = 0.0;
  double   t_max   = 0.0;
  double   t_total = 0.0;
  double   t_var2  = 0.0;
  uint64_t count   = 0.0;
  uint64_t t_idle  = 0;
  uint64_t t_suspend  = 0;
  uint64_t t_request  = 0;
  uint64_t cnt_steal = 0;

  kaapi_event_buffer_t evb;
  kaapi_event_t* e = evb.buffer;
    
  uint64_t d0 =0, d1 =0;
  uint64_t d0_sched =0, d1_sched =0;
  uint64_t d0_suspend =0, d1_suspend =0;
  uint64_t d0_req =0, d1_req =0;
  void* task =0;

  while (1)
  {
    ssize_t sz_read = read(fd, evb.buffer, KAAPI_EVENT_BUFFER_SIZE*sizeof(kaapi_event_t) );
    if (sz_read ==0) break;
    if (sz_read ==-1)
    {
      fprintf(stderr, "*** error while reading file '%s' : %s", filename, strerror(errno));
      break;
    }
    ssize_t sevents = sz_read/sizeof(kaapi_event_t);

    for (ssize_t i=0; i< sevents; ++i)
    {
      switch (e[i].evtno) {
        case KAAPI_EVT_TASK_BEG:
          d0   = e[i].date;
          task = e[i].d0.p;
          break;
        case KAAPI_EVT_TASK_END:
          if (0) //e[i].d0.p != task
          {
            printf("*** invalid events format, ignore time for task: %p\n", task );
          }
          else {
            d1 = e[i].date;
            uint64_t d = d1-d0;
            double v = 1.0e-9*(double)d; /* time in s */
            if (count ==0) 
            { 
              t_max    = v;
              t_min    = v;
              t_total  = v;
              t_var2   = v*v;
            } else {
              if (v > t_max) t_max = v;
              if (v < t_min) t_min = v;
              t_total += v;
              t_var2  += v*v;
            }
            ++count;
          }
          task = 0;
        case KAAPI_EVT_SCHED_IDLE_BEG:
          d0_sched = e[i].date;
        break;

        case KAAPI_EVT_SCHED_IDLE_END:
          d1_sched = e[i].date;
          t_idle   += (d1_sched - d0_sched);
        break;

        case KAAPI_EVT_SCHED_SUSPEND_BEG:
          d0_suspend = e[i].date;
        break;

        case KAAPI_EVT_SCHED_SUSPEND_END:
          d1_suspend = e[i].date;
          t_idle    -= (d1_suspend - d0_suspend);
          t_suspend += (d1_suspend - d0_suspend);
        break;

        case KAAPI_EVT_REQUESTS_BEG:
          d0_req   = e[i].date;
        break;

        case KAAPI_EVT_REQUESTS_END:
          d1_req   = e[i].date;
          t_request += (d1_req - d0_req);
        break;

        case KAAPI_EVT_STEAL_OP:
          ++cnt_steal;
        break;

        default:
          break;
      }
    }
  }

  
  /* print statistic */
  double average  = t_total / (double)count;
  double variance = t_var2/(double)count - average * average;
  double stddev = sqrt( variance );  
  size_t c = count;
  if (c >1) --c;
  double delta = 2.576 * sqrt( variance / c);
  
  printf("*** Statistics:\n");
  printf("Ttotal    (s)     : %e\n", t_total);
  printf("Tidle     (s)     : %e\n", 1.0e-9 * (double)t_idle);
  printf("Tsuspend  (s)     : %e\n", 1.0e-9 * (double)t_suspend);
  printf("Trequest  (s)     : %e\n", 1.0e-9 * (double)t_request);
  printf("Count     (#task) : %zu\n", count);
  printf("Count     (#steal): %zu\n", cnt_steal);
  printf("Tmin      (s)     : %e\n", t_min);
  printf("Tmax      (s)     : %e\n", t_max);
  printf("Taverage  (s)     : %e\n", average);
  printf("Tvariance (s^2)   : %e\n", variance);
  printf("Tstd_dev  (s)     : %e\n", stddev);
  printf("IC, 95%%   (s)     : %e\n", delta);  
#endif
}


/* set of successfull steal request, one entry per processor kid
*/
static std::vector<std::set<uint64_t> > gantt_steal_op_issuccess;

/* current event is stealop: search if reply status is success or not 
   Each processor that emits steal request has the following order of event:
   - KAAPI_EVT_STEAL_OP
   - KAAPI_EVT_RECV_REPLY
   And the combinator processor that has processed the request generate:
   - KAAPI_EVT_SEND_REPLY
*/
static void paje_mark_steal_status( const kaapi_event_t* event )
{
  uint64_t serial;

  if ( event->evtno != KAAPI_EVT_STEAL_OP)
  {
    fprintf(stderr,"[internal error]: Bad call to paje_mark_steal_status\n");
    exit(1);
  }
  serial = event->d1.i;

  while (event->evtno != KAAPI_EVT_KPROC_STOP)
  {
    if (event->evtno == KAAPI_EVT_RECV_REPLY)
    {
      if (event->d1.i != serial)
      {
        fprintf(stderr,"Trace is corrupted, RECV_REPLY event does not match STEAL_OP event\n");
        exit(1);
      }
      int status = (event->d2.i !=0);
      if (gantt_steal_op_issuccess.size() <= event->kid)
        gantt_steal_op_issuccess.resize(1+event->kid);
      if (status)
        gantt_steal_op_issuccess[event->kid].insert( serial );
      return;
    }
    ++event;
  }
  fprintf(stderr,
    "Trace is corrupted, STEAL_OP is does not matched a RECV_REPLY event\n"
  );
  exit(1);
}


/*
 * Output PAJE format for one event
 * name is the input name of the container : if  (case of the START event),
 * then initialize it.
 */
static int fnc_paje_gantt_header();
static int fnc_paje_gantt_close(uint64_t tmax);

static void callback_display_paje_event(
  char* name,
  const kaapi_event_t* event
)
{
  char tmp[128];
  char key[128];
  int kid;
  int serial __attribute__((unused)); 
  double d0 =0.0, d1 = 0.0;

  switch (event->evtno) 
  {
    case KAAPI_EVT_KPROC_START:
      d0   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"thread-%i",kid);
      pajeCreateContainer (d0, tmp, "THREAD", "root", tmp);
      sprintf(name,"steal-%i",kid);
      pajeCreateContainer (d0, name, "STEAL", tmp, name);
#if defined(KAAPI_USE_CUDA)
      if( event->type == KAAPI_PROC_TYPE_CUDA ){
	  sprintf(name,"htod-%i",kid);
	  pajeCreateContainer (d0, name, "GPU", tmp, name);
	  sprintf(name,"kernel-%i",kid);
	  pajeCreateContainer (d0, name, "GPU", tmp, name);
	  sprintf(name,"dtoh-%i",kid);
	  pajeCreateContainer (d0, name, "GPU", tmp, name);
      } 
#endif
      sprintf(name,"work-%i",kid);
      pajeCreateContainer (d0, name, "WORKER", tmp, name);
      break;

    case KAAPI_EVT_KPROC_STOP:
      d0   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(name,"steal-%i",kid);
      pajeDestroyContainer (d0, "THREAD", name);
#if defined(KAAPI_USE_CUDA)
      if( event->type == KAAPI_PROC_TYPE_CUDA ){
	  sprintf(name,"dtoh-%i",kid);
	  pajeDestroyContainer (d0, "GPU", name);
	  sprintf(name,"kernel-%i",kid);
	  pajeDestroyContainer (d0, "GPU", name);
	  sprintf(name,"htod-%i",kid);
	  pajeDestroyContainer (d0, "GPU", name);
      }
#endif
      sprintf(name,"work-%i",kid);
      pajeDestroyContainer (d0, "WORKER", name);
      sprintf(name,"thread-%i",kid);
      pajeDestroyContainer (d0, "THREAD", name);
      break;

    /* standard task exec */
    case KAAPI_EVT_TASK_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "a");
      break;

    case KAAPI_EVT_TASK_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE");
    break;

    /* */
    case KAAPI_EVT_FRAME_TL_BEG:
      d0   = 1e-9*(double)event->date;
    break;

    case KAAPI_EVT_FRAME_TL_END:
      d1   = 1e-9*(double)event->date;
    break;

    /* unroll graph for static schedule */
    case KAAPI_EVT_STATIC_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "st");          
    break;

    case KAAPI_EVT_STATIC_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE");
    break;

    /* exec task in static graph partitioning */
    case KAAPI_EVT_STATIC_TASK_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "b");
    break;

    case KAAPI_EVT_STATIC_TASK_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE");
    break;

    /* idle = steal state */
    case KAAPI_EVT_SCHED_IDLE_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "i");          
    break;

    case KAAPI_EVT_SCHED_IDLE_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE");          
    break;

    /* suspend state */
    case KAAPI_EVT_SCHED_SUSPEND_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "s");          
    break;

    case KAAPI_EVT_SCHED_SUSPEND_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE");          
    break;

    case KAAPI_EVT_SCHED_SUSPEND_POST:
      d0   = 1e-9*(double)event->date;
      pajeNewEvent(d0, name, "SUSPEND", "su");
    break;

    case KAAPI_EVT_SCHED_SUSPWAIT_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "s"); 
    break;

    case KAAPI_EVT_SCHED_SUSPWAIT_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE"); 
    break;


    case KAAPI_EVT_FOREACH_STEAL:
      d0   = 1e-9*(double)event->date;
      pajeNewEvent(d0, name, "STEAL", "fo");
    break;

    case KAAPI_EVT_FOREACH_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "f");
    break;

    case KAAPI_EVT_FOREACH_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE"); 
    break;

    /* processing request */
    case KAAPI_EVT_REQUESTS_BEG:
      d0  = 1e-9*(double)event->date;
      kid = event->d0.i;
      serial = event->d1.i;
      sprintf(tmp,"steal-%i",kid);
#if 0 /* do no pollute gantt diagram */
      sprintf(key,"b%i-%i-%i",event->kid, kid, serial);
      pajeStartLink(d0, "root", "LINK", name, "li", key);
      pajeEndLink(d0, "root", "LINK", tmp, "li", key);
#endif
      pajePushState (d0, tmp, "STATE", "r"); 
    break;

    case KAAPI_EVT_REQUESTS_END:
      d1  = 1e-9*(double)event->date;
      kid = event->d0.i;
      serial = event->d1.i;
      sprintf(tmp,"steal-%i",kid);
      pajePopState (d1, tmp, "STATE");
#if 0
      sprintf(key,"e%i-%i-%i",event->kid, kid, serial);
      pajeStartLink(d1, "root", "LINK", tmp, "li", key);
      pajeEndLink(d1, "root", "LINK", name, "li", key);
#endif
    break;

    /* emit steal */
    case KAAPI_EVT_STEAL_OP:
      /* find in next event (safe in callback) if steal success */
      paje_mark_steal_status( event );

      d0  = 1e-9*(double)event->date;
      kid = event->d0.i; /* victim id */
#if 0
      pajeNewEvent(d0, name, "STEAL", "so");
#endif
      if (kid != event->kid)
      {
        sprintf(key,"s-%" PRIuPTR,event->d1.i*100000+event->kid);
        sprintf(tmp,"steal-%i",kid);

        if (   gantt_steal_op_issuccess[event->kid].find(event->d1.i) 
             != gantt_steal_op_issuccess[event->kid].end()
           )
        {
          pajeStartLink(d0, "root", "LINK", name, "riok", key);
          pajeEndLink(d0, "root", "LINK", tmp, "riok", key);
        }
#if 0 /* do no pollute gantt diagram */
        else {
          pajeStartLink(d0, "root", "LINK", name, "li", key);
          pajeEndLink(d0, "root", "LINK", tmp, "li", key);
        }
#endif
      }
    break;

    /* emit reply */
    case KAAPI_EVT_SEND_REPLY:
      d0  = 1e-9*(double)event->date;
      kid = event->d0.i; /* victimid */
      if (kid != (uint16_t)event->d1.i) /* thief id */
      {
        /* key using (serial, thiefid) */
        sprintf(tmp,"steal-%i",kid);
        sprintf(key,"r-%" PRIuPTR,event->d2.i*100000+event->d1.i);

        if (   gantt_steal_op_issuccess[event->d1.i].find(event->d2.i) 
             != gantt_steal_op_issuccess[event->d1.i].end()
           )
          pajeStartLink(d0, "root", "LINK", tmp, "riok", key);
#if 0 /* do no pollute gantt diagram */
        else
          pajeStartLink(d0, "root", "LINK", tmp, "ri", key);
#endif
      }
    break;

    /* recv reply */
    case KAAPI_EVT_RECV_REPLY:
      d0  = 1e-9*(double)event->date;
      kid = event->d0.i; /* kid of the victim */
      if (kid != event->kid)
      {
        /* key using (serial, thiefid) */
        sprintf(key,"r-%"PRIuPTR,event->d1.i*100000+event->kid);
        if (   gantt_steal_op_issuccess[event->kid].find(event->d1.i) 
             != gantt_steal_op_issuccess[event->kid].end()
           )
          pajeEndLink(d0, "root", "LINK", name, "riok", key);
#if 0 /* do no pollute gantt diagram */
        else
          pajeEndLink(d0, "root", "LINK", name, "ri", key);
#endif        
        /* do not reset entry, because event may have same date and recv may be processed before send... 
        gantt_steal_op_issuccess[event->kid].first = (uint64_t)-1;
        */
      }
    break;

#if defined(KAAPI_USE_CUDA)
    case KAAPI_EVT_CUDA_GPU_HTOD_BEG:
      d0   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"htod-%i",kid);
      pajePushState (d0, tmp, "STATE", "htod");
    break;

    case KAAPI_EVT_CUDA_GPU_HTOD_END:
      d1   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"htod-%i",kid);
      pajePopState (d1, tmp, "STATE");
    break;

    case KAAPI_EVT_CUDA_GPU_KERNEL_BEG:
      d0   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"kernel-%i",kid);
      pajePushState (d0, tmp, "STATE", "kernel");
    break;

    case KAAPI_EVT_CUDA_GPU_KERNEL_END:
      d1   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"kernel-%i",kid);
      pajePopState (d1, tmp, "STATE");
    break;

    case KAAPI_EVT_CUDA_GPU_DTOH_BEG:
      d0   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"dtoh-%i",kid);
      pajePushState (d0, tmp, "STATE", "dtoh");
    break;

    case KAAPI_EVT_CUDA_GPU_DTOH_END:
      d1   = 1e-9*(double)event->date;
      kid = event->kid;
      sprintf(tmp,"dtoh-%i",kid);
      pajePopState (d1, tmp, "STATE");
    break;

    case KAAPI_EVT_CUDA_CPU_SYNC_BEG:
      d0   = 1e-9*(double)event->date;
      pajePushState (d0, name, "STATE", "sync");          
    break;

    case KAAPI_EVT_CUDA_CPU_SYNC_END:
      d1   = 1e-9*(double)event->date;
      pajePopState (d1, name, "STATE");
    break;
#endif

    default:
      printf("***Unkown event number: %i\n", event->evtno);
      break;
  }
}

/*
*/
static void fnc_paje_gantt( int count, const char** filenames )
{
  int err;
  FileSet* fs;
  kaapi_eventfile_header header;
  uint64_t tmin, tmax;

  gantt_steal_op_issuccess.resize( 256 );

  fs = OpenFiles( count, filenames );
  if (fs ==0) 
    return;
  if (GetHeader(fs, 0, &header) !=0)
    return;
  if (GetInterval(fs, &tmin, &tmax) !=0)
    return;
    
  /* output paje header */
  err = fnc_paje_gantt_header();
  if (err !=0) 
  {
    fprintf(stderr, "*** cannot open file: 'gantt.paje'\n");
    exit(1);
  }

  /* generate all states/container etc... */
  ReadFiles(fs, callback_display_paje_event );
  
  /* close & umap */
  CloseFiles(fs);
  
  /* close output file */
  err = fnc_paje_gantt_close(tmax);
  if (err !=0) 
  {
    fprintf(stderr, "*** cannot close file: 'gantt.paje'\n");
    exit(1);
  }
}


static int fnc_paje_gantt_header()
{
  pajeOpen("gantt.paje");
  pajeHeader();

  pajeDefineContainerType ("ROOT", "0", "ROOT");  

  pajeDefineContainerType("THREAD", "ROOT", "THREAD");
  pajeDefineContainerType("WORKER", "THREAD", "WORKER");
  pajeDefineContainerType("STEAL", "THREAD", "STEAL");
#if defined(KAAPI_USE_CUDA)
  pajeDefineContainerType("GPU", "THREAD", "GPU");
#endif

  pajeDefineStateType("STATE",   "THREAD", "STATE");

  pajeDefineEventType("STEAL",   "THREAD", "STEAL", "blue");
  pajeDefineEventType("SUSPEND", "THREAD", "SUSPEND", "blue");
  pajeDefineEventType("FSTEAL",  "THREAD", "FSTEAL", "blue");

  pajeDefineLinkType("LINK", "ROOT", "THREAD", "THREAD", "LINK");

  /* actif state */
  pajeDefineEntityValue("a", "STATE", "running", "0.0 0.0 1.0");

  /* actif state when executing static task */
  pajeDefineEntityValue("b", "STATE", "running", "0.6 0.0 1.0");

  /* idle (stealing) state */
  pajeDefineEntityValue("i", "STATE", "stealing", "1 0.5 0.0");

  /* suspended state */
  pajeDefineEntityValue("s", "STATE", "suspended", "1.0 0.0 0.0");

  /* execution of steal request */
  pajeDefineEntityValue("r", "STATE", "request", "8.0 0.6 0.4");

  /* execution of foreach code */
  pajeDefineEntityValue("f", "STATE", "foreach", "0.0 0.2 1.0");

  /* execution of static task to unroll graph */
  pajeDefineEntityValue("st", "STATE", "unroll", "0.5 1.0 0.0");

  /* steal operation */
  pajeDefineEntityValue("so", "STEAL", "steal", "1.0 0.1 0.1");

  /* successful steal operation */
  pajeDefineEntityValue("sok", "STEAL", "steal", "0.0 1.0 0.1");

  /* suspend post operation */
  pajeDefineEntityValue("su", "SUSPEND", "suspend", "0.8 0.8 0.8");

  /* foreach steal event */
  pajeDefineEntityValue("fo", "STEAL", "foreachsteal", "0.5 0.25 0.0");

  /* link  */
  pajeDefineEntityValue("li", "LINK", "link", "1.0 0.1 0.1");

  /* link  */
  pajeDefineEntityValue("ri", "LINK", "reply", "0.2 0.2 0.2");

  /* link: successfull steal  */
  pajeDefineEntityValue("riok", "LINK", "steal", "0.0 1.0 0.2");

#if defined(KAAPI_USE_CUDA)
  /* CUDA host-to-device copy operation */
  pajeDefineEntityValue("htod", "STATE", "htod", "0.6 0.0 1.0");

  /* CUDA kernel execution time */
  pajeDefineEntityValue("kernel", "STATE", "kernel", "0.6 0.0 1.0");

  /* CUDA device-to-host copy operation */
  pajeDefineEntityValue("dtoh", "STATE", "dtoh", "0.6 0.0 1.0");

//  /* CUDA synchronizations by CPU */
  pajeDefineEntityValue("sync", "STATE", "sync", "0.0 0.6 0.6");
#endif

#if 0

  /* CUDA device memory allocations */
  pajeDefineEntityValue("al", "STATE", "Alloc", "0.0 0.0 0.0");
#endif

  /* create the root container */
  pajeCreateContainer (0.00, "root", "ROOT", "0", "root");

  return 0;
}


static int fnc_paje_gantt_close(uint64_t gantt_tmax)
{
  pajeDestroyContainer (1e-9*gantt_tmax, "ROOT", "root");
  pajeClose();
  fprintf(stdout, "*** Paje file 'gantt.paje' closed\n");
  return 0;
}


/* Parse options:
   * nooption : nocode: print usage
   * -a       : 0 : display all data, using function fnc_print_evt
   * -m       : 1 : display the mapping of tasks only, use function fnc_print_map_task_kid
   * -s       : 2 : display statistics
   * -p       : 2 : display paje gantt
*/
static kaapi_fnc_event parse_option( const char* arg)
{
  char option =' ';
  
  if (strcmp(arg, "-a") ==0)
    option = 'a';
#if 0
  if (strcmp(arg, "-m") ==0)
    option = 'm';
  if (strcmp(arg, "-s") ==0)
    option = 's';
#endif
  if (strcmp(arg, "-p") ==0)
    option = 'p';
  
  switch (option) {
  case 'a':
    return fnc_print_evt;

#if 0
  case 'm':
    return fnc_print_map_task_kid;

  case 's':
    return fnc_statistic_task_kid;
#endif

  case 'p':
    return fnc_paje_gantt;

  default:
    print_usage();
  }
  return 0;
}


/* main entry point : Kaapi initialization
*/
int main(int argc, char** argv)
{
  if (argc <2)
    print_usage();
  
  kaapi_fnc_event function = parse_option( argv[1] );

  if (function ==0) 
    return -1;
  if (argc-2 <= 0)
    print_usage();

  function( argc-2, (const char**)(argv+2) );
  
  return 0;
}