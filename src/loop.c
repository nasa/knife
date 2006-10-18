
/* loop of subtri sides used for provable boundary recovery */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include "loop.h"

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				      \
  if (NULL == (pointer)) {				      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
  }

Loop loop_create( void )
{
  Loop loop;
  
  loop = (Loop) malloc( sizeof(LoopStruct) );
  if (NULL == loop) {
    printf("%s: %d: malloc failed in loop_create self\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  loop->nside = 0;
  loop->allocated = 10;

  loop->side = (Subnode *) malloc( 2 * array->allocated * sizeof(Subnode) );
  if (NULL == loop->side) {
    printf("%s: %d: malloc failed in loop_create side\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  return loop;
}

void loop_free( Loop loop )
{
  if ( NULL == loop ) return;
  if ( NULL != loop->side ) free(loop->side);
  free( loop );
}

