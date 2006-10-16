
/* domain for PDE solvers */

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
#include "domain.h"
#include "cut.h"
#include "near.h"

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

Domain domain_create( Primal primal, Surface surface)
{
  Domain domain;

  domain = (Domain) malloc( sizeof(DomainStruct) );
  if (NULL == domain) {
    printf("%s: %d: malloc failed in domain_create\n",__FILE__,__LINE__);
    return NULL;
  }

  domain->primal = primal;
  domain->surface = surface;

  domain->npoly = EMPTY;
  domain->poly = NULL;

  domain->ntriangle = EMPTY;
  domain->triangle = NULL;

  return domain;
}

void domain_free( Domain domain )
{
  if ( NULL == domain ) return;
  
  if ( NULL != domain->poly ) free(domain->poly);
  if ( NULL != domain->triangle ) free(domain->triangle);

  free(domain);
}

KNIFE_STATUS domain_tetrahedral_elements( Domain domain )
{
  int node;
  double xyz[3];
  int edge;
  int edge_nodes[2];
  int tri;
  int tri_nodes[3];
  int edge0, edge1, edge2;
  int poly;

  domain->npoly = primal_ncell(domain->primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->poly,
		     "domain_tetrahedral_elements poly");
  for (poly = 0 ; poly < domain_npoly(domain) ; poly++)
    poly_initialize(domain_poly(domain,poly));

  domain->nnode = primal_nnode(domain->primal);
  domain->node = (NodeStruct *)malloc( domain->nnode * 
				       sizeof(NodeStruct));
  domain_test_malloc(domain->node,
		     "domain_tetrahedral_elements node");
  for (node = 0 ; node < primal_nnode(domain->primal) ; node++)
    {
      TRY( primal_xyz( domain->primal, node, xyz), "xyz" );
      node_initialize( domain_node(domain,node), xyz, node);
    }

  domain->nsegment = primal_nedge(domain->primal);
  domain->segment = (SegmentStruct *)malloc( domain->nsegment * 
					       sizeof(SegmentStruct));
  domain_test_malloc(domain->segment,
		     "domain_tetrahedral_elements segment");
  for (edge = 0 ; edge < primal_nedge(domain->primal) ; edge++)
    {
      TRY( primal_edge( domain->primal, edge, edge_nodes), "primal_edge" );
      segment_initialize( domain_segment(domain,edge),
			  domain_node(domain,edge_nodes[0]),
			  domain_node(domain,edge_nodes[1]));
    }

  domain->ntriangle = primal_ntri(domain->primal);
  domain->triangle = (TriangleStruct *)malloc( domain->ntriangle * 
					       sizeof(TriangleStruct));
  domain_test_malloc(domain->triangle,
		     "domain_tetrahedral_elements triangle");
  for (tri = 0 ; tri < primal_ntri(domain->primal) ; tri++)
    {
      TRY( primal_tri( domain->primal, tri, tri_nodes), "tri" );
      TRY( primal_find_edge( domain->primal, 
			     tri_nodes[0], tri_nodes[1],
			     &edge2 ), "edge2" );
      TRY( primal_find_edge( domain->primal, 
			     tri_nodes[1], tri_nodes[2],
			     &edge0 ), "edge0" );
      TRY( primal_find_edge( domain->primal, 
			     tri_nodes[2], tri_nodes[0],
			     &edge1 ), "edge1" );
      triangle_initialize( domain_triangle(domain,tri),
			   domain_segment(domain,edge0),
			   domain_segment(domain,edge1),
			   domain_segment(domain,edge2),
			   FALSE );
    }

  printf("domain_tetrahedral_elements: implement on_boundary determination\n");
  return (KNIFE_IMPLEMENT);
}

