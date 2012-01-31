
#include <stdio.h>
#include <cuda_runtime_api.h>

#include "kaapi_impl.h"
//#include "../memory/kaapi_mem.h"
#include "../../src/machine/mt/kaapi_mt_machine.h"
#include "kaapi_cuda_mem.h"
//#include "kaapi_cuda_error.h"

typedef struct kaapi_cuda_mem_blk_t {
	struct kaapi_cuda_mem_blk_t* next;
	struct kaapi_cuda_mem_blk_t* prev;
	kaapi_pointer_t		    ptr;
	size_t			    size;
} kaapi_cuda_mem_blk_t;

static int
kaapi_cuda_mem_blk_insert(
		kaapi_processor_t*	proc,
		kaapi_pointer_t*	ptr,
		size_t			size
	)
{
	kaapi_hashentries_t* entry;
	kaapi_cuda_mem_t* cuda_mem = &proc->cuda_proc.memory;
	kaapi_cuda_mem_blk_t *blk= (kaapi_cuda_mem_blk_t*)malloc(
			sizeof(kaapi_cuda_mem_blk_t) );
	if( blk == NULL )
		return -1;

	blk->ptr = *ptr;
	blk->size = size;
	blk->prev = blk->next = NULL;
	if( cuda_mem->beg == NULL ) {
		cuda_mem->beg = blk;
	} else {
		blk->prev= cuda_mem->end;
		cuda_mem->end->next = blk;
	}
	cuda_mem->end = blk;

	entry = kaapi_big_hashmap_findinsert( &cuda_mem->kmem,
		__kaapi_pointer2void(*ptr) );
	entry->u.block = blk;
	cuda_mem->used += size;

	return 0;
}

static int 
kaapi_cuda_mem_blk_remove( 
		kaapi_processor_t*	proc,
		const size_t size
	)
{
	kaapi_pointer_t ptr;
	kaapi_hashentries_t* entry;
	kaapi_cuda_mem_blk_t *blk;
	kaapi_cuda_mem_t* cuda_mem = &proc->cuda_proc.memory;
//	kaapi_data_t* ptr;
	const kaapi_mem_host_map_t* cuda_map = kaapi_get_current_mem_host_map();
	const kaapi_mem_asid_t cuda_asid = kaapi_mem_host_map_get_asid(cuda_map);
	kaapi_mem_data_t *kmd;
	size_t mem_free= 0;
	size_t ptr_size;


	if( cuda_mem->beg == NULL )
		return -1;

	while( NULL != (blk= cuda_mem->beg) ) {
		cuda_mem->beg = blk->next;
		if( cuda_mem->beg != NULL )
		    cuda_mem->beg->prev = NULL;
		ptr = blk->ptr;
		ptr_size = blk->size;
		free( blk );
		kaapi_mem_host_map_find_or_insert( 
			(kaapi_mem_addr_t)__kaapi_pointer2void(ptr), &kmd );
		entry = kaapi_big_hashmap_findinsert( &cuda_mem->kmem,
			__kaapi_pointer2void(ptr) );
		entry->u.block = NULL;
#if 0
		if( kaapi_mem_data_has_addr( kmd, cuda_asid ) &&
			kaapi_mem_data_is_dirty( kmd, cuda_asid ) ) {
		    valid_asid= kaapi_mem_data_get_nondirty_asid( kmd );
		    fprintf( stdout, "[%s] DIRTY devptr=%p valid_asid=%lu\n",
			    __FUNCTION__,
			    devptr,
			    valid_asid );
		    fflush(stdout);
		}
#endif
#if 1
		fprintf( stdout, "[%s] TOFREE devptr=%p\n",
			__FUNCTION__, __kaapi_pointer2void(ptr) );
		fflush(stdout);
#endif
		if( kaapi_mem_data_has_addr( kmd, cuda_asid ) ) {
//		    ptr = (kaapi_data_t*) kaapi_mem_data_get_addr( kmd, cuda_asid );
		    kaapi_mem_data_clear_addr( kmd, cuda_asid );
		    kaapi_mem_data_clear_dirty( kmd, cuda_asid );
		}
#if 1
		fprintf( stdout, "[%s] FREE devptr=%p size=%lu\n",
			__FUNCTION__,
			__kaapi_pointer2void(ptr),
			 ptr_size );
		fflush(stdout);
#endif
		/* nor allocated neither valid */
		kaapi_cuda_mem_free( &ptr );
		mem_free += ptr_size;
#if 0
	       	else {
		    const kaapi_mem_host_map_t* host_map = 
			kaapi_processor_get_mem_host_map(kaapi_all_kprocessors[0]);
		    const kaapi_mem_asid_t host_asid =
			kaapi_mem_host_map_get_asid(host_map);
		    ptr = (kaapi_data_t*) kaapi_mem_data_get_addr( kmd, host_asid );
		    mem_free += kaapi_memory_view_size( &ptr->view );
		}
#endif
		if( mem_free >= (size * KAAPI_CUDA_MEM_FREE_FACTOR) )
			break;
	}
#if 1
	fprintf(stdout, "[%s] kid=%lu lid=%d size=%lu(%lu) used=%lu total=%lu\n",
			__FUNCTION__,
			(long unsigned int)kaapi_get_current_kid(),
			cuda_asid, size, mem_free, cuda_mem->used,
			cuda_mem->total );
	fflush(stdout);
#endif
	if( cuda_mem->used < mem_free )
	    cuda_mem->used = 0;
	else
	    cuda_mem->used -= mem_free;

	return 0;
}

