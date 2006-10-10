
/* defined by three segments and their three shared nodes */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "knife_definitions.h"
#include "segment.h"
#include "node.h"

BEGIN_C_DECLORATION

typedef struct TriangleStruct TriangleStruct;
struct TriangleStruct {
  Segment segment[3];
  Node node0, node1, node2;
};
typedef TriangleStruct * Triangle;

Triangle triangle_create(Segment segment0, Segment segment1, Segment segment2);
KNIFE_STATUS triangle_initialize(Triangle,
				 Segment segment0, 
				 Segment segment1, 
				 Segment segment2);
void triangle_free( Triangle );

#define triangle_xyz0(triangle) (node_xyz((triangle)->node0))
#define triangle_xyz1(triangle) (node_xyz((triangle)->node1))
#define triangle_xyz2(triangle) (node_xyz((triangle)->node2))

#define triangle_segment(triangle,segment_index)	\
  ((triangle)->segment[segment_index])

KNIFE_STATUS triangle_extent( Triangle, double *center, double *radius );

KNIFE_STATUS triangle_triangulate_cuts( Triangle );

END_C_DECLORATION

#endif /* TRIANGLE_H */
