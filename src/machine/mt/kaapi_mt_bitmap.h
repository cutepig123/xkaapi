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
#ifndef _KAAPI_MT_BITMAP_H_
#define _KAAPI_MT_BITMAP_H_ 1

#ifndef _KAAPI_IMPL_H
#  error This file must not be directly included. Use kaapi_impl.h instead
#endif
#include <stdint.h>


/* ============================= Kprocessor ============================ */

#  if (KAAPI_MAX_PROCESSOR == 0)
#    define KAAPI_MAX_PROCESSOR_GENERIC
#    define KAAPI_MAX_PROCESSOR_32
#    define KAAPI_MAX_PROCESSOR_64
#    define KAAPI_MAX_PROCESSOR_128
#    define KAAPI_MAX_PROCESSOR_192
#    define KAAPI_MAX_PROCESSOR_256
//#    define KAAPI_MAX_PROCESSOR_LARGE
#  elif (KAAPI_MAX_PROCESSOR <=32)
#    define KAAPI_MAX_PROCESSOR_32
#  elif (KAAPI_MAX_PROCESSOR <=64)
#    define KAAPI_MAX_PROCESSOR_64
#  elif (KAAPI_MAX_PROCESSOR <=128)
#    define KAAPI_MAX_PROCESSOR_128
#  elif (KAAPI_MAX_PROCESSOR <=192)
#    define KAAPI_MAX_PROCESSOR_192
#  elif (KAAPI_MAX_PROCESSOR <=256)
#    define KAAPI_MAX_PROCESSOR_256
#  else // (KAAPI_MAX_PROCESSOR >256)
#    define KAAPI_MAX_PROCESSOR_LARGE
#  endif

#  ifdef KAAPI_MAX_PROCESSOR_LARGE
#    define KAAPI_MAX_PROCESSOR_LIMIT ((unsigned int)(-1) >> 1)
#  elif defined(KAAPI_MAX_PROCESSOR_256)
#    define KAAPI_MAX_PROCESSOR_LIMIT 256
#  elif defined(KAAPI_MAX_PROCESSOR_192)
#    define KAAPI_MAX_PROCESSOR_LIMIT 192
#  elif defined(KAAPI_MAX_PROCESSOR_128)
#    define KAAPI_MAX_PROCESSOR_LIMIT 128
#  elif defined(KAAPI_MAX_PROCESSOR_64)
#    define KAAPI_MAX_PROCESSOR_LIMIT 64
#  elif defined(KAAPI_MAX_PROCESSOR_32)
#    define KAAPI_MAX_PROCESSOR_LIMIT 32
#  else
#    error No implementation are available to select processors
#  endif


typedef struct {
	kaapi_atomic32_t proc32;
} kaapi_bitmap32_t;
typedef struct {
	kaapi_atomic64_t proc64;
} kaapi_bitmap64_t;
typedef struct {
	kaapi_atomic64_t proc128[2];
} kaapi_bitmap128_t;
typedef struct {
        kaapi_atomic64_t proc192[3];
} kaapi_bitmap192_t;
typedef struct {
        kaapi_atomic64_t proc256[4];
} kaapi_bitmap256_t;

typedef struct {
	uint32_t proc32;
} kaapi_bitmap_value32_t;
typedef struct {
	uint64_t proc64;
} kaapi_bitmap_value64_t;
typedef struct {
	uint64_t proc128[2];
} kaapi_bitmap_value128_t;
typedef struct {
        uint64_t proc192[3];
} kaapi_bitmap_value192_t;
typedef struct {
        uint64_t proc256[4];
} kaapi_bitmap_value256_t;


/******* 32 */
static inline void kaapi_bitmap_clear_32( kaapi_bitmap32_t* b ) 
{
  KAAPI_ATOMIC_WRITE(&b->proc32, 0);
}

static inline int kaapi_bitmap_empty_32( kaapi_bitmap32_t* b )
{ 
  return (KAAPI_ATOMIC_READ(&b->proc32) ==0);
}

static inline int kaapi_bitmap_init_32( 
    kaapi_bitmap32_t* b,
    const kaapi_bitmap_value32_t* v 
)
{ 
  KAAPI_ATOMIC_WRITE(&b->proc32, v->proc32);
  return 0;
}

static inline void kaapi_bitmap_value_clear_32( kaapi_bitmap_value32_t* b )
{ 
  b->proc32 =0;
}

static inline void kaapi_bitmap_value_full_32( kaapi_bitmap_value32_t* b )
{ 
  b->proc32 =~(uint32_t)0;
}

static inline int kaapi_bitmap_value_empty_32( const kaapi_bitmap_value32_t* b )
{ 
  return b->proc32 ==0;
}

static inline int kaapi_bitmap_value_count_32( const kaapi_bitmap_value32_t* b )
{ 
  return __builtin_popcount(b->proc32);
}

static inline void kaapi_bitmap_value_set_32( kaapi_bitmap_value32_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<32) && (i>=0) );
  (b->proc32) |= ((uint32_t)1)<< i; 
}

static inline int kaapi_bitmap_value_get_32( const kaapi_bitmap_value32_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<32) && (i>=0) );
  return ((b->proc32) & ((uint32_t)1)<< i) !=0; 
}

static inline void kaapi_bitmap_value_unset_32( kaapi_bitmap_value32_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<32) && (i>=0) );
  (b->proc32) &= ~(((uint32_t)1)<< i); 
}

static inline void kaapi_bitmap_value_copy_32( kaapi_bitmap_value32_t* retval, const kaapi_bitmap_value32_t* b ) 
{ 
  retval->proc32 = b->proc32;
}

static inline void kaapi_bitmap_swap0_32( kaapi_bitmap32_t* b, kaapi_bitmap_value32_t* v ) 
{
  v->proc32 = KAAPI_ATOMIC_AND_ORIG(&b->proc32, 0);
}

static inline void kaapi_bitmap_or0_32( kaapi_bitmap32_t* b, kaapi_bitmap_value32_t* v ) 
{
  v->proc32 |= KAAPI_ATOMIC_AND_ORIG(&b->proc32, 0);
}

static inline void kaapi_bitmap_and_32
(
 kaapi_bitmap_value32_t* fu,
 kaapi_bitmap32_t* b,
 /* const */ kaapi_bitmap_value32_t* v
) 
{
  fu->proc32 = KAAPI_ATOMIC_AND_ORIG(&b->proc32, v->proc32);
}

static inline void kaapi_bitmap_value_neg_32
(
 kaapi_bitmap_value32_t* fu,
 const kaapi_bitmap_value32_t* bar
) 
{
  fu->proc32 = ~bar->proc32;
}

static inline void kaapi_bitmap_value_or_32
( kaapi_bitmap_value32_t* fu, const kaapi_bitmap_value32_t* bar ) 
{
  fu->proc32 |= bar->proc32;
}

static inline void kaapi_bitmap_value_and_32
( kaapi_bitmap_value32_t* fu, const kaapi_bitmap_value32_t* bar ) 
{
  fu->proc32 &= bar->proc32;
}

static inline int kaapi_bitmap_set_32( kaapi_bitmap32_t* b, int i )
{
  kaapi_assert_debug( (i<32) && (i>=0) );
  KAAPI_ATOMIC_OR(&b->proc32, 1U<<i);
  return 0;
}

