
/* cut between two Triangles of two Intersections */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef CUT_H
#define CUT_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct CutStruct CutStruct;
typedef CutStruct * Cut;
END_C_DECLORATION

#include "triangle.h"
#include "intersection.h"

BEGIN_C_DECLORATION

struct CutStruct {
  Triangle triangle0, triangle1;
  Intersection intersection0, intersection1;
};

Cut cut_between( Triangle, Triangle );
void cut_free( Cut );

#define cut_other_triangle(cut,triangle)				\
  ( ((triangle)==(cut)->triangle1) ? (cut)->triangle0 : (cut)->triangle1 )

#define cut_intersection0(cut) ((cut)->intersection0)
#define cut_intersection1(cut) ((cut)->intersection1)

END_C_DECLORATION

#endif /* CUT_H */
