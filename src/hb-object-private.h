/*
 * Copyright (C) 2007 Chris Wilson
 * Copyright (C) 2009  Red Hat, Inc.
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Contributor(s):
 *	Chris Wilson <chris@chris-wilson.co.uk>
 * Red Hat Author(s): Behdad Esfahbod
 */

#ifndef HB_REFCOUNT_PRIVATE_H
#define HB_REFCOUNT_PRIVATE_H


#ifndef HB_DEBUG_OBJECT
#define HB_DEBUG_OBJECT HB_DEBUG
#endif

#if HB_DEBUG_OBJECT
#include <stdio.h>
#define HB_OBJECT_DEBUG_OUT(obj) fprintf (stderr, "%p refcount=%d %s\n", \
					  obj, \
					  HB_REFERENCE_COUNT_GET_VALUE (obj->ref_count), \
					  __FUNCTION__)
#else
#define HB_OBJECT_DEBUG_OUT(obj) (void) 0
#endif


/* Encapsulate operations on the object's reference count */
typedef struct {
  hb_atomic_int_t ref_count;
} hb_reference_count_t;

#define hb_reference_count_inc(RC) hb_atomic_int_fetch_and_add ((RC).ref_count, 1)
#define hb_reference_count_dec(RC) hb_atomic_int_fetch_and_add ((RC).ref_count, -1)

#define HB_REFERENCE_COUNT_INIT(RC, VALUE) ((RC).ref_count = (VALUE))

#define HB_REFERENCE_COUNT_GET_VALUE(RC) hb_atomic_int_get ((RC).ref_count)
#define HB_REFERENCE_COUNT_SET_VALUE(RC, VALUE) hb_atomic_int_set ((RC).ref_count, (VALUE))

#define HB_REFERENCE_COUNT_INVALID_VALUE ((hb_atomic_int_t) -1)
#define HB_REFERENCE_COUNT_INVALID {HB_REFERENCE_COUNT_INVALID_VALUE}

#define HB_REFERENCE_COUNT_IS_INVALID(RC) (HB_REFERENCE_COUNT_GET_VALUE (RC) == HB_REFERENCE_COUNT_INVALID_VALUE)

#define HB_REFERENCE_COUNT_HAS_REFERENCE(RC) (HB_REFERENCE_COUNT_GET_VALUE (RC) > 0)



/* Helper macros */

#define HB_OBJECT_IS_INERT(obj) \
    (HB_UNLIKELY (HB_REFERENCE_COUNT_IS_INVALID ((obj)->ref_count)))

#define HB_OBJECT_DO_INIT_EXPR(obj) \
    HB_REFERENCE_COUNT_INIT (obj->ref_count, 1)

#define HB_OBJECT_DO_INIT(obj) \
  HB_STMT_START { \
    HB_OBJECT_DO_INIT_EXPR (obj); \
  } HB_STMT_END

#define HB_OBJECT_DO_CREATE(Type, obj) \
  HB_LIKELY (( \
	     (obj) = (Type *) calloc (1, sizeof (Type)), \
	     HB_OBJECT_DO_INIT_EXPR (obj), \
	     HB_OBJECT_DEBUG_OUT (obj), \
	     (obj) \
	     ))

#define HB_OBJECT_DO_REFERENCE(obj) \
  HB_STMT_START { \
    int old_count; \
    if (HB_UNLIKELY (!(obj) || HB_OBJECT_IS_INERT (obj))) \
      return obj; \
    HB_OBJECT_DEBUG_OUT (obj); \
    old_count = hb_reference_count_inc (obj->ref_count); \
    assert (old_count > 0); \
    return obj; \
  } HB_STMT_END

#define HB_OBJECT_DO_GET_REFERENCE_COUNT(obj) \
  HB_STMT_START { \
    if (HB_UNLIKELY (!(obj) || HB_OBJECT_IS_INERT (obj))) \
      return 0; \
    return HB_REFERENCE_COUNT_GET_VALUE (obj->ref_count); \
  } HB_STMT_END

#define HB_OBJECT_DO_DESTROY(obj) \
  HB_STMT_START { \
    int old_count; \
    if (HB_UNLIKELY (!(obj) || HB_OBJECT_IS_INERT (obj))) \
      return; \
    HB_OBJECT_DEBUG_OUT (obj); \
    old_count = hb_reference_count_dec (obj->ref_count); \
    assert (old_count > 0); \
    if (old_count != 1) \
      return; \
  } HB_STMT_END


#endif /* HB_REFCOUNT_PRIVATE_H */
