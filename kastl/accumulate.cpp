/*
 *  test_accumulate.cpp
 *  xkaapi
 *
 *  Created by TD on Avril 09.
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */

#include "kaapi_adapt.h"
#include <sys/time.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <algorithm>
#include "accumulate.h"
#include "random.h"


/** Return the number of seconds + micro seconds since the epoch
*/
inline double gettime()
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  return (double)(tp.tv_sec) + (double)(tp.tv_usec)*1e-6;
}

typedef double val_t;

/* The main thread
*/
int main(int argc, char** argv)
{
  int cpu, l, n, iter;
  double t0, t1;
  double avrg = 0;

  if (argc >1) n = atoi(argv[1]);
  else n = 10000;
  if (argc >2) cpu = atoi(argv[2]);
  else cpu = 1;
  if (argc >3) iter = atoi(argv[3]);
  else iter = 1;

  t0 = gettime();
  double* input  = new double[n];
  t0 = gettime() - t0;

  double res; // result of the computation 

  std::cout << "Time allocate:" << t0 << std::endl;
  Random random(42 + iter);


  /* */
  std::cout << "Main thread has stack id:" << kaapi_adapt_current_stack()->_index << std::endl;


  kaapi_steal_stack_t* current_stack = kaapi_adapt_current_stack();
  
  /* Declare the context to steal work of the sequential computation
     The stack size for storing steal requests (both input and input) is at most 4096 bytes.
     In case of steal request with no enough memory, then the steal request fails.
  */
  kaapi_steal_context_declare( stealcontext, CKAAPI_STEALCONTEXT_STACKSIZE, malloc(CKAAPI_STEALCONTEXT_STACKSIZE) );

  /* push the context to be visible from outside control */
  kaapi_steal_context_push( current_stack, &stealcontext);

  /* set the concurrency number */
  kaapi_adapt_setconcurrency( cpu );
  
  usleep(100000);

  double* ti = new double[iter];
  t0 = gettime();
  //suite de valeurs necessaires pour le calcul de ln(2)
  for(int i = 0; i < n; ++i) {
    input[i] =  val_t(1)/val_t(i+1) ;
    if(i%2) input[i] = -input[i];
  }

  //suite de valeurs necessaires pour le calcul de ln(2)
  //une serie qui converge plus vite, mais instable numeriquement
  /*int x;
  for(int i = 0; i < n; ++i) {
    x = 1 << (i+1);
    input[i] =  val_t(1)/val_t((i+1)*x) ;
  }
  std::cout << "input[n-1] = " << input[n-1] << std::endl;
  */

  t0 = gettime() - t0;
  std::cout << "Time init:" << t0 << std::endl;


  t0 = gettime();
  for (l=0; l<iter; ++l)
  {
    res = accumulate(&stealcontext, input, input+n, val_t(0));
  }
  t0 = (gettime() -t0);
  avrg = t0;
  avrg /= (double)iter;
  delete [] ti;

   std::cout.precision(9);
   std::cout.setf(std::ios::fixed, std::ios_base::floatfield);
   // Verification of the result
   std::cout << "res = ln(2) = " << res << std::endl;
   double res_stl = std::accumulate(input, input+n, val_t(0));
   std::cout << "res_stl = ln(2) = " << res_stl << std::endl;
   if(res-res_stl < 1e-6 || res_stl-res < 1e-6) std::cout << "Verification OK!!!!" << std::endl; 
   else std::cout << "Verification failed, KO!!!!!!!!!!!!" << std::endl;
   //std::cout.setf(0, std::ios_base::floatfield);


  std::cout << "Result-> cpu:" << cpu << "  size: " << n << "  time: " << avrg << std::endl;

  delete [] input;

  //exit(1);
  /* Pop the steal context
  */
  kaapi_steal_context_pop( current_stack );
  
  /* Destroy the context 
  */
  kaapi_steal_context_destroy( &stealcontext );

  return 0;
}
