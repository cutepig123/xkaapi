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
#ifndef _KAAPI_CPUSET_H_
#define _KAAPI_CPUSET_H_ 1

#if defined(__cplusplus)
extern "C" {
#endif


/* ========================================================================= */
/* Implementation note: kaapi_cpuset_t is the public type.
   It should possible to cast a kaapi_cpuset_t* to a kaapi_bitmap_value_t*.
   It means that bit must match.   
*/
typedef union {
    uint64_t     bits64[4];
} kaapi_cpuset_t;


/**
*/
extern const char* kaapi_cpuset2string( int nproc, kaapi_cpuset_t* affinity );


/**
*/
static inline void kaapi_cpuset_clear(kaapi_cpuset_t* affinity )
{
  affinity->bits64[0] = 0;
  affinity->bits64[1] = 0;
  affinity->bits64[2] = 0;
  affinity->bits64[3] = 0;
}


/**
*/
static inline void kaapi_cpuset_full(kaapi_cpuset_t* affinity )
{
  affinity->bits64[0] = ~0UL;
  affinity->bits64[1] = ~0UL;
  affinity->bits64[2] = ~0UL;
  affinity->bits64[3] = ~0UL;
}


/**
*/
static inline int kaapi_cpuset_intersect(
    const kaapi_cpuset_t* s1, 
    const kaapi_cpuset_t* s2
)
{
  return ((s1->bits64[0] & s2->bits64[0]) != 0) 
      || ((s1->bits64[1] & s2->bits64[1]) != 0)
      || ((s1->bits64[2] & s2->bits64[2]) != 0)
      || ((s1->bits64[3] & s2->bits64[3]) != 0);
}


/**
*/
static inline int kaapi_cpuset_empty(const kaapi_cpuset_t* affinity)
{
  return (affinity->bits64[0] == 0) 
      && (affinity->bits64[1] == 0)
      && (affinity->bits64[2] == 0)
      && (affinity->bits64[3] == 0)
  ;
}


/**
*/
static inline int kaapi_cpuset_set(
    kaapi_cpuset_t* affinity, 
    unsigned int i 
)
{
  kaapi_assert_debug( i < sizeof(kaapi_cpuset_t)*8 );
  if (i <64)
    affinity->bits64[0] |= ((uint64_t)1)<<i;
  else if (i <128)
    affinity->bits64[1] |= ((uint64_t)1)<<(i-64);
  else if (i <192)
    affinity->bits64[2] |= ((uint64_t)1)<<(i-128);
  else
    affinity->bits64[3] |= ((uint64_t)1)<<(i-192);
  return 0;
}


/**
*/
static inline int kaapi_cpuset_copy(
    kaapi_cpuset_t* dest, 
    const kaapi_cpuset_t* src 
)
{
  dest->bits64[0] = src->bits64[0];
  dest->bits64[1] = src->bits64[1];
  dest->bits64[2] = src->bits64[2];
  dest->bits64[3] = src->bits64[3];
  return 0;
}


/* return position of the lowest bit set to 1 and zero it.
   return -1 iff cpuset is empty
*/
extern int kaapi_cpuset_firstone_zero(
    kaapi_cpuset_t* cpuset
);

/** Return non 0 iff th as affinity with kid
*/
static inline int kaapi_cpuset_has(
    const kaapi_cpuset_t* affinity, 
    unsigned int i 
)
{
  kaapi_assert_debug( i < sizeof(kaapi_cpuset_t)*8 );
  if (i <64)
    return (affinity->bits64[0] & ((uint64_t)1)<<i) !=0;
  else if (i <128)
    return (affinity->bits64[1] & ((uint64_t)1)<<(i-64)) !=0;
  else if (i <192)
    return (affinity->bits64[2] & ((uint64_t)1)<<(i-128)) !=0;
  else
    return (affinity->bits64[3] & ((uint64_t)1)<<(i-192)) !=0;
}

/** Return *dest &= mask
*/
static inline void kaapi_cpuset_and(kaapi_cpuset_t* dest, const kaapi_cpuset_t* mask )
{
  dest->bits64[0] &= mask->bits64[0];
  dest->bits64[1] &= mask->bits64[1];
  dest->bits64[2] &= mask->bits64[2];
  dest->bits64[3] &= mask->bits64[3];
}

/** Return *dest |= mask
*/
static inline void kaapi_cpuset_or(kaapi_cpuset_t* dest, const kaapi_cpuset_t* mask )
{
  dest->bits64[0] |= mask->bits64[0];
  dest->bits64[1] |= mask->bits64[1];
  dest->bits64[2] |= mask->bits64[2];
  dest->bits64[3] |= mask->bits64[3];
}

/** Return *dest &= ~mask
*/
static inline void kaapi_cpuset_notand(kaapi_cpuset_t* dest, const kaapi_cpuset_t* mask )
{
  dest->bits64[0] ^= mask->bits64[0];
  dest->bits64[1] ^= mask->bits64[1];
  dest->bits64[2] ^= mask->bits64[2];
  dest->bits64[3] ^= mask->bits64[3];
}

#if defined(__cplusplus)
}
#endif

#endif