static inline int kaapi_bitmap_get_32( const kaapi_bitmap32_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<32) && (i>=0) );
  return kaapi_bitmap_value_get_32((const kaapi_bitmap_value32_t*)b, i);
}

static inline int kaapi_bitmap_unset_32( kaapi_bitmap32_t* b, int i )
{
  uint32_t x;
  kaapi_assert_debug( (i<32) && (i>=0) );
  x = KAAPI_ATOMIC_AND_ORIG(&b->proc32, ~(1U<<i));
  /* was not unset before, success */
  if (x & (1 << i)) return 0;
  return -1;
}

static inline int kaapi_bitmap_count_32( const kaapi_bitmap32_t* b ) 
{ return __builtin_popcount(KAAPI_ATOMIC_READ(&b->proc32)); }

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_value_first1_and_zero_32( kaapi_bitmap_value32_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffs( b->proc32 );
  if (fb ==0) return 0;
  b->proc32 &= ~( 1 << (fb-1) );
  return fb;
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0. Return 0 if atomicity is failed.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero_32( kaapi_bitmap32_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffs( KAAPI_ATOMIC_READ(&b->proc32) );
  if (fb ==0) return 0;
  if (kaapi_bitmap_unset_32(b, fb-1) ==0)
    return fb;
  return 0;
}

/* set all the [0, i[ bits 
*/
static inline void kaapi_bitmap_value_set_low_bits_32( kaapi_bitmap_value32_t* b, unsigned int i)
{
  kaapi_assert_debug(i <= 32);
  if (i == 32) 
    b->proc32 = ~(uint32_t)0;
  else
    b->proc32 = ((uint32_t)1 << i) - (uint32_t)1;
}

/* set all the [0, i[ bits 
*/
static inline void kaapi_bitmap_set_low_bits_32( kaapi_bitmap32_t* b, unsigned int i)
{
  kaapi_assert_debug(i < 32);
  KAAPI_ATOMIC_WRITE(&b->proc32, ((uint32_t)1 << i) - (uint32_t)1);
}


static inline int kaapi_bitmap_value_first1_32( const kaapi_bitmap_value32_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  return __builtin_ffs( b->proc32 );
}

static inline int kaapi_bitmap_first1_32( const kaapi_bitmap32_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  return __builtin_ffs( b->proc32._counter );
}


/******* 64 */
static inline void kaapi_bitmap_clear_64( kaapi_bitmap64_t* b ) 
{ KAAPI_ATOMIC_WRITE(&b->proc64, 0); }

static inline int kaapi_bitmap_empty_64( kaapi_bitmap64_t* b )
{ 
  return (KAAPI_ATOMIC_READ(&b->proc64) ==0);
}

static inline int kaapi_bitmap_init_64( 
    kaapi_bitmap64_t* b,
    const kaapi_bitmap_value64_t* v 
)
{ 
  KAAPI_ATOMIC_WRITE(&b->proc64, v->proc64);
  return 0;
}

 static inline int kaapi_bitmap_value_count_64
( const kaapi_bitmap_value64_t* b )
{ 
  return __builtin_popcountl(b->proc64);
}

static inline void kaapi_bitmap_value_clear_64( kaapi_bitmap_value64_t* b )
{ 
  b->proc64 =0;
}

static inline void kaapi_bitmap_value_full_64( kaapi_bitmap_value64_t* b )
{ 
  b->proc64 =~(uint64_t)0;
}

static inline int kaapi_bitmap_value_empty_64( const kaapi_bitmap_value64_t* b )
{ 
  return b->proc64 == 0;
}

static inline void kaapi_bitmap_value_set_64( kaapi_bitmap_value64_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<64) && (i>=0) );
  (b->proc64) |= ((uint64_t)1)<< i; 
}

static inline int kaapi_bitmap_value_get_64( const kaapi_bitmap_value64_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<64) && (i>=0) );
  return ((b->proc64) & ((uint64_t)1)<< i) !=0; 
}

static inline void kaapi_bitmap_value_unset_64( kaapi_bitmap_value64_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<64) && (i>=0) );
  (b->proc64) &= ~(((uint64_t)1)<< i); 
}

static inline void kaapi_bitmap_value_copy_64( kaapi_bitmap_value64_t* retval, const kaapi_bitmap_value64_t* b ) 
{ 
  retval->proc64 = b->proc64;
}

static inline void kaapi_bitmap_copy_64( kaapi_bitmap_value64_t* retval, const kaapi_bitmap64_t* b ) 
{ 
  retval->proc64 = KAAPI_ATOMIC_READ(&b->proc64);
}

static inline void kaapi_bitmap_swap0_64( kaapi_bitmap64_t* b, kaapi_bitmap_value64_t* v ) 
{
  v->proc64 = KAAPI_ATOMIC_AND64_ORIG(&b->proc64, (uint64_t)0);
}

static inline void kaapi_bitmap_or0_64( kaapi_bitmap64_t* b, kaapi_bitmap_value64_t* v ) 
{
  v->proc64 |= KAAPI_ATOMIC_AND64_ORIG(&b->proc64, (uint64_t)0);
}

static inline void kaapi_bitmap_and_64
(
 kaapi_bitmap_value64_t* fu,
 kaapi_bitmap64_t* b,
 kaapi_bitmap_value64_t* v
) 
{
  fu->proc64 = KAAPI_ATOMIC_AND_ORIG(&b->proc64, v->proc64);
}

static inline void kaapi_bitmap_value_neg_64
(
 kaapi_bitmap_value64_t* fu,
 const kaapi_bitmap_value64_t* bar
) 
{
  fu->proc64 = ~bar->proc64;
}

static inline void kaapi_bitmap_value_or_64
( kaapi_bitmap_value64_t* fu, const kaapi_bitmap_value64_t* bar ) 
{
  fu->proc64 |= bar->proc64;
}

static inline void kaapi_bitmap_value_and_64
( kaapi_bitmap_value64_t* fu, const kaapi_bitmap_value64_t* bar ) 
{
  fu->proc64 &= bar->proc64;
}

static inline int kaapi_bitmap_set_64( kaapi_bitmap64_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<64) && (i>=0) );
  KAAPI_ATOMIC_OR64(&b->proc64, ((uint64_t)1)<<i); 
  return 1;
}

static inline int kaapi_bitmap_fetch_and_set_64( kaapi_bitmap64_t* b, int i ) 
{ 
  uint64_t x;
  
  kaapi_assert_debug( (i<64) && (i>=0) );
  x = KAAPI_ATOMIC_OR64_ORIG(&b->proc64, ((uint64_t)1)<<i); 
  return (x & ((uint64_t)1 << (uint64_t)i)) !=0;
}

static inline int kaapi_bitmap_get_64( const kaapi_bitmap64_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<64) && (i>=0) );
  return kaapi_bitmap_value_get_64((const kaapi_bitmap_value64_t*)b, i);
}

static inline int kaapi_bitmap_unset_64( kaapi_bitmap64_t* b, int i ) 
{ 
  uint64_t x;

  kaapi_assert_debug( (i<64) && (i>=0) );
  x = KAAPI_ATOMIC_AND64_ORIG(&b->proc64, ~(((uint64_t)1)<<i)); 
  /* was not unset before, success */
  if (x & ((uint64_t)1 << i)) return 0;
  return -1;
}

