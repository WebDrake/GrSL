/* sampling/vitter.c
 *
 * ---------------------------------------------------------------------
 * Provides an implementation of Algorithms A and D introduced by
 * Jeffrey Scott Vitter in the following articles:
 *
 *   Vitter JS (1984) 'Faster methods for random sampling'.
 *     Commun. ACM 27(7): 703--718
 *
 *   Vitter JS (1987) 'An efficient algorithm for sequential random
 *     sampling.'  ACM T. Math. Softw. 13(1): 58--67.
 * ---------------------------------------------------------------------
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

#include <gsl/gsl_rng.h>
#include <gsl/gsl_sampling.h>

/* Vitter (1984) introduces Algorithm A as a component part of the
   still-more-efficient Algorithm D.

   By itself it runs in O(N) time, where N is the total number of
   records, but generates only n random variates, where n is the
   number of records to be sampled.

   Algorithm D uses this method when the number of remaining samples
   to be taken is greater than a certain proportion (0.05--0.15) of
   the total number of remaining records to be sampled from.

   --NOTES--

     * Use of gsl_rng_uniform_pos to set value of V.  Vitter
       (1984, 1987) assumes random variates are in the open interval
       (0,1).  It is not clear whether this is essential.  If it is
       not required a (miniscule:-) performance improvement may be
       possible here.

     * Use of gsl_rng_uniform_int if the number of remaining samples
       is only 1.  Vitter (1984) simply calls for the truncation
       (floor?) of the product of a random variate in (0,1) and the
       number N of remaining records.  Vitter (1987) gives a more
       complicated expression,

           S := TRUNC( ROUND(Nreal) * UNIFORMRV() )

       (where Nreal is a real-valued variable equal to the integer N),
       in order to avoid the out-of-range possibility of S being
       exactly equal to N.  All these appear to merely be complicated
       ways of describing generating a uniformly-distributed integer
       in the range [0, N-1], which is what gsl_rng_uniform_int
       provides.

     * Alteration of *remaining_records and *remaining_samples -- see
       if this can be removed entirely outside these individual skip
       functions and placed entirely in gsl_sampler_skip.
 */
static size_t
vitter_a_skip(const gsl_rng * r, size_t * const remaining_records,
              size_t * const remaining_samples)
{
  size_t S;
  double top, V, quot;

  if (*remaining_samples == 1)
    {
      S = gsl_rng_uniform_int(r, *remaining_records);
      *remaining_records -= S;
    }
  else
    {
      S = 0;
      top = *remaining_records - *remaining_samples;
      V = gsl_rng_uniform_pos(r);
      quot = top/(*remaining_records);

      while (quot > V)
        {
          ++S;
          --top;
          --(*remaining_records);
          quot *= top/(*remaining_records);
        }
    }

  return S;
}

static const gsl_sampler vitter_a = {"vitter_a", &vitter_a_skip};

const gsl_sampler *gsl_sampler_vitter_a = &vitter_a;

/* Algorithm D, introduced in Vitter (1984), requires only ~n random
   variates to be generated and runs in O(n) time.  This implementation
   follows the more efficient version introduced in Vitter (1987).

   The method calls Algorithm A to generate the skip size when the
   number of remaining samples to be taken is greater than a certain
   proportion alpha of the total number of remaining records.  This
   implementation follows Vitter (1987) in taking alpha = 1/13.

   A still more refined version is found in Algorithm E introduced by
   Nair (1990).

   --NOTES--

     * An option to set alpha (or alpha_inverse) could be useful.
       This could be a reason to resurrect the more complicated
       gsl_sampler implementation modelled on the gsl_rng example,
       with a sampler containing some void *state which can be set
       with a config function.

     * ... in any case, test with different values of alpha_inverse.
 */