static inline int
__kaapi_cuda_mem_is_full( kaapi_processor_t* proc, const size_t size )
{
	if( (proc->cuda_proc.memory.used+size) >= 
			(proc->cuda_proc.memory.total) )
//			(proc->cuda_proc.memory.total+(2<<20)) )
	    return 1;
	else
	    return 0;
}

int
kaapi_cuda_mem_alloc(
		kaapi_pointer_t *ptr,
		const kaapi_address_space_id_t kasid,
		const size_t size,
		const int flag
	)
{
	void* devptr;
	cudaError_t res;
  	kaapi_processor_t* const proc = kaapi_get_current_processor();

	if( __kaapi_cuda_mem_is_full( proc, size) )
		if( kaapi_cuda_mem_blk_remove( proc, size ) )
			return -1;

out_of_memory:
	res = cudaMalloc( &devptr, size );
	if( res == CUDA_ERROR_OUT_OF_MEMORY ) {
		if( kaapi_cuda_mem_blk_remove( proc, size ) )
			return -1;
		goto out_of_memory;
	}
	if (res != CUDA_SUCCESS) {
		fprintf( stdout, "[%s] ERROR cuMemAlloc (%d) size=%lu kid=%lu\n",
				__FUNCTION__, res, size, 
		      		(long unsigned int)kaapi_get_current_kid() ); 
		fflush( stdout );
	}else {
		ptr->ptr = (uintptr_t)devptr;
		ptr->asid = kasid;
	}
	kaapi_cuda_mem_blk_insert( proc, ptr, size );

#if 0
	fprintf(stdout, "[%s] mem=%lu used=%lu kid=%lu\n", __FUNCTION__,
			(size_t)(proc->cuda_proc.memory.total/(2<<20)),
			(size_t)(proc->cuda_proc.memory.used/(2<<20)),
			(long unsigned int)kaapi_get_current_kid() );
	fflush( stdout );
#endif
	fprintf(stdout, "[%s] data=%p size=%lu kid=%lu\n", __FUNCTION__,
			ptr->ptr,
			size,
			kaapi_get_current_kid() );
	fflush( stdout );
	return res;
}

int
kaapi_cuda_mem_free( kaapi_pointer_t *ptr )
{
#if 1
    fprintf( stdout, "[%s] ptr=%p\n", __FUNCTION__,
	kaapi_pointer2void(*ptr) );
    fflush(stdout);
#endif
	cudaFree( __kaapi_pointer2void(*ptr) );
	ptr->ptr = 0;
	ptr->asid = 0;
	return 0;
}

