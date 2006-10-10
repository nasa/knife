
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

BEGIN_C_DECLORATION
typedef struct TriangleStruct TriangleStruct;
typedef TriangleStruct * Triangle;
END_C_DECLORATION

#include "array.h"
#include "segment.h"
#include "node.h"
#include "subnode.h"
#include "subtri.h"
#include "intersection.h"

BEGIN_C_DECLORATION

struct TriangleStruct {
  Segment segment[3];
  Node node0, node1, node2;
  Array subnode;
  Array subtri;
  Array cut;
};

Triangle triangle_create(Segment segment0, Segment segment1, Segment segment2);
KNIFE_STATUS triangle_initialize(Triangle,
				 Segment segment0, 
				 Segment segment1, 
				 Segment segment2);
void triangle_free( Triangle );

#define triangle_segment(triangle,segment_index)	\
  ((triangle)->segment[segment_index])

#define triangle_xyz0(triangle) (node_xyz((triangle)->node0))
#define triangle_xyz1(triangle) (node_xyz((triangle)->node1))
#define triangle_xyz2(triangle) (node_xyz((triangle)->node2))

#define triangle_add_cut( triangle, new_cut )		\
  array_add( (triangle)->cut, (ArrayItem)(new_cut) )
#define triangle_ncut( triangle )  		\
  array_size( (triangle)->cut )
#define triangle_cut( triangle, cut_index )		\
  ((Cut)array_item( (triangle)->cut, (cut_index) ))

#define triangle_add_subnode( triangle, new_subnode )		\
  array_add( (triangle)->subnode, (ArrayItem)(new_subnode) )
#define triangle_nsubnode( triangle )		\
  array_size( (triangle)->subnode )
#define triangle_subnode( triangle, subnode_index )		\
  ((Subnode)array_item( (triangle)->subnode, (subnode_index) ))

#define triangle_add_subtri( triangle, new_subtri )		\
  array_add( (triangle)->subtri, (ArrayItem)(new_subtri) )
#define triangle_nsubtri( triangle )		\
  array_size( (triangle)->subtri )
#define triangle_subtri( triangle, subtri_index )		\
  ((Subtri)array_item( (triangle)->subtri, (subtri_index) ))

KNIFE_STATUS triangle_extent( Triangle, double *center, double *radius );

KNIFE_STATUS triangle_triangulate_cuts( Triangle );

Subnode triangle_unique_subnode( Triangle, Intersection );

Subnode triangle_subnode_with_intersection( Triangle, Intersection );

KNIFE_STATUS triangle_enclosing_subtri( Triangle, Subnode,
					Subtri, double *bary );

KNIFE_STATUS triangle_insert( Triangle, Subnode );
KNIFE_STATUS triangle_insert_into_side( Triangle, Subnode, 
					Subnode n0, Subnode n1 );

END_C_DECLORATION

#endif /* TRIANGLE_H */
