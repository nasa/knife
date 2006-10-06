
/* commonly used #define's for knife package */
  
/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef KNIFE_DEFINITIONS_H
#define KNIFE_DEFINITIONS_H

#ifdef __cplusplus
#  define BEGIN_C_DECLORATION extern "C" {
#  define END_C_DECLORATION }
#else
#  define BEGIN_C_DECLORATION
#  define END_C_DECLORATION
#endif

BEGIN_C_DECLORATION

#define EMPTY (-1)

#if !defined(ABS)
#define ABS(a)   ((a)>0?(a):-(a))
#endif

typedef short KnifeBool;

#undef TRUE
#undef FALSE
#define TRUE    ((KnifeBool)1)
#define FALSE   ((KnifeBool)0)

#if !defined(MIN)
#define MIN(a,b) ((a)<(b)?(a):(b)) 
#endif
#if !defined(MAX)
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
END_C_DECLORATION

#endif /* KNIFE_DEFINITIONS_H */