static inline void kaapi_bitmap_full_except_64( kaapi_bitmap64_t* b, int i )  
{
  KAAPI_ATOMIC_WRITE( &b->proc64, ~(((uint64_t)1)<<i) );
}

static inline int kaapi_bitmap_count_64( const kaapi_bitmap64_t* b ) 
{ return __builtin_popcountl(KAAPI_ATOMIC_READ(&b->proc64)); }

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_value_first1_and_zero_64( kaapi_bitmap_value64_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( b->proc64 );
  if (fb ==0) return 0;
  b->proc64 &= ~( ((uint64_t)1) << (fb-1) );
  return fb;
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero_64( kaapi_bitmap64_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&b->proc64) );
  if (fb ==0) return 0;
  if (kaapi_bitmap_unset_64(b, fb-1) ==0)
    return fb;
  return 0;
}

static inline int kaapi_bitmap_value_first1_64( const kaapi_bitmap_value64_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  return __builtin_ffsl( b->proc64 );
}

static inline int kaapi_bitmap_first1_64( const kaapi_bitmap64_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  return __builtin_ffsl( b->proc64._counter );
}

static inline void kaapi_bitmap_value_set_low_bits_64( kaapi_bitmap_value64_t* b, unsigned int i)
{
  kaapi_assert_debug(i <= 64);
  if (i == 64)
    b->proc64 = ~(uint64_t)0;
  else
    b->proc64 = ((uint64_t)1 << i) - (uint64_t)1;
}

/* set all the [0, i[ bits 
*/
static inline void kaapi_bitmap_set_low_bits_64( kaapi_bitmap64_t* b, unsigned int i)
{
  kaapi_assert_debug(i < 64);
  KAAPI_ATOMIC_WRITE(&b->proc64, ((uint64_t)1 << i) - (uint64_t)1);
}


/******* 128 */
static inline void kaapi_bitmap_clear_128( kaapi_bitmap128_t* b ) 
{ KAAPI_ATOMIC_WRITE( &(b->proc128)[0], 0); KAAPI_ATOMIC_WRITE( &(b->proc128)[1], 0); }

static inline int kaapi_bitmap_empty_128( kaapi_bitmap128_t* b )
{ 
  return (KAAPI_ATOMIC_READ(&b->proc128[0]) ==0) && (KAAPI_ATOMIC_READ(&b->proc128[1]) ==0);
}

static inline int kaapi_bitmap_init_128( 
    kaapi_bitmap128_t* b,
    const kaapi_bitmap_value128_t* v 
)
{ 
  KAAPI_ATOMIC_WRITE(&b->proc128[0], v->proc128[0]);
  KAAPI_ATOMIC_WRITE(&b->proc128[1], v->proc128[1]);
  return 0;
}


static inline void kaapi_bitmap_value_clear_128( kaapi_bitmap_value128_t* b )
{ 
  b->proc128[0] =0;
  b->proc128[1] =0;
}

static inline void kaapi_bitmap_value_full_128( kaapi_bitmap_value128_t* b )
{ 
  b->proc128[0] =~(uint64_t)0;
  b->proc128[1] =~(uint64_t)0;
}

static inline int kaapi_bitmap_value_count_128
( const kaapi_bitmap_value128_t* b )
{ 
  return 
     __builtin_popcountl(b->proc128[0]) +
     __builtin_popcountl(b->proc128[1])
  ;
}

static inline int kaapi_bitmap_value_empty_128( const kaapi_bitmap_value128_t* b )
{ 
  return ((b->proc128)[0] ==0) && ((b->proc128)[1] ==0);
}

static inline void kaapi_bitmap_value_set_128( kaapi_bitmap_value128_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<128) && (i>=0) );
  if (i<64)
    (b->proc128)[0] |= ((uint64_t)1)<< i; 
  else
    (b->proc128)[1] |= ((uint64_t)1)<< (i-64); 
}

static inline int kaapi_bitmap_value_get_128( const kaapi_bitmap_value128_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<128) && (i>=0) );
  if (i<64)
    return ((b->proc128)[0] & ((uint64_t)1)<< i) !=0; 
  else
    return ((b->proc128)[1] & ((uint64_t)1)<< (i-64)) !=0; 
}

static inline void kaapi_bitmap_value_unset_128( kaapi_bitmap_value128_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<128) && (i>=0) );
  if (i<64)
    (b->proc128)[0] &= ~(((uint64_t)1)<< i); 
  else
    (b->proc128)[1] &= ~(((uint64_t)1)<< (i-64)); 
}

static inline void kaapi_bitmap_value_copy_128( kaapi_bitmap_value128_t* retval, const kaapi_bitmap_value128_t* b ) 
{ 
  (retval->proc128)[0] = (b->proc128)[0];
  (retval->proc128)[1] = (b->proc128)[1];
}

static inline void kaapi_bitmap_swap0_128( kaapi_bitmap128_t* b, kaapi_bitmap_value128_t* v ) 
{ 
  (v->proc128)[0] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc128)[0], (uint64_t)0); 
  (v->proc128)[1] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc128)[1], (uint64_t)0); 
}

static inline void kaapi_bitmap_or0_128( kaapi_bitmap128_t* b, kaapi_bitmap_value128_t* v ) 
{ 
  (v->proc128)[0] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc128)[0], (uint64_t)0); 
  (v->proc128)[1] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc128)[1], (uint64_t)0); 
}

static inline void kaapi_bitmap_and_128
(
 kaapi_bitmap_value128_t* fu,
 kaapi_bitmap128_t* b,
 kaapi_bitmap_value128_t* v
) 
{
  fu->proc128[0] = KAAPI_ATOMIC_AND_ORIG(&b->proc128[0], v->proc128[0]);
  fu->proc128[1] = KAAPI_ATOMIC_AND_ORIG(&b->proc128[1], v->proc128[1]);
}

static inline void kaapi_bitmap_value_neg_128
( kaapi_bitmap_value128_t* fu, const kaapi_bitmap_value128_t* bar ) 
{
  fu->proc128[0] = ~bar->proc128[0];
  fu->proc128[1] = ~bar->proc128[1];
}

static inline void kaapi_bitmap_value_or_128
( kaapi_bitmap_value128_t* fu, const kaapi_bitmap_value128_t* bar ) 
{
  fu->proc128[0] |= bar->proc128[0];
  fu->proc128[1] |= bar->proc128[1];
}

static inline void kaapi_bitmap_value_and_128
( kaapi_bitmap_value128_t* fu, const kaapi_bitmap_value128_t* bar ) 
{
  fu->proc128[0] &= bar->proc128[0];
  fu->proc128[1] &= bar->proc128[1];
}

static inline int kaapi_bitmap_set_128( kaapi_bitmap128_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<128) && (i>=0) );
  if (i<64)
    KAAPI_ATOMIC_OR64( &(b->proc128)[0], ((uint64_t)1)<< i); 
  else
    KAAPI_ATOMIC_OR64( &(b->proc128)[1], ((uint64_t)1)<< (i-64)); 
  return 1;
}

static inline int kaapi_bitmap_get_128( const kaapi_bitmap128_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<128) && (i>=0) );
  return kaapi_bitmap_value_get_128((const kaapi_bitmap_value128_t*)b, i);
}

