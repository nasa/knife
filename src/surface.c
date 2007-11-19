
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

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

Surface surface_from( Primal primal, Set bcs, 
		      KnifeBool inward_pointing_normal  )
{
  Surface surface;
  int face[4];
  int local_iface, global_iface;
  int local_nface;
  int other_global, other_local, other_side;
  int *face_g2l, *face_l2g, *f2s;
  int side, node0, node1;
  int *node_g2l;
  int local_node, global_node;
  int i;
  double xyz[3];
  int *s2n;
  int segment_index;

  surface = (Surface)malloc( sizeof(SurfaceStruct) );
  if (NULL == surface) {
    printf("%s: %d: malloc failed in surface_from\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  face_g2l = (int *)malloc( primal_nface(primal)*sizeof(int) );
  for (global_iface=0;global_iface<primal_nface(primal);global_iface++) 
    face_g2l[global_iface] = EMPTY;

  local_nface = 0;
  for ( global_iface = 0 ; 
	global_iface < primal_nface(primal) ; 
	global_iface++ )
    {
      primal_face(primal, global_iface, face);
      if (set_contains(bcs,face[3])) 
	{
	  face_g2l[global_iface] = local_nface;
	  local_nface++;
	}
    }

  face_l2g = (int *)malloc( local_nface*sizeof(int) );
  for (global_iface=0;
       global_iface<primal_nface(primal);
       global_iface++)
    { 
      local_iface = face_g2l[global_iface];
      if (EMPTY != local_iface) face_l2g[local_iface] = global_iface;
    }
  
  f2s = (int *)malloc( 3*local_nface*sizeof(int) );

  for (local_iface=0;local_iface<local_nface;local_iface++) 
    {
      f2s[0+3*local_iface] = EMPTY;
      f2s[1+3*local_iface] = EMPTY;
      f2s[2+3*local_iface] = EMPTY;
    }

  surface->nsegment = 0;
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = face_l2g[local_iface]; 
      primal_face(primal, global_iface, face);
      for ( side = 0 ; side<3; side++ )
	if (EMPTY == f2s[side+3*local_iface])
	  {
	    f2s[side+3*local_iface] = surface->nsegment;
	    node0 = face[primal_face_side_node0(side)];
	    node1 = face[primal_face_side_node1(side)];
	    /* the other face may not be there if not watertight */
	    if (KNIFE_SUCCESS == primal_find_face_side(primal, 
						       node1, node0, 
						       &other_global, 
						       &other_side)) 
	      {
		other_local = face_g2l[other_global];
		if (EMPTY != other_local)
		  f2s[other_side+3*other_local] = surface->nsegment;
	      }

	    surface->nsegment++;
	  }
    }

  node_g2l = (int *)malloc( primal_nnode(primal)*sizeof(int) );
  for (global_node=0;global_node<primal_nnode(primal);global_node++) 
    node_g2l[global_node] = EMPTY;

  surface->nnode = 0;
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = face_l2g[local_iface];
      primal_face(primal, global_iface, face);
      for (i=0;i<3;i++)
	if (EMPTY == node_g2l[face[i]]) 
	  {
	    node_g2l[face[i]] = surface_nnode(surface);
	    (surface->nnode)++;
	  }
    }

  surface->node = (NodeStruct *) malloc( surface_nnode(surface) * 
					 sizeof(NodeStruct) );
  for (global_node=0;global_node<primal_nnode(primal);global_node++) 
    {
      local_node = node_g2l[global_node]; 
      if ( EMPTY != local_node )
	{
	  primal_xyz(primal, global_node, xyz);
	  node_initialize( surface_node(surface,local_node), xyz );
	}
    }

  s2n = (int *) malloc( 2 * surface->nsegment * sizeof(int));
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = face_l2g[local_iface]; 
      primal_face(primal, global_iface, face);
      for ( side = 0 ; side<3; side++ )
	{
	  node0 = MIN( face[primal_face_side_node0(side)],
		       face[primal_face_side_node1(side)] );
	  node1 = MAX( face[primal_face_side_node0(side)],
		       face[primal_face_side_node1(side)] );
	  segment_index = f2s[side+3*local_iface];
	  s2n[0+2*segment_index] = node_g2l[node0];
	  s2n[1+2*segment_index] = node_g2l[node1];
	}
    }

  surface->segment = (SegmentStruct *) malloc( surface->nsegment * 
					       sizeof(SegmentStruct) );
  for (segment_index=0;segment_index<surface->nsegment;segment_index++) 
    segment_initialize( surface_segment(surface,segment_index),
			surface_node(surface,s2n[0+2*segment_index]),
			surface_node(surface,s2n[1+2*segment_index]) );

  free(s2n);
  
  surface->ntriangle = local_nface;
  surface->triangle = (TriangleStruct *) malloc( local_nface * 
						 sizeof(TriangleStruct));
  for ( local_iface = 0 ; local_iface < local_nface ; local_iface++ )
    {
      global_iface = face_l2g[local_iface];
      primal_face(primal, global_iface, face);
      if ( inward_pointing_normal )
	{
	  triangle_initialize( surface_triangle(surface,local_iface),
			       surface_segment(surface,f2s[0+3*local_iface]),
			       surface_segment(surface,f2s[1+3*local_iface]),
			       surface_segment(surface,f2s[2+3*local_iface]),
			       face[3] );
	}
      else
	{
	  triangle_initialize( surface_triangle(surface,local_iface),
			       surface_segment(surface,f2s[1+3*local_iface]),
			       surface_segment(surface,f2s[0+3*local_iface]),
			       surface_segment(surface,f2s[2+3*local_iface]),
			       face[3] );
	}
      segment_add_triangle( surface_segment(surface,f2s[0+3*local_iface]), 
			    surface_triangle(surface,local_iface) );
      segment_add_triangle( surface_segment(surface,f2s[1+3*local_iface]), 
			    surface_triangle(surface,local_iface) );
      segment_add_triangle( surface_segment(surface,f2s[2+3*local_iface]), 
			    surface_triangle(surface,local_iface) );
    }

  free(face_l2g);
  free(face_g2l);
  free(f2s);
  free(node_g2l);

  return surface;
}

