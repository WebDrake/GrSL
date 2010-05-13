/* sampling/sampling.c
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

#include <config.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sampling.h>

gsl_sampler *
gsl_sampler_alloc(const gsl_sampling_algorithm *A)
{
  gsl_sampler *s = malloc(sizeof(gsl_sampler));

  if (s == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for sampler struct",
                     GSL_ENOMEM, 0);
    };

  s->state = malloc(A->size);

  if (s->state == 0)
    {
      free (s);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for sampler state",
                     GSL_ENOMEM, 0);
    };

  s->records = malloc(sizeof(gsl_sampling_records));

  if (s->records == 0)
    {
      free(s->state);
      free(s);

      GSL_ERROR_VAL ("failed to allocate space for records",
                     GSL_ENOMEM, 0);
    }

  s->sample = malloc(sizeof(gsl_sampling_records));

  if (s->sample == 0)
    {
      free(s->records);
      free(s->state);
      free(s);
      GSL_ERROR_VAL ("failed to allocate space for sample",
                     GSL_ENOMEM, 0);
    }

  s->algorithm = A;

  s->sample->remaining = s->sample->total = s->records->remaining = s->records->total = 0;

  return s;
}

void
gsl_sampler_free(gsl_sampler * s)
{
  RETURN_IF_NULL(s);
  free(s->sample);
  free(s->records);
  free(s->state);
  free(s);
}

int
gsl_sampler_init(const gsl_sampler * s, const gsl_rng *r, size_t sample_size,
                 size_t records)
{
  if ( sample_size > records )
    {
      GSL_ERROR ("Sample size cannot be greater than the number of records!",
                 GSL_EINVAL) ;
    }

  s->sample->remaining = s->sample->total = sample_size;
  s->records->remaining = s->records->total = records;
  (s->algorithm->init) (s->state, s->sample, s->records, r);

  return GSL_SUCCESS;
}

/* Include a copy of ... well, copy ... for gsl_sampler_choose to use.
   TODO: check if you can actually use the one in gsl's randist/shuffle.c.
 */
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

/* Reimplements the gsl_ran_choose function in randist/shuffle.c of the
   GSL to use GrSL sampling algorithms to select the chosen subset.

   Assuming that sample size n is << the number N of records, this runs
   in approximately 2/3 the time of gsl_ran_choose when using the simple
   Algorithm A.  (Both algorithms are O(N) in running time but Algorithm
   A requires only n random variates to be generated.)
 */
int
gsl_sampler_choose(const gsl_sampler * s, const gsl_rng * r, void * dest,
                   size_t k, void * src, size_t n, size_t size)
{
  size_t i, current_record = 0, selected_record;

  if ( k > n )
    {
      GSL_ERROR ("k is greater than n, cannot sample more than n items",
                 GSL_EINVAL) ;
    }

  gsl_sampler_init(s, r, k, n);

  for(i=0;i<k;++i)
    {
      selected_record = gsl_sampler_select(s, r, &current_record);
      copy(dest, i, src, selected_record, size);
    }

  return GSL_SUCCESS;
}
