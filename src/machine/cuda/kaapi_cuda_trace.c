/*
** xkaapi
** 
** Created on Jan 2012
** Copyright 2010 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** Joao.Lima@imag.fr
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


#include <stdio.h>
#include <cuda_runtime_api.h>

#include "kaapi_impl.h"
#include "kaapi_cuda_impl.h"

#if defined(KAAPI_USE_PERFCOUNTER) && defined(KAAPI_USE_CUPTI)

#include <cupti.h>

#define ALIGN_SIZE (8)
#define BUFFER_SIZE	(1024 * 1024)
#define ALIGN_BUFFER(buffer, align) \
  (((uintptr_t) (buffer) & ((align)-1)) ? ((buffer) + (align) - ((uintptr_t) (buffer) & ((align)-1))) : (buffer))

static size_t kaapi_cuda_trace_buffer_size = 0;

#if 0
static const char *getMemcpyKindString(CUpti_ActivityMemcpyKind kind)
{
  switch (kind) {
  case CUPTI_ACTIVITY_MEMCPY_KIND_HTOD:
    return "HtoD";
  case CUPTI_ACTIVITY_MEMCPY_KIND_DTOH:
    return "DtoH";
  case CUPTI_ACTIVITY_MEMCPY_KIND_HTOA:
    return "HtoA";
  case CUPTI_ACTIVITY_MEMCPY_KIND_ATOH:
    return "AtoH";
  case CUPTI_ACTIVITY_MEMCPY_KIND_ATOA:
    return "AtoA";
  case CUPTI_ACTIVITY_MEMCPY_KIND_ATOD:
    return "AtoD";
  case CUPTI_ACTIVITY_MEMCPY_KIND_DTOA:
    return "DtoA";
  case CUPTI_ACTIVITY_MEMCPY_KIND_DTOD:
    return "DtoD";
  case CUPTI_ACTIVITY_MEMCPY_KIND_HTOH:
    return "HtoH";
  default:
    break;
  }

  return "<unknown>";
}
#endif

static inline void
kaapi_cuda_trace_record_memcpy(kaapi_processor_t * kproc,
			       CUpti_ActivityMemcpy * memcpy)
{
  CUpti_ActivityMemcpyKind kind =
      (CUpti_ActivityMemcpyKind) memcpy->copyKind;

  switch (kind) {
    case CUPTI_ACTIVITY_MEMCPY_KIND_HTOD:
      KAAPI_CUDA_EVENT_PUSH0_(kproc, memcpy->start, KAAPI_EVT_CUDA_GPU_HTOD_BEG);
      KAAPI_CUDA_EVENT_PUSH0_(kproc, memcpy->end, KAAPI_EVT_CUDA_GPU_HTOD_END);
      break;
    case CUPTI_ACTIVITY_MEMCPY_KIND_DTOH:
      KAAPI_CUDA_EVENT_PUSH0_(kproc, memcpy->start, KAAPI_EVT_CUDA_GPU_DTOH_BEG);
      KAAPI_CUDA_EVENT_PUSH0_(kproc, memcpy->end, KAAPI_EVT_CUDA_GPU_DTOH_END);
      break;
    default:
      break;
  }
}

static inline void
kaapi_cuda_trace_record_kernel(kaapi_processor_t * kproc,
			       CUpti_ActivityKernel * kernel)
{
  KAAPI_CUDA_EVENT_PUSH0_(kproc, kernel->start, KAAPI_EVT_CUDA_GPU_KERNEL_BEG);
  KAAPI_CUDA_EVENT_PUSH0_(kproc, kernel->end, KAAPI_EVT_CUDA_GPU_KERNEL_END);
}

static inline void kaapi_cuda_trace_record(CUpti_Activity * record)
{
  kaapi_processor_t *kproc;

  switch (record->kind) {
  case CUPTI_ACTIVITY_KIND_MEMCPY:
    {
      CUpti_ActivityMemcpy *memcpy = (CUpti_ActivityMemcpy *) record;
      kproc = kaapi_cuda_get_proc_by_dev(memcpy->deviceId);
      kaapi_cuda_trace_record_memcpy(kproc, memcpy);
      break;
    }
  case CUPTI_ACTIVITY_KIND_KERNEL:
    {
      CUpti_ActivityKernel *kernel = (CUpti_ActivityKernel *) record;
      kproc = kaapi_cuda_get_proc_by_dev(kernel->deviceId);
      kaapi_cuda_trace_record_kernel(kproc, kernel);
      break;
    }
  default:
    break;
  }
}


static inline void
kaapi_cuda_trace_add_buffer(CUcontext ctx, uint32_t stream)
{
  size_t size = kaapi_cuda_trace_buffer_size;
  uint8_t *buffer = (uint8_t *) malloc(size + ALIGN_SIZE);
  const CUptiResult res = cuptiActivityEnqueueBuffer(ctx, stream,
						     ALIGN_BUFFER(buffer, ALIGN_SIZE),
						     size);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiActivityEnqueueBuffer ERROR %d\n",
	    __FUNCTION__, res);
    fflush(stdout);
  }
}


static uint8_t *kaapi_cuda_trace_dump(CUcontext context, uint32_t streamId)
{
  uint8_t *buffer = NULL;
  size_t validBufferSizeBytes;
  CUptiResult status;
  status = cuptiActivityDequeueBuffer(context, streamId, &buffer,
				      &validBufferSizeBytes);
  if (status == CUPTI_ERROR_QUEUE_EMPTY)
    return NULL;

  CUpti_Activity *record = NULL;
  do {
    status = cuptiActivityGetNextRecord(buffer, validBufferSizeBytes,
					&record);
    if (status == CUPTI_SUCCESS) {
      kaapi_cuda_trace_record(record);
    } else if (status == CUPTI_ERROR_MAX_LIMIT_REACHED) {
      break;
    } else {
      goto error;
    }
  }
  while (1);

  // report any records dropped from the queue
  size_t dropped;
  cuptiActivityGetNumDroppedRecords(context, streamId, &dropped);
  if (dropped != 0)
    fprintf(stdout,
	    "%s: *** Kaapi: (kid=%d) ERROR dropped %u CUPTI activity records (to avoid this warning see '%s').\n", __FUNCTION__,
      kaapi_get_self_kid(),
	    (unsigned int)dropped,
      "KAAPI_CUDA_TRACE_BUFFER"
      );

  return buffer;
error:
  return NULL;
}

static inline uint8_t *kaapi_cuda_trace_record_if_full(CUcontext context,
						       uint32_t streamId)
{
  size_t validBufferSizeBytes;
  CUptiResult status;
  status =
      cuptiActivityQueryBuffer(context, streamId, &validBufferSizeBytes);
  if (status == CUPTI_ERROR_MAX_LIMIT_REACHED)
    return kaapi_cuda_trace_dump(context, streamId);

  return NULL;
}

static void
kaapi_cuda_trace_handle_synchronization(CUpti_CallbackId cbid,
			const CUpti_SynchronizeData * syncData)
{
  // check the top buffer of the global queue and dequeue if full. If
  // we dump a buffer add it back to the queue
  uint8_t *buffer = kaapi_cuda_trace_record_if_full(NULL, 0);
  if (buffer != NULL) {
    cuptiActivityEnqueueBuffer(NULL, 0, buffer, kaapi_cuda_trace_buffer_size);
  }
  // dump context buffer on context sync
  if (cbid == CUPTI_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED) {
    buffer = kaapi_cuda_trace_record_if_full(syncData->context, 0);
    if (buffer != NULL) {
      cuptiActivityEnqueueBuffer(syncData->context, 0, buffer,
				 kaapi_cuda_trace_buffer_size);
    }
  } else if (cbid == CUPTI_CBID_SYNCHRONIZE_STREAM_SYNCHRONIZED) {
    /* dump the first buffer at each synchronization */
    uint32_t streamId;
    cuptiGetStreamId(syncData->context, syncData->stream, &streamId);
    buffer = kaapi_cuda_trace_dump(syncData->context, streamId);
    cuptiActivityEnqueueBuffer(syncData->context, streamId, buffer, kaapi_cuda_trace_buffer_size);
  }
}

