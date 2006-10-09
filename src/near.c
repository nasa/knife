
/* a near tree to speed up geometric searches */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <values.h>
#include "near.h"

Near near_create( int index, double x, double y, double z, double radius )
{
  Near near;

  near = malloc( sizeof(NearStruct) );

  near_initialize( near, index, x, y, z, radius );

  return near;
}

KNIFE_STATUS near_initialize( Near near, 
			      int index, double x, double y, double z, 
			      double radius )
{
  near->index = index;
  near->x = x;
  near->y = y;
  near->z = z;
  near->radius = radius;

  near->left_child = NULL;
  near->right_child = NULL;

  near->left_radius = 0;
  near->right_radius = 0;

  return KNIFE_SUCCESS;
}

void near_free( Near near )
{
  free( near );
}

Near near_insert( Near near, Near child )
{
  double child_radius;
  double left_distance, right_distance;

  child_radius = near_distance(near,child) + child->radius;

  if (NULL==near->left_child){ 
    near->left_child = child;
    near->left_radius = child_radius;
    return near;
  }
  if (NULL==near->right_child){ 
    near->right_child = child;
    near->right_radius = child_radius;
    return near;
  }

  left_distance  = near_distance(near->left_child,child);
  right_distance = near_distance(near->right_child,child);

  if ( left_distance < right_distance) {
    if ( near->left_child == near_insert(near->left_child,child) ) {
      near->left_radius = MAX(child_radius,near->left_radius);
      return near;
    }
  }else{
    if ( near->right_child == near_insert(near->right_child,child) ) {
      near->right_radius = MAX(child_radius,near->right_radius);
      return near;
    }
  }
  return NULL;
}

KNIFE_STATUS near_visualize( Near near )
{
  if (NULL == near) return KNIFE_NULL;

  printf("index %d (%f,%f,%f) radius %f\n",
	 near_index(near), near->x, near->y, near->z, near->radius);
  printf("  left index %d (%f) right index %d (%f)\n",
	 near_left_index(near),near_left_radius(near),
	 near_right_index(near),near_right_radius(near));
  near_visualize(near->left_child);
  near_visualize(near->right_child);

  return KNIFE_SUCCESS;
}

int near_collisions(Near near, Near target)
{
  int collisions = 0;

  if (NULL==near || NULL == target) return 0;

  collisions += near_collisions(near->right_child,target);
  collisions += near_collisions(near->left_child,target);

  if (near_clearance(near,target) <= 0) collisions++;
  
  return collisions;
}

KNIFE_STATUS near_touched(Near near, Near target, 
			 int *found, int maxfound, int *list)
{
  double distance, safe_zone;

  if (NULL==near || NULL==target) return KNIFE_NULL;

  distance = near_distance( near, target);
  safe_zone = distance - target->radius;

  if (safe_zone <= near_left_radius(near) ) 
    near_touched(near->left_child, target, found, maxfound, list);
  if (safe_zone <= near_right_radius(near) ) 
    near_touched(near->right_child, target, found, maxfound, list);

  if ( near->radius >= safe_zone ) {
    if (*found >= maxfound) return KNIFE_BIGGER;
    list[*found] = near->index;
    (*found)++;
  }

  return KNIFE_SUCCESS;
}

