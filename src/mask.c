
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
  for (subtri_index = 0; subtri_index < nsubtri; subtri_index++)
    if (mask->active[subtri_index]) nsubtri++;

  return nsubtri;
}

KNIFE_STATUS mask_deactivate_all_subtri( Mask mask )
{
  int nsubtri;
  int subtri_index;

  nsubtri = triangle_nsubtri( mask_triangle(mask) );

  mask->active = (KnifeBool *) malloc( sizeof(int) );
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
  Triangle triangle;
  int subtri_index;

  triangle = mask_triangle(mask);

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    if ( subtri == triangle_subtri(triangle, subtri_index) )
      {
	mask->active[subtri_index] = TRUE;
      }

  return KNIFE_NOT_FOUND; 
}


KNIFE_STATUS mask_dump_geom( Mask mask, FILE *f )
{
  Triangle triangle;
  int subtri_index;

  if ( NULL == mask->active ) return triangle_dump_geom(mask_triangle(mask),f);

  triangle = mask_triangle(mask);

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    if ( mask->active[subtri_index]) 
      subtri_dump_geom( triangle_subtri(triangle, subtri_index), f );

  return KNIFE_SUCCESS;
}
