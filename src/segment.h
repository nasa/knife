
/* connection between two Nodes */

/* Copyright 2007 United States Government as represented by the
 * Administrator of the National Aeronautics and Space
 * Administration. No copyright is claimed in the United States under
 * Title 17, U.S. Code.  All Other Rights Reserved.
 *
 * The knife platform is licensed under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
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

Segment segment_create( Node node0, Node node1 );
KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 );
void segment_free( Segment );

Node segment_common_node( Segment segment0, Segment segment1 );

KNIFE_STATUS segment_extent( Segment segment, 
			     double *center, double *diameter );

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

#define segment_node0(segment) ((segment)->node0)
#define segment_node1(segment) ((segment)->node1)

#define segment_xyz0(segment) (node_xyz(segment_node0(segment)))
#define segment_xyz1(segment) (node_xyz(segment_node1(segment)))

END_C_DECLORATION

#endif /* SEGMENT_H */
