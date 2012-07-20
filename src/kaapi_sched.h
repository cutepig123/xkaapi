
#ifndef _KAAPI_SCHED_H_
#define _KAAPI_SCHED_H_

/** Used to steal a tasklist in the frame
*/
extern void kaapi_sched_stealtasklist( 
                           kaapi_thread_context_t*       thread, 
                           kaapi_tasklist_t*             tasklist, 
                           kaapi_listrequest_t*          lrequests, 
                           kaapi_listrequest_iterator_t* lrrange
);

/** Used to steal a tasklist from local readylist of victim
*/
extern void kaapi_sched_stealreadytasklist( 
                           kaapi_thread_context_t*       thread, 
                           kaapi_readytasklist_t*        rtl, 
                           kaapi_listrequest_t*          lrequests, 
                           kaapi_listrequest_iterator_t* lrrange
);


/** How to execute task with readylist
    It is assumed that top frame is a frame with a ready list.
*/
extern int kaapi_thread_execframe_tasklist( struct kaapi_thread_context_t* thread );

#endif /* _KAAPI_SCHED_H_ */