void surface_free( Surface surface )
{
  if ( NULL == surface ) return;
  free( surface->node );
  free( surface->segment );
  free( surface->triangle );
  free( surface );
}

KNIFE_STATUS surface_triangulate( Surface surface )
{
  int triangle_index;

  for ( triangle_index = 0;
	triangle_index < surface_ntriangle(surface); 
	triangle_index++)
    TRY( triangle_triangulate_cuts( surface_triangle(surface,triangle_index) ), 
				    "triangle_triangulate_cuts" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS surface_export_tec( Surface surface, char *filename )
{
  FILE *f;
  int node, tri;
  Triangle triangle;

  if (NULL == surface) return KNIFE_NULL;

  if (NULL == filename)
    {
      f = fopen( "surface.t", "w" );
    } else {
      f = fopen( filename, "w" );
    }

  fprintf( f, "title=\"tecplot knife surface geometry file\"\n" );
  fprintf( f, "variables=\"x\",\"y\",\"z\"\n" );

  if ( NULL == f ) return KNIFE_FILE_ERROR;

  fprintf( f,
	   "zone t=surf, i=%d, j=%d, f=fepoint, et=triangle\n",
	   surface_nnode(surface), surface_ntriangle(surface) );

  for ( node = 0 ; node < surface_nnode(surface) ; node++ )
    {
      fprintf( f, "%25.17e %25.17e %25.17e\n", 
	       node_x(surface_node(surface, node)),
	       node_y(surface_node(surface, node)),
	       node_z(surface_node(surface, node)) );
    }

  for ( tri = 0 ; tri < surface_ntriangle(surface) ; tri++ )
    {
      triangle = surface_triangle( surface, tri );
      fprintf(f, "%d %d %d\n", 
	      ( (int)(triangle_node0(triangle)-(surface->node)) )+1,
	      ( (int)(triangle_node1(triangle)-(surface->node)) )+1,
	      ( (int)(triangle_node2(triangle)-(surface->node)) )+1);
    }

  fclose(f);

  return KNIFE_SUCCESS;
}

