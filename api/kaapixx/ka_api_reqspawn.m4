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
      
#if !defined(KAAPI_NDEBUG)
      M4_PARAM(`typedef typename TraitFormalParam$1_t::mode_t mode_formal$1_t;
      ', `', `')
      M4_PARAM(`typedef typename TraitFormalParam<E$1>::mode_t mode_effective$1_t;
      ', `', `')
      M4_PARAM(`WARNING_UNDEFINED_PASSING_RULE<mode_effective$1_t, mode_formal$1_t, E$1, F$1, FOR_ARG<$1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      ', `', `')
#endif

      TaskArg_t* arg = (TaskArg_t*)kaapi_request_pushdata( _req, sizeof(TaskArg_t));

      // todo -> grep a type in UAMTYpe with Effective type in parameter in place of actual inclosure
      M4_PARAM(`new (&arg->f$1) inclosure$1_t(e$1);
      ', `', `')

      kaapi_task_init( 
        kaapi_request_toptask(_req), 
        KaapiFormatTask_t::default_bodies.cpu_body,
        arg
      );
      KAAPI_REQPUSHER(KAAPI_NUMBER_PARAMS)<
        TraitSplitter<TASK>::has_splitter,
        TASK
        M4_PARAM(`TraitFormalParam$1_t', `,', `,')
      >::push( _req, _adaptivetask, _flag );
    }

    template<M4_PARAM(`class E$1', `', `,')>
    void operator()( M4_PARAM(`const E$1& e$1', `', `, ') )
    {
      KAAPI_NAME(PushArg,KAAPI_NUMBER_PARAMS)(
         &TASK::dummy_method_to_have_formal_param_type, M4_PARAM(`e$1', `', `, ') 
      );
    }
