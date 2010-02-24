/* KAAPI public interface */
/*
** athapascan-2.h
** xkaapi
** 
** Created on Tue Mar 31 15:19:14 2009
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
#ifndef _KAAPI_CPP_H_
#define _KAAPI_CPP_H_

#include "kaapi.h"
#include "ka_error.h"
#include "ka_timer.h"
#include <vector>
#include <typeinfo>

namespace ka{}

namespace ka {

  /* take a constant... should be adjusted */
  enum { STACK_ALLOC_THRESHOLD = KAAPI_MAX_DATA_ALIGNMENT };  

  // --------------------------------------------------------------------
  typedef kaapi_uint8_t  ka_uint8_t;
  typedef kaapi_uint16_t ka_uint16_t;
  typedef kaapi_uint32_t ka_uint32_t;
  typedef kaapi_uint64_t ka_uint64_t;

  typedef kaapi_int8_t   ka_int8_t;
  typedef kaapi_int16_t  ka_int16_t;
  typedef kaapi_int32_t  ka_int32_t;
  typedef kaapi_int64_t  ka_int64_t;


  // --------------------------------------------------------------------
  namespace FormatDef {
#  define KAAPI_DECL_EXT_FORMAT(TYPE,OBJ)\
    extern const kaapi_format_t OBJ;
    
    extern const kaapi_format_t Null;
    KAAPI_DECL_EXT_FORMAT(bool, Bool)
    KAAPI_DECL_EXT_FORMAT(char, Char)
    extern const kaapi_format_t Byte;
    KAAPI_DECL_EXT_FORMAT(signed char, SChar)
    KAAPI_DECL_EXT_FORMAT(unsigned char, UChar)
    KAAPI_DECL_EXT_FORMAT(int, Int)
    KAAPI_DECL_EXT_FORMAT(unsigned int, UInt)
    KAAPI_DECL_EXT_FORMAT(short, Short)
    KAAPI_DECL_EXT_FORMAT(unsigned short, UShort)
    KAAPI_DECL_EXT_FORMAT(long, Long)
    KAAPI_DECL_EXT_FORMAT(unsigned long, ULong)
    KAAPI_DECL_EXT_FORMAT(long long, LLong)
    KAAPI_DECL_EXT_FORMAT(unsigned long long, ULLong)
    KAAPI_DECL_EXT_FORMAT(float, Float)
    KAAPI_DECL_EXT_FORMAT(double, Double)
    KAAPI_DECL_EXT_FORMAT(long double, LDouble)
  }
  
  /** link C++ format -> kaapi format */
  class Format : public kaapi_format_t {
  public:
    Format( 
        const std::string& name,
        size_t             size,
        void             (*cstor)( void* dest),
        void             (*dstor)( void* dest),
        void             (*cstorcopy)( void* dest, const void* src),
        void             (*copy)( void* dest, const void* src),
        void             (*assign)( void* dest, const void* src),
        void             (*print)( FILE* file, const void* src)
    );
  };

  /** format for update function */
  class FormatUpdateFnc : public Format {
  public:
    FormatUpdateFnc( 
      const std::string& name,
      int (*update_mb)(void* data, const struct kaapi_format_t* fmtdata,
                       const void* value, const struct kaapi_format_t* fmtvalue )
    );
  };
  
  /* for next networking part */
  class IStream;
  class OStream;
  class ODotStream;
  class SyncGuard;
  
  // --------------------------------------------------------------------
  class Community {
  protected:
    friend class System;
    Community( void* com )
    { }

  public:
    Community( const Community& com );

    /* */
    void commit();

    /* */
    void leave();

    /* */
    bool is_leader() const;
  };

  // --------------------------------------------------------------------
  class Thread;
  
  // --------------------------------------------------------------------
  class System {
  public:
    static Community join_community( int& argc, char**& argv )
      throw (RuntimeError,RestartException,ServerException);

    static Community initialize_community( int& argc, char**& argv )
      throw (RuntimeError,RestartException,ServerException);

    static Thread* get_current_thread();
    static int getRank();
    static void terminate();

  public:
  };

  // --------------------------------------------------------------------
  inline Thread* System::get_current_thread()
  {
    return (Thread*)kaapi_self_stack();
  }

  // --------------------------------------------------------------------
  template<class T>
  T* Alloca(size_t size)
  {
     void* data = kaapi_stack_pushdata( kaapi_self_stack(), sizeof(T)*size );
     return new (data) T[size];
  }

  // --------------------------------------------------------------------
  struct SetStack {};
  extern SetStack SetInStack;

  // --------------------------------------------------------------------
  struct SetHeap {};
  extern SetHeap SetInHeap;

  // --------------------------------------------------------------------
  class SetStickyC{};
  extern SetStickyC SetSticky;

  template<class T>
  class pointer_rpwp;
  template<class T>
  class pointer_rw;
  template<class T>
  class pointer_rp;
  template<class T>
  class pointer_r;
  template<class T>
  class pointer_wp;
  template<class T>
  class pointer_w;
  template<class T>
  class pointer_cwp;
  template<class T>
  class pointer_cw;

  template<class T>
  struct base_pointer {
    base_pointer() 
#if defined(KAAPI_DEBUG)
     : _ptr(0)
#endif
    {
    }
    base_pointer( T* p ) : _ptr(p)
    {
    }
    T* ptr() const { return _ptr; }
  protected:
    mutable T* _ptr;
  };

  template<class T>
  class value_ref {
  public:
    value_ref(T* p) : ptr(p){}
    void operator=( const T& value ) { *ptr = value; }
  protected:
    T* ptr;
  };
  
  
  /* typenames for access mode */
  struct ACCESS_MODE_V {};
  struct ACCESS_MODE_R {};
  struct ACCESS_MODE_W {};
  struct ACCESS_MODE_RW {};
  struct ACCESS_MODE_CW {};
  struct ACCESS_MODE_RP {};
  struct ACCESS_MODE_WP {};
  struct ACCESS_MODE_RPWP {};
  struct ACCESS_MODE_CWP {};

  // --------------------------------------------------------------------
  /* Information notes.
     - Shared types are defined to be used in signature definition of
     tasks. They should not be used to declare variables or used as effective
     parameters during a fork.
     - Effective parameters may be pointer (T* or const T*) but no verification
     of the parameter passing rules between effective parameters and formal parameters 
     could be done.
     - In order to profit of the capability to detect at compilation type correctness
     with respect to the parameter passing rules, one must used type pointer types.
     They are closed to the Shared types of the previous API but may be used like
     normal pointer and deferencing of pointers.
  */
  // --------------------------------------------------------------------
  template<class T>
  class pointer_rpwp : public base_pointer<T> {
  public:
    typedef T value_type;
    pointer_rpwp() : base_pointer<T>() {}
    pointer_rpwp( value_type* ptr ) : base_pointer<T>(ptr) {}
    explicit pointer_rpwp( kaapi_access_t& ptr ) : base_pointer<T>(kaapi_data(value_type, &ptr)) {}
  };

  template<class T>
  class pointer : public base_pointer<T> {
  public:
    typedef T value_type;
    pointer() : base_pointer<T>() {}
    pointer( value_type* ptr ) : base_pointer<T>(ptr) {}
  };

  template<class T>
  class RPWP {/* instanciante by default body if signature is not redefined */
  public:
    RPWP( const pointer_rpwp<T>& p ) {}
  };

  // --------------------------------------------------------------------
  template<class T>
  class pointer_rp : public base_pointer<T> {
  public:
    typedef T value_type;
    pointer_rp() : base_pointer<T>() {}
    pointer_rp( value_type* ptr ) : base_pointer<T>(ptr) {}
    explicit pointer_rp( kaapi_access_t& ptr ) : base_pointer<T>(kaapi_data(value_type, &ptr)) {}
//    pointer_rp( const pointer_rp<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_rp( const pointer_rpwp<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_rp( const pointer<T>& ptr ) : base_pointer<T>(ptr) {}
  };

  template<class T>
  class RP {/* instanciante by default body if signature is not redefined */
    RP() {}
  public:
    RP( const pointer_rp<T>& p ) {}
  };
  

  // --------------------------------------------------------------------
  template<class T>
  class pointer_r : public base_pointer<T> {
  public:
    typedef T value_type;
    pointer_r() : base_pointer<T>() {}
    pointer_r( value_type* ptr ) : base_pointer<T>(ptr) {}
    explicit pointer_r( kaapi_access_t& ptr ) : base_pointer<T>(kaapi_data(value_type, &ptr)) {}
    pointer_r( const pointer_rpwp<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_r( const pointer<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_r( const pointer_rp<T>& ptr ) : base_pointer<T>(ptr) {}
    operator const T* () const { return base_pointer<T>::ptr(); }
    const T& operator*() const { return *base_pointer<T>::ptr(); }
    const T& operator[](int i) const { return base_pointer<T>::ptr()[i]; }
    const T& operator[](unsigned int i) const { return base_pointer<T>::ptr()[i]; }
  };

  template<class T>
  class R  { /* instanciante by default body if signature is not redefined */
  public:
    R( const pointer_r<T>& p ) {}
  };

  // --------------------------------------------------------------------
  template<class T>
  class pointer_wp : public base_pointer<T> {
  public:
    typedef T value_type;
    pointer_wp() : base_pointer<T>() {}
    pointer_wp( value_type* ptr ) : base_pointer<T>(ptr) {}
    explicit pointer_wp( kaapi_access_t& ptr ) : base_pointer<T>(kaapi_data(value_type, &ptr)) {}
    pointer_wp( const pointer_rpwp<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_wp( const pointer<T>& ptr ) : base_pointer<T>(ptr) {}
  };

  template<class T>
  class WP  {/* instanciante by default body if signature is not redefined */
  public:
    WP( const pointer_wp<T>& p ){}
  };

  // --------------------------------------------------------------------
  template<class T>
  class pointer_w : public base_pointer<T> {
  public:
    typedef T value_type;
    pointer_w() : base_pointer<T>() {}
    pointer_w( value_type* ptr ) : base_pointer<T>(ptr) {}
    explicit pointer_w( kaapi_access_t& ptr ) : base_pointer<T>(kaapi_data(value_type, &ptr)) {}
    pointer_w( const pointer_rpwp<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_w( const pointer<T>& ptr ) : base_pointer<T>(ptr) {}
    pointer_w( const pointer_wp<T>& ptr ) : base_pointer<T>(ptr) {}
    operator value_type* () { return base_pointer<T>::ptr(); }
    value_ref<T> operator*() { return value_ref<T>(base_pointer<T>::ptr()); }
    value_ref<T> operator[](int i) { return value_ref<T>(base_pointer<T>::ptr()+i); }
    value_ref<T> operator[](unsigned int i) { return value_ref<T>(base_pointer<T>::ptr()+i); }
  };

  template<class T>
  class W {/* instanciante by default body if signature is not redefined */
  public:
    W( const pointer_w<T>& p ){}
  };

  // --------------------------------------------------------------------
  template<class T>
  class pointer_rw: public base_pointer<T> {
  public:
    typedef T value_type;
    pointer_rw() : base_pointer<T>() {}
    pointer_rw( value_type* ptr ) : base_pointer<T>(ptr) {}
    explicit pointer_rw( kaapi_access_t& ptr ) : base_pointer<T>(kaapi_data(value_type, &ptr)) {}
    pointer_rw( const pointer_rpwp<T>& ptr ) : base_pointer<T>(ptr) {}
    value_type& operator*() { return *base_pointer<T>::ptr(); }
    value_type& operator[](int i) { return base_pointer<T>::ptr()[i]; }
    value_type& operator[](unsigned int i) { return base_pointer<T>::ptr()[i]; }
  };
  
  template<class T>
  class RW {/* instanciante by default body if signature is not redefined */
  public:
    RW( const pointer_rw<T>& p ) {}
  };

  // --------------------------------------------------------------------
  template<class T>
  struct DefaultAdd {
    void operator()( T& result, const T& value ) const
    {
      result += value;
    }
  };
  
  template<class T, class OpCumul = DefaultAdd<T> >
  class CWP {
  public:    
//    typedef T value_type;
//    Shared_cwp( value_type* p ) : ptr(p) {}
//    value_type* ptr;
  };

  template<class T, class OpCumul = DefaultAdd<T> >
  class CW {
  public:
//    typedef T value_type;
//    Shared_cw( value_type* p ) : ptr(p) {}
//    value_type* ptr;
  };

  // --------------------------------------------------------------------  
  class DefaultAttribut {
  public:
    kaapi_task_t* operator()( kaapi_stack_t*, kaapi_task_t* clo) const
    { return clo; }
  };
  extern DefaultAttribut SetDefault;
  
  /* */
  class UnStealableAttribut {
  public:
    kaapi_task_t* operator()( kaapi_stack_t*, kaapi_task_t* clo) const
    { clo->flag |= KAAPI_TASK_STICKY; return clo; }
  };
  inline UnStealableAttribut SetUnStealable()
  { return UnStealableAttribut(); }

  /* like default attribut: not yet distributed computation */
  class SetLocalAttribut {
  public:
    kaapi_task_t* operator()( kaapi_stack_t*, kaapi_task_t* clo) const
    { 
      kaapi_task_setflags( clo, KAAPI_TASK_STICKY );
      return clo; 
    }
  };
  extern SetLocalAttribut SetLocal;

#if 0
  /* DEPRECATED??? to nothing... not yet distributed implementation */
  class AttributSetCost {
    float _cost;
  public:
    AttributSetCost( float c ) : _cost(c) {}
    kaapi_task_t* operator()( kaapi_stack_t*, kaapi_task_t* clo) const
    { return clo; }
  };
  inline AttributSetCost SetCost( float c )
  { return AttributSetCost(c); }
#endif

  /* to nothing... not yet distributed implementation */
  class AttributSetSite {
    int _site;
  public:
    AttributSetSite( int s ) : _site(s) {}
    kaapi_task_t* operator()( kaapi_stack_t*, kaapi_task_t* clo) const
    { return clo; }
  };

  inline AttributSetSite SetSite( int s )
  { return AttributSetSite(s); }
  

  class SetStaticSchedAttribut {
    int _npart;
    int _niter;
  public:
    SetStaticSchedAttribut( int n, int m  ) 
     : _npart(n), _niter(m) {}
    template<class A1_CLO>
    kaapi_task_t* operator()( kaapi_stack_t*, A1_CLO*& clo) const
    { 
      return clo; 
    }
  };
  inline SetStaticSchedAttribut SetStaticSched(int npart, int iter = 1 )
  { return SetStaticSchedAttribut(npart, iter); }

#if 0 //\TODO dans un monde ideal, il faudrait ca
#include "atha_spacecollection.h"
#endif

  template <class T>
  class WrapperFormat {
  public:
    static const Format* format;
    static const Format* get_format() { return &theformat; }
    static const Format theformat;
    static void cstor( void* dest) { new (dest) T; }
    static void dstor( void* dest) { T* d = (T*)dest; d->T::~T(); } 
    static void cstorcopy( void* dest, const void* src) { T* s = (T*)src; new (dest) T(*s); } 
    static void copy( void* dest, const void* src) { T* d = (T*)dest; T* s = (T*)src; *d = *s; } 
    static void assign( void* dest, const void* src) { T* d = (T*)dest; T* s = (T*)src; *d = *s; } 
    static void print( FILE* file, const void* src) { } 
  };
  
  template <class UpdateFnc>
  class WrapperFormatUpdateFnc : public FormatUpdateFnc {
  protected:
    template<class UF, class T, class Y>
    static bool Caller( bool (UF::*)( T&, const Y& ), void* d, const void* v )
    {
      static UpdateFnc ufc;
      T* data = static_cast<T*>(d);
      const Y* value = static_cast<const Y*>(v);
      return ufc( *data, *value );
    }
    
  public:
    static int update_kaapi( void* data, const kaapi_format_t* fmtdata, const void* value, const kaapi_format_t* fmtvalue )
    {
      return Caller( &UpdateFnc::operator(), data, value ) ? 1 : 0;
    }
    static const FormatUpdateFnc* format;
    static const FormatUpdateFnc theformat;
  };

  template <class T>
  const Format WrapperFormat<T>::theformat( typeid(T).name(),
    sizeof(T),
    WrapperFormat<T>::cstor, 
    WrapperFormat<T>::dstor, 
    WrapperFormat<T>::cstorcopy, 
    WrapperFormat<T>::copy, 
    WrapperFormat<T>::assign, 
    WrapperFormat<T>::print 
  );
  template <class T>
  const Format* WrapperFormat<T>::format = &WrapperFormat<T>::theformat;

  template <>
  const Format* WrapperFormat<kaapi_int8_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_int16_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_int32_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_int64_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_uint8_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_uint16_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_uint32_t>::format;
  template <>
  const Format* WrapperFormat<kaapi_uint64_t>::format;
  template <>
  const Format* WrapperFormat<float>::format;
  template <>
  const Format* WrapperFormat<double>::format;

  template <class UpdateFnc>
  const FormatUpdateFnc WrapperFormatUpdateFnc<UpdateFnc>::theformat (
    typeid(UpdateFnc).name(),
    &WrapperFormatUpdateFnc<UpdateFnc>::update_kaapi
  );
  template <class UpdateFnc>
  const FormatUpdateFnc* WrapperFormatUpdateFnc<UpdateFnc>::format = &WrapperFormatUpdateFnc<UpdateFnc>::theformat;

  // --------------------------------------------------------------------
  template<class T>
  struct Trait_ParamClosure {
    typedef T type_inclosure;
    typedef T type_inuserfunction;
    enum { isshared = false };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_V mode;
    enum { modepostponed = false };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_V };
    template<class E>
    static void link( type_inclosure& f, const E& e) { f = e; }
  };

  template<class T>
  struct Trait_ParamClosure<const T&> {
    typedef T type_inclosure;
    typedef const T& type_inuserfunction;
    enum { isshared = false };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_V mode;
    enum { modepostponed = false };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_V };
    template<class E>
    static void link( type_inclosure& f, const E& e) { f = e; }
  };

  template<class T>
  struct Trait_ParamClosure<RW<T> > {
    typedef kaapi_access_t type_inclosure;
    typedef pointer_rw<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_RW mode;
    enum { modepostponed = false };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_RW };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T>
  struct Trait_ParamClosure<R<T> > {
    typedef kaapi_access_t type_inclosure;
    typedef pointer_r<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_R mode;
    enum { modepostponed = false };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_R };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T>
  struct Trait_ParamClosure<W<T> > {
    typedef kaapi_access_t type_inclosure;
    typedef pointer_w<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_W mode;
    enum { modepostponed = false };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_W };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T, class F>
  struct Trait_ParamClosure<CW<T, F> > {
    typedef kaapi_access_t type_inclosure;
//    typedef Shared_cw<T,F> value_type;
//    typedef pointer_rw<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_CW mode;
    enum { modepostponed = false };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_CW };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T>
  struct Trait_ParamClosure<RPWP<T> > {
    typedef kaapi_access_t type_inclosure;
    typedef RPWP<T> value_type;
    typedef pointer_rpwp<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_RPWP mode;
    enum { modepostponed = true };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_RW| KAAPI_ACCESS_MODE_P };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T>
  struct Trait_ParamClosure<RP<T> > {
    typedef kaapi_access_t type_inclosure;
    typedef pointer_rp<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_RP mode;
    enum { modepostponed = true };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_R| KAAPI_ACCESS_MODE_P };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T>
  struct Trait_ParamClosure<WP<T> > {
    typedef kaapi_access_t type_inclosure;
    typedef pointer_wp<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_WP mode;
    enum { modepostponed = true };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_W| KAAPI_ACCESS_MODE_P };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };

  template<class T, class F>
  struct Trait_ParamClosure<CWP<T, F> > {
    typedef kaapi_access_t type_inclosure;
//    typedef Shared_cwp<T,F> value_type;
//    typedef pointer_rpwp<T> type_inuserfunction;
    enum { isshared = true };
    static const kaapi_format_t* get_format() { return WrapperFormat<T>::get_format(); }
    typedef ACCESS_MODE_CWP mode;
    enum { modepostponed = true };
    enum { xkaapi_mode = KAAPI_ACCESS_MODE_CW| KAAPI_ACCESS_MODE_P };
    template<class S>
    static void link( type_inclosure& f, const S& e) { kaapi_access_init(&f, e.ptr()); }
  };


  // --------------------------------------------------------------------
  template<int i>
  struct Task {};
  
  /* ICI: signature avec kaapi_stack & kaapi_task as first parameter ?
     Quel interface C++ pour les tâches adaptatives ?
  */

} // end of namespace atha: following definition sould be in global namespace in 
  // order to be specialized easily

  // --------------------------------------------------------------------
  
  template<class TASK>
  struct TaskBodyCPU : public TASK {};

  // --------------------------------------------------------------------
  template<class TASK>
  struct TaskBodyGPU : public TASK {};


