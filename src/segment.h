
/* connection between two Nodes */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SEGMENT_H
#define SEGMENT_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct SegmentStruct SegmentStruct;
typedef SegmentStruct * Segment;
END_C_DECLORATION

#include "node.h"
#include "array.h"
#include "intersection.h"
#include "triangle.h"

BEGIN_C_DECLORATION

struct SegmentStruct {
  Node node0, node1;
  Array intersection;
  Array triangle;
};

KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 );
Node segment_common_node( Segment segment0, Segment segment1 );

#define segment_add_intersection( segment, new_intersection )	\
  array_add( (segment)->intersection, (ArrayItem)(new_intersection) )

#define segment_nintersection( segment ) \
  array_size( (segment)->intersection )

#define segment_intersection( segment, intersection_index )	\
  ((Intersection)array_item( (segment)->intersection, (intersection_index) ))

#define segment_add_triangle( segment, new_triangle )	\
  array_add( (segment)->triangle, (ArrayItem)(new_triangle) )

#define segment_ntriangle( segment ) \
  array_size( (segment)->triangle )

#define segment_triangle( segment, triangle_index )	\
  ((Triangle)array_item( (segment)->triangle, (triangle_index) ))

#define segment_xyz0(segment) (node_xyz((segment)->node0))
#define segment_xyz1(segment) (node_xyz((segment)->node1))

END_C_DECLORATION

#endif /* SEGMENT_H */
