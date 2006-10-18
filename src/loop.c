
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

KNIFE_STATUS loop_remove_side( Loop loop, Subnode node0, Subnode node1 )
{
  int existing_side;
  int sweep;
  KnifeBool found;

  found = FALSE;
  for ( existing_side = 0 ; existing_side < loop_nside(loop) ; existing_side++ )
    if ( node0 == loop->side[0+2*existing_side] &&
	 node1 == loop->side[1+2*existing_side] ) 
      {
	found = TRUE;
	for ( sweep = existing_side+1 ; sweep < loop_nside(loop) ; sweep++ )
	  {
	    loop->side[0+2*(sweep-1)] = loop->side[0+2*sweep];
	    loop->side[1+2*(sweep-1)] = loop->side[1+2*sweep];
	    loop->nside--;
	  }
      }
  
  return ( found ? KNIFE_SUCCESS : KNIFE_NOT_FOUND );
}

KNIFE_STATUS loop_add_subtri( Loop loop, Subtri subtri )
{
  Subnode node0, node1;
  KNIFE_STATUS remove_status;

  node0 = subtri_n1(subtri);
  node1 = subtri_n2(subtri);

  remove_status = loop_remove_side( loop, node1, node0 );
  if (KNIFE_NOT_FOUND == remove_status)
    {
      TRY( loop_add_side( loop, node0, node1 ), "add side");
    }
  else
    {
      TRY( remove_status, "remove side");
    }
  
  node0 = subtri_n2(subtri);
  node1 = subtri_n0(subtri);

  remove_status = loop_remove_side( loop, node1, node0 );
  if (KNIFE_NOT_FOUND == remove_status)
    {
      TRY( loop_add_side( loop, node0, node1 ), "add side");
    }
  else
    {
      TRY( remove_status, "remove side");
    }
  
  node0 = subtri_n0(subtri);
  node1 = subtri_n1(subtri);

  remove_status = loop_remove_side( loop, node1, node0 );
  if (KNIFE_NOT_FOUND == remove_status)
    {
      TRY( loop_add_side( loop, node0, node1 ), "add side");
    }
  else
    {
      TRY( remove_status, "remove side");
    }
  
  return KNIFE_SUCCESS;
}
