/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __G_LIB_H__
#define __G_LIB_H__

/* system specific config file glibconfig.h provides definitions for
 * the extrema of many of the standard types. These are:
 *
 *  G_MINSHORT, G_MAXSHORT
 *  G_MININT, G_MAXINT
 *  G_MINLONG, G_MAXLONG
 *  G_MINFLOAT, G_MAXFLOAT
 *  G_MINDOUBLE, G_MAXDOUBLE
 *
 * It also provides the following typedefs:
 *
 *  gint8, guint8
 *  gint16, guint16
 *  gint32, guint32
 *  gint64, guint64
 *
 * It defines the G_BYTE_ORDER symbol to one of G_*_ENDIAN (see later in
 * this file). 
 *
 * And it provides a way to store and retrieve a `gint' in/from a `gpointer'.
 * This is useful to pass an integer instead of a pointer to a callback.
 *
 *  GINT_TO_POINTER(i), GUINT_TO_POINTER(i)
 *  GPOINTER_TO_INT(p), GPOINTER_TO_UINT(p)
 *
 * Finally, it provide the following wrappers to STDC functions:
 *
 *  g_ATEXIT
 *    To register hooks which are executed on exit().
 *    Usually a wrapper for STDC atexit.
 *
 *  void *g_memmove(void *dest, const void *src, guint count);
 *    A wrapper for STDC memmove, or an implementation, if memmove doesn't
 *    exist.  The prototype looks like the above, give or take a const,
 *    or size_t.
 */

/* include varargs functions for assertment macros
 */
#include <stdarg.h>
#include <stdio.h>

/* optionally feature DMALLOC memory allocation debugger
 */
#ifdef USE_DMALLOC
#include "dmalloc.h"
#endif


#ifdef NATIVE_WIN32

/* On native Win32, directory separator is the backslash, and search path
 * separator is the semicolon.
 */
#define G_DIR_SEPARATOR '\\'
#define G_DIR_SEPARATOR_S "\\"
#define G_SEARCHPATH_SEPARATOR ';'
#define G_SEARCHPATH_SEPARATOR_S ";"

#else  /* !NATIVE_WIN32 */

#ifndef __EMX__
/* Unix */

#define G_DIR_SEPARATOR '/'
#define G_DIR_SEPARATOR_S "/"
#define G_SEARCHPATH_SEPARATOR ':'
#define G_SEARCHPATH_SEPARATOR_S ":"

#else
/* EMX/OS2 */

#define G_DIR_SEPARATOR '/'
#define G_DIR_SEPARATOR_S "/"
#define G_SEARCHPATH_SEPARATOR ';'
#define G_SEARCHPATH_SEPARATOR_S ";"

#endif

#endif /* !NATIVE_WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Provide definitions for some commonly used macros.
 *  Some of them are only provided if they haven't already
 *  been defined. It is assumed that if they are already
 *  defined then the current definition is correct.
 */
#ifndef	NULL
#define	NULL	((void*) 0)
#endif

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef	ABS
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

#undef	CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


/* Define G_VA_COPY() to do the right thing for copying va_list variables.
 * glibconfig.h may have already defined G_VA_COPY as va_copy or __va_copy.
 */
#if !defined (G_VA_COPY)
#  if defined (__GNUC__) && defined (__PPC__) && (defined (_CALL_SYSV) || defined (_WIN32))
#  define G_VA_COPY(ap1, ap2)	  (*(ap1) = *(ap2))
#  elif defined (G_VA_COPY_AS_ARRAY)
#  define G_VA_COPY(ap1, ap2)	  g_memmove ((ap1), (ap2), sizeof (va_list))
#  else /* va_list is a pointer */
#  define G_VA_COPY(ap1, ap2)	  ((ap1) = (ap2))
#  endif /* va_list is a pointer */
#endif /* !G_VA_COPY */



#define g_warning   printf
#define g_error		printf


/* Provide convenience macros for handling structure
 * fields through their offsets.
 */
#define G_STRUCT_OFFSET(struct_type, member)	\
    ((gulong) ((gchar*) &((struct_type*) 0)->member))