static inline int kaapi_bitmap_unset_128( kaapi_bitmap128_t* b, int i ) 
{ 
  uint64_t x;

  kaapi_assert_debug( (i<128) && (i>=0) );
  if (i<64)
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc128)[0], ~(((uint64_t)1)<< i)); 
    if (x & ((uint64_t)1 << i)) return 0;
  }
  else
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc128)[1], ~(((uint64_t)1)<< (i-64))); 
    if (x & ((uint64_t)1 << (i - 64))) return 0;
  }

  return -1;
}

static inline int kaapi_bitmap_count_128( const kaapi_bitmap128_t* b ) 
{ return __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc128[0])) + 
         __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc128[1])) ; }

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_value_first1_and_zero_128( kaapi_bitmap_value128_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( (b->proc128)[0] );
  if (fb !=0) {
    b->proc128[0] &= ~( ((uint64_t)1) << (fb-1) );
    return fb;
  }
  fb = __builtin_ffsl( (b->proc128)[1] );
  if (fb ==0) return 0;
  (b->proc128)[1] &= ~( ((uint64_t)1) << (fb-1) );
  return 64+fb;
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero_128( kaapi_bitmap128_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc128)[0]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc128[0], fb-1) ==0)
      return fb;
    return 0;
  }
  fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc128)[1]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc128[1], fb-1) ==0)
      return 64+fb;
  }
  return 0;
}

static inline int kaapi_bitmap_value_first1_128( const kaapi_bitmap_value128_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  if (b->proc128[0] != 0)
    return __builtin_ffsl( (b->proc128)[0] );

  if (b->proc128[1] == 0) 
    return 0;
  return 64+ __builtin_ffsl( (b->proc128)[1] );
}

static inline int kaapi_bitmap_first1_128( const kaapi_bitmap128_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  if (b->proc128[0]._counter != 0)
    return __builtin_ffsl( (b->proc128)[0]._counter );

  if (b->proc128[1]._counter == 0) 
    return 0;
  return 64+ __builtin_ffsl( (b->proc128)[1]._counter );
}

static inline void kaapi_bitmap_value_set_low_bits_128( kaapi_bitmap_value128_t* b, unsigned int i)
{
  kaapi_assert_debug(i <= 128);
  if (i < 64)
  {
    b->proc128[0] = ((uint64_t)1 << i) - (uint64_t)1;
    b->proc128[1] = 0;
    return;
  }

  b->proc128[0] = ~(uint64_t)0;

  if (i == 128)
    b->proc128[1] = ~(uint64_t)0;
  else
    b->proc128[1] = ((uint64_t)1 << (i-64)) - (uint64_t)1;
}

static inline void kaapi_bitmap_set_low_bits_128( kaapi_bitmap128_t* b, unsigned int i)
{
  if (i < 64)
  {
    KAAPI_ATOMIC_WRITE(&b->proc128[0], ((uint64_t)1 << i) - (uint64_t)1);
    return;
  }

  KAAPI_ATOMIC_WRITE(&b->proc128[0], ~(uint64_t)0);

  kaapi_assert_debug(i < 128);
  KAAPI_ATOMIC_WRITE(&b->proc128[1], ((uint64_t)1 << (i-64)) - (uint64_t)1 );
}


/******* 192 */
static inline void kaapi_bitmap_clear_192( kaapi_bitmap192_t* b ) 
{ KAAPI_ATOMIC_WRITE( &(b->proc192)[0], 0); KAAPI_ATOMIC_WRITE( &(b->proc192)[1], 0);
  KAAPI_ATOMIC_WRITE( &(b->proc192)[2], 0);
}

static inline int kaapi_bitmap_empty_192( kaapi_bitmap192_t* b )
{ 
  return (KAAPI_ATOMIC_READ(&b->proc192[0]) ==0) && (KAAPI_ATOMIC_READ(&b->proc192[1]) ==0) && (KAAPI_ATOMIC_READ(&b->proc192[2]) ==0);
}

static inline int kaapi_bitmap_init_192( 
    kaapi_bitmap192_t* b,
    const kaapi_bitmap_value192_t* v 
)
{ 
  KAAPI_ATOMIC_WRITE(&b->proc192[0], v->proc192[0]);
  KAAPI_ATOMIC_WRITE(&b->proc192[1], v->proc192[1]);
  KAAPI_ATOMIC_WRITE(&b->proc192[2], v->proc192[2]);
  return 0;
}


static inline void kaapi_bitmap_value_clear_192( kaapi_bitmap_value192_t* b )
{ 
  b->proc192[0] =0;
  b->proc192[1] =0;
  b->proc192[2] =0;
}

static inline void kaapi_bitmap_value_full_192( kaapi_bitmap_value192_t* b )
{ 
  b->proc192[0] =~(uint64_t)0;
  b->proc192[1] =~(uint64_t)0;
  b->proc192[2] =~(uint64_t)0;
}

static inline int kaapi_bitmap_value_count_192
( const kaapi_bitmap_value192_t* b )
{ 
  return 
     __builtin_popcountl(b->proc192[0]) +
     __builtin_popcountl(b->proc192[1]) +
     __builtin_popcountl(b->proc192[2])
  ;
}

static inline int kaapi_bitmap_value_empty_192( const kaapi_bitmap_value192_t* b )
{ 
  return ((b->proc192)[0] ==0) && ((b->proc192)[1] ==0) && ((b->proc192)[2] ==0);
}

static inline void kaapi_bitmap_value_set_192( kaapi_bitmap_value192_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<192) && (i>=0) );
  if (i<64)
    (b->proc192)[0] |= ((uint64_t)1)<< i; 
  else if (i<128)
    (b->proc192)[1] |= ((uint64_t)1)<< (i-64); 
  else
    (b->proc192)[2] |= ((uint64_t)1)<< (i-128); 
}

static inline int kaapi_bitmap_value_get_192( const kaapi_bitmap_value192_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<192) && (i>=0) );
  if (i<64)
    return ((b->proc192)[0] & ((uint64_t)1)<< i) !=0; 
  else if (i<128)
    return ((b->proc192)[1] & ((uint64_t)1)<< (i-64)) !=0; 
  else
    return ((b->proc192)[2] & ((uint64_t)1)<< (i-128)) !=0; 
}

static inline void kaapi_bitmap_value_unset_192( kaapi_bitmap_value192_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<192) && (i>=0) );
  if (i<64)
    (b->proc192)[0] &= ~(((uint64_t)1)<< i); 
  else if (i<128)
    (b->proc192)[1] &= ~(((uint64_t)1)<< (i-64)); 
  else
    (b->proc192)[1] &= ~(((uint64_t)1)<< (i-128)); 
}

static inline void kaapi_bitmap_value_copy_192( kaapi_bitmap_value192_t* retval, const kaapi_bitmap_value192_t* b ) 
{ 
  (retval->proc192)[0] = (b->proc192)[0];
  (retval->proc192)[1] = (b->proc192)[1];
  (retval->proc192)[2] = (b->proc192)[2];
}

static inline void kaapi_bitmap_swap0_192( kaapi_bitmap192_t* b, kaapi_bitmap_value192_t* v ) 
{ 
  (v->proc192)[0] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[0], (uint64_t)0); 
  (v->proc192)[1] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[1], (uint64_t)0); 
  (v->proc192)[2] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[2], (uint64_t)0); 
}

