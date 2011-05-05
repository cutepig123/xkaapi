/*
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


/** Create a new address space and returns the identifier.
    The address space identifier contains the major information:
    - the site where is the data.
    - the type (CPU or CUDA) in order to drive allocation, 
    copies etc..
    - the size of the address space.
    
    Several address spaces exist prior to the execution:
    - each kprocessor has its own address space
    - each detected level in the memory hierarchy has its own address 
    space. Each L2, or L3 has a specific address space and address space
    identifier.
    - each GPU has its own address space.

    The type of the address space drives some capabilities:
    - CPU type: transfert from CPU to CPU may be asynchronous 
    if the destination of the address is a remote site (asynchrony 
    comes from asynchrony in the communication layer).
    Else, if both address spaces are located into the same machine, 
    copy is synchronous.
    - GPU type: copy from and copy to GPU pass through the main 
    memory of the CPU and may be asynchronous, depending of 
    the hardward capability.
    
    Two address spaces share data if it exists an address space at
    an upper level that contains them.
    Data shared by two address spaces can be accessed directly by
    tasks. Coherency of memory between address spaces is guaranteed between
    writers and readers.
    A task that reads data in a different address space than the task
    that writes the data will view the correct value write.
    On current non sequential consistency multicore, the implementation
    relies on memory barrier before signaling the synchronisation.
*/

#define KAAPI_MAX_ADDRESS_SPACE 32

/** Initialize global hashmap
*/
extern void kaapi_memory_init(void);

/**
*/
extern void kaapi_memory_destroy(void);

/**
*/
struct kaapi_metadata_info_t;


/** Represent the address space of a remote (or local) memory.
*/
#define KAAPI_MEM_TYPE_CPU   KAAPI_PROC_TYPE_HOST        /* virtual address space of a processus */
#define KAAPI_MEM_TYPE_CUDA  KAAPI_PROC_TYPE_CUDA        /* CUDA allocation scheme */

typedef struct kaapi_address_space_t {
  uint64_t    asid;
  uintptr_t   segaddr;    /* base address allocation */
  uintptr_t   segsp;      /* next free position case of iso address allocation */
  uintptr_t   segsize;    /* size of address space */
} kaapi_address_space_t;


#define KAAPI_EMPTY_ADDRESS_SPACE_ID 0ULL

/* Create a new address space.
   The user is invited to use predefined address space identifier
   in place of creating a new address space.  
   The function takes the globalid, the type and the size of an
   address space. On return a new address space is created. 
   The newly created address space is not equal to any other created
   address space.
   
   \param gid  [IN] the global identifier of the process
   \param type [IN] the type of the address space (CPU or GPU)
   \param size [IN] the declared size of the address space.
   \return the address space identifier
*/
extern kaapi_address_space_id_t kaapi_memory_address_space_create(
  kaapi_globalid_t gid, 
  int type, 
  size_t size 
);

#define KAAPI_ASID_MASK_LID   0x00000000FFFFFFFFULL
#define KAAPI_ASID_MASK_GID   0x00FFFFFF00000000ULL
#define KAAPI_ASID_MASK_ARCH  0xF000000000000000ULL

/** Return true iff two address space points to the same memory
    \param kasid1 [IN] an address space identifier
    \param kasid2 [IN] an address space identifier
    \return 0 iff the two address spaces are equals
*/
static inline int kaapi_memory_address_space_isequal( 
  kaapi_address_space_id_t kasid1, 
  kaapi_address_space_id_t kasid2
)
{
  return kasid1 == kasid2;
}


/** Return the type of the address space location.
    \param kasid [IN] an address space identifier
    \return the type encoded into the address space identifier
*/
static inline int kaapi_memory_address_space_gettype( kaapi_address_space_id_t kasid )
{ return (int)(kasid >> 56UL); }


/** Return the gid of the address space
    \param kasid [IN] an address space identifier
    \return the gid encoded into the address space identifier
*/
static inline kaapi_globalid_t kaapi_memory_address_space_getgid( kaapi_address_space_id_t kasid )
{ return (kaapi_globalid_t)((kasid & KAAPI_ASID_MASK_GID)>> 32UL); }


