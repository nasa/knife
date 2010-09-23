
/* intersection of a Segment and Triangle */



/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct IntersectionStruct IntersectionStruct;
typedef IntersectionStruct * Intersection;
END_C_DECLORATION

#include "triangle.h"
#include "segment.h"

BEGIN_C_DECLORATION

struct IntersectionStruct {
  Triangle triangle;
  Segment segment;
  double t;
  double uvw[3];
};

KNIFE_STATUS intersection_of( Triangle, Segment, 
			      Intersection *returned_intersection );
void intersection_free( Intersection );

#define intersection_triangle( intersection ) ((intersection)->triangle)
#define intersection_segment( intersection ) ((intersection)->segment)
#define intersection_t( intersection ) ((intersection)->t)

KNIFE_STATUS intersection_core( double *t0, double *t1, double *t2, 
				double *s0, double *s1,
				double *t,
				double *uvw );
double intersection_volume6( double *a, double *b, double *c, double *d);

KNIFE_STATUS intersection_uvw( Intersection, Triangle, double *uvw);
KNIFE_STATUS intersection_xyz( Intersection, double *xyz);

double intersection_x( Intersection );
double intersection_y( Intersection );
double intersection_z( Intersection );

END_C_DECLORATION

#endif /* INTERSECTION_H */
