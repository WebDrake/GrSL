/* sampling/gsl_sampling.h
 *
 * Copyright (C) 2010 Joseph Rushton Wakeling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * --REFERENCES--
 *
 *   Nair KA (1990) 'An improved algorithm for ordered sequential
 *     random sampling.'  ACM T. Math. Softw. 16(3): 269--274.
 *
 *   Vitter JS (1984) 'Faster methods for random sampling.'
 *     Commun. ACM 27(7): 703--718
 *
 *   Vitter JS (1987) 'An efficient algorithm for sequential random
 *     sampling.'  ACM T. Math. Softw. 13(1): 58--67.
 *
 */

#ifndef __GSL_SAMPLING_H__
#define __GSL_SAMPLING_H__
#include <gsl/gsl_types.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_rng.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS


typedef struct
  {
    const char *name;
    unsigned long int (*skip) (const gsl_rng *r,
                               unsigned long int * const remaining_records,
                               unsigned long int * const remaining_samples);
  }
gsl_sampler;


GSL_VAR const gsl_sampler *gsl_sampler_vitter_a;
GSL_VAR const gsl_sampler *gsl_sampler_vitter_d;
GSL_VAR const gsl_sampler *gsl_sampler_nair_e;


#ifdef HAVE_INLINE

INLINE_FUN unsigned long int
gsl_sampler_skip (const gsl_sampler *s,
                  const gsl_rng *r,
                  unsigned long int * const remaining_records,
                  unsigned long int * const remaining_samples)
{
  if (*remaining_records == 0)
    {
      GSL_ERROR_VAL ("number of remaining_records is 0.",
                     GSL_EINVAL, 0) ;
      return 0;
    }
  else if (*remaining_samples == 0)
    {
      GSL_ERROR_VAL ("number of remaining_samples is 0.",
                     GSL_EINVAL, 0) ;
      return 0;
    }
  return (s->skip) (r, remaining_records, remaining_samples);
}

INLINE_FUN unsigned long int
gsl_sampler_select(const gsl_sampler *s,
                   const gsl_rng *r,
                   unsigned long int * const current_record,
                   unsigned long int * const remaining_records,
                   unsigned long int * const remaining_samples)
{
  *current_record += gsl_sampler_skip(s, r, remaining_records, remaining_samples);
  return *current_record++;
}

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_SAMPLING_H__ */