int
kaapi_cuda_mem_inc_use( kaapi_pointer_t *ptr ) 
{
    kaapi_hashentries_t* entry;
    void* devptr = __kaapi_pointer2void(*ptr);
    kaapi_cuda_mem_t* cuda_mem =
	&kaapi_get_current_processor()->cuda_proc.memory;
    kaapi_cuda_mem_blk_t *blk;
    kaapi_cuda_mem_blk_t *blk_next;
    kaapi_cuda_mem_blk_t *blk_prev;

    entry = kaapi_big_hashmap_findinsert( &cuda_mem->kmem, (void*)devptr );
    if (entry->u.block == 0)
	return -1;
    blk= (kaapi_cuda_mem_blk_t*)entry->u.block;
    if( cuda_mem->end == blk )
	return 0;

    blk_prev = blk->prev;
    blk_next = blk->next;
    /* remove */

    blk_next->prev = blk_prev;
    if( blk_prev != NULL )
	blk_prev->next = blk_next;
    else /* first block */
	cuda_mem->beg = blk_next;

    if( cuda_mem->end != NULL )
	cuda_mem->end->next = blk;
    blk->prev = cuda_mem->end;
    blk->next = NULL;
    cuda_mem->end = blk;

    return 0;
}

int kaapi_cuda_mem_copy_htod(
	kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
	const kaapi_pointer_t src, const kaapi_memory_view_t* view_src
		)
{
#if 0
		fprintf(stdout, "[%s] src=%p dst=%p size=%ld\n", __FUNCTION__,
				__kaapi_pointer2void(src),
				__kaapi_pointer2void(dest),
			       	kaapi_memory_view_size( view_src ));
		fflush(stdout);
#endif
	switch (view_src->type) {
	case KAAPI_MEMORY_VIEW_1D:
	{
		return kaapi_cuda_mem_1dcopy_htod( dest, view_dest,
			       src, view_src );
		break;
	}

	case KAAPI_MEMORY_VIEW_2D:
	{
		return kaapi_cuda_mem_2dcopy_htod( dest, view_dest,
				src, view_src );
		break;
	}

	/* not supported */
	default:
	{
		kaapi_assert(0);
		goto on_error;
		break ;
	}
	}

	return 0;
on_error:
	return -1;
}

int kaapi_cuda_mem_copy_dtoh(
	kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
	const kaapi_pointer_t src, const kaapi_memory_view_t* view_src
		)
{
#if 0
		fprintf(stdout, "[%s] src=%p dst=%p size=%ld\n", __FUNCTION__,
				__kaapi_pointer2void(src),
				__kaapi_pointer2void(dest),
			       	kaapi_memory_view_size( view_src ));
		fflush(stdout);
#endif
	switch (view_src->type) {
	case KAAPI_MEMORY_VIEW_1D:
	{
		return kaapi_cuda_mem_1dcopy_dtoh( dest, view_dest,
			       src, view_src );
		break;
	}

	case KAAPI_MEMORY_VIEW_2D:
	{
		return kaapi_cuda_mem_2dcopy_dtoh( dest, view_dest,
				src, view_src );
		break;
	}

	/* not supported */
	default:
	{
		kaapi_assert(0);
		goto on_error;
		break ;
	}
	}

	return 0;
on_error:
	return -1;
}

//#endif /* KAAPI_CUDA_ASYNC */

int kaapi_cuda_mem_register( kaapi_pointer_t ptr, 
		const kaapi_memory_view_t *view )
{
#if 0
	const cudaError_t res = 
#endif
#if KAAPI_CUDA_ASYNC
	cudaHostRegister(
		(void*)__kaapi_pointer2void(ptr),
		kaapi_memory_view_size(view),
		cudaHostRegisterPortable );
#endif

#if 0
	if (res != CUDA_SUCCESS) {
		fprintf(stdout, "[%s] ERROR: %d\n", __FUNCTION__, res );
		fflush(stdout);
		return -1;
	}
	else {
		fprintf(stdout, "[%s] OK: %d\n", __FUNCTION__, res );
		fflush(stdout);
	}
#endif
	return 0;
}

int
kaapi_cuda_mem_1dcopy_htod(
	kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
	const kaapi_pointer_t src, const kaapi_memory_view_t* view_src
	)
{
	const size_t size = kaapi_memory_view_size( view_src );

#if KAAPI_CUDA_ASYNC
	const cudaError_t res = cudaMemcpyAsync(
			 __kaapi_pointer2void(dest),
			__kaapi_pointer2void(src),
			size,
			cudaMemcpyHostToDevice,
			kaapi_cuda_HtoD_stream() );
#else
	const cudaError_t res = cudaMemcpy(
			 __kaapi_pointer2void(dest),
			__kaapi_pointer2void(src),
			size,
			cudaMemcpyHostToDevice );
#endif
	if (res != CUDA_SUCCESS) {
		fprintf(stdout, "[%s] ERROR: %d\n", __FUNCTION__, res );
		fflush(stdout);
	}

	return res;
}