/** Return the local index of the address space
    This is not a public function.
    \param kasid [IN] an address space identifier
    \return the local index of the address space identifier
*/
static inline uint16_t _kaapi_memory_address_space_getlid( kaapi_address_space_id_t kasid )
{ return (kasid & KAAPI_ASID_MASK_LID); }


/** Print info about address space
    The format of the output is [<type>, <gid>], the type is the type of the address space
    and gid the global identifier of the process that handles the address space.
    \param file [IN/OUT] the file descriptor where to dump the representation of the address
    space identifier.
    \param kasid [IN] an address space identifier
    \return the return value of the fprintf.
*/
extern int kaapi_memory_address_space_fprintf( FILE* file, kaapi_address_space_id_t kasid );


/** Allocate in the address space kasid a contiguous array of size bytes.
    Return the pointer in the address space 'kasid'
    If the allocation failed, then returns a null pointer.
    Allocation on a remote address space is a priori not possible and nul pointer is returned. 
    
    The memory region may be purely accessed localy. In that case the flag MEM_LOCAL should
    be put. If the memory is intented to be access from remote address space, then MEM_SHARABLE
    must be given.
    For instance a RDMAable memory should be allocated with sharable flag.
    
    \param kasid [IN] address space identifier
    \param size [IN] the size in bytes
    \param flag [IN] either KAAPI_MEM_LOCAL or KAAPI_MEM_SHARABLE
    \retval the pointer into the address space or null pointer if the allocation failed.
*/
#define KAAPI_MEM_LOCAL      0x1
#define KAAPI_MEM_SHARABLE   0x2
extern kaapi_pointer_t kaapi_memory_allocate( 
    kaapi_address_space_id_t kasid, 
    size_t size, 
    int flag 
);


/** Allocate a memory region enough to store a view.
    The view is passed in input to allocate enough bytes for the data. 
    In output, the view is updated to fit the new allocation.

    \param kasid [IN] address space identifier
    \param view [IN/OUT] the view in bytes of the memory region    
    \param flag [IN] either KAAPI_MEM_LOCAL or KAAPI_MEM_SHARABLE
    \retval the pointer into the address space or null pointer if the allocation failed.
*/
extern kaapi_pointer_t kaapi_memory_allocate_view( 
    kaapi_address_space_id_t kasid, 
    kaapi_memory_view_t* view, 
    int flag 
);


/** Global memory barrier between all nodes:
    - after this barrier, all nodes are able to read or writer memory location of remote node
*/
extern void kaapi_memory_global_barrier(void);


/** Deallocate the data pointed by the pointer
    The function can only deallocated pointer allocated on the local process.
    If pointer points to a data not in the same process, then the return 
    value of the function is EINVAL.

    \param ptr [IN/OUT] the pointer to the data to delete
    \return 0 in case of success
    \return EINVAL iff the pointer points to a non local data 
*/
extern int kaapi_memory_deallocate( 
  kaapi_pointer_t ptr 
);



/** Copy a view of a data to an other view in a remote address space.
    Source and destination memory region cannot overlap.
    \retval 0 in case of success
*/
extern int kaapi_memory_copy( 
  kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
  kaapi_pointer_t src,  const kaapi_memory_view_t* view_src 
);

/** Expose copy from cpu2cpu
*/
extern int kaapi_memory_write_cpu2cpu
(
  kaapi_pointer_t dest,
  const kaapi_memory_view_t* view_dest,
  const void* src,
  const kaapi_memory_view_t* view_src
);


/** Copy a view of a data to an other view in a remote address space.
    Source and destination memory region cannot overlap.
    \retval 0 in case of success
*/
extern int kaapi_memory_asyncopy( 
  kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
  const kaapi_pointer_t src, const kaapi_memory_view_t* view_src,
  void (*callback)(void*), void* argcallback
);


