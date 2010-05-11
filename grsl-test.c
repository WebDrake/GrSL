/* grsl-test.c
 *
 * ---------------------------------------------------------------------
 * A small and very daft little set of tests for GrSL's sampling
 * functions.
 * ---------------------------------------------------------------------
 *
 * Copyright (C) 2010 Joseph Rushton Wakeling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <config.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sampling.h>

/* Include a copy of ... well, copy ... for gsl_sampler_choose to use */
static inline void
copy (void * dest, size_t i, void * src, size_t j, size_t size)
{
  register char * a = size * i + (char *) dest ;
  register char * b = size * j + (char *) src ;
  register size_t s = size ;

  do
    {
      *a++ = *b++;
    }
  while (--s > 0);
}

int
gsl_sampler_choose(const gsl_sampler * s, const gsl_rng * r, void * dest,
                   size_t k, void * src, size_t n, size_t size)
{
  size_t k_ = k, N = n;
  size_t current_record = 0, selected_record;

  if ( k > n)
    {
      GSL_ERROR ("k is greater than n, cannot sample more than n items",
                 GSL_EINVAL) ;
    }

  while(k_ > 0)
    {
      selected_record = gsl_sampler_select(s, r, &current_record, &N, &k_);
      copy(dest, k-k_, src, selected_record, size);
    }

  return GSL_SUCCESS;
}

int main(void)
{
  size_t i, N, n, current_record, selected_record;
  size_t record_count[10];
  gsl_rng *r = gsl_rng_alloc(gsl_rng_mt19937);
  double *dest, *src;
  clock_t start_time, end_time;

  gsl_rng_set(r,time(NULL));

  printf("Hello! I'm GrSL, the GSL random Sampling Library.\n\n");

  printf("Basically, I'm a cheeky little scamp from out of town with dreams of\n");
  printf("joining the GNU Scientific Library when I grow up.\n\n");

  printf("Let me show you what I can do so far.\n\n");

  printf("First, I'm going to make a sample of 5 records out of 10.\n\n");

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

  printf("\n");
  printf("Now, I'm going to make a sample of 3 records out of 10, but do so\n");
  printf("10 million times.  Then you can count how many times each record gets\n");
  printf("picked.  (This is just a stupid way of checking for obvious bias:-)\n\n");

  for(i=0;i<10;++i)
    record_count[i] = 0;

  for(i=0;i<10000000;++i)
    {
      N = 10;
      n = 3;
      current_record = 0;  /* note that we start with record 0 this time,
                              as we are going to be picking from an array ... */

      while ( n > 0 )
        {
          selected_record =
            gsl_sampler_select(gsl_sampler_vitter_a,r,&current_record,&N,&n);
          record_count[selected_record]++;
        }
    }

  for(i=0;i<10;++i)
    printf("\trecord %lu was picked %lu times.\n",i+1,record_count[i]);

  printf("\n");
  printf("Next up, we provide a comparison of the gsl_ran_choose function with\n");
  printf("the new gsl_sampler_choose, which provides the same functionality but\n");
  printf("employing the new sampling algorithms provided by GrSL.\n\n");

  printf("We are going to pick 100,000 records from an array of 10 million.\n");

  src = malloc(10000000*sizeof(*src));
  dest = malloc(100000*sizeof(*dest));

  for(i=0;i<10000000;++i)
    src[i] = i+1;

  printf("\tgsl_ran_choose:\n");

  start_time = clock();
  gsl_ran_choose(r, dest, 100000, src, 10000000, sizeof(double));
  end_time = clock();

  printf("\t\tfinished in %lu clock ticks.\n",end_time-start_time);

  printf("\tgsl_sampler_choose:\n");

  start_time = clock();
  gsl_sampler_choose(gsl_sampler_vitter_a, r, dest, 100000, src, 10000000, sizeof(double));
  end_time=clock();

  printf("\t\tfinished in %lu clock ticks with gsl_sampler_vitter_a.\n",
         end_time-start_time);

  free(dest);
  free(src);

  gsl_rng_free(r);

  return EXIT_SUCCESS;
}