int
kaapi_cuda_mem_1dcopy_dtoh(
	kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
	const kaapi_pointer_t src, const kaapi_memory_view_t* view_src
	)
{
	const size_t size = kaapi_memory_view_size( view_src );

#if KAAPI_CUDA_ASYNC
	const cudaError_t res = cudaMemcpyAsync(
			 __kaapi_pointer2void(dest),
			__kaapi_pointer2void(src),
			size,
			cudaMemcpyDeviceToHost,
			kaapi_cuda_DtoH_stream() );
#else
	const cudaError_t res = cudaMemcpy(
			 __kaapi_pointer2void(dest),
			__kaapi_pointer2void(src),
			size,
			cudaMemcpyDeviceToHost);
#endif
	if (res != CUDA_SUCCESS) {
		fprintf(stdout, "[%s] ERROR: %d\n", __FUNCTION__, res );
		fflush(stdout);
	}

	return res;
}

int
kaapi_cuda_mem_2dcopy_htod(
	kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
	const kaapi_pointer_t src, const kaapi_memory_view_t* view_src
	)
{
	cudaError_t res;

#if KAAPI_VERBOSE
		fprintf(stdout, "[%s] src=%p %ldx%ld lda=%ld dst=%p %ldx%ld lda=%ld size=%ld\n",
				__FUNCTION__,
				__kaapi_pointer2void(src),
				view_src->size[0], view_src->size[1],
				view_src->lda,
				__kaapi_pointer2void(dest),
				view_dest->size[0], view_dest->size[1],
				view_dest->lda,
			       	kaapi_memory_view_size( view_src ));
		fflush(stdout);
#endif
#if 0
	CUDA_MEMCPY2D m;
	m.srcMemoryType = CU_MEMORYTYPE_HOST;
	m.srcHost = __kaapi_pointer2void(src);
	m.srcPitch = view_src->lda * view_src->wordsize;
	m.srcXInBytes = 0;
	m.srcY = 0;

	m.dstXInBytes = 0;
	m.dstY = 0;
	m.dstMemoryType = CU_MEMORYTYPE_DEVICE;
	m.dstDevice = (uintptr_t) __kaapi_pointer2void(dest);
	m.dstPitch = view_dest->size[1] * view_dest->wordsize;

	m.WidthInBytes = view_dest->size[1] * view_dest->wordsize;
	m.Height = view_dest->size[0];
#endif

#if KAAPI_CUDA_ASYNC
	res = cudaMemcpy2DAsync(
	    __kaapi_pointer2void(dest),
	    view_dest->size[1] * view_dest->wordsize,
	    __kaapi_pointer2void(src),
	    view_src->lda * view_src->wordsize,
	    view_dest->size[1] * view_dest->wordsize,
	    view_dest->size[0],
	    cudaMemcpyHostToDevice,
	   kaapi_cuda_HtoD_stream() );
#else
	res = cudaMemcpy2D(
	    __kaapi_pointer2void(dest),
	    view_dest->size[1] * view_dest->wordsize,
	    __kaapi_pointer2void(src),
	    view_src->lda * view_src->wordsize,
	    view_dest->size[1] * view_dest->wordsize,
	    view_dest->size[0],
	    cudaMemcpyHostToDevice );
#endif
	if (res != CUDA_SUCCESS) {
		fprintf( stdout, "[%s] ERROR cuMemcpy2D (%d) kid=%lu src=%p dst=%p size=%lu\n",
				__FUNCTION__, res,
		      		(long unsigned int)kaapi_get_current_kid(),
				__kaapi_pointer2void(src),
				__kaapi_pointer2void(dest),
				kaapi_memory_view_size(view_src) ); 
		fflush( stdout );
	}

	return res;
}

