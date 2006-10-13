
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

#include "array.h"
#include "node.h"

BEGIN_C_DECLORATION

struct SegmentStruct {
  Node node0, node1;
  Array intersection;
};

KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 );
Node segment_common_node( Segment segment0, Segment segment1 );

#define segment_add_intersection( segment, intersection )	\
  array_add( (segment)->intersection, (ArrayItem)(intersection) )

#define segment_nintersection( segment ) \
  array_size( (segment)->intersection )

#define segment_intersection( segment, intersection_index )	\
  ((Intersection)array_item( (segment)->intersection, (intersection_index) ))

#define segment_xyz0(segment) (node_xyz((segment)->node0))
#define segment_xyz1(segment) (node_xyz((segment)->node1))

END_C_DECLORATION

#endif /* SEGMENT_H */
