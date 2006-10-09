
/* collection of segments and triangles defining a surface */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SURFACE_H
#define SURFACE_H

#include "knife_definitions.h"
#include "array.h"
#include "primal.h"
#include "node.h"
#include "segment.h"
#include "triangle.h"

BEGIN_C_DECLORATION

typedef struct SurfaceStruct SurfaceStruct;
struct SurfaceStruct {
  int nnode;
  NodeStruct *node;
  int nsegment;
  SegmentStruct *segment;
  int ntriangle;
  TriangleStruct *triangle;
};
typedef SurfaceStruct * Surface;

Surface surface_from( Primal, Array of_bcs );

void surface_free( Surface );

#define surface_nnode(surface) ((surface)->nnode)
#define surface_node(surface,node_index) \
  (&((surface)->node[(node_index)]))

#define surface_nsegment(surface) ((surface)->nsegment)
#define surface_segment(surface,segment_index) \
  (&((surface)->segment[(segment_index)]))

#define surface_ntriangle(surface) ((surface)->ntriangle)
#define surface_triangle(surface,triangle_index) \
  (&((surface)->triangle[(triangle_index)]))

END_C_DECLORATION

#endif /* SURFACE_H */
