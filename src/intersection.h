
/* intersection of a Segment and Triangle */

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
