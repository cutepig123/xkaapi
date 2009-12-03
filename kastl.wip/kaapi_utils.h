/*
 *  transform.cpp
 *  xkaapi
 *
 *  Created by FLM
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */


#ifndef _XKAAPI_KAAPI_UTILS_H
#define _XKAAPI_KAAPI_UTILS_H



#include "kaapi.h"



// TODO: should be in kaapi.h
static inline void kaapi_stack_popdata(kaapi_stack_t* stack, kaapi_uint32_t count)
{
  stack->sp_data -= count;
}



namespace kaapi_utils
{
  // prototypes

  void fail_requests(kaapi_stack_t*, kaapi_task_t*, int, kaapi_request_t*);

  // static function definistions

  template<typename SelfType>
  static void static_thiefentrypoint( kaapi_task_t* task, kaapi_stack_t* stack )
  {
    SelfType* self_work = kaapi_task_getargst(task, SelfType);
    self_work->doit(task, stack);
  }

  template<typename SelfType>
  static int static_splitter( kaapi_stack_t* victim_stack, kaapi_task_t* task, int count, kaapi_request_t* request )
  {
    SelfType* const self_work = kaapi_task_getargst(task, SelfType);
    return self_work->splitter( victim_stack, task, count, request );
  }

  template<typename SelfType>
  static int foreach_request
  (
   kaapi_stack_t* victim_stack,
   kaapi_task_t* task,
   int count,
   kaapi_request_t* request,
   typename SelfType::request_handler_t& handler,
   SelfType* this_work
   )
  {
    // process the requests and fail
    // the one we did not reply to

    const int replied_count = count;
    SelfType* output_work = 0;

    for (; count > 0; ++request)
      {
	if (!kaapi_request_ok(request))
	  continue ;

	kaapi_stack_t* const thief_stack = request->stack;
	kaapi_task_t*  const thief_task  = kaapi_stack_toptask(thief_stack);
	void* const stack_data = kaapi_stack_pushdata(thief_stack, sizeof(SelfType));

	kaapi_task_init(thief_stack, thief_task, KAAPI_TASK_ADAPTIVE);
	kaapi_task_setbody(thief_task, &static_thiefentrypoint<SelfType>);
	kaapi_task_setargs(thief_task, stack_data);

	output_work = kaapi_task_getargst(thief_task, SelfType);

	// stop request processing
	if (handler(this_work, output_work) == false)
	  {
	    kaapi_stack_popdata(thief_stack, sizeof(SelfType));
	    break;
	  }

	kaapi_stack_pushtask(thief_stack);

	// reply ok (1) to the request
	kaapi_request_reply(victim_stack, task, request, thief_stack, 1);

	--count; 
      }

    fail_requests(victim_stack, task, count, request);

    return replied_count;
  }


  template<typename SelfType>
  static void start_adaptive_task(SelfType* work)
  {
    kaapi_stack_t* const stack = kaapi_self_stack();
    kaapi_task_t* const task = kaapi_stack_toptask(stack);

    kaapi_task_init(stack, task, KAAPI_TASK_ADAPTIVE);
    kaapi_task_setargs(task, work);
    kaapi_stack_pushtask(stack);

    work->doit(task, stack);

    kaapi_stack_poptask(stack);
  }

} // kaapi_utils namespace



#endif // ! _XKAAPI_KAAPI_UTILS_H
