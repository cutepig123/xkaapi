#include <stdio.h>
#include <stdlib.h>

/** The macro FIBOCODE and MAIN CODE are used to allows separate compilation
    of the fibo code and the main code.
*/
#if defined(__cplusplus)
extern "C" {
#endif
extern double kaapi_get_elapsedtime();
#if defined(__cplusplus)
}
#endif

#if defined(FIBOCODE)
/* Sequential fibo function
*/
long fiboseq( const long n ) {
    if( n<2 )
        return n;
    else
        return fiboseq(n-1)+fiboseq(n-2);
}

#else
extern long fiboseq(long);
#endif

#if defined(MAINCODE)
int main(int argc, char** argv)
{
  int i;
  int n;
  int niter;
  long result = 0;
  double t0, t1;

  if (argc >1)
    n = atoi(argv[1]);
  else 
    n = 20;
  if (argc >2)
    niter =  atoi(argv[2]);
  else 
    niter = 1;
    
  t0 = kaapi_get_elapsedtime();
  {
    for (i=0; i<niter; ++i)
    {
      result += fiboseq(n);
    }
  }
  t1 = kaapi_get_elapsedtime();
/*  printf("Fibo(%i) = %i *** Time: t1=%e(s), t0=%e(s)\n", n, result, t1,t0 );*/
  printf("Fibo(%i) = %li *** Time(s): %e\n", n, result/niter, (t1-t0)/(double)niter );
  return 0;
}
#endif
