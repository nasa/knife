
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
  int other_global, other_local, other_side;
  int *face_g2l, *face_l2g, *f2f;
  int side, node0, node1;
  int nseg;
  int nnode;
  int *node_g2l;
  int global_node;
  int i;

  surface = surface_create( );

  face_g2l = (int *)malloc( primal_nface(primal)*sizeof(int) );
  for (global_iface=0;global_iface<primal_nface(primal);global_iface++) 
    face_g2l[global_iface] = EMPTY;

  local_nface = 0;
  for ( global_iface = 0 ; 
	global_iface < primal_nface(primal) ; 
	global_iface++ )
    {
      primal_face(primal, global_iface, face);
      if (array_contains_int(bcs,face[3])) 
	{
	  face_g2l[global_iface] = local_nface;
	  local_nface++;
	}
    }

  printf("number of triangular faces in the surface %d\n",local_nface);

  face_l2g = (int *)malloc( local_nface*sizeof(int) );
  for (global_iface=0;
       global_iface<primal_nface(primal);
       global_iface++)
    { 
      local_iface = face_g2l[global_iface];
      if (EMPTY != local_iface) face_l2g[local_iface] = global_iface;
    }
  
  f2f = (int *)malloc( 3*local_nface*sizeof(int) );

  for (local_iface=0;local_iface<local_nface;local_iface++) 
    {
      f2f[0+3*local_iface] = EMPTY;
      f2f[1+3*local_iface] = EMPTY;
      f2f[2+3*local_iface] = EMPTY;
    }

  nseg = 0;
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = face_l2g[local_iface]; 
      primal_face(primal, global_iface, face);
      for ( side = 0 ; side<3; side++ )
	if (EMPTY == f2f[side+3*local_iface])
	  {
	    f2f[side+3*local_iface] = nseg;
	    node0 = face[primal_face_side_node0(side)];
	    node1 = face[primal_face_side_node1(side)];
	    if (KNIFE_SUCCESS == primal_find_face_side(primal, 
						       node1, node0, 
						       &other_global, 
						       &other_side)) 
	      {
		other_local = face_g2l[other_global];
		if (EMPTY != other_local)
		  f2f[other_side+3*other_local] = nseg;
	      }

	    nseg++;
	  }
    }

  printf("number of segments in the surface %d\n",nseg);

  node_g2l = (int *)malloc( primal_nnode(primal)*sizeof(int) );
  for (global_node=0;global_node<primal_nnode(primal);global_node++) 
    node_g2l[global_node] = EMPTY;

  nnode = 0;
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = face_l2g[local_iface];
      primal_face(primal, global_iface, face);
      for (i=0;i<3;i++)
	if (EMPTY == node_g2l[face[i]]) 
	  {
	    node_g2l[face[i]] = nnode;
	    nnode++;
	  }
    }

  printf("number of nodes in the surface %d\n",nnode);

  free(face_l2g);
  free(face_g2l);
  free(f2f);
  free(node_g2l);

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