static inline void kaapi_bitmap_or0_192( kaapi_bitmap192_t* b, kaapi_bitmap_value192_t* v ) 
{ 
  (v->proc192)[0] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[0], (uint64_t)0); 
  (v->proc192)[1] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[1], (uint64_t)0); 
  (v->proc192)[2] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[2], (uint64_t)0); 
}

static inline void kaapi_bitmap_and_192
(
    kaapi_bitmap_value192_t* fu,
    kaapi_bitmap192_t* b,
    kaapi_bitmap_value192_t* v
) 
{
  fu->proc192[0] = KAAPI_ATOMIC_AND_ORIG(&b->proc192[0], v->proc192[0]);
  fu->proc192[1] = KAAPI_ATOMIC_AND_ORIG(&b->proc192[1], v->proc192[1]);
  fu->proc192[2] = KAAPI_ATOMIC_AND_ORIG(&b->proc192[2], v->proc192[2]);
}

static inline void kaapi_bitmap_value_neg_192
( kaapi_bitmap_value192_t* fu, const kaapi_bitmap_value192_t* bar ) 
{
  fu->proc192[0] = ~bar->proc192[0];
  fu->proc192[1] = ~bar->proc192[1];
  fu->proc192[2] = ~bar->proc192[2];
}

static inline void kaapi_bitmap_value_or_192
( kaapi_bitmap_value192_t* fu, const kaapi_bitmap_value192_t* bar ) 
{
  fu->proc192[0] |= bar->proc192[0];
  fu->proc192[1] |= bar->proc192[1];
  fu->proc192[2] |= bar->proc192[2];
}

static inline void kaapi_bitmap_value_and_192
( kaapi_bitmap_value192_t* fu, const kaapi_bitmap_value192_t* bar ) 
{
  fu->proc192[0] &= bar->proc192[0];
  fu->proc192[1] &= bar->proc192[1];
  fu->proc192[2] &= bar->proc192[2];
}

static inline int kaapi_bitmap_set_192( kaapi_bitmap192_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<192) && (i>=0) );
  if (i<64)
    KAAPI_ATOMIC_OR64( &(b->proc192)[0], ((uint64_t)1)<< i); 
  else if (i<128)
    KAAPI_ATOMIC_OR64( &(b->proc192)[1], ((uint64_t)1)<< (i-64)); 
  else
    KAAPI_ATOMIC_OR64( &(b->proc192)[2], ((uint64_t)1)<< (i-128)); 
  return 1;
}

static inline int kaapi_bitmap_get_192( const kaapi_bitmap192_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<192) && (i>=0) );
  return kaapi_bitmap_value_get_192((const kaapi_bitmap_value192_t*)b, i);
}

static inline int kaapi_bitmap_unset_192( kaapi_bitmap192_t* b, int i ) 
{ 
  uint64_t x;

  kaapi_assert_debug( (i<192) && (i>=0) );
  if (i<64)
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[0], ~(((uint64_t)1)<< i)); 
    if (x & ((uint64_t)1 << i)) return 0;
  }
  else if (i<128)
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[1], ~(((uint64_t)1)<< (i-64))); 
    if (x & ((uint64_t)1 << (i - 64))) return 0;
  }
  else
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc192)[2], ~(((uint64_t)1)<< (i-128))); 
    if (x & ((uint64_t)1 << (i - 128))) return 0;
  }

  return -1;
}

static inline int kaapi_bitmap_count_192( const kaapi_bitmap192_t* b ) 
{ return __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc192[0])) +
         __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc192[1])) + 
         __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc192[2])) ; 
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_value_first1_and_zero_192( kaapi_bitmap_value192_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( (b->proc192)[0] );
  if (fb !=0) {
    b->proc192[0] &= ~( ((uint64_t)1) << (fb-1) );
    return fb;
  }
  fb = __builtin_ffsl( (b->proc192)[1] );
  if (fb !=0) {
    b->proc192[1] &= ~( ((uint64_t)1) << (fb-1) );
    return 64+fb;
  }
  fb = __builtin_ffsl( (b->proc192)[2] );
  if (fb ==0) return 0;
  (b->proc192)[2] &= ~( ((uint64_t)1) << (fb-1) );
  return 128+fb;
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero_192( kaapi_bitmap192_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc192)[0]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc192[0], fb-1) ==0)
      return fb;
    return 0;
  }
  fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc192)[1]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc192[1], fb-1) ==0)
      return 64+fb;
    return 0;
  }
  fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc192)[2]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc192[2], fb-1) ==0)
      return 128+fb;
  }
  return 0;
}

static inline int kaapi_bitmap_value_first1_192( const kaapi_bitmap_value192_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  if (b->proc192[0] != 0)
    return __builtin_ffsl( (b->proc192)[0] );

  if (b->proc192[1] != 0)
    return 64+__builtin_ffsl( (b->proc192)[1] );

  if (b->proc192[2] == 0) 
    return 0;
  return 128+ __builtin_ffsl( (b->proc192)[2] );
}

static inline int kaapi_bitmap_first1_192( const kaapi_bitmap192_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  if (b->proc192[0]._counter != 0)
    return __builtin_ffsl( (b->proc192)[0]._counter );

  if (b->proc192[1]._counter != 0)
    return 64+__builtin_ffsl( (b->proc192)[1]._counter );

  if (b->proc192[2]._counter == 0) 
    return 0;
  return 128+ __builtin_ffsl( (b->proc192)[2]._counter );
}

static inline void kaapi_bitmap_value_set_low_bits_192( kaapi_bitmap_value192_t* b, unsigned int i)
{
  kaapi_assert_debug(i <= 192);
  if (i < 64)
  {
    b->proc192[0] = ((uint64_t)1 << i) - (uint64_t)1;
    b->proc192[1] = 0;
    b->proc192[2] = 0;
    return;
  }
  b->proc192[0]   = ~(uint64_t)0;

  if (i < 128)
  {
    b->proc192[1] = ((uint64_t)1 << (i-64)) - (uint64_t)1;
    b->proc192[2] = 0;
    return;
  }
  b->proc192[1]   = ~(uint64_t)0;

  if (i == 192)
    b->proc192[2] = ~(uint64_t)0;
  else
    b->proc192[2] = ((uint64_t)1 << (i-128)) - (uint64_t)1;
}

static inline void kaapi_bitmap_set_low_bits_192( kaapi_bitmap192_t* b, unsigned int i)
{
  if (i < 64)
  {
    KAAPI_ATOMIC_WRITE(&b->proc192[0], ((uint64_t)1 << i) - (uint64_t)1);
    KAAPI_ATOMIC_WRITE(&b->proc192[1], 0);
    KAAPI_ATOMIC_WRITE(&b->proc192[2], 0);
    return;
  }
  KAAPI_ATOMIC_WRITE(&b->proc192[0], ~(uint64_t)0);

  if (i < 128)
  {
    KAAPI_ATOMIC_WRITE(&b->proc192[1], ((uint64_t)1 << (i-64)) - (uint64_t)1);
    KAAPI_ATOMIC_WRITE(&b->proc192[2], 0);
    return;
  }
  KAAPI_ATOMIC_WRITE(&b->proc192[1], ~(uint64_t)0);

  kaapi_assert_debug(i < 192);
  KAAPI_ATOMIC_WRITE(&b->proc192[2], ((uint64_t)1 << (i-128)) - (uint64_t)1 );
}



