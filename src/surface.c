
/* collection of segments and triangles defining a surface */

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
#include "surface.h"
#include "segment.h"
#include "triangle.h"

Surface surface_from( Primal primal, Array bcs )
{
  Surface surface;
  int face[4], iface;
  KnifeBool include_face;
  int ibc, *bc;
  int ntriangle;

  surface = surface_create( );
  ntriangle = 0;
  for ( iface = 0 ; iface < primal_nface(primal) ; iface ++ )
    {
      primal_face(primal, iface, face);

      include_face = FALSE;
      for ( ibc = 0 ; ibc < array_size(bcs) ; ibc++ )
	{
	  bc = (int *)array_item(bcs, ibc);
	  if (face[3] == *bc) {
	    include_face = TRUE;
	  }
	}
      if (include_face) ntriangle++;
    }

  printf("number of triangles in the surface %d\n",ntriangle);

  return surface;
}

Surface surface_create( void )
{
  Surface surface;
  
  surface = (Surface)malloc( sizeof(SurfaceStruct) );
  if (NULL == surface) {
    printf("%s: %d: malloc failed in surface_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  surface->segments  = array_create( 50000, 50000);
  surface->triangles = array_create( 50000, 50000);
  
  return surface;
}

void surface_free( Surface surface )
{
  if ( NULL == surface ) return;
  free( surface );
}
