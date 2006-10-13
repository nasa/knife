
/* a triangle wrapper that extends to include orientation and active subtri */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <stdlib.h>
#include <stdio.h>
#include "mask.h"

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

Mask mask_create( Triangle traingle, KnifeBool inward_pointing_normal )
{
  Mask mask;
  
  mask = (Mask)malloc( sizeof(MaskStruct) );
  if (NULL == mask) {
    printf("%s: %d: malloc failed in mask_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  mask->triangle = traingle;
  mask->inward_pointing_normal = inward_pointing_normal;
  mask->active   = NULL;

  return mask;
}

void mask_free( Mask mask )
{
  if ( NULL == mask ) return;
  if ( NULL != mask->active )free( mask->active );
  free( mask );
}

int mask_nsubtri( Mask mask )
{
  int nsubtri;
  int subtri_index;

  if ( NULL == mask->active ) return triangle_nsubtri(mask_triangle(mask));

  nsubtri = 0;
  for (subtri_index = 0;
       subtri_index < triangle_nsubtri(mask_triangle(mask));
       subtri_index++)
    if (mask->active[subtri_index]) nsubtri++;

  return nsubtri;
}

KNIFE_STATUS mask_deactivate_all_subtri( Mask mask )
{
  int nsubtri;
  int subtri_index;

  nsubtri = triangle_nsubtri( mask_triangle(mask) );

  mask->active = (KnifeBool *) malloc( nsubtri * sizeof(int) );
  if (NULL == mask->active) {
    printf("%s: %d: malloc mask->active failed in mask_deactivate_all_subtri\n",
	   __FILE__,__LINE__);
    return KNIFE_MEMORY; 
  }
  
  for (subtri_index = 0; subtri_index < nsubtri; subtri_index++)
    mask->active[subtri_index] = FALSE;
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_activate_subtri( Mask mask, Subtri subtri)
{
  int subtri_index;

  if ( NULL == mask ) return KNIFE_NULL;

  TRY( triangle_subtri_index( mask_triangle(mask), subtri, &subtri_index ),
       "no index for subtri" );

  return mask_activate_subtri_index( mask, subtri_index ); 
}

KNIFE_STATUS mask_activate_subtri_index( Mask mask, int subtri_index )
{

  if ( NULL == mask ) return KNIFE_NULL;
  if ( NULL == mask->active ) return KNIFE_ARRAY_BOUND;

  mask->active[subtri_index] = TRUE;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_paint( Mask mask )
{
  Triangle triangle;
  int subtri_index, neighbor_index;
  Subtri subtri, neighbor;
  Subnode subnode0, subnode1;
  Cut cut;

  KnifeBool more_paint;

  triangle = mask_triangle(mask);

  more_paint = FALSE;

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle,subtri_index); 
      subnode0 = subtri_n0(subtri);
      subnode1 = subtri_n1(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  if (KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, 
							     subnode1, subnode0,
							     &cut ) )
	    {
	      TRY( triangle_subtri_index( triangle, neighbor,
					  &neighbor_index ), "neighbor_index");
	      if ( mask->active[subtri_index] || mask->active[neighbor_index] )
		{
		 if ( !mask->active[subtri_index] || 
		      !mask->active[neighbor_index] ) more_paint = TRUE;
		  mask->active[subtri_index] = TRUE;
		  mask->active[neighbor_index] = TRUE;
		}
	    }
	}
      subnode0 = subtri_n1(subtri);
      subnode1 = subtri_n2(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  if (KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, 
							     subnode1, subnode0,
							     &cut ) )
	    {
	      TRY( triangle_subtri_index( triangle, neighbor,
					  &neighbor_index ), "neighbor_index");
	      if ( mask->active[subtri_index] || mask->active[neighbor_index] )
		{
		 if ( !mask->active[subtri_index] || 
		      !mask->active[neighbor_index] ) more_paint = TRUE;
		  mask->active[subtri_index] = TRUE;
		  mask->active[neighbor_index] = TRUE;
		}
	    }
	}
      subnode0 = subtri_n2(subtri);
      subnode1 = subtri_n0(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  if (KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, 
							     subnode1, subnode0,
							     &cut ) )
	    {
	      TRY( triangle_subtri_index( triangle, neighbor,
					  &neighbor_index ), "neighbor_index");
	      if ( mask->active[subtri_index] || mask->active[neighbor_index] )
		{
		 if ( !mask->active[subtri_index] || 
		      !mask->active[neighbor_index] ) more_paint = TRUE;
		  mask->active[subtri_index] = TRUE;
		  mask->active[neighbor_index] = TRUE;
		}
	    }
	}
    } 
 
  if (more_paint) return mask_paint( mask );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_dump_geom( Mask mask, FILE *f )
{
  Triangle triangle;
  int subtri_index;

  triangle = mask_triangle(mask);

  if ( NULL == mask->active ) return triangle_dump_geom(triangle,f);

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    if ( mask->active[subtri_index] ) 
      subtri_dump_geom( triangle_subtri(triangle, subtri_index), f );

  return KNIFE_SUCCESS;
}
