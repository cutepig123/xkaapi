#include "kaapi++"
#include <iostream>


// --------------------------------------------------------------------
struct TaskRW: public ka::Task<1>::Signature<ka::RW<int> > {};
template<>
struct TaskBodyCPU<TaskRW> {
  void operator() ( ka::pointer_rw<int> d )
  {
    std::cout << "In Task RW=" << *d << ", @:" << (int*)d << std::endl;
    *d += 10;
  }
};


/* Main of the program
*/
struct doit {
  void operator()(int argc, char** argv )
  {
    std::cout << "My pid=" << getpid() << std::endl;

    ka::ThreadGroup threadgroup( 2 );
    ka::auto_pointer<int> a      = ka::Alloca<int>(1);

    threadgroup.begin_partition();

    threadgroup.Spawn<TaskRW> (ka::SetPartition(0))  ( a );
    threadgroup.Spawn<TaskRW> (ka::SetPartition(1))  ( a );
    
    threadgroup.end_partition();

    threadgroup.execute();
  }
};


/*
*/
int main( int argc, char** argv ) 
{
  try {
    ka::Community com = ka::System::join_community( argc, argv );
    
    ka::SpawnMain<doit>()(argc, argv); 
          
    com.leave();

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
