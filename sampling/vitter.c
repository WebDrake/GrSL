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

#include <math.h>
#include <stdbool.h>
#include <gsl/gsl_randist.h>
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
 */
void
vitter_a_init(void * vstate, const gsl_sampling_records * const sample,
              const gsl_sampling_records * const records, const gsl_rng *r)
{
  /* Algorithm A does not require any initialisation :-)
     For the same reason, no vitter_a_state_t is defined.
   */
}

/*
   A few notes on the implementation of the Algorithm A skip function:

     * Use of gsl_rng_uniform_pos to set value of V.  Vitter (1984,
       1987) assumes random variates are in the open interval (0,1).
       The requirement from this seems to stem (in Algorithm A at
       least) from the fact (AFAICS, do not trust me on this one:-)
       that when top == 0 (and hence quot==0), it is _essential_ to
       pick the next record, i.e. we cannot allow the rare but
       possible case that V==0.

         -- Note that this could allow a saving in the number of
            required random variates by including an if(top>0)
            statement.  However, the cost of the continually-being-
            checked if() statement turns out to be greater than the
            savings from the (apparently rare) cases when top==0.

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

     * Alteration of s->records->remaining and s->samples->remaining:
       see if this can be removed entirely outside these individual
       skip functions and placed entirely in gsl_sampler_skip.
 */
static size_t
vitter_a_skip(void * vstate, gsl_sampling_records * const sample,
              gsl_sampling_records * const records, const gsl_rng *r)
{
  register size_t S;
  register double V, quot, top;

  if (sample->remaining == 1)
    {
      S = gsl_rng_uniform_int(r, records->remaining);
    }
  else
    {
      S = 0;
      top = records->remaining - sample->remaining;
      quot = top/(records->remaining);
      V = gsl_rng_uniform_pos(r);

      while (quot > V)
        {
          ++S;
          quot *= (top - S) / (records->remaining - S);
        }
    }

  return S;
}

static const gsl_sampling_algorithm vitter_a =
{"vitter_a",                  /* name */
 0,                           /* size */
 &vitter_a_init,              /* init */
 &vitter_a_skip               /* skip */
};

const gsl_sampling_algorithm *gsl_sampler_vitter_a = &vitter_a;


/* Algorithm D, introduced in Vitter (1984), requires only ~n random
   variates to be generated and runs in O(n) time.  This implementation
   follows the more efficient version introduced in Vitter (1987).

   The method calls Algorithm A to generate the skip size when the
   number of remaining samples to be taken is greater than a certain
   proportion alpha of the total number of remaining records.  This
   implementation follows Vitter (1987) in taking alpha = 1/13.

   Brief and entirely inadequate testing on the present author's part
   suggests that this is indeed an optimal choice. :-)

   The algorithm was further refined by Nair (1990) whose Algorithm E
   takes advantage of some cases where skip values of Algorithm A can
   be calculated exactly in a single step.
 */
typedef struct
  {
    double Vprime;
    bool use_algorithm_a;
  }
vitter_d_state_t;

/* As per Vitter (1984, 1987) we do not store the floating-point value
   of alpha but store an integer value that is 1/alpha.  Following
   Vitter's suggestion this is set to 13. */
static const short int vitter_d_alpha_inverse = 13;

/* Algorithm D stores two pieces of information: the random variate
   Vprime, which must be preserved between calls to the skip function,
   and a boolean to indicate whether or not to generate remaining
   skip values using Algorithm A. */
void
vitter_d_init(void * vstate, const gsl_sampling_records * const sample,
              const gsl_sampling_records * const records, const gsl_rng *r)
{
  vitter_d_state_t *state = vstate;

  /* We can save ourselves one random variate by checking at the very
     beginning whether or not the sample size is larger than the threshold
     to start using Algorithm A. */
  if ( (vitter_d_alpha_inverse * sample->remaining) > records->remaining )
    {
      state->use_algorithm_a = true;
    }
  else
    {
      state->Vprime = exp ( log(gsl_rng_uniform_pos(r)) / (sample->remaining) ) ;
      state->use_algorithm_a = false;
    }
}

