#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <config.h>
#include <gsl/gsl_sampling.h>

int main(void)
{
  unsigned long int N, n, current_record, selected_record;
  gsl_rng *r = gsl_rng_alloc(gsl_rng_mt19937);

  gsl_rng_set(r,time(NULL));

  printf("Hello! I'm GrSL, the GSL random Sampling Library.\n\n");

  printf("First, I'm going to make a sample of 5 records out of 10.\n");

  N = 10;
  n = 5;
  current_record = 1;

  while ( n > 0 )
    {
      selected_record = gsl_sampler_select(gsl_sampler_vitter_a,r,&current_record,&N,&n);
      printf("\tselected record %lu.",selected_record);
      printf("\trecords remaining: %lu.",N);
      printf("\tremaining to select: %lu.\n",n);
    }

  return EXIT_SUCCESS;
}