static void
kaapi_cuda_trace_handle_resource(CUpti_CallbackId cbid,
                                        const CUpti_ResourceData * data)
{
  uint32_t stream_id;
  cuptiGetStreamId(data->context, data->resourceHandle.stream, &stream_id);
  if(cbid == CUPTI_CBID_RESOURCE_STREAM_CREATED){
    kaapi_cuda_trace_add_buffer(data->context, stream_id);    
  } else if( cbid == CUPTI_CBID_RESOURCE_STREAM_DESTROY_STARTING) {
    while (kaapi_cuda_trace_dump(data->context, stream_id) != NULL);
  }
}

static void CUPTIAPI
kaapi_cuda_trace_callback(void *userdata, CUpti_CallbackDomain domain,
			  CUpti_CallbackId cbid, const void *cbdata)
{
  if (domain == CUPTI_CB_DOMAIN_RESOURCE) {
    kaapi_cuda_trace_handle_resource(cbid, (CUpti_ResourceData *) cbdata);
  } else if (domain == CUPTI_CB_DOMAIN_SYNCHRONIZE) {
    kaapi_cuda_trace_handle_synchronization(cbid, (CUpti_SynchronizeData *) cbdata);
  }
}

int kaapi_cuda_trace_init(void)
{
  CUptiResult res;
  CUpti_SubscriberHandle subscriber;
  const char* buffersize = getenv("KAAPI_CUDA_TRACE_BUFFER");
  
  if(buffersize != 0)
  {
    long size = atol(buffersize);
    if(size > 0)
    {
      kaapi_cuda_trace_buffer_size = BUFFER_SIZE * size;
    }
    else
    {
      kaapi_cuda_trace_buffer_size = BUFFER_SIZE;
      fprintf(stderr,"%s:%d:%s: *** Kaapi: ERROR invalid buffer size for '%s' (%s)\n",
              __FILE__, __LINE__, __FUNCTION__,
              "KAAPI_CUDA_TRACE_BUFFER", buffersize
            );
      fflush(stderr);
    }
  }
  else
  {
    /* default buffer size is 1MB */
    kaapi_cuda_trace_buffer_size = BUFFER_SIZE;
  }

  /* library init */
  kaapi_cuda_trace_add_buffer(NULL, 0);

  res = cuptiActivityEnable(CUPTI_ACTIVITY_KIND_DEVICE);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiActivityEnable ERROR %d\n", __FUNCTION__,
	    res);
    fflush(stdout);
  }

  res = cuptiSubscribe(&subscriber,
		       (CUpti_CallbackFunc) kaapi_cuda_trace_callback,
		       NULL);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiSubscribe ERROR %d\n", __FUNCTION__, res);
    fflush(stdout);
  }

  res = cuptiEnableDomain(1, subscriber, CUPTI_CB_DOMAIN_SYNCHRONIZE);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiEnableDomain ERROR %d\n", __FUNCTION__, res);
    fflush(stdout);
  }
  res = cuptiEnableDomain(1, subscriber, CUPTI_CB_DOMAIN_RESOURCE);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiEnableDomain ERROR %d\n", __FUNCTION__, res);
    fflush(stdout);
  }
