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

   Runs in approximately 2/3 the time of gsl_ran_choose when using the
   simple Algorithm A.
 */
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
