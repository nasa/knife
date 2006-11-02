
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

#include <stdlib.h>
#include <stdio.h>
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
#include "cut.h"

BEGIN_C_DECLORATION

struct TriangleStruct {
  int boundary_face_index;
  Segment segment[3];
  Node node0, node1, node2;
  Array subnode;
  Array subtri;
  Array cut;
};

Triangle triangle_create(Segment segment0, Segment segment1, Segment segment2,
			 int boundary_face_index );
KNIFE_STATUS triangle_initialize(Triangle,
				 Segment segment0, 
				 Segment segment1, 
				 Segment segment2,
				 int boundary_face_index );
void triangle_free( Triangle );

#define triangle_segment(triangle,segment_index)	\
  ((triangle)->segment[segment_index])

#define triangle_has1(triangle,node)		\
  ( (triangle)->node0 == node ||		\
    (triangle)->node1 == node ||		\
    (triangle)->node2 == node )
    
#define triangle_has2(triangle,n0,n1) \
  ( ((triangle)->node0==n0||(triangle)->node1==n0||(triangle)->node2==n0) && \
    ((triangle)->node0==n1||(triangle)->node1==n1||(triangle)->node2==n1) )

#define triangle_boundary_face_index(triangle) \
  ((triangle)->boundary_face_index)

#define triangle_on_boundary(triangle) \
  (EMPTY != triangle_boundary_face_index(triangle))

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
#define triangle_remove_subtri( triangle, old_subtri )		\
  array_remove( (triangle)->subtri, (ArrayItem)(old_subtri) )
#define triangle_nsubtri( triangle )		\
  array_size( (triangle)->subtri )
#define triangle_subtri( triangle, subtri_index )		\
  ((Subtri)array_item( (triangle)->subtri, (subtri_index) ))

KNIFE_STATUS triangle_set_tecplot_frame( int frame );

KNIFE_STATUS triangle_extent( Triangle, double *center, double *radius );
KNIFE_STATUS triangle_normal_area( Triangle, double *normal, double *area );

KNIFE_STATUS triangle_neighbor( Triangle, Segment, Triangle *other );

KNIFE_STATUS triangle_triangulate_cuts( Triangle );

KNIFE_STATUS triangle_verify_subtri_area( Triangle );

KNIFE_STATUS triangle_insert_unique_subnode( Triangle, Intersection );

Subnode triangle_subnode_with_intersection( Triangle, Intersection );

KNIFE_STATUS triangle_enclosing_subtri( Triangle, Subnode,
					Subtri *enclosing, double *bary );

KNIFE_STATUS triangle_insert( Triangle, Subnode );
KNIFE_STATUS triangle_insert_into_side( Triangle, Subnode, 
					Subnode n0, Subnode n1 );

KNIFE_STATUS triangle_insert_into_center( Triangle, Subnode, Subtri );

KNIFE_STATUS triangle_subtri_index( Triangle, Subtri, int *subtri_index );
KNIFE_STATUS triangle_subnode_index( Triangle, Subnode, int *subnode_index );

KNIFE_STATUS triangle_subtri_with_subnodes( Triangle, 
					    Subnode n0, Subnode n1,
					    Subtri *subtri );
KNIFE_STATUS triangle_subtri_index_with_nodes( Triangle, 
					       Node n0, Node n1,
					       int *subtri_index );

KNIFE_STATUS triangle_cut_with_subnodes( Triangle, 
					 Subnode n0, Subnode n1,
					 Cut *cut );

KNIFE_STATUS triangle_subtri_with_intersections( Triangle, 
						 Intersection i0, 
						 Intersection i1,
						 Subtri *subtri );

KNIFE_STATUS triangle_first_blocking_side( Triangle, 
					   Subnode n0, Subnode n1,
					   Subnode *s0, Subnode *s1 );
KNIFE_STATUS triangle_next_blocking_side( Triangle, 
					  Subnode n0, Subnode n1, 
					  Subnode target,
					  Subnode *s0, Subnode *s1 );

KNIFE_STATUS triangle_eps( Triangle );
KNIFE_STATUS triangle_tecplot( Triangle );
KNIFE_STATUS triangle_export( Triangle );

KNIFE_STATUS triangle_delaunay( Triangle, Subnode );
KNIFE_STATUS triangle_suspect_edge( Triangle, Subnode, Subtri );
KNIFE_STATUS triangle_swap_side( Triangle, Subnode node0, Subnode node1 );
KNIFE_STATUS triangle_recover_side( Triangle, Subnode node0, Subnode node1 );

KNIFE_STATUS triangle_swap_neg_area( Triangle );

double triangle_min_subtri_area( Triangle );

void triangle_examine_subnodes(Triangle triangle);

KNIFE_STATUS triangle_provable_recovery( Triangle, 
					 Subnode node0, Subnode node1 );

END_C_DECLORATION

#endif /* TRIANGLE_H */