#define G_STRUCT_MEMBER_P(struct_p, struct_offset)   \
    ((gpointer) ((gchar*) (struct_p) + (gulong) (struct_offset)))
#define G_STRUCT_MEMBER(member_type, struct_p, struct_offset)   \
    (*(member_type*) G_STRUCT_MEMBER_P ((struct_p), (struct_offset)))


/* inlining hassle. for compilers that don't allow the `inline' keyword,
 * mostly because of strict ANSI C compliance or dumbness, we try to fall
 * back to either `__inline__' or `__inline'.
 * we define G_CAN_INLINE, if the compiler seems to be actually
 * *capable* to do function inlining, in which case inline function bodys
 * do make sense. we also define G_INLINE_FUNC to properly export the
 * function prototypes if no inlining can be performed.
 * we special case most of the stuff, so inline functions can have a normal
 * implementation by defining G_INLINE_FUNC to extern and G_CAN_INLINE to 1.
 */
#ifndef G_INLINE_FUNC
#  define G_CAN_INLINE 1
#endif
#ifdef G_HAVE_INLINE
#  if defined (__GNUC__) && defined (__STRICT_ANSI__)
#    undef inline
#    define inline __inline__
#  endif
#else /* !G_HAVE_INLINE */
#  undef inline
#  if defined (G_HAVE___INLINE__)
#    define inline __inline__
#  else /* !inline && !__inline__ */
#    if defined (G_HAVE___INLINE)
#      define inline __inline
#    else /* !inline && !__inline__ && !__inline */
#      define inline /* don't inline, then */
#      ifndef G_INLINE_FUNC
#	 undef G_CAN_INLINE
#      endif
#    endif
#  endif
#endif
#ifndef G_INLINE_FUNC
#  ifdef __GNUC__
#    ifdef __OPTIMIZE__
#      define G_INLINE_FUNC extern inline
#    else
#      undef G_CAN_INLINE
#      define G_INLINE_FUNC extern
#    endif
#  else /* !__GNUC__ */
#    ifdef G_CAN_INLINE
#      define G_INLINE_FUNC static inline
#    else
#      define G_INLINE_FUNC extern
#    endif
#  endif /* !__GNUC__ */
#endif /* !G_INLINE_FUNC */


/* Provide simple macro statement wrappers (adapted from Perl):
 *  G_STMT_START { statements; } G_STMT_END;
 *  can be used as a single statement, as in
 *  if (x) G_STMT_START { ... } G_STMT_END; else ...
 *
 *  For gcc we will wrap the statements within `({' and `})' braces.
 *  For SunOS they will be wrapped within `if (1)' and `else (void) 0',
 *  and otherwise within `do' and `while (0)'.
 */
#if !(defined (G_STMT_START) && defined (G_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define G_STMT_START	(void)(
#    define G_STMT_END		)
#  else
#    if (defined (sun) || defined (__sun__))
#      define G_STMT_START	if (1)
#      define G_STMT_END	else (void)0
#    else
#      define G_STMT_START	do
#      define G_STMT_END	while (0)
#    endif
#  endif
#endif


/* Provide macros to feature the GCC function attribute.
 */
#if	__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define G_GNUC_PRINTF( format_idx, arg_idx )	\
  __attribute__((format (printf, format_idx, arg_idx)))
#define G_GNUC_SCANF( format_idx, arg_idx )	\
  __attribute__((format (scanf, format_idx, arg_idx)))
#define G_GNUC_FORMAT( arg_idx )		\
  __attribute__((format_arg (arg_idx)))
#define G_GNUC_NORETURN				\
  __attribute__((noreturn))
#define G_GNUC_CONST				\
  __attribute__((const))
#define G_GNUC_UNUSED				\
  __attribute__((unused))
#else	/* !__GNUC__ */
#define G_GNUC_PRINTF( format_idx, arg_idx )
#define G_GNUC_SCANF( format_idx, arg_idx )
#define G_GNUC_FORMAT( arg_idx )
#define G_GNUC_NORETURN
#define G_GNUC_CONST
#define	G_GNUC_UNUSED
#endif	/* !__GNUC__ */


