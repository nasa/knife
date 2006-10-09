
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

  domain->npoly = primal_ncell(domain->primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->poly,
		     "domain_tetrahedral_elements poly");

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
  domain->npoly = primal_nnode(domain->primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->poly,"domain_dual_elements poly");
  
  domain->ntriangle = 12*primal_ncell(domain->primal) + 
                       6*primal_nface(domain->primal);
  domain->triangle = (TriangleStruct *)malloc( domain->ntriangle * 
					       sizeof(TriangleStruct));
  domain_test_malloc(domain->triangle,"domain_dual_elements triangle");
  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_boolean_subtract( Domain domain )
{
  int triangle_index;
  int i;

  for ( triangle_index = 0;
	triangle_index < domain->ntriangle; 
	triangle_index++)
    {
      for (i=0;i<surface_ntriangle(domain->surface);i++)
	{
	  cut_between( domain_triangle(domain,triangle_index),
		       surface_triangle(domain->surface,i) );
	}
    }

  return (KNIFE_SUCCESS);
}

