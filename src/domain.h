/* domain for PDE solvers */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef DOMAIN_H
#define DOMAIN_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct DomainStruct DomainStruct;
typedef DomainStruct * Domain;
END_C_DECLORATION

#include "primal.h"
#include "surface.h"
#include "poly.h"
#include "node.h"
#include "segment.h"
#include "triangle.h"

BEGIN_C_DECLORATION

struct DomainStruct {
  Primal primal;

  Surface surface;

  int npoly;
  PolyStruct *poly;

  int nnode;
  NodeStruct *node;

  int nsegment;
  SegmentStruct *segment;

  int ntriangle;
  TriangleStruct *triangle;
};

#define domain_test_malloc(ptr,fcn)		       \
  if (NULL == (ptr)) {				       \
    printf("%s: %d: malloc failed in %s\n",	       \
	   __FILE__,__LINE__,(fcn));		       \
    return KNIFE_MEMORY;			       \
  }

Domain domain_create( Primal, Surface );
void domain_free( Domain );

#define domain_primal(domain) ((domain)->primal)

#define domain_npoly(domain) ((domain)->npoly)
#define domain_poly(domain,poly_index) \
  (&((domain)->poly[(poly_index)]))

#define domain_nnode(domain) ((domain)->nnode)
#define domain_node(domain,node_index) \
  (&((domain)->node[(node_index)]))

#define domain_nsegment(domain) ((domain)->nsegment)
#define domain_segment(domain,segment_index) \
  (&((domain)->segment[(segment_index)]))

#define domain_ntriangle(domain) ((domain)->ntriangle)
#define domain_triangle(domain,triangle_index) \
  (&((domain)->triangle[(triangle_index)]))

KNIFE_STATUS domain_tetrahedral_elements( Domain );
KNIFE_STATUS domain_dual_elements( Domain );

KNIFE_STATUS domain_boolean_subtract( Domain );

KNIFE_STATUS domain_triangulate( Domain );
KNIFE_STATUS domain_gather_surf( Domain );
KNIFE_STATUS domain_determine_active_subtri( Domain );
KNIFE_STATUS domain_set_dual_topology( Domain );

KNIFE_STATUS domain_export_fun3d( Domain );

KNIFE_STATUS domain_tecplot( Domain, char *filename );

END_C_DECLORATION

#endif /* DOMAIN_H */