/* Algorithm D's skip function employs some clever tricks to minimise
   the number of random variates that must be generated -- if we are
   lucky, the algorithm exits with a condition such that the next
   variate in the sequence can be calculated cheaply and numerically
   directly from the one before.

   In principle the log(gsl_rng_uniform_pos(r)) statements could be
   replaced with calls to the exponential distribution, but it's not
   clear this would be any more than simply a notational shortcut.
   There might also be some issues with finnicky details of the
   implementation that are different from what Vitter intends or
   assumes, so the safe choice is made here. :-)
 */
static size_t
vitter_d_skip(void * vstate, gsl_sampling_records * const sample,
              gsl_sampling_records * const records, const gsl_rng *r)
{
  size_t S;
  vitter_d_state_t *state = vstate;
  size_t top, t, limit;
  size_t qu1 = 1 + records->remaining - sample->remaining;
  double X, y1, y2, bottom;

  /* If the remainining number of sample points needed is greater than
     a certain proportion of the remaining records, we finish off using
     Algorithm A... */
  if ( (vitter_d_alpha_inverse * sample->remaining) > records->remaining )
    state->use_algorithm_a = true;

  /* ... like this. :-) */
  if ( state->use_algorithm_a )
    {
      return vitter_a_skip(NULL, sample, records, r);
    }
  else if ( sample->remaining > 1)
    {
      while ( 1 )
        {
          /* Step D2: set X and U */
          while ( 1 )
            {
              X = records->remaining * (1 - state->Vprime);
              S = trunc(X);
              if ( S < qu1 )
                break;
              else
                state->Vprime = exp ( log(gsl_rng_uniform_pos(r)) / (sample->remaining) );
            }

          y1 = exp ( log(gsl_rng_uniform_pos(r) * ((double) records->remaining)/qu1)
                       * (1.0/(sample->remaining - 1)) );

          state->Vprime
            = y1 * ((-X/records->remaining)+1.0) * ( qu1/( ((double) qu1) - S ) );

          /* Step D3: if state->Vprime <= 1.0 our work is done, otherwise ... */
          if ( state->Vprime > 1.0 )
            {
              y2 = 1.0;
              top = records->remaining - 1;

              if ( sample->remaining > (S+1) )
                {
                  bottom = records->remaining - sample->remaining;
                  limit = records->remaining - S;
                }
              else
                {
                  bottom = records->remaining - (S+1);
                  limit = qu1;
                }

              for ( t = (records->remaining - 1); t >= limit; --t)
                {
                  y2 = (y2 * top)/bottom;
                  --top;
                  --bottom;
                }

              /* Step D4: decide whether or not to go right back to the start
                 of this damn while() loop ... :-) */

              if( (records->remaining/(records->remaining - X))
                    < (y1 * exp(log(y2)/(sample->remaining - 1))) )
                {
                  /* If we're unlucky, we just have to generate a new Vprime
                     and go right back to the beginning.
                     printf("D4 fail.  "); fflush(stdout); */
                  state->Vprime
                    = exp ( log(gsl_rng_uniform_pos(r)) / (sample->remaining) ) ;
                }
              else
                {
                  /* If we're lucky, we accept S and generate a new Vprime ...
                     printf("D4 exit: %zu\n",S); fflush(stdout); */
                  state->Vprime
                    = exp ( log(gsl_rng_uniform_pos(r)) / (sample->remaining - 1) ) ;
                  return S;
                }
            }
          else
            {
              /* printf("D3 exit: %zu\n",S); fflush(stdout); */
              return S;
            }
        }
    }
  else
    {
      /* If only one sample point remains to be taken ... */
      return trunc ( records->remaining * state->Vprime );
    }
}

static const gsl_sampling_algorithm vitter_d =
{"vitter_d",                  /* name */
 sizeof(vitter_d_state_t),    /* size */
 &vitter_d_init,              /* init */
 &vitter_d_skip               /* skip */
};

const gsl_sampling_algorithm *gsl_sampler_vitter_d = &vitter_d;
