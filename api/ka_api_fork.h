/* KAAPI public interface */
// KAAPI library source
// -----------------------------------------
// by Thierry Gautier
//(c) INRIA, projet MOAIS, 2006-2010
//
// **********************************************************
// WARNING! This file has been automatically generated by M4
// Jeu 25 fév 2010 16:42:38 CET
// *********************************************************














    /* 1 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1>
    kaapi_task_t* PushArg1( void (SIGNATURE::*)( Thread* , F1 ), E1 e1 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      
      typedef KaapiTaskArg1 <uamttype1_t> TaskArg_t;

      typedef FormatClosure1<TASK, TraitUAMParam<F1> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      
      return clo;
    }

    template<class E1>
    void operator()( E1 e1 )
    {
      kaapi_task_t* clo = PushArg1(
         &TASK::dummy_method_to_have_formal_param_type, e1 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 2 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2>
    kaapi_task_t* PushArg2( void (SIGNATURE::*)( Thread* , F1, F2 ), E1 e1,E2 e2 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      
      typedef KaapiTaskArg2 <uamttype1_t,uamttype2_t> TaskArg_t;

      typedef FormatClosure2<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      
      return clo;
    }

    template<class E1,class E2>
    void operator()( E1 e1, E2 e2 )
    {
      kaapi_task_t* clo = PushArg2(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 3 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3>
    kaapi_task_t* PushArg3( void (SIGNATURE::*)( Thread* , F1, F2, F3 ), E1 e1,E2 e2,E3 e3 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      
      typedef KaapiTaskArg3 <uamttype1_t,uamttype2_t,uamttype3_t> TaskArg_t;

      typedef FormatClosure3<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      
      return clo;
    }

    template<class E1,class E2,class E3>
    void operator()( E1 e1, E2 e2, E3 e3 )
    {
      kaapi_task_t* clo = PushArg3(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 4 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4>
    kaapi_task_t* PushArg4( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4 ), E1 e1,E2 e2,E3 e3,E4 e4 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      
      typedef KaapiTaskArg4 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t> TaskArg_t;

      typedef FormatClosure4<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4 )
    {
      kaapi_task_t* clo = PushArg4(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 5 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5>
    kaapi_task_t* PushArg5( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4, F5 ), E1 e1,E2 e2,E3 e3,E4 e4,E5 e5 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      typedef typename TraitUAMParam<F5>::uamttype_t uamttype5_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      typedef typename uamttype5_t::template UAMParam<TYPE_INTASK>::type_t inclosure5_t;
      
      typedef KaapiTaskArg5 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t,uamttype5_t> TaskArg_t;

      typedef FormatClosure5<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> ,TraitUAMParam<F5> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      typedef typename TraitUAMParam<F5>::mode_t mode_formal5_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      typedef typename TraitUAMParam<E5>::mode_t mode_effective5_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective5_t, mode_formal5_t, FOR_ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      new (&arg->f5) inclosure5_t(e5);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4, E5 e5 )
    {
      kaapi_task_t* clo = PushArg5(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4, e5 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 6 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6>
    kaapi_task_t* PushArg6( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4, F5, F6 ), E1 e1,E2 e2,E3 e3,E4 e4,E5 e5,E6 e6 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      typedef typename TraitUAMParam<F5>::uamttype_t uamttype5_t;
      typedef typename TraitUAMParam<F6>::uamttype_t uamttype6_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      typedef typename uamttype5_t::template UAMParam<TYPE_INTASK>::type_t inclosure5_t;
      typedef typename uamttype6_t::template UAMParam<TYPE_INTASK>::type_t inclosure6_t;
      
      typedef KaapiTaskArg6 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t,uamttype5_t,uamttype6_t> TaskArg_t;

      typedef FormatClosure6<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> ,TraitUAMParam<F5> ,TraitUAMParam<F6> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      typedef typename TraitUAMParam<F5>::mode_t mode_formal5_t;
      typedef typename TraitUAMParam<F6>::mode_t mode_formal6_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      typedef typename TraitUAMParam<E5>::mode_t mode_effective5_t;
      typedef typename TraitUAMParam<E6>::mode_t mode_effective6_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective5_t, mode_formal5_t, FOR_ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective6_t, mode_formal6_t, FOR_ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      new (&arg->f5) inclosure5_t(e5);
      new (&arg->f6) inclosure6_t(e6);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4, E5 e5, E6 e6 )
    {
      kaapi_task_t* clo = PushArg6(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4, e5, e6 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 7 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7>
    kaapi_task_t* PushArg7( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4, F5, F6, F7 ), E1 e1,E2 e2,E3 e3,E4 e4,E5 e5,E6 e6,E7 e7 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      typedef typename TraitUAMParam<F5>::uamttype_t uamttype5_t;
      typedef typename TraitUAMParam<F6>::uamttype_t uamttype6_t;
      typedef typename TraitUAMParam<F7>::uamttype_t uamttype7_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      typedef typename uamttype5_t::template UAMParam<TYPE_INTASK>::type_t inclosure5_t;
      typedef typename uamttype6_t::template UAMParam<TYPE_INTASK>::type_t inclosure6_t;
      typedef typename uamttype7_t::template UAMParam<TYPE_INTASK>::type_t inclosure7_t;
      
      typedef KaapiTaskArg7 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t,uamttype5_t,uamttype6_t,uamttype7_t> TaskArg_t;

      typedef FormatClosure7<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> ,TraitUAMParam<F5> ,TraitUAMParam<F6> ,TraitUAMParam<F7> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      typedef typename TraitUAMParam<F5>::mode_t mode_formal5_t;
      typedef typename TraitUAMParam<F6>::mode_t mode_formal6_t;
      typedef typename TraitUAMParam<F7>::mode_t mode_formal7_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      typedef typename TraitUAMParam<E5>::mode_t mode_effective5_t;
      typedef typename TraitUAMParam<E6>::mode_t mode_effective6_t;
      typedef typename TraitUAMParam<E7>::mode_t mode_effective7_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective5_t, mode_formal5_t, FOR_ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective6_t, mode_formal6_t, FOR_ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective7_t, mode_formal7_t, FOR_ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      new (&arg->f5) inclosure5_t(e5);
      new (&arg->f6) inclosure6_t(e6);
      new (&arg->f7) inclosure7_t(e7);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4, E5 e5, E6 e6, E7 e7 )
    {
      kaapi_task_t* clo = PushArg7(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4, e5, e6, e7 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 8 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7, class E8, class F8>
    kaapi_task_t* PushArg8( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4, F5, F6, F7, F8 ), E1 e1,E2 e2,E3 e3,E4 e4,E5 e5,E6 e6,E7 e7,E8 e8 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      typedef typename TraitUAMParam<F5>::uamttype_t uamttype5_t;
      typedef typename TraitUAMParam<F6>::uamttype_t uamttype6_t;
      typedef typename TraitUAMParam<F7>::uamttype_t uamttype7_t;
      typedef typename TraitUAMParam<F8>::uamttype_t uamttype8_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      typedef typename uamttype5_t::template UAMParam<TYPE_INTASK>::type_t inclosure5_t;
      typedef typename uamttype6_t::template UAMParam<TYPE_INTASK>::type_t inclosure6_t;
      typedef typename uamttype7_t::template UAMParam<TYPE_INTASK>::type_t inclosure7_t;
      typedef typename uamttype8_t::template UAMParam<TYPE_INTASK>::type_t inclosure8_t;
      
      typedef KaapiTaskArg8 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t,uamttype5_t,uamttype6_t,uamttype7_t,uamttype8_t> TaskArg_t;

      typedef FormatClosure8<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> ,TraitUAMParam<F5> ,TraitUAMParam<F6> ,TraitUAMParam<F7> ,TraitUAMParam<F8> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      typedef typename TraitUAMParam<F5>::mode_t mode_formal5_t;
      typedef typename TraitUAMParam<F6>::mode_t mode_formal6_t;
      typedef typename TraitUAMParam<F7>::mode_t mode_formal7_t;
      typedef typename TraitUAMParam<F8>::mode_t mode_formal8_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      typedef typename TraitUAMParam<E5>::mode_t mode_effective5_t;
      typedef typename TraitUAMParam<E6>::mode_t mode_effective6_t;
      typedef typename TraitUAMParam<E7>::mode_t mode_effective7_t;
      typedef typename TraitUAMParam<E8>::mode_t mode_effective8_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective5_t, mode_formal5_t, FOR_ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective6_t, mode_formal6_t, FOR_ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective7_t, mode_formal7_t, FOR_ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective8_t, mode_formal8_t, FOR_ARG<8>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      new (&arg->f5) inclosure5_t(e5);
      new (&arg->f6) inclosure6_t(e6);
      new (&arg->f7) inclosure7_t(e7);
      new (&arg->f8) inclosure8_t(e8);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7,class E8>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4, E5 e5, E6 e6, E7 e7, E8 e8 )
    {
      kaapi_task_t* clo = PushArg8(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4, e5, e6, e7, e8 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 9 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7, class E8, class F8, class E9, class F9>
    kaapi_task_t* PushArg9( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4, F5, F6, F7, F8, F9 ), E1 e1,E2 e2,E3 e3,E4 e4,E5 e5,E6 e6,E7 e7,E8 e8,E9 e9 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      typedef typename TraitUAMParam<F5>::uamttype_t uamttype5_t;
      typedef typename TraitUAMParam<F6>::uamttype_t uamttype6_t;
      typedef typename TraitUAMParam<F7>::uamttype_t uamttype7_t;
      typedef typename TraitUAMParam<F8>::uamttype_t uamttype8_t;
      typedef typename TraitUAMParam<F9>::uamttype_t uamttype9_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      typedef typename uamttype5_t::template UAMParam<TYPE_INTASK>::type_t inclosure5_t;
      typedef typename uamttype6_t::template UAMParam<TYPE_INTASK>::type_t inclosure6_t;
      typedef typename uamttype7_t::template UAMParam<TYPE_INTASK>::type_t inclosure7_t;
      typedef typename uamttype8_t::template UAMParam<TYPE_INTASK>::type_t inclosure8_t;
      typedef typename uamttype9_t::template UAMParam<TYPE_INTASK>::type_t inclosure9_t;
      
      typedef KaapiTaskArg9 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t,uamttype5_t,uamttype6_t,uamttype7_t,uamttype8_t,uamttype9_t> TaskArg_t;

      typedef FormatClosure9<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> ,TraitUAMParam<F5> ,TraitUAMParam<F6> ,TraitUAMParam<F7> ,TraitUAMParam<F8> ,TraitUAMParam<F9> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      typedef typename TraitUAMParam<F5>::mode_t mode_formal5_t;
      typedef typename TraitUAMParam<F6>::mode_t mode_formal6_t;
      typedef typename TraitUAMParam<F7>::mode_t mode_formal7_t;
      typedef typename TraitUAMParam<F8>::mode_t mode_formal8_t;
      typedef typename TraitUAMParam<F9>::mode_t mode_formal9_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      typedef typename TraitUAMParam<E5>::mode_t mode_effective5_t;
      typedef typename TraitUAMParam<E6>::mode_t mode_effective6_t;
      typedef typename TraitUAMParam<E7>::mode_t mode_effective7_t;
      typedef typename TraitUAMParam<E8>::mode_t mode_effective8_t;
      typedef typename TraitUAMParam<E9>::mode_t mode_effective9_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective5_t, mode_formal5_t, FOR_ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective6_t, mode_formal6_t, FOR_ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective7_t, mode_formal7_t, FOR_ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective8_t, mode_formal8_t, FOR_ARG<8>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective9_t, mode_formal9_t, FOR_ARG<9>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      new (&arg->f5) inclosure5_t(e5);
      new (&arg->f6) inclosure6_t(e6);
      new (&arg->f7) inclosure7_t(e7);
      new (&arg->f8) inclosure8_t(e8);
      new (&arg->f9) inclosure9_t(e9);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7,class E8,class E9>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4, E5 e5, E6 e6, E7 e7, E8 e8, E9 e9 )
    {
      kaapi_task_t* clo = PushArg9(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4, e5, e6, e7, e8, e9 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }














    /* 10 is the number of possible parameters */
    template<class SIGNATURE, class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7, class E8, class F8, class E9, class F9, class E10, class F10>
    kaapi_task_t* PushArg10( void (SIGNATURE::*)( Thread* , F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 ), E1 e1,E2 e2,E3 e3,E4 e4,E5 e5,E6 e6,E7 e7,E8 e8,E9 e9,E10 e10 )
    {
      typedef typename TraitUAMParam<F1>::uamttype_t uamttype1_t;
      typedef typename TraitUAMParam<F2>::uamttype_t uamttype2_t;
      typedef typename TraitUAMParam<F3>::uamttype_t uamttype3_t;
      typedef typename TraitUAMParam<F4>::uamttype_t uamttype4_t;
      typedef typename TraitUAMParam<F5>::uamttype_t uamttype5_t;
      typedef typename TraitUAMParam<F6>::uamttype_t uamttype6_t;
      typedef typename TraitUAMParam<F7>::uamttype_t uamttype7_t;
      typedef typename TraitUAMParam<F8>::uamttype_t uamttype8_t;
      typedef typename TraitUAMParam<F9>::uamttype_t uamttype9_t;
      typedef typename TraitUAMParam<F10>::uamttype_t uamttype10_t;
      
      typedef typename uamttype1_t::template UAMParam<TYPE_INTASK>::type_t inclosure1_t;
      typedef typename uamttype2_t::template UAMParam<TYPE_INTASK>::type_t inclosure2_t;
      typedef typename uamttype3_t::template UAMParam<TYPE_INTASK>::type_t inclosure3_t;
      typedef typename uamttype4_t::template UAMParam<TYPE_INTASK>::type_t inclosure4_t;
      typedef typename uamttype5_t::template UAMParam<TYPE_INTASK>::type_t inclosure5_t;
      typedef typename uamttype6_t::template UAMParam<TYPE_INTASK>::type_t inclosure6_t;
      typedef typename uamttype7_t::template UAMParam<TYPE_INTASK>::type_t inclosure7_t;
      typedef typename uamttype8_t::template UAMParam<TYPE_INTASK>::type_t inclosure8_t;
      typedef typename uamttype9_t::template UAMParam<TYPE_INTASK>::type_t inclosure9_t;
      typedef typename uamttype10_t::template UAMParam<TYPE_INTASK>::type_t inclosure10_t;
      
      typedef KaapiTaskArg10 <uamttype1_t,uamttype2_t,uamttype3_t,uamttype4_t,uamttype5_t,uamttype6_t,uamttype7_t,uamttype8_t,uamttype9_t,uamttype10_t> TaskArg_t;

      typedef FormatClosure10<TASK, TraitUAMParam<F1> ,TraitUAMParam<F2> ,TraitUAMParam<F3> ,TraitUAMParam<F4> ,TraitUAMParam<F5> ,TraitUAMParam<F6> ,TraitUAMParam<F7> ,TraitUAMParam<F8> ,TraitUAMParam<F9> ,TraitUAMParam<F10> > KaapiFormatTask_t;
      
#if defined(KAAPI_DEBUG)
      typedef typename TraitUAMParam<F1>::mode_t mode_formal1_t;
      typedef typename TraitUAMParam<F2>::mode_t mode_formal2_t;
      typedef typename TraitUAMParam<F3>::mode_t mode_formal3_t;
      typedef typename TraitUAMParam<F4>::mode_t mode_formal4_t;
      typedef typename TraitUAMParam<F5>::mode_t mode_formal5_t;
      typedef typename TraitUAMParam<F6>::mode_t mode_formal6_t;
      typedef typename TraitUAMParam<F7>::mode_t mode_formal7_t;
      typedef typename TraitUAMParam<F8>::mode_t mode_formal8_t;
      typedef typename TraitUAMParam<F9>::mode_t mode_formal9_t;
      typedef typename TraitUAMParam<F10>::mode_t mode_formal10_t;
      
      typedef typename TraitUAMParam<E1>::mode_t mode_effective1_t;
      typedef typename TraitUAMParam<E2>::mode_t mode_effective2_t;
      typedef typename TraitUAMParam<E3>::mode_t mode_effective3_t;
      typedef typename TraitUAMParam<E4>::mode_t mode_effective4_t;
      typedef typename TraitUAMParam<E5>::mode_t mode_effective5_t;
      typedef typename TraitUAMParam<E6>::mode_t mode_effective6_t;
      typedef typename TraitUAMParam<E7>::mode_t mode_effective7_t;
      typedef typename TraitUAMParam<E8>::mode_t mode_effective8_t;
      typedef typename TraitUAMParam<E9>::mode_t mode_effective9_t;
      typedef typename TraitUAMParam<E10>::mode_t mode_effective10_t;
      
      WARNING_UNDEFINED_PASSING_RULE<mode_effective1_t, mode_formal1_t, FOR_ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective2_t, mode_formal2_t, FOR_ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective3_t, mode_formal3_t, FOR_ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective4_t, mode_formal4_t, FOR_ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective5_t, mode_formal5_t, FOR_ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective6_t, mode_formal6_t, FOR_ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective7_t, mode_formal7_t, FOR_ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective8_t, mode_formal8_t, FOR_ARG<8>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective9_t, mode_formal9_t, FOR_ARG<9>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      WARNING_UNDEFINED_PASSING_RULE<mode_effective10_t, mode_formal10_t, FOR_ARG<10>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      
#endif
      kaapi_task_t* clo = kaapi_stack_toptask( _stack);
      kaapi_task_initdfg( _stack, clo, KaapiFormatTask_t::bodyid, kaapi_stack_pushdata(_stack, sizeof(TaskArg_t)) );
      TaskArg_t* arg = kaapi_task_getargst( clo, TaskArg_t);
      new (&arg->f1) inclosure1_t(e1);
      new (&arg->f2) inclosure2_t(e2);
      new (&arg->f3) inclosure3_t(e3);
      new (&arg->f4) inclosure4_t(e4);
      new (&arg->f5) inclosure5_t(e5);
      new (&arg->f6) inclosure6_t(e6);
      new (&arg->f7) inclosure7_t(e7);
      new (&arg->f8) inclosure8_t(e8);
      new (&arg->f9) inclosure9_t(e9);
      new (&arg->f10) inclosure10_t(e10);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7,class E8,class E9,class E10>
    void operator()( E1 e1, E2 e2, E3 e3, E4 e4, E5 e5, E6 e6, E7 e7, E8 e8, E9 e9, E10 e10 )
    {
      kaapi_task_t* clo = PushArg10(
         &TASK::dummy_method_to_have_formal_param_type, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10 
      );
      _attr(_stack, clo );
      kaapi_stack_pushtask( _stack);    
    }
