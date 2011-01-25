/*
** kaapi_data.h
** xkaapi
** 
** Copyright 2010 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** fabien.lementec@imag.fr
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
#ifndef _KAAPI_MEMORY_H_
#define _KAAPI_MEMORY_H_ 1
#include <stdint.h>

/** Type of pointer for all address spaces.
    Pointer arithmetic is allowed on this type for some architecture (e.g.: remote pointer
    of a remote unix process; remote pointer as a device pointer with cuda).
    OpenCL seems not to support arithmetic pointer.
*/
typedef uintptr_t kaapi_pointer_t;


#if 0 /* definition moved into kaapi.h for public interface */
/** Type of allowed memory view for the memory interface:
    - 1D array (base, size)
      simple contiguous 1D array
    - 2D array (base, size[2], lda)
      assume a row major storage of the memory : the 2D array has
      size[0] rows of size[1] rowwidth. lda is used to pass from
      one row to the next one.
    The base (kaapi_pointer_t) is not part of the view description
*/
#define KAAPI_MEM_VIEW_1D 1
#define KAAPI_MEM_VIEW_2D 2  /* assume row major */
#define KAAPI_MEM_VIEW_3D 3
typedef struct kaapi_memory_view_t {
  int    type;
  size_t size[2];
  size_t lda;
} kaapi_memory_view_t;
#endif

/** Clear the view
*/
static inline void kaapi_memory_view_clear( kaapi_memory_view_t* kmv )
{
  kmv->type = -1;
#if defined(KAAPI_DEBUG)
  kmv->size[0] = kmv->size[0] = 0;
  kmv->lda = 0;
#endif
}

/** return the size of the view
*/
static inline size_t kaapi_memory_view_size( kaapi_memory_view_t* kmv )
{
  switch (kmv->type) 
  {
    case KAAPI_MEM_VIEW_1D: return kmv->size[0];
    case KAAPI_MEM_VIEW_2D: return kmv->size[0]*kmv->size[1];
    default:
      kaapi_assert(0);
      break;
  }
  return 0;
}


/** assume that now the view points to a new allocate view
*/
static inline void kaapi_memory_view_reallocated( kaapi_memory_view_t* kmv )
{
  switch (kmv->type) 
  {
    case KAAPI_MEM_VIEW_1D: return;
    case KAAPI_MEM_VIEW_2D: kmv->lda = kmv->size[1]; return;
    default:
      kaapi_assert(0);
      break;
  }
}

/** Return non negative value iff the view is contiguous
*/
static inline int kaapi_memory_view_iscontiguous( kaapi_memory_view_t* kmv )
{
  switch (kmv->type) {
    case KAAPI_MEM_VIEW_1D: return 1;
    case KAAPI_MEM_VIEW_2D: return  kmv->lda == kmv->size[1]; /* row major storage */
    default:
      break;
  } 
  return 0;
}


/** Meta data attached to a pointer and its remote copies.
    This data structure stores all the valid and invalid copies of the data.
    lptr[i].ptr is valid iff validbits & (1<<i) is not null.
    lptr[i].ptr is not valid but yet allocated if dirtybits & ( 1<<i) is not null.
    If lptr[i].ptr is 0, then no data has been allocated in the i-th local
    memory, and both validbits & (1<<i) and dirtybits & (1<<i) are nul.
    Value sticky & (1<<i) is not null iff the data of the address space could not be deallocated
    (for instance application level data).
    The size of each allocated block of data are stored in order to be able to reuse or not
    the data.
*/
typedef struct kaapi_metadata_info_t {
  uint64_t                validbits;
  uint64_t                dirtybits;
  uint64_t                stickybits;
  kaapi_pointer_t         ptr[1];
  size_t                  size[1];
} kaapi_metadata_info_t;


/** Represent the address space of a remote (or local) memory.
    This data structure is valid for both the CPU, remote CPU or GPU, or any kind of device.
*/
typedef unsigned int kaapi_address_space_t;

typedef struct kaapi_address_space_rep_t {
  kaapi_address_space_t asid;                                        /* address space identifier */
  uintptr_t            (*allocate)(size_t size);                     /* allocate memory */
  void                 (*deallocate)(uintptr_t, size_t size);        /* free allocated memory */
} kaapi_address_space_rep_t;



/** Global functions
*/

/** Allocate in the address space kasid a contiguous array of size size bytes.
    Initialize the kmdi meta data structure if the allocation success.
    Return the pointer in the address space 'kasid'
    If the allocation failed, then returns a null pointer and the kmdi is not updated.
    Some allocate on remote address space is not possible, without any support. For instance,
    remote allocation to a remote address space of a unix process is not allowed.
*/
extern kaapi_pointer_t kaapi_mem_allocate( kaapi_address_space_t kasid, kaapi_metadata_info_t* kmdi, size_t size );

/** Deallocate in the address space kasid the array referenced by kdi.
    If no data resides in klm, then do nothing.
    Else the data is free.
*/
extern void kaapi_mem_deallocate( kaapi_address_space_t kasid, kaapi_metadata_info_t* kdmi );

/** Bind an address of the calling virtual space of the process in the address space data structure kasid.
    Once binded the memory will be deallocated if the sticky flag is not set in flag.
    Return the pointer.
*/
extern kaapi_pointer_t kaapi_mem_bind( kaapi_address_space_t kasid, kaapi_metadata_info_t* kdmi, int flag, void* ptr, size_t size );
#define KAAPI_MEM_DEFAULT_FLAG 0x0
#define KAAPI_MEM_STICKY_FLAG 0x1

/** Unbind the address from the address space kasid.
    If the data exist in kasid, then the caller takes the owner ship of the data.
    If the data does not exist in the address space kasid, then the method do nothing, even if the data
    resides in other address spaces. 
*/
extern void* kaapi_mem_unbind( kaapi_address_space_t kasid, kaapi_metadata_info_t* kdmi );

/** Post an asynchronous fetch operation to copy to data pointed by kdmi into the address space kasid.
    Once the asynchronous operation has finished, the address space kasid contains a copy of data.
    The source of the data is selected from one of the valid sources of the copies.
    The data must be allocated into kasid, prior to call this function.
    
    MANQUE LE FORMAT ICI pour une copie asid -> asid
*/
extern void kaapi_mem_asyncfetch( kaapi_address_space_t kasid, kaapi_metadata_info_t* kdmi, void (*callback)(void*), void* argcallback );

/** Mark as dirty all copies except the copy into kasid.
    If no copy exists into kasid, then an error code is returned. All copies in other addresspace space than kasid
    are marked invalid.
*/
extern int kaapi_mem_setdirty_except( kaapi_metadata_info_t* kdmi, kaapi_address_space_t kasid );

/** Return the meta data associated to a virtual address @ in the host.
    Return 0 if no meta data is attached.
*/
extern kaapi_metadata_info_t* kaapi_mem_find( void* ptr );

/** Return the meta data associated to a virtual address @ in the host.
    Return a new allocated data_info if no meta data is attached.
    The newly created data info structure has no
*/
extern kaapi_metadata_info_t* kaapi_mem_findinsert( void* ptr, size_t size );

/** Get a copy to the local address space...
TODO 
*/
extern kaapi_pointer_t kaapi_mem_synchronize_( kaapi_metadata_info_t* kdmi );

#endif /*_KAAPI_DATA_H_*/