/** Make a new pointer object
*/
static inline kaapi_pointer_t kaapi_make_pointer( kaapi_address_space_id_t asid, void* ptr)
{ kaapi_pointer_t p; p.asid = asid; p.ptr = (uintptr_t)ptr; return p; }

/** Make a null pointer object
*/
static inline kaapi_pointer_t kaapi_make_nullpointer(void)
{ kaapi_pointer_t p = {0, (uintptr_t)0}; return p; }

/** Make a null pointer object
*/
static inline void kaapi_pointer_setnull(kaapi_pointer_t* ptr )
{ ptr->ptr = 0; ptr->asid = 0; }

/** Return non null value if the pointer is null.
    A null pointer is independent from its location (asid).
*/
static inline int kaapi_pointer_isnull( kaapi_pointer_t p)
{ return p.ptr ==0; }

/* cast to void* 
*/
static inline void* kaapi_pointer2void(kaapi_pointer_t p)
{ return (void*)p.ptr; }

/* cast to uintptr 
*/
static inline uintptr_t kaapi_pointer2uintptr(kaapi_pointer_t p)
{ return p.ptr; }

/* cast from void* 
*/
static inline void kaapi_void2pointer(kaapi_pointer_t* p, void* ptr)
{ p->ptr = (uintptr_t)ptr; }

/* return the address space identifier
*/
static inline kaapi_address_space_id_t kaapi_pointer2asid(kaapi_pointer_t p)
{ return p.asid; }

/* return the location (gid) where the pointer points.
*/
static inline kaapi_globalid_t kaapi_pointer2gid(kaapi_pointer_t p)
{ return kaapi_memory_address_space_getgid(p.asid); }


/** Clear the view
*/
static inline void kaapi_memory_view_clear( kaapi_memory_view_t* kmv )
{
  kmv->type = -1;
#if defined(KAAPI_DEBUG)
  kmv->size[0]  = kmv->size[1] = 0;
  kmv->lda      = 0;
  kmv->wordsize = 0;
#endif
}


/** assume that now the view points to a new allocate view
*/
static inline void kaapi_memory_view_reallocated( kaapi_memory_view_t* kmv )
{
  switch (kmv->type) 
  {
    case KAAPI_MEMORY_VIEW_1D: return;
    case KAAPI_MEMORY_VIEW_2D: kmv->lda = kmv->size[1]; return;
    default:
      kaapi_assert(0);
      break;
  }
}



/** Meta data attached to a pointer and its remote copies on a set of address spaces.
    The data structure store information for at most 64 address spaces.
    This data structure stores all the valid and invalid copies of the data.
    data[i] is valid iff validbits & (1<<i) is not null.
    data[i] is not valid but yet allocated if dirtybits & ( 1<<i) is not null.
    If data[i] is 0, then no data has been allocated in the i-th local
    memory, and both validbits & (1<<i) and dirtybits & (1<<i) are nul.
    The view of each allocated block of data is stored in the kaapi_data_t structure.
*/
typedef struct kaapi_metadata_info_t {
  uint64_t                 validbits;
  kaapi_data_t             data[KAAPI_MAX_ADDRESS_SPACE];
  struct kaapi_version_t*  version[KAAPI_MAX_ADDRESS_SPACE];          /* opaque */
} kaapi_metadata_info_t;


/** Return the meta data associated to a virtual address @ in the host.
    Return 0 if no meta data is attached.
*/
extern kaapi_metadata_info_t* kaapi_memory_find_metadata( void* ptr );

/** Return the meta data associated to a virtual address @ in the host.
    Return a new allocated data_info if no meta data is attached.
    The newly created data info structure has no
*/
extern kaapi_metadata_info_t* kaapi_mem_findinsert_metadata( void* ptr );

/** Get a copy to the local address space...
TODO 
*/
extern kaapi_pointer_t kaapi_memory_synchronize_( kaapi_metadata_info_t* kdmi );

