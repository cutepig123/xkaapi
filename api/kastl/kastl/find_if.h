/*
 ** xkaapi
 ** 
 **
 ** Copyright 2009,2010,2011,2012 INRIA.
 **
 ** Contributors :
 **
 ** thierry.gautier@inrialpes.fr
 ** fabien.lementec@gmail.com / fabien.lementec@imag.fr
 
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


#ifndef KASTL_FIND_IF_H_INCLUDED
# define KASTL_FIND_IF_H_INCLUDED


#include "kastl_loop.h"
#include "kastl_sequences.h"


namespace kastl
{

template<typename Iterator, typename Predicate>
struct find_if_body
{
  typedef kastl::impl::touched_algorithm_result<Iterator> result_type;

  Predicate _pred;

  find_if_body(const Predicate& pred)
    : _pred(pred) {}

  bool operator()(result_type& res, const Iterator& pos)
  {
    if (!_pred(*pos))
      return false;

    res.set_iter(pos);
    return true;
  }

  bool reduce(result_type& lhs, const result_type& rhs)
  {
    if (rhs._is_touched == false)
      return false;
    lhs.set_iter(rhs._iter);
    return true;
  }

};

template<typename Iterator, typename Predicate, typename Settings>
Iterator find_if
(Iterator first, Iterator last, Predicate pred, const Settings& settings)
{
  kastl::rts::Sequence<Iterator> seq(first, last - first);
  find_if_body<Iterator, Predicate> body(pred);
  kastl::impl::touched_algorithm_result<Iterator> res(last);
  kastl::impl::while_reduce_loop(res, seq, body, settings);
  return res._iter;
}

template<typename Iterator, typename Predicate>
Iterator find_if(Iterator first, Iterator last, Predicate pred)
{
  kastl::impl::static_settings settings(512, 512);
  return kastl::find_if(first, last, pred, settings);
}

} // kastl::



#endif // KASTL_FIND_IF_H_INCLUDED