namespace ka {

  // --------------------------------------------------------------------
  template<class TASK>
  struct KaapiTask0 {
    static void body( kaapi_task_t* task, kaapi_stack_t* stack )
    { 
      static TASK dummy;
      dummy();
    }
  };

#include "ka_api_clo.h"

  // --------------------------------------------------------------------
  /* New API: thread.Spawn<TASK>([ATTR])( args )
     Spawn<TASK>([ATTR])(args) with be implemented on top of 
     System::get_current_thread()->Spawn<TASK>([ATTR])( args ).
  */
  class Thread {
  public:

    template<class T>
    T* Alloca(size_t size)
    {
       void* data = kaapi_stack_pushdata( &_stack, sizeof(T)*size );
       return new (data) T[size];
    }

    template<class TASK, class Attr>
    class Spawner {
    public:
      Spawner( kaapi_stack_t* s, const Attr& a ) : _stack(s), _attr(a) {}

      /**
      **/      
      void operator()()
      { 
        kaapi_task_t* clo = kaapi_stack_toptask( _stack);
        kaapi_task_initdfg( _stack, clo, KaapiTask0<TASK>::body, 0 );
        _attr(_stack, clo);
        kaapi_stack_pushtask( _stack);    
      }

#include "ka_api_fork.h"

    protected:
      kaapi_stack_t* _stack;
      const Attr&    _attr;
    };
        
