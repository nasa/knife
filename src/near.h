
/* a near tree to speed up geometric searches */

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

#ifndef NEAR_H
#define NEAR_H

#include "knife_definitions.h"
#include <math.h>

BEGIN_C_DECLORATION

typedef struct NearStruct NearStruct;
typedef NearStruct * Near;
struct NearStruct {
  int index;
  double x, y, z;
  double radius;
  Near left_child, right_child;
  double left_radius, right_radius;
};

Near near_create( int index, double x, double y, double z, double radius );
KNIFE_STATUS near_initialize( Near, 
			      int index, double x, double y, double z, 
			      double radius );
void near_free( Near );
#define near_index( near ) (NULL==(near)?EMPTY:(near)->index)
#define near_left_index( near ) near_index((near)->left_child)
#define near_right_index( near ) near_index((near)->right_child)
#define near_distance(near,other)		      \
  ( sqrt( (near->x - other->x)*(near->x - other->x) + \
          (near->y - other->y)*(near->y - other->y) +	\
          (near->z - other->z)*(near->z - other->z) ) )
#define near_clearance( near , other) \
  (near_distance(near,other)-(near)->radius-(other)->radius)

Near near_insert( Near , Near child );
#define near_left_radius(near) (near->left_radius)
#define near_right_radius(near) (near->right_radius)

KNIFE_STATUS near_visualize( Near );

int near_collisions( Near, Near target);
KNIFE_STATUS near_touched( Near, Near target, 
			   int *found, int maxfound, int *list);

END_C_DECLORATION

#endif /* NEAR_H */
