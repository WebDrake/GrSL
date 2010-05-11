/* sampling/vitter_a.c
 *
 * ---------------------------------------------------------------------
 * Provides an implementation of 'Algorithm A' described in
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

static size_t
vitter_a_skip(const gsl_rng * r, size_t * const remaining_records,
              size_t * const remaining_samples)
{
  size_t S;
  double top, V, quot;

  if (*remaining_samples == 1)
    {
      S = gsl_rng_uniform_int(r, *remaining_records);
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

  --(*remaining_records);
  --(*remaining_samples);

  return S;
}

static const gsl_sampler vitter_a = {"vitter_a", &vitter_a_skip};

const gsl_sampler *gsl_sampler_vitter_a = &vitter_a;
