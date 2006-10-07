
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
  int face[4];
  int local_iface, global_iface;
  int local_nface;
  int global_other, local_other, side;
  int *g2l, *l2g, *f2f;
  int nseg;

  surface = surface_create( );

  g2l = (int *)malloc( primal_nface(primal)*sizeof(int) );
  for (global_iface=0;global_iface<primal_nface(primal);global_iface++) 
    g2l[global_iface] = EMPTY;

  local_nface = 0;
  for ( global_iface = 0 ; 
	global_iface < primal_nface(primal) ; 
	global_iface++ )
    {
      primal_face(primal, global_iface, face);
      if (array_contains_int(bcs,face[3])) 
	{
	  g2l[global_iface] = local_nface;
	  local_nface++;
	}
    }

  printf("number of triangular faces in the surface %d\n",local_nface);

  l2g = (int *)malloc( local_nface*sizeof(int) );
  for (global_iface=0;
       global_iface<primal_nface(primal);
       global_iface++)
    { 
      local_iface = g2l[global_iface];
      if (EMPTY != local_iface) l2g[local_iface] = global_iface;
    }
  
  f2f = (int *)malloc( 3*local_nface*sizeof(int) );

  for (local_iface=0;local_iface<3*local_nface;local_iface++) 
    f2f[local_iface] = EMPTY;

  nseg = 0;
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = l2g[local_iface]; 
      primal_face(primal, global_iface, face);
      if (EMPTY == f2f[0+3*local_iface])
	{
	  f2f[0+3*local_iface] = nseg;
	  
	  if (KNIFE_SUCCESS == primal_find_face_side(primal, 
						     face[2], face[1], 
						     &global_other, &side)) 
	    {
	      local_other = g2l[global_other];
	      if (EMPTY == local_other) {
		printf("%s: %d: surface_from f2f failure\n",__FILE__,__LINE__);
		return NULL;
	      }
	      f2f[side+3*local_iface] = nseg;
	    }

	  nseg++;
	}
    }

  free(l2g);
  free(g2l);
  free(f2f);

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