KNIFE_STATUS domain_dual_elements( Domain domain )
{
  int node;
  int cell, edge, tri, face;
  int side;
  int cell_center, tri_center, edge_center;
  int edge_index, segment_index, triangle_index;
  int tri_side, cell_side;
  int tri_nodes[3], cell_nodes[4], face_nodes[4];
  double xyz[3];
  int cell_edge;
  int segment0, segment1, segment2;
  int node0, node1;
  int poly;
  int surface_nnode;
  int *node_g2l;
  int node_index;
  int other_face, other_side;
  int *f2s;

  printf("primal: nnode %d nface %d ncell %d nedge %d ntri %d\n",
	 primal_nnode(domain->primal),
	 primal_nface(domain->primal),
	 primal_ncell(domain->primal),
	 primal_nedge(domain->primal),
	 primal_ntri(domain->primal));

  domain->npoly = primal_nnode(domain->primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->poly,"domain_dual_elements poly");
  for (poly = 0 ; poly < domain_npoly(domain) ; poly++)
    poly_initialize(domain_poly(domain,poly));
  

  node_g2l = (int *)malloc( primal_nnode(domain->primal)*sizeof(int) );
  for ( node = 0 ; node < primal_nnode(domain->primal) ; node++) 
    node_g2l[node] = EMPTY;

  surface_nnode = 0;
  for ( face = 0 ; face < primal_nface(domain->primal) ; face++ )
    {
      primal_face(domain->primal, face, face_nodes);
      for (node=0;node<3;node++)
	if (EMPTY == node_g2l[face_nodes[node]]) 
	  {
	    node_g2l[face_nodes[node]] = surface_nnode;
	    surface_nnode++;
	  }
    }

  printf("number of nodes in the surface %d\n",surface_nnode);

  domain->nnode = 
    primal_ncell(domain->primal) +
    primal_ntri(domain->primal) +
    primal_nedge(domain->primal) +
    surface_nnode;
  domain->node = (NodeStruct *)malloc( domain->nnode * 
				       sizeof(NodeStruct));
  domain_test_malloc(domain->node,
		     "domain_tetrahedral_elements node");
  printf("number of dual nodes in the volume %d\n",domain->nnode);
  for ( cell = 0 ; cell < primal_ncell(domain->primal) ; cell++)
    {
      node = cell;
      TRY( primal_cell_center( domain->primal, cell, xyz), "cell center" );
      node_initialize( domain_node(domain,node), xyz, node);
    }
  for ( tri = 0 ; tri < primal_ntri(domain->primal) ; tri++)
    {
      node = tri + primal_ncell(domain->primal);
      TRY( primal_tri_center( domain->primal, tri, xyz), "tri center" );
      node_initialize( domain_node(domain,node), xyz, node);
    }
  for ( edge = 0 ; edge < primal_nedge(domain->primal) ; edge++)
    {
      node = edge + primal_ntri(domain->primal) + primal_ncell(domain->primal);
      TRY( primal_edge_center( domain->primal, edge, xyz), "edge center" );
      node_initialize( domain_node(domain,node), xyz, node);
    }
  for ( node_index = 0 ; 
	node_index < primal_nnode(domain->primal) ; 
	node_index++) 
    if ( EMPTY != node_g2l[node_index] )
      {
	node = 
	  node_g2l[node_index] + 
	  primal_nedge(domain->primal) + 
	  primal_ntri(domain->primal) + 
	  primal_ncell(domain->primal);
	primal_xyz(domain->primal,node_index,xyz);
	node_initialize( domain_node(domain,node), xyz, node);
      }

  domain->nsegment = 
    10 * primal_ncell(domain->primal) +
    3  * primal_ntri(domain->primal) +
    3  * primal_nface(domain->primal);

  f2s = (int *)malloc( 3*primal_nface(domain->primal)*sizeof(int) );

  for ( face = 0 ; face < primal_nface(domain->primal) ; face++ ) 
    {
      f2s[0+3*face] = EMPTY;
      f2s[1+3*face] = EMPTY;
      f2s[2+3*face] = EMPTY;
    }

  for ( face = 0 ; face < primal_nface(domain->primal) ; face++ ) 
    {
      primal_face(domain->primal, face, face_nodes);
      for ( side = 0 ; side<3; side++ )
	if (EMPTY == f2s[side+3*face])
	  {
	    f2s[side+3*face] = domain->nsegment;
	    node0 = face_nodes[primal_face_side_node0(side)];
	    node1 = face_nodes[primal_face_side_node1(side)];
	    TRY( primal_find_face_side(domain->primal, node1, node0, 
				       &other_face, &other_side), "face_side"); 
	    f2s[other_side+3*other_face] = domain->nsegment;
	    domain->nsegment += 2; /* a tri side has 2 segments */
	  }
    }

  domain->segment = (SegmentStruct *)malloc( domain->nsegment * 
					       sizeof(SegmentStruct));
  domain_test_malloc(domain->segment,
		     "domain_tetrahedral_elements segment");
  printf("number of dual segments in the volume %d\n",domain->nsegment);

  for ( cell = 0 ; cell < primal_ncell(domain->primal) ; cell++)
    {
      cell_center = cell;
      for ( side = 0 ; side < 4 ; side++)
	{
	  tri = primal_c2t(domain->primal,cell,side);
	  tri_center = tri + primal_ncell(domain->primal);
	  segment_index = side + 10 * cell;
	  segment_initialize( domain_segment(domain,segment_index),
			      domain_node(domain,cell_center),
			      domain_node(domain,tri_center));
	}
      for ( edge = 0 ; edge < 6 ; edge++)
	{
	  edge_index = primal_c2e(domain->primal,cell,edge);
	  edge_center = edge_index + primal_ntri(domain->primal) 
	                           + primal_ncell(domain->primal);
	  segment_index = edge + 4 + 10 * cell;
	  segment_initialize( domain_segment(domain,segment_index),
			      domain_node(domain,cell_center),
			      domain_node(domain,edge_center));
	}
    }

  for ( tri = 0 ; tri < primal_ntri(domain->primal) ; tri++)
    {
      tri_center = tri + primal_ncell(domain->primal);;
      primal_tri(domain->primal,tri,tri_nodes);
      for ( side = 0 ; side < 3 ; side++)
	{
	  TRY( primal_find_edge( domain->primal, 
				 tri_nodes[primal_face_side_node0(side)], 
				 tri_nodes[primal_face_side_node1(side)], 
				 &edge_index ), "tri seg find edge" );
	  edge_center = edge_index + primal_ntri(domain->primal) 
	                           + primal_ncell(domain->primal);
	  segment_index = side + 3 * tri + 10 * primal_ncell(domain->primal);
	  segment_initialize( domain_segment(domain,segment_index),
			      domain_node(domain,tri_center),
			      domain_node(domain,edge_center));
	}
    }

  for ( face = 0 ; face < primal_nface(domain->primal) ; face++)
    {
      primal_face(domain->primal, face, face_nodes);
      TRY( primal_find_tri( domain->primal, 
			    face_nodes[0], face_nodes[1], face_nodes[2],
			    &tri ), "find tri for face" );
      tri_center = tri + primal_ncell(domain->primal);;
      primal_tri(domain->primal,tri,tri_nodes);
      for ( node = 0 ; node < 3 ; node++)
	{
	  segment_index = node + 3 * face + 
	    3 *primal_ntri(domain->primal) + 10 * primal_ncell(domain->primal);
	  node_index = 
	    node_g2l[face_nodes[node]] + 
	    primal_nedge(domain->primal) + 
	    primal_ntri(domain->primal) + 
	    primal_ncell(domain->primal);
	  segment_initialize( domain_segment(domain,segment_index),
			      domain_node(domain,tri_center),
			      domain_node(domain,node_index));
	}
      for ( side = 0 ; side < 3 ; side++)
	{
	  node0 = face_nodes[primal_face_side_node0(side)];
	  node1 = face_nodes[primal_face_side_node1(side)];
	  TRY( primal_find_face_side(domain->primal, node1, node0, 
				     &other_face, &other_side), "i face_side"); 
	  if ( face < other_face ) /* only init segment once */
	    {
	      TRY( primal_find_edge( domain->primal, node0, node1, 
				     &edge_index ), "face seg find edge" );
	      edge_center = edge_index + primal_ntri(domain->primal) 
	                  + primal_ncell(domain->primal);
	      segment_index = 0 + f2s[side+3*face];
	      node_index = 
		node_g2l[node0] + 
		primal_nedge(domain->primal) + 
		primal_ntri(domain->primal) + 
		primal_ncell(domain->primal);
	      segment_initialize( domain_segment(domain,segment_index),
				  domain_node(domain,node_index),
				  domain_node(domain,edge_center));
	      segment_index = 1 + f2s[side+3*face];
	      node_index = 
		node_g2l[node1] + 
		primal_nedge(domain->primal) + 
		primal_ntri(domain->primal) + 
		primal_ncell(domain->primal);
	      segment_initialize( domain_segment(domain,segment_index),
				  domain_node(domain,edge_center),
				  domain_node(domain,node_index));
	    }
	}
    }

  domain->ntriangle = 12*primal_ncell(domain->primal)
                    +  6*primal_nface(domain->primal);
  domain->triangle = (TriangleStruct *)malloc( domain->ntriangle * 
					       sizeof(TriangleStruct));
  domain_test_malloc(domain->triangle,"domain_dual_elements triangle");
  printf("number of dual triangles in the volume %d\n",domain->ntriangle);

  for ( cell = 0 ; cell < primal_ncell(domain->primal) ; cell++)
    {
      for ( cell_edge = 0 ; cell_edge < 6 ; cell_edge++)
	{
	  primal_cell(domain->primal,cell,cell_nodes);
	  node0 = cell_nodes[primal_cell_edge_node0(cell_edge)];
	  node1 = cell_nodes[primal_cell_edge_node1(cell_edge)];

	  cell_side = primal_cell_edge_left_side(cell_edge);
	  tri = primal_c2t(domain->primal,cell,cell_side);
	  TRY( primal_find_tri_side( domain->primal, tri, node0, node1,
				     &tri_side ), "dual int find lf tri side");
	  triangle_index = 0 + 2 * cell_edge + 12 * cell;
	  segment0 = cell_side + 10 * cell;
	  segment1 = tri_side + 3 * tri + 10 * primal_ncell(domain->primal);
	  segment2 = cell_edge + 4 + 10 * cell;
	  /* triangle normal points from node0 to node1 */
	  triangle_initialize( domain_triangle(domain,triangle_index),
			       domain_segment(domain,segment0),
			       domain_segment(domain,segment1),
			       domain_segment(domain,segment2), FALSE );
	  poly_add_triangle( domain_poly(domain,node0),
			     domain_triangle(domain,triangle_index), FALSE );
	  poly_add_triangle( domain_poly(domain,node1),
			     domain_triangle(domain,triangle_index), TRUE );

	  cell_side = primal_cell_edge_right_side(cell_edge);
	  tri = primal_c2t(domain->primal,cell,cell_side);
	  TRY( primal_find_tri_side( domain->primal, tri, node0, node1,
				     &tri_side ), "dual int find rt tri side");
	  triangle_index = 1 + 2 * cell_edge + 12 * cell;
	  segment0 = cell_side + 10 * cell;
	  segment1 = cell_edge + 4 + 10 * cell;
	  segment2 = tri_side + 3 * tri + 10 * primal_ncell(domain->primal);
	  /* triangle normal points from node0 to node1 */
	  TRY( triangle_initialize( domain_triangle(domain,triangle_index),
				    domain_segment(domain,segment0),
				    domain_segment(domain,segment1),
				    domain_segment(domain,segment2), FALSE ), 
	       "int tri init");
	  poly_add_triangle( domain_poly(domain,node0),
			     domain_triangle(domain,triangle_index), FALSE );
	  poly_add_triangle( domain_poly(domain,node1),
			     domain_triangle(domain,triangle_index), TRUE );
	}
    }

  for ( face = 0 ; face < primal_nface(domain->primal) ; face++)
    {
      primal_face(domain->primal, face, face_nodes);
      TRY( primal_find_tri( domain->primal, 
			    face_nodes[0], face_nodes[1], face_nodes[2],
			    &tri ), "find tri for triangle init" );
      for ( side = 0 ; side < 3 ; side++)
	{
	  node0 = face_nodes[primal_face_side_node0(side)];
	  node1 = face_nodes[primal_face_side_node1(side)];
	  TRY( primal_find_face_side(domain->primal, node1, node0, 
				     &other_face, &other_side), "u face_side"); 
	  TRY( primal_find_tri_side( domain->primal, tri, node0, node1,
				     &tri_side ), "dual int find rt tri side");
	  triangle_index= 0 + 2*side + 6*face + 12*primal_ncell(domain->primal);
	  segment0 = tri_side + 3 * tri + 10 * primal_ncell(domain->primal);
	  segment1 = primal_face_side_node0(side) + 3 * face + 
	    3 *primal_ntri(domain->primal) + 10 * primal_ncell(domain->primal);
	  segment2 = 0 + f2s[side+3*face];
	  if (other_face < face) segment2 = 1 + f2s[side+3*face];
	  TRY( triangle_initialize( domain_triangle(domain,triangle_index),
				    domain_segment(domain,segment0),
				    domain_segment(domain,segment1),
				    domain_segment(domain,segment2), TRUE),
	       "boundary tri init 0");
	  poly_add_triangle( domain_poly(domain,node0),
			     domain_triangle(domain,triangle_index), TRUE );

	  triangle_index= 1 + 2*side + 6*face + 12*primal_ncell(domain->primal);
	  segment0 = tri_side + 3 * tri + 10 * primal_ncell(domain->primal);
	  segment1 = 1 + f2s[side+3*face];
	  if (other_face < face) segment1 = 0 + f2s[side+3*face];
	  segment2 = primal_face_side_node1(side) + 3 * face + 
	    3 *primal_ntri(domain->primal) + 10 * primal_ncell(domain->primal);
	  TRY( triangle_initialize( domain_triangle(domain,triangle_index),
				    domain_segment(domain,segment0),
				    domain_segment(domain,segment1),
				    domain_segment(domain,segment2), TRUE),
	       "boundary tri init 1");
	  poly_add_triangle( domain_poly(domain,node1),
			     domain_triangle(domain,triangle_index), TRUE );
	}
    }
	  
  free(node_g2l);
  free(f2s);
  
  printf("dual completed\n");

  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_boolean_subtract( Domain domain )
{
  int triangle_index;
  int i;
  NearStruct *near_tree;
  double center[3], diameter;
  int max_touched, ntouched;
  int *touched;
  NearStruct target;

  printf("forming surface near tree\n");

  near_tree = (NearStruct *)malloc( surface_ntriangle(domain->surface) * 
				    sizeof(NearStruct));
  for (triangle_index=0;
       triangle_index<surface_ntriangle(domain->surface);
       triangle_index++)
    {
      triangle_extent(surface_triangle(domain->surface,triangle_index),
		      center, &diameter);
      near_initialize( &(near_tree[triangle_index]), 
		       triangle_index, 
		       center[0], center[1], center[2], 
		       diameter );
      if (triangle_index > 0) near_insert( near_tree,
					   &(near_tree[triangle_index]) );
    }

  printf("surface near tree formed\n");

  printf("compute cuts\n");

  max_touched = surface_ntriangle(domain->surface);

  touched = (int *) malloc( max_touched * sizeof(int) );

  for ( triangle_index = 0;
	triangle_index < domain_ntriangle(domain); 
	triangle_index++)
    {
      triangle_extent(domain_triangle(domain,triangle_index),
		      center, &diameter);
      near_initialize( &target, 
		       EMPTY, 
		       center[0], center[1], center[2], 
		       diameter );
      ntouched = 0;
      near_touched(near_tree, &target, &ntouched, max_touched, touched);
      for (i=0;i<ntouched;i++)
	{
	  cut_between( domain_triangle(domain,triangle_index),
		       surface_triangle(domain->surface,touched[i]) );
	}
    }

  free(touched);

  printf("cuts computed\n");

  printf("start triangulation\n");

  TRY( domain_triangulate(domain), "domain_triangulate" );

  printf("triangulation complete\n");

  printf("gather surface\n");

  TRY( domain_gather_surf(domain), "domain_gather_surf" );

  printf("determine active subtris\n");

  TRY( domain_determine_active_subtri(domain), 
       "domain_determine_active_subtri" );

  printf("dual_topology\n");

  TRY( domain_set_dual_topology( domain ), "domain_set_dual_topology" );

  printf("boolean subtract completed\n");

  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_triangulate( Domain domain )
{
  int triangle_index;

  TRY( surface_triangulate(domain->surface), "surface_triangulate" );

  printf("surface triangulated\n");

  for ( triangle_index = 0;
	triangle_index < domain_ntriangle(domain); 
	triangle_index++)
    TRY( triangle_triangulate_cuts( domain_triangle(domain, triangle_index) ), 
	 "volume triangulate_cuts" );

  printf("volume triangulated\n");

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_gather_surf( Domain domain )
{
  int poly_index;
  int cut_poly;

  cut_poly = 0;
  for ( poly_index = 0;
	poly_index < domain_npoly(domain); 
	poly_index++)
    {
      TRY( poly_gather_surf( domain_poly(domain,poly_index) ), "poly_gather_surf" );
      if ( poly_has_surf( domain_poly( domain, poly_index ) ) ) cut_poly++;
    }

  printf("polyhedra %d of %d have been cut\n",cut_poly,domain_npoly(domain));

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_determine_active_subtri( Domain domain )
{
  int poly_index;

  for ( poly_index = 0;
	poly_index < domain_npoly(domain); 
	poly_index++)
    if ( poly_has_surf( domain_poly( domain, poly_index ) ) )
      TRY( poly_determine_active_subtri( domain_poly(domain,poly_index) ),
	   "poly_determine_active_subtri" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_set_dual_topology( Domain domain )
{
  int poly_index;
  int edge;
  int edge_nodes[2];
  Poly poly0, poly1;
  POLY_TOPO topo0, topo1;
  int node_index;
  Node node;
  KnifeBool active;

  KnifeBool requires_another_sweep;

  if (NULL == domain) return KNIFE_NULL;

  for ( poly_index = 0;
	poly_index < domain_npoly(domain); 
	poly_index++)
    {
      if ( poly_has_surf( domain_poly( domain, poly_index ) ) ) 
	domain_poly(domain,poly_index)->topo = POLY_CUT;
    }

  for (edge = 0 ; edge < primal_nedge(domain->primal) ; edge++)
    {
      node_index = edge + 
	primal_ntri(domain->primal) + primal_ncell(domain->primal);
      node = domain_node(domain,node_index);
      TRY( primal_edge( domain->primal, edge, edge_nodes), 
	   "dual_topo cut int primal_edge" );
      poly0 = domain_poly(domain,edge_nodes[0]);
      topo0 = poly_topo(poly0);
      poly1 = domain_poly(domain,edge_nodes[1]);
      topo1 = poly_topo(poly1);

      if ( POLY_CUT == topo0 && POLY_INTERIOR == topo1 )
	{
	  TRY( poly_mask_surrounding_node_activity( poly0, node,
						    &active ), "active01");
	  if ( !active ) poly_topo(poly1) = POLY_EXTERIOR;
	}

      if ( POLY_CUT == topo1 && POLY_INTERIOR == topo0 )
	{
	  TRY( poly_mask_surrounding_node_activity( poly1, node,
						    &active ), "active10");
	  if ( !active ) poly_topo(poly0) = POLY_EXTERIOR;
	}

    }

  requires_another_sweep = TRUE;
  while (requires_another_sweep) 
    {
      requires_another_sweep = FALSE;
      for (edge = 0 ; edge < primal_nedge(domain->primal) ; edge++)
	{
	  TRY( primal_edge( domain->primal, edge, edge_nodes), 
	       "dual_topo ext int primal_edge" );
	  poly0 = domain_poly(domain,edge_nodes[0]);
	  topo0 = poly_topo(poly0);
	  poly1 = domain_poly(domain,edge_nodes[1]);
	  topo1 = poly_topo(poly1);

	  if ( POLY_EXTERIOR == topo0 && POLY_INTERIOR == topo1 )
	    {
	      requires_another_sweep = TRUE;
	      poly_topo(poly1) = POLY_EXTERIOR;
	    }

	  if ( POLY_EXTERIOR == topo1 && POLY_INTERIOR == topo0 )
	    {
	      requires_another_sweep = TRUE;
	      poly_topo(poly0) = POLY_EXTERIOR;
	    }
	}

    }

  {
    int poly_index;
    int ninterior, ncut, nexterior;

    ninterior = 0;
    ncut = 0;
    nexterior = 0;
    
    for ( poly_index = 0;
	  poly_index < domain_npoly(domain);
	  poly_index++)
      {
	if ( POLY_INTERIOR == poly_topo( domain_poly( domain, poly_index ) ) )
	  ninterior++;
	if ( POLY_CUT      == poly_topo( domain_poly( domain, poly_index ) ) )
	  ncut++;
	if ( POLY_EXTERIOR == poly_topo( domain_poly( domain, poly_index ) ) )
	  nexterior++;
      }

    printf( "poly: %d interior %d cut %d exterior\n",
	    ninterior, ncut, nexterior);
  }
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_export_fun3d( Domain domain )
{
  int poly_index;
  Poly poly;
  int nnode;
  double xyz[3], center[3], volume;
  int *node_g2l;
  int node;
  FILE *f;

  node_g2l = (int *)malloc( primal_nnode(domain->primal)*sizeof(int) );
  for ( node = 0 ; node < primal_nnode(domain->primal) ; node++)
    node_g2l[node] = EMPTY;

  nnode = 0;
  for ( poly_index = 0;
	poly_index < domain_npoly(domain);
	poly_index++)
    {
      poly = domain_poly(domain,poly_index);
      if (poly_active(poly))
	{
	  node_g2l[poly_index] = nnode;
	  nnode++;
	}
    }

  f = fopen("postslice.nodes","w");
  NOT_NULL(f,"nodes file not open");

  fprintf(f,"%d\n",nnode);

  for ( poly_index = 0;
	poly_index < domain_npoly(domain);
	poly_index++)
    {
      if ( EMPTY != node_g2l[poly_index] )
	{
	  poly = domain_poly(domain,poly_index);
	  primal_xyz(domain_primal(domain),poly_index,xyz);
	  if (poly_cut(poly)) 
	    {
	      TRY( poly_centroid_volume(poly,xyz,center,&volume), "Cent vol");
	      fprintf(f,"%30.20e %30.20e %30.20e\n",
		      center[0],center[1],center[2]);	  
	    }
	  else
	    {
	      fprintf(f,"%30.20e %30.20e %30.20e\n",
		      xyz[0],xyz[1],xyz[2]);	  
	    }
	}
    }

  for ( poly_index = 0;
	poly_index < domain_npoly(domain);
	poly_index++)
    {
      if ( EMPTY != node_g2l[poly_index] )
	{
	  poly = domain_poly(domain,poly_index);
	  primal_xyz(domain_primal(domain),poly_index,xyz);
	  TRY( poly_centroid_volume(poly,xyz,center,&volume), "cent Vol");
	  fprintf(f,"%30.20e\n",volume);	  
	}
    }

  fclose(f);

  free(node_g2l);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_tecplot( Domain domain, char *filename )
{
  FILE *f;
  int poly;

  if (NULL == filename) 
    {
      printf("tecplot output to domain.t\n");
      f = fopen("domain.t", "w");
    }
  else
    {
      printf("tecplot output to %s\n",filename);
      f = fopen(filename, "w");
    }
  
  fprintf(f,"title=domain_geometry\nvariables=x,y,z\n");
  
  for (poly = 0 ; poly < domain_npoly(domain) ; poly++)
    if ( poly_active( domain_poly(domain,poly) ) )
      TRY( poly_tecplot_zone(domain_poly(domain,poly), f ), 
	   "poly_tecplot_zone");

  fclose(f);

  printf("tecplot output complete\n");
  
  return KNIFE_SUCCESS;
}