/* Wrap the gcc __PRETTY_FUNCTION__ and __FUNCTION__ variables with
 * macros, so we can refer to them as strings unconditionally.
 */
#ifdef	__GNUC__
#define	G_GNUC_FUNCTION		__FUNCTION__
#define	G_GNUC_PRETTY_FUNCTION	__PRETTY_FUNCTION__
#else	/* !__GNUC__ */
#define	G_GNUC_FUNCTION		""
#define	G_GNUC_PRETTY_FUNCTION	""
#endif	/* !__GNUC__ */

/* we try to provide a usefull equivalent for ATEXIT if it is
 * not defined, but use is actually abandoned. people should
 * use g_atexit() instead.
 */
#ifndef ATEXIT
# define ATEXIT(proc)	g_ATEXIT(proc)
#else
# define G_NATIVE_ATEXIT
#endif /* ATEXIT */

/* Hacker macro to place breakpoints for elected machines.
 * Actual use is strongly deprecated of course ;)
 */
#if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#define	G_BREAKPOINT()		G_STMT_START{ __asm__ __volatile__ ("int $03"); }G_STMT_END
#elif defined (__alpha__) && defined (__GNUC__) && __GNUC__ >= 2
#define	G_BREAKPOINT()		G_STMT_START{ __asm__ __volatile__ ("bpt"); }G_STMT_END
#else	/* !__i386__ && !__alpha__ */
#define	G_BREAKPOINT()
#endif	/* __i386__ */


/* Provide macros for easily allocating memory. The macros
 *  will cast the allocated memory to the specified type
 *  in order to avoid compiler warnings. (Makes the code neater).
 */

#ifdef __DMALLOC_H__
#  define g_new(type, count)		(ALLOC (type, count))
#  define g_new0(type, count)		(CALLOC (type, count))
#  define g_renew(type, mem, count)	(REALLOC (mem, type, count))
#else /* __DMALLOC_H__ */
#  define g_new(type, count)	  \
      ((type *) g_malloc ((unsigned) sizeof (type) * (count)))
#  define g_new0(type, count)	  \
      ((type *) g_malloc0 ((unsigned) sizeof (type) * (count)))
#  define g_renew(type, mem, count)	  \
      ((type *) g_realloc (mem, (unsigned) sizeof (type) * (count)))
#endif /* __DMALLOC_H__ */

#define g_mem_chunk_create(type, pre_alloc, alloc_type)	( \
  g_mem_chunk_new (#type " mem chunks (" #pre_alloc ")", \
		   sizeof (type), \
		   sizeof (type) * (pre_alloc), \
		   (alloc_type)) \
)
#define g_chunk_new(type, chunk)	( \
  (type *) g_mem_chunk_alloc (chunk) \
)
#define g_chunk_new0(type, chunk)	( \
  (type *) g_mem_chunk_alloc0 (chunk) \
)
#define g_chunk_free(mem, mem_chunk)	G_STMT_START { \
  g_mem_chunk_free ((mem_chunk), (mem)); \
} G_STMT_END


#define g_string(x) #x


/* Provide macros for error handling. The "assert" macros will
 *  exit on failure. The "return" macros will exit the current
 *  function. Two different definitions are given for the macros
 *  if G_DISABLE_ASSERT is not defined, in order to support gcc's
 *  __PRETTY_FUNCTION__ capability.
 */

#define G_DISABLE_ASSERT   //add by dszhang  in 2008.5.29

#ifdef G_DISABLE_ASSERT

#define g_assert(expr)
#define g_assert_not_reached()

#else /* !G_DISABLE_ASSERT */

#ifdef __GNUC__

#define g_assert(expr)			G_STMT_START{		\
     if (!(expr))						\
       g_log (G_LOG_DOMAIN,					\
	      G_LOG_LEVEL_ERROR,				\
	      "file %s: line %d (%s): assertion failed: (%s)",	\
	      __FILE__,						\
	      __LINE__,						\
	      __PRETTY_FUNCTION__,				\
	      #expr);			}G_STMT_END

#define g_assert_not_reached()		G_STMT_START{		\
     g_log (G_LOG_DOMAIN,					\
	    G_LOG_LEVEL_ERROR,					\
	    "file %s: line %d (%s): should not be reached",	\
	    __FILE__,						\
	    __LINE__,						\
	    __PRETTY_FUNCTION__);	}G_STMT_END