#if 0
  cuptiActivityEnable(CUPTI_ACTIVITY_KIND_CONTEXT);
  cuptiActivityEnable(CUPTI_ACTIVITY_KIND_DRIVER);
  cuptiActivityEnable(CUPTI_ACTIVITY_KIND_RUNTIME);
  cuptiActivityEnable(CUPTI_ACTIVITY_KIND_MEMSET);
#endif
  cuptiActivityEnable(CUPTI_ACTIVITY_KIND_MEMCPY);
  cuptiActivityEnable(CUPTI_ACTIVITY_KIND_KERNEL);
  cuptiGetTimestamp(&kaapi_default_param.cudastartuptime);

  return 0;
}

int kaapi_cuda_trace_thread_init(void)
{
  return 0;
}

void kaapi_cuda_trace_finalize(void)
{
  while (kaapi_cuda_trace_dump(NULL, 0) != NULL);
}

void kaapi_cuda_trace_thread_finalize(void)
{
  CUptiResult res;
  CUcontext ctx;
  uint32_t stream_id;
  
  kaapi_cuda_ctx_set_(kaapi_cuda_self_device());
  
  cuCtxGetCurrent(&ctx);
  res = cuptiGetStreamId(ctx, (CUstream) kaapi_cuda_HtoD_stream(),
                         &stream_id);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiGetStreamId ERROR %d\n", __FUNCTION__, res);
    fflush(stdout);
  }
  cudaStreamSynchronize(kaapi_cuda_HtoD_stream());
  while (kaapi_cuda_trace_dump(ctx, stream_id) != NULL);
  
  res = cuptiGetStreamId(ctx, (CUstream) kaapi_cuda_DtoH_stream(),
                         &stream_id);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiGetStreamId ERROR %d\n", __FUNCTION__, res);
    fflush(stdout);
  }
  cudaStreamSynchronize(kaapi_cuda_DtoH_stream());
  while (kaapi_cuda_trace_dump(ctx, stream_id) != NULL);
  
  res = cuptiGetStreamId(ctx, (CUstream) kaapi_cuda_kernel_stream(),
                         &stream_id);
  if (res != CUPTI_SUCCESS) {
    fprintf(stdout, "%s: cuptiGetStreamId ERROR %d\n", __FUNCTION__, res);
    fflush(stdout);
  }
  cudaStreamSynchronize(kaapi_cuda_kernel_stream());
  while (kaapi_cuda_trace_dump(ctx, stream_id) != NULL);
  
  kaapi_cuda_ctx_exit_(kaapi_cuda_self_device());
}

#endif
