
/* commonly used #define's for knife package */
  


/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
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

#if !defined(MIN3)
#define MIN3(a) (MIN((a)[0],MIN((a)[1],(a)[2]))) 
#endif

#define knife_double_zero(x) ( !(0.0 > (x)) && !(0.0 < (x)) )

typedef int KNIFE_STATUS;

#define KNIFE_SUCCESS       (0)
#define KNIFE_FAILURE       (1)
#define KNIFE_MEMORY        (2)
#define KNIFE_ARRAY_BOUND   (3)
#define KNIFE_NOT_FOUND     (4)
#define KNIFE_SINGULAR      (5)
#define KNIFE_NO_INT        (6)
#define KNIFE_NULL          (7)
#define KNIFE_BIGGER        (8)
#define KNIFE_IMPLEMENT     (9)
#define KNIFE_NEG_AREA     (10)
#define KNIFE_MISSING      (11)
#define KNIFE_RECOVERED    (12)
#define KNIFE_DIV_ZERO     (13)
#define KNIFE_UNIQUENESS   (14)
#define KNIFE_NOT_IMPROVED (15)
#define KNIFE_IMPROPER     (16)
#define KNIFE_NO_MAS       (17)
#define KNIFE_INCONSISTENT (18)
#define KNIFE_FILE_ERROR   (19)

#define TSS(fcn,msg)							\
  {									\
    KNIFE_STATUS code;							\
    code = (fcn);							\
    if (KNIFE_SUCCESS != code){						\
      printf("%s: %d: %s: %d %s\n",__FILE__,__LINE__,__func__,code,(msg)); \
      return code;							\
    }									\
  }

#define TNS(ptr,msg)							\
  {									\
    if ( NULL == (ptr)){				                \
      printf("%s: %d: %s: NULL pointer %s\n",                           \
             __FILE__,__LINE__,__func__,(msg));                         \
      return KNIFE_NULL;		    			        \
    }									\
  }

END_C_DECLORATION

#endif /* KNIFE_DEFINITIONS_H */