#else /* !__GNUC__ */

#define g_assert(expr)			G_STMT_START{		\
     if (!(expr))						\
       g_log (G_LOG_DOMAIN,					\
	      G_LOG_LEVEL_ERROR,				\
	      "file %s: line %d: assertion failed: (%s)",	\
	      __FILE__,						\
	      __LINE__,						\
	      #expr);			}G_STMT_END

#define g_assert_not_reached()		G_STMT_START{	\
     g_log (G_LOG_DOMAIN,				\
	    G_LOG_LEVEL_ERROR,				\
	    "file %s: line %d: should not be reached",	\
	    __FILE__,					\
	    __LINE__);		}G_STMT_END

#endif /* __GNUC__ */

#endif /* !G_DISABLE_ASSERT */


#define G_DISABLE_CHECKS   //add by dszhang 2008.5.29

#ifdef G_DISABLE_CHECKS

#define g_return_if_fail(expr)			\
if (!(expr))							\
	 return ;
#define g_return_val_if_fail(expr,val)	 \
if (!(expr))							\
	 return val;						

#else /* !G_DISABLE_CHECKS */

#ifdef __GNUC__

#define g_return_if_fail(expr)		G_STMT_START{			\
     if (!(expr))							\
       {								\
	 g_log (G_LOG_DOMAIN,						\
		G_LOG_LEVEL_CRITICAL,					\
		"file %s: line %d (%s): assertion `%s' failed.",	\
		__FILE__,						\
		__LINE__,						\
		__PRETTY_FUNCTION__,					\
		#expr);							\
	 return;							\
       };				}G_STMT_END

#define g_return_val_if_fail(expr,val)	G_STMT_START{			\
     if (!(expr))							\
       {								\
	 g_log (G_LOG_DOMAIN,						\
		G_LOG_LEVEL_CRITICAL,					\
		"file %s: line %d (%s): assertion `%s' failed.",	\
		__FILE__,						\
		__LINE__,						\
		__PRETTY_FUNCTION__,					\
		#expr);							\
	 return val;							\
       };				}G_STMT_END

#else /* !__GNUC__ */

#define g_return_if_fail(expr)		G_STMT_START{		\
     if (!(expr))						\
       {							\
	 g_log (G_LOG_DOMAIN,					\
		G_LOG_LEVEL_CRITICAL,				\
		"file %s: line %d: assertion `%s' failed.",	\
		__FILE__,					\
		__LINE__,					\
		#expr);						\
	 return;						\
       };				}G_STMT_END

#define g_return_val_if_fail(expr, val)	G_STMT_START{		\
     if (!(expr))						\
       {							\
	 g_log (G_LOG_DOMAIN,					\
		G_LOG_LEVEL_CRITICAL,				\
		"file %s: line %d: assertion `%s' failed.",	\
		__FILE__,					\
		__LINE__,					\
		#expr);						\
	 return val;						\
       };				}G_STMT_END

#endif /* !__GNUC__ */

#endif /* !G_DISABLE_CHECKS */


/* Provide type definitions for commonly used types.
 *  These are useful because a "gint8" can be adjusted
 *  to be 1 byte (8 bits) on all platforms. Similarly and
 *  more importantly, "gint32" can be adjusted to be
 *  4 bytes (32 bits) on all platforms.
 */

typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef 	int    gint;
typedef gint   gboolean;

typedef unsigned char	guchar;
typedef unsigned short	gushort;
typedef unsigned long	gulong;
typedef unsigned int	guint;

typedef float	gfloat;
typedef double	gdouble;

typedef int 	gint32;
typedef unsigned int guint32 ;


/* HAVE_LONG_DOUBLE doesn't work correctly on all platforms.
 * Since gldouble isn't used anywhere, just disable it for now */

#if 0
#ifdef HAVE_LONG_DOUBLE
typedef long double gldouble;
#else /* HAVE_LONG_DOUBLE */
typedef double gldouble;
#endif /* HAVE_LONG_DOUBLE */
#endif /* 0 */

