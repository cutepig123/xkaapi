#include <stdio.h>
#include <omp.h>

void f(int a, int b)
{
}


int
main (int argc, char **argv)
{
  int a;
#pragma omp parallel num_threads(10)
  {
    int b;
    printf ("Hello world! %i\n", omp_get_thread_num());
    sleep(1);
  }

  return 0;
}