/******* 256 */
static inline void kaapi_bitmap_clear_256( kaapi_bitmap256_t* b ) 
{ KAAPI_ATOMIC_WRITE( &(b->proc256)[0], 0); KAAPI_ATOMIC_WRITE( &(b->proc256)[1], 0);
  KAAPI_ATOMIC_WRITE( &(b->proc256)[2], 0); KAAPI_ATOMIC_WRITE( &(b->proc256)[3], 0);
}

static inline int kaapi_bitmap_empty_256( kaapi_bitmap256_t* b )
{ 
  return (KAAPI_ATOMIC_READ(&b->proc256[0]) ==0) 
      && (KAAPI_ATOMIC_READ(&b->proc256[1]) ==0) 
      && (KAAPI_ATOMIC_READ(&b->proc256[2]) ==0)
      && (KAAPI_ATOMIC_READ(&b->proc256[3]) ==0);
}

static inline int kaapi_bitmap_init_256( 
    kaapi_bitmap256_t* b,
    const kaapi_bitmap_value256_t* v 
)
{ 
  KAAPI_ATOMIC_WRITE(&b->proc256[0], v->proc256[0]);
  KAAPI_ATOMIC_WRITE(&b->proc256[1], v->proc256[1]);
  KAAPI_ATOMIC_WRITE(&b->proc256[2], v->proc256[2]);
  KAAPI_ATOMIC_WRITE(&b->proc256[3], v->proc256[3]);
  return 0;
}


static inline void kaapi_bitmap_value_clear_256( kaapi_bitmap_value256_t* b )
{ 
  b->proc256[0] =0;
  b->proc256[1] =0;
  b->proc256[2] =0;
  b->proc256[3] =0;
}

static inline void kaapi_bitmap_value_full_256( kaapi_bitmap_value256_t* b )
{ 
  b->proc256[0] =~(uint64_t)0;
  b->proc256[1] =~(uint64_t)0;
  b->proc256[2] =~(uint64_t)0;
  b->proc256[3] =~(uint64_t)0;
}

static inline int kaapi_bitmap_value_count_256
( const kaapi_bitmap_value256_t* b )
{ 
  return 
     __builtin_popcountl(b->proc256[0]) +
     __builtin_popcountl(b->proc256[1]) +
     __builtin_popcountl(b->proc256[2]) +
     __builtin_popcountl(b->proc256[3])
  ;
}

static inline int kaapi_bitmap_value_empty_256( const kaapi_bitmap_value256_t* b )
{ 
  return ((b->proc256)[0] ==0) && ((b->proc256)[1] ==0) && ((b->proc256)[2] ==0) && ((b->proc256)[3] ==0);
}

static inline void kaapi_bitmap_value_set_256( kaapi_bitmap_value256_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<256) && (i>=0) );
  if (i<64)
    (b->proc256)[0] |= ((uint64_t)1)<< i; 
  else if (i<128)
    (b->proc256)[1] |= ((uint64_t)1)<< (i-64); 
  else if (i<192)
    (b->proc256)[2] |= ((uint64_t)1)<< (i-128); 
  else
    (b->proc256)[3] |= ((uint64_t)1)<< (i-192); 
}

static inline int kaapi_bitmap_value_get_256( const kaapi_bitmap_value256_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<256) && (i>=0) );
  if (i<64)
    return ((b->proc256)[0] & ((uint64_t)1)<< i) !=0; 
  else if (i<128)
    return ((b->proc256)[1] & ((uint64_t)1)<< (i-64)) !=0; 
  else if (i<192)
    return ((b->proc256)[2] & ((uint64_t)1)<< (i-128)) !=0; 
  else
    return ((b->proc256)[3] & ((uint64_t)1)<< (i-192)) !=0; 
}

static inline void kaapi_bitmap_value_unset_256( kaapi_bitmap_value256_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<256) && (i>=0) );
  if (i<64)
    (b->proc256)[0] &= ~(((uint64_t)1)<< i); 
  else if (i<128)
    (b->proc256)[1] &= ~(((uint64_t)1)<< (i-64)); 
  else if (i<192)
    (b->proc256)[2] &= ~(((uint64_t)1)<< (i-128)); 
  else
    (b->proc256)[3] &= ~(((uint64_t)1)<< (i-192)); 
}

static inline void kaapi_bitmap_value_copy_256( kaapi_bitmap_value256_t* retval, const kaapi_bitmap_value256_t* b ) 
{ 
  (retval->proc256)[0] = (b->proc256)[0];
  (retval->proc256)[1] = (b->proc256)[1];
  (retval->proc256)[2] = (b->proc256)[2];
  (retval->proc256)[3] = (b->proc256)[3];
}

static inline void kaapi_bitmap_swap0_256( kaapi_bitmap256_t* b, kaapi_bitmap_value256_t* v ) 
{ 
  (v->proc256)[0] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[0], (uint64_t)0); 
  (v->proc256)[1] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[1], (uint64_t)0); 
  (v->proc256)[2] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[2], (uint64_t)0); 
  (v->proc256)[3] = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[3], (uint64_t)0); 
}

static inline void kaapi_bitmap_or0_256( kaapi_bitmap256_t* b, kaapi_bitmap_value256_t* v ) 
{ 
  (v->proc256)[0] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[0], (uint64_t)0); 
  (v->proc256)[1] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[1], (uint64_t)0); 
  (v->proc256)[2] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[2], (uint64_t)0); 
  (v->proc256)[3] |= KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[3], (uint64_t)0); 
}

static inline void kaapi_bitmap_and_256
(
    kaapi_bitmap_value256_t* fu,
    kaapi_bitmap256_t* b,
    kaapi_bitmap_value256_t* v
) 
{
  fu->proc256[0] = KAAPI_ATOMIC_AND_ORIG(&b->proc256[0], v->proc256[0]);
  fu->proc256[1] = KAAPI_ATOMIC_AND_ORIG(&b->proc256[1], v->proc256[1]);
  fu->proc256[2] = KAAPI_ATOMIC_AND_ORIG(&b->proc256[2], v->proc256[2]);
  fu->proc256[3] = KAAPI_ATOMIC_AND_ORIG(&b->proc256[3], v->proc256[3]);
}

static inline void kaapi_bitmap_value_neg_256
( kaapi_bitmap_value256_t* fu, const kaapi_bitmap_value256_t* bar ) 
{
  fu->proc256[0] = ~bar->proc256[0];
  fu->proc256[1] = ~bar->proc256[1];
  fu->proc256[2] = ~bar->proc256[2];
  fu->proc256[3] = ~bar->proc256[3];
}

static inline void kaapi_bitmap_value_or_256
( kaapi_bitmap_value256_t* fu, const kaapi_bitmap_value256_t* bar ) 
{
  fu->proc256[0] |= bar->proc256[0];
  fu->proc256[1] |= bar->proc256[1];
  fu->proc256[2] |= bar->proc256[2];
  fu->proc256[3] |= bar->proc256[3];
}

