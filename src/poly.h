
/* volume defined by a watertight collection of masks */

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

#ifndef POLY_H
#define POLY_H

#include <stdlib.h>
#include <stdio.h>
#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct PolyStruct PolyStruct;
typedef PolyStruct * Poly;
END_C_DECLORATION

#include "array.h"
#include "triangle.h"
#include "mask.h"
#include "surface.h"

BEGIN_C_DECLORATION

struct PolyStruct {
  Array mask;
  Array surf;
};

Poly poly_create( void );
KNIFE_STATUS poly_initialize( Poly );
void poly_free( Poly );

KNIFE_STATUS poly_set_frame( int frame );

KNIFE_STATUS poly_add_triangle( Poly, Triangle, 
				KnifeBool inward_pointing_normal );

KnifeBool poly_has_surf_triangle( Poly, Triangle );

KNIFE_STATUS poly_mask_with_triangle( Poly, Triangle, Mask * );

KNIFE_STATUS poly_gather_surf( Poly );

KNIFE_STATUS poly_activate_all_subtri( Poly );
KNIFE_STATUS poly_determine_active_subtri( Poly );

KNIFE_STATUS poly_activate_subtri_at_cuts( Poly );
KNIFE_STATUS poly_paint( Poly );
KNIFE_STATUS poly_paint_surf( Poly, Mask surf, Segment );
KNIFE_STATUS poly_verify_painting( Poly );
KNIFE_STATUS poly_relax_region( Poly poly );
KNIFE_STATUS poly_relax_nodes( Poly poly, Mask mask, Node, Node,
			       KnifeBool *more_relaxation );
KNIFE_STATUS poly_relax_segment( Poly poly, Mask mask, Segment segment, 
				 KnifeBool *more_relaxation );

KNIFE_STATUS poly_collapse_regions( Poly, int region0, int region1 );

KNIFE_STATUS poly_gather_active_surf( Poly, Triangle, int region );

KnifeBool poly_active_mask_with_nodes( Poly, Node n0, Node n1, Node n2, 
				       int *region );

KNIFE_STATUS poly_mask_surrounding_node_activity( Poly, Node,
                                                  KnifeBool *active );

#define poly_add_mask( poly, new_mask )			\
  array_add( (poly)->mask, (ArrayItem)(new_mask) )
#define poly_nmask( poly )			\
  array_size( (poly)->mask )
#define poly_mask( poly, mask_index )			\
  ((Mask)array_item( (poly)->mask, (mask_index) ))

#define poly_add_surf( poly, new_surf )			\
  array_add( (poly)->surf, (ArrayItem)(new_surf) )
#define poly_nsurf( poly )			\
  array_size( (poly)->surf )
#define poly_surf( poly, surf_index )			\
  ((Mask)array_item( (poly)->surf, (surf_index) ))

#define poly_has_surf( poly ) \
  ( 0 < poly_nsurf( poly ) )

KNIFE_STATUS poly_regions( Poly, int *regions );
KNIFE_STATUS poly_centroid_volume( Poly, int region, double *origin, 
				   double *centroid, double *volume );
KNIFE_STATUS poly_average_face_center( Poly, int region, double *centroid );

KNIFE_STATUS poly_nsubtri_between( Poly poly1, int region1, 
				   Poly poly2, int region2, 
				   Node, int *nsubtri );
KNIFE_STATUS poly_subtri_between( Poly poly1, int region1, 
				  Poly poly2, int region2, 
				  Node, int nsubtri, 
				  double *triangle_node0, 
				  double *triangle_node1,
				  double *triangle_node2,
				  double *triangle_normal,
				  double *triangle_area );

KNIFE_STATUS poly_between_sens( Poly poly1, int region1, 
				Poly poly2, int region2,
				Node node, int nsubtri,
				int *parent_int,
				double *parent_xyz, 
				Surface surface );

KNIFE_STATUS poly_surface_nsubtri( Poly, int region, int *nsubtri );
KNIFE_STATUS poly_surface_subtri( Poly, int region, int nsubtri, 
				  double *triangle_node0, 
				  double *triangle_node1,
				  double *triangle_node2,
				  double *triangle_normal,
				  double *triangle_area,
				  int *triangle_tag );

KNIFE_STATUS poly_surface_sens( Poly, int region, int nsubtri, 
				int *constraint_type,
				double *constraint_xyz,
				Surface surface );

KNIFE_STATUS poly_boundary_nsubtri( Poly, int face_index, int region, 
				    int *nsubtri );
KNIFE_STATUS poly_boundary_subtri( Poly, int face_index, int region, 
				   int nsubtri, 
				   double *triangle_node0, 
				   double *triangle_node1,
				   double *triangle_node2,
				   double *triangle_normal,
				   double *triangle_area );

KNIFE_STATUS poly_boundary_sens( Poly, int face_index, int region, 
				 int nsubtri, 
				 int *parent_int,
				 double *parent_xyz, 
				 Surface surface );

KNIFE_STATUS poly_boundary_face_geometry( Poly, int face_index, FILE * );
KNIFE_STATUS poly_surf_geometry( Poly, FILE * );

KNIFE_STATUS poly_tecplot( Poly );
KNIFE_STATUS poly_tecplot_zone( Poly, FILE * );

END_C_DECLORATION

#endif /* POLY_H */
