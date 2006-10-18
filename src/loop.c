
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

KNIFE_STATUS loop_add_to_front( Loop loop, Subnode node0, Subnode node1 )
{
  KNIFE_STATUS remove_status;
  
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

KNIFE_STATUS loop_add_subtri( Loop loop, Subtri subtri )
{

  TRY( loop_add_to_front( loop, subtri_n1(subtri), subtri_n2(subtri) ), "fnt0");
  TRY( loop_add_to_front( loop, subtri_n2(subtri), subtri_n0(subtri) ), "fnt1");
  TRY( loop_add_to_front( loop, subtri_n0(subtri), subtri_n1(subtri) ), "fnt2");
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_hard_edge( Loop loop, Subnode node0, Subnode node1 )
{

  TRY( loop_add_side( loop, node0, node1 ), "add side");
  TRY( loop_add_side( loop, node1, node0 ), "add side");  

  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_triangulate( Loop loop, Triangle triangle )
{
  int side0, side1;
  Subnode node0, node1, node2;
  Subtri subtri;

  while (0 < loop_nside(loop))
    { 
      TRY( loop_most_convex( loop, &side0, &side1 ), "most convex failed");
      
      node0 = loop->side[0+2*side0];
      node1 = loop->side[1+2*side0];
      if ( node1 != loop->side[0+2*side1] ) return KNIFE_FAILURE;
      node2 = loop->side[1+2*side1];
      
      /* add the subtri sides in reverse */
      TRY( loop_add_to_front( loop, node2, node1 ), "triangulate front 0");
      TRY( loop_add_to_front( loop, node0, node2 ), "triangulate front 1");
      TRY( loop_add_to_front( loop, node1, node0 ), "triangulate front 2");

      subtri = subtri_create( node0, node1, node2 );
      NOT_NULL( subtri, "subtri creation failed" );
      TRY( triangle_add_subtri(triangle,subtri), "add new subtri");
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_most_convex( Loop loop, int *side0_index, int *side1_index )
{
  int side0, side1;
  Subnode node0, node1, node2;
  double area, best_area;
  int best_side0, best_side1;
  
  best_area = -1.0;
  best_side0 = EMPTY;
  best_side1 = EMPTY;

  for ( side0 = 0 ; side0 < loop_nside(loop) ; side0++ )
    for ( side1 = 0 ; side1 < loop_nside(loop) ; side1++ )
      if ( loop->side[1+2*side0] == loop->side[0+2*side1] )
	{
	  node0 = loop->side[0+2*side0];
	  node1 = loop->side[1+2*side0];
	  node2 = loop->side[1+2*side1];
	  
	  area = subnode_area( node0, node1, node2 );
	  if ( area > best_area )
	    {
	      best_area = area;
	      best_side0 = side0;
	      best_side1 = side1;
	    }
	}

  if (best_area < 0.0) return KNIFE_NOT_FOUND;

  *side0_index = best_side0;
  *side1_index = best_side1;

  return KNIFE_SUCCESS;
}
