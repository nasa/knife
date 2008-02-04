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

typedef int POLY_TOPO;

#define POLY_EXTERIOR (0)
#define POLY_CUT      (1)
#define POLY_INTERIOR (2)
#define POLY_GHOST    (3)

struct DomainStruct {
  Primal primal;

  Surface surface;

  int nnode;
  Node *node;

  int nsegment;
  Segment *segment;

  int ntriangle;
  Triangle *triangle;

  int npoly;
  Poly *poly;

  POLY_TOPO *topo;

  int nside;
  int *f2s;
  int *s2fs;
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
#define domain_surface(domain) ((domain)->surface)

#define domain_nnode(domain) ((domain)->nnode)
Node domain_node( Domain, int node_index );
Node domain_node_at_edge_center( Domain, int edge_index );

#define domain_nsegment(domain) ((domain)->nsegment)
Segment domain_segment( Domain, int segment_index );

#define domain_ntriangle(domain) ((domain)->ntriangle)
Triangle domain_triangle( Domain, int triangle_index );

#define domain_npoly0(domain) (primal_nnode0(domain_primal(domain)))

#define domain_npoly(domain) ((domain)->npoly)
#define domain_poly(domain,poly_index)					\
  ( ((poly_index) < 0 || (poly_index) > domain_npoly(domain) ) ?	\
    NULL:(domain)->poly[(poly_index)])

#define domain_topo(domain,poly_index)					\
  ((NULL == (domain)->topo)?POLY_INTERIOR:(domain)->topo[(poly_index)])

#define domain_cut(domain,poly_index) \
  (POLY_CUT == domain_topo(domain,poly_index))

#define domain_original(domain,poly_index) \
  (POLY_INTERIOR == domain_topo(domain,poly_index))

#define domain_active(domain,poly_index) \
  (domain_cut(domain,poly_index) || domain_original(domain,poly_index))

KNIFE_STATUS domain_required_local_dual( Domain domain, int *required );
KNIFE_STATUS domain_create_dual( Domain domain, int *required );

KNIFE_STATUS domain_add_interior_poly( Domain domain, int index );

KNIFE_STATUS domain_required_dual( Domain domain );
KNIFE_STATUS domain_all_dual( Domain domain );

KNIFE_STATUS domain_dual_elements( Domain domain );

KNIFE_STATUS domain_boolean_subtract( Domain );

KNIFE_STATUS domain_triangulate( Domain );
KNIFE_STATUS domain_gather_surf( Domain );
KNIFE_STATUS domain_determine_active_subtri( Domain );
KNIFE_STATUS domain_set_dual_topology( Domain );

KNIFE_STATUS domain_tecplot( Domain, char *filename );

END_C_DECLORATION

#endif /* DOMAIN_H */