typedef void* gpointer;
typedef const void *gconstpointer;


typedef gint32	gssize;
typedef guint32 gsize;
typedef guint32 GQuark;
typedef gint32	GTime;


/* Portable endian checks and conversions
 *
 * glibconfig.h defines G_BYTE_ORDER which expands to one of
 * the below macros.
 */
#define G_LITTLE_ENDIAN 1234
#define G_BIG_ENDIAN    4321
#define G_PDP_ENDIAN    3412		/* unused, need specific PDP check */	


/* Basic bit swapping functions
 */
#define GUINT16_SWAP_LE_BE_CONSTANT(val)	((guint16) ( \
    (((guint16) (val) & (guint16) 0x00ffU) << 8) | \
    (((guint16) (val) & (guint16) 0xff00U) >> 8)))
#define GUINT32_SWAP_LE_BE_CONSTANT(val)	((guint32) ( \
    (((guint32) (val) & (guint32) 0x000000ffU) << 24) | \
    (((guint32) (val) & (guint32) 0x0000ff00U) <<  8) | \
    (((guint32) (val) & (guint32) 0x00ff0000U) >>  8) | \
    (((guint32) (val) & (guint32) 0xff000000U) >> 24)))



#if 0   //add by dszhang in 2008.5.29
/* Intel specific stuff for speed
 */
#if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#  define GUINT16_SWAP_LE_BE_X86(val) \
     (__extension__					\
      ({ register guint16 __v;				\
	 if (__builtin_constant_p (val))		\
	   __v = GUINT16_SWAP_LE_BE_CONSTANT (val);	\
	 else						\
	   __asm__ __const__ ("rorw $8, %w0"		\
			      : "=r" (__v)		\
			      : "0" ((guint16) (val)));	\
	__v; }))
#  define GUINT16_SWAP_LE_BE(val) (GUINT16_SWAP_LE_BE_X86 (val))
#  if !defined(__i486__) && !defined(__i586__) \
      && !defined(__pentium__) && !defined(__i686__) && !defined(__pentiumpro__)
#     define GUINT32_SWAP_LE_BE_X86(val) \
        (__extension__						\
         ({ register guint32 __v;				\
	    if (__builtin_constant_p (val))			\
	      __v = GUINT32_SWAP_LE_BE_CONSTANT (val);		\
	  else							\
	    __asm__ __const__ ("rorw $8, %w0\n\t"		\
			       "rorl $16, %0\n\t"		\
			       "rorw $8, %w0"			\
			       : "=r" (__v)			\
			       : "0" ((guint32) (val)));	\
	__v; }))
#  else /* 486 and higher has bswap */
#     define GUINT32_SWAP_LE_BE_X86(val) \
        (__extension__						\
         ({ register guint32 __v;				\
	    if (__builtin_constant_p (val))			\
	      __v = GUINT32_SWAP_LE_BE_CONSTANT (val);		\
	  else							\
	    __asm__ __const__ ("bswap %0"			\
			       : "=r" (__v)			\
			       : "0" ((guint32) (val)));	\
	__v; }))
#  endif /* processor specific 32-bit stuff */
#  define GUINT32_SWAP_LE_BE(val) (GUINT32_SWAP_LE_BE_X86 (val))
#else /* !__i386__ */
#  define GUINT16_SWAP_LE_BE(val) (GUINT16_SWAP_LE_BE_CONSTANT (val))
#  define GUINT32_SWAP_LE_BE(val) (GUINT32_SWAP_LE_BE_CONSTANT (val))
#endif /* __i386__ */

#ifdef G_HAVE_GINT64
#  define GUINT64_SWAP_LE_BE_CONSTANT(val)	((guint64) ( \
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0xff00000000000000U)) >> 56)))
#  if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#    define GUINT64_SWAP_LE_BE_X86(val) \
	(__extension__						\
	 ({ union { guint64 __ll;				\
		    guint32 __l[2]; } __r;			\
	    if (__builtin_constant_p (val))			\
	      __r.__ll = GUINT64_SWAP_LE_BE_CONSTANT (val);	\
	    else						\
	      {							\
	 	union { guint64 __ll;				\
			guint32 __l[2]; } __w;			\
		__w.__ll = ((guint64) val);			\
		__r.__l[0] = GUINT32_SWAP_LE_BE (__w.__l[1]);	\
		__r.__l[1] = GUINT32_SWAP_LE_BE (__w.__l[0]);	\
	      }							\
	  __r.__ll; }))