static inline void kaapi_bitmap_value_and_256
( kaapi_bitmap_value256_t* fu, const kaapi_bitmap_value256_t* bar ) 
{
  fu->proc256[0] &= bar->proc256[0];
  fu->proc256[1] &= bar->proc256[1];
  fu->proc256[2] &= bar->proc256[2];
  fu->proc256[3] &= bar->proc256[3];
}

static inline int kaapi_bitmap_set_256( kaapi_bitmap256_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<256) && (i>=0) );
  if (i<64)
    KAAPI_ATOMIC_OR64( &(b->proc256)[0], ((uint64_t)1)<< i); 
  else if (i<128)
    KAAPI_ATOMIC_OR64( &(b->proc256)[1], ((uint64_t)1)<< (i-64)); 
  else if (i<192)
    KAAPI_ATOMIC_OR64( &(b->proc256)[2], ((uint64_t)1)<< (i-128)); 
  else
    KAAPI_ATOMIC_OR64( &(b->proc256)[3], ((uint64_t)1)<< (i-192)); 
  return 1;
}

static inline int kaapi_bitmap_get_256( const kaapi_bitmap256_t* b, int i ) 
{ 
  kaapi_assert_debug( (i<256) && (i>=0) );
  return kaapi_bitmap_value_get_256((const kaapi_bitmap_value256_t*)b, i);
}

static inline int kaapi_bitmap_unset_256( kaapi_bitmap256_t* b, int i ) 
{ 
  uint64_t x;

  kaapi_assert_debug( (i<256) && (i>=0) );
  if (i<64)
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[0], ~(((uint64_t)1)<< i)); 
    if (x & ((uint64_t)1 << i)) return 0;
  }
  else if (i<128)
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[1], ~(((uint64_t)1)<< (i-64))); 
    if (x & ((uint64_t)1 << (i - 64))) return 0;
  }
  else if (i<192)
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[2], ~(((uint64_t)1)<< (i-128))); 
    if (x & ((uint64_t)1 << (i - 128))) return 0;
  }
  else
  {
    x = KAAPI_ATOMIC_AND64_ORIG( &(b->proc256)[3], ~(((uint64_t)1)<< (i-192))); 
    if (x & ((uint64_t)1 << (i - 192))) return 0;
  }
  return -1;
}

static inline int kaapi_bitmap_count_256( const kaapi_bitmap256_t* b ) 
{ return __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc256[0])) +
         __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc256[1])) + 
         __builtin_popcountl( KAAPI_ATOMIC_READ(&b->proc256[2])) ; 
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_value_first1_and_zero_256( kaapi_bitmap_value256_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( (b->proc256)[0] );
  if (fb !=0) {
    b->proc256[0] &= ~( ((uint64_t)1) << (fb-1) );
    return fb;
  }

  fb = __builtin_ffsl( (b->proc256)[1] );
  if (fb !=0) {
    b->proc256[1] &= ~( ((uint64_t)1) << (fb-1) );
    return 64+fb;
  }

  fb = __builtin_ffsl( (b->proc256)[2] );
  if (fb !=0) {
    b->proc256[2] &= ~( ((uint64_t)1) << (fb-1) );
    return 128+fb;
  }

  fb = __builtin_ffsl( (b->proc256)[3] );
  if (fb ==0) return 0;
  (b->proc256)[3] &= ~( ((uint64_t)1) << (fb-1) );
  return 192+fb;
}

/* Return the 1+index of the least significant bit set to 1.
   If the value is 0 return 0.
   Else return the number of trailing zero (from to least significant
   bit to the most significant bit). And set to 0 the bit.
*/
static inline int kaapi_bitmap_first1_and_zero_256( kaapi_bitmap256_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  int fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc256)[0]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc256[0], fb-1) ==0)
      return fb;
    return 0;
  }
  fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc256)[1]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc256[1], fb-1) ==0)
      return 64+fb;
    return 0;
  }
  fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc256)[2]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc256[2], fb-1) ==0)
      return 128+fb;
  }
  fb = __builtin_ffsl( KAAPI_ATOMIC_READ(&(b->proc256)[3]) );
  if (fb !=0) 
  {
    if (kaapi_bitmap_unset_64((kaapi_bitmap64_t*)&b->proc256[3], fb-1) ==0)
      return 192+fb;
  }
  return 0;
}

static inline int kaapi_bitmap_value_first1_256( const kaapi_bitmap_value256_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  if (b->proc256[0] != 0)
    return __builtin_ffsl( (b->proc256)[0] );

  if (b->proc256[1] != 0)
    return 64+__builtin_ffsl( (b->proc256)[1] );

  if (b->proc256[2] != 0) 
    return 128+ __builtin_ffsl( (b->proc256)[2] );

  if (b->proc256[3] == 0) 
    return 0;
  return 192+ __builtin_ffsl( (b->proc256)[3] );
}

static inline int kaapi_bitmap_first1_256( const kaapi_bitmap256_t* b )
{
  /* Note: for WIN32, to have a look at _BitScanForward */
  if (b->proc256[0]._counter != 0)
    return __builtin_ffsl( (b->proc256)[0]._counter );

  if (b->proc256[1]._counter != 0)
    return 64+__builtin_ffsl( (b->proc256)[1]._counter );

  if (b->proc256[2]._counter != 0) 
    return 128+ __builtin_ffsl( (b->proc256)[2]._counter );

  if (b->proc256[3]._counter == 0) 
    return 0;
  return 192+ __builtin_ffsl( (b->proc256)[3]._counter );
}

static inline void kaapi_bitmap_value_set_low_bits_256( kaapi_bitmap_value256_t* b, unsigned int i)
{
  kaapi_assert_debug(i <= 256);
  if (i < 64)
  {
    b->proc256[0] = ((uint64_t)1 << i) - (uint64_t)1;
    b->proc256[1] = 0;
    b->proc256[2] = 0;
    b->proc256[3] = 0;
    return;
  }
  b->proc256[0]   = ~(uint64_t)0;

  if (i < 128)
  {
    b->proc256[1] = ((uint64_t)1 << (i-64)) - (uint64_t)1;
    b->proc256[2] = 0;
    b->proc256[3] = 0;
    return;
  }
  b->proc256[1]   = ~(uint64_t)0;

  if (i < 192)
  {
    b->proc256[2] = ((uint64_t)1 << (i-128)) - (uint64_t)1;
    b->proc256[3] = 0;
    return;
  }
  b->proc256[2]   = ~(uint64_t)0;

  if (i == 256)
    b->proc256[3] = ~(uint64_t)0;
  else
    b->proc256[3] = ((uint64_t)1 << (i-192)) - (uint64_t)1;
}

