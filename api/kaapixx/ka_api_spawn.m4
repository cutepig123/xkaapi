
    // KAAPI_NUMBER_PARAMS is the number of possible parameters
    template<class SIGNATURE, M4_PARAM(`class E$1, class F$1', `', `, ')>
    void KAAPI_NAME(PushArg,KAAPI_NUMBER_PARAMS)( void (SIGNATURE::*)( Thread* M4_PARAM(`, F$1', `', `') ), M4_PARAM(`const E$1& e$1', `', `,') )
    {
      M4_PARAM(`typedef TraitFormalParam<F$1> TraitFormalParam$1_t;
      ', `', `')
      M4_PARAM(`typedef typename TraitFormalParam$1_t::type_inclosure_t inclosure$1_t;
      ', `', `')
      typedef KAAPI_TASKARG(KAAPI_NUMBER_PARAMS) ifelse(KAAPI_NUMBER_PARAMS,0,`',`<M4_PARAM(`TraitFormalParam$1_t', `', `,')>') TaskArg_t;

      typedef KAAPI_FORMATCLOSURE(KAAPI_NUMBER_PARAMS)<TASK, M4_PARAM(`TraitFormalParam$1_t ', `', `,')> KaapiFormatTask_t;
      
/* Comment this line that disable verification when compiled with --enable_mode=release 
#if !defined(KAAPI_NDEBUG)
*/
      M4_PARAM(`typedef typename TraitFormalParam$1_t::mode_t mode_formal$1_t;
      ', `', `')
      M4_PARAM(`typedef typename TraitFormalParam<E$1>::mode_t mode_effective$1_t;
      ', `', `')
      M4_PARAM(`WARNING_UNDEFINED_PASSING_RULE<mode_effective$1_t, mode_formal$1_t, E$1, F$1, FOR_ARG<$1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      ', `', `')
/* See above
#endif
*/

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      if (KaapiFormatTask_t::default_bodies.cpu_body != 0)
	kaapi_task_init( clo, KaapiFormatTask_t::default_bodies.cpu_body, kaapi_thread_pushdata(_thread, sizeof(TaskArg_t)) );
      else
	kaapi_task_init( clo, KaapiFormatTask_t::default_bodies.gpu_body, kaapi_thread_pushdata(_thread, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);

      // here we do not detect a compile time the error without compilation with -DKAAPI_DEBUG 
      // todo -> grep a type in UAMTYpe with Effective type in parameter in place of actual inclosure
      M4_PARAM(`ConvertEffective2InClosure<E$1,F$1,inclosure$1_t>::doit(&arg->f$1, e$1);
      ', `', `')
      
      KAAPI_PUSHER(KAAPI_NUMBER_PARAMS)<
        TraitSplitter<TASK>::has_splitter,
        ATTR,
        TASK
        M4_PARAM(`TraitFormalParam$1_t', `,', `,')
      >::push( _attr, _thread );
    }
    

    template<M4_PARAM(`class E$1', `', `,')>
    void operator()( M4_PARAM(`const E$1& e$1', `', `, ') )
    {
      KAAPI_NAME(PushArg,KAAPI_NUMBER_PARAMS)(
         &TASK::dummy_method_to_have_formal_param_type, M4_PARAM(`e$1', `', `, ') 
      );
    }
