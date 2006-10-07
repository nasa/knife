
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
  int nface;

  surface = surface_create( );
  nface = 0;
  for ( iface = 0 ; iface < primal_nface(primal) ; iface ++ )
    {
      primal_face(primal, iface, face);
      if (array_contains_int(bcs,face[3])) nface++;
    }

  printf("number of triangular faces in the surface %d\n",nface);

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