static inline void kaapi_bitmap_set_low_bits_256( kaapi_bitmap256_t* b, unsigned int i)
{
  if (i < 64)
  {
    KAAPI_ATOMIC_WRITE(&b->proc256[0], ((uint64_t)1 << i) - (uint64_t)1);
    KAAPI_ATOMIC_WRITE(&b->proc256[1], 0);
    KAAPI_ATOMIC_WRITE(&b->proc256[2], 0);
    KAAPI_ATOMIC_WRITE(&b->proc256[3], 0);
    return;
  }
  KAAPI_ATOMIC_WRITE(&b->proc256[0], ~(uint64_t)0);

  if (i < 128)
  {
    KAAPI_ATOMIC_WRITE(&b->proc256[1], ((uint64_t)1 << (i-64)) - (uint64_t)1);
    KAAPI_ATOMIC_WRITE(&b->proc256[2], 0);
    KAAPI_ATOMIC_WRITE(&b->proc256[3], 0);
    return;
  }
  KAAPI_ATOMIC_WRITE(&b->proc256[1], ~(uint64_t)0);

  if (i < 192)
  {
    KAAPI_ATOMIC_WRITE(&b->proc256[2], ((uint64_t)1 << (i-128)) - (uint64_t)1);
    KAAPI_ATOMIC_WRITE(&b->proc256[3], 0);
    return;
  }
  KAAPI_ATOMIC_WRITE(&b->proc256[2], ~(uint64_t)0);

  kaapi_assert_debug(i < 256);
  KAAPI_ATOMIC_WRITE(&b->proc256[3], ((uint64_t)1 << (i-192)) - (uint64_t)1 );
}

#  ifdef KAAPI_MAX_PROCESSOR_LARGE
#    error "To be implemented"
#  endif




#  ifdef KAAPI_MAX_PROCESSOR_GENERIC
/* Global function pointers initialized in kaapi_hw_standardinit() */
extern void (*kaapi_bitmap_clear)( kaapi_bitmap_t* b );
extern void (*kaapi_bitmap_full)( kaapi_bitmap_t* b );
extern int (*kaapi_bitmap_empty)( kaapi_bitmap_t* b );
extern int (*kaapi_bitmap_value_empty)( const kaapi_bitmap_value_t* b );
extern unsigned long (*kaapi_bitmap_value_count)( const kaapi_bitmap_value_t* b );
extern void (*kaapi_bitmap_value_set)( kaapi_bitmap_value_t* b, int i );
extern void (*kaapi_bitmap_value_get)( const kaapi_bitmap_value_t* b, int i );
extern void (*kaapi_bitmap_value_unset)( kaapi_bitmap_value_t* b, int i );
extern void (*kaapi_bitmap_value_copy)( kaapi_bitmap_value_t* retval, kaapi_bitmap_value_t* b);
extern void (*kaapi_bitmap_swap0)( kaapi_bitmap_t* b, kaapi_bitmap_value_t* v );
extern void (*kaapi_bitmap_or0)( kaapi_bitmap_t* b, kaapi_bitmap_value_t* v );
extern void (*kaapi_bitmap_and)( kaapi_bitmap_value_t*, kaapi_bitmap_t* b, kaapi_bitmap_value_t* v );
extern void (*kaapi_bitmap_value_or)( kaapi_bitmap_value_t* b, const kaapi_bitmap_value_t* v ) ;
extern void (*kaapi_bitmap_value_and)( kaapi_bitmap_value_t* b, const kaapi_bitmap_value_t* v ) ;
extern void (*kaapi_bitmap_value_neg)( kaapi_bitmap_value_t* b, const kaapi_bitmap_value_t* v ) ;
extern int (*kaapi_bitmap_set)( kaapi_bitmap_t* b, int i );
extern int (*kaapi_bitmap_get)( const kaapi_bitmap_t* b, int i );
extern int (*kaapi_bitmap_unset)( kaapi_bitmap_t* b, int i );
extern int (*kaapi_bitmap_count)( const kaapi_bitmap_t* b );
extern int (*kaapi_bitmap_value_first1_and_zero)( kaapi_bitmap_value_t* b );
#  else
#    if defined(KAAPI_MAX_PROCESSOR_32)
#      define KAAPI_MAX_PROCESSOR_SUFFIX(f) f##_32
typedef kaapi_bitmap32_t kaapi_bitmap_t;
typedef kaapi_bitmap_value32_t kaapi_bitmap_value_t;
#    elif defined(KAAPI_MAX_PROCESSOR_64)
typedef kaapi_bitmap64_t kaapi_bitmap_t;
typedef kaapi_bitmap_value64_t kaapi_bitmap_value_t;
#      define KAAPI_MAX_PROCESSOR_SUFFIX(f) f##_64
#    elif defined(KAAPI_MAX_PROCESSOR_128)
typedef kaapi_bitmap128_t kaapi_bitmap_t;
typedef kaapi_bitmap_value128_t kaapi_bitmap_value_t;
#      define KAAPI_MAX_PROCESSOR_SUFFIX(f) f##_128
#    elif defined(KAAPI_MAX_PROCESSOR_192)
typedef kaapi_bitmap192_t kaapi_bitmap_t;
typedef kaapi_bitmap_value192_t kaapi_bitmap_value_t;
#      define KAAPI_MAX_PROCESSOR_SUFFIX(f) f##_192
#    elif defined(KAAPI_MAX_PROCESSOR_256)
typedef kaapi_bitmap256_t kaapi_bitmap_t;
typedef kaapi_bitmap_value256_t kaapi_bitmap_value_t;
#      define KAAPI_MAX_PROCESSOR_SUFFIX(f) f##_256
#    elif defined(KAAPI_MAX_PROCESSOR_LARGE)
#      define KAAPI_MAX_PROCESSOR_SUFFIX(f) f##_large
#    else
#      error No implementation available
#    endif
#    define kaapi_bitmap_clear(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_clear)(b)
#    define kaapi_bitmap_empty(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_empty)(b)
#    define kaapi_bitmap_init(b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_init)(b,v)
#    define kaapi_bitmap_value_clear(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_clear)(b)
#    define kaapi_bitmap_value_full(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_full)(b)
#    define kaapi_bitmap_value_count(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_count)(b)
#    define kaapi_bitmap_value_empty(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_empty)(b)
#    define kaapi_bitmap_value_set(b,i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_set)((b),(i))
#    define kaapi_bitmap_value_get(b,i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_get)((b),(i))
#    define kaapi_bitmap_value_unset(b,i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_unset)((b),(i))
#    define kaapi_bitmap_value_copy(r,b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_copy)((r),(b))
#    define kaapi_bitmap_swap0(b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_swap0)((b),(v))
#    define kaapi_bitmap_or0(b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_or0)((b),(v))
#    define kaapi_bitmap_and(fu,b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_and)((fu), (b),(v))
#    define kaapi_bitmap_value_or(b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_or)((b),(v))
#    define kaapi_bitmap_value_and(b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_and)((b),(v))
#    define kaapi_bitmap_value_neg(b,v) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_neg)((b),(v))
#    define kaapi_bitmap_set(b, i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_set)((b), (i))
#    define kaapi_bitmap_get(b, i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_get)((b), (i))
#    define kaapi_bitmap_unset(b, i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_unset)((b), (i))
#    define kaapi_bitmap_count(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_count)(b)
#    define kaapi_bitmap_value_first1_and_zero(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_first1_and_zero)(b)
#    define kaapi_bitmap_first1_and_zero(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_first1_and_zero)(b)
#    define kaapi_bitmap_value_first1(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_first1)(b)
#    define kaapi_bitmap_first1(b) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_first1)(b)
#    define kaapi_bitmap_value_set_low_bits(b,i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_value_set_low_bits)(b,i)
#    define kaapi_bitmap_set_low_bits(b,i) KAAPI_MAX_PROCESSOR_SUFFIX(kaapi_bitmap_set_low_bits)(b,i)
#  endif

#endif /* _KAAPI_MT_BITMAP_H_ */
