
/* a near tree to speed up geometric searches */



/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
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
