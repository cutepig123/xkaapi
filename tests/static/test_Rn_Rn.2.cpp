#include "kaapi++"
#include <iostream>


// --------------------------------------------------------------------
struct TaskR: public ka::Task<1>::Signature<ka::R<int> > {};
template<>
struct TaskBodyCPU<TaskR> {
  void operator() ( ka::pointer_r<int> d )
  {
    std::cout << "In Task R=" << *d << ", @:" << (int*)d << std::endl;
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
    *a = 123;
    ka::auto_pointer<int> b      = ka::Alloca<int>(1);
    *b = 246;

    threadgroup.begin_partition();

    threadgroup.Spawn<TaskR> (ka::SetPartition(0))  ( a );
    threadgroup.Spawn<TaskR> (ka::SetPartition(1))  ( a );
    threadgroup.Spawn<TaskR> (ka::SetPartition(0))  ( b );
    threadgroup.Spawn<TaskR> (ka::SetPartition(1))  ( a );
    threadgroup.Spawn<TaskR> (ka::SetPartition(0))  ( b );
    threadgroup.Spawn<TaskR> (ka::SetPartition(1))  ( b );
    threadgroup.Spawn<TaskR> (ka::SetPartition(0))  ( b );
    threadgroup.Spawn<TaskR> (ka::SetPartition(1))  ( a );

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
  catch (const ka::Exception& E) {
    ka::logfile() << "Catch : "; E.print(std::cout); std::cout << std::endl;
  }
  catch (...) {
    ka::logfile() << "Catch unknown exception: " << std::endl;
  }
  return 0;    
}
