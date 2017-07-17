
/* a triangle wrapper that extends to include orientation and active subtri */

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

#ifndef MASK_H
#define MASK_H

#include <stdlib.h>
#include <stdio.h>
#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct MaskStruct MaskStruct;
typedef MaskStruct * Mask;
END_C_DECLORATION

#include "triangle.h"

BEGIN_C_DECLORATION
struct MaskStruct {
  Triangle triangle;
  KnifeBool inward_pointing_normal;
  int *region;
};

Mask mask_create( Triangle, KnifeBool inward_pointing_normal );
void mask_free( Mask );

#define mask_triangle( mask )((mask)->triangle)
#define mask_inward_pointing_normal( mask )((mask)->inward_pointing_normal)

KNIFE_STATUS mask_set_frame( int frame );

int mask_nsubtri( Mask );

KNIFE_STATUS mask_activate_all_subtri( Mask );
KNIFE_STATUS mask_deactivate_all_subtri( Mask );

KNIFE_STATUS mask_activate_subtri( Mask, Subtri, int region );
KNIFE_STATUS mask_activate_subtri_index( Mask, int subtri_index, int region );

#define mask_subtri_active(mask,subtri_index)	\
  ( 0 != mask_subtri_region(mask,subtri_index) )

#define mask_subtri_region(mask,subtri_index)	\
  ( NULL == (mask)->region ? 1 : (mask)->region[(subtri_index)] )

KNIFE_STATUS mask_intersection_region( Mask, Intersection, Intersection,
				       int *region );
KNIFE_STATUS mask_set_intersection_region( Mask, Intersection, Intersection,
					   int region );

KNIFE_STATUS mask_paint( Mask );
KNIFE_STATUS mask_verify_paint( Mask );
KNIFE_STATUS mask_collapse_regions( Mask, int region0, int region1 );

KNIFE_STATUS mask_dump_geom( Mask, FILE * );

KNIFE_STATUS mask_find_subtri_with( Mask, FILE * );

KNIFE_STATUS mask_centroid_volume_contribution( Mask, int region, 
						double *origin,
						double *centroid, 
						double *volume );

KNIFE_STATUS mask_tecplot( Mask );

END_C_DECLORATION

#endif /* MASK_H */
