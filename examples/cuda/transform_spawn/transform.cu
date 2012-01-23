/*
** xkaapi
** 
** Created on Tue Mar 31 15:19:14 2009
** Copyright 2009 INRIA.
**
** Contributors :
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
#include <stdint.h>
#include <sys/types.h>
#include <cuda.h>
#include "kaapi++"

#include <unistd.h>

// missing decls
///typedef uintptr_t kaapi_mem_addr_t;
//extern "C" void kaapi_mem_delete_host_mappings(kaapi_mem_addr_t, size_t);
typedef float double_type;

struct TaskHello: public ka::Task<1>::Signature< int >{};

template<> struct TaskBodyCPU<TaskHello>
{
    void operator()( int i )
    {
	fprintf( stdout,"HELLO CPU\n" );
	fflush(stdout);
    }
};

template<> struct TaskBodyGPU<TaskHello>
{
    void operator()( ka::gpuStream stream, int i )
    {
	fprintf( stdout,"HELLO CUDA\n" );
	fflush(stdout);
    }
};

///////////////////////////////////////////////////////////////////////////////

// task signature
struct TaskAddone : public ka::Task<1>::Signature<ka::RW<ka::range1d<double_type> > > {};

// cuda kernel
__global__ void addone(double_type* array, unsigned int size)
{
  const unsigned int per_thread = size / blockDim.x;
  unsigned int i = threadIdx.x * per_thread;

  unsigned int j = size;
  if (threadIdx.x != (blockDim.x - 1)) j = i + per_thread;

  for (; i < j; ++i) ++array[i];
}

template<> struct TaskBodyGPU<TaskAddone>
{
  void operator()(ka::gpuStream stream, ka::range1d_rw<double_type> range)
  {
    fprintf(stdout,"gpuTask (%lx, %lu)\n",
	   (uintptr_t)range.begin(), range.size() );
    fflush(stdout);

    const CUstream custream = (CUstream)stream.stream;
    addone<<<1, 256, 0, custream>>>(range.begin(), range.size());

//    ka::Spawn<TaskHello>() (1);
  }
};

// cpu implementation
template<> struct TaskBodyCPU<TaskAddone>
{
  void operator() (ka::range1d_rw<double_type> range)
  {
    const size_t range_size = range.size();
    fprintf(stdout,"cpuTask (%lx, %lu)\n",
	   (uintptr_t)range.begin(), range.size() ); 
    fflush(stdout);
    for (size_t i = 0; i < range_size; ++i)
      range[i] += 1;

//    ka::Spawn<TaskHello>() (1);
  }
};

struct TaskAddoneMain : public ka::Task<1>::Signature<ka::RW<ka::range1d<double_type> > > {};

// cpu implementation
template<> struct TaskBodyCPU<TaskAddoneMain>
{
  void operator() (ka::range1d_rw<double_type> range)
  {
    const size_t range_size = range.size();
    const size_t nb_size = range_size / 8;

    fprintf(stdout,"cpuTaskMAIN (%lx, %lu)\n",
	   (uintptr_t)range.begin(), range.size());
    fflush(stdout);

    for( size_t pos= 0; pos < range_size; pos+= nb_size ){
    	ka::range1d<double_type> l(range.begin()+pos, nb_size);
	ka::Spawn<TaskAddone>()( l );
    }
  }
};

// main task
struct doit {
  void operator()(int argc, char** argv)
  {
    double t0,t1;
    double sum = 0.f;
    size_t size = 100000;
    if (argc >1) size = atoi(argv[1]);
    
    double_type* array = (double_type*) calloc(size, sizeof(double_type));
    fprintf(stdout,"MAIN array=%p\n", array);fflush(stdout);

    for (int iter = 0; iter < 1; ++iter)
    {
      // initialize, apply, check
      for (size_t i = 0; i < size; ++i)
        array[i] = 0.f;
        
      t0 = kaapi_get_elapsedns();

      // fork the root task
      ka::range1d<double_type> range(array, size);
      //ka::array<1,double_type> A(array, size);
       ka::Spawn<TaskAddoneMain>()(range);
//       ka::Spawn<TaskAddoneMain>(ka::SetStaticSched())(range);
      //ka::Spawn<TaskAddone>()(range);
      //waitabit(); // gpu task scheduled
      ka::Sync();
    kaapi_memory_synchronize();

      //kaapi_mem_delete_host_mappings
	//((kaapi_mem_addr_t)array, sizeof(double_type) * size);

      t1 = kaapi_get_elapsedns();

      sum += (t1-t0)/1000; // ms

      for (size_t i = 0; i < size; ++i)
      {
        if (array[i] != 1.f)
        {
          std::cout << "ERROR invalid @" << i << " == " << array[i] << std::endl;
          break ;
        }
      }
    }

    std::cout << "Done " << sum/100 << " (ms)" << std::endl;
  }
};


/* main entry point : Kaapi initialization
*/
int main(int argc, char** argv)
{
  try {
    /* Join the initial group of computation : it is defining
       when launching the program by a1run.
    */
    ka::Community com = ka::System::join_community( argc, argv );
    
    /* Start computation by forking the main task */
    ka::SpawnMain<doit>()(argc, argv); 
    
    /* Leave the community: at return to this call no more athapascan
       tasks or shared could be created.
    */
    com.leave();

    /* */
    ka::System::terminate();
  }
  catch (const std::exception& E) {
    ka::logfile() << "Catch : " << E.what() << std::endl;
  }
  catch (...) {
    ka::logfile() << "Catch unknown exception: " << std::endl;
  }
  
  return 0;
}
