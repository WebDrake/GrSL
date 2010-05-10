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
 */

#ifndef __GSL_SAMPLING_H__
#define __GSL_SAMPLING_H__
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
                               const unsigned long int *remaining_records,
                               const unsigned long int *remaining_samples);
    unsigned long int (*select) (const gsl_rng *r,
                                 const unsigned long int *current_record,
                                 const unsigned long int *remaining_records,
                                 const unsigned long int *remaining_samples);
  }
gsl_sampler_type;


typedef struct
  {
    const gsl_sampling_type * type;
  }
gsl_sampler;


#ifdef HAVE_INLINE

INLINE_FUN unsigned long int
gsl_sampler_skip (const gsl_sampler *s,
                  const gsl_rng *r,
                  const unsigned long int *remaining_records,
                  const unsigned long int *remaining_samples)
{
  return (s->type->skip) (r, remaining_records, remaining_samples);
}

INLINE_FUN unsigned long int
gsl_sampler_select(const gsl_sampler *s,
                   const gsl_rng *r,
                   const unsigned long int *current_record,
                   const unsigned long int *remaining_records,
                   const unsigned long int *remaining_samples)
{
  *current_record += gsl_sampler_skip(s, r, remaining_records, remaining_samples);
  return *current_record++;
}

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_SAMPLING_H__ */
