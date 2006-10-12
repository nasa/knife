
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

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
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
			   domain_segment(domain,edge2) );
    }

  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_dual_elements( Domain domain )
{
  int node;
  int cell, edge, tri;
  int side;
  int cell_center, tri_center, edge_center;
  int edge_index, segment_index, triangle_index;
  int tri_side, cell_side;
  int tri_nodes[3], cell_nodes[4];
  double xyz[3];
  int cell_edge;
  int segment0, segment1, segment2;
  int node0, node1;
  int poly;
  
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
  
  domain->nnode = 
    primal_ncell(domain->primal) +
    primal_ntri(domain->primal) +
    primal_nedge(domain->primal);
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

  domain->nsegment = 
    10 * primal_ncell(domain->primal) +
    3  * primal_ntri(domain->primal);
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

  domain->ntriangle = 12*primal_ncell(domain->primal);
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
			       domain_segment(domain,segment2));
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
	  triangle_initialize( domain_triangle(domain,triangle_index),
			       domain_segment(domain,segment0),
			       domain_segment(domain,segment1),
			       domain_segment(domain,segment2));
	  poly_add_triangle( domain_poly(domain,node0),
			     domain_triangle(domain,triangle_index), FALSE );
	  poly_add_triangle( domain_poly(domain,node1),
			     domain_triangle(domain,triangle_index), TRUE );
	}
    }

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
  KNIFE_STATUS code;

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

  code = domain_triangulate(domain);
  if (KNIFE_SUCCESS != code)
    {
      printf("%s: %d: domain_triangulate returned %d\n",
	     __FILE__,__LINE__,code);
      return code;
    }

  printf("triangulation complete\n");

  code = domain_gather_surf(domain);
  if (KNIFE_SUCCESS != code)
    {
      printf("%s: %d: domain_gather_surf returned %d\n",
	     __FILE__,__LINE__,code);
      return code;
    }

  printf("surface gathered\n");

  code = domain_determine_active_subtri(domain);
  if (KNIFE_SUCCESS != code)
    {
      printf("%s: %d: domain_determine_active_subtri returned %d\n",
	     __FILE__,__LINE__,code);
      return code;
    }

  printf("active subtris determined\n");

  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_triangulate( Domain domain )
{
  KNIFE_STATUS code;
  int triangle_index;

  code = surface_triangulate(domain->surface);
  if (KNIFE_SUCCESS != code)
    {
      printf("%s: %d: surface_triangulate returned %d\n",
	     __FILE__,__LINE__,code);
      return code;
    }

  printf("surface triangulated\n");

  for ( triangle_index = 0;
	triangle_index < domain_ntriangle(domain); 
	triangle_index++)
    {
      code = triangle_triangulate_cuts( domain_triangle(domain,
							triangle_index) );
      if (KNIFE_SUCCESS != code)
	{
	  printf("%s: %d: triangle_triangulate_cuts returned %d\n",
		 __FILE__,__LINE__,code);
	  return code;
	}
    }

  printf("volume triangulated\n");

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_gather_surf( Domain domain )
{
  KNIFE_STATUS code;
  int poly_index;
  int cut_poly;

  cut_poly =0;
  for ( poly_index = 0;
	poly_index < domain_npoly(domain); 
	poly_index++)
    {
      code = poly_gather_surf( domain_poly(domain,poly_index) );
      if (KNIFE_SUCCESS != code)
	{
	  printf("%s: %d: poly_gather_surf returned %d\n",
		 __FILE__,__LINE__,code);
	  return code;
	}
      if ( 0 < poly_nsurf( domain_poly(domain,poly_index) ) ) cut_poly++;
    }

  printf("polyhedra %d of %d have been cut\n",cut_poly,domain_npoly(domain));

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_determine_active_subtri( Domain domain )
{
  KNIFE_STATUS code;
  int poly_index;

  for ( poly_index = 0;
	poly_index < domain_npoly(domain); 
	poly_index++)
    {
      code = poly_determine_active_subtri( domain_poly(domain,poly_index) );
      if (KNIFE_SUCCESS != code)
	{
	  printf("%s: %d: poly_determine_active_subtri returned %d\n",
		 __FILE__,__LINE__,code);
	  return code;
	}
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS domain_tecplot( Domain domain, char *filename )
{
  FILE *f;
  int poly;

  if (NULL == filename) 
    {
      f = fopen("domain.t", "w");
    }
  else
    {
      f = fopen(filename, "w");
    }
  
  fprintf(f,"title='domain geometry'\nvariables='x','y','z'\n");
  
  for (poly = 0 ; poly < domain_npoly(domain) ; poly++)
    poly_tecplot_zone(domain_poly(domain,poly), f );

  fclose(f);
  
  return KNIFE_SUCCESS;
}