/** Bind an address of the calling virtual space of the process in the address space data structure kasid.
    Once binded the memory will be deallocated if the sticky flag is not set in flag.
    Return the pointer.
*/
extern kaapi_metadata_info_t* kaapi_memory_bind( 
  kaapi_address_space_id_t kasid, 
  int flag, 
  void* ptr, 
  size_t size 
);


/** Bind an address of the calling virtual space of the process in the address space data structure kasid.
    Once binded the memory will be deallocated if the sticky flag is not set in flag.
    Return the pointer.
*/
extern kaapi_metadata_info_t* kaapi_memory_bind_view( 
  kaapi_address_space_id_t kasid, 
  int flag, 
  void* ptr, 
  const kaapi_memory_view_t* view 
);


/** Unbind the address from the address space kasid.
    If the data exist in kasid, then the caller takes the owner ship of the data.
    If the data does not exist in the address space kasid, then the method do nothing, even if the data
    resides in other address spaces. 
*/
extern void* kaapi_memory_unbind( kaapi_metadata_info_t* kmdi );


/**/
extern struct kaapi_version_t** _kaapi_metadata_info_bind_data( 
    kaapi_metadata_info_t* kmdi, 
    kaapi_address_space_id_t kasid, 
    void* ptr, const kaapi_memory_view_t* view
);


/**/
static inline void _kaapi_metadata_info_unbind_alldata( 
    kaapi_metadata_info_t* kmdi
)
{
  kmdi->validbits = 0UL;
}


/**/
static inline kaapi_data_t* _kaapi_metadata_info_get_data( 
    kaapi_metadata_info_t* kmdi, 
    kaapi_address_space_id_t kasid
)
{
  uint16_t lid = _kaapi_memory_address_space_getlid( kasid );
  kaapi_assert_debug( lid < KAAPI_MAX_ADDRESS_SPACE );
  return &kmdi->data[lid];
}

/**/
static inline int _kaapi_metadata_info_is_valid( 
    kaapi_metadata_info_t* kmdi, 
    kaapi_address_space_id_t kasid
)
{ 
  uint16_t lid = _kaapi_memory_address_space_getlid( kasid );
  kaapi_assert_debug( lid < KAAPI_MAX_ADDRESS_SPACE );
  return (kmdi->validbits & (1UL<<lid)) !=0;
}

/**/
static inline int _kaapi_metadata_info_is_novalid( 
    kaapi_metadata_info_t* kmdi
)
{ 
  return (kmdi->validbits == 0UL);
}

/**/
static inline void _kaapi_metadata_info_set_writer( 
    kaapi_metadata_info_t* kmdi,
    kaapi_address_space_id_t kasid
)
{ 
  uint16_t lid = _kaapi_memory_address_space_getlid( kasid );
  kaapi_assert_debug( lid < KAAPI_MAX_ADDRESS_SPACE );
  kmdi->validbits = 1UL << lid;
}

#if 0
/**/
extern struct kaapi_version_t* _kaapi_metadata_info_get_version( 
    kaapi_metadata_info_t* kmdi,
    kaapi_address_space_id_t kasid
);
#endif

/** Return a version that handle a valid copy in order to make a copy to kasid 
*/
extern struct kaapi_version_t* _kaapi_metadata_info_find_onewriter(
    kaapi_metadata_info_t* kmdi,
    kaapi_address_space_id_t kasid
);

/* Make a copy of the data into kasid
*/
static inline struct kaapi_version_t** _kaapi_metadata_info_copy_data(
    kaapi_metadata_info_t* kmdi,
    kaapi_address_space_id_t kasid,
    const kaapi_memory_view_t* view    
)
{
  uint16_t lid = _kaapi_memory_address_space_getlid( kasid );
  kmdi->validbits |= 1UL << lid;
  kmdi->data[lid].ptr  = kaapi_make_pointer(kasid, 0);
  kmdi->data[lid].view = *view;
  kaapi_memory_view_reallocated(&kmdi->data[lid].view);
  return &kmdi->version[lid];
}


#endif /*_KAAPI_DATA_H_*/
