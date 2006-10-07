
/* collection of segments and triangles defining a surface */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SURFACE_H
#define SURFACE_H

#include "knife_definitions.h"
#include "array.h"
#include "primal.h"
#include "segment.h"
#include "triangle.h"

BEGIN_C_DECLORATION

typedef struct SurfaceStruct SurfaceStruct;
struct SurfaceStruct {
  int nsegment;
  SegmentStruct *segments;
  int ntriangle;
  TriangleStruct *triangles;
};
typedef SurfaceStruct * Surface;

Surface surface_from( Primal, Array of_bcs );

void surface_free( Surface );

END_C_DECLORATION

#endif /* SURFACE_H */