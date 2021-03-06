/*
** xkaapi
** 
** Copyright 2010 INRIA.
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
#ifndef _TAKNET_DEVICE_H_
#define _TAKNET_DEVICE_H_

#include "kanet_device.h"

extern "C" {
#include <taktuk.h>
}
#define TAKNET_OK 0
#define TAKNET_SAFE(err) kaapi_assert( (err) == TAKNET_OK)

namespace TAKNET {

class OutChannel;

// --------------------------------------------------------------------
/** \name Channel
    \ingroup Net
    A Device object should be implemented in order to be able to create Channel
    object over a specific transport protocol (tcp, myrinet, mpi, ib, taktuk,...)
    A device is dynamicly linked to the process in order to select the right device
    online : more over, if the network is not used, then the code is not loaded.
*/
class Device : public ka::Device {
public:
  /** Constructor 
  */
  Device();

  /** Destructor 
  */
  ~Device();

  /**
  */
  int initialize(int* argc, char*** argv);

  /**
  */
  int commit();

  /** 
  */
  int terminate();

  /** 
  */
  int finalize();

  /**
  */
  void poll();
  
  /**
  */
  void barrier();

  /**
  */
  void* allocate( size_t size );
  
  /**
  */
  void deallocate( void* addr );

  /** 
  */
  void* bind( uintptr_t addr, size_t size );

  /**
  */  
  ka::SegmentInfo get_seginfo( ka::GlobalId gid ) const;

  /** 
  */
  ka::OutChannel* open_channel( const char* url );
  
  /**
  */
  int close_channel( ka::OutChannel* channel);

  /** 
  */
  const char* get_urlconnect( ) const;

protected:
  /* thread entry point */
  static void* skeleton( void* arg );
  int skel( );

  /* Kaapi level service */
  static void service_term(int err, ka::GlobalId source, void* buffer, size_t sz_buffer );
  static void service_barrier(int err, ka::GlobalId source, void* buffer, size_t sz_buffer );
  static void service_ackbarrier(int err, ka::GlobalId source, void* buffer, size_t sz_buffer );

  enum {
    service_term_id       = 8,
    service_barrier_id    = 9,
    service_ackbarrier_id = 10
  };

  friend class OutChannel;
protected:
  unsigned long             _wcom_rank;  ///< my rank 
  unsigned long             _wcom_size;  ///< number of nodes
  void*                     _segaddr;    ///< Segment base address
  uintptr_t                 _segsize;    ///< Segment size
  uintptr_t                 _segsp;      ///< next position for allocation
  ka::atomic_t<32>          _state;      ///< state of the device
  volatile bool             _ack_term;   ///< On node 0
  pthread_t                 _tid;        ///< Thread that wait incomming message
    
  enum DeviceState {
    S_CREATE,
    S_INIT,
    S_TERM,
    S_FINISHED,
    S_ERROR
  };

  /* static field for barrier */
  static size_t _waiting;   /* number of process that have reach the barrier */
  static size_t _count;     /* number of process that have reach the barrier */
  static size_t _recv;      /* number of ack from barrier */
  static pthread_mutex_t _posix_mutex;
  static pthread_cond_t  _posix_cond;
  
}; // -- end class device


} // end namespace 
#endif 