#    define GUINT64_SWAP_LE_BE(val) (GUINT64_SWAP_LE_BE_X86 (val))
#  else /* !__i386__ */
#    define GUINT64_SWAP_LE_BE(val) (GUINT64_SWAP_LE_BE_CONSTANT(val))
#  endif
#endif



#define GUINT16_SWAP_LE_PDP(val)	((guint16) (val))
#define GUINT16_SWAP_BE_PDP(val)	(GUINT16_SWAP_LE_BE (val))
#define GUINT32_SWAP_LE_PDP(val)	((guint32) ( \
    (((guint32) (val) & (guint32) 0x0000ffffU) << 16) | \
    (((guint32) (val) & (guint32) 0xffff0000U) >> 16)))
#define GUINT32_SWAP_BE_PDP(val)	((guint32) ( \
    (((guint32) (val) & (guint32) 0x00ff00ffU) << 8) | \
    (((guint32) (val) & (guint32) 0xff00ff00U) >> 8)))

/* The G*_TO_?E() macros are defined in glibconfig.h.
 * The transformation is symmetric, so the FROM just maps to the TO.
 */
#define GINT16_FROM_LE(val)	(GINT16_TO_LE (val))
#define GUINT16_FROM_LE(val)	(GUINT16_TO_LE (val))
#define GINT16_FROM_BE(val)	(GINT16_TO_BE (val))
#define GUINT16_FROM_BE(val)	(GUINT16_TO_BE (val))
#define GINT32_FROM_LE(val)	(GINT32_TO_LE (val))
#define GUINT32_FROM_LE(val)	(GUINT32_TO_LE (val))
#define GINT32_FROM_BE(val)	(GINT32_TO_BE (val))
#define GUINT32_FROM_BE(val)	(GUINT32_TO_BE (val))

#ifdef G_HAVE_GINT64
#define GINT64_FROM_LE(val)	(GINT64_TO_LE (val))
#define GUINT64_FROM_LE(val)	(GUINT64_TO_LE (val))
#define GINT64_FROM_BE(val)	(GINT64_TO_BE (val))
#define GUINT64_FROM_BE(val)	(GUINT64_TO_BE (val))
#endif

#define GLONG_FROM_LE(val)	(GLONG_TO_LE (val))
#define GULONG_FROM_LE(val)	(GULONG_TO_LE (val))
#define GLONG_FROM_BE(val)	(GLONG_TO_BE (val))
#define GULONG_FROM_BE(val)	(GULONG_TO_BE (val))

#define GINT_FROM_LE(val)	(GINT_TO_LE (val))
#define GUINT_FROM_LE(val)	(GUINT_TO_LE (val))
#define GINT_FROM_BE(val)	(GINT_TO_BE (val))
#define GUINT_FROM_BE(val)	(GUINT_TO_BE (val))


/* Portable versions of host-network order stuff
 */
#define g_ntohl(val) (GUINT32_FROM_BE (val))
#define g_ntohs(val) (GUINT16_FROM_BE (val))
#define g_htonl(val) (GUINT32_TO_BE (val))
#define g_htons(val) (GUINT16_TO_BE (val))


/* Glib version.
 * we prefix variable declarations so they can
 * properly get exported in windows dlls.
 */
#ifdef NATIVE_WIN32
#  ifdef GLIB_COMPILATION
#    define GUTILS_C_VAR __declspec(dllexport)
#  else /* !GLIB_COMPILATION */
#    define GUTILS_C_VAR extern __declspec(dllimport)
#  endif /* !GLIB_COMPILATION */
#else /* !NATIVE_WIN32 */
#  define GUTILS_C_VAR extern
#endif /* !NATIVE_WIN32 */

