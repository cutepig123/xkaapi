/*
 *  test_sort.cpp
 *  xkaapi
 *
 *  Created by DT on juin 2009.
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */
#ifndef _CKAAPI_SORT_H
#define _CKAAPI_SORT_H
#include "kaapi_adapt.h"
#include <algorithm>
#include<numeric>
#include<functional>
#include<list>


template<class RandomAccessIterator, class Compare>
void  sort ( kaapi_steal_context_t* stealcontext, 
                    RandomAccessIterator begin, RandomAccessIterator end, Compare comp);

template<class RandomAccessIterator>
void  sort ( kaapi_steal_context_t* stealcontext, 
                    RandomAccessIterator begin, RandomAccessIterator end);

typedef ptrdiff_t Dis_type;


template <typename T, typename cmp=std::less<T> >
struct csort_compare_to_median
{
  T _value;
  csort_compare_to_median(){};
  csort_compare_to_median(T value):_value(value){ }
  bool operator()(T value) {
     return cmp()(value, _value);
  }
};


/** Stucture of a work for sort
*/
template<class RandomAccessIterator, class Compare>
class SortStruct {
public:
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type val_t;
  typedef typename std::pair<RandomAccessIterator, RandomAccessIterator> Interval_t;
  /* cstor */
  SortStruct(
    kaapi_steal_context_t* sc,
    RandomAccessIterator ibeg,
    RandomAccessIterator iend,
    Compare comp) : _sc(sc), _ibeg(ibeg), _iend(iend), _comp(comp)
  {
    _work_list = new std::list< Interval_t >();
  }
  
  /* do sort */
  void doit();


 
protected:  
  kaapi_steal_context_t* _sc;
  RandomAccessIterator  _ibeg;
  RandomAccessIterator  _iend;
  Compare _comp;
  std::list< Interval_t >* _work_list;
 
   /* split_work */
  void split_work( ) 
  {
      Dis_type sz = _iend-_ibeg;
      val_t median = val_t(std:: __median(*_ibeg, *(_ibeg+sz/2), *(_ibeg+sz-1)));
     //Partion into two parts
      RandomAccessIterator split = std::partition(_ibeg, _iend, csort_compare_to_median<val_t>(median));
      
      if( (_iend-split) >= (split-_ibeg) ) {
         _work_list->push_front(std::make_pair(split, _iend));
         _iend = split;
      } else {
        _work_list->push_front(std::make_pair(_ibeg, split));
        _ibeg = split;
      }
  
  }

  /* Entry in case of thief execution */
  static void thief_entrypoint(kaapi_steal_context_t* sc, void* data)
  {
    SortStruct<RandomAccessIterator, Compare>* w = (SortStruct<RandomAccessIterator, Compare>*)data;
    w->_sc = sc;
    w->doit();
  }