    template<class TASK>
    Spawner<TASK, DefaultAttribut> Spawn() { return Spawner<TASK, DefaultAttribut>(&_stack, DefaultAttribut()); }

    template<class TASK, class Attr>
    Spawner<TASK, Attr> Spawn(const Attr& a) { return Spawner<TASK, Attr>(&_stack, a); }

  protected:
    kaapi_stack_t _stack;
    friend class SyncGuard;
  };

  
  
  // --------------------------------------------------------------------
  /** Top level Spawn */
  template<class TASK>
  Thread::Spawner<TASK, DefaultAttribut> Spawn() { return Thread::Spawner<TASK, DefaultAttribut>(kaapi_self_stack(), DefaultAttribut()); }

  template<class TASK, class Attr>
  Thread::Spawner<TASK, Attr> Spawn(const Attr& a) { return Thread::Spawner<TASK, Attr>(kaapi_self_stack(), a); }



  // --------------------------------------------------------------------
  /** Wait execution of all forked tasks of the running task */
  extern void Sync();



  // --------------------------------------------------------------------
  /* Main task */
  template<class TASK>
  struct MainTask {
    int    argc;
    char** argv;
    static void body_cpu( kaapi_task_t* task, kaapi_stack_t* stack )
    {
      MainTask<TASK>* args = kaapi_task_getargst( task, MainTask<TASK>);
      TASK()( args->argc, args->argv );
    }
    static void body_gpu( kaapi_task_t* task, kaapi_stack_t* stack )
    {
      MainTask<TASK>* args = kaapi_task_getargst( task, MainTask<TASK>);
      TASK()( args->argc, args->argv );
    }
    void operator()( kaapi_task_t* task, kaapi_stack_t* stack )
    {
      MainTask<TASK>* args = kaapi_task_getargst( task, MainTask<TASK>);
      TASK()( args->argc, args->argv );
    }
    static kaapi_format_t* registerformat()
    {
      if (MainTask::fmid != 0) return &MainTask::format;
      MainTask::fmid = kaapi_format_taskregister( 
            &MainTask::getformat, 
            -1, 
            &MainTask::body_cpu, 
            typeid(MainTask).name(),
            sizeof(MainTask),
            0,
            0,
            0,
            0
        );
     int (TASK::*f_defaultcpu)(...) = (int (TASK::*)(...))&TASK::operator();  /* inherited from Signature */
     int (TASK::*f_cpu)(...) = (int (TASK::*)(...))&TaskBodyCPU<TASK>::operator();
     if (f_cpu == f_defaultcpu) {
       MainTask::format.entrypoint[KAAPI_PROC_TYPE_CPU] = 0;
     }
     else {
       MainTask::format.entrypoint[KAAPI_PROC_TYPE_CPU] = &MainTask::body_cpu;
     }
     int (MainTask::*f_defaultgpu)(...) = (int (MainTask::*)(...))&MainTask::operator();  /* inherited from Signature */
     int (MainTask::*f_gpu)(...) = (int (MainTask::*)(...))&TaskBodyGPU<MainTask>::operator();
     if (f_gpu == f_defaultgpu) {
       MainTask::format.entrypoint[KAAPI_PROC_TYPE_GPU] = 0;
     }
     else {
       MainTask::format.entrypoint[KAAPI_PROC_TYPE_GPU] = &MainTask::body_gpu;
     }
      return &MainTask::format;
    }  
    static const kaapi_task_bodyid_t bodyid;
    static kaapi_format_t    format;
    static kaapi_format_id_t fmid;
    static kaapi_format_t* getformat()
    { return &format; }
  };
  