int
kaapi_cuda_mem_2dcopy_dtoh(
	kaapi_pointer_t dest, const kaapi_memory_view_t* view_dest,
	const kaapi_pointer_t src, const kaapi_memory_view_t* view_src
	)
{
	cudaError_t res;

#if KAAPI_VERBOSE
		fprintf(stdout, "[%s] src=%p %ldx%ld lda=%ld dst=%p %ldx%ld lda=%ld size=%ld\n",
				__FUNCTION__,
				__kaapi_pointer2void(src),
				view_src->size[0], view_src->size[1],
				view_src->lda,
				__kaapi_pointer2void(dest),
				view_dest->size[0], view_dest->size[1],
				view_dest->lda,
			       	kaapi_memory_view_size( view_src ));
		fflush(stdout);
#endif
#if 0
	CUDA_MEMCPY2D m;
	m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
	m.srcDevice = (uintptr_t) __kaapi_pointer2void(src);
	m.srcPitch = view_src->size[1] * view_src->wordsize;
	m.srcXInBytes = 0;
	m.srcY = 0;

	m.dstXInBytes = 0;
	m.dstY = 0;
	m.dstMemoryType = CU_MEMORYTYPE_HOST;
	m.dstHost = __kaapi_pointer2void(dest);
	m.dstPitch = view_dest->lda * view_dest->wordsize;

	m.WidthInBytes = view_src->size[1] * view_src->wordsize;
	m.Height = view_src->size[0];
#endif

#if KAAPI_CUDA_ASYNC
	res = cudaMemcpy2DAsync(
		__kaapi_pointer2void(dest),
		view_dest->lda * view_dest->wordsize,
		__kaapi_pointer2void(src),
		view_src->size[1] * view_src->wordsize,
		view_src->size[1] * view_src->wordsize,
		view_src->size[0],
		cudaMemcpyDeviceToHost,
	       kaapi_cuda_DtoH_stream()	);
#else
	res = cudaMemcpy2D(
		__kaapi_pointer2void(dest),
		view_dest->lda * view_dest->wordsize,
		__kaapi_pointer2void(src),
		view_src->size[1] * view_src->wordsize,
		view_src->size[1] * view_src->wordsize,
		view_src->size[0],
		cudaMemcpyDeviceToHost );
#endif
	if (res != CUDA_SUCCESS) {
		fprintf( stdout, "[%s] ERROR cuMemcpy2D (%d) kid=%lu src=%p dst=%p size=%lu\n",
				__FUNCTION__, res,
		      		(long unsigned int)kaapi_get_current_kid(),
				__kaapi_pointer2void(src),
				__kaapi_pointer2void(dest),
				kaapi_memory_view_size(view_src) ); 
		fflush( stdout );
		  return EINVAL;
	}

	return res;
}

#if 0
/* here it checks if the CPU pointer is present in the GPU.
 * If not, it allocates and copy to GPU.
 * Context: it executes right before a CUDA task (kernel).
 */
static inline int
xxx_kaapi_cuda_mem_sync_params_findinsert(
		kaapi_thread_context_t* thread,
		kaapi_metadata_info_t*  mdi,
		kaapi_data_t* dest,
		kaapi_data_t* src
		)
{
	if( __kaapi_pointer2void(dest->ptr) == NULL ) {
		kaapi_cuda_mem_alloc( mdi, &dest->ptr, thread->asid,
			kaapi_memory_view_size(&dest->view), 0 );

		_kaapi_metadata_info_bind_data( mdi, thread->asid,
			__kaapi_pointer2void(dest->ptr), &dest->view );

		kaapi_mem_findinsert_metadata2( __kaapi_pointer2void(dest->ptr), mdi );

		kaapi_cuda_mem_register( src->ptr, &src->view );

		kaapi_cuda_mem_copy_htod( dest->ptr, &dest->view, 
			src->ptr, &src->view );
	}

	return 0;
}

/* The function checks if the dest memory is valid on GPU 
 * */
static inline int
xxx_kaapi_cuda_mem_sync_params_ro(
		kaapi_thread_context_t* thread,
		kaapi_metadata_info_t*  mdi,
		kaapi_data_t* dest,
		kaapi_data_t* src
		)
{
	if ( !_kaapi_metadata_info_is_valid(mdi, thread->asid) ) {
#if 0
	fprintf(stdout, "[%s] NOT VALID kid=%lu\n", __FUNCTION__,
		kaapi_get_current_kid() );
	fflush( stdout );
#endif
		kaapi_cuda_mem_copy_htod( dest->ptr, &dest->view,
			src->ptr, &src->view );
		_kaapi_metadata_info_set_valid( mdi, thread->asid );
	}
	return 0;
}

/*
 * The function sets the current GPU thread as a writter 
 */
