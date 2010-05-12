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

void grsl_test_simple(const gsl_sampler *s, const gsl_rng *r, size_t n, size_t N)
{
  size_t i, current_record, selected_record;

  gsl_sampler_init(s,r,5,10);
  current_record = 1;

  printf("%s, %zu from %zu:\n",s->algorithm->name, n, N);

  for ( i = 0 ; i < s->sample->total ; ++i)
    {
      selected_record = gsl_sampler_select(s,r,&current_record);
      printf("\tselected record %zu.",selected_record);
      printf("\trecords remaining: %zu.",s->records->remaining);
      printf("\tremaining to select: %zu.\n",s->sample->remaining);
    }


}

void grsl_test_aggregate(const gsl_sampler *s, const gsl_rng *r, size_t n, size_t N,
                         size_t repeats)
{
  size_t i, j, current_record, selected_record;
  size_t *record_count;
  clock_t start_time, end_time;

  record_count = malloc(N*sizeof(size_t));

  for(i=0;i<N;++i)
    record_count[i] = 0;

  printf("%s, %zu from %zu x %zu:\n",s->algorithm->name, n, N, repeats);

  start_time = clock();

  for(i=0;i<repeats;++i)
    {
      gsl_sampler_init(s,r,n,N);
      current_record = 0;  /* note that we start with record 0 this time,
                              as we are going to be picking from an array ... */

      for ( j = 0 ; j < s->sample->total ; ++j)
        {
          selected_record = gsl_sampler_select(s,r,&current_record);
          record_count[selected_record]++;
        }
    }

  end_time = clock();

  for(i=0;i<N;++i)
    printf("\trecord %zu was picked %zu times.\n",i+1,record_count[i]);

  printf("\t\tsampling completed in %g seconds.\n",
         ((double) (end_time-start_time))/CLOCKS_PER_SEC);

  free(record_count);
}

int main(void)
{
  size_t i;
  gsl_sampler *s = gsl_sampler_alloc(gsl_sampler_vitter_a);
  gsl_rng *r = gsl_rng_alloc(gsl_rng_mt19937);
  double *dest, *src;
  clock_t start_time, end_time;

  gsl_rng_set(r,time(NULL));

  printf("Hello! I'm GrSL, the GSL random Sampling Library.\n\n");

  printf("Basically, I'm a cheeky little scamp from out of town with dreams of\n");
  printf("joining the GNU Scientific Library when I grow up.\n\n");

  printf("Let me show you what I can do so far.\n\n");

  printf("First, I'm going to make a sample of 5 records out of 10.\n\n");

  grsl_test_simple(s, r, 5, 10);

  printf("\n");
  printf("Now, I'm going to make a sample of 3 records out of 10, but do so\n");
  printf("10 million times.  Then you can count how many times each record gets\n");
  printf("picked.  (This is just a stupid way of checking for obvious bias:-)\n\n");

  grsl_test_aggregate(s, r, 3, 10, 10000000);

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

  printf("\t\tfinished in %g seconds.\n",
         ((double) (end_time-start_time))/CLOCKS_PER_SEC);

  printf("\tgsl_sampler_choose:\n");

  start_time = clock();
  gsl_sampler_choose(s, r, dest, 100000, src, 10000000, sizeof(double));
  end_time=clock();

  printf("\t\tfinished in %g seconds with %s.\n",
         ((double) (end_time-start_time))/CLOCKS_PER_SEC, s->algorithm->name);

  free(dest);
  free(src);

  gsl_sampler_free(s);
  gsl_rng_free(r);

  return EXIT_SUCCESS;
}