  template<class TASK>
  kaapi_format_t    MainTask<TASK>::format;
  template<class TASK>
  kaapi_format_id_t MainTask<TASK>::fmid =0;

  template<class TASK>
  const kaapi_task_bodyid_t MainTask<TASK>::bodyid = registerformat()->bodyid;
  
  template<class TASK>
  struct SpawnerMain
  {
    SpawnerMain() 
    { }

    void operator()( int argc, char** argv)
    {
      kaapi_stack_t* stack = kaapi_self_stack();
      kaapi_task_t* clo = kaapi_stack_toptask( stack);
      kaapi_task_initdfg( stack, clo, MainTask<TASK>::bodyid, kaapi_stack_pushdata(stack, sizeof(MainTask<TASK>)) );
      kaapi_task_setflags( clo, KAAPI_TASK_STICKY );
      MainTask<TASK>* arg = kaapi_task_getargst( clo, MainTask<TASK>);
      arg->argc = argc;
      arg->argv = argv;
      kaapi_stack_pushtask( stack);    
    }
  };

  template<class TASK>
  SpawnerMain<TASK> SpawnMain()
  { 
    return SpawnerMain<TASK>();
  }
    


  // --------------------------------------------------------------------
  extern std::ostream& logfile();

  // --------------------------------------------------------------------
  class SyncGuard {
      Thread       *_thread;
      kaapi_frame_t _frame;
  public:
      SyncGuard() : _thread( System::get_current_thread() )
      {
        kaapi_stack_save_frame( &_thread->_stack, &_frame );
      }
      ~SyncGuard()
      {
        kaapi_sched_sync( &_thread->_stack );
        kaapi_stack_restore_frame( &_thread->_stack, &_frame );
      }
  };
} // namespace ka

#ifndef _KAAPIPLUSPLUS_NOT_IN_NAMESPACE
using namespace ka;
#endif

#endif