static inline int
xxx_kaapi_cuda_mem_sync_params_w(
		kaapi_thread_context_t* thread,
	       	kaapi_data_t* handle 
		)
{
	kaapi_metadata_info_t*  mdi =
		kaapi_memory_find_metadata( __kaapi_pointer2void(handle->ptr) );
	_kaapi_metadata_info_set_writer( mdi, thread->asid );
	return 0;
}

/* 
 * Context: it executes right before a CUDA task (kernel).
 * The function goes through every parameter and checks if it is allocated and
 * valid on GPU memory.
 */
int kaapi_cuda_mem_sync_params( 
	kaapi_thread_context_t* thread,
	kaapi_taskdescr_t*         td,
	kaapi_task_t*              pc
)
{
	size_t count_params = kaapi_format_get_count_params(td->fmt, pc->sp );
	unsigned int i;
	kaapi_metadata_info_t*  mdi;

#if KAAPI_VERBOSE
	fprintf(stdout, "[%s] CUDA params=%ld kid=%lu\n", __FUNCTION__,
		count_params,
		(unsigned long)kaapi_get_current_kid() );
	fflush( stdout );
#endif
	for ( i=0; i < count_params; i++ ) {
		kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE(
			kaapi_format_get_mode_param(td->fmt, i, pc->sp) );
		if (m == KAAPI_ACCESS_MODE_V) 
			continue;

		kaapi_access_t access = kaapi_format_get_access_param( td->fmt,
			       	i, pc->sp );
		kaapi_data_t* handle = (kaapi_data_t*)access.data;
		//mdi = kaapi_mem_findinsert_metadata( __kaapi_pointer2void(handle->ptr) );
		mdi = kaapi_memory_find_metadata( __kaapi_pointer2void(handle->ptr) );
		kaapi_data_t* dest = _kaapi_metadata_info_get_data( mdi,
			       	thread->asid );
		dest->view = kaapi_format_get_view_param( td->fmt,
			i, pc->sp );
		dest->mdi= mdi;
		xxx_kaapi_cuda_mem_sync_params_findinsert( thread, mdi,
			       	dest, handle );

		if( KAAPI_ACCESS_IS_READ(m) )
			xxx_kaapi_cuda_mem_sync_params_ro( thread, mdi,
					dest, handle );

		if( KAAPI_ACCESS_IS_WRITE(m) )
			xxx_kaapi_cuda_mem_sync_params_w( thread, handle );

		/* sets new pointer to the task */
		access.data =  dest;
		kaapi_format_set_access_param( td->fmt, i, pc->sp, &access );
	}

	return 0;
}

static inline int
xxx_kaapi_cuda_mem_sync_params_dtoh(
		kaapi_thread_context_t* thread,
		kaapi_metadata_info_t*  mdi,
		kaapi_data_t* h_dest,
		kaapi_data_t* d_src
		)
{
	kaapi_cuda_mem_copy_dtoh( h_dest->ptr, &h_dest->view,
		d_src->ptr, &d_src->view );
	return 0;
}

int kaapi_cuda_mem_sync_params_dtoh( 
	kaapi_thread_context_t* thread,
	kaapi_taskdescr_t*         td,
	kaapi_task_t*              pc
)
{
	size_t count_params = kaapi_format_get_count_params(td->fmt, pc->sp );
	unsigned int i;
	kaapi_metadata_info_t*  mdi;
	const kaapi_address_space_id_t host_asid = 0UL;

#if KAAPI_VERBOSE
	fprintf(stdout, "[%s] CUDA params=%ld kid=%lu\n", __FUNCTION__,
		count_params,
		(unsigned long)kaapi_get_current_kid() );
	fflush( stdout );
#endif
	for ( i=0; i < count_params; i++ ) {
		kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE(
			kaapi_format_get_mode_param(td->fmt, i, pc->sp) );
		if (m == KAAPI_ACCESS_MODE_V) 
			continue;

		if( KAAPI_ACCESS_IS_WRITE(m) ) {
		kaapi_access_t access = kaapi_format_get_access_param( td->fmt,
			       	i, pc->sp );
		kaapi_data_t* handle = (kaapi_data_t*)access.data;
		mdi = handle->mdi;
		kaapi_data_t* dest = _kaapi_metadata_info_get_data( mdi,
			       	host_asid );
		xxx_kaapi_cuda_mem_sync_params_dtoh( thread, mdi, dest, handle);
		_kaapi_metadata_info_set_valid( mdi, host_asid );
		}
	}

	return 0;
}
#endif