GUTILS_C_VAR const guint glib_major_version;
GUTILS_C_VAR const guint glib_minor_version;
GUTILS_C_VAR const guint glib_micro_version;
GUTILS_C_VAR const guint glib_interface_age;
GUTILS_C_VAR const guint glib_binary_age;

#define GLIB_CHECK_VERSION(major,minor,micro)    \
    (GLIB_MAJOR_VERSION > (major) || \
     (GLIB_MAJOR_VERSION == (major) && GLIB_MINOR_VERSION > (minor)) || \
     (GLIB_MAJOR_VERSION == (major) && GLIB_MINOR_VERSION == (minor) && \
      GLIB_MICRO_VERSION >= (micro)))

/* Forward declarations of glib types.
 */
typedef struct _GAllocator	GAllocator;
typedef struct _GArray		GArray;
typedef struct _GByteArray	GByteArray;
typedef struct _GCache		GCache;
typedef struct _GCompletion	GCompletion;
typedef	struct _GData		GData;
typedef struct _GDebugKey	GDebugKey;
typedef struct _GHashTable	GHashTable;
typedef struct _GHook		GHook;
typedef struct _GHookList	GHookList;
typedef struct _GList		GList;
typedef struct _GMemChunk	GMemChunk;
typedef struct _GNode		GNode;
typedef struct _GPtrArray	GPtrArray;
typedef struct _GRelation	GRelation;
typedef struct _GScanner	GScanner;
typedef struct _GScannerConfig	GScannerConfig;
typedef struct _GSList		GSList;
typedef struct _GString		GString;
typedef struct _GStringChunk	GStringChunk;
typedef struct _GTimer		GTimer;
typedef struct _GTree		GTree;
typedef struct _GTuples		GTuples;
typedef union  _GTokenValue	GTokenValue;
typedef struct _GIOChannel	GIOChannel;


#endif
typedef  int		GMemChunk;
typedef  int		GTree;

/* Tree traverse flags */
typedef enum
{
  G_TRAVERSE_LEAFS	= 1 << 0,
  G_TRAVERSE_NON_LEAFS	= 1 << 1,
  G_TRAVERSE_ALL	= G_TRAVERSE_LEAFS | G_TRAVERSE_NON_LEAFS,
  G_TRAVERSE_MASK	= 0x03
} GTraverseFlags;

/* Tree traverse orders */
typedef enum
{
  G_IN_ORDER,
  G_PRE_ORDER,
  G_POST_ORDER,
  G_LEVEL_ORDER
} GTraverseType;

/* Log level shift offset for user defined
 * log levels (0-7 are used by GLib).
 */
#define	G_LOG_LEVEL_USER_SHIFT	(8)

/* Glib log levels and flags.
 */
typedef enum
{
  /* log flags */
  G_LOG_FLAG_RECURSION		= 1 << 0,
  G_LOG_FLAG_FATAL		= 1 << 1,
  
  /* GLib log levels */
  G_LOG_LEVEL_ERROR		= 1 << 2,	/* always fatal */
  G_LOG_LEVEL_CRITICAL		= 1 << 3,
  G_LOG_LEVEL_WARNING		= 1 << 4,
  G_LOG_LEVEL_MESSAGE		= 1 << 5,
  G_LOG_LEVEL_INFO		= 1 << 6,
  G_LOG_LEVEL_DEBUG		= 1 << 7,
  
  G_LOG_LEVEL_MASK		= ~(G_LOG_FLAG_RECURSION | G_LOG_FLAG_FATAL)
} GLogLevelFlags;

/* GLib log levels that are considered fatal by default */
#define	G_LOG_FATAL_MASK	(G_LOG_FLAG_RECURSION | G_LOG_LEVEL_ERROR)


typedef	void		(*GVoidFunc)(void);

typedef gint	(*GCompareFunc)	(gconstpointer	a,
						 gconstpointer	b);
typedef gint		(*GSearchFunc)		(gpointer	key,
						 gpointer	data);
typedef gint		(*GTraverseFunc)	(gpointer	key,
						 gpointer	value,
						 gpointer	data);
typedef gint		(*GAuxiliaryFunc)		(gpointer	value,
						 gpointer	data);







