
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

