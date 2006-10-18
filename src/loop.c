
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

  loop->side = (Subnode *) malloc( 2 * loop->allocated * sizeof(Subnode) );
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

KNIFE_STATUS loop_add_side( Loop loop, Subnode node0, Subnode node1 )
{
  Subnode *new_side;

  if (loop->nside >= loop->allocated)
    {
      loop->allocated += 10;
      new_side = (Subnode *) realloc( loop->side, 
				      2 * loop->allocated * sizeof(Subnode) );
      if (NULL == new_side) {
	printf("%s: %d: realloc failed in loop_add_side\n",
	       __FILE__,__LINE__);
	loop->allocated -= 10;
	return KNIFE_MEMORY;
      }
      loop->side = new_side;
    }

  loop->side[0+2*loop->nside] = node0;
  loop->side[1+2*loop->nside] = node1;

  loop->nside++;

  return KNIFE_SUCCESS;
}