/* Balanced binary trees
 */
GTree*	 g_tree_new	 (GCompareFunc	 key_compare_func);
void	 g_tree_destroy	 (GTree		*tree);
void	 g_tree_insert	 (GTree		*tree,
			  gpointer	 key,
			  gpointer	 value);
void	 g_tree_remove	 (GTree		*tree,
			  gpointer	 key);
gpointer g_tree_lookup	 (GTree		*tree,
			  gpointer	 key);
void	 g_tree_traverse (GTree		*tree,
			  GTraverseFunc	 traverse_func,
			  GTraverseType	 traverse_type,
			  gpointer	 data);
gpointer g_tree_search	 (GTree		*tree,
			  GSearchFunc	 search_func,
			  gpointer	 data);
guint 	g_tree_search_num (GTree       *tree,
	       GSearchFunc  search_func,gpointer     data,
	       GAuxiliaryFunc  auxiliary_func,gpointer    auxi_data,guint offset,guint num);
gint	 g_tree_height	 (GTree		*tree);
gint	 g_tree_nnodes	 (GTree		*tree);





/* Memory allocation and debugging
 */
#ifdef USE_DMALLOC

#define g_malloc(size)	     ((gpointer) MALLOC (size))
#define g_malloc0(size)	     ((gpointer) CALLOC (char, size))
#define g_realloc(mem,size)  ((gpointer) REALLOC (mem, char, size))
#define g_free(mem)	     FREE (mem)

#else /* !USE_DMALLOC */

gpointer g_malloc      (gulong	  size);
gpointer g_malloc0     (gulong	  size);
gpointer g_realloc     (gpointer  mem,
			gulong	  size);
void	 g_free	       (gpointer  mem);

#endif /* !USE_DMALLOC */

void	 g_mem_profile (void);
void	 g_mem_check   (gpointer  mem);




/* "g_mem_chunk_new" creates a new memory chunk.
 * Memory chunks are used to allocate pieces of memory which are
 *  always the same size. Lists are a good example of such a data type.
 * The memory chunk allocates and frees blocks of memory as needed.
 *  Just be sure to call "g_mem_chunk_free" and not "g_free" on data
 *  allocated in a mem chunk. ("g_free" will most likely cause a seg
 *  fault...somewhere).
 *
 * Oh yeah, GMemChunk is an opaque data type. (You don't really
 *  want to know what's going on inside do you?)
 */

/* ALLOC_ONLY MemChunk's can only allocate memory. The free operation
 *  is interpreted as a no op. ALLOC_ONLY MemChunk's save 4 bytes per
 *  atom. (They are also useful for lists which use MemChunk to allocate
 *  memory but are also part of the MemChunk implementation).
 * ALLOC_AND_FREE MemChunk's can allocate and free memory.
 */

#define G_ALLOC_ONLY	  1
#define G_ALLOC_AND_FREE  2

GMemChunk* g_mem_chunk_new     (gchar	  *name,
				gint	   atom_size,
				gulong	   area_size,
				gint	   type);
void	   g_mem_chunk_destroy (GMemChunk *mem_chunk);
gpointer   g_mem_chunk_alloc   (GMemChunk *mem_chunk);
gpointer   g_mem_chunk_alloc0  (GMemChunk *mem_chunk);
void	   g_mem_chunk_free    (GMemChunk *mem_chunk,
				gpointer   mem);
void	   g_mem_chunk_clean   (GMemChunk *mem_chunk);
void	   g_mem_chunk_reset   (GMemChunk *mem_chunk);
void	   g_mem_chunk_print   (GMemChunk *mem_chunk);
void	   g_mem_chunk_info    (void);

/* Ah yes...we have a "g_blow_chunks" function.
 * "g_blow_chunks" simply compresses all the chunks. This operation
 *  consists of freeing every memory area that should be freed (but
 *  which we haven't gotten around to doing yet). And, no,
 *  "g_blow_chunks" doesn't follow the naming scheme, but it is a
 *  much better name than "g_mem_chunk_clean_all" or something
 *  similar.
 */
void g_blow_chunks (void);




#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __G_LIB_H__ */