  /** splitter_work is called within the context of the steal point
  */
  static void splitter( kaapi_steal_context_t* stealcontext, int count, kaapi_steal_request_t** request, 
                        RandomAccessIterator& ibeg, RandomAccessIterator& iend, Compare comp,
                        std::list< Interval_t >* work_list )
  {
    
    Dis_type size = iend-ibeg; 
    RandomAccessIterator local_end = iend;
    RandomAccessIterator local_beg = ibeg;
    Dis_type par_grain = 32;
    SortStruct<RandomAccessIterator, Compare>* output_work =0;
    int i = 0;
    int nb_work = work_list->size();    
    
    /* threshold should be defined (...) */
    if(!nb_work) {
       if (size < par_grain) goto reply_failed;
    }
 
    while (count >0)
    {

      size =  local_end - local_beg; 
     
      /** split work if no work to steal */
       nb_work = work_list->size();
       if(!nb_work || size < par_grain ) goto reply_failed;  

       /*if(!nb_work) 
       {
       
         if ( (size < par_grain) ) goto reply_failed;
        
          //std::cout << "splitting......" << std::endl;
         // split reamining work into two parts 
         val_t median = val_t(std:: __median(*local_beg, *(local_beg+size/2), *(local_beg+size-1)));
         //Partion into two parts
         RandomAccessIterator split = std::partition(local_beg, local_end, csort_compare_to_median<val_t>(median));

        if( (local_end-split) >= (split-local_beg) ) {
           work_list->push_front(std::make_pair(split, local_end));
           local_end = split;
        } else {
          work_list->push_front(std::make_pair(local_beg, split));
          local_beg = split;
        }
 
      }*/

      if (request[i] !=0)
      {
        
         //static int cpt = 0;
         //if(stealcontext->_stack->_index==0) std::cout << "id 0 : nb_steal = " << ++cpt << std::endl;

        if (kaapi_steal_context_alloc_result( stealcontext,
                                              request[i],
                                              (void**)&output_work,
                                              sizeof(SortStruct<RandomAccessIterator, Compare>)
                                             ) ==0)
        {



         std::pair<RandomAccessIterator, RandomAccessIterator> interval = work_list->back();
         output_work->_ibeg = interval.first;
         output_work->_iend = interval.second;
         output_work->_comp = comp;
         output_work->_work_list = new std::list<typename std::pair<RandomAccessIterator, RandomAccessIterator> >(); 
         work_list->pop_back();

         //xkaapi_assert( output_work->_iend - output_work->_ibeg >=0);
        //std::cout << "output_work->_iend-output_work->_ibeg=" << output_work->_iend-output_work->_ibeg << std::endl;
          /* reply ok (1) to the request */
         kaapi_request_reply( request[i], stealcontext, &thief_entrypoint, 1, CKAAPI_MASTER_FINALIZE_FLAG);

       }
        else {
        /* reply failed (=last 0 in parameter) to the request */
        kaapi_request_reply( request[i], stealcontext, 0, 0, CKAAPI_DEFAULT_FINALIZE_FLAG);
     }
        --count;
    }
    ++i;
  }
  /* mute the beg and end of input work of the victim */
  iend  = local_end;
  ibeg  = local_beg;
  xkaapi_assert( iend - ibeg >=0);
  return;

   reply_failed :
     while (count >0)
    {
      if (request[i] !=0)
      {
        /* reply failed (=last 0 in parameter) to the request */
        kaapi_request_reply( request[i], stealcontext, 0, 0, CKAAPI_DEFAULT_FINALIZE_FLAG);
        --count;
      }
      ++i;
    }
   /* mute the beg and end of input work of the victim */
  iend  = local_end;
  ibeg  = local_beg;
  xkaapi_assert( iend - ibeg >=0);
 }


/* Called by the victim thread to collect work from one other thread
  */
static void reducer( kaapi_steal_context_t* sc, void* thief_data,
                     SortStruct<RandomAccessIterator, Compare>* victim_data)
 {


 }

};


/** Adaptive sort
*/
template<class RandomAccessIterator, class Compare>
void SortStruct<RandomAccessIterator, Compare>::doit()
{

  /* amount of work per iteration of the nano loop */
  Dis_type unit_size  = 1024;

  /* call of std::partition to split work into two parts*/
  Dis_type tmp_size = tmp_size = _iend-_ibeg ;
  bool not_finished = true;
  int nb_work = 0;   
 
   //To anticipate the work to steal
   if(tmp_size > unit_size) {
        //std::cout << "1 tmp_size = " << tmp_size << std::endl;
       split_work( );
   }

  //Local computation
  do {
    /* definition of the steal point where steal_work may be called in case of steal request
       -here size is pass as parameter and updated in case of steal.
    */

    kaapi_stealpoint( _sc, splitter, _ibeg, _iend, _comp, _work_list);

     tmp_size = _iend-_ibeg;
     /* call of std::partition to split work into two parts*/
     if(tmp_size > unit_size) {
        //std::cout << "1 tmp_size = " << tmp_size << std::endl; 
       split_work( );
     } 
     else {
       //std::cout << "2 tmp_size = " << tmp_size << std::endl;
     /** Local sorting */
      std::sort(_ibeg, _iend, _comp);

      nb_work = _work_list->size();

      if(!nb_work) not_finished = false;
      else {
        /** pop next work */
        std::pair<RandomAccessIterator, RandomAccessIterator> interval = _work_list->front();
        _ibeg = interval.first;
        _iend = interval.second;
        _work_list->pop_front();
        _ibeg = interval.first;
        _iend = interval.second;

        not_finished = true;
       }
     }
   } while ( not_finished  );

  /* definition of the finalization point where all stolen work a interrupt and collected */
  kaapi_finalize_steal( _sc, 0, (kaapi_reducer_function_t)&reducer, this);

}

/**
*/
template<class RandomAccessIterator, class Compare>
void sort(kaapi_steal_context_t* stealcontext, 
                 RandomAccessIterator begin, RandomAccessIterator end, Compare comp)
{
  typedef typename std::iterator_traits<RandomAccessIterator>::pointer ptr_type;
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type val_t;
  Dis_type size = end-begin;
  SortStruct<RandomAccessIterator, Compare> work( stealcontext, begin, end, comp);
  work.doit();
}

template<class RandomAccessIterator>
void sort(kaapi_steal_context_t* stealcontext, 
                 RandomAccessIterator begin, RandomAccessIterator end)
{
  kaapi_steal_context_initpush( stealcontext );
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type val_t;
  sort(stealcontext, begin, end, std::less<val_t>());
}

#endif
