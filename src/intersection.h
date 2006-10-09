
/* intersection of a Segment and Triangle */

/* $Id$ */

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
struct IntersectionStruct {
  Triangle triangle;
  Segment segment;
  double t;
  double uvw[3];
};
typedef IntersectionStruct * Intersection;

Intersection intersection_of( Triangle, Segment );
void intersection_free( Intersection );

KNIFE_STATUS intersection_core( double *t0, double *t1, double *t2, 
				double *s0, double *s1,
				double *t,
				double *uvw );
double knife_volume6( double *a, double *b, double *c, double *d);

END_C_DECLORATION

#endif /* INTERSECTION_H